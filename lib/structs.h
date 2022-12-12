#ifndef STRUCTS_H
#define STRUCTS_H

#include <string>
#include <ctime>

using namespace std;

class Structs {
public:
    Structs();

    typedef struct _Partition {
        char part_status = '0'; // 0 no usado. 1 ya esta en uso
        char part_type;  // P, E, L
        char part_fit; // configuracion wf, bf, ff
        int part_start = -1; //
        int part_size = 0; //
        char part_name[16]; // nombre maximo de 16 char
    } Partition; // Pesa 27 bytes

    typedef struct _MBR {
        int mbr_tamano; // 4 bytes
        time_t mbr_fecha_creacion; // 24 bytes
        int mbr_disk_signature; // 4 bytes
        char disk_fit; // 1 byte
        Partition mbr_Partition_1; // {status, type, fit, start, size, name} // 27 bytes
        Partition mbr_Partition_2; // 27 bytes
        Partition mbr_Partition_3; // 27 bytes
        Partition mbr_Partition_4;// n logicas // 27 bytes
    } MBR; // Pesa 128 bytes

    typedef struct _EBR {
        char part_status = '0';
        char part_fit;
        int part_start;
        int part_size = 0;
        int part_next = -1;
        char part_name[16];
    } EBR;
};

#endif
