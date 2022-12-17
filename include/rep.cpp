#include "../lib/rep.h"
#include <iostream>
#include <stdlib.h>
#include "string"
#include <locale>
#include <regex>
#include <sys/stat.h>
#include <fstream>

using namespace std;

Rep::Rep() {}

void Rep::rep(vector<string> tokens, Mount::MountedDisk mounted[]) {
    vector<string> required = { "name", "path", "id" };
    string name;
    string path;
    string id;
    string ruta;

    for (auto token: tokens) {
        string _id = shared.lower(token.substr(0, token.find("=")));
        token.erase(0, _id.length() + 1);
        if (token.substr(0, 1) == "\"") {
            token = token.substr(1, token.length()-2);
        }

        if (shared.compare(_id, "name")) {
            if (count(required.begin(), required.end(), _id)){
                auto itr = find(required.begin(), required.end(), _id);
                required.erase(itr);
                name = token;
            }
        } else if (shared.compare(_id, "path")) {
            if (count(required.begin(), required.end(), _id)) {
                auto itr = find(required.begin(), required.end(), _id);
                required.erase(itr);
                path = token;
            }
        } else if (shared.compare(_id, "id")) {
            if (count(required.begin(), required.end(), _id)) {
                auto itr = find(required.begin(), required.end(), _id);
                required.erase(itr);
                id = token;
            }
        } else if (shared.compare(_id, "ruta")) {
            if (count(required.begin(), required.end(), _id)) {
                auto itr = find(required.begin(), required.end(), _id);
                required.erase(itr);
                ruta = token;
            }
        } else {
            shared.handler("REP", "no se esperaba el parametro " + _id);
            return;
        }
    }
    if (required.size() != 0) {
        shared.handler("REP", "requiere ciertos parámetros obligatorios");
        return;
    }

    if (shared.compare(name, "MBR")) {
        MBR(name, path, id, mounted);
    } else if (shared.compare(name, "disk")) {
        disk(name, path, id, mounted);
    } else if (shared.compare(name, "inode")) {
        cout << endl;
    } else if (shared.compare(name, "Journaling")) {
        cout << endl;
    } else if (shared.compare(name, "block")) {
        cout << endl;
    } else if (shared.compare(name, "bm_inode")) {
        cout << endl;
    } else if (shared.compare(name, "bm_block")) {
        cout << endl;
    } else if (shared.compare(name, "tree")) {
        cout << endl;
    } else if (shared.compare(name, "sb")) {
        cout << endl;
    } else if (shared.compare(name, "file")) {
        cout << endl;
    } else if (shared.compare(name, "sb")) {
        cout << endl;
    } else {
        shared.handler("REP", "El name debe ser MBR, disk, inode, Journaling, block, bm_inode, bm_block, tree, sb, file, ls");
    }
}

void Rep::MBR(string name, string path, string id, Mount::MountedDisk mounted[]) {
    string tk = id;
    string path_disco;
    int number;

    if (regex_match(id, regex("27[1-9][0-9]*[a-z]"))) {
        id.erase(0, 2);
        number = stoi(id.substr(0, id.length()-1));
        bool error = true;
        for (int i = 0; i < 99; i++) {
            if (i == number-1 && mounted[i].status == '1') {
                path_disco = mounted[i].path;
                error = false;
            }
        }
        if (error) {
            shared.handler("REP", "-id=" + tk + " no existe");
            return;
        }
    } else {
        shared.handler("REP", "id no válido");
        return;
    }

    Structs::MBR disco;
    FILE *file = fopen(path_disco.c_str(), "r");
    fseek(file, 0, SEEK_SET);
    fread(&disco, sizeof(Structs::MBR), 1, file);
    fclose(file);

    Structs::_Partition particiones[4];
    particiones[0] = disco.mbr_Partition_1;
    particiones[1] = disco.mbr_Partition_2;
    particiones[2] = disco.mbr_Partition_3;
    particiones[3] = disco.mbr_Partition_4;

    string path_dir = path.substr(0, path.find_last_of("/"));
    const char* dir = path_dir.c_str();
    struct stat sb;

    if (stat(dir, &sb) != 0) {
        string comando_mkdir = "mkdir -p \"" + path_dir + "\"";
        system(comando_mkdir.c_str());
    }

    struct tm *tm;
    tm = localtime(&disco.mbr_fecha_creacion);
    char mostrar_fecha [20];
    strftime(mostrar_fecha, 20, "%Y/%m/%d %H:%M:%S", tm);
    string nombre_disco = path_disco.substr(path_disco.find_last_of("/") + 1);

    string path_dot = path.substr(0, path.find_last_of(".")) + ".dot";
    ofstream fs;
    fs.open(path_dot, ios::out);

    fs << "digraph structs {\nlabelloc=\"t\"\nlabel=\"" + nombre_disco + "\"\n";
    fs << "node [shape=plaintext];\nstruct1 [label=<<TABLE>\n";
    fs << "<TR><TD COLSPAN=\"2\" BGCOLOR=\"gray\">MBR</TD></TR>\n";
    fs << "<TR><TD>mbr_tamano</TD><TD>" + to_string(disco.mbr_tamano) + "</TD></TR>\n";
    fs << "<TR><TD>mbr_fecha_creacion</TD><TD>" + string(mostrar_fecha) + "</TD></TR>\n";
    fs << "<TR><TD>mbr_dsk_signature</TD><TD>" + to_string(disco.mbr_disk_signature) + "</TD></TR>\n";
    fs << "<TR><TD>dsk_fit</TD><TD>" + string(1, disco.disk_fit) + "</TD></TR>\n";

    for (int i = 0; i < 4; i++) {
        fs << "<TR><TD COLSPAN=\"2\" BGCOLOR=\"gray\">mbr_partition_" + to_string(i + 1) + "</TD></TR>\n";

        if (particiones[i].part_status == '1') {
            fs << "<TR><TD>part_status</TD><TD>" + string(1, particiones[i].part_status) + "</TD></TR>\n";
            fs << "<TR><TD>part_type</TD><TD>" + string(1, particiones[i].part_type) + "</TD></TR>\n";
            fs << "<TR><TD>part_fit</TD><TD>" + string(1, particiones[i].part_fit) + "</TD></TR>\n";
            fs << "<TR><TD>part_start</TD><TD>" + to_string(particiones[i].part_start) + "</TD></TR>\n";
            fs << "<TR><TD>part_s</TD><TD>" + to_string(particiones[i].part_size) + "</TD></TR>\n";
            fs << "<TR><TD>part_name</TD><TD>" + string(particiones[i].part_name) + "</TD></TR>\n";
        } else {
            fs << "<TR><TD>part_status</TD><TD>0</TD></TR>\n";
            fs << "<TR><TD>part_type</TD><TD></TD></TR>\n";
            fs << "<TR><TD>part_fit</TD><TD></TD></TR>\n";
            fs << "<TR><TD>part_start</TD><TD>-1</TD></TR>\n";
            fs << "<TR><TD>part_s</TD><TD>0</TD></TR>\n";
            fs << "<TR><TD>part_name</TD><TD></TD></TR>\n";
        }
    }

    fs << "</TABLE>>];\n}";
    fs.close();

    string extension = path.substr(path.find_last_of(".") + 1);
    string comando_dot = "dot -T" + extension + " " + path_dot + " -o" + path;
    system(comando_dot.c_str());
    string comando_rm = "rm " + path_dot;
    system(comando_rm.c_str());
    shared.response("REP", "MBR " + nombre_disco);
}

