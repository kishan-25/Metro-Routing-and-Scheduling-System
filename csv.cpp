#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
using namespace std;

// Function to count the number of rows in the file
int no_of_rows(ifstream& file) {
    string line;
    int counter = 0;
    while (getline(file, line)) {
        counter++;
    }
    file.clear();
    file.seekg(0, ios::beg);
    return counter;
}

// Function to count the number of columns in the first row
int no_of_cols(ifstream& file) {
    string line;
    if (getline(file, line)) {
        stringstream ss(line);
        string cell;
        int counter = 0;
        while (getline(ss, cell, ',')) {
            counter++;
        }
        file.clear();
        file.seekg(0, ios::beg);
        return counter;
    }
    return 0;
}

// Function to read the CSV data into a 2D vector
vector<vector<string>> read_csv(ifstream& file, int rows, int cols) {
    vector<vector<string>> matrix(rows, vector<string>(cols));
    string line;
    int row = 0;
    
    while (getline(file, line) && row < rows) {
        stringstream ss(line);
        string cell;
        int col = 0;
        
        while (getline(ss, cell, ',') && col < cols) {
            // Trim whitespace from the cell
            cell.erase(0, cell.find_first_not_of(" \t\r\n"));
            cell.erase(cell.find_last_not_of(" \t\r\n") + 1);
            matrix[row][col] = cell;
            col++;
        }
        row++;
    }
    return matrix;
}

int main() {
    string filename = "data.csv";  // Replace with your actual file name
    ifstream file(filename);
    
    if (!file.is_open()) {
        cout << "Failed to open the file: " << filename << endl;
        return 1;
    }

    int rows = no_of_rows(file);
    int cols = no_of_cols(file);

    cout << "Number of rows: " << rows << endl;
    cout << "Number of columns: " << cols << endl;

    vector<vector<string>> matrix = read_csv(file, rows, cols);
    file.close();

    // Display the matrix
    cout << "\nCSV Data:\n";
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout << matrix[i][j];
            if (j < cols - 1) cout << " | ";  // Using | as a separator for better readability
        }
        cout << endl;
    }

    return 0;
}