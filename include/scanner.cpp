#include "../lib/scanner.h"
#include "../lib/disco.h"
#include "../lib/mount.h"
#include "../lib/rep.h"
#include <iostream>
#include <stdlib.h>
#include <locale>
#include <cstring>
#include <locale>
#include <fstream>
#include <vector>
#include <cstdlib>
using namespace std;

Disk disco;
Mount mount;
Rep rep;
void print_function(string, vector<string>);

scanner::scanner() { 
}

void scanner::start() {
    system("clear");
    while (true) {
        string texto;
        cout << "\033[1;37m>> \033[0m";
        getline(cin, texto);
        if (compare(texto, "exit")) {
            break;
        }
        string tk = token(texto);
        texto.erase(0, tk.length()+1);
        vector<string> tks = split_tokens(texto);
        functions(tk, tks);
    }
}
    
void scanner::functions(string token, vector<string> tks) {
    if (compare(token, "MKDISK")) {
        disco.mkdisk(tks);
    } else if (compare(token, "RMDISK")) {
        disco.rmdisk(tks);
    } else if (compare(token, "FDISK")) {
        disco.fdisk(tks);
    } else if (compare(token, "MOUNT")) {
        mount.mount(tks);
    } else if (compare(token, "UNMOUNT")) {
        mount.unmount(tks);
    } else if (compare(token, "MKFS")) {
        print_function("MKFS", tks);
    } else if (compare(token, "LOGIN")) {
        print_function("LOGIN", tks);
    } else if (compare(token, "LOGOUT")) {
        print_function("LOGOUT", tks);
    } else if (compare(token, "MKGRP")) {
        print_function("MKGRP", tks);
    } else if (compare(token, "RMDISK")) {
        print_function("RMDISK", tks);
    } else if (compare(token, "RMGRP")) {
        print_function("RMGRP", tks);
    } else if (compare(token, "MKUSR")) {
        print_function("MKUSR", tks);
    } else if (compare(token, "RMUSR")) {
        print_function("RMUSR", tks);
    } else if (compare(token, "CHMOD")) {
        print_function("CHMOD", tks);
    } else if (compare(token, "MKFILE")) {
        print_function("MKFILE", tks);
    } else if (compare(token, "CAT")) {
        print_function("CAT", tks);
    } else if (compare(token, "REMOVE")) {
        print_function("REMOVE", tks);
    } else if (compare(token, "EDIT")) {
        print_function("EDIT", tks);
    } else if (compare(token, "RENAME")) {
        print_function("RENAME", tks);
    } else if (compare(token, "MKDIR")) {
        print_function("MKDIR", tks);
    } else if (compare(token, "COPY")) {
        print_function("COPY", tks);
    } else if (compare(token, "MOVE")) {
        print_function("MOVE", tks);
    } else if (compare(token, "FIND")) {
        print_function("FIND", tks);
    } else if (compare(token, "CHOWN")) {
        print_function("CHOWN", tks);
    } else if (compare(token, "CHGRP")) {
        print_function("CHGRP", tks);
    } else if (compare(token, "RECOVERY")) {
        print_function("RECOVERY", tks);
    } else if (compare(token, "LOSS")) {
        print_function("LOSS", tks);
    } else if (compare(token, "EXEC")) {
        funcion_exec(tks);
    } else if (compare(token, "REP")) {
        rep.rep(tks, mount.mounted);
    } else if(compare(token.substr(0,1), "#")){
        respuesta("COMENTARIO", token);
    } else{
        errores("SYSTEM", "El comando ingresado no se reconoce en el sistema \""+token+"\"");
    }
}

string scanner::token(string text) {
    string tkn = "";
    bool terminar = false;
    for (char &c : text) {
        if (terminar) {
            if (c==' ' || c=='-') {
                break;
            }
            tkn += c;
        }
        else if (c!=' ' && !terminar) {
            if (c == '#') {
                tkn = text;
                break;
            } else {
                tkn += c;
                terminar = true;
            }
        }
    }
    return tkn;
}

