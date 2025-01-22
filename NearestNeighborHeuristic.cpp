#include <bits/stdc++.h>

using namespace std;

struct Customer {
    int id;
    double x, y;
    double demand;
    double ready_time;
    double due_time;
    double service_time;
};

double calculateDistance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

bool isRouteValid(const vector<int> &route, const vector<vector<double>> &distance, const vector<Customer> &customers) {
    double current_time = 0.0;
    int prev = 0;
    for (int customer : route) {
        current_time += distance[prev][customer];
        current_time = max(current_time, customers[customer].ready_time);
        if (current_time > customers[customer].due_time) {
            return false;
        }
        current_time += customers[customer].service_time;
        prev = customer;
    }
    current_time += distance[prev][0];
    return current_time <= customers[0].due_time;
}

void applyTwoOpt(vector<int> &route, const vector<vector<double>> &distance, const vector<Customer> &customers) {
    bool improved = true;
    while (improved) {
        improved = false;
        for (size_t i = 1; i < route.size() - 1; ++i) {
            for (size_t j = i + 1; j < route.size(); ++j) {
                double current_cost = distance[route[i - 1]][route[i]] + distance[route[j]][route[(j + 1) % route.size()]];
                double new_cost = distance[route[i - 1]][route[j]] + distance[route[i]][route[(j + 1) % route.size()]];
                if (new_cost < current_cost) {
                    vector<int> temp_route = route;
                    reverse(temp_route.begin() + i, temp_route.begin() + j + 1);
                    double current_time = 0.0;
                    int prev = 0;
                    bool valid = true;
                    for (size_t k = 0; k < temp_route.size(); ++k) {
                        int customer_id = temp_route[k];
                        current_time += distance[prev][customer_id];
                        current_time = max(current_time, customers[customer_id].ready_time);
                        if (current_time > customers[customer_id].due_time) {
                            valid = false;
                            break;
                        }
                        current_time += customers[customer_id].service_time;
                        prev = customer_id;
                    }
                    if (valid) {
                        reverse(route.begin() + i, route.begin() + j + 1);
                        improved = true;
                    }
                }
            }
        }
    }
}

void solveVRPTW(int vehicle_count, double vehicle_capacity, const vector<Customer> &customers, double alpha1, double alpha2, double alpha3) {
    int n = customers.size() - 1;
    vector<bool> visited(n + 1, false);
    vector<vector<double>> distance(n + 1, vector<double>(n + 1, 0.0));
    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == j) continue;
            distance[i][j] = calculateDistance(customers[i].x, customers[i].y, customers[j].x, customers[j].y);
        }
    }
    visited[0] = true;
    vector<vector<int>> routes;
    for (int vehicle = 0; vehicle < vehicle_count; ++vehicle) {
        vector<int> route;
        double current_load = 0.0;
        double current_time = 0.0;
        int current_customer = 0;
        while (true) {
            int next_customer = -1;
            double min_cost = numeric_limits<double>::max();
            for (int i = 1; i <= n; ++i) {
                if (!visited[i] && current_load + customers[i].demand <= vehicle_capacity) {
                    double distance = calculateDistance(customers[current_customer].x, customers[current_customer].y, customers[i].x, customers[i].y);
                    double arrival_time = max(current_time + distance, customers[i].ready_time);
                    double waiting_time = max(0.0, customers[i].ready_time - (current_time + distance));
                    double urgency = customers[i].due_time - arrival_time;
                    if (arrival_time <= customers[i].due_time) {
                        double cost = alpha1 * distance + alpha2 * waiting_time + alpha3 * urgency;
                        if (cost < min_cost) {
                            min_cost = cost;
                            next_customer = i;
                        }
                    }
                }
            }
            if (next_customer == -1) break;
            
            // Thêm khách hàng vào tuyến đường
            route.push_back(next_customer);
            visited[next_customer] = true;
            current_load += customers[next_customer].demand;

            // Kiểm tra tính hợp lệ của tuyến đường
            if (!isRouteValid(route, distance, customers)) {
                // Nếu tuyến đường không hợp lệ, loại bỏ khách hàng vừa thêm và thoát khỏi vòng lặp
                route.pop_back();
                visited[next_customer] = false;
                break;
            }
            current_time = max(current_time + calculateDistance(customers[current_customer].x, customers[current_customer].y, customers[next_customer].x, customers[next_customer].y), customers[next_customer].ready_time) + customers[next_customer].service_time;
            current_customer = next_customer;
        }
        if (!route.empty()) {
            routes.push_back(route);
        }
    }
    for (auto &route : routes) {
        applyTwoOpt(route, distance, customers);
    }
    double total_cost = 0.0;
    for (size_t idx = 0; idx < routes.size(); ++idx) {
        const auto &route = routes[idx];
        double route_cost = 0.0;
        int prev = 0;
        cout << "Route #" << idx + 1 << ": ";
        for (int customer : route) {
            route_cost += distance[prev][customer];
            cout << customer << " ";
            prev = customer;
        }
        route_cost += distance[prev][0];
        cout << "\n";
        total_cost += route_cost;
    }
    cout << "Cost " << total_cost << endl;
}

int main() {
    if (fopen("C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\input.txt", "r")) {
        freopen("C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\input.txt", "r", stdin);
        freopen("C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\outputNNH.txt", "w", stdout);
    }
    int vehicle_count;
    double vehicle_capacity;
    cin >> vehicle_count >> vehicle_capacity;
    int customer_count;
    cin >> customer_count;
    vector<Customer> customers(customer_count + 1);
    customers[0].id = 0;
    cin >> customers[0].x >> customers[0].y >> customers[0].demand >> customers[0].ready_time >> customers[0].due_time >> customers[0].service_time;
    for (int i = 1; i <= customer_count; ++i) {
        customers[i].id = i;
        cin >> customers[i].x >> customers[i].y >> customers[i].demand >> customers[i].ready_time >> customers[i].due_time >> customers[i].service_time;
    }
    double alpha1 = 0.4;
    double alpha2 = 0.4;
    double alpha3 = 1 - alpha1 - alpha2;
    solveVRPTW(vehicle_count, vehicle_capacity, customers, alpha1, alpha2, alpha3);
    return 0;
}
