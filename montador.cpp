#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <iomanip>

using namespace std;
bool PRINT_DEBUG = true;
bool PRINT_FILE_STATUS = true;

void printTable(
    const map<string, pair<int, char>>& tabelaSimbolos = {}, 
    const map<string, int>& tabelaDefinicoes = {}, 
    const map<string, string>& macroMap = {},
    const vector<pair<string, int>>& tabelaUso = {}
    ) {
    // Imprimindo a tabela de símbolos
    if (!tabelaSimbolos.empty()) {
    cout << '\n';

        cout << "Tabela de Simbolos" << endl;
        cout << "+----------------+--------+---------+\n";
        cout << "| Sim            | vlr    | externo |\n";
        cout << "+----------------+--------+---------+\n";

        for (const auto& x : tabelaSimbolos) {
            cout << "| " 
                      << setw(14) << left << x.first << " | "
                      << setw(6) << x.second.first << " | "
                      << setw(7) << x.second.second << " |\n";
        }

        cout << "+----------------+--------+---------+\n\n";
    }

    // Imprimindo a tabela de definições
    if (!tabelaDefinicoes.empty()) {

        cout << "Tabela de Definicoes" << endl;
        cout << "+----------------+--------+\n";
        cout << "| Sim            | vlr    |\n";
        cout << "+----------------+--------+\n";

        for (const auto& x : tabelaDefinicoes) {
            cout << "| "
                      << setw(14) << left << x.first << " | "
                      << setw(6) << x.second << " |\n";
        }

        cout << "+----------------+--------+\n\n";
    }

    // Imprimindo a tabela de macros
    if (!macroMap.empty()) {
      cout << "+----------------+-------------------------------+\n";
      cout << "| Macro          | Valor                         |\n";
      cout << "+----------------+-------------------------------+\n";

      for (const auto& x : macroMap) {
        string macroName = x.first;
        string macroValue = x.second;

        stringstream ss(macroValue);
        string line;
        bool isFirstLine = true;

        while (getline(ss, line)) {
            if (isFirstLine) {
                // Primeira linha: imprime o macroName
                cout << "| " << setw(14) << left << macroName << " | "
                     << setw(30) << left << line << " |\n";
                isFirstLine = false;
            } else {
                // Linhas subsequentes: deixa a coluna Macro vazia
                cout << "| " << setw(14) << " " << " | "
                     << setw(30) << left << line << " |\n";
            }
        }
    }
    cout << "+----------------+-------------------------------+\n\n";
    }

    // Imprimindo a tabela de uso
    if(!tabelaUso.empty()){
      cout << "Tabela de Uso" << endl;
      cout << "+----------------+----------+\n";
      cout << "| Sim            | endereco |\n";
      cout << "+----------------+----------+\n";
      for (const auto& item : tabelaUso) {
        cout << "| " << setw(14) << left << item.first << " | "
             << setw(8) << item.second << " |\n";
      }
      cout << "+----------------+----------+\n";
    }
    cout << endl;
}

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
    size_t virgula = result.find(',');
    size_t mais = result.find('+');

    if(mais != string::npos) {
      string before = result.substr(0, mais);
      string after = result.substr(mais + 1);

      before = trim(before);
      after = trim(after);

      result = before + "+" + after;
    }

    if (virgula != string::npos) {
        string before = result.substr(0, virgula);
        string after = result.substr(virgula + 1);

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
        return;
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
            if (v[0].back() == '\n') {
                v[0].pop_back();
            }
            
            if (macroMap.find(v[0]) != macroMap.end()) {
              v[0] = macroMap[v[0]];
            };

            if (v[0].back() == '\n') {
                v[0].pop_back();
            }
            
            macroMap[macro_flag] += v[0] + "\n";
        }
    }
    
    inputFile.close();

    if (section_data.back() == '\n') {
        section_data.pop_back();
    }

    if (PRINT_DEBUG) {
      cout << section_text << section_data << endl;
      printTable({}, {}, macroMap, {});
    }

    string nomeArquivo = arquivo.substr(0, arquivo.find_last_of('.')) + ".pre";
    ofstream file(nomeArquivo);

    if (file.is_open()) {
        file << section_text << section_data;
        file.close();
        if (PRINT_FILE_STATUS) cout << "Arquivo " << nomeArquivo << " criado com sucesso!" << endl;
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
    {"SECTION", 1}, {"SPACE", 0}, {"CONST", 0}, {"MACRO", 0}, {"ENDMACRO", 0}, {"BEGIN", 0}, {"END", 0}  
};