void Rep::disk(string name, string path, string id, Mount::MountedDisk mounted[]) {
    string tk = id;
    string path_disco;
    int number;

    if (regex_match(id, regex("27[1-9][0-9]*[a-z]"))) {
        id.erase(0, 2);
        number = stoi(id.substr(0, id.length()-1));
        bool error = true;
        for (int i = 0; i < 99; i++) {
            if (i == number-1 && mounted[i].status == '1') {
                path_disco = mounted[i].path;
                error = false;
            }
        }
        if (error) {
            shared.handler("REP", "-id=" + tk + " no existe");
            return;
        }
    } else {
        shared.handler("REP", "id no válido");
        return;
    }

    Structs::MBR disco;
    FILE *file = fopen(path_disco.c_str(), "r");
    fseek(file, 0, SEEK_SET);
    fread(&disco, sizeof(Structs::MBR), 1, file);
    fclose(file);

    Structs::_Partition particiones[4];
    particiones[0] = disco.mbr_Partition_1;
    particiones[1] = disco.mbr_Partition_2;
    particiones[2] = disco.mbr_Partition_3;
    particiones[3] = disco.mbr_Partition_4;

    string path_dir = path.substr(0, path.find_last_of("/"));
    const char* dir = path_dir.c_str();
    struct stat sb;

    if (stat(dir, &sb) != 0) {
        string comando_mkdir = "mkdir -p \"" + path_dir + "\"";
        system(comando_mkdir.c_str());
    }

    struct tm *tm;
    tm = localtime(&disco.mbr_fecha_creacion);
    char mostrar_fecha [20];
    strftime(mostrar_fecha, 20, "%Y/%m/%d %H:%M:%S", tm);
    string nombre_disco = path_disco.substr(path_disco.find_last_of("/") + 1);

    string path_dot = path.substr(0, path.find_last_of(".")) + ".dot";
    ofstream fs;
    fs.open(path_dot, ios::out);

    fs << "digraph structs {\nlabelloc=\"t\"\nlabel=\"" + nombre_disco + "\"\n";
    fs << "node [shape=plaintext];\nstruct1 [label=<<TABLE CELLPADDING=\"15\">\n";
    fs << "<TR>\n";

    double porcentaje = ((double)sizeof(disco) * (double)100) / (double)disco.mbr_tamano;
    fs << "<TD>MBR<BR/>" + to_string(porcentaje) + "%</TD>\n";

    int libre = 0;

    for (int i = 0; i < 4; i++) {
        if (particiones[i].part_status == '1') {
            porcentaje = ((double)particiones[i].part_size * (double)100) / (double)disco.mbr_tamano;
            if (particiones[i].part_type == 'P') {
                fs << "<TD>Primaria<BR/>" + to_string(porcentaje) + "%</TD>\n";
            } else if (particiones[i].part_type == 'E') {
                fs << "<TD>Extendida<BR/>" + to_string(porcentaje) + "%</TD>\n";
            }
            libre = disco.mbr_tamano - (particiones[i].part_start + particiones[i].part_size);
        } else {
            libre = disco.mbr_tamano - (particiones[i - 1].part_start + particiones[i - 1].part_size);
            break;
        }
    }

    if (libre > 0) {
        porcentaje = ((double)libre * (double)100) / (double)disco.mbr_tamano;
        fs << "<TD>Libre<BR/>" + to_string(porcentaje) + "%</TD>\n";
    }

    fs << "</TR>\n";
    fs << "</TABLE>>];\n}";
    fs.close();

    string extension = path.substr(path.find_last_of(".") + 1);
    string comando_dot = "dot -T" + extension + " " + path_dot + " -o" + path;
    system(comando_dot.c_str());
    string comando_rm = "rm " + path_dot;
    system(comando_rm.c_str());
    shared.response("REP", "disk " + nombre_disco);
}
