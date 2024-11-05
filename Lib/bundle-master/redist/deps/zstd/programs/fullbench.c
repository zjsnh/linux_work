/*
    fullbench.c - Detailed bench program for zstd
    Copyright (C) Yann Collet 2014-2015

    GPL v2 License

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License along
    with this program; if not, write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

    You can contact the author at :
    - zstd source repository : https://github.com/Cyan4973/zstd
    - ztsd public forum : https://groups.google.com/forum/#!forum/lz4c
*/

/**************************************
*  Compiler Options
**************************************/
/* Disable some Visual warning messages */
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE     /* VS2005 */

/* Unix Large Files support (>4GB) */
#if (defined(__sun__) && (!defined(__LP64__)))   /* Sun Solaris 32-bits requires specific definitions */
#  define _LARGEFILE_SOURCE
#  define _FILE_OFFSET_BITS 64
#elif ! defined(__LP64__)                        /* No point defining Large file for 64 bit */
#  define _LARGEFILE64_SOURCE
#endif

/* S_ISREG & gettimeofday() are not supported by MSVC */
#if defined(_MSC_VER) || defined(_WIN32)
#  define BMK_LEGACY_TIMER 1
#endif


/**************************************
*  Includes
**************************************/
#include <stdlib.h>       /* malloc */
#include <stdio.h>        /* fprintf, fopen, ftello64 */
#include <sys/types.h>    /* stat64 */
#include <sys/stat.h>     /* stat64 */
#include <string.h>       /* strcmp */

/* Use ftime() if gettimeofday() is not available on your target */
#if defined(BMK_LEGACY_TIMER)
#  include <sys/timeb.h>  /* timeb, ftime */
#else
#  include <sys/time.h>   /* gettimeofday */
#endif

#include "mem.h"
#include "zstd.h"
#include "fse_static.h"
#include "datagen.h"


/**************************************
*  Compiler Options
**************************************/
/* S_ISREG & gettimeofday() are not supported by MSVC */
#if !defined(S_ISREG)
#  define S_ISREG(x) (((x) & S_IFMT) == S_IFREG)
#endif


/**************************************
*  Constants
**************************************/
#define PROGRAM_DESCRIPTION "Zstandard speed analyzer"
#ifndef ZSTD_VERSION
#  define ZSTD_VERSION ""
#endif
#define AUTHOR "Yann Collet"
#define WELCOME_MESSAGE "*** %s %s %i-bits, by %s (%s) ***\n", PROGRAM_DESCRIPTION, ZSTD_VERSION, (int)(sizeof(void*)*8), AUTHOR, __DATE__


#define KB *(1<<10)
#define MB *(1<<20)

#define NBLOOPS    6
#define TIMELOOP   2500

#define KNUTH      2654435761U
#define MAX_MEM    (1984 MB)
#define DEFAULT_CHUNKSIZE   (4<<20)

#define COMPRESSIBILITY_DEFAULT 0.50
static const size_t sampleSize = 10000000;


/**************************************
*  Macros
**************************************/
#define DISPLAY(...)  fprintf(stderr, __VA_ARGS__)


/**************************************
*  Benchmark Parameters
**************************************/
static int nbIterations = NBLOOPS;
static double g_compressibility = COMPRESSIBILITY_DEFAULT;

void BMK_SetNbIterations(int nbLoops)
{
    nbIterations = nbLoops;
    DISPLAY("- %i iterations -\n", nbIterations);
}


/*********************************************************
*  Private functions
*********************************************************/

#if defined(BMK_LEGACY_TIMER)

static int BMK_GetMilliStart(void)
{
  /* Based on Legacy ftime()
  *  Rolls over every ~ 12.1 days (0x100000/24/60/60)
  *  Use GetMilliSpan to correct for rollover */
  struct timeb tb;
  int nCount;
  ftime( &tb );
  nCount = (int) (tb.millitm + (tb.time & 0xfffff) * 1000);
  return nCount;
}

#else