map<string, pair<int, char>> tabelaSimbolos;
map<string, int> tabelaDefinicoes;
vector<pair<string, int>> tabelaUso;
string informacaoRealocacao;

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

        if (PRINT_DEBUG) {
            cout << "Contador de linha: " << contador_linha << " Instrucao: " << line << endl;
        }

        // Separa os elementos da linha: rótulo, operação, operandos
        string rotulo;
        string operacao;
        string argumento;

        //Se existe rótulo:
        if(line.find(":") != string::npos) {
          vector<string> separacao_instrucao = split(line, ":");
          vector<string> instrucao = split(superTrim(separacao_instrucao[1]), " ");
          
          rotulo = separacao_instrucao[0];

          // Veririca se o rótulo não é vazio ou composto por espaços em branco.
          if (rotulo.empty()) {
            cerr << "Linha: " << contador_linha << " " << "ERRO SINTATICO: Rotulo " << rotulo << " invalido" << endl;
            return;
          }

          // Verifica se o primeiro caractere é uma letra ou underscore.
          if (!isalpha(rotulo[0]) && rotulo[0] != '_') {
            cerr << "Linha: " << contador_linha << " " << "ERRO SINTATICO: Rotulo " << rotulo << " invalido" << endl;
            return;
          }

          // Procura se existe algum caractere que não seja letra, numero ou underscore.
          for (int i = 1; i < rotulo.size(); i++) {
            if (!isalnum(rotulo[i]) && rotulo[i] != '_') {
              cerr << "Linha: " << contador_linha << " " << "ERRO SINTATICO: Rotulo " << rotulo << " invalido" << endl;
              return;
            }
          }

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

            // if (regex_search(operacao, regex_begin)) {
            //   tabelaSimbolos[rotulo] = { 0, 'N' };
            //   tabelaDefinicoes[rotulo] = 0;
            //   continue;
            // }

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

    for (const auto& x : tabelaDefinicoes) {
      tabelaDefinicoes[x.first] = tabelaSimbolos[x.first].first;
    }

    // Imprime a tabela de símbolos
    // Iterar e imprimir os elementos do map
    if (PRINT_DEBUG) {
        printTable(tabelaSimbolos, tabelaDefinicoes, macroMap, {}); 
    }
}


