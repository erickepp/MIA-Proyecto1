#ifndef DISK_H
#define DISK_H

#include<vector>
#include <string>
#include <algorithm>
#include "../lib/disco.h"
#include "../lib/scanner.h"
#include "../lib/structs.h"
#include "../lib/shared.h"

using namespace std;

class Disk {
    public:
        Disk();
        typedef struct _Transition {
            int partition;
            int start;
            int end;
            int before;
            int after;
        } Transition;

        void mkdisk(vector<string> tokens);
        void makeDisk(string s, string f, string u, string p);
    private:
        Shared shared;   
};

#endif