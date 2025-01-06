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

        result = before + ", " + after;
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

    int aux = 0;
    string line, section_text = "", section_data = "";
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
    {"SPACE", 0}, {"CONST", 0}, {"BEGIN", 0}, {"END", 0}  
};

map<string, pair<int, char>> tabelaSimbolos;
map<string, int>tabelaDefinicoes;

void PrimeiraPassagem(int argc, char *argv[]) {
    string arquivo = argv[1];
    ifstream inputFile(arquivo);

    if (!inputFile.is_open()) {
        cerr << "Erro ao abrir arquivo de pré-processamento" << endl;
        return;
    }

    int contador_linha = 1;
    int contador_posicao = 0;
    regex regex_space("\\bSPACE\\b"), regex_const("\\bCONST\\b"), regex_begin("\\bBEGIN\\b"), regex_end("\\bEND\\b"), regex_public("\\bPUBLIC\\b"), regex_extern("\\bEXTERN\\b");

    string line;
    // Enquanto arquivo fonte não chegou ao fim, faça: Obtém uma linha do fonte
    while (getline(inputFile, line)) {
        if (line == "SECTION TEXT" || line == "SECTION DATA") {
            continue;
        }

        cout << "Contador de posição: " << contador_posicao << " Instrução: " << line << endl;
        // Separa os elementos da linha: rótulo, operação, operandos
        string rotulo;
        string operacao;
        string argumento;

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
              tabelaSimbolos[rotulo] = { contador_posicao, 'N' };

              if (instrucao.size() == 2) {
                int valor = stoi(instrucao[1]);
                contador_posicao += valor;
              } else {
                contador_posicao += 1;
              }

              // Se não achou: Insere rótulo e contador_posição na TS
            //   tabelaSimbolos[rotulo] = { tabelaDiretivas["SPACE"], 'N' };
              
              contador_linha++;
              continue;
            }

            if (regex_search(operacao, regex_const)) {
              
              // Se não achou: Insere rótulo e contador_posição na TS
              // tabelaSimbolos[rotulo] = {stoi(instrucao[1]), 'N'};
              tabelaSimbolos[rotulo] = {contador_posicao, 'N'};

              contador_posicao += 1;
              contador_linha++;
              continue;
            }

            if (regex_search(operacao, regex_extern)) {
              tabelaSimbolos[rotulo] = { 0, 'S' };
              continue;
            }

            if (regex_search(operacao, regex_begin)) {
              tabelaSimbolos[rotulo] = { 0, 'N' };
              tabelaDefinicoes[rotulo] = 0;
              continue;
            }

            tabelaSimbolos[rotulo] = {contador_posicao, 'N'};
          }

          contador_posicao += tabelaInstrucoes[operacao].tamanho;
        }
        // Se não existe rótulo:
        else {
          // Procura operação na tabela de instruções
          vector<string> instrucao = split(superTrim(line), " ");
          operacao = instrucao[0];
          if (instrucao.size() > 1) argumento = instrucao[1];

          
          if (tabelaInstrucoes.find(operacao) != tabelaInstrucoes.end()) {
            // Se achou: contador_posição = contador_posição + tamanho da instrução
            contador_posicao += tabelaInstrucoes[operacao].tamanho;
          } else {
            
            if (regex_search(operacao, regex_public)) {
            //   tabelaSimbolos[argumento] = { 0, 'N' };
              tabelaDefinicoes[argumento] = 0;
              continue;
            }

            if (regex_search(operacao, regex_end)) {
              continue;
            }

            cerr << "Linha: " << contador_linha << " " << "ERRO SINTÁTICO: operação " << operacao << " não identificada" << endl;
            return;
          }
        }

      contador_linha++;
    }

    // Imprime a tabela de símbolos
    // Iterar e imprimir os elementos do map
    for (auto const& x : tabelaSimbolos) {
        cout << "Rótulo: " << x.first << ", Pos: " << x.second.first << ", Externo: " << x.second.second << endl;
    }

    cout << "\n\n";

    for (auto const& x : tabelaDefinicoes) {
        cout << x.first << " " << x.second << endl;
    } 
}


