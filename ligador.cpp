#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <iomanip>
#include <cctype>

using namespace std;

void printTable(const map<string, int>& tabelaDefinicoes = {}, const vector<pair<string, int>>& tabelaUsoA = {}, const vector<pair<string, int>>& tabelaUsoB = {}){
  if(!tabelaDefinicoes.empty()){
    cout << "Tabela de Definicoes" << endl;
    cout << "+----------------+--------+" << endl;
    cout << "| Sim            | vlr    |" << endl;
    cout << "+----------------+--------+" << endl;
    for(const auto& item : tabelaDefinicoes){
      cout << "| " << setw(14) << left << item.first << " | " << setw(6) << item.second << " |" << endl;
    }
    cout << "+----------------+--------+" << endl;
    cout << endl;
  }
  if(!tabelaUsoA.empty()){
    cout << "Tabela de Uso" << endl;
    cout << "+----------------+----------+" << endl;
    cout << "| Sim            | endereco |" << endl;
    cout << "+----------------+----------+" << endl;
    for(const auto& item : tabelaUsoA){
      cout << "| " << setw(14) << left << item.first << " | " << setw(8) << item.second << " |" << endl;
    }
    cout << "+----------------+----------+" << endl;
    cout << endl;
  }
  if(!tabelaUsoB.empty()){
    cout << "Tabela de Uso" << endl;
    cout << "+----------------+----------+" << endl;
    cout << "| Sim            | endereco |" << endl;
    cout << "+----------------+----------+" << endl;
    for(const auto& item : tabelaUsoB){
      cout << "| " << setw(14) << left << item.first << " | " << setw(8) << item.second << " |" << endl;
    }
    cout << "+----------------+----------+" << endl;
    cout << endl;
  }
}

vector<string> split(string s, string delimiter){
  size_t pos_start = 0, pos_end, delim_len = delimiter.length();
  string token;
  vector<string> res;
  while((pos_end = s.find(delimiter, pos_start)) != string::npos){
    token = s.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delim_len;
    res.push_back(token);
  }
  res.push_back(s.substr(pos_start));
  return res;
}

void Ligacao(const string& arquivo1, const string& arquivo2){
  map<string, int> tabelaDefinicoesGlobal;
  vector<pair<string, int>> tabelaUsoA;
  vector<pair<string, int>> tabelaUsoB;
  string codigo_objetoA = "";
  string codigo_objetoB = "";
  string codigo_objeto = "";
  string line;
  ifstream file1(arquivo1);
  ifstream file2(arquivo2);
  regex number_regex("\\b\\d+\\b");
  int fatorCorrecao = 0;

  if(!file1.is_open()){
    cerr << "Erro ao abrir o arquivo 1" << endl;
    return;
  }

  while(getline(file1, line)){
    if(isdigit(line[0])){
      auto words_begin = sregex_iterator(line.begin(), line.end(), number_regex);
      auto words_end = sregex_iterator();
      fatorCorrecao += distance(words_begin, words_end);
      codigo_objetoA = line;
    }
    else if(line[0] == 'D'){
      vector<string> definicao = split(line, " ");
      tabelaDefinicoesGlobal[definicao[1]] = stoi(definicao[2]);
    }
    else if(line[0] == 'U'){
      vector<string> uso = split(line, " ");
      tabelaUsoA.push_back(make_pair(uso[1], stoi(uso[2])));
    }
  }

  file1.close();

  if(!file2.is_open()){
    cerr << "Erro ao abrir o arquivo 2" << endl;
    return;
  }

  while(getline(file2, line)){
    if(isdigit(line[0])){
      codigo_objetoB = line;
    }
    else if(line[0] == 'D'){
      vector<string> definicao = split(line, " ");
      tabelaDefinicoesGlobal[definicao[1]] = stoi(definicao[2]) + fatorCorrecao;
    }
    else if(line[0] == 'U'){
      vector<string> uso = split(line, " ");
      tabelaUsoB.push_back(make_pair(uso[1], stoi(uso[2]) + fatorCorrecao));
    }
  }

  file2.close();

  vector<string> codigo_objetoAB = split(codigo_objetoA + " " + codigo_objetoB, " ");

  for(const auto& item : tabelaUsoA){
    codigo_objetoAB[item.second] = to_string(stoi(codigo_objetoAB[item.second]) + tabelaDefinicoesGlobal[item.first]);
  }

  for(const auto& item : tabelaUsoB){
    codigo_objetoAB[item.second] = to_string(tabelaDefinicoesGlobal[item.first]);
  }

  cout << "Codigo Objeto" << endl;
  for(int i = 0; i < codigo_objetoAB.size(); i++){
    cout << codigo_objetoAB[i] << " ";
  }
  cout << endl;
  cout << endl;

  printTable(tabelaDefinicoesGlobal, tabelaUsoA, tabelaUsoB);
}

int main(int argc, char *argv[]){
  regex regex_obj_file(".*\\.obj$", regex::icase);

  if(argc != 3){
    cerr << "Forneca o nome dos arquivos como argumento. Exemplo: ./ligador prog1.obj prog2.obj" << endl;
    return 1;
  }

  if(!(regex_match(argv[1], regex_obj_file)) || !(regex_match(argv[2], regex_obj_file))){
    cerr << "Arquivo invalido. Forneca um arquivo com extensao .obj" << endl;
    return 1;
  }

  if(regex_match(argv[1], regex_obj_file)){
    Ligacao(argv[1], argv[2]);
  }

  return 0;
}