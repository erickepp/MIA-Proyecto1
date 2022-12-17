#ifndef REP_H
#define REP_H

#include<vector>
#include <string>
#include <algorithm>
#include "../lib/disco.h"
#include "../lib/scanner.h"
#include "../lib/structs.h"
#include "../lib/shared.h"
#include "../lib/mount.h"

using namespace std;

class Rep {
    public:
        Rep();
        void rep(vector<string> tokens, Mount::MountedDisk mounted[]);
        void MBR(string name, string path, string id, Mount::MountedDisk mounted[]);
        void disk(string name, string path, string id, Mount::MountedDisk mounted[]);
    private:
        Shared shared;
};

#endif