static int BMK_GetMilliStart(void)
{
  /* Based on newer gettimeofday()
  *  Use GetMilliSpan to correct for rollover */
  struct timeval tv;
  int nCount;
  gettimeofday(&tv, NULL);
  nCount = (int) (tv.tv_usec/1000 + (tv.tv_sec & 0xfffff) * 1000);
  return nCount;
}

#endif


static int BMK_GetMilliSpan( int nTimeStart )
{
  int nSpan = BMK_GetMilliStart() - nTimeStart;
  if ( nSpan < 0 )
    nSpan += 0x100000 * 1000;
  return nSpan;
}


static size_t BMK_findMaxMem(U64 requiredMem)
{
    size_t step = 64 MB;
    BYTE* testmem=NULL;

    requiredMem = (((requiredMem >> 26) + 1) << 26);
    if (requiredMem > MAX_MEM) requiredMem = MAX_MEM;

    requiredMem += 2*step;
    while (!testmem)
    {
        requiredMem -= step;
        testmem = (BYTE*) malloc ((size_t)requiredMem);
    }

    free (testmem);
    return (size_t) (requiredMem - step);
}


static U64 BMK_GetFileSize(char* infilename)
{
    int r;
#if defined(_MSC_VER)
    struct _stat64 statbuf;
    r = _stat64(infilename, &statbuf);
#else
    struct stat statbuf;
    r = stat(infilename, &statbuf);
#endif
    if (r || !S_ISREG(statbuf.st_mode)) return 0;   /* No good... */
    return (U64)statbuf.st_size;
}


/*********************************************************
*  Benchmark wrappers
*********************************************************/
typedef enum { bt_compressed, bt_raw, bt_rle, bt_end } blockType_t;
typedef struct
{
    blockType_t blockType;
    U32 unusedBits;
    U32 origSize;
} blockProperties_t;

static size_t g_cSize = 0;
static U32 g_litCtx[40 * 1024];

extern size_t ZSTD_getcBlockSize(const void* src, size_t srcSize, blockProperties_t* bpPtr);
extern size_t ZSTD_decodeSeqHeaders(int* nbSeq, const BYTE** dumpsPtr, size_t* dumpsLengthPtr, FSE_DTable* DTableLL, FSE_DTable* DTableML, FSE_DTable* DTableOffb, const void* src, size_t srcSize);

size_t local_ZSTD_compress(void* dst, size_t dstSize, void* buff2, const void* src, size_t srcSize)
{
    (void)buff2;
    return ZSTD_compress(dst, dstSize, src, srcSize);
}

size_t local_ZSTD_decompress(void* dst, size_t dstSize, void* buff2, const void* src, size_t srcSize)
{
    (void)src; (void)srcSize;
    return ZSTD_decompress(dst, dstSize, buff2, g_cSize);
}

extern size_t ZSTD_decodeLiteralsBlock(void* ctx, const void* src, size_t srcSize);
size_t local_ZSTD_decodeLiteralsBlock(void* dst, size_t dstSize, void* buff2, const void* src, size_t srcSize)
{
    (void)src; (void)srcSize; (void)dst; (void)dstSize;
    return ZSTD_decodeLiteralsBlock(g_litCtx, buff2, g_cSize);
}

size_t local_ZSTD_decodeSeqHeaders(void* dst, size_t dstSize, void* buff2, const void* src, size_t srcSize)
{
    U32 DTableML[1<<11], DTableLL[1<<10], DTableOffb[1<<9];
    const BYTE* dumps;
    size_t length;
    int nbSeq;
    (void)src; (void)srcSize; (void)dst; (void)dstSize;
    return ZSTD_decodeSeqHeaders(&nbSeq, &dumps, &length, DTableLL, DTableML, DTableOffb, buff2, g_cSize);
}

size_t local_conditionalNull(void* dst, size_t dstSize, void* buff2, const void* src, size_t srcSize)
{
    U32 i;
    size_t total = 0;
    BYTE* data = (BYTE*)buff2;

    (void)dst; (void)dstSize; (void)src;
    for (i=0; i < srcSize; i++)
    {
        U32 b = data[i];
        total += b;
        if (b==0) total = 0;   // 825
        //if (!b) total = 0;     // 825
        //total = b ? total : 0; // 622
        //total &= -!b;          // 622
        //total *= !!b;          // 465
    }
    return total;
}

