#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std;

struct Customer {
    int id;
    double x, y;
    double demand;
    double ready_time;
    double due_time;
    double service_time;
};

void parseData(const string &filename, vector<Customer> &customers, int &vehicle_count, double &vehicle_capacity) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        exit(1);
    }

    string line;
    // Read vehicle count and capacity
    while (getline(file, line)) {
        if (line.find("VEHICLE") != string::npos) {
            getline(file, line); // Skip "NUMBER CAPACITY"
            file >> vehicle_count >> vehicle_capacity;
        } else if (line.find("CUSTOMER") != string::npos) {
            getline(file, line); // Skip header lines
            getline(file, line);
            break;
        }
    }

    // Read customer data
    while (getline(file, line)) {
        istringstream iss(line);
        Customer c;
        iss >> c.id >> c.x >> c.y >> c.demand >> c.ready_time >> c.due_time >> c.service_time;
        customers.push_back(c);
    }
    file.close();
}

void writeFormattedInput(const vector<Customer> &customers, int vehicle_count, double vehicle_capacity, const string &output_filename) {
    ofstream output(output_filename);
    if (!output.is_open()) {
        cerr << "Error: Unable to open output file " << output_filename << endl;
        exit(1);
    }

    output << vehicle_count << endl;
    output << static_cast<int>(vehicle_capacity) << endl;
    output << customers.size() - 1 << endl;

    for (const auto &customer : customers) {
        output << customer.x << " " << customer.y << " "
               << customer.demand << " " << customer.ready_time << " "
               << customer.due_time << " " << customer.service_time << endl;
    }
    output.close();
}

int main() {
    string input_filename = "C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\Vrp-Set-Solomon\\RC208.txt";  // Change to your file path
    string output_filename = "C:\\Users\\Admin\\Desktop\\c++\\.vscode\\Project1\\Data\\RC208.txt";
    vector<Customer> customers;
    int vehicle_count;
    double vehicle_capacity;

    parseData(input_filename, customers, vehicle_count, vehicle_capacity);
    writeFormattedInput(customers, vehicle_count, vehicle_capacity, output_filename);

    cout << "Formatted input has been written to " << output_filename << endl;
    return 0;
}
