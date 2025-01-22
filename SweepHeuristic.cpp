#include <bits/stdc++.h>

using namespace std;

const double PI = 3.14;

struct Customer {
    int id;
    double x, y;
    double demand;
    double ready_time;
    double due_time; 
    double service_time;
};

typedef pair<int, double> Angle;
typedef vector<vector<int>> Clusters;
typedef map<int, pair<double, int>> CostMap;

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

double calculateDistance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
}

double calculatePolarAngle(double depot_x, double depot_y, double x, double y) {
    return atan2(y - depot_y, x - depot_x);
}

vector<vector<double>> calculate_distance(const vector<Customer>& customers) {
    int n = customers.size() - 1;
    vector<vector<double>> distance(n + 1, vector<double>(n + 1, 0.0));
    for (int i = 0; i <= n; ++i) {
        for (int j = 0; j <= n; ++j) {
            if (i == j) continue;
            distance[i][j] = calculateDistance(customers[i].x, customers[i].y, customers[j].x, customers[j].y);
        }
    }
    return distance;
}

Clusters clustering(const vector<Angle>& angles, double vehicle_capacity, const vector<Customer>& customers, vector<int>& remain) {
    Clusters clusters;
    vector<int> current_cluster;
    int current_load = 0;
    for (const auto& angle: angles) {
        int id = angle.first;
        if (find(remain.begin(), remain.end(), id) == remain.end()) {
            continue;
        }
        if (current_load + customers[id].demand > vehicle_capacity) {
            clusters.push_back(current_cluster);
            current_cluster.clear();
            current_load = 0;
        }
        current_cluster.push_back(id);
        current_load += customers[id].demand;
    }
    if (!current_cluster.empty()) {
        clusters.push_back(current_cluster);
    }
    return clusters;
}

vector<Angle> calculateAngle(const vector<Customer>& customers) {
    vector<Angle> angles;
    for (const auto& customer: customers) {
        int cust_id = customer.id;
        double dx = customer.x - customers[0].x;
        double dy = customer.y - customers[0].y;
        if (dx == 0 && dy == 0) {
            continue;
        }
        else {
            double angle = atan2(dy, dx);
            if (angle < 0) angle += 2 * PI;
            angles.push_back({cust_id, angle});
        }
    }
    sort(angles.begin(), angles.end(), [](const Angle& a, const Angle& b) {
        return a.second < b.second;
    });
    return angles;
}

double calculate_new_time(const vector<pair<double, double>>& curr_serve_time, int add_customer,
                         int insertion, int prev_customer, int next_customer, 
                         const vector<Customer>& customers, const vector<vector<double>>& distance) {
    double prev_to_curr = distance[prev_customer][add_customer];
    double curr_to_next = distance[add_customer][next_customer];
    double add_time = max(customers[add_customer].ready_time, prev_to_curr + 
                        curr_serve_time[insertion - 1].first) + customers[add_customer].service_time;
    if (add_time - customers[add_customer].service_time > customers[add_customer].due_time) {
        return -1;
    }
    return max(add_time + curr_to_next, customers[next_customer].ready_time) + customers[next_customer].service_time;
}

pair<vector<int>, vector<pair<double, double>>> update_current_time (
    vector<int>& route, vector<pair<double, double>>& curr_route_time, int insertion,
    int insert_customer, const vector<Customer>& customers, const vector<vector<double>>& distance) {
        route.insert(route.begin() + insertion, insert_customer);
        curr_route_time.insert(curr_route_time.begin() + insertion, {0.0, 0.0});
        curr_route_time[insertion].first = max(customers[insert_customer].ready_time, 
        distance[route[insertion - 1]][insert_customer] + curr_route_time[insertion - 1].first) + customers[insert_customer].service_time;
        curr_route_time[insertion].second = max(0.0, customers[insert_customer].ready_time - distance[route[insertion - 1]][insert_customer] - curr_route_time[insertion - 1].first);
        for (size_t i = insertion + 1; i < route.size(); ++i) {
            curr_route_time[i].first = max(
                curr_route_time[i - 1].first + distance[route[i - 1]][route[i]],
                customers[route[i]].ready_time
            ) + customers[route[i]].service_time;
            curr_route_time[i].second = max(
                0.0,
                customers[route[i]].ready_time - curr_route_time[i - 1].first - distance[route[i - 1]][route[i]]
            );
        }
        return {route, curr_route_time};
}

