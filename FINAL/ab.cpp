#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <iomanip>
#include <sstream>
#include <queue>
#include <algorithm>
using namespace std;

class Time {
private:
    int hours;
    int minutes;

public:
    Time(int h = 0, int m = 0) : hours(h), minutes(m) {
        normalize();
    }

    Time(const string& timeStr) {
        sscanf(timeStr.c_str(), "%d:%d", &hours, &minutes);
        normalize();
    }

    void normalize() {
        hours += minutes / 60;
        minutes %= 60;
        hours %= 24;
    }

    Time operator+(int mins) const {
        Time result(hours, minutes + mins);
        result.normalize();
        return result;
    }

    Time operator-(const Time& other) const {
        int totalMins = (hours * 60 + minutes) - (other.hours * 60 + other.minutes);
        return Time(0, totalMins);
    }

    int toMinutes() const {
        return hours * 60 + minutes;
    }

    bool operator<(const Time& other) const {
        return toMinutes() < other.toMinutes();
    }

    bool operator<=(const Time& other) const {
        return toMinutes() <= other.toMinutes();
    }

    bool operator==(const Time& other) const {
        return toMinutes() == other.toMinutes();
    }

    string toString() const {
        stringstream ss;
        ss << setfill('0') << setw(2) << hours << ":" 
           << setfill('0') << setw(2) << minutes;
        return ss.str();
    }
};

struct Station {
    string name;
    int platformCount;
    int minStopTime;      // Minimum time train should stop at station
    int maxStopTime;      // Maximum time train can stop at station
    bool isTerminal;      // Is this a terminal station
    bool isJunction;      // Is this a junction station
    
    Station(string n, int p = 2, int minStop = 1, int maxStop = 3, 
            bool term = false, bool junc = false) 
        : name(n), platformCount(p), minStopTime(minStop), 
          maxStopTime(maxStop), isTerminal(term), isJunction(junc) {}
};

struct TrainSchedule {
    string lineId;
    string trainId;
    vector<pair<string, Time>> stops;  // station name and arrival time
};

class MetroScheduler {
private:
    map<string, vector<Station>> lines;
    map<string, Station> stations;
    map<string, vector<string>> intersections;
    vector<TrainSchedule> schedules;
    map<string, map<Time, int>> stationOccupancy;  // station -> time -> platform count

    // Station-to-station travel times (in minutes)
    map<pair<string, string>, int> travelTimes;

public:
    MetroScheduler() {
        initializeStations();
        initializeLines();
        initializeTravelTimes();
    }

    void initializeStations() {
        // Initialize stations with their constraints
        // Format: name, platforms, min stop time, max stop time, isTerminal, isJunction
        
        // Major terminals
        stations["Bairagarh"] = Station("Bairagarh", 3, 5, 10, true, false);
        stations["AIIMS"] = Station("AIIMS", 3, 5, 10, true, false);
        stations["Mandideep"] = Station("Mandideep", 3, 5, 10, true, false);
        
        // Major junctions
        stations["Railway Station"] = Station("Railway Station", 4, 3, 5, false, true);
        stations["MACT Square"] = Station("MACT Square", 4, 3, 5, false, true);
        stations["Pul Bogda"] = Station("Pul Bogda", 4, 3, 5, false, true);
        
        // Regular stations (adding a few examples - you would add all stations)
        stations["Polytechnic Square"] = Station("Polytechnic Square", 2, 1, 3, false, false);
        stations["Krishna Campus"] = Station("Krishna Campus", 2, 1, 3, false, false);
        // ... Add all other stations similarly
    }

    void initializeLines() {
        // Line 1
        vector<Station> line1Stations = {
            stations["Bairagarh"],
            stations["Bairagarh Stadium"],
            // ... add all Line 1 stations
            stations["Awadhpuri"]
        };
        lines["Line-1"] = line1Stations;

        // Similarly initialize other lines
        // ... add remaining lines
    }

    void initializeTravelTimes() {
        // Set travel times between adjacent stations
        // Example: ("Station1", "Station2") -> minutes
        travelTimes[{"Bairagarh", "Bairagarh Stadium"}] = 4;
        // ... add all station-to-station travel times
    }

