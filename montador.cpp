#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>

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

// Mapeamento das macros
map<string, string> macroMap;

string lowerCase(string &str) {
    string res = "";
    for (int i = 0; i < str.length(); i++) {
        
        // Remove espaços e tababulações
        // if (str[i] == ' ' || str[i] == '\t') {
        //     str.erase(i, 1);
        // }

        res += tolower(str[i]);
    }

    return res; 
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


int main(int argc, char* argv[]) {

    if (argc != 2) {
        cerr << "Forneça o nome do arquivo como argumento. Exemplo: ./montador exemplo1.asm" << endl;
        return 1;
    }

    string arquivo = argv[1];

    ifstream inputFile(arquivo);

    if (!inputFile.is_open()) {
        cerr << "Erro ao abrir arquivos" << endl;
        return 1;
    }

    string line;
    string section_text = "";
    string section_data = "";

    int aux = 0;
    string macro_flag = "";

    while (getline(inputFile, line)) {
        vector<string> v = split (line, ";");
        
        if(v[0].empty()) {
            continue;
        }

        // Regex para encontrar a palavra "macro" isolada
        regex find_macro("\\bmacro\\b");

        if (regex_search(lowerCase(v[0]), find_macro)) {
            macro_flag = split(v[0], ":")[0];
            macroMap[macro_flag] = {""};
            continue;
        }

        // Regex para encontrar a palavra "endmacro" isolada
        regex find_endmacro("\\bendmacro\\b");
        if (regex_search(lowerCase(v[0]), find_endmacro)) {
            macro_flag = "";
            continue;
        }
    
        if (macro_flag == "") {
            if (v[0] == "SECTION TEXT") aux = 0;
            if (v[0] == "SECTION DATA") aux = 1;

            if (aux == 0) {
                string modified = v[0];
                modified.erase(remove(modified.begin(), modified.end(), '\n'), modified.end());
                modified.erase(remove(modified.begin(), modified.end(), ' '), modified.end());
                


                if (macroMap.find(modified) != macroMap.end()) {
                    if (macroMap[modified].back() == '\n') {
                        macroMap[modified].pop_back();
                    }
                    v[0] = macroMap[modified];
                }

                section_text += v[0] + "\n";
            }

            if (aux == 1) section_data += v[0] + "\n";
        } else {
            macroMap[macro_flag] += v[0] + "\n";
        }

    }

    inputFile.close();

    cout << section_text << section_data;
    // for (const auto& [rotulo, macro] : macroMap) {
    //     cout << "Rótulo: " << rotulo << ", Corpo: " << macroMap[rotulo] << endl;
    // }


    // Nome do arquivo de saída
    string nomeArquivo = "myfile.pre";

    std::ofstream file(nomeArquivo);
    
    if (file.is_open()) {
        file << section_text << section_data;
        
        file.close();
        cout << "Arquivo " << nomeArquivo << " criado com sucesso!" << endl;
    } else {
        cerr << "Erro ao abrir o arquivo!" << endl;
    }



    return 0;
}