void SegundaPassagem(){ 

  int contador_posicao = 0;
  int contador_linha = 1;
  
  string codigo_objeto = "";
  string line;

  ifstream inputFile("myfile.pre");

  if(!inputFile.is_open()){
    cerr << "Erro ao abrir arquivo de pre-processamento" << endl;
    return;
  }

  // Enquanto arquivo fonte não chegou ao fim, faça: Obtém uma linha do código fonte.
  while(getline(inputFile, line)){

    // Se a liha do código fonte for "SECTION TEXT" ou "SECTION DATA", ignore essa linha do código fonte.
    if(line == "SECTION TEXT" || line == "SECTION DATA"){
      continue;
    }
    // Se a liha do código fonte não for "SECTION TEXT" ou "SECTION DATA", continue o algoritmo de segunda passagem.
    else{
      // Separa os elementos da linha: rótulo, operação, operando1 e operando2.
      vector<string> instrucao = split(line, " ");
      string rotulo;
      string operacao;
      string operando1;
      string operando2;

      if(instrucao[0].back() == ':'){
        rotulo = instrucao[0];
        operacao = instrucao[1];
        if(instrucao.size() == 2){
          operando1 = "";
          operando2 = "";
        }
        else if(instrucao.size() == 3){
          operando1 = instrucao[2];
          operando2 = "";
        }
        else if(instrucao.size() == 4){
          operando1 = instrucao[2];
          operando1.pop_back();
          operando2 = instrucao[3];
        }
      }
      else{
        rotulo = "";
        operacao = instrucao[0];
        if(instrucao.size() == 1){
          operando1 = "";
          operando2 = "";
        }
        else if(instrucao.size() == 2){
          operando1 = instrucao[1];
          operando2 = "";
        }
        else if(instrucao.size() == 3){
          operando1 = instrucao[1];
          operando1.pop_back();
          operando2 = instrucao[2];
        }
      }
      // Para cada operando que é símbolo, procura operando na Tabela de Símbolos
      if(operacao != "CONST" && operacao != "SPACE"){
        if(operando1 != ""){
          // Se não achou:
          if(tabelaSimbolos.find(operando1) == tabelaSimbolos.end()){
            cerr << "ERRO: Simbolo Indefinido" << endl;
          }
        }
        // Para cada operando que é símbolo, procura operando na Tabela de Símbolos
        if(operando2 != ""){
          // Se não achou:
          if(tabelaSimbolos.find(operando2) == tabelaSimbolos.end()){
            cerr << "ERRO: Simbolo Indefinido" << endl;
          }
        }
      }
      // Procura operação na Tabela de Instruções
      // Se achou na Tabalea de Instruções:
      if(tabelaInstrucoes.find(operacao) != tabelaInstrucoes.end()){
        contador_posicao = contador_posicao + tabelaInstrucoes[operacao].tamanho;
        codigo_objeto = codigo_objeto + tabelaInstrucoes[operacao].opcode + " ";
        // Se o número e o tipo dos operandos está correto então gera código objeto conforme formato da instrução:
        if(operando1 != ""){
          codigo_objeto = codigo_objeto + to_string(tabelaSimbolos[operando1].first) + " ";
        }
        if(operando2 != ""){
          codigo_objeto = codigo_objeto + to_string(tabelaSimbolos[operando2].first) + " ";
        }
      }
      // Se não achou na Tabela de Instruções:
      // Procura operação na Tabela de Diretivas
      else if(tabelaDiretivas.find(operacao) != tabelaDiretivas.end()){
        // Se achou na Tabela de Diretivas:
        // Chama subrotina que executa a diretiva
        // contador_posicao = valor retornado pela subrotina;
        if(operacao == "CONST"){
          codigo_objeto = codigo_objeto + operando1 + " ";
        }
        else if(operacao == "SPACE"){
          if(operando1 == ""){
            codigo_objeto = codigo_objeto + "00" + " ";
          }
          else if(operando1 != ""){
            for(int i = 0; i < stoi(operando1); i++){
              codigo_objeto = codigo_objeto + "00 ";
            }
          }
        }
      }
      // Se não achou na Tabela de Diretivas:
      else{
        cerr << "ERRO: Operacao Nao Identificada" << endl;
      }
    }
    contador_linha = contador_linha + 1;
  }
  cout << codigo_objeto << endl;
  if(codigo_objeto.back() == ' '){
    codigo_objeto.pop_back();
  }
  cout << codigo_objeto << endl;
  string nomeArquivo = "myfile.obj";
  ofstream file(nomeArquivo);

  if(file.is_open()){
    file << codigo_objeto;
    file.close();
    cout << "\nArquivo " << nomeArquivo << " criado com sucesso!" << endl;
  }
  else {
    cerr << "\nErro ao abrir o arquivo!" << endl;
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
    SegundaPassagem();
  }


  return 0;
}