    bool checkPlatformAvailability(const string& station, const Time& time) {
        if (stationOccupancy[station].find(time) == stationOccupancy[station].end()) {
            return true;
        }
        return stationOccupancy[station][time] < stations[station].platformCount;
    }

    void occupyPlatform(const string& station, const Time& time) {
        stationOccupancy[station][time]++;
    }

    bool generateTrainSchedule(const string& lineId, const string& trainId, 
                             const Time& startTime, bool forward) {
        TrainSchedule schedule;
        schedule.lineId = lineId;
        schedule.trainId = trainId;

        const vector<Station>& lineStations = lines[lineId];
        Time currentTime = startTime;
        
        vector<Station> stationsToVisit = forward ? lineStations : 
            vector<Station>(lineStations.rbegin(), lineStations.rend());

        for (size_t i = 0; i < stationsToVisit.size(); i++) {
            const Station& station = stationsToVisit[i];
            
            // Check platform availability
            if (!checkPlatformAvailability(station.name, currentTime)) {
                return false;  // Schedule creation failed
            }

            // Add stop to schedule
            schedule.stops.push_back({station.name, currentTime});
            
            // Occupy platform
            occupyPlatform(station.name, currentTime);

            // Calculate next arrival time if not at last station
            if (i < stationsToVisit.size() - 1) {
                string nextStation = stationsToVisit[i + 1].name;
                pair<string, string> route = {station.name, nextStation};
                
                // Add station stop time and travel time to next station
                currentTime = currentTime + station.minStopTime + travelTimes[route];
            }
        }

        schedules.push_back(schedule);
        return true;
    }

    void generateAllSchedules(const string& startTimeStr = "06:00", 
                            const string& endTimeStr = "20:00", 
                            int frequencyMinutes = 30) {
        Time startTime(startTimeStr);
        Time endTime(endTimeStr);

        for (const auto& line : lines) {
            string lineId = line.first;
            int trainCount = 1;
            bool forward = true;

            for (Time currentTime = startTime; currentTime <= endTime; 
                 currentTime = currentTime + frequencyMinutes) {
                
                string trainId = lineId + "_Train_" + to_string(trainCount);
                
                if (generateTrainSchedule(lineId, trainId, currentTime, forward)) {
                    trainCount++;
                    forward = !forward;  // Alternate direction
                }
            }
        }
    }

    void printStationSchedule(const string& stationName) {
        cout << "\nSchedule for " << stationName << "\n";
        cout << string(70, '=') << endl;
        
        vector<pair<Time, string>> stationTimes;
        
        for (const auto& schedule : schedules) {
            for (const auto& stop : schedule.stops) {
                if (stop.first == stationName) {
                    stationTimes.push_back({stop.second, 
                        schedule.lineId + " " + schedule.trainId});
                }
            }
        }
        
        sort(stationTimes.begin(), stationTimes.end());
        
        cout << left << setw(10) << "Time" 
             << setw(15) << "Line" 
             << setw(20) << "Train ID" 
             << "Platform\n";
        cout << string(70, '-') << endl;
        
        for (const auto& entry : stationTimes) {
            cout << left << setw(10) << entry.first.toString()
                 << setw(35) << entry.second
                 << "P" << (stationOccupancy[stationName][entry.first]) << "\n";
        }
    }

    void printLineSchedule(const string& lineId) {
        cout << "\nSchedule for " << lineId << "\n";
        cout << string(100, '=') << endl;
        
        for (const auto& schedule : schedules) {
            if (schedule.lineId == lineId) {
                cout << "\nTrain ID: " << schedule.trainId << "\n";
                cout << string(50, '-') << endl;
                
                for (const auto& stop : schedule.stops) {
                    cout << left << setw(30) << stop.first 
                         << stop.second.toString() << "\n";
                }
            }
        }
    }
};

int main() {
    MetroScheduler scheduler;
    scheduler.generateAllSchedules();

    // Print schedules for major stations
    vector<string> majorStations = {
        "Railway Station", "MACT Square", "Pul Bogda",
        "Bairagarh", "AIIMS", "Mandideep"
    };

    for (const string& station : majorStations) {
        scheduler.printStationSchedule(station);
    }

    // Print schedules for each line
    for (int i = 1; i <= 6; i++) {
        scheduler.printLineSchedule("Line-" + to_string(i));
    }

    return 0;
}