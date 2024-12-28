#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
#include <typeinfo>

using namespace std;

// Estrutura para mapeamento de instruções para opcodes
struct OpcodeMap {
    string instruction;
    string opcode;
};

// Mapeamento de instruções para opcodes
vector<OpcodeMap> opcodeMap = {
    {"ADD", "01"},
    {"SUB", "02"},
    {"MUL", "03"},
    {"DIV", "04"},
    {"JMP", "05"},
    {"JMPN", "06"},
    {"JMPP", "07"},
    {"JMPZ", "08"},
    {"COPY", "09"},
    {"LOAD", "10"},
    {"STORE", "11"},
    {"INPUT", "12"},
    {"OUTPUT", "13"},
    {"STOP", "14"}
};

string lowerCase(string &str) {
    for (int i = 0; i < str.length(); i++) {
        
        // Remove espaços e tababulações
        // if (str[i] == ' ' || str[i] == '\t') {
        //     str.erase(i, 1);
        // }

        str[i] = tolower(str[i]);
    }

    return str; 
}   

vector<string> split(string s, string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos) {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

int main() {
    ifstream inputFile("exemplo1.asm");

    if (!inputFile.is_open()) {
        cerr << "Erro ao abrir arquivos" << endl;
        return 1;
    }

    string line;
    string section_text = "";
    string section_data = "";

    int linha = 0;
    int aux = 0;

    while (getline(inputFile, line)) {
        vector<string> v = split (line, ";");
        
        if(v[0].empty()) {
            continue;
        }

        if (v[0] == "SECTION TEXT") aux = 0;
        if (v[0] == "SECTION DATA") aux = 1;

        if (aux == 0) section_text += v[0] + "\n";
        if (aux == 1) section_data += v[0] + "\n";

        // cout << v[0] << endl;

        // linha++;
    }

    inputFile.close();

    cout << section_text << section_data;
    return 0;
}
