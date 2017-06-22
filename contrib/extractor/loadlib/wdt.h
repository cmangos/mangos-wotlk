#ifndef WDT_H
#define WDT_H
#include "loadlib.h"

//**************************************************************************************
// WDT file class and structures
//**************************************************************************************

#define WDT_MAP_SIZE 64

#define fcc_MWMO 0x4d574d4f // MWMO
#define fcc_MPHD 0x4d504844 // MPHD
#define fcc_MAIN 0x4d41494e // MAIN

class wdt_MWMO
{
    private:
        uint32 fcc;

    public:
        uint32 size;
        bool prepareLoadedData();
};

class wdt_MPHD
{
    private:
        uint32 fcc;

    public:
        uint32 size;
        uint32 data1;
        uint32 data2;
        uint32 data3;
        uint32 data4;
        uint32 data5;
        uint32 data6;
        uint32 data7;
        uint32 data8;
        bool   prepareLoadedData();
};

class wdt_MAIN
{
    private:
        uint32 fcc;

    public:
        uint32 size;
        struct adtData
        {
            uint32 exist;
            uint32 data1;
        } adt_list[64][64];

        bool   prepareLoadedData();
};

class WDT_file : public FileLoader
{
    public:
        bool prepareLoadedData();

        WDT_file();
        ~WDT_file();
        void free();

        wdt_MPHD* mphd;
        wdt_MAIN* main;
        wdt_MWMO* wmo;
};

#endif