size_t local_decodeLiteralsForward(void* dst, size_t dstSize, void* buff2, const void* src, size_t srcSize)
{
    (void)src; (void)srcSize;
    return FSE_decompress(dst, dstSize, buff2, g_cSize);
}



/*********************************************************
*  Bench functions
*********************************************************/
size_t benchMem(void* src, size_t srcSize, U32 benchNb)
{
    BYTE*  dstBuff;
    size_t dstBuffSize;
    BYTE*  buff2;
    int loopNb;
    const char* benchName;
    size_t (*benchFunction)(void* dst, size_t dstSize, void* verifBuff, const void* src, size_t srcSize);
    double bestTime = 100000000.;
    size_t errorCode = 0;

    /* Selection */
    switch(benchNb)
    {
    case 1:
        benchFunction = local_ZSTD_compress; benchName = "ZSTD_compress";
        break;
    case 11:
        benchFunction = local_ZSTD_decompress; benchName = "ZSTD_decompress";
        break;
    case 31:
        benchFunction = local_ZSTD_decodeLiteralsBlock; benchName = "ZSTD_decodeLiteralsBlock";
        break;
    case 32:
        benchFunction = local_ZSTD_decodeSeqHeaders; benchName = "ZSTD_decodeSeqHeaders";
        break;
    case 101:
        benchFunction = local_conditionalNull; benchName = "conditionalNull";
        break;
    case 102:
        benchFunction = local_decodeLiteralsForward; benchName = "ZSTD_decodeLiteralsForward";
        break;
    default :
        return 0;
    }

    /* Allocation */
    dstBuffSize = ZSTD_compressBound(srcSize);
    dstBuff = (BYTE*)malloc(dstBuffSize);
    buff2 = (BYTE*)malloc(dstBuffSize);
    if ((!dstBuff) || (!buff2))
    {
        DISPLAY("\nError: not enough memory!\n");
        free(dstBuff); free(buff2);
        return 12;
    }

    /* Preparation */
    switch(benchNb)
    {
    case 11:
        g_cSize = ZSTD_compress(buff2, dstBuffSize, src, srcSize);
        break;
    case 31:  /* ZSTD_decodeLiteralsBlock */
        {
            blockProperties_t bp;
            g_cSize = ZSTD_compress(dstBuff, dstBuffSize, src, srcSize);
            ZSTD_getcBlockSize(dstBuff+4, dstBuffSize, &bp);   // Get first block type
            if (bp.blockType != bt_compressed)
            {
                DISPLAY("ZSTD_decodeLiteralsBlock : impossible to test on this sample (not compressible)\n");
                free(dstBuff);
                free(buff2);
                return 0;
            }
            memcpy(buff2, dstBuff+7, g_cSize-7);
            srcSize = srcSize > 128 KB ? 128 KB : srcSize;   // relative to block
            break;
        }
    case 32:   /* ZSTD_decodeSeqHeaders */
        {
            blockProperties_t bp;
            const BYTE* ip = dstBuff;
            const BYTE* iend;
            size_t blockSize;
            ZSTD_compress(dstBuff, dstBuffSize, src, srcSize);
            ip += 4;   // Jump magic Number
            blockSize = ZSTD_getcBlockSize(ip, dstBuffSize, &bp);   // Get first block type
            if (bp.blockType != bt_compressed)
            {
                DISPLAY("ZSTD_decodeSeqHeaders : impossible to test on this sample (not compressible)\n");
                free(dstBuff);
                free(buff2);
                return 0;
            }
            iend = ip + 3 + blockSize;   // Get end of first block
            ip += 3;   // jump first block header
            ip += ZSTD_decodeLiteralsBlock(g_litCtx, ip, iend-ip);  // jump literal sub block and its header
            g_cSize = iend-ip;
            memcpy(buff2, ip, g_cSize);   // copy rest of block (starting with SeqHeader)
            srcSize = srcSize > 128 KB ? 128 KB : srcSize;   // speed relative to block
            break;
        }

    /* test functions */

    case 101:   /* conditionalNull */
        {
            size_t i;
            for (i=0; i<srcSize; i++)
                buff2[i] = i & 15;
            break;
        }
    case 102:   /* local_decodeLiteralsForward */
        {
            blockProperties_t bp;
            ZSTD_compress(dstBuff, dstBuffSize, src, srcSize);
            g_cSize = ZSTD_getcBlockSize(dstBuff+7, dstBuffSize, &bp);
            memcpy(buff2, dstBuff+10, g_cSize);
            //srcSize = benchFunction(dstBuff, dstBuffSize, buff2, src, srcSize);   // real speed
            srcSize = srcSize > 128 KB ? 128 KB : srcSize;   // relative to block
            break;
        }
    default : ;
    }

    { size_t i; for (i=0; i<dstBuffSize; i++) dstBuff[i]=(BYTE)i; }     /* warming up memory */

    for (loopNb = 1; loopNb <= nbIterations; loopNb++)
    {
        double averageTime;
        int milliTime;
        U32 nbRounds=0;

        DISPLAY("%2i- %-30.30s : \r", loopNb, benchName);

        milliTime = BMK_GetMilliStart();
        while(BMK_GetMilliStart() == milliTime);
        milliTime = BMK_GetMilliStart();
        while(BMK_GetMilliSpan(milliTime) < TIMELOOP)
        {
            errorCode = benchFunction(dstBuff, dstBuffSize, buff2, src, srcSize);
            if (ZSTD_isError(errorCode)) { DISPLAY("ERROR ! %s() => %s !! \n", benchName, ZSTD_getErrorName(errorCode)); exit(1); }
            nbRounds++;
        }
        milliTime = BMK_GetMilliSpan(milliTime);

        averageTime = (double)milliTime / nbRounds;
        if (averageTime < bestTime) bestTime = averageTime;
        DISPLAY("%2i- %-30.30s : %7.1f MB/s  (%9u)\r", loopNb, benchName, (double)srcSize / bestTime / 1000., (U32)errorCode);
    }

    DISPLAY("%2u- %-30.30s : %7.1f MB/s  (%9u)\n", benchNb, benchName, (double)srcSize / bestTime / 1000., (U32)errorCode);

    free(dstBuff);
    free(buff2);
    return 0;
}


