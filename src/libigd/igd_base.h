//=================================================================================
//Common structs, parameters, functions
//by Jianglin Feng  05/12/2018
//re-designed 7/1/2019
//updated by Nathan Sheffield 04/2020
//database intervals sorted by _start: 8/12/2019
//---------------------------------------------------------------------------------
#ifndef __IGD_BASE_H__
#define __IGD_BASE_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <err.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include <glob.h>
#include <zlib.h>
#include <errno.h>
#include <sysexits.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <assert.h>
#include "khash.h"
#include "kseq.h"
#include "kradix.h"

#define PROGRAM_NAME  "igd"
#define MAJOR_VERSION "0"
#define MINOR_VERSION "1"
#define REVISION_VERSION "1"
#define BUILD_VERSION "0"
#define VERSION MAJOR_VERSION "." MINOR_VERSION "." REVISION_VERSION
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define maxCount 268435456              //16* = 4GB memory

#ifdef __cplusplus
extern "C"{
#endif 


//---------------------------------------------------------------------------------
typedef struct{                         //default
    int32_t idx;                        //genomic object--data set index
    int32_t start;                      //region start
    int32_t end;                        //region end
    int32_t value;
} gdata_t;

typedef struct{
    char* fileName;                     //dataset file
    int32_t nr;                         //number regions/dataset
    double md;                          //average width of the regions
} info_t;

typedef struct{
    int32_t ncnts, nCnts, mcnts;        //batch counts, total, max
    gdata_t *gList;
} tile_t;

typedef struct{
    char *name;                         //name of the contig
    int32_t mTiles;                     //determined by the interval start and end
    tile_t *gTile;            //tile data
} ctg_t;

typedef struct{
    int32_t nbp, gType, nctg, mctg;     // number of base pairs, data type: 0, 1, 2 etc; size differs
    int64_t total;                      //total region in each ctg
    ctg_t *ctg;                     //list of contigs (of size _n_ctg_)
    void *hc;                               //dict for converting contig names to int
} igd_t; 

                                   //For creation: internal...

/**
 * @brief Representation of IGD database objects
 *
 * Detailed explanation.
 */
typedef struct{                         //For search: external...
    int32_t nbp, gType, nCtg, nFiles;
    int32_t preIdx, preChr;
    char fname[64];
    char **cName;                       //name of ctgs
    int32_t *nTile;                     //num of tiles in each ctg
    int32_t **nCnt;                     //num of counts in each tile
    int64_t **tIdx;                     //tile index *sizeof -> location in .igd file
    gdata_t *gData;
    info_t *finfo;
    FILE *fP;
    void *hc;
} iGD_t;

//---------------------------------------------------------------------------------
//Parse a line of BED file
void str_splits( char* str, int *nmax, char **splits);
char *parse_bed(char *s, int32_t *st_, int32_t *en_);

//Binary search
int32_t bSearch(gdata_t *gdata, int32_t t0, int32_t tc, int32_t qe);

//Add an interval
void igd_add(igd_t *igd, const char *chrm, int32_t s, int32_t e, int32_t v, int32_t idx);

//Get id from igd dict
int32_t get_id(iGD_t *iGD, const char *chrm);

//Get nFiles from iGD
int32_t get_nFiles(iGD_t *iGD);

//Get file info from .tsv
info_t *get_fileinfo(char *ifName, int32_t *nFiles);

//Get igd info from .igd
iGD_t *open_iGD(char *igdFile);

//Initialize igd_t
igd_t *igd_init(int tile_size);

//Initialize iGD_t
iGD_t *iGD_init();

//Save tile data
void igd_saveT(igd_t *igd, char *oPath);

//Sort and save igd
void igd_save(igd_t *igd, char *oPath, char *igdName);

//Free igd data
void igd_destroy(igd_t *igd);

//Free iGD data
void close_iGD(iGD_t *iGD);


#ifdef __cplusplus
}
#endif

#endif