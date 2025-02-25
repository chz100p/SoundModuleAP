/**********************************************************************

  Audacity: A Digital Audio Editor

  Dither.cpp

  Steve Harris
  Markus Meyer

*******************************************************************//*!

\class Dither
\brief

  This class implements various functions for dithering and is derived
  from the dither code in the Ardour project, written by Steve Harris.

  Dithering is only done if it really is necessary. Otherwise (e.g.
  when the source and destination format of the samples is the same),
  the samples are only copied or converted. However, copied samples
  are always checked for out-of-bounds values and possibly clipped
  accordingly.

  These dither algorithms are currently implemented:
  - No dithering at all
  - Rectangle dithering
  - Triangle dithering
  - Noise-shaped dithering

Dither class. You must construct an instance because it keeps
state. Call Dither::Apply() to apply the dither. You can call
Reset() between subsequent dithers to reset the dither state
and get deterministic behaviour.

*//*******************************************************************/


// Erik de Castro Lopo's header file that
// makes sure that we have lrint and lrintf
// (Note: this file should be included first)
#include "float_cast.h"

#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <sys/types.h>
//#include <memory.h>
//#include <assert.h>

#include <wx/defs.h>

#include "Dither.h"

//////////////////////////////////////////////////////////////////////////

// Constants for the noise shaping buffer
const int Dither::BUF_MASK = 7;
const int Dither::BUF_SIZE = 8;

// Lipshitz's minimally audible FIR
const float Dither::SHAPED_BS[] = { 2.033f, -2.165f, 1.959f, -1.590f, 0.6149f };

// This is supposed to produce white noise and no dc
#define DITHER_NOISE (rand() / (float)RAND_MAX - 0.5f)

// The following is a rather ugly, but fast implementation
// of a dither loop. The macro "DITHER" is expanded to an implementation
// of a dithering algorithm, which contains no branches in the inner loop
// except the branches for clipping the sample, and therefore should
// be quite fast.

#if 0
// To assist in understanding what the macros are doing, here's an example of what
// the result would be for Shaped dither:
//
// DITHER(ShapedDither, dest, destFormat, destStride, source, sourceFormat, sourceStride, len);

    do {
        if (sourceFormat == int24Sample && destFormat == int16Sample)
            do {
                char *d, *s;
                unsigned int i;
                int x;
                for (d = (char*)dest, s = (char*)source, i = 0; i < len; i++, d += (int16Sample >> 16) * destStride, s += (int24Sample >> 16) * sourceStride)
                    do {
                        x = lrintf((ShapedDither((((*(( int*)(s)) / float(1<<23))) * float(1<<15)))));
                        if (x>(32767))
                            *((short*)((((d)))))=(32767);
                        else if (x<(-32768))
                            *((short*)((((d)))))=(-32768);
                        else *((short*)((((d)))))=(short)x;
                    } while (0);
            } while (0);
        else if (sourceFormat == floatSample && destFormat == int16Sample)
            do {
                char *d, *s;
                unsigned int i;
                int x;
                for (d = (char*)dest, s = (char*)source, i = 0; i < len; i++, d += (int16Sample >> 16) * destStride, s += (floatSample >> 16) * sourceStride)
                    do {
                        x = lrintf((ShapedDither((((*((float*)(s)) > 1.0 ? 1.0 : *((float*)(s)) < -1.0 ? -1.0 : *((float*)(s)))) * float(1<<15)))));
                        if (x>(32767))
                            *((short*)((((d)))))=(32767);
                        else if (x<(-32768))
                            *((short*)((((d)))))=(-32768);
                        else *((short*)((((d)))))=(short)x;
                    } while (0);
            } while (0);
        else if (sourceFormat == floatSample && destFormat == int24Sample)
            do {
                char *d, *s;
                unsigned int i;
                int x;
                for (d = (char*)dest, s = (char*)source, i = 0; i < len; i++, d += (int24Sample >> 16) * destStride, s += (floatSample >> 16) * sourceStride)
                    do {
                        x = lrintf((ShapedDither((((*((float*)(s)) > 1.0 ? 1.0 : *((float*)(s)) < -1.0 ? -1.0 : *((float*)(s)))) * float(1<<23)))));
                        if (x>(8388607))
                            *((int*)((((d)))))=(8388607);
                        else if (x<(-8388608))
                            *((int*)((((d)))))=(-8388608);
                        else *((int*)((((d)))))=(int)x;
                    } while (0);
            } while (0);
        else {
            if ( false )
                ;
            else wxOnAssert(L"c:\\users\\yam\\documents\\audacity\\mixer\\n\\audacity\\src\\dither.cpp", 348,  __FUNCTION__  , L"false", 0);
        }
    } while (0);