void SegundaPassagem(char *argv[]){
  bool ligacao = false;
  string arquivo = argv[1];
  string nomeArquivo = arquivo.substr(0, arquivo.find_last_of('.')) + ".pre";

  int contador_posicao = 0;
  int contador_linha = 1;
  
  string codigo_objeto = "";
  string line;

  ifstream inputFile(nomeArquivo);

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
      string operandoPuro1;
      string operandoPuro2;
      int operandoPuro1Valor = 0;
      int operandoPuro2Valor = 0;

      if(instrucao[0].back() == ':'){
        rotulo = instrucao[0];
        operacao = instrucao[1];
        if(instrucao.size() == 2){
          operando1 = "";
          operando2 = "";
        }
        else if(instrucao.size() == 3){
          if(operacao == "COPY"){
            vector<string> operandos = split(instrucao[2], ",");
            operando1 = operandos[0];
            operando2 = operandos[1];
          }
          else{
            operando1 = instrucao[2];
            operando2 = "";
          }	
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
          if(operacao == "COPY"){
            vector<string> operandos = split(instrucao[1], ",");
            operando1 = operandos[0];
            operando2 = operandos[1];
          }
          else{
            operando1 = instrucao[1];
            operando2 = "";
          }
        }
      }
      if(operacao != "CONST" && operacao != "SPACE"){
        if(operando1 != ""){
          if (operando1.find("+") != string::npos) {
            vector<string> split_operando = split(operando1, "+");
            operandoPuro1 = split_operando[0];
            operandoPuro1Valor = stoi(split_operando[1]);
            if(tabelaSimbolos[operandoPuro1].second == 'S'){
              int endereco = contador_posicao + 1;
              tabelaUso.push_back(make_pair(operandoPuro1, endereco));
            }
            if(tabelaSimbolos.find(operandoPuro1) == tabelaSimbolos.end()){
              cerr << "Linha: " << contador_linha << " " << "ERRO: Simbolo Indefinido" << endl;
            }
          } 
          else {
            operandoPuro1 = operando1;
            if(tabelaSimbolos[operando1].second == 'S'){
              int endereco = contador_posicao + 1;
              tabelaUso.push_back(make_pair(operando1, endereco));
            }
            if(tabelaSimbolos.find(operando1) == tabelaSimbolos.end()){
              cerr << "Linha: " << contador_linha << " " << "ERRO: Simbolo Indefinido" << endl;
            }
          }
        }
        if(operando2 != "") {
          if (operando2.find("+") != string::npos) {
            vector<string> split_operando = split(operando2, "+");
            operandoPuro2 = split_operando[0];
            operandoPuro2Valor = stoi(split_operando[1]);
            if(tabelaSimbolos[operandoPuro2].second == 'S'){
              int endereco = contador_posicao + 1;
              tabelaUso.push_back(make_pair(operandoPuro2, endereco));
            }
            if (tabelaSimbolos.find(operandoPuro2) == tabelaSimbolos.end()){
              cerr << "Linha: " << contador_linha << " " << "ERRO: Simbolo Indefinido" << endl;
            }
          } 
          else {
            operandoPuro2 = operando2;
            if(tabelaSimbolos[operando2].second == 'S'){
              int endereco = contador_posicao + 2;
              tabelaUso.push_back(make_pair(operando2, endereco));
            }
            if (tabelaSimbolos.find(operando2) == tabelaSimbolos.end()){
              cerr << "Linha: " << contador_linha << " " << "ERRO: Simbolo Indefinido" << endl;
            }
          }
        }
      }
      // Procura operação na Tabela de Instruções
      // Se achou na Tabalea de Instruções:
      if(tabelaInstrucoes.find(operacao) != tabelaInstrucoes.end()){
        contador_posicao = contador_posicao + tabelaInstrucoes[operacao].tamanho;
        codigo_objeto = codigo_objeto + tabelaInstrucoes[operacao].opcode + " ";
        if(operacao != "BEGIN"){
          informacaoRealocacao+="0 ";
        }
        // Se o número e o tipo dos operandos está correto então gera código objeto conforme formato da instrução:
        if(operando1 != ""){
          codigo_objeto = codigo_objeto + to_string(tabelaSimbolos[operandoPuro1].first + operandoPuro1Valor) + " ";
          if(tabelaDefinicoes.find(operandoPuro1) != tabelaDefinicoes.end()){
            informacaoRealocacao+="1 ";
          }
          else{
            if(tabelaSimbolos.find(operandoPuro1) != tabelaSimbolos.end()){
              if(tabelaSimbolos[operandoPuro1].second == 'N'){
                informacaoRealocacao+="1 ";
              }
              else{
                informacaoRealocacao+="0 ";
              }
            }
          }
        }
        if(operando2 != ""){
          codigo_objeto = codigo_objeto + to_string(tabelaSimbolos[operandoPuro2].first + operandoPuro2Valor) + " ";
          if(tabelaDefinicoes.find(operandoPuro2) != tabelaDefinicoes.end()){
            informacaoRealocacao+="1 ";
          }
          else{
            if(tabelaSimbolos.find(operandoPuro2) != tabelaSimbolos.end()){
              if(tabelaSimbolos[operandoPuro2].second == 'N'){
                informacaoRealocacao+="1 ";
              }
              else{
                informacaoRealocacao+="0 ";
              }
            }
          }
        }
      }
      // Se não achou na Tabela de Instruções:
      // Procura operação na Tabela de Diretivas
      else if(tabelaDiretivas.find(operacao) != tabelaDiretivas.end()){
        // Se achou na Tabela de Diretivas:
        // Chama subrotina que executa a diretiva
        // contador_posicao = valor retornado pela subrotina;
        if(operacao == "CONST"){
          informacaoRealocacao+="0 ";
          regex hex_regex("^-?0X[0-9A-Fa-f]+$");
          int num;
          if (regex_match(operando1, hex_regex)) {
            num = stoi(operando1, nullptr, 16); 
          } 
          else {
            num = stoi(operando1);
          }
          codigo_objeto = codigo_objeto + to_string(num) + " ";
        }
        else if(operacao == "SPACE") {
          if(operando1 == "") {
            codigo_objeto = codigo_objeto + "00" + " ";
          }
          else if(operando1 != "") {
            for(int i = 0; i < stoi(operando1); i++){
              codigo_objeto = codigo_objeto + "00 ";
              informacaoRealocacao+="0 ";
            }
          }
        }
      }
      else if (operacao == "BEGIN" || operacao == "END" || operacao == "EXTERN" || operacao == "PUBLIC"){
        ligacao = true;
        continue;
      }
      // Se não achou na Tabela de Diretivas:
      else {
        cerr << "ERRO: Operacao Nao Identificada" << endl;
      }
    }
    contador_linha = contador_linha + 1;
  }
  if(codigo_objeto.back() == ' '){
    codigo_objeto.pop_back();
  }
  if(codigo_objeto.front() == ' '){
    codigo_objeto.erase(0, 1);
  }

  if (PRINT_DEBUG) {
    printTable({}, {}, {}, tabelaUso);
    cout << "Codigo Objeto" << endl;
    cout << codigo_objeto << endl;
    cout << endl;
  }

  nomeArquivo = arquivo.substr(0, arquivo.find_last_of('.')) + ".obj";
  ofstream file(nomeArquivo);

  if(file.is_open()){

    if(ligacao){
      // Imprimindoa tabela de relativos e absolutos;
      if (!informacaoRealocacao.empty()) {
        cout << "Tabela de Relativos e Absolutos" << endl;
        for(int i = 0; i < informacaoRealocacao.size(); i++){
          cout << informacaoRealocacao[i];
        }
        cout << endl;
      }
      for (const auto& x : tabelaDefinicoes){
        file << "D, " << x.first << " " << x.second << endl;
      }
      for (const auto& x : tabelaUso){
        file << "U, " << x.first << " " << x.second << endl;
      }
      file << "R, " << informacaoRealocacao << endl;
    }

    file << codigo_objeto;
    file.close();

    if (PRINT_FILE_STATUS){
      cout << "Arquivo " << nomeArquivo << " criado com sucesso!" << endl;
    }
  }
  else {
    cerr << "Erro ao abrir o arquivo!" << endl;
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
    SegundaPassagem(argv);
  }


  return 0;
}