int benchSample(U32 benchNb)
{
    char* origBuff;
    size_t benchedSize = sampleSize;
    const char* name = "Sample 10MiB";

    /* Allocation */
    origBuff = (char*) malloc((size_t)benchedSize);
    if(!origBuff)
    {
        DISPLAY("\nError: not enough memory!\n");
        return 12;
    }

    /* Fill buffer */
    RDG_genBuffer(origBuff, benchedSize, g_compressibility, 0.0, 0);

    /* bench */
    DISPLAY("\r%79s\r", "");
    DISPLAY(" %s : \n", name);
    if (benchNb)
        benchMem(origBuff, benchedSize, benchNb);
    else
        for (benchNb=0; benchNb<100; benchNb++) benchMem(origBuff, benchedSize, benchNb);

    free(origBuff);
    return 0;
}


int benchFiles(char** fileNamesTable, int nbFiles, U32 benchNb)
{
    int fileIdx=0;

    /* Loop for each file */
    while (fileIdx<nbFiles)
    {
        FILE* inFile;
        char* inFileName;
        U64   inFileSize;
        size_t benchedSize;
        size_t readSize;
        char* origBuff;

        /* Check file existence */
        inFileName = fileNamesTable[fileIdx++];
        inFile = fopen( inFileName, "rb" );
        if (inFile==NULL)
        {
            DISPLAY( "Pb opening %s\n", inFileName);
            return 11;
        }

        // Memory allocation & restrictions
        inFileSize = BMK_GetFileSize(inFileName);
        benchedSize = (size_t) BMK_findMaxMem(inFileSize*3) / 3;
        if ((U64)benchedSize > inFileSize) benchedSize = (size_t)inFileSize;
        if (benchedSize < inFileSize)
        {
            DISPLAY("Not enough memory for '%s' full size; testing %i MB only...\n", inFileName, (int)(benchedSize>>20));
        }

        /* Alloc */
        origBuff = (char*) malloc((size_t)benchedSize);
        if(!origBuff)
        {
            DISPLAY("\nError: not enough memory!\n");
            fclose(inFile);
            return 12;
        }

        /* Fill input buffer */
        DISPLAY("Loading %s...       \r", inFileName);
        readSize = fread(origBuff, 1, benchedSize, inFile);
        fclose(inFile);

        if(readSize != benchedSize)
        {
            DISPLAY("\nError: problem reading file '%s' !!    \n", inFileName);
            free(origBuff);
            return 13;
        }

        /* bench */
        DISPLAY("\r%79s\r", "");
        DISPLAY(" %s : \n", inFileName);
        if (benchNb)
            benchMem(origBuff, benchedSize, benchNb);
        else
            for (benchNb=0; benchNb<100; benchNb++) benchMem(origBuff, benchedSize, benchNb);
    }

    return 0;
}