#endif

// Defines for sample conversion
#define CONVERT_DIV8  float(1<< 7)
#define CONVERT_DIV16 float(1<<15)
#define CONVERT_DIV24 float(1<<23)

// Dereference sample pointer and convert to float sample
#define FROM_INT8(ptr)  (*(( char*)(ptr)) / CONVERT_DIV8 )
#define FROM_INT16(ptr) (*((short*)(ptr)) / CONVERT_DIV16)
#define FROM_INT24(ptr) (*((  int*)(ptr)) / CONVERT_DIV24)

// For float, we internally allow values greater than 1.0, which
// would blow up the dithering to int values.  FROM_FLOAT is
// only used to dither to int, so clip here.
#define FROM_FLOAT(ptr) (*((float*)(ptr)) >  1.0 ?  1.0 : \
                         *((float*)(ptr)) < -1.0 ? -1.0 : \
                         *((float*)(ptr)))

// Promote sample to range of specified type, keep it float, though
#define PROMOTE_TO_INT8(sample)  ((sample) * CONVERT_DIV8 )
#define PROMOTE_TO_INT16(sample) ((sample) * CONVERT_DIV16)
#define PROMOTE_TO_INT24(sample) ((sample) * CONVERT_DIV24)

// Store float sample 'sample' into pointer 'ptr', clip it, if necessary
// Note: This assumes, a variable 'x' of type int is valid which is
//       used by this macro.
#define IMPLEMENT_STORE(ptr, sample, ptr_type, min_bound, max_bound) \
    do { \
    x = lrintf(sample); \
    if (x>(max_bound)) *((ptr_type*)(ptr))=(max_bound); \
    else if (x<(min_bound)) *((ptr_type*)(ptr))=(min_bound); \
    else *((ptr_type*)(ptr))=(ptr_type)x; } while (0)

#define STORE_INT8(ptr, sample)  IMPLEMENT_STORE((ptr), (sample), char, -128, 127)
#define STORE_INT16(ptr, sample) IMPLEMENT_STORE((ptr), (sample), short, -32768, 32767)
#define STORE_INT24(ptr, sample) IMPLEMENT_STORE((ptr), (sample), int, -8388608, 8388607)

// Dither single float 'sample' and store it in pointer 'dst', using 'dither' as algorithm
#define DITHER_TO_INT8(dither, dst, sample)  STORE_INT8((dst), dither(PROMOTE_TO_INT8(sample)))
#define DITHER_TO_INT16(dither, dst, sample) STORE_INT16((dst), dither(PROMOTE_TO_INT16(sample)))
#define DITHER_TO_INT24(dither, dst, sample) STORE_INT24((dst), dither(PROMOTE_TO_INT24(sample)))

// Implement one single dither step
#define DITHER_STEP(dither, store, load, dst, src) \
    store(dither, (dst), load(src))

// Implement a dithering loop
// Note: The variable 'x' is needed for the STORE_... macros
#define DITHER_LOOP(dither, store, load, dst, dstFormat, dstStride, src, srcFormat, srcStride, len) \
    do { \
       char *d, *s; \
       unsigned int i; \
       int x; \
       for (d = (char*)dst, s = (char*)src, i = 0; \
            i < len; \
            i++, d += SAMPLE_SIZE(dstFormat) * dstStride, \
                 s += SAMPLE_SIZE(srcFormat) * srcStride) \
          DITHER_STEP(dither, store, load, d, s); \
   } while (0)

// Shortcuts to dithering loops
#define DITHER_INT16_TO_INT8(dither, dst, dstStride, src, srcStride, len) \
    DITHER_LOOP(dither, DITHER_TO_INT8 , FROM_INT16, dst, int8Sample , dstStride, src, int16Sample, srcStride, len)
#define DITHER_INT24_TO_INT8(dither, dst, dstStride, src, srcStride, len) \
    DITHER_LOOP(dither, DITHER_TO_INT8 , FROM_INT24, dst, int8Sample , dstStride, src, int24Sample, srcStride, len)
#define DITHER_FLOAT_TO_INT8(dither, dst, dstStride, src, srcStride, len) \
    DITHER_LOOP(dither, DITHER_TO_INT8 , FROM_FLOAT, dst, int8Sample , dstStride, src, floatSample, srcStride, len)