bool check_time_window(vector<int>& route, vector<pair<double, double>>& curr_route_time1,
                       int insertion, int insert_customer, double PF,
                       const vector<Customer>& customers, const vector<vector<double>>& distance) {
    vector<pair<double, double>> curr_route_time = curr_route_time1;
    route.insert(route.begin() + insertion, insert_customer);
    // Tính thời gian tại vị trí chèn
    curr_route_time.insert(curr_route_time.begin() + insertion, {0.0, 0.0});
    curr_route_time[insertion].first = max(customers[insert_customer].ready_time,
        distance[route[insertion - 1]][insert_customer] + curr_route_time[insertion - 1].first) 
        + customers[insert_customer].service_time;
    curr_route_time[insertion].second = max(0.0, 
        customers[insert_customer].ready_time 
        - distance[route[insertion - 1]][insert_customer] 
        - curr_route_time[insertion - 1].first);

    // Duyệt qua các khách hàng còn lại trong tuyến
    double PF_i = PF;
    for (size_t i = insertion + 2; i < route.size(); ++i) {
        // Thời gian phục vụ của khách hàng hiện tại
        if (PF_i == 0) return true;
        if (PF_i + curr_route_time[i].first - customers[route[i]].service_time > customers[route[i]].due_time) {
            return false;
        }
        PF_i = max(0.0, PF - curr_route_time[i].second);
    }
    return true;
}


bool check_capacity(const vector<int>& route, int add_customer, const vector<Customer>& customers, double vehicle_capacity) {
    int current_demand = accumulate(route.begin(), route.end(), 0, [&](int sum, int customer) {
        return sum + customers[customer].demand;
    });
    return current_demand + customers[add_customer].demand <= vehicle_capacity;
}

vector<CostMap> find_insertion(
    vector<int>& route, const vector<int>& unvisited, const vector<Customer>& customers,
    const vector<vector<double>>& distance, const vector<pair<double, double>>& curr_route_time,
    double alpha1, double alpha2, double lambda, double vehicle_capacity, double m
) {
    vector<CostMap> list_c1_cost;
    for (int u: unvisited) {
        int best_insertion = -1;
        double min_c1 = numeric_limits<double>::infinity();
        for (size_t ind = 0; ind < route.size() - 1; ind++) {
            vector<pair<double, double>> temp_curr_route_time = curr_route_time;
            int i = route[ind];
            int j = route[ind + 1];
            double c11 = distance[i][u] + distance[u][j] - m * distance[i][j];
            double new_time = calculate_new_time(temp_curr_route_time, u, ind + 1, i, j, customers, distance);
            if (new_time == -1 || new_time - customers[j].service_time > customers[j].due_time) continue;
            double c12 = new_time - temp_curr_route_time[ind + 1].first;
            double c1 = alpha1 * c11 + alpha2 * c12;
            vector<int> temp_route = route;
            if (!check_time_window(temp_route, temp_curr_route_time, ind + 1, u, c12, customers, distance)) continue;
            if (!check_capacity(temp_route, u, customers, vehicle_capacity)) continue;
            if (c1 < min_c1) {
                min_c1 = c1;
                best_insertion = ind + 1;
            }
        }
        if (best_insertion != -1) {
            list_c1_cost.push_back({{u, {min_c1, best_insertion}}});
        }
    }
    if (list_c1_cost.empty()) {
        return {};
    }
    sort(list_c1_cost.begin(), list_c1_cost.end(), [](const CostMap& a, const CostMap& b) {
        return a.begin()->second.first < b.begin()->second.first;
    });
    return list_c1_cost;
}

