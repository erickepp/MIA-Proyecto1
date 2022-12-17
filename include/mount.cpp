#include "../lib/mount.h"
#include <iostream>
#include <stdlib.h>
#include "string"
#include <locale>
#include <regex>

using namespace std;

Mount::Mount() {}

void Mount::mount(vector<string> command) {
    vector<string> required = { "path", "name" };
    string path;
    string name;

    for (auto current: command) {
        string id = shared.lower(current.substr(0, current.find("=")));
        current.erase(0, id.length() + 1);
        if (current.substr(0, 1) == "\"") {
            current = current.substr(1, current.length()-2);
        }

        if (shared.compare(id, "name")) {
            if (count(required.begin(), required.end(), id)){
                auto itr = find(required.begin(), required.end(), id);
                required.erase(itr);
                name = current;
            }
        } else if (shared.compare(id, "path")) {
            if (count(required.begin(), required.end(), id)) {
                auto itr = find(required.begin(), required.end(), id);
                required.erase(itr);
                path = current;
            }
        }
    }
    if (required.size() != 0) {
        shared.handler("MOUNT", "requiere ciertos parámetros obligatorios");
        return;
    }
    mount(path, name);
}

void Mount::mount(string p, string n) {
    try {
        FILE *validate = fopen(p.c_str(), "r");
        if (validate == NULL) {
            shared.handler("MOUNT", "Disco no existe");
            return;
        }
        Structs::MBR disk;
        rewind(validate);
        fread(&disk, sizeof(Structs::MBR), 1, validate);
        fclose(validate);
        
        Structs::Partition partition = dsk.findby(disk, n, p);
        if (partition.part_type == 'E') {
            vector<Structs::EBR> ebrs = dsk.getlogics(partition, p);
            if (!ebrs.empty()) {
                Structs::EBR ebr = ebrs.at(0);
                n = ebr.part_name;
                //shared.handler("", "se montará una partición lógica");
            } else {
                throw runtime_error("No se puede montar una extendida");
            }
        }

        for (int i = 0; i < 99; i++) {
            if (mounted[i].path == p) {
                for (int j = 0; j < 26; j++) {
                    MountedPartition partition = mounted[i].mpartitions[j];
                    if (partition.status == '1' && partition.name == n) {
                        shared.handler("MOUNT", "la partición " + string(partition.name) + " ya está montada");
                        return;
                    }
                }

                for (int j = 0; j < 26; j++) {
                    if (Mount::mounted[i].mpartitions[j].status == '0') {
                        mounted[i].status = '1';
                        mounted[i].mpartitions[j].status = '1';
                        mounted[i].mpartitions[j].letter = alfabeto.at(j);
                        strcpy(mounted[i].mpartitions[j].name, n.c_str());
                        string re = to_string(i + 1) + alfabeto.at(j);
                        shared.response("MOUNT", "se ha realizado correctamente el mount -id=27" + re);
                        listmount();
                        return;
                    }
                }
            }
        }

        for (int i = 0; i < 99; i++) {
            if (mounted[i].status == '0') {
                mounted[i].status = '1';
                strcpy(mounted[i].path, p.c_str());
                for (int j = 0; j < 26; j++) {
                    if (Mount::mounted[i].mpartitions[j].status == '0') {
                        mounted[i].mpartitions[j].status = '1';
                        mounted[i].mpartitions[j].letter = alfabeto.at(j);
                        strcpy(mounted[i].mpartitions[j].name, n.c_str());
                        string re = to_string(i + 1) + alfabeto.at(j);
                        shared.response("MOUNT", "se ha realizado correctamente el mount -id=27" + re);
                        listmount();
                        return;
                    }
                }
            }
        }

    } catch(exception &e) {
        shared.handler("MOUNT", e.what());
        return;
    }
}

void Mount::unmount(vector<string> command) {
    string id = "";
    bool error = false;
    for(string current: command){
        string tk = current.substr(0, current.find("="));
        current.erase(0, tk.length()+1);
        if (shared.compare(tk, "id")) {
            if (id.empty()) {
                id = current;
            } else{
                shared.handler("UNMOUNT", "parametro id repetido");
                error = true;
                break;
            }    
        } else {
            shared.handler("UNMOUNT", "no se esperaba el parametro " + tk);
            error = true;
            break;
        }
    }
    if (error) {
        return;
    } else if(id.empty()) {
        shared.handler("UNMOUNT", "se requiere parametro id para este comando");
        return;
    }

    unmount(id);
}

void Mount::unmount(string id) {
    string tk = id;
    char letter;
    int number;

    if (regex_match(id, regex("27[1-9][0-9]*[a-z]"))) {
        id.erase(0, 2);
        number = stoi(id.substr(0, id.length()-1));
        id.erase(0, id.length()-1);
        letter = id[0];
    } else {
        shared.handler("UNMOUNT", "id no válido");
        return;
    }

    for (int i = 0; i < 99; i++) {
        if (i == number-1 && mounted[i].status == '1') {
            for (int j = 0; j < 26; j++) {
                if (letter == mounted[i].mpartitions[j].letter && mounted[i].mpartitions[j].status == '1') {
                    mounted[i].mpartitions[j].status = '0';
                    shared.response("UNMOUNT", "se ha realizado correctamente el unmount -id=" + tk);
                    
                    char status = '0';
                    for (int k = 0; k < 26; k++) {
                        if (mounted[i].mpartitions[k].status == '1') {
                            status = '1';
                            break;
                        }
                    }

                    if (status == '0') {
                        mounted[i].status = '0';
                    }
                    
                    return;
                }
            }
        }
    }

    shared.handler("UNMOUNT", "-id=" + tk + " no existe");
}

void Mount::listmount() {
    cout << "<-------------------------- MOUNTS -------------------------->" << endl;
    for (int i = 0; i < 99; i++) {
        for (int j = 0; j < 26; j++) {
            if (mounted[i].mpartitions[j].status == '1') {
                cout << "> 27" << i + 1 << alfabeto.at(j) << ", " << mounted[i].mpartitions[j].name << endl;
            }
        }
    }
    cout << endl;
}
