#include "../lib/disco.h"
#include "../lib/structs.h"
#include "../lib/scanner.h"
#include <vector>
#include <iostream>
#include <stdlib.h>
#include <string>
#include <algorithm>

using namespace std;

scanner scan;

Disk::Disk() {
}

void Disk::mkdisk(vector<string> tokens) {
    string size = "";
    string u = "";
    string path = "";
    string f = "";
    bool error = false;
    for(string token: tokens){
        string tk = token.substr(0, token.find("="));
        token.erase(0, tk.length()+1);
        if (scan.compare(tk, "f")) {
            if(f.empty()) {
                f = token;
            } else {
                scan.errores("MKDISK", "El parametro F ya fue ingresado en el comando" + tk);
            }
        } else if(scan.compare(tk, "s")) {
            if (size.empty()) {
                size = token;
            } else{
                scan.errores("MKDISK", "parametro SIZE repetido en el comando" + tk);
            }
        } else if (scan.compare(tk, "u")) {
            if (u.empty()) {
                u = token;
            } else{
                scan.errores("MKDISK", "parametro U repetido en el comando" + tk);
            }
        } else if (scan.compare(tk, "path")) {
            if (path.empty()) {
                path = token;
            } else{
                scan.errores("MKDISK", "parametro PATH repetido en el comando" + tk);
            }    
        } else {
            scan.errores("MKDISK", "no se esperaba el parametro " + tk);
            error = true;
            break;
        }
    }
    if (error) {
        return;
    }

    if (f.empty()) {
        f = "BF";
    }
    if (u.empty()) {
        u = "M";
    }

    if (path.empty() && size.empty()) {
        scan.errores("MKDISK", "se requiere parametro Path y Size para este comando");
    } else if(path.empty()) {
        scan.errores("MKDISK", "se requiere parametro Path para este comando");
    } else if (size.empty()) {
        scan.errores("MKDISK", "se requiere parametro Size para este comando");
    } else if (!scan.compare(f, "BF") && !scan.compare(f, "FF") && !scan.compare(f, "WF")) {
        scan.errores("MKDISK", "valores de parametro F no esperados");
    } else if (!scan.compare(u, "k") && !scan.compare(u, "m")) {
        scan.errores("MKDISK", "valores de parametro U no esperados");
    }else{
        makeDisk(size, f, u, path);
    }  
}

void Disk::makeDisk(string s, string f, string u, string path) {
    Structs::MBR disco; 
    try {
        int size = stoi(s);
        if (size <= 0) {
            scan.errores("MKDISK", "Size debe ser mayor a 0");
            return;
        }
        if (scan.compare(u, "M")) {
            size = size * 1024 * 1024;
        }
        if (scan.compare(u, "K")) {
            size = size * 1024;
        }
        f = f.substr(0, 1);
        disco.mbr_tamano = size;
        disco.mbr_fecha_creacion = time(nullptr);
        disco.disk_fit = toupper(f[0]);
        disco.mbr_disk_signature = rand() % 9999 + 100;

        string path2 = path;
        if (path.substr(0, 1) == "\"") {
            path2 = path.substr(1, path.length()-2);
        }

        FILE *file = fopen(path2.c_str(), "r");
        if (file != NULL) {
            scan.errores("MKDISK", "El disco ya existe");
            fclose(file);
            return;
        }

        // disco.mbr_Partition_1 = Structs::Partition();
        // disco.mbr_Partition_2 = Structs::Partition();
        // disco.mbr_Partition_3 = Structs::Partition();
        // disco.mbr_Partition_4 = Structs::Partition();

        if (!scan.compare(path2.substr(path2.find_last_of(".") + 1), "dsk")) {
            scan.errores("MKDISK", "El disco debe ser de tipo .dsk");
            return;
        }

        try {
            FILE *file = fopen(path2.c_str(), "w+b");
            if (file != NULL) {
                fwrite("\0", 1, 1, file);
                fseek(file, size-1, SEEK_SET);
                fwrite("\0", 1, 1, file);
                rewind(file);
                fwrite(&disco, sizeof(Structs::MBR), 1, file);
                fclose(file);
            } else {
                // string comando1 = "mkdir -p \"" + path + "\"";
                string comando1 = "mkdir -p \"" + path2.substr(0, path2.find_last_of("/")) + "\"";
                // string comando2 = "rmdir \"" + path + "\"";
                system(comando1.c_str());
                // system(comando2.c_str());
                FILE *file = fopen(path2.c_str(), "w+b");
                fwrite("\0", 1, 1, file);
                fseek(file, size-1, SEEK_SET);
                fwrite("\0", 1, 1, file);
                rewind(file);
                fwrite(&disco, sizeof(Structs::MBR), 1, file);
                fclose(file);
            }

            FILE *imprimir = fopen(path2.c_str(), "r");
            if (imprimir != NULL) {
                Structs::MBR discoI;
                fseek(imprimir, 0, SEEK_SET);
                fread(&discoI, sizeof(Structs::MBR), 1, imprimir);
                struct tm *tm;
                tm = localtime(&discoI.mbr_fecha_creacion);
                char mostrar_fecha [20];
                strftime(mostrar_fecha, 20, "%Y/%m/%d %H:%M:%S", tm);                
                scan.respuesta("MKDISK", "Disco creado exitosamente");
                cout << "Size:  " << discoI.mbr_tamano << endl;
                cout << "Fecha:  " << mostrar_fecha << endl;
                cout << "Fit:  " << discoI.disk_fit << endl;
                cout << "Disk_Signature:  " << discoI.mbr_disk_signature << endl;
                cout << "Bytes del MBR:  " << sizeof(Structs::MBR) << endl;
                cout << "Path:  " << path2 << endl << endl;
            }
            fclose(imprimir);

        }catch(const exception& e){
            scan.errores("MKDISK", "Error al crear el disco");
        }
    }catch(invalid_argument &e){
        scan.errores("MKDISK", "Size debe ser un entero");
    }
}
