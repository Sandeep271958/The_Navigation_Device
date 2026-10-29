import SwiftUI
import MapKit

// ====================================================================
// NavigateMe Companion Main Interface (SwiftUI)
// ====================================================================

struct ContentView: View {
    @ObservedObject var bleManager = NavigateMeBLEManager.shared
    @ObservedObject var navEngine = NavigationEngine.shared

    @State private var searchQuery = ""
    @State private var searchResults: [MKMapItem] = []
    @State private var selectedDestinationName = ""

    var body: some View {
        NavigationView {
            VStack(spacing: 0) {
                // Bluetooth Status Header
                HStack {
                    Circle()
                        .fill(bleManager.isConnected ? Color.green : Color.red)
                        .frame(width: 10, height: 10)
                    Text(bleManager.statusMessage)
                        .font(.footnote)
                        .foregroundColor(.secondary)
                    Spacer()
                    if bleManager.isConnected {
                        Text("Connected")
                            .font(.caption)
                            .padding(.horizontal, 8)
                            .padding(.vertical, 3)
                            .background(Color.green.opacity(0.15))
                            .foregroundColor(.green)
                            .cornerRadius(6)
                    }
                }
                .padding(.horizontal)
                .padding(.vertical, 8)
                .background(Color(UIColor.secondarySystemBackground))

                // Destination Search Bar
                HStack {
                    Image(systemName: "magnifyingglass")
                        .foregroundColor(.gray)
                    TextField("Search destination in Hyderabad...", text: $searchQuery, onCommit: performSearch)
                        .textFieldStyle(PlainTextFieldStyle())
                    if !searchQuery.isEmpty {
                        Button(action: { searchQuery = "" }) {
                            Image(systemName: "xmark.circle.fill").foregroundColor(.gray)
                        }
                    }
                }
                .padding(10)
                .background(Color(UIColor.systemBackground))
                .cornerRadius(10)
                .padding(.horizontal)
                .padding(.vertical, 6)

                // Search Results Dropdown
                if !searchResults.isEmpty && !navEngine.isNavigating {
                    List(searchResults, id: \.self) { item in
                        Button(action: { selectDestination(item) }) {
                            VStack(alignment: .leading) {
                                Text(item.name ?? "Unknown Location")
                                    .font(.headline)
                                Text(item.placemark.title ?? "")
                                    .font(.caption)
                                    .foregroundColor(.secondary)
                            }
                        }
                    }
                    .frame(maxHeight: 200)
                }

                // Active Route Dashboard
                if navEngine.isNavigating {
                    VStack(spacing: 12) {
                        HStack {
                            VStack(alignment: .leading) {
                                Text("UPCOMING MANEUVER")
                                    .font(.caption2)
                                    .foregroundColor(.secondary)
                                Text(navEngine.upcomingInstruction)
                                    .font(.headline)
                                    .lineLimit(2)
                            }
                            Spacer()
                            VStack(alignment: .trailing) {
                                Text("\(navEngine.distanceToTurn) m")
                                    .font(.system(size: 32, weight: .bold, design: .rounded))
                                    .foregroundColor(.blue)
                            }
                        }
                        .padding()
                        .background(Color(UIColor.secondarySystemBackground))
                        .cornerRadius(12)

                        HStack(spacing: 20) {
                            TelemetryBox(title: "SPEED", value: "\(navEngine.currentSpeedKmh) km/h")
                            TelemetryBox(title: "ESTIMATED TIME", value: "\(navEngine.etaMinutes) min")
                        }

                        Button(action: { navEngine.stopNavigation() }) {
                            Text("Stop Navigation")
                                .font(.headline)
                                .foregroundColor(.white)
                                .frame(maxWidth: .infinity)
                                .padding()
                                .background(Color.red)
                                .cornerRadius(12)
                        }
                    }
                    .padding()
                } else {
                    // Quick Destination Suggestions in Hyderabad
                    VStack(alignment: .leading, spacing: 10) {
                        Text("Popular Hyderabad Routes")
                            .font(.subheadline)
                            .foregroundColor(.secondary)
                            .padding(.horizontal)
                            .padding(.top, 10)

                        HStack {
                            QuickRouteButton(title: "HITEC City", coord: CLLocationCoordinate2D(latitude: 17.4474, longitude: 78.3762))
                            QuickRouteButton(title: "Gachibowli", coord: CLLocationCoordinate2D(latitude: 17.4401, longitude: 78.3489))
                        }
                        .padding(.horizontal)

                        HStack {
                            QuickRouteButton(title: "Jubilee Hills", coord: CLLocationCoordinate2D(latitude: 17.4319, longitude: 78.4073))
                            QuickRouteButton(title: "Secunderabad", coord: CLLocationCoordinate2D(latitude: 17.4399, longitude: 78.4983))
                        }
                        .padding(.horizontal)

                        Spacer()
                    }
                }
            }
            .navigationTitle("NavigateMe 🛵")
        }
    }

    private func performSearch() {
        let request = MKLocalSearch.Request()
        request.naturalLanguageQuery = searchQuery
        request.region = MKCoordinateRegion(
            center: CLLocationCoordinate2D(latitude: 17.3850, longitude: 78.4867), // Hyderabad center
            span: MKCoordinateSpan(latitudeDelta: 0.3, longitudeDelta: 0.3)
        )
        let search = MKLocalSearch(request: request)
        search.start { response, _ in
            if let items = response?.mapItems {
                self.searchResults = items
            }
        }
    }

    private func selectDestination(_ item: MKMapItem) {
        selectedDestinationName = item.name ?? "Destination"
        searchResults.removeAll()
        searchQuery = selectedDestinationName
        navEngine.startRoute(to: item.placemark.coordinate, destinationName: selectedDestinationName)
    }
}

struct TelemetryBox: View {
    let title: String
    let value: String

    var body: some View {
        VStack(alignment: .leading) {
            Text(title)
                .font(.caption2)
                .foregroundColor(.secondary)
            Text(value)
                .font(.headline)
        }
        .frame(maxWidth: .infinity, alignment: .leading)
        .padding()
        .background(Color(UIColor.secondarySystemBackground))
        .cornerRadius(10)
    }
}

struct QuickRouteButton: View {
    let title: String
    let coord: CLLocationCoordinate2D

    var body: some View {
        Button(action: {
            NavigationEngine.shared.startRoute(to: coord, destinationName: title)
        }) {
            HStack {
                Image(systemName: "location.fill")
                    .foregroundColor(.blue)
                Text(title)
                    .font(.subheadline)
                    .foregroundColor(.primary)
            }
            .frame(maxWidth: .infinity)
            .padding(12)
            .background(Color(UIColor.secondarySystemBackground))
            .cornerRadius(10)
        }
    }
}

