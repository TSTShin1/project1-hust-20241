#include <bits/stdc++.h>

using namespace std;

// Customer(Mã KH, Toạ độ, Lượng hàng, TG bắt đầu, TG kết thúc, TG phục vụ)
struct Customer {
    int id;
    double x, y;
    double demand;
    double ready_time;
    double due_time;
    double service_time;
};

// KC giữa 2 điểm
double calculateDistance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

// Giá trị tiết kiệm khi gộp 2 điểm i, j
struct Saving {
    int i, j;
    double saving;
};

// Sort giảm dần theo saving 
bool compareSavings(const Saving &a, const Saving &b) {
    return a.saving > b.saving;
}

// Cải tiến bằng 2-opt
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

// Kiểm tra tính hợp lệ của tuyến đường
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

void solveVRPTW(int vehicle_count, double vehicle_capacity, const vector<Customer> &customers) {
    int n = customers.size() - 1;
    vector<vector<double>> distance(n + 1, vector<double>(n + 1, 0.0));
    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == j) continue;
            distance[i][j] = calculateDistance(customers[i].x, customers[i].y, customers[j].x, customers[j].y);
        }
    }
    vector<Saving> savings;
    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            double saving_value = distance[0][i] + distance[0][j] - 1.0*distance[i][j];
            savings.push_back({i, j, saving_value});
        }
    }
    sort(savings.begin(), savings.end(), compareSavings);
    vector<vector<int>> routes(n);
    vector<double> route_load(n, 0.0);
    for (int i = 0; i < n; ++i) {
        routes[i] = {i + 1};
        route_load[i] = customers[i + 1].demand;
    }
    for (const auto &s : savings) {
        int r1 = -1, r2 = -1, r3 = -1, r4 = -1;
        for (int k = 0; k < routes.size(); ++k) {
            if (find(routes[k].begin(), routes[k].end(), s.i) != routes[k].end()) r1 = k, r3 = k;
            if (find(routes[k].begin(), routes[k].end(), s.j) != routes[k].end()) r2 = k, r4 = k;
        }
        if (r1 != r2 && route_load[r1] + route_load[r2] <= vehicle_capacity) {
            vector<int> merged_route = routes[r1];
            vector<int> merged_route2 = routes[r2];
            merged_route.insert(merged_route.end(), routes[r2].begin(), routes[r2].end());
            if (isRouteValid(merged_route, distance, customers)) {
                routes[r1] = merged_route;
                route_load[r1] += route_load[r2];
                routes.erase(routes.begin() + r2);
                route_load.erase(route_load.begin() + r2);
            }
        }
    }
    // Cải thiện các tuyến bằng thuật toán 2-opt
    for (auto &route : routes) {
        applyTwoOpt(route, distance, customers);
    }
    // Tính tổng chi phí và in tuyến đường
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
        freopen("C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\outputSH.txt", "w", stdout);
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
    solveVRPTW(vehicle_count, vehicle_capacity, customers);
    return 0;
}