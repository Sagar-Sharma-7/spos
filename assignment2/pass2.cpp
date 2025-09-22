// macro_pass2.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

// Struct to hold MNT data
struct MntEntry {
    string name;
    int mdt_index;
};

// Function to load MNT from file
void load_mnt(map<string, int>& mnt) {
    ifstream mntFile("mnt.txt");
    string name;
    int index;
    while (mntFile >> name >> index) {
        mnt[name] = index;
    }
    mntFile.close();
}

// Function to load MDT from file
void load_mdt(vector<string>& mdt) {
    ifstream mdtFile("mdt.txt");
    string line;
    while (getline(mdtFile, line)) {
        mdt.push_back(line);
    }
    mdtFile.close();
}

int main() {
    map<string, int> MNT;
    vector<string> MDT;

    load_mnt(MNT);
    load_mdt(MDT);

    ifstream intermediateFile("intermediate.txt");
    ofstream expandedFile("expanded_code.txt");

    if (!intermediateFile.is_open()) {
        cout << "Error opening intermediate file." << endl;
        return 1;
    }

    string line;
    while (getline(intermediateFile, line)) {
        stringstream ss(line);
        string word;
        vector<string> tokens;
        while (ss >> word) {
            tokens.push_back(word);
        }

        if (tokens.empty()) continue;

        // Check if the first token is a macro name in our MNT
        if (MNT.count(tokens[0])) {
            // It's a macro call, so expand it
            int mdt_start_index = MNT[tokens[0]];
            
            // Create a list of the actual arguments from the call
            vector<string> actual_args;
            for (size_t i = 1; i < tokens.size(); ++i) {
                if (tokens[i].back() == ',') {
                    tokens[i].pop_back();
                }
                actual_args.push_back(tokens[i]);
            }

            // Start expanding from the line after the macro prototype in MDT
            for (int i = mdt_start_index + 1; i < MDT.size(); ++i) {
                string mdt_line = MDT[i];
                if (mdt_line == "MEND") {
                    break; // Stop expanding when we hit MEND
                }

                // Substitute positional arguments (#0, #1) with actual arguments
                stringstream mdt_ss(mdt_line);
                string mdt_word;
                string expanded_line = "";
                
                mdt_ss >> mdt_word; // Get the instruction
                expanded_line += mdt_word;

                while(mdt_ss >> mdt_word) { // Get the operands
                    if (mdt_word[0] == '#') {
                        int arg_index = stoi(mdt_word.substr(1));
                        expanded_line += " " + actual_args[arg_index];
                    } else {
                        expanded_line += " " + mdt_word;
                    }
                }
                expandedFile << expanded_line << endl;
            }
        } else {
            // Not a macro call, so just copy the line to the output
            expandedFile << line << endl;
        }
    }

    intermediateFile.close();
    expandedFile.close();

    cout << "Pass 2 of Macro Processor finished successfully." << endl;
    cout << "Check expanded_code.txt for the final output." << endl;

    return 0;
}