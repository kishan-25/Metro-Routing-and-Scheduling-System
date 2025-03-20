import csv

# Full station list (as provided)
stations_full = [
    "Bairagarh","Bairagarh Stadium","Hemu Kalani","Hala Pura Bus Stand","Lal Ghati Square","Idgarh Hills", 
    "Collectorate","Cambridge School MG Hospital","Curfew Wali Mata Mandir","Kamala Park","Polytechnic Square",
    "Roshanpura Square","TT Nagar Stadium","Mata Mandir","Jain Mandir","Sharda Mandir","Nutan College",
    "Mansarovar Complex/ HBJ Station","Khushabhau Thakrey ISBT","Anna Nagar","Carmel Convent",
    "Piplani Gurdwara Square","Mahatma Gandhi Square","Awadhpuri",
    # Line 2 (stations 24 to 37)
    "Karond Circle","Krishi Upaj Mandi","DIG Bungalow","Sindhi Colony","Nandra Bus Stand", 
    "Railway Station","Aish Bagh Crossing","Subhash Nagar Underpass","Kendriya Vidyalaya", 
    "DB City Mall","Sargam Cinema","Habibganj Naka","Alkapuri Bus Stand","AIIMS",
    # Line 3 (stations 38 to 47)
    "Bhauri Bypass/NH12 Junction","Peepainer Village (Aero city)","Airport","Airport Road","Gandhinagar","Airport Tiraha",
    "Manubhan Tekri","10 No. Square","Sai Board","Vasant Kunj Bus Stop",
    # Line 4 (stations 48 to 61)
    "Mother Teresa School","Bhairagarh Chichali", "Sanskar Marriage Garden","CI Height Nayapura Bus Stop","Mansarovar School",
    "IIPM SarvadhaM","Bhoj University (Chunnabhatti)","Sahapura Lake", "Ekant Park","Panchsheel Nagar Bus Stop","MACT Square", 
    "Jumerati Road","Krishna Campus","Ashok Garden Auto Stand",
    # Line 5 (stations 62 to 74)
    "Bhadbhada Square","Depot Square","Jawahar Chowk","Minto Hall","Lily Talkies","Pul Bogda","Prabhat Square",
    "Govindpura","Govindpura End Area","J.K.Road","Indrapuri","Piplani (BHEL Township)","Ratnagiri Tiraha", 
    # Line 6 (stations 75 to 85)
    "RRL","BU University","Bagsewania Bus Stop","Ashima The Lake City Mall","Sri Ram Colony","C21 Mall", 
    "Scope College (Bharopur)","Regional 1 / NH-12","Harigangar Nagar","Regional 2","Mandideep"
]

# Common parameters
travel_time = 2         # minutes between stations
regular_dwell = 2       # dwell time at regular stops
junction_dwell = 4      # dwell time at junctions
turnaround_time = 10    # turnaround time at terminal
first_departure = 6 * 60  # 06:00 in minutes

def generate_line_schedule(line_number, stations, metros, headway, junction_set):
    """
    Generates a schedule for one line.
    
    :param line_number: Integer (for file naming)
    :param stations: List of station names for the line.
    :param metros: List of metro IDs (e.g., ["M1", "M2", ...]).
    :param headway: Headway in minutes between consecutive metro departures from the origin.
    :param junction_set: Set of station names (subset of stations) that are junctions.
    :return: None (writes CSV file)
    """
    schedule = []
    # For each metro (each starting tour)
    for tour, metro in enumerate(metros):
        dep_time = first_departure + tour * headway  # starting departure for this metro
        current_time = dep_time  # working variable for time (in minutes)
        # For each station in the line
        for station in stations:
            arrival = current_time if station == stations[0] else current_time + travel_time
            # For the first station, we only mark departure
            if station == stations[0]:
                dwell = 0
                departure = arrival  # departure is the start time
            else:
                dwell = junction_dwell if station in junction_set else regular_dwell
                departure = arrival + dwell
            # Append schedule entry; store numeric time for internal calculation
            schedule.append({
                "Metro ID": metro,
                "Station": station,
                "Arrival": f"{arrival // 60:02}:{arrival % 60:02}",
                "Departure": f"{departure // 60:02}:{departure % 60:02}",
                "Departure (min)": departure
            })
            current_time = departure
        # After finishing the tour (terminal station), add turnaround time for next tour of the same metro.
        current_time += turnaround_time
    # Remove internal numeric field
    for entry in schedule:
        if "Departure (min)" in entry:
            del entry["Departure (min)"]
    
    # Save schedule as CSV
    filename = f"/mnt/data/metro_schedule_line{line_number}.csv"
    with open(filename, mode="w", newline="") as file:
        writer = csv.DictWriter(file, fieldnames=["Metro ID", "Station", "Arrival", "Departure"])
        writer.writeheader()
        writer.writerows(schedule)
    return filename

# Define parameters per line:
# Line 2: stations indexes 24-37, 14 stations, 3 metros, headway=43, junctions: {"Railway Station"}
stations_line2 = stations_full[24:38]
metros_line2 = ["M1", "M2", "M3"]
junctions_line2 = {"Railway Station"}

# Line 3: stations indexes 38-47, 10 stations, 2 metros, headway=46, no junctions
stations_line3 = stations_full[38:48]
metros_line3 = ["M1", "M2"]
junctions_line3 = set()

# Line 4: stations indexes 48-61, 14 stations, 3 metros, headway=41, no junctions
stations_line4 = stations_full[48:62]
metros_line4 = ["M1", "M2", "M3"]
junctions_line4 = set()

# Line 5: stations indexes 62-75, 13 stations, 2 metros, headway=60, junctions: {"Pul Bogda"}
stations_line5 = stations_full[62:75]
metros_line5 = ["M1", "M2"]
junctions_line5 = {"Pul Bogda"}

# Line 6: stations indexes 75-86, 11 stations, 2 metros, headway=50, no junctions
stations_line6 = stations_full[75:86]
metros_line6 = ["M1", "M2"]
junctions_line6 = set()

# Generate CSV files
csv_line2 = generate_line_schedule(2, stations_line2, metros_line2, 43, junctions_line2)
csv_line3 = generate_line_schedule(3, stations_line3, metros_line3, 46, junctions_line3)
csv_line4 = generate_line_schedule(4, stations_line4, metros_line4, 41, junctions_line4)
csv_line5 = generate_line_schedule(5, stations_line5, metros_line5, 60, junctions_line5)
csv_line6 = generate_line_schedule(6, stations_line6, metros_line6, 50, junctions_line6)

print(csv_line2, csv_line3, csv_line4, csv_line5, csv_line6)
