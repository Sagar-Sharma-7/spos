// pass1.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

// Structure to hold information about an opcode
struct OpcodeInfo {
    string opcode;
    string type; // IS, AD, DS
};

// Global Opcode Table (OPTAB)
map<string, OpcodeInfo> OPTAB;

// Function to initialize the Opcode Table
void initialize_optab() {
    OPTAB["STOP"] = {"00", "IS"};
    OPTAB["ADD"] = {"01", "IS"};
    OPTAB["SUB"] = {"02", "IS"};
    OPTAB["MULT"] = {"03", "IS"};
    OPTAB["MOVER"] = {"04", "IS"};
    OPTAB["MOVEM"] = {"05", "IS"};
    OPTAB["COMP"] = {"06", "IS"};
    OPTAB["BC"] = {"07", "IS"};
    OPTAB["READ"] = {"09", "IS"};
    OPTAB["PRINT"] = {"10", "IS"};
    
    OPTAB["START"] = {"01", "AD"};
    OPTAB["END"] = {"02", "AD"};
    OPTAB["ORIGIN"] = {"03", "AD"};
    OPTAB["EQU"] = {"04", "AD"};
    OPTAB["LTORG"] = {"05", "AD"};

    OPTAB["DC"] = {"01", "DS"};
    OPTAB["DS"] = {"02", "DS"};
}

// Helper to check if a string is a number
bool is_number(const string& s) {
    return !s.empty() && s.find_first_not_of("0123456789") == string::npos;
}


int main() {
    initialize_optab();

    // Data structures for Pass 1
    map<string, int> SYMTAB;
    vector<pair<string, int>> LITTAB;
    vector<int> POOLTAB;
    POOLTAB.push_back(0); // First pool starts at index 0 of LITTAB

    int lc = 0; // Location Counter
    int littab_ptr = 0;

    ifstream inputFile("input.txt");
    ofstream icFile("ic.txt");
    ofstream symtabFile("symtab.txt");
    ofstream littabFile("littab.txt");
    ofstream pooltabFile("pooltab.txt");

    if (!inputFile.is_open()) {
        cout << "Error opening input file." << endl;
        return 1;
    }

    string line;
    while (getline(inputFile, line)) {
        stringstream ss(line);
        string word;
        vector<string> tokens;
        while (ss >> word) {
            tokens.push_back(word);
        }

        string label = "", mnemonic = "", op1 = "", op2 = "";
        bool hasLabel = (OPTAB.find(tokens[0]) == OPTAB.end());

        if (hasLabel) {
            label = tokens[0];
            mnemonic = tokens[1];
            if (tokens.size() > 2) op1 = tokens[2];
            if (tokens.size() > 3) op2 = tokens[3];
        } else {
            mnemonic = tokens[0];
            if (tokens.size() > 1) op1 = tokens[1];
            if (tokens.size() > 2) op2 = tokens[2];
        }

        // 1. Handle Label
        if (!label.empty()) {
            if (SYMTAB.find(label) == SYMTAB.end()) {
                SYMTAB[label] = lc;
            } else {
                // If label is already there, it might be a forward reference updated by EQU
                // For simplicity, we just update it. A real assembler might error on re-definition.
                if (mnemonic != "EQU") {
                     SYMTAB[label] = lc;
                }
            }
        }
        
        // 2. Process Mnemonic
        if (OPTAB.find(mnemonic) != OPTAB.end()) {
            OpcodeInfo info = OPTAB[mnemonic];
            
            icFile << "(" << info.type << "," << info.opcode << ") ";

            if (mnemonic == "START") {
                lc = stoi(op1);
                icFile << "(C," << op1 << ")" << endl;
                continue; // Don't increment lc for START
            } 
            
            else if (mnemonic == "END" || mnemonic == "LTORG") {
                // Process literal pool
                for (int i = littab_ptr; i < LITTAB.size(); ++i) {
                    LITTAB[i].second = lc;
                    icFile << "(DL,01) (C," << LITTAB[i].first.substr(2, LITTAB[i].first.length() - 3) << ")" << endl;
                    lc++;
                }
                POOLTAB.push_back(LITTAB.size());
                littab_ptr = LITTAB.size();
                if(mnemonic == "END") icFile << endl;

            } 
            
            else if (mnemonic == "EQU") {
                // For simplicity, we handle only simple assignment like 'A EQU B'
                // A more complex handler would parse expressions like B+5
                if (SYMTAB.find(op1) != SYMTAB.end()) {
                    SYMTAB[label] = SYMTAB[op1];
                } else {
                    // Handle forward reference if needed, or assume defined
                    SYMTAB[label] = 0; // Or some error indicator
                }
                icFile << "(S," << op1 << ")" << endl;
                continue; // No LC increment for EQU
            } 
            
            else if (mnemonic == "DS") {
                int size = stoi(op1);
                icFile << "(C," << size << ")" << endl;
                lc += size;
                continue;
            } 
            
            else if (mnemonic == "DC") {
                icFile << "(C," << op1 << ")" << endl;
            } 
            
            else { // It's an Imperative Statement (IS)
                if (!op1.empty()) {
                    if (op1 == "AREG") icFile << "(1) ";
                    else if (op1 == "BREG") icFile << "(2) ";
                    else if (op1 == "CREG") icFile << "(3) ";
                    else { // It's a symbol
                        if (SYMTAB.find(op1) == SYMTAB.end()) {
                            SYMTAB[op1] = -1; // Forward reference
                        }
                        icFile << "(S," << op1 << ") ";
                    }
                }
                if (!op2.empty()) {
                    if (op2.rfind("='", 0) == 0) { // It's a literal
                        LITTAB.push_back({op2, -1});
                        icFile << "(L," << LITTAB.size()-1 << ")";
                    } else { // It's a symbol
                         if (SYMTAB.find(op2) == SYMTAB.end()) {
                            SYMTAB[op2] = -1; // Forward reference
                        }
                        icFile << "(S," << op2 << ")";
                    }
                }
                icFile << endl;
            }
            lc++;
        }
    }
    
    // Write tables to files
for (auto const& pair : SYMTAB) {
    symtabFile << pair.first << " " << pair.second << endl;
}

for (size_t i = 0; i < LITTAB.size(); ++i) {
    littabFile << LITTAB[i].first << " " << LITTAB[i].second << endl;
}
for (int index : POOLTAB) {
    pooltabFile << index << endl;
}

    inputFile.close();
    icFile.close();
    symtabFile.close();
    littabFile.close();
    pooltabFile.close();

    cout << "Pass 1 finished successfully." << endl;
    cout << "Check ic.txt, symtab.txt, littab.txt, and pooltab.txt" << endl;

    return 0;
}