int usage(char* exename)
{
    DISPLAY( "Usage :\n");
    DISPLAY( "      %s [arg] file1 file2 ... fileX\n", exename);
    DISPLAY( "Arguments :\n");
    DISPLAY( " -H/-h  : Help (this text + advanced options)\n");
    return 0;
}

int usage_advanced(void)
{
    DISPLAY( "\nAdvanced options :\n");
    DISPLAY( " -b#    : test only function # \n");
    DISPLAY( " -i#    : iteration loops [1-9](default : %i)\n", NBLOOPS);
    DISPLAY( " -P#    : sample compressibility (default : %.1f%%)\n", COMPRESSIBILITY_DEFAULT * 100);
    return 0;
}

int badusage(char* exename)
{
    DISPLAY("Wrong parameters\n");
    usage(exename);
    return 0;
}

int main(int argc, char** argv)
{
    int i,
        filenamesStart=0,
        result;
    char* exename=argv[0];
    char* input_filename=0;
    U32 benchNb = 0, main_pause = 0;

    // Welcome message
    DISPLAY(WELCOME_MESSAGE);

    if (argc<1) { badusage(exename); return 1; }

    for(i=1; i<argc; i++)
    {
        char* argument = argv[i];

        if(!argument) continue;   // Protection if argument empty

        // Decode command (note : aggregated commands are allowed)
        if (argument[0]=='-')
        {
            while (argument[1]!=0)
            {
                argument ++;

                switch(argument[0])
                {
                    /* Display help on usage */
                case 'h' :
                case 'H': usage(exename); usage_advanced(); return 0;

                    /* Pause at the end (hidden option) */
                case 'p': main_pause = 1; break;

                    /* Select specific algorithm to bench */
                case 'b':
                    benchNb = 0;
                    while ((argument[1]>= '0') && (argument[1]<= '9'))
                    {
                        benchNb *= 10;
                        benchNb += argument[1] - '0';
                        argument++;
                    }
                    break;

                    /* Modify Nb Iterations */
                case 'i':
                    if ((argument[1] >='0') && (argument[1] <='9'))
                    {
                        int iters = argument[1] - '0';
                        BMK_SetNbIterations(iters);
                        argument++;
                    }
                    break;

                    /* Select specific algorithm to bench */
                case 'P':
                    {
                        U32 proba32 = 0;
                        while ((argument[1]>= '0') && (argument[1]<= '9'))
                        {
                            proba32 *= 10;
                            proba32 += argument[1] - '0';
                            argument++;
                        }
                        g_compressibility = (double)proba32 / 100.;
                    }
                    break;

                    /* Unknown command */
                default : badusage(exename); return 1;
                }
            }
            continue;
        }

        /* first provided filename is input */
        if (!input_filename) { input_filename=argument; filenamesStart=i; continue; }
    }

    if (filenamesStart==0)
        result = benchSample(benchNb);
    else result = benchFiles(argv+filenamesStart, argc-filenamesStart, benchNb);

    if (main_pause) { int unused; printf("press enter...\n"); unused = getchar(); (void)unused; }

    return result;
}

