#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>

using namespace std;

// Funções auxiliares
bool BothAreSpaces(char lhs, char rhs) {
    return (lhs == rhs) && (lhs == ' ');
}

string removeSpaces(string &str) {
    string::iterator res = unique(str.begin(), str.end(), BothAreSpaces);
    str.erase(res, str.end());
    return str;
}

string lowerCase(string &str) {
    string res = "";
    for (char c : str) {
        res += tolower(c);
    }
    return res;
}

string upperCase(string &str) {
    string res = "";
    for (char c : str) {
        res += toupper(c);
    }
    return res;
}

string trim(string &str) {
    auto start = find_if_not(str.begin(), str.end(), [](unsigned char ch) {
        return isspace(ch);
    });

    auto end = find_if_not(str.rbegin(), str.rend(), [](unsigned char ch) {
        return isspace(ch);
    }).base();

    return (start < end ? string(start, end) : string());
}

string superTrim(string &str) {
    string result = str;
    size_t pos = result.find(',');

    if (pos != string::npos) {
        string before = result.substr(0, pos);
        string after = result.substr(pos + 1);

        before = trim(before);
        after = trim(after);

        result = before + "," + after;
    }
    return trim(result);
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

// Map das macros
map<string, string> macroMap;
void PreProcessamento(int argc, char *argv[]) {
    string arquivo = argv[1];
    ifstream inputFile(arquivo);

    if (!inputFile.is_open()) {
        cerr << "Erro ao abrir arquivos" << endl;
    }

    string line, section_text = "", section_data = "";
    int aux = 0;
    string macro_flag = "";

    while (getline(inputFile, line)) {
        vector<string> v = split(upperCase(line), ";");

        if (v[0].empty()) continue;

        // Verificar início e fim de macros
        regex find_macro("\\bMACRO\\b"), find_endmacro("\\bENDMACRO\\b");

        if (regex_search(v[0], find_macro)) {
            macro_flag = split(v[0], ":")[0];
            macroMap[macro_flag] = {""};
            continue;
        }

        if (regex_search(v[0], find_endmacro)) {
            macro_flag = "";
            continue;
        }

        if (macro_flag.empty()) {
            if (v[0] == "SECTION TEXT") aux = 0;
            if (v[0] == "SECTION DATA") aux = 1;

            string modified = v[0];
            modified.erase(remove(modified.begin(), modified.end(), '\n'), modified.end());
            modified.erase(remove(modified.begin(), modified.end(), ' '), modified.end());

            if (macroMap.find(modified) != macroMap.end()) {
                if (macroMap[modified].back() == '\n') {
                    macroMap[modified].pop_back();
                }
                v[0] = macroMap[modified];
            }

            v[0] = removeSpaces(v[0]);
            v[0] = superTrim(v[0]);

            // Remove quebra de linha do rótulo
            vector<string> tratar_rotulo = split(upperCase(v[0]), ":");
            if(tratar_rotulo.size() > 1 && tratar_rotulo[1].empty() && aux == 0) {
              section_text += v[0] + " ";
              continue;
            }

            if (aux == 0) section_text += superTrim(v[0]) + "\n";
            if (aux == 1) section_data += superTrim(v[0]) + "\n";

        } else {
            macroMap[macro_flag] += v[0] + "\n";
        }
    }

    inputFile.close();

    if (section_data.back() == '\n') {
        section_data.pop_back();
    }

    cout << section_text << section_data;

    string nomeArquivo = "myfile.pre";
    ofstream file(nomeArquivo);

    if (file.is_open()) {
        file << section_text << section_data;
        file.close();
        cout << "\nArquivo " << nomeArquivo << " criado com sucesso!" << endl;
    } else {
        cerr << "\nErro ao abrir o arquivo!" << endl;
    }
}


// Estrutura para mapeamento de instruções para opcodes
struct OpcodeInfo {
    string opcode;
    int tamanho;
};

// Mapeamento de instruções e diretivas
map<string, OpcodeInfo> tabelaInstrucoes = {
        {"ADD", {"01", 2}}, {"SUB", {"02", 2}}, {"MUL", {"03", 2}},
        {"DIV", {"04", 2}}, {"JMP", {"05", 2}}, {"JMPN", {"06", 2}},
        {"JMPP", {"07", 2}}, {"JMPZ", {"08", 2}}, {"COPY", {"09", 3}},
        {"LOAD", {"10", 2}}, {"STORE", {"11", 2}}, {"INPUT", {"12", 2}},
        {"OUTPUT", {"13", 2}}, {"STOP", {"14", 1}}
    };

map<string, int> tabelaDiretivas = {
    {"SPACE", 0}, {"CONST", 0}
};

map<string, int> tabelaSimbolos;

void PrimeiraPassagem(int argc, char *argv[]) {
    string arquivo = argv[1];
    ifstream inputFile(arquivo);

    if (!inputFile.is_open()) {
        cerr << "Erro ao abrir arquivo de pré-processamento" << endl;
        return;
    }

    int contador_posicao = 0;
    int contador_linha = 1;
    // regex para SPACE e CONST
    regex regex_space("\\bSPACE\\b"), regex_const("\\bCONST\\b");

    string line;
    // Enquanto arquivo fonte não chegou ao fim, faça: Obtém uma linha do fonte
    while (getline(inputFile, line)) {
        if (line == "SECTION TEXT" || line == "SECTION DATA") {
            continue;
        }
        // Separa os elementos da linha: rótulo, operação, operandos
        string rotulo;
        string operacao;
        //Se existe rótulo:
        if(line.find(":") != string::npos) {
          vector<string> separacao_instrucao = split(line, ":");
          vector<string> instrucao = split(superTrim(separacao_instrucao[1]), " ");
          
          rotulo = separacao_instrucao[0];
          operacao = split(superTrim(separacao_instrucao[1]), " ")[0];

          // Procura rótulo na Tabela de Símbolos
          if (tabelaSimbolos.find(rotulo) != tabelaSimbolos.end()) {
            // Se achou: Erro à símbolo redefinido
            cerr << "Linha: " << contador_linha << " " << "ERRO SINTÁTICO: Erro símbolo " << rotulo << " redefinido" << endl;
          } else {
            // Se achou: contador_posição = contador_posição + valor da diretiva
            if (regex_search(operacao, regex_space)) {
              // if (instrucao.size() == 2) {
              //   int valor = stoi(instrucao[1]);
              //   contador_posicao += valor;
              // } else {
              //   contador_posicao += tabelaDiretivas["SPACE"];
              // }

              // Se não achou: Insere rótulo e contador_posição na TS
              tabelaSimbolos[rotulo] = tabelaDiretivas["SPACE"];
              contador_linha++;
              continue;
            }
            if (regex_search(operacao, regex_const)) {
              // contador_posicao += tabelaDiretivas["CONST"];

              // Se não achou: Insere rótulo e contador_posição na TS
              tabelaSimbolos[rotulo] = stoi(instrucao[1]);

              contador_linha++;
              continue;
            }

            tabelaSimbolos[rotulo] = contador_posicao;
          }

          contador_posicao += tabelaInstrucoes[operacao].tamanho;
        }
        // Se não existe rótulo:
        else {
          // Procura operação na tabela de instruções
          operacao = split(superTrim(line), " ")[0];
          // cout << "Operação = " << operacao << endl;
          if (tabelaInstrucoes.find(operacao) != tabelaInstrucoes.end()) {
            // Se achou: contador_posição = contador_posição + tamanho da instrução
            contador_posicao += tabelaInstrucoes[operacao].tamanho;
          } else {
            cerr << "Linha: " << contador_linha << " " << "ERRO SINTÁTICO: operação " << operacao << " não identificada" << endl;
            return;
          }
        }

      contador_linha++;
    }

    // Imprime a tabela de símbolos
    cout << "Tabela de Símbolos:" << endl;
    for (auto const& x : tabelaSimbolos) {
        cout << x.first << " " << x.second << endl;
    } 
}


void SegundaPassagem() {  
    int contador_posicao = 0;
    int contador_linha = 1;

    string codigo_objeto = "";

    ifstream inputFile("myfile.pre");
    if (!inputFile.is_open()) {
        cerr << "Erro ao abrir arquivo de pré-processamento" << endl;
        return;
    }
    string line;
    // Enquanto arquivo fonte não chegou ao fim, faça: Obtém uma linha do fonte
    while (getline(inputFile, line)) {
        if (line == "SECTION TEXT" || line == "SECTION DATA") {
            continue;
        }
        // Separa os elementos da linha: rótulo, operação, operandos
        vector<string> instrucao = split(line, " ");
        string rotulo;
        string operacao;
        // Para cada operando que é símbolo, Procura operação na TS
        // Se não achou: Erro, símbolo indefinido
        // cerr << "ERRO SINTÁTICO: símbolo não foi definido" << endl;
        // Procura operação na tabela de instruções
        // Se achou: contador_posição = contador_posição + tamanho da instrução
          // Se o número e o tipo dos operandos está correto então gera código objeto conforme formato da instrução
          // codigo_objeto += //coloca o que precisa
          // Se não: Erro, operando inválido
        // Se não achou: Procura operação na tabela de diretivas
          // Se achou: chama subrotina que executa a diretiva contador_posição = valor retornado pela subrotina
          // Se não achou: Erro, operação não identificada
        // contador_linha = contador_linha + 1
    }
}


// Funções principais
int main(int argc, char *argv[]) {
  regex regex_asm_file(".*\\.asm$", regex::icase);
  regex regex_pre_file(".*\\.pre$", regex::icase);

  if (argc != 2) {
    cerr << "Forneça o nome do arquivo como argumento. Exemplo: ./montador exemplo1.asm" << endl;
    return 1;
  }

  if (!(regex_match(argv[1], regex_asm_file) || regex_match(argv[1], regex_pre_file))) {
    cerr << "Arquivo inválido. Forneça um arquivo com extensão .asm ou .pre" << endl;
    return 1;
  }

  if (regex_match(argv[1], regex_asm_file)) {
    PreProcessamento(argc, argv);
  }

  if (regex_match(argv[1], regex_pre_file)) {
    PrimeiraPassagem(argc, argv);
    // SegundaPassagem();
  }


  return 0;
}