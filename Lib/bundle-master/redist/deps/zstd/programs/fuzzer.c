/*
    Fuzzer test tool for zstd
    Copyright (C) Yann Collet 2014-2105

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
    - ZSTD source repository : https://github.com/Cyan4973/zstd
    - ZSTD public forum : https://groups.google.com/forum/#!forum/lz4c
*/

/**************************************
*  Compiler specific
**************************************/
#ifdef _MSC_VER    /* Visual Studio */
#  define _CRT_SECURE_NO_WARNINGS     /* fgets */
#  pragma warning(disable : 4127)     /* disable: C4127: conditional expression is constant */
#  pragma warning(disable : 4146)     /* disable: C4146: minus unsigned expression */
#endif

#define GCC_VERSION (__GNUC__ * 100 + __GNUC_MINOR__)
#ifdef __GNUC__
#  pragma GCC diagnostic ignored "-Wmissing-braces"   /* GCC bug 53119 : doesn't accept { 0 } as initializer (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119) */
#  pragma GCC diagnostic ignored "-Wmissing-field-initializers"   /* GCC bug 53119 : doesn't accept { 0 } as initializer (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=53119) */
#endif


/**************************************
*  Includes
**************************************/
#include <stdlib.h>      /* free */
#include <stdio.h>       /* fgets, sscanf */
#include <sys/timeb.h>   /* timeb */
#include <string.h>      /* strcmp */
#include "zstd_static.h"
#include "zstdhc_static.h"
#include "datagen.h"     /* RDG_genBuffer */
#include "xxhash.h"      /* XXH64 */
#include "mem.h"


/**************************************
 Constants
**************************************/
#ifndef ZSTD_VERSION
#  define ZSTD_VERSION ""
#endif

#define KB *(1U<<10)
#define MB *(1U<<20)
#define GB *(1U<<30)

static const U32 nbTestsDefault = 32 KB;
#define COMPRESSIBLE_NOISE_LENGTH (10 MB)
#define FUZ_COMPRESSIBILITY_DEFAULT 50
static const U32 prime1 = 2654435761U;
static const U32 prime2 = 2246822519U;



/**************************************
*  Display Macros
**************************************/
#define DISPLAY(...)          fprintf(stderr, __VA_ARGS__)
#define DISPLAYLEVEL(l, ...)  if (g_displayLevel>=l) { DISPLAY(__VA_ARGS__); }
static U32 g_displayLevel = 2;

#define DISPLAYUPDATE(l, ...) if (g_displayLevel>=l) { \
            if ((FUZ_GetMilliSpan(g_time) > g_refreshRate) || (g_displayLevel>=4)) \
            { g_time = FUZ_GetMilliStart(); DISPLAY(__VA_ARGS__); \
            if (g_displayLevel>=4) fflush(stdout); } }
static const U32 g_refreshRate = 150;
static U32 g_time = 0;


/*********************************************************
*  Fuzzer functions
*********************************************************/
static U32 FUZ_GetMilliStart(void)
{
    struct timeb tb;
    U32 nCount;
    ftime( &tb );
    nCount = (U32) (((tb.time & 0xFFFFF) * 1000) +  tb.millitm);
    return nCount;
}


static U32 FUZ_GetMilliSpan(U32 nTimeStart)
{
    U32 nCurrent = FUZ_GetMilliStart();
    U32 nSpan = nCurrent - nTimeStart;
    if (nTimeStart > nCurrent)
        nSpan += 0x100000 * 1000;
    return nSpan;
}


#  define FUZ_rotl32(x,r) ((x << r) | (x >> (32 - r)))
unsigned int FUZ_rand(unsigned int* src)
{
    U32 rand32 = *src;
    rand32 *= prime1;
    rand32 += prime2;
    rand32  = FUZ_rotl32(rand32, 13);
    *src = rand32;
    return rand32 >> 5;
}


static unsigned FUZ_highbit32(U32 v32)
{
    unsigned nbBits = 0;
    if (v32==0) return 0;
    while (v32)
    {
        v32 >>= 1;
        nbBits ++;
    }
    return nbBits;
}


