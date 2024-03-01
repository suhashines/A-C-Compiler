#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

using namespace std;

// Function to perform optimizations
void optimizeAssembly(const string& inputFilename, const string& outputFilename) {
    ifstream inputFile(inputFilename);
    ofstream outputFile(outputFilename);

    if (!inputFile.is_open()) {
        cerr << "Error: Unable to open input file: " << inputFilename << endl;
        return;
    }

    if (!outputFile.is_open()) {
        cerr << "Error: Unable to open output file: " << outputFilename << endl;
        return;
    }

    vector<string> assemblyCode;
    string line;

    
    while (getline(inputFile, line)) {
        assemblyCode.push_back(line);
    }

    vector<string> optimizedCode;

    // optimizations
    for (size_t i = 0; i < assemblyCode.size(); ++i) {
        string currentLine = assemblyCode[i];

        if (i < assemblyCode.size() - 1) {
            string nextLine = assemblyCode[i + 1];

            // Eliminating redundant PUSH and POP operations
            if (currentLine.find("PUSH") != string::npos && nextLine.find("POP") != string::npos) {
                string pushOperand = currentLine.substr(currentLine.find(' ') + 1);
                string popOperand = nextLine.substr(nextLine.find(' ') + 1);
                if (pushOperand == popOperand) {
                    ++i; 
                    continue;
                }
            }

            // Reducing unnecessary arithmetic operations
            if ((currentLine.find("ADD") != string::npos && currentLine.find(",0") != string::npos) ||
                (currentLine.find("MUL") != string::npos && currentLine.find(",1") != string::npos)) {
                continue; 
            }

            //  Eliminating redundant MOV instructions
            if (currentLine.find("MOV") != string::npos && nextLine.find("MOV") != string::npos) {
                string mov1Src, mov1Dest, mov2Src, mov2Dest;
                stringstream ss1(currentLine), ss2(nextLine);
                ss1 >> mov1Src >> mov1Dest;
                ss2 >> mov2Src >> mov2Dest;
                if (mov1Dest == mov2Src && mov1Src == mov2Dest) {
                    ++i; 
                    continue;
                }
            }
        }

        // no optimization needed
        optimizedCode.push_back(currentLine);
    }

   
    for (const string& optimizedLine : optimizedCode) {
        outputFile << optimizedLine << endl;
    }

    cout << "Optimization complete. Output written to " << outputFilename << endl;
}

int main() {
    string inputFilename = "2005037_assembly.txt";
    string outputFilename = "2005037_code.asm";

    optimizeAssembly(inputFilename, outputFilename);

    return 0;
}