#define DITHER_INT24_TO_INT16(dither, dst, dstStride, src, srcStride, len) \
    DITHER_LOOP(dither, DITHER_TO_INT16, FROM_INT24, dst, int16Sample, dstStride, src, int24Sample, srcStride, len)
#define DITHER_FLOAT_TO_INT16(dither, dst, dstStride, src, srcStride, len) \
    DITHER_LOOP(dither, DITHER_TO_INT16, FROM_FLOAT, dst, int16Sample, dstStride, src, floatSample, srcStride, len)
#define DITHER_FLOAT_TO_INT24(dither, dst, dstStride, src, srcStride, len) \
    DITHER_LOOP(dither, DITHER_TO_INT24, FROM_FLOAT, dst, int24Sample, dstStride, src, floatSample, srcStride, len)

// Implement a dither. There are only 3 cases where we must dither,
// in all other cases, no dithering is necessary.
#define DITHER(dither, dst, dstFormat, dstStride, src, srcFormat, srcStride, len) \
    do { if (srcFormat == int16Sample && dstFormat == int8Sample ) \
        DITHER_INT16_TO_INT8(dither, dst, dstStride, src, srcStride, len); \
    else if (srcFormat == int24Sample && dstFormat == int8Sample ) \
        DITHER_INT24_TO_INT8(dither, dst, dstStride, src, srcStride, len); \
    else if (srcFormat == floatSample && dstFormat == int8Sample ) \
        DITHER_FLOAT_TO_INT8(dither, dst, dstStride, src, srcStride, len); \
    else if (srcFormat == int24Sample && dstFormat == int16Sample) \
        DITHER_INT24_TO_INT16(dither, dst, dstStride, src, srcStride, len); \
    else if (srcFormat == floatSample && dstFormat == int16Sample) \
        DITHER_FLOAT_TO_INT16(dither, dst, dstStride, src, srcStride, len); \
    else if (srcFormat == floatSample && dstFormat == int24Sample) \
        DITHER_FLOAT_TO_INT24(dither, dst, dstStride, src, srcStride, len); \
    else { wxASSERT(false); } \
    } while (0)


Dither::Dither()
{
    // On startup, initialize dither by resetting values
    Reset();
}

void Dither::Reset()
{
    mTriangleState = 0;
    mPhase = 0;
    memset(mBuffer, 0, sizeof(float) * BUF_SIZE);
}

// This only decides if we must dither at all, the dithers
// are all implemented using macros.
//
// "source" and "dest" can contain either interleaved or non-interleaved
// samples.  They do not have to be the same...one can be interleaved while
// the otner non-interleaved.
//
// The "len" argument specifies the number of samples to process.
//
// If either stride value equals 1 then the corresponding buffer contains
// non-interleaved samples.
//
// If either stride value is greater than 1 then the corresponding buffer
// contains interleaved samples and they will be processed by skipping every
// stride number of samples.

