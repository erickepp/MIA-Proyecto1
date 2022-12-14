#ifndef SCANNER_H
#define SCANNER_H

#include <vector>
#include <string>
using namespace std;

class scanner {
    public:
        scanner();
        void start();
        void functions(string token, vector<string> tks);
        string token(string text);                      
        vector<string> split_tokens(string text);
        bool compare(string a, string b);
        string upper(string a);
        void errores(string operacion, string mensaje);
        void respuesta(string operacion, string mensaje);
        bool confirmar(string mensaje);
        void funcion_exec(vector<string> tokens);
        void exec(string path);
};

#endif
