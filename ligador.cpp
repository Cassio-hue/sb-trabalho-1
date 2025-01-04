#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <map>

using namespace std;

int main(int argc, char *argv[]) {
  regex regex_obj_file(".*\\.obj$", regex::icase);

  if (argc != 3) {
    cerr << "Forneça o nome dos arquivos como argumento. Exemplo: ./ligador prog1.obj prog2.obj" << endl;
    return 1;
  }

  if (!(regex_match(argv[1], regex_obj_file)) || (regex_match(argv[2], regex_obj_file))) {
    cerr << "Arquivo inválido. Forneça um arquivo com extensão .obj" << endl;
    return 1;
  }


  return 0;
}