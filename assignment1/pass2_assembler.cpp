// pass2.cpp

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <map>

using namespace std;

// Helper to load Symbol Table from file
void load_symtab(map<string, int>& symtab) {
    ifstream symtabFile("symtab.txt");
    string symbol;
    int address;
    while (symtabFile >> symbol >> address) {
        symtab[symbol] = address;
    }
    symtabFile.close();
}

// Helper to load Literal Table from file
void load_littab(vector<pair<string, int>>& littab) {
    ifstream littabFile("littab.txt");
    string literal;
    int address;
    while (littabFile >> literal >> address) {
        littab.push_back({literal, address});
    }
    littabFile.close();
}

int main() {
    map<string, int> SYMTAB;
    vector<pair<string, int>> LITTAB;

    load_symtab(SYMTAB);
    load_littab(LITTAB);

    ifstream icFile("ic.txt");
    ofstream machineCodeFile("machine_code.txt");

    if (!icFile.is_open()) {
        cout << "Error opening intermediate code file." << endl;
        return 1;
    }

    string line;
    int lc = 0;

    while (getline(icFile, line)) {
        stringstream ss(line);
        string token;
        vector<string> tokens;
        
        // Simple parser for the intermediate code format (e.g., "(IS,04) (1) (S,A)")
        // This is a basic parser and can be made more robust
        size_t pos = 0;
        string temp_line = line;
        while ((pos = temp_line.find('(')) != string::npos) {
            size_t end_pos = temp_line.find(')');
            tokens.push_back(temp_line.substr(pos + 1, end_pos - pos - 1));
            temp_line.erase(0, end_pos + 2); // +2 to remove ')' and space
        }
        
        if (tokens.empty()) continue;

        string class_type = tokens[0].substr(0, tokens[0].find(','));
        string opcode = tokens[0].substr(tokens[0].find(',') + 1);

        if (class_type == "IS") {
            machineCodeFile << "+ " << opcode << " ";
            
            if (tokens.size() > 1) { // Register or first operand
                if (tokens[1].length() == 1) { // Register
                     machineCodeFile << tokens[1] << " ";
                }
            } else {
                 machineCodeFile << "0 "; // No register
            }

            if (tokens.size() > 2) { // Memory operand (Symbol or Literal)
                string op_type = tokens[2].substr(0, tokens[2].find(','));
                string op_value = tokens[2].substr(tokens[2].find(',') + 1);

                if (op_type == "S") {
                    machineCodeFile << SYMTAB[op_value] << endl;
                } else if (op_type == "L") {
                    machineCodeFile << LITTAB[stoi(op_value)].second << endl;
                }
            } else {
                 machineCodeFile << "000" << endl; // For instructions like STOP
            }
        } 
        
        else if (class_type == "DL" && opcode == "01") { // DC - Declare Constant
            string value = tokens[1].substr(tokens[1].find(',') + 1);
            machineCodeFile << "+ 00 0 00" << value << endl;
        } 
        
        // AD and DS (except DC) do not generate machine code, so we ignore them.
    }

    icFile.close();
    machineCodeFile.close();

    cout << "Pass 2 finished successfully." << endl;
    cout << "Check machine_code.txt for the output." << endl;

    return 0;
}