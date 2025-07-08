#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;

string removeComments(const string& code) {
    string result;
    bool inSingleLineComment = false;
    bool inMultiLineComment = false;

    int i = 0;
    int length = code.length();

    while (i < length) {
        if (inSingleLineComment) {
            if (code[i] == '\n') {
                inSingleLineComment = false;
                result += '\n';
            }
        }
        else if (inMultiLineComment) {
            if (code[i] == '*' && i + 1 < length && code[i + 1] == '/') {
                inMultiLineComment = false;
                i++; 
            }
        }
        else {
            if (code[i] == '/' && i + 1 < length) {
                if (code[i + 1] == '/') {
                    inSingleLineComment = true;
                    i++;
                } else if (code[i + 1] == '*') {
                    inMultiLineComment = true;
                    i++;
                } else {
                    result += code[i];
                }
            } else {
                result += code[i];
            }
        }
        i++;
    }

    return result;
}

int main() {
    string filename = "input.cpp";
    ifstream inFile(filename);
    if (!inFile) {
        cerr << "Failed to open " << filename << " for reading.\n";
        return 1;
    }

    string code = "";
    string line;

    while (getline(inFile, line)) {
        code += line + '\n';
    }

    inFile.close();

    string cleanedCode = removeComments(code);

    ofstream outFile(filename);
    if (!outFile) {
        cerr << "Failed to open " << filename << " for writing.\n";
        return 1;
    }

    outFile << cleanedCode;
    outFile.close();

    cout << "Comments removed and file updated: " << filename << '\n';

    return 0;
}