pair<vector<int>, vector<int>> i1_heuristic(
    double alpha1, double alpha2, double lambda, double vehicle_capacity, double m,
    vector<int> unvisited, const vector<Customer>& customers, const vector<vector<double>>& distance
) {
    vector<int> route = {0, 0};
    vector<pair<double, double>> curr_route_time = {{0.0, 0.0}, {0.0, 0.0}};
    while (true) {
        vector<vector<double>> c2_list;
        vector<CostMap> position = find_insertion(route, unvisited, customers, distance, curr_route_time, alpha1, alpha2, lambda, vehicle_capacity, m);
        if (position.empty()) {
            break;
        }
        for (const auto& item: position) {
            for (const auto& [u, cost]: item) {
                double min_c1 = cost.first;
                int insertion = cost.second;
                c2_list.push_back({(double)u, lambda * distance[0][u] - min_c1, (double)insertion});
            }
        }
        sort(c2_list.begin(), c2_list.end(), [](const vector<double>& a, const vector<double>& b) {
            return a[1] > b[1];
        });
        int best_customer = (int)c2_list[0][0];
        int best_insertion = (int)c2_list[0][2];
        tie(route, curr_route_time) = update_current_time(route, curr_route_time, best_insertion, best_customer, customers, distance);
        auto it = find(unvisited.begin(), unvisited.end(), best_customer);
        if (it != unvisited.end()) {
            unvisited.erase(it);
        }
    }
    return {route, unvisited};
}

double calculate_cost(const vector<vector<int>>& routes, const vector<vector<double>>& distance) {
    if (!routes.empty()) {
        double cost = 0.0;
        for (const auto& route: routes) {
            for (size_t i = 0; i < route.size() - 1; ++i) {
                cost += distance[route[i]][route[i + 1]];
            }
        }
        return cost;
    }
    return numeric_limits<double>::infinity();
}

vector<vector<int>> time_oriented_sweep(
    double alpha1, double alpha2, double lambda, double vehicle_capacity, double m,
    vector<int> curr_customer, const vector<Customer>& customers, const vector<vector<double>>& distance,
    const vector<Angle>& angles
) {
    vector<vector<int>> clusters = clustering(angles, vehicle_capacity, customers, curr_customer);
    vector<int> remain;
    vector<vector<int>> routes;
    for (const auto& cluster: clusters) {
        vector<int> route, remain_node;
        tie(route, remain_node) = i1_heuristic(alpha1, alpha2, lambda, vehicle_capacity, m, cluster, customers, distance);
        remain.insert(remain.end(), remain_node.begin(), remain_node.end());
        routes.push_back(route);
    }
    if (!remain.empty()) {
        vector<vector<int>> remain_routes = time_oriented_sweep(alpha1, alpha2, lambda, vehicle_capacity, m, remain, customers, distance, angles);
        routes.insert(routes.end(), remain_routes.begin(), remain_routes.end());
    }
    return routes;
}

int main() {
    if (fopen("C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\input.txt", "r")) {
        freopen("C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\input.txt", "r", stdin);
        freopen("C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\outputSWH.txt", "w", stdout);
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
    vector<vector<double>> distance = calculate_distance(customers);
    vector<Angle> angles = calculateAngle(customers);
    vector<int> remain_customers;
    for (int i = 1; i <= customer_count; ++i) {
        remain_customers.push_back(i);
    }
    double m = 1;
    double lambda = 1;
    double alpha1 = 1;
    double alpha2 = 0;
    vector<vector<int>> routes = time_oriented_sweep(alpha1, alpha2, lambda, vehicle_capacity, m, remain_customers, customers, distance, angles);
    for (auto& route: routes) {
        applyTwoOpt(route, distance, customers);
    }
    for (const auto& route: routes) {
        cout << "Route: ";
        for (int customer: route) {
            cout << customer << " ";
        }
        cout << "\n";
    }
    cout << "Cost: " << calculate_cost(routes, distance) << endl;
}
