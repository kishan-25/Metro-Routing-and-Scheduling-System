#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <fstream>
#include <sstream>
using namespace std;

const int NUM_STATIONS = 86; // 0 to 85
const int NUM_LINES = 6;     
const int START_TIME = 6;    // 6 AM
const int END_TIME = 20;     // 8 PM
const int FREQUENCY = 30;    // minutes between trains on same line

struct Station {
    int id;
    string name;
};

struct MetroLine {
    int id;
    string name;
    vector<Station> stations;
    vector<vector<pair<int, int>>> daily_schedule; // station_id, time for each trip

    MetroLine(int _id, const string& _name) : id(_id), name(_name) {}
};

class MetroScheduler {
private:
    vector<MetroLine> lines;
    map<int,string> station_names;

    int calculate_travel_time(int from_station, int to_station) {
        // Simplified travel time calculation
        return abs(from_station - to_station) * 2 + 1; // 2 minutes per station + 1 minute stop time
    }

public:
    void add_station_name(int id, const string& name) {
        station_names[id] = name;
    }

    void add_line(int id, const string& name, const vector<int>& station_ids) {
        if (id < 1 || id > NUM_LINES) {
            throw out_of_range("Line ID out of valid range");
        }
        MetroLine line(id, name);
        for (int station_id : station_ids) {
            if (station_names.find(station_id) == station_names.end()) {
                throw runtime_error("Station name not found for ID: " + to_string(station_id));
            }
            line.stations.push_back({station_id, station_names[station_id]});
        }
        lines.push_back(line);
    }

    bool generate_schedule() {
        for (auto& line : lines) {
            int start_time = START_TIME * 60;
            int end_time = END_TIME * 60;

            while (start_time < end_time) {
                vector<pair<int, int>> trip_schedule;
                int current_time = start_time;

                for (size_t i = 0; i < line.stations.size(); ++i) {
                    trip_schedule.emplace_back(line.stations[i].id, current_time);
                    
                    if (i < line.stations.size() - 1) {
                        int travel_time = calculate_travel_time(line.stations[i].id, line.stations[i + 1].id);
                        current_time += travel_time;
                    }
                }

                line.daily_schedule.push_back(trip_schedule);
                start_time += FREQUENCY;
            }
        }
        return true;
    }

    void print_schedule() const {
        for (const auto& line : lines) {
            cout << "Line " << line.id << " (" << line.name << ") schedule:\n";
            cout << setw(30) << "Station" << " | ";
            
            // Print header with times
            for (int hour = START_TIME; hour < END_TIME; ++hour) {
                for (int minute = 0; minute < 60; minute += FREQUENCY) {
                    cout << setw(5) << setfill('0') << right << hour << ":" << setw(2) << minute << " | ";
                }
            }
            cout << "\n" << string(250, '-') << "\n";
            
            // Print schedule for each station
            for (const auto& station : line.stations) {
                cout << setw(30) << setfill(' ') << left << station.name << " | ";
                
                for (const auto& trip : line.daily_schedule) {
                    for (const auto& [trip_station, time] : trip) {
                        if (trip_station == station.id) {
                            cout << setw(5) << setfill('0') << right << time / 60 << ":" << setw(2) << time % 60 << " | ";
                            break;
                        }
                    }
                }
                cout << "\n";
            }
            cout << "\n\n";
        }
    }

    void export_to_csv(const string& filename) const {
        ofstream file(filename);
        if (!file.is_open()) {
            throw runtime_error("Unable to open file: " + filename);
        }

        // Write header
        file << "Line,Station,";
        for (int hour = START_TIME; hour < END_TIME; ++hour) {
            for (int minute = 0; minute < 60; minute += FREQUENCY) {
                file << setfill('0') << setw(2) << hour << ":" << setw(2) << minute << ",";
            }
        }
        file << "\n";

        // Write schedule for each line
        for (const auto& line : lines) {
            for (const auto& station : line.stations) {
                file << line.name << "," << station.name << ",";
                
                for (const auto& trip : line.daily_schedule) {
                    for (const auto& [trip_station, time] : trip) {
                        if (trip_station == station.id) {
                            file << setfill('0') << setw(2) << time / 60 << ":" << setw(2) << time % 60 << ",";
                            break;
                        }
                    }
                }
                file << "\n";
            }
        }

        cout << "Schedule has been exported to " << filename << endl;
    }
};