static int basicUnitTests(U32 seed, double compressibility)
{
    int testResult = 0;
    void* CNBuffer;
    void* compressedBuffer;
    void* decodedBuffer;
    U32 randState = seed;
    size_t result, cSize;
    U32 testNb=0;

    /* Create compressible test buffer */
    CNBuffer = malloc(COMPRESSIBLE_NOISE_LENGTH);
    compressedBuffer = malloc(ZSTD_compressBound(COMPRESSIBLE_NOISE_LENGTH));
    decodedBuffer = malloc(COMPRESSIBLE_NOISE_LENGTH);
    if (!CNBuffer || !compressedBuffer || !decodedBuffer)
    {
        DISPLAY("Not enough memory, aborting\n");
        testResult = 1;
        goto _end;
    }
    RDG_genBuffer(CNBuffer, COMPRESSIBLE_NOISE_LENGTH, compressibility, 0., randState);

    /* Basic tests */
    DISPLAYLEVEL(4, "test%3i : compress %u bytes : ", testNb++, COMPRESSIBLE_NOISE_LENGTH);
    result = ZSTD_compress(compressedBuffer, ZSTD_compressBound(COMPRESSIBLE_NOISE_LENGTH), CNBuffer, COMPRESSIBLE_NOISE_LENGTH);
    if (ZSTD_isError(result)) goto _output_error;
    cSize = result;
    DISPLAYLEVEL(4, "OK (%u bytes : %.2f%%)\n", (U32)cSize, (double)cSize/COMPRESSIBLE_NOISE_LENGTH*100);

    DISPLAYLEVEL(4, "test%3i : decompress %u bytes : ", testNb++, COMPRESSIBLE_NOISE_LENGTH);
    result = ZSTD_decompress(decodedBuffer, COMPRESSIBLE_NOISE_LENGTH, compressedBuffer, cSize);
    if (ZSTD_isError(result)) goto _output_error;
    DISPLAYLEVEL(4, "OK \n");

    {
        size_t i;
        DISPLAYLEVEL(4, "test%3i : check decompressed result : ", testNb++);
        for (i=0; i<COMPRESSIBLE_NOISE_LENGTH; i++)
        {
            if (((BYTE*)decodedBuffer)[i] != ((BYTE*)CNBuffer)[i]) goto _output_error;;
        }
        DISPLAYLEVEL(4, "OK \n");
    }

    DISPLAYLEVEL(4, "test%3i : decompress with 1 missing byte : ", testNb++);
    result = ZSTD_decompress(decodedBuffer, COMPRESSIBLE_NOISE_LENGTH, compressedBuffer, cSize-1);
    if (!ZSTD_isError(result)) goto _output_error;
    if (result != ERROR(srcSize_wrong)) goto _output_error;
    DISPLAYLEVEL(4, "OK \n");

    DISPLAYLEVEL(4, "test%3i : decompress with 1 too much byte : ", testNb++);
    result = ZSTD_decompress(decodedBuffer, COMPRESSIBLE_NOISE_LENGTH, compressedBuffer, cSize+1);
    if (!ZSTD_isError(result)) goto _output_error;
    if (result != ERROR(srcSize_wrong)) goto _output_error;
    DISPLAYLEVEL(4, "OK \n");

    /* Decompression defense tests */
    DISPLAYLEVEL(4, "test%3i : Check input length for magic number : ", testNb++);
    result = ZSTD_decompress(decodedBuffer, COMPRESSIBLE_NOISE_LENGTH, CNBuffer, 3);
    if (!ZSTD_isError(result)) goto _output_error;
    if (result != ERROR(srcSize_wrong)) goto _output_error;
    DISPLAYLEVEL(4, "OK \n");

    DISPLAYLEVEL(4, "test%3i : Check magic Number : ", testNb++);
    ((char*)(CNBuffer))[0] = 1;
    result = ZSTD_decompress(decodedBuffer, COMPRESSIBLE_NOISE_LENGTH, CNBuffer, 4);
    if (!ZSTD_isError(result)) goto _output_error;
    DISPLAYLEVEL(4, "OK \n");

    /* long rle test */
    {
        size_t sampleSize = 0;
        DISPLAYLEVEL(4, "test%3i : Long RLE test : ", testNb++);
        RDG_genBuffer(CNBuffer, sampleSize, compressibility, 0., randState);
        memset((char*)CNBuffer+sampleSize, 'B', 256 KB - 1);
        sampleSize += 256 KB - 1;
        RDG_genBuffer((char*)CNBuffer+sampleSize, 96 KB, compressibility, 0., randState);
        sampleSize += 96 KB;
        cSize = ZSTD_compress(compressedBuffer, ZSTD_compressBound(sampleSize), CNBuffer, sampleSize);
        if (ZSTD_isError(cSize)) goto _output_error;
        result = ZSTD_decompress(decodedBuffer, sampleSize, compressedBuffer, cSize);
        if (ZSTD_isError(result)) goto _output_error;
        if (result!=sampleSize) goto _output_error;
        DISPLAYLEVEL(4, "OK \n");
    }

_end:
    free(CNBuffer);
    free(compressedBuffer);
    free(decodedBuffer);
    return testResult;

_output_error:
    testResult = 1;
    DISPLAY("Error detected in Unit tests ! \n");
    goto _end;
}