void Dither::Apply(enum DitherType ditherType,
                   const samplePtr source, sampleFormat sourceFormat,
                   samplePtr dest, sampleFormat destFormat,
                   unsigned int len,
                   unsigned int sourceStride /* = 1 */,
                   unsigned int destStride /* = 1 */)
{
    unsigned int i;

    // This code is not designed for 16-bit or 64-bit machine
    wxASSERT(sizeof(int) == 4);
    wxASSERT(sizeof(short) == 2);

    // Check parameters
    wxASSERT(source);
    wxASSERT(dest);
    wxASSERT(len >= 0);
    wxASSERT(sourceStride > 0);
    wxASSERT(destStride > 0);

    if (len == 0)
        return; // nothing to do

    if (destFormat == sourceFormat)
    {
        // No need to dither, because source and destination
        // format are the same. Just copy samples.
        if (destStride == 1 && sourceStride == 1)
            memcpy(dest, source, len * SAMPLE_SIZE(destFormat));
        else
        {
            if (sourceFormat == floatSample)
            {
                float* d = (float*)dest;
                float* s = (float*)source;

                for (i = 0; i < len; i++, d += destStride, s += sourceStride)
                    *d = *s;
            } else
            if (sourceFormat == int24Sample)
            {
                int* d = (int*)dest;
                int* s = (int*)source;

                for (i = 0; i < len; i++, d += destStride, s += sourceStride)
                    *d = *s;
            } else
            if (sourceFormat == int16Sample)
            {
                short* d = (short*)dest;
                short* s = (short*)source;

                for (i = 0; i < len; i++, d += destStride, s += sourceStride)
                    *d = *s;
            } else
            if (sourceFormat == int8Sample )
            {
                char* d = (char*)dest;
                char* s = (char*)source;

                for (i = 0; i < len; i++, d += destStride, s += sourceStride)
                    *d = *s;
            } else {
                wxASSERT(false); // source format unknown
            }
        }
    } else
    if (destFormat == floatSample)
    {
        // No need to dither, just convert samples to float.
        // No clipping should be necessary.
        float* d = (float*)dest;

        if (sourceFormat == int8Sample )
        {
            char* s = (char*)source;
            for (i = 0; i < len; i++, d += destStride, s += sourceStride)
                *d = FROM_INT8(s);
        } else
        if (sourceFormat == int16Sample)
        {
            short* s = (short*)source;
            for (i = 0; i < len; i++, d += destStride, s += sourceStride)
                *d = FROM_INT16(s);
        } else
        if (sourceFormat == int24Sample)
        {
            int* s = (int*)source;
            for (i = 0; i < len; i++, d += destStride, s += sourceStride)
                *d = FROM_INT24(s);
        } else {
            wxASSERT(false); // source format unknown
        }
    } else
    if (destFormat == int16Sample && sourceFormat == int8Sample )
    {
        // Special case when promoting 8 bit to 16 bit
        short* d = (short*)dest;
        char* s = (char*)source;
        for (i = 0; i < len; i++, d += destStride, s += sourceStride)
            *d = ((short)*s) << 8;
    } else
    if (destFormat == int24Sample && sourceFormat == int8Sample )
    {
        // Special case when promoting 8 bit to 24 bit
        int* d = (int*)dest;
        char* s = (char*)source;
        for (i = 0; i < len; i++, d += destStride, s += sourceStride)
            *d = ((int)*s) << 16;
    } else
    if (destFormat == int24Sample && sourceFormat == int16Sample)
    {
        // Special case when promoting 16 bit to 24 bit
        int* d = (int*)dest;
        short* s = (short*)source;
        for (i = 0; i < len; i++, d += destStride, s += sourceStride)
            *d = ((int)*s) << 8;
    } else
    {
        // We must do dithering
        switch (ditherType)
        {
        case none:
            DITHER(NoDither, dest, destFormat, destStride, source, sourceFormat, sourceStride, len);
            break;
        case rectangle:
            DITHER(RectangleDither, dest, destFormat, destStride, source, sourceFormat, sourceStride, len);
            break;
        case triangle:
            Reset(); // reset dither filter for this new conversion
            DITHER(TriangleDither, dest, destFormat, destStride, source, sourceFormat, sourceStride, len);
            break;
        case shaped:
            Reset(); // reset dither filter for this new conversion
            DITHER(ShapedDither, dest, destFormat, destStride, source, sourceFormat, sourceStride, len);
            break;
        default:
            wxASSERT(false); // unknown dither algorithm
        }
    }
}

// Dither implementations

// No dither, just return sample
inline float Dither::NoDither(float sample)
{
    return sample;
}

// Rectangle dithering, apply one-step noise
inline float Dither::RectangleDither(float sample)
{
    return sample - DITHER_NOISE;
}

// Triangle dither - high pass filtered
inline float Dither::TriangleDither(float sample)
{
    float r = DITHER_NOISE;
    float result = sample + r - mTriangleState;
    mTriangleState = r;

    return result;
}

// Shaped dither
inline float Dither::ShapedDither(float sample)
{
    // Generate triangular dither, +-1 LSB, flat psd
    float r = DITHER_NOISE + DITHER_NOISE;
    if(sample != sample)  // test for NaN
       sample = 0; // and do the best we can with it

    // Run FIR
    float xe = sample + mBuffer[mPhase] * SHAPED_BS[0]
        + mBuffer[(mPhase - 1) & BUF_MASK] * SHAPED_BS[1]
        + mBuffer[(mPhase - 2) & BUF_MASK] * SHAPED_BS[2]
        + mBuffer[(mPhase - 3) & BUF_MASK] * SHAPED_BS[3]
        + mBuffer[(mPhase - 4) & BUF_MASK] * SHAPED_BS[4];

    // Accumulate FIR and triangular noise
    float result = xe + r;

    // Roll buffer and store last error
    mPhase = (mPhase + 1) & BUF_MASK;
    mBuffer[mPhase] = xe - lrintf(result);

    return result;
}
