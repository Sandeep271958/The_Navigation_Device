import Foundation
import CoreLocation
import MapKit

// ====================================================================
// MapKit Navigation Engine
// Calculates routes and tracks live maneuvers with zero API cost
// ====================================================================

class NavigationEngine: NSObject, ObservableObject, CLLocationManagerDelegate {
    static let shared = NavigationEngine()

    @Published var isNavigating = false
    @Published var currentRoute: MKRoute?
    @Published var currentStepIndex = 0
    @Published var upcomingInstruction = "Search destination to start"
    @Published var distanceToTurn = 0
    @Published var currentSpeedKmh = 0
    @Published var etaMinutes = 0

    private var locationManager: CLLocationManager!
    private var lastLocation: CLLocation?

    override init() {
        super.init()
        locationManager = CLLocationManager()
        locationManager.delegate = self
        locationManager.desiredAccuracy = kCLLocationAccuracyBestForNavigation
        locationManager.allowsBackgroundLocationUpdates = true
        locationManager.pausesLocationUpdatesAutomatically = false
        locationManager.requestAlwaysAuthorization()
    }

    func startRoute(to destinationCoordinate: CLLocationCoordinate2D, destinationName: String) {
        guard let userLoc = locationManager.location?.coordinate else {
            print("[Nav] Current location unknown")
            return
        }

        let request = MKDirections.Request()
        request.source = MKMapItem(placemark: MKPlacemark(coordinate: userLoc))
        request.destination = MKMapItem(placemark: MKPlacemark(coordinate: destinationCoordinate))
        request.transportType = .automobile

        let directions = MKDirections(request: request)
        directions.calculate { [weak self] response, error in
            guard let self = self, let route = response?.routes.first else {
                print("[Nav] Route calculation failed: \(error?.localizedDescription ?? "unknown")")
                return
            }

            DispatchQueue.main.async {
                self.currentRoute = route
                self.currentStepIndex = 0
                self.isNavigating = true
                self.etaMinutes = Int(route.expectedTravelTime / 60.0)
                self.locationManager.startUpdatingLocation()
                print("[Nav] Route started with \(route.steps.count) steps. ETA: \(self.etaMinutes) min")
            }
        }
    }

    func stopNavigation() {
        isNavigating = false
        currentRoute = nil
        locationManager.stopUpdatingLocation()
        NavigateMeBLEManager.shared.sendNavigationUpdate(
            maneuver: 13, // Destination / Arrived
            distance: 0,
            currentRoad: "Navigation Stopped",
            nextRoad: "",
            nextManeuver: 0,
            nextDistance: 0,
            speed: 0,
            eta: 0
        )
    }

    func locationManager(_ manager: CLLocationManager, didUpdateLocations locations: [CLLocation]) {
        guard let location = locations.last, isNavigating, let route = currentRoute else { return }

        // Update speed
        let speedMps = max(0, location.speed)
        currentSpeedKmh = Int(speedMps * 3.6)

        // Find current step
        if currentStepIndex < route.steps.count {
            let step = route.steps[currentStepIndex]
            let stepLocation = CLLocation(latitude: step.polyline.coordinate.latitude, longitude: step.polyline.coordinate.longitude)
            let distance = Int(location.distance(from: stepLocation))

            distanceToTurn = distance
            upcomingInstruction = step.instructions

            // Check if step completed (within 20m of waypoint)
            if distance < 20 && currentStepIndex < route.steps.count - 1 {
                currentStepIndex += 1
            }

            // Extract Maneuvers
            let primaryManeuver = mapInstructionsToManeuver(step.instructions)
            
            // Next-Next Step preview
            var nextManeuver = 0
            var nextDist = 0
            if currentStepIndex + 1 < route.steps.count {
                let nextStep = route.steps[currentStepIndex + 1]
                nextManeuver = mapInstructionsToManeuver(nextStep.instructions)
                nextDist = Int(nextStep.distance)
            }

            // Transmit over BLE to NavigateMe pod
            NavigateMeBLEManager.shared.sendNavigationUpdate(
                maneuver: primaryManeuver,
                distance: max(0, distanceToTurn),
                currentRoad: step.notice ?? "Navigating",
                nextRoad: step.instructions,
                nextManeuver: nextManeuver,
                nextDistance: nextDist,
                speed: currentSpeedKmh,
                eta: etaMinutes
            )
        }
    }

    /// Maps human-readable instructions to NavigateMe vector icon codes
    private func mapInstructionsToManeuver(_ text: String) -> Int {
        let lower = text.lowercased()
        if lower.contains("u-turn") { return 8 }
        if lower.contains("flyover") || lower.contains("keep right") { return 11 }
        if lower.contains("keep left") { return 10 }
        if lower.contains("sharp left") { return 4 }
        if lower.contains("sharp right") { return 5 }
        if lower.contains("slight left") || lower.contains("bear left") { return 6 }
        if lower.contains("slight right") || lower.contains("bear right") { return 7 }
        if lower.contains("left") { return 2 }
        if lower.contains("right") { return 3 }
        if lower.contains("roundabout") { return 9 }
        if lower.contains("arrived") || lower.contains("destination") { return 13 }
        return 1 // Straight
    }
}