static size_t findDiff(const void* buf1, const void* buf2, size_t max)
{
    const BYTE* b1 = (const BYTE*)buf1;
    const BYTE* b2 = (const BYTE*)buf2;
    size_t i;
    for (i=0; i<max; i++)
    {
        if (b1[i] != b2[i]) break;
    }
    return i;
}

#   define CHECK(cond, ...) if (cond) { DISPLAY("Error => "); DISPLAY(__VA_ARGS__); \
                            DISPLAY(" (seed %u, test nb %u)  \n", seed, testNb); goto _output_error; }

static const U32 maxSrcLog = 23;
static const U32 maxSampleLog = 22;

int fuzzerTests(U32 seed, U32 nbTests, unsigned startTest, double compressibility)
{
    BYTE* cNoiseBuffer[5];
    BYTE* srcBuffer;
    BYTE* cBuffer;
    BYTE* dstBuffer;
    size_t srcBufferSize = (size_t)1<<maxSrcLog;
    size_t dstBufferSize = (size_t)1<<maxSampleLog;
    size_t cBufferSize   = ZSTD_compressBound(dstBufferSize);
    U32 result = 0;
    U32 testNb = 0;
    U32 coreSeed = seed, lseed = 0;
    ZSTD_CCtx* ctx;
    ZSTD_HC_CCtx* hcctx;

    /* allocation */
    ctx = ZSTD_createCCtx();
    hcctx = ZSTD_HC_createCCtx();
    cNoiseBuffer[0] = (BYTE*)malloc (srcBufferSize);
    cNoiseBuffer[1] = (BYTE*)malloc (srcBufferSize);
    cNoiseBuffer[2] = (BYTE*)malloc (srcBufferSize);
    cNoiseBuffer[3] = (BYTE*)malloc (srcBufferSize);
    cNoiseBuffer[4] = (BYTE*)malloc (srcBufferSize);
    dstBuffer = (BYTE*)malloc (dstBufferSize);
    cBuffer   = (BYTE*)malloc (cBufferSize);
    CHECK (!cNoiseBuffer[0] || !cNoiseBuffer[1] || !cNoiseBuffer[2] || !dstBuffer || !cBuffer || !ctx || !hcctx,
           "Not enough memory, fuzzer tests cancelled");

    /* Create initial samples */
    RDG_genBuffer(cNoiseBuffer[0], srcBufferSize, 0.00, 0., coreSeed);    /* pure noise */
    RDG_genBuffer(cNoiseBuffer[1], srcBufferSize, 0.05, 0., coreSeed);    /* barely compressible */
    RDG_genBuffer(cNoiseBuffer[2], srcBufferSize, compressibility, 0., coreSeed);
    RDG_genBuffer(cNoiseBuffer[3], srcBufferSize, 0.95, 0., coreSeed);    /* highly compressible */
    RDG_genBuffer(cNoiseBuffer[4], srcBufferSize, 1.00, 0., coreSeed);    /* sparse content */
    srcBuffer = cNoiseBuffer[2];

    /* catch up testNb */
    for (testNb=1; testNb < startTest; testNb++)
        FUZ_rand(&coreSeed);

    /* test loop */
    for ( ; testNb <= nbTests; testNb++ )
    {
        size_t sampleSize, sampleStart;
        size_t cSize, dSize, dSupSize;
        U32 sampleSizeLog, buffNb;
        U64 crcOrig, crcDest;
        int cLevel;

        /* init */
        DISPLAYUPDATE(2, "\r%6u/%6u   ", testNb, nbTests);
        FUZ_rand(&coreSeed);
        lseed = coreSeed ^ prime1;
        buffNb = FUZ_rand(&lseed) & 127;
        if (buffNb & 7) buffNb=2;
        else
        {
            buffNb >>= 3;
            if (buffNb & 7)
            {
                const U32 tnb[2] = { 1, 3 };
                buffNb = tnb[buffNb >> 3];
            }
            else
            {
                const U32 tnb[2] = { 0, 4 };
                buffNb = tnb[buffNb >> 3];
            }
        }
        srcBuffer = cNoiseBuffer[buffNb];
        sampleSizeLog = FUZ_rand(&lseed) % maxSampleLog;
        sampleSize = (size_t)1 << sampleSizeLog;
        sampleSize += FUZ_rand(&lseed) & (sampleSize-1);
        sampleStart = FUZ_rand(&lseed) % (srcBufferSize - sampleSize);
        crcOrig = XXH64(srcBuffer + sampleStart, sampleSize, 0);

        /* HC compression test */
        cLevel = (FUZ_rand(&lseed) & 3) + 2;
        cSize = ZSTD_HC_compressCCtx(hcctx, cBuffer, cBufferSize, srcBuffer + sampleStart, sampleSize, cLevel);
        CHECK(ZSTD_isError(cSize), "ZSTD_compress failed");

        /* compression test */
        cSize = ZSTD_compressCCtx(ctx, cBuffer, cBufferSize, srcBuffer + sampleStart, sampleSize);
        CHECK(ZSTD_isError(cSize), "ZSTD_compress failed");

        /* compression failure test : too small dest buffer */
        if (cSize > 3)
        {
            size_t errorCode;
            const size_t missing = (FUZ_rand(&lseed) % (cSize-2)) + 1;   /* no problem, as cSize > 4 (frameHeaderSizer) */
            const size_t tooSmallSize = cSize - missing;
            static const U32 endMark = 0x4DC2B1A9;
            U32 endCheck;
            memcpy(dstBuffer+tooSmallSize, &endMark, 4);
            errorCode = ZSTD_compressCCtx(ctx, dstBuffer, tooSmallSize, srcBuffer + sampleStart, sampleSize);
            CHECK(!ZSTD_isError(errorCode), "ZSTD_compress should have failed ! (buffer too small)");
            memcpy(&endCheck, dstBuffer+tooSmallSize, 4);
            CHECK(endCheck != endMark, "ZSTD_compress : dst buffer overflow");
        }

        /* successfull decompression tests*/
        dSupSize = (FUZ_rand(&lseed) & 1) ? 0 : (FUZ_rand(&lseed) & 31) + 1;
        dSize = ZSTD_decompress(dstBuffer, sampleSize + dSupSize, cBuffer, cSize);
        CHECK(dSize != sampleSize, "ZSTD_decompress failed (%s)", ZSTD_getErrorName(dSize));
        crcDest = XXH64(dstBuffer, sampleSize, 0);
        CHECK(crcOrig != crcDest, "dstBuffer corrupted (pos %u / %u)", (U32)findDiff(srcBuffer+sampleStart, dstBuffer, sampleSize), (U32)sampleSize);

        /* truncated src decompression test */
        {
            size_t errorCode;
            const size_t missing = (FUZ_rand(&lseed) % (cSize-2)) + 1;   /* no problem, as cSize > 4 (frameHeaderSizer) */
            const size_t tooSmallSize = cSize - missing;
            void* cBufferTooSmall = malloc(tooSmallSize);   /* valgrind will catch overflows */
            CHECK(cBufferTooSmall == NULL, "not enough memory !");
            memcpy(cBufferTooSmall, cBuffer, tooSmallSize);
            errorCode = ZSTD_decompress(dstBuffer, dstBufferSize, cBufferTooSmall, tooSmallSize);
            CHECK(!ZSTD_isError(errorCode), "ZSTD_decompress should have failed ! (truncated src buffer)");
            free(cBufferTooSmall);
        }

        /* too small dst decompression test */
        if (sampleSize > 3)
        {
            size_t errorCode;
            const size_t missing = (FUZ_rand(&lseed) % (sampleSize-2)) + 1;   /* no problem, as cSize > 4 (frameHeaderSizer) */
            const size_t tooSmallSize = sampleSize - missing;
            static const BYTE token = 0xA9;
            dstBuffer[tooSmallSize] = token;
            errorCode = ZSTD_decompress(dstBuffer, tooSmallSize, cBuffer, cSize);
            CHECK(!ZSTD_isError(errorCode), "ZSTD_decompress should have failed : %u > %u (dst buffer too small)", (U32)errorCode, (U32)tooSmallSize);
            CHECK(dstBuffer[tooSmallSize] != token, "ZSTD_decompress : dst buffer overflow");
        }

        /* noisy src decompression test */
        if (cSize > 6)
        {
            const U32 maxNbBits = FUZ_highbit32((U32)(cSize-4));
            size_t pos = 4;   /* preserve magic number (too easy to detect) */
            U32 nbBits = FUZ_rand(&lseed) % maxNbBits;
            size_t mask = (1<<nbBits) - 1;
            size_t skipLength = FUZ_rand(&lseed) & mask;
            pos += skipLength;

            while (pos < cSize)
            {
                /* add noise */
                size_t noiseStart, noiseLength;
                nbBits = FUZ_rand(&lseed) % maxNbBits;
                if (nbBits>0) nbBits--;
                mask = (1<<nbBits) - 1;
                noiseLength = (FUZ_rand(&lseed) & mask) + 1;
                if ( pos+noiseLength > cSize ) noiseLength = cSize-pos;
                noiseStart = FUZ_rand(&lseed) % (srcBufferSize - noiseLength);
                memcpy(cBuffer + pos, srcBuffer + noiseStart, noiseLength);
                pos += noiseLength;

                /* keep some original src */
                nbBits = FUZ_rand(&lseed) % maxNbBits;
                mask = (1<<nbBits) - 1;
                skipLength = FUZ_rand(&lseed) & mask;
                pos += skipLength;
            }

            /* decompress noisy source */
            {
                U32 noiseSrc = FUZ_rand(&lseed) % 5;
                const U32 endMark = 0xA9B1C3D6;
                U32 endCheck;
                size_t errorCode;
                srcBuffer = cNoiseBuffer[noiseSrc];
                memcpy(dstBuffer+sampleSize, &endMark, 4);
                errorCode = ZSTD_decompress(dstBuffer, sampleSize, cBuffer, cSize);
                /* result *may* be an unlikely success, but even then, it must strictly respect dest buffer boundaries */
                CHECK((!ZSTD_isError(errorCode)) && (errorCode>sampleSize),
                      "ZSTD_decompress on noisy src : result is too large : %u > %u (dst buffer)", (U32)errorCode, (U32)sampleSize);
                memcpy(&endCheck, dstBuffer+sampleSize, 4);
                CHECK(endMark!=endCheck, "ZSTD_decompress on noisy src : dst buffer overflow");
            }
        }
    }
    DISPLAY("\rAll fuzzer tests completed   \n");

_cleanup:
    ZSTD_freeCCtx(ctx);
    ZSTD_HC_freeCCtx(hcctx);
    free(cNoiseBuffer[0]);
    free(cNoiseBuffer[1]);
    free(cNoiseBuffer[2]);
    free(cNoiseBuffer[3]);
    free(cNoiseBuffer[4]);
    free(cBuffer);
    free(dstBuffer);
    return result;

_output_error:
    result = 1;
    goto _cleanup;
}