vector<string> scanner::split_tokens(string text) {
    vector<string> tokens;
    if (text.empty()) {
        return tokens;
    }
    text.push_back(' ');
    string token = "";
    int estado = 0;
    for(char &c: text){
        if (estado==0 && c=='-') {
            estado = 1;
        } else if(estado==0 && c=='#') {
            continue;
        } else if(estado != 0){
            if (estado == 1) {
                if (c == '=') {
                    estado = 2;
                } else if(c == ' ') {
                    continue;
                }
            }else if(estado == 2){
                if (c == '\"') {
                    estado = 3;
                } else {
                    estado = 4;
                }
            } else if (estado == 3){
                if (c == '\"') {
                    estado = 4;
                }
            } else if (estado==4 && c=='\"') {
                tokens.clear();
                continue;
            } else if (estado==4 && c==' ') {
                estado = 0;
                tokens.push_back(token);
                token = "";
                continue;
            }
            token += c;
        }
    }
    return tokens;
}

string scanner::upper(string a){
    string up = "";
    for(char &a: a) {
        up += toupper(a);
    }
    return up;  
}

bool scanner::compare(string a, string b){
    if (upper(a) == upper(b)) {
        return true;
    }
    return false;
}

void scanner::errores(string operacion, string mensaje) {
    cout << "\033[1;41mError\033[0;31m(" + operacion + ")~~> \033[0m" << mensaje << endl << endl;
}

void scanner::respuesta(string operacion, string mensaje) {
    cout << "\033[1;42m(" + operacion + ")\033[0;32m~~> \033[0m" << mensaje << endl << endl;
}

bool scanner::confirmar(string mensaje){
    cout << mensaje << "Confirmar(S), cualquier otra letra para cancelar" << endl;
    string respuesta;
    getline(cin,respuesta);
    cout << endl;
    if (compare(respuesta, "s")) {
        return true;
    }
    return false;
}

void scanner::funcion_exec(vector<string> tokens) {
    string path = "";
    for (string token: tokens) {
        string tk = token.substr(0, token.find("="));
        token.erase(0, tk.length()+1);
        if (token.substr(0, 1) == "\"") {
            path = token.substr(1, token.length()-2);
        }
        if (compare(tk, "path")) {
            path = token;
        }
    }
    if (path.empty()) {
        errores("EXEC", "Se requiere path para este comando");
        return;
    }
    exec(path);
}

void scanner::exec(string path) {
    if (!compare(path.substr(path.find_last_of(".") + 1), "mia")) {
        errores("EXEC", "El archivo debe ser de tipo .mia");
        return;
    }
    string filename(path);
    vector <string> lines;
    string line;
    ifstream input_file(filename);
    if (!input_file.is_open()) {
        errores("EXEC", "No se puede abrir el archivo " + filename);
        return;
    }
    while (getline(input_file, line)) {
        lines.push_back(line);
    }
    cout << endl;
    for (const auto &i: lines) {
        string texto = i;
        string tk = token(texto);
        if (texto != "") {
            cout << "\033[1;37m>> \033[0m" << texto << endl;
            if(compare(texto, "PAUSE")) {
                string pause;
                cout << "\033[1;42m(PAUSE)\033[0;32m~~> \033[0mPresione enter para continuar...";
                getline(cin,pause);
                cout << endl;
                continue;
            }
            texto.erase(0, tk.length()+1);
            vector <string> tks = split_tokens(texto);
            functions(tk, tks);
        }
    }
    input_file.close();
    return;
}

void print_function(string token, vector<string> tks) {
    cout << "FUNCION " << token << endl;
        cout << "Instrucciones: " << endl;
        for (int i = 0; i < tks.size(); i++) {
            cout << tks[i] << endl;
    }
    cout << endl;
}
