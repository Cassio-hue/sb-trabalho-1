#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <map>
#include <iomanip>

int NUM_EXEMPLOS = 11;
// int NUM_LIGACOES = 10;

using namespace std;

int main() {
    for (int i = 1; i < NUM_EXEMPLOS; i++) {
        string arquivo = "exemplo" + to_string(i) + ".obj";
        string arquivo_gab = "exemplo" + to_string(i) + "_gab" + ".obj";

        ifstream inputFile(arquivo);
        ifstream inputFile_gab(arquivo_gab);

        if (!inputFile.is_open()) {
            cerr << "Erro ao abrir arquivos" << endl;
            return 0;
        }

        string ex;
        string gab;
        string line;

        while (getline(inputFile, line)) {
            ex += line;
        }

        while (getline(inputFile_gab, line)) {
            gab += line;
        }
        
        cout << "Exemplo " << i << endl;
        cout << "Exemplo : " << ex << endl;  
        cout << "Gabarito: " << gab << endl;

        if (ex != gab) {
            cout << "ERRO: exemplo " << i << endl;
        } else {
            cout << "OK: exemplo " << i << endl;
        }

        cout << "\n\n";
    }

    return 0;
}