/*********************************************************
*  Command line
*********************************************************/
int FUZ_usage(char* programName)
{
    DISPLAY( "Usage :\n");
    DISPLAY( "      %s [args]\n", programName);
    DISPLAY( "\n");
    DISPLAY( "Arguments :\n");
    DISPLAY( " -i#    : Nb of tests (default:%u) \n", nbTestsDefault);
    DISPLAY( " -s#    : Select seed (default:prompt user)\n");
    DISPLAY( " -t#    : Select starting test number (default:0)\n");
    DISPLAY( " -P#    : Select compressibility in %% (default:%i%%)\n", FUZ_COMPRESSIBILITY_DEFAULT);
    DISPLAY( " -v     : verbose\n");
    DISPLAY( " -p     : pause at the end\n");
    DISPLAY( " -h     : display help and exit\n");
    return 0;
}


int main(int argc, char** argv)
{
    U32 seed=0;
    int seedset=0;
    int argNb;
    int nbTests = nbTestsDefault;
    int testNb = 0;
    int proba = FUZ_COMPRESSIBILITY_DEFAULT;
    int result=0;
    U32 mainPause = 0;
    char* programName;

    /* Check command line */
    programName = argv[0];
    for(argNb=1; argNb<argc; argNb++)
    {
        char* argument = argv[argNb];

        if(!argument) continue;   /* Protection if argument empty */

        /* Handle commands. Aggregated commands are allowed */
        if (argument[0]=='-')
        {
            argument++;

            while (*argument!=0)
            {
                switch(*argument)
                {
                case 'h':
                    return FUZ_usage(programName);
                case 'v':
                    argument++;
                    g_displayLevel=4;
                    break;
                case 'q':
                    argument++;
                    g_displayLevel--;
                    break;
                case 'p': /* pause at the end */
                    argument++;
                    mainPause = 1;
                    break;

                case 'i':
                    argument++;
                    nbTests=0;
                    while ((*argument>='0') && (*argument<='9'))
                    {
                        nbTests *= 10;
                        nbTests += *argument - '0';
                        argument++;
                    }
                    break;

                case 's':
                    argument++;
                    seed=0;
                    seedset=1;
                    while ((*argument>='0') && (*argument<='9'))
                    {
                        seed *= 10;
                        seed += *argument - '0';
                        argument++;
                    }
                    break;

                case 't':
                    argument++;
                    testNb=0;
                    while ((*argument>='0') && (*argument<='9'))
                    {
                        testNb *= 10;
                        testNb += *argument - '0';
                        argument++;
                    }
                    break;

                case 'P':   /* compressibility % */
                    argument++;
                    proba=0;
                    while ((*argument>='0') && (*argument<='9'))
                    {
                        proba *= 10;
                        proba += *argument - '0';
                        argument++;
                    }
                    if (proba<0) proba=0;
                    if (proba>100) proba=100;
                    break;

                default:
                    return FUZ_usage(programName);
                }
            }
        }
    }

    /* Get Seed */
    DISPLAY("Starting zstd tester (%i-bits, %s)\n", (int)(sizeof(size_t)*8), ZSTD_VERSION);

    if (!seedset) seed = FUZ_GetMilliStart() % 10000;
    DISPLAY("Seed = %u\n", seed);
    if (proba!=FUZ_COMPRESSIBILITY_DEFAULT) DISPLAY("Compressibility : %i%%\n", proba);

    if (nbTests<=0) nbTests=1;

    if (testNb==0) result = basicUnitTests(0, ((double)proba) / 100);  /* constant seed for predictability */
    if (!result)
        result = fuzzerTests(seed, nbTests, testNb, ((double)proba) / 100);
    if (mainPause)
    {
        int unused;
        DISPLAY("Press Enter \n");
        unused = getchar();
        (void)unused;
    }
    return result;
}
