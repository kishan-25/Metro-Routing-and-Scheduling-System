#include <iostream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
using namespace std;

const int NUM_STATIONS = 87; // 0 to 86
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
    vector<std::vector<pair<int, int>>> daily_schedule; // station_id, time for each trip

    MetroLine(int _id, const string& _name) : id(_id), name(_name) {}
};

class MetroScheduler {
private:
    vector<MetroLine> lines;
    map<int,string> station_names;

    int calculate_travel_time(int from_station, int to_station) {
        // Simplified travel time calculation
        return abs(from_station - to_station) + 2; // Minimum 2 minutes between stations
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
                vector<std::pair<int, int>> trip_schedule;
                int current_time = start_time;

                for (int i = 0; i < line.stations.size(); ++i) {
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
                for (int minute = 0; minute < 60; minute += 30) {
                    cout << setw(5) << setfill('0') << right << hour << ":" << setw(2) << minute << " | ";
                }
            }
            cout << "\n" << string(250, '-') << "\n";
            
            // Print schedule for each station
            for (const auto& station : line.stations) {
                cout << setw(30) << setfill(' ') << left << station.name << " | ";
                
                for (int hour = START_TIME; hour < END_TIME; ++hour) {
                    for (int minute = 0; minute < 60; minute += 30) {
                        bool found = false;
                        for (const auto& trip : line.daily_schedule) {
                            for (const auto& [trip_station, time] : trip) {
                                if (trip_station == station.id && time / 60 == hour && (time % 60) / 30 == minute / 30) {
                                    cout << setw(5) << setfill('0') << right << time / 60 << ":" << setw(2) << time % 60 << " | ";
                                    found = true;
                                    break;
                                }
                            }
                            if (found) break;
                        }
                        if (!found) {
                            cout << setw(7) << setfill(' ') << " " << " | ";
                        }
                    }
                }
                cout << "\n";
            }
            cout << "\n\n";
        }
    }
};

int main() {
    try {
        MetroScheduler scheduler;

        std::vector<std::string> station_names = {
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
            "Scope College (Bharopur)", "Regional 1 / NH-12", "Harigangar Nagar", "Regional 2"
        };

        for (int i = 0; i < station_names.size(); ++i) {
            scheduler.add_station_name(i, station_names[i]);
        }

        // Define all six lines
        scheduler.add_line(1, "Green", {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21});
        scheduler.add_line(2, "Red", {22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37});
        scheduler.add_line(3, "Blue", {38, 39, 40, 41, 42, 43, 44, 45, 46, 47});
        scheduler.add_line(4, "Yellow", {48, 49, 27, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61});
        scheduler.add_line(5, "Pink", {62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74});
        scheduler.add_line(6, "Brown", {75, 76, 77, 78, 79, 80, 81, 82, 83, 84});

        if (scheduler.generate_schedule()) {
            std::cout << "Successfully generated schedule:\n\n";
            scheduler.print_schedule();
        } else {
            std::cout << "Failed to generate a valid schedule.\n";
        }

    } catch (const std::exception& e) {
        std::cerr << "An error occurred: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}


//g++ Scheduler.cpp -I/home/balkishan/Downloads/or-tools_x86_64_Ubuntu-24.04_cpp_v9.11.4210/include -L/home/balkishan/Downloads/or-tools_x86_64_Ubuntu-24.04_cpp_v9.11.4210/lib -lortools -Wno-deprecated-declarations