int main() {
    try {
        MetroScheduler scheduler;

        vector<string> station_names = {
            "Bairagarh", "Bairagarh Stadium", "Hemu Kalani", "Hala Pura Bus Stand", "Lal Ghati Square", "Idgarh Hills",
            "Collectorate", "Cambridge School MG Hospital", "Curfew Wali Mata Mandir", "Kamala Park", "Polytechnic Square",
            "TT Nagar Stadium", "Mata Mandir", "Jain Mandir", "Sharda Mandir", "Nutan College", "Khushabhau Thakrey ISBT",
            "Anna Nagar", "Carmel Convent", "Piplani Gurdwara Square", "Mahatma Gandhi Square", "Awadhpuri",
            "Karond Circle", "Krishi Upaj Mandi", "DIG Bungalow", "Sindhi Colony", "Nandra Bus Stand",
            "Railway Station", "Aish Bagh Crossing", "Pul Bogda", "Subhash Nagar Underpass", "Kendriya Vidyalaya",
            "DB City Mall", "Sargam Cinema", "Mansarovar Complex/ HBJ Station", "Habibganj Naka", "Alkapuri Bus Stand", "AIIMS",
            "Bhauri Bypass/NH12 Junction", "Peepainer Village (Aero city)", "Airport", "Airport Road", "Gandhinagar", "Airport Tiraha",
            "Manubhan Tekri", "10 No. Square", "Sai Board", "Vasant Kunj Bus Stop",
            "Ashok Garden Auto Stand", "Krishna Campus", "Jumerati Road", "MACT Square", "Panchsheel Nagar Bus Stop",
            "Ekant Park", "Sahapura Lake", "Bhoj University (Chunnabhatti)", "IIPM Sarvadham", "Mansarovar School",
            "CI Height Nayapura Bus Stop", "Sanskar Marriage Garden", "Bhairagarh Chichali", "Mother Teresa School",
            "Bhadbhada Square", "Depot Square", "Jawahar Chowk", "Roshanpura Square", "Minto Hall", "Lily Talkies",
            "Prabhat Square", "Govindpura", "Govindpura End Area", "J.K.Road", "Indrapuri", "Piplani (BHEL Township)", "Ratnagiri Tiraha",
            "RRL", "BU University", "Bagsewania Bus Stop", "Ashima The Lake City Mall", "Sri Ram Colony", "C21 Mall",
            "Scope College (Bharopur)", "Regional 1 / NH-12", "Harigangar Nagar", "Regional 2","Mandeep"
        };

        for (int i = 0; i < station_names.size(); ++i) {
            scheduler.add_station_name(i, station_names[i]);
        }

        // Define all six lines
        scheduler.add_line(1, "Green", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23});
        scheduler.add_line(2, "Red", {24, 25, 26, 27, 28, 29, 30, 67, 31, 32, 33, 34, 35, 36, 37});
        scheduler.add_line(3, "Blue", {38, 39, 40, 41, 42, 43, 44, 45, 46, 47});
        scheduler.add_line(4, "Yellow", {48, 49, 27, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61});
        scheduler.add_line(5, "Pink", {62, 63, 64, 65, 66, 68, 69, 70, 71, 72, 73, 74});
        scheduler.add_line(6, "Brown", {75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85});

        if (scheduler.generate_schedule()) {
            cout << "Successfully generated schedule. Displaying on terminal:\n\n";
            scheduler.print_schedule();
            
            string csv_filename = "metro_schedule.csv";
            scheduler.export_to_csv(csv_filename);
        } else {
            cout << "Failed to generate a valid schedule.\n";
        }

    } catch (const exception& e) {
        cerr << "An error occurred: " << e.what() << endl;
        return 1;
    }

    return 0;
}

//g++ Scheduler.cpp -I/home/balkishan/Downloads/or-tools_x86_64_Ubuntu-24.04_cpp_v9.11.4210/include -L/home/balkishan/Downloads/or-tools_x86_64_Ubuntu-24.04_cpp_v9.11.4210/lib -lortools -Wno-deprecated-declarations