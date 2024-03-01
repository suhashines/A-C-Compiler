#include <iostream>
#include <map>
#include <string>

class Register {
private:
    std::map<std::string, bool> registers;

public:
    //registers map initialization
    Register() : registers{{"AX", true}, {"BX", true}, {"CX", true}, {"DX", true}} {}

    // Function to get a register and mark it as used
    std::string getRegister() {
        for (auto& reg : registers) {
            if (reg.second) { // If the register is available
                reg.second = false; // Mark it as used
                return reg.first; // Return the register name
            }
        }
        return ""; // If no available registers
    }

    // Function to reset registers
    void resetRegister(const std::string& reg1, const std::string& reg2) {
        if (registers.find(reg1) != registers.end())
            registers[reg1] = true; // Mark reg1 as available
        if (registers.find(reg2) != registers.end())
            registers[reg2] = true; // Mark reg2 as available
    }

    void resetRegister(const std::string& reg){
        if (registers.find(reg) != registers.end())
            registers[reg] = true; // Mark reg as available
    }

    void acquireRegister(const std::string & reg){
        if (registers.find(reg) != registers.end())
            registers[reg] = false; // Mark reg as unavailable
    }
};