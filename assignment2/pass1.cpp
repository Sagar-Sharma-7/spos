// macro_pass1.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

// Using a struct for MNT entries for clarity
struct MntEntry {
    string name;
    int mdt_index;
};

int main() {
    // Data structures for Pass 1
    vector<MntEntry> MNT;
    vector<string> MDT;
    map<string, int> arg_list; // To map argument name to its position (#1, #2)

    ifstream inputFile("input_macro.txt");
    ofstream mntFile("mnt.txt");
    ofstream mdtFile("mdt.txt");
    ofstream intermediateFile("intermediate.txt");

    if (!inputFile.is_open()) {
        cout << "Error opening input file." << endl;
        return 1;
    }

    string line;
    bool is_defining_macro = false;

    while (getline(inputFile, line)) {
        stringstream ss(line);
        string word;
        vector<string> tokens;
        while (ss >> word) {
            tokens.push_back(word);
        }

        if (tokens.empty()) continue;

        // Check for the start of a macro definition
        if (tokens.size() > 1 && tokens[1] == "MACRO") {
            is_defining_macro = true;
            
            // 1. Create MNT entry
            MntEntry mnt_entry;
            mnt_entry.name = tokens[0];
            mnt_entry.mdt_index = MDT.size();
            MNT.push_back(mnt_entry);

            // 2. Process arguments and create the first line of MDT
            arg_list.clear();
            string mdt_line = tokens[0]; // Start with the macro name
            for (size_t i = 2; i < tokens.size(); ++i) {
                // Remove commas if they exist
                if (tokens[i].back() == ',') {
                    tokens[i].pop_back();
                }
                arg_list[tokens[i]] = i - 2; // &ARG1 -> 0, &ARG2 -> 1
                mdt_line += " #" + to_string(i - 2);
            }
            MDT.push_back(mdt_line);

            continue; // Skip to the next line of input
        }

        // Check for the end of a macro definition
        if (tokens[0] == "MEND") {
            is_defining_macro = false;
            MDT.push_back("MEND");
            continue;
        }

        // If we are inside a macro definition, add the line to MDT
        if (is_defining_macro) {
            string mdt_line = tokens[0]; // The instruction
            for (size_t i = 1; i < tokens.size(); ++i) {
                string operand = tokens[i];
                if (operand.back() == ',') {
                    operand.pop_back();
                }

                // Replace formal arguments with positional notation (#0, #1, ...)
                if (arg_list.count(operand)) {
                    mdt_line += " #" + to_string(arg_list[operand]);
                } else {
                    mdt_line += " " + operand;
                }
                if (i < tokens.size() - 1) {
                    mdt_line += ",";
                }
            }
            MDT.push_back(mdt_line);
        } 
        
        // If not in a macro, write to the intermediate file
        else {
            intermediateFile << line << endl;
        }
    }

    // Write MNT to mnt.txt
    for (const auto& entry : MNT) {
        mntFile << entry.name << " " << entry.mdt_index << endl;
    }

    // Write MDT to mdt.txt
    for (const auto& def_line : MDT) {
        mdtFile << def_line << endl;
    }

    inputFile.close();
    mntFile.close();
    mdtFile.close();
    intermediateFile.close();

    cout << "Pass 1 of Macro Processor finished successfully." << endl;
    cout << "Check mnt.txt, mdt.txt, and intermediate.txt" << endl;

    return 0;
}