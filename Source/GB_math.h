//------------------------------------------------------------------------------
// GB_math.h: definitions for complex types, and mathematical operators
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// This file is #include'd in GB.h, which defines the complex typedefs:
// GxB_FC32_t and GxB_FC64_t.

#ifndef GB_COMPLEX_H
#define GB_COMPLEX_H

//------------------------------------------------------------------------------
// complex macros
//------------------------------------------------------------------------------

#if ( _MSC_VER && !__INTEL_COMPILER )

    //--------------------------------------------------------------------------
    // Microsoft Visual Studio compiler with its own complex type
    //--------------------------------------------------------------------------

    // complex-complex multiply: z = x*y where both x and y are complex
    #define GB_FC32_CtimesC(x,y) (_FCmulcc (x, y))
    #define GB_FC64_CtimesC(x,y) ( _Cmulcc (x, y))

    // complex-real multiply: z = x*y where x is complex and y is real
    #define GB_FC32_CtimesR(x,y) (_FCmulcr (x, y))
    #define GB_FC64_CtimesR(x,y) ( _Cmulcr (x, y))

    // complex-complex addition: z = x+y where both x and y are complex
    #define GB_FC32_CplusC(x,y) \
        GxB_CMPLXF (crealf (x) + crealf (y), cimagf (x) + cimagf (y))
    #define GB_FC64_CplusC(x,y) \
        GxB_CMPLX  (creal  (x) + creal  (y), cimag  (x) + cimag  (y))

    // complex-complex subtraction: z = x-y where both x and y are complex
    #define GB_FC32_CminusC(x,y) \
        GxB_CMPLXF (crealf (x) - crealf (y), cimagf (x) - cimagf (y))
    #define GB_FC64_CminusC(x,y) \
        GxB_CMPLX  (creal  (x) - creal  (y), cimag  (x) - cimag  (y))

    // complex negation: z = -x
    #define GB_FC32_ainv(x) GxB_CMPLXF (-crealf (x), -cimagf (x))
    #define GB_FC64_ainv(x) GxB_CMPLX (-creal  (x), -cimag  (x))

#else

    //--------------------------------------------------------------------------
    // ANSI C99 with native complex type support
    //--------------------------------------------------------------------------

    // complex-complex multiply: z = x*y where both x and y are complex
    #define GB_FC32_CtimesC(x,y) ((x) * (y))
    #define GB_FC64_CtimesC(x,y) ((x) * (y))

    // complex-real multiply: z = x*y where x is complex and y is real
    #define GB_FC32_CtimesR(x,y) ((x) * (y))
    #define GB_FC64_CtimesR(x,y) ((x) * (y))

    // complex-complex addition: z = x+y where both x and y are complex
    #define GB_FC32_CplusC(x,y) ((x) + (y))
    #define GB_FC64_CplusC(x,y) ((x) + (y))

    // complex-complex subtraction: z = x-y where both x and y are complex
    #define GB_FC32_CminusC(x,y) ((x) - (y))
    #define GB_FC64_CminusC(x,y) ((x) - (y))

    // complex negation
    #define GB_FC32_ainv(x) (-(x))
    #define GB_FC64_ainv(x) (-(x))

#endif

// complex inverse: z = 1/x
#define GB_FC32_minv(x) GB_FC32_div (GxB_CMPLXF (1,0), x)
#define GB_FC64_minv(x) GB_FC64_div (GxB_CMPLX (1,0), x)

// complex-real multiply: z = x*y where x is real and y is complex
#define GB_FC32_RtimesC(x,y) GB_FC32_CtimesR(y,x) 
#define GB_FC64_RtimesC(x,y) GB_FC64_CtimesR(y,x) 

// complex comparisons
#define GB_FC32_eq(x,y) ((crealf(x) == crealf(y)) && (cimagf(x) == cimagf(y)))
#define GB_FC64_eq(x,y) ((creal (x) == creal (y)) && (cimag (x) == cimag (y)))

#define GB_FC32_ne(x,y) ((crealf(x) != crealf(y)) || (cimagf(x) != cimagf(y)))
#define GB_FC64_ne(x,y) ((creal (x) != creal (y)) || (cimag (x) != cimag (y)))

#define GB_FC32_iseq(x,y) GxB_CMPLXF ((float) GB_FC32_eq (x,y), 0)
#define GB_FC64_iseq(x,y) GxB_CMPLX  ((float) GB_FC64_eq (x,y), 0)

#define GB_FC32_isne(x,y) GxB_CMPLXF ((float) GB_FC32_ne (x,y), 0)
#define GB_FC64_isne(x,y) GxB_CMPLX  ((float) GB_FC64_ne (x,y), 0)

//------------------------------------------------------------------------------
// complex division
//------------------------------------------------------------------------------

// z = x/y where z, x, and y are double complex.  The real and imaginary parts
// are passed as separate arguments to this routine.  The NaN case is ignored
// for the double relop yr >= yi.  Returns 1 if the denominator is zero, 0
// otherwise.
//
// This uses ACM Algo 116, by R. L. Smith, 1962, which tries to avoid underflow
// and overflow.
//
// z can be the same variable as x or y.
//
// Note that this function has the same signature as SuiteSparse_divcomplex.

inline int GB_divcomplex
(
    double xr, double xi,       // real and imaginary parts of x
    double yr, double yi,       // real and imaginary parts of y
    double *zr, double *zi      // real and imaginary parts of z
)
{
    double tr, ti, r, den ;

    int yr_class = fpclassify (yr) ;
    int yi_class = fpclassify (yi) ;

    if (yi_class == FP_ZERO)
    {
        den = yr ;
        if (xi == 0)
        {
            tr = xr / den ;
            ti = 0 ;
        }
        else if (xr == 0)
        {
            tr = 0 ;
            ti = xi / den ;
        }
        else
        {
            tr = xr / den ;
            ti = xi / den ;
        }
    }
    else if (yr_class == FP_ZERO)
    {
        den = yi ;
        if (xr == 0)
        {
            tr = xi / den ;
            ti = 0 ;
        }
        else if (xi == 0)
        {
            tr = 0 ;
            ti = -xr / den ;
        }
        else
        {
            tr = xi / den ;
            ti = -xr / den ;
        }
    }
    else if (yi_class == FP_INFINITE && yr_class == FP_INFINITE)
    {
        r = (signbit (yr) == signbit (yi)) ? (1) : (-1) ;
        den = yr + r * yi ;
        tr = (xr + xi * r) / den ;
        ti = (xi - xr * r) / den ;
    }
    else if (fabs (yr) >= fabs (yi))
    {
        r = yi / yr ;
        den = yr + r * yi ;
        tr = (xr + xi * r) / den ;
        ti = (xi - xr * r) / den ;
    }
    else
    {
        r = yr / yi ;
        den = r * yr + yi ;
        tr = (xr * r + xi) / den ;
        ti = (xi * r - xr) / den ;
    }
    (*zr) = tr ;
    (*zi) = ti ;
    return (den == 0) ;
}

inline GxB_FC64_t GB_FC64_div (GxB_FC64_t x, GxB_FC64_t y)
{
    double zr, zi ;
    GB_divcomplex (creal (x), cimag (x), creal (y), cimag (y), &zr, &zi) ;
    return (GxB_CMPLX (zr, zi)) ;
}

inline GxB_FC32_t GB_FC32_div (GxB_FC32_t x, GxB_FC32_t y)
{
    // single complex division: typecast to double complex, do the division,
    // and then typecast back to single complex
    double zr, zi ;
    GB_divcomplex ((double) crealf (x), (double) cimagf (x),
                   (double) crealf (y), (double) cimagf (y), &zr, &zi) ;
    return (GxB_CMPLX (zr, zi)) ;
}

//------------------------------------------------------------------------------
// z = x^y: wrappers for pow, powf, cpow, and cpowf
//------------------------------------------------------------------------------

// The following rules are used to try to align the results with what MATLAB
// computes for x^y:

//      if x or y are NaN, then z is NaN
//      if y is zero, then z is 1
//      if (x and y are complex but with zero imaginary parts, and
//          (x >= 0 or if y is an integer, NaN, or Inf)), then z is real
//      else use the built-in C library function, z = pow (x,y)

inline float GB_powf (float x, float y)
{
    int xr_class = fpclassify (x) ;
    int yr_class = fpclassify (y) ;
    if (xr_class == FP_NAN || yr_class == FP_NAN)
    {
        // z is nan if either x or y are nan
        return (NAN) ;
    }
    if (yr_class == FP_ZERO)
    {
        // z is 1 if y is zero
        return (1) ;
    }
    // otherwise, z = powf (x,y)
    return (powf (x, y)) ;
}

inline double GB_pow (double x, double y)
{
    int xr_class = fpclassify (x) ;
    int yr_class = fpclassify (y) ;
    if (xr_class == FP_NAN || yr_class == FP_NAN)
    {
        // z is nan if either x or y are nan
        return (NAN) ;
    }
    if (yr_class == FP_ZERO)
    {
        // z is 1 if y is zero
        return (1) ;
    }
    // otherwise, z = pow (x,y)
    return (pow (x, y)) ;
}

inline GxB_FC32_t GB_cpowf (GxB_FC32_t x, GxB_FC32_t y)
{
    float xr = crealf (x) ;
    float yr = crealf (y) ;
    int xr_class = fpclassify (xr) ;
    int yr_class = fpclassify (yr) ;
    int xi_class = fpclassify (cimagf (x)) ;
    int yi_class = fpclassify (cimagf (y)) ;
    if (xi_class == FP_ZERO && yi_class == FP_ZERO)
    {
        // both x and y are real; see if z should be real
        if (xr >= 0 || yr_class == FP_NAN || yr_class == FP_INFINITE ||
            yr == truncf (yr))
        {
            // z is real if x >= 0, or if y is an integer, NaN, or Inf
            return (GxB_CMPLXF (GB_powf (xr, yr), 0)) ;
        }
    }
    if (xr_class == FP_NAN || xi_class == FP_NAN ||
        yr_class == FP_NAN || yi_class == FP_NAN)
    {
        // z is (nan,nan) if any part of x or y are nan
        return (GxB_CMPLXF (NAN, NAN)) ;
    }
    if (yr_class == FP_ZERO && yi_class == FP_ZERO)
    {
        // z is (1,0) if y is (0,0)
        return (GxB_CMPLXF (1, 0)) ;
    }
    return (cpowf (x, y)) ;
}

inline GxB_FC64_t GB_cpow (GxB_FC64_t x, GxB_FC64_t y)
{
    double xr = creal (x) ;
    double yr = creal (y) ;
    int xr_class = fpclassify (xr) ;
    int yr_class = fpclassify (yr) ;
    int xi_class = fpclassify (cimag (x)) ;
    int yi_class = fpclassify (cimag (y)) ;
    if (xi_class == FP_ZERO && yi_class == FP_ZERO)
    {
        // both x and y are real; see if z should be real
        if (xr >= 0 || yr_class == FP_NAN || yr_class == FP_INFINITE ||
            yr == trunc (yr))
        {
            // z is real if x >= 0, or if y is an integer, NaN, or Inf
            return (GxB_CMPLX (GB_pow (xr, yr), 0)) ;
        }
    }
    if (xr_class == FP_NAN || xi_class == FP_NAN ||
        yr_class == FP_NAN || yi_class == FP_NAN)
    {
        // z is (nan,nan) if any part of x or y are nan
        return (GxB_CMPLX (NAN, NAN)) ;
    }
    if (yr_class == FP_ZERO && yi_class == FP_ZERO)
    {
        // z is (1,0) if y is (0,0)
        return (GxB_CMPLX (1, 0)) ;
    }
    return (cpow (x, y)) ;
}

inline int8_t GB_pow_int8 (int8_t x, int8_t y)
{
    double t = GB_pow ((double) x, (double) y) ;
    int8_t z ;
    GB_CAST_SIGNED (z, t, 8) ;
    return (z) ;
}

inline int16_t GB_pow_int16 (int16_t x, int16_t y)
{
    double t = GB_pow ((double) x, (double) y) ;
    int16_t z ;
    GB_CAST_SIGNED (z, t, 16) ;
    return (z) ;
}

inline int32_t GB_pow_int32 (int32_t x, int32_t y)
{
    double t = GB_pow ((double) x, (double) y) ;
    int32_t z ;
    GB_CAST_SIGNED (z, t, 32) ;
    return (z) ;
}

inline int64_t GB_pow_int64 (int64_t x, int64_t y)
{
    double t = GB_pow ((double) x, (double) y) ;
    int64_t z ;
    GB_CAST_SIGNED (z, t, 64) ;
    return (z) ;
}

inline uint8_t GB_pow_uint8 (uint8_t x, uint8_t y)
{
    double t = GB_pow ((double) x, (double) y) ;
    uint8_t z ;
    GB_CAST_UNSIGNED (z, t, 8) ;
    return (z) ;
}

inline uint16_t GB_pow_uint16 (uint16_t x, uint16_t y)
{
    double t = GB_pow ((double) x, (double) y) ;
    uint16_t z ;
    GB_CAST_UNSIGNED (z, t, 16) ;
    return (z) ;
}

inline uint32_t GB_pow_uint32 (uint32_t x, uint32_t y)
{
    double t = GB_pow ((double) x, (double) y) ;
    uint32_t z ;
    GB_CAST_UNSIGNED (z, t, 32) ;
    return (z) ;
}

inline uint64_t GB_pow_uint64 (uint64_t x, uint64_t y)
{
    double t = GB_pow ((double) x, (double) y) ;
    uint64_t z ;
    GB_CAST_UNSIGNED (z, t, 64) ;
    return (z) ;
}

//------------------------------------------------------------------------------
// frexp for float and double
//------------------------------------------------------------------------------

inline float GB_frexpxf (float x)
{
    int exp_ignored ;
    return (frexpf (x, &exp_ignored) ;
}

inline float GB_frexpef (float x)
{
    int exp ;
    float mantissa_ignored = frexpf (x, &exp) ;
    return ((float) exp) ;
}

inline double GB_frexpx (double x)
{
    int exp_ignored ;
    return (frexp (x, &exp_ignored) ;
}

inline double GB_frexpe (double x)
{
    int exp ;
    double mantissa_ignored = frexp (x, &exp) ;
    return ((double) exp) ;
}

//------------------------------------------------------------------------------
// bitget, bitset, bitclr
//------------------------------------------------------------------------------

// bitget (x,k) returns a single bit from x, as 0 or 1, whose position is given
// by k.  k = 1 is the least significant bit, and k = bits (64 for uint64)
// is the most significant bit. If k is outside this range, the result is zero.

#define GB_BITGET(x,k,type,bits)                                \
(                                                               \
    (k >= 1 && k <= bits) ?                                     \
    (                                                           \
        /* get the kth bit */                                   \
        ((x & (((type) 1) << (k-1))) ? 1 : 0)                   \
    )                                                           \
    :                                                           \
    (                                                           \
        0                                                       \
    )                                                           \
)

// bitset (x,k) returns x modified by setting a bit from x to 1, whose position
// is given by k.  If k is in the range 1 to bits, then k gives the position
// of the bit to set.  If k is outside the range 1 to bits, then z = x is
// returned, unmodified.

#define GB_BITSET(x,k,type,bits)                                \
(                                                               \
    (k >= 1 && k <= bits) ?                                     \
    (                                                           \
        /* set the kth bit to 1 */                              \
        (x | (((type) 1) << (k-1)))                             \
    )                                                           \
    :                                                           \
    (                                                           \
        x                                                       \
    )                                                           \
}

// bitclr (x,k) returns x modified by setting a bit from x to 0, whose position
// is given by k.  If k is in the range 1 to bits, then k gives the position of
// the bit to clear.  If k is outside the range 1 to GB_BITS, then z = x is
// returned, unmodified.

#define GB_BITCLR(x,k,type,bits)                                \
(                                                               \
    (k >= 1 && k <= bits)                                       \
    (                                                           \
        /* set the kth bit to 0 */                              \
        (x & ~(((type) 1) << (k-1)))                            \
    )                                                           \
    :                                                           \
    (                                                           \
        x                                                       \
    )                                                           \
)

//------------------------------------------------------------------------------
// z = bitshift (x,y) when x and z are unsigned
//------------------------------------------------------------------------------

inline uint8_t GB_bitshift_uint8 (uint8_t x, int8_t k)
{
    if (k == 0)
    {
        // no shift to do at all
        return (x) ;
    }
    else if (k >= 8 || k <= -8)
    {
        // ANSI C11 states that the result of x << k is undefined if k is
        // negative or if k is greater than the # of bits in x.  Here, the
        // result is defined to be zero (the same as if shifting left
        // or right by 8).
        return (0) ;
    }
    else if (k > 0)
    {
        // left shift x by k bits.  z is defined by ANSI C11 as
        // (x * (2^k)) mod (uintmax + 1).
        return (x << k) ;
    }
    else
    {
        k = -k ;
        // right shift x by k bits.  z is defined by ANSI C11 as the
        // integral part of the quotient of x / (2^k).
        return (x >> k) ;
    }
}

inline uint16_t GB_bitshift_uint16 (uint16_t x, int8_t k)
{
    if (k == 0)
    {
        // no shift
        return (x) ;
    }
    else if (k >= 16 || k <= -16)
    {
        return (0) ;
    }
    else if (k > 0)
    {
        // left shift
        return (x << k) ;
    }
    else
    {
        // right shift
        return (x >> (-k)) ;
    }
}

inline uint32_t GB_bitshift_uint32 (uint32_t x, int8_t k)
{
    if (k == 0)
    {
        // no shift
        return (x) ;
    }
    else if (k >= 32 || k <= -32)
    {
        return (0) ;
    }
    else if (k > 0)
    {
        // left shift
        return (x << k) ;
    }
    else
    {
        // right shift
        return (x >> (-k)) ;
    }
}

inline uint64_t GB_bitshift_uint64 (uint64_t x, int8_t k)
{
    if (k == 0)
    {
        // no shift
        return (x) ;
    }
    else if (k >= 64 || k <= -64)
    {
        return (0) ;
    }
    else if (k > 0)
    {
        // left shift
        return (x << k) ;
    }
    else
    {
        // right shift
        return (x >> (-k)) ;
    }
}

//------------------------------------------------------------------------------
// z = bitshift (x,y) when x and z are signed
//------------------------------------------------------------------------------

inline int8_t GB_bitshift_int8 (int8_t x, int8_t k)
{
    if (k == 0)
    {
        // no shift to do at all
        return (x) ;
    }
    else if (k >= 8)
    {
        // ANSI C11 states that z = x << k is undefined if k is greater
        // than the # of bits in x.  Here, the result is defined to be zero.
        return (0) ;
    }
    else if (k <= -8)
    {
        // ANSI C11 states that z = x >> (-k) is undefined if (-k) is
        // greater than the # of bits in x.  Here, the result is defined to
        // be the sign of x (z = 0 if x >= 0 and z = -1 if x is negative).
        return ((x >= 0) ? 0 : -1) ;
    }
    else if (k > 0)
    {
        // left shift x by k bits (where k is in range 1 to #bits - 1).
        // ANSI C11 states that z is defined only if x is non-negative and
        // x * (2^k) is representable.  This computation assumes x and z
        // are represented in 2's complement.  The result depends on the
        // underlying machine architecture and the compiler.
        return (x << k) ;
    }
    else
    {
        k = -k ;
        // right shift x by k bits (where k is in range 1 to 8)
        if (x >= 0)
        {
            // ANSI C11 defines z as the integral part of the quotient
            // of x / (2^k).
            return (x >> k) ;
        }
        else
        {
            // ANSI C11 states that the result is implementation-defined if
            // x is negative.  This computation assumes x and z are in 2's
            // complement, so 1-bits are shifted in on the left, and thus
            // the sign bit is always preserved.  The result depends on the
            // underlying machine architecture and the compiler.
            return ((x >> k) | (~(UINT8_MAX >> k))) ;
        }
    }
}

inline int16_t GB_bitshift_int16 (int16_t x, int8_t k)
{
    if (k == 0)
    {
        // no shift
        return (x) ;
    }
    else if (k >= 16)
    {
        return (0) ;
    }
    else if (k <= -16)
    {
        return ((x >= 0) ? 0 : -1) ;
    }
    else if (k > 0)
    {
        // left shift
        return (x << k) ;
    }
    else
    {
        // right shift
        k = -k ;
        if (x >= 0)
        {
            return (x >> k) ;
        }
        else
        {
            return ((x >> k) | (~(UINT16_MAX >> k))) ;
        }
    }
}

inline int32_t GB_bitshift_int32 (int32_t x, int8_t k)
{
    if (k == 0)
    {
        // no shift
        return (x) ;
    }
    else if (k >= 32)
    {
        return (0) ;
    }
    else if (k <= -32)
    {
        return ((x >= 0) ? 0 : -1) ;
    }
    else if (k > 0)
    {
        // left shift
        return (x << k) ;
    }
    else
    {
        // right shift
        k = -k ;
        if (x >= 0)
        {
            return (x >> k) ;
        }
        else
        {
            return ((x >> k) | (~(UINT32_MAX >> k))) ;
        }
    }
}

inline int64_t GB_bitshift_int64 (int64_t x, int8_t k)
{
    if (k == 0)
    {
        // no shift
        return (x) ;
    }
    else if (k >= 64)
    {
        return (0) ;
    }
    else if (k <= -64)
    {
        return ((x >= 0) ? 0 : -1) ;
    }
    else if (k > 0)
    {
        // left shift
        return (x << k) ;
    }
    else
    {
        // right shift
        k = -k ;
        if (x >= 0)
        {
            return (x >> k) ;
        }
        else
        {
            return ((x >> k) | (~(UINT64_MAX >> k))) ;
        }
    }
}

//------------------------------------------------------------------------------
// complex functions
//------------------------------------------------------------------------------

// The ANSI C11 math.h header defines the ceil, floor, round, trunc,
// exp2, expm1, log10, log1pm, or log2 functions for float and double,
// but the corresponding functions do not appear in the ANSI C11 complex.h.
// These functions are used instead, for float complex and double complex.

//------------------------------------------------------------------------------
// z = ceil (x) for float complex
//------------------------------------------------------------------------------

inline GxB_FC32_t GB_cceilf (GxB_FC32_t x)
{
    return (GxB_CMPLXF (ceilf (crealf (x)), ceilf (cimagf (x)))) ;
}

//------------------------------------------------------------------------------
// z = ceil (x) for double complex
//------------------------------------------------------------------------------

inline GxB_FC64_t GB_cceil (GxB_FC64_t x)
{
    return (GxB_CMPLX (ceil (creal (x)), ceil (cimag (x)))) ;
}

//------------------------------------------------------------------------------
// z = floor (x) for float complex
//------------------------------------------------------------------------------

inline GxB_FC32_t GB_cfloorf (GxB_FC32_t x)
{
    return (GxB_CMPLXF (floorf (crealf (x)), floorf (cimagf (x)))) ;
}

//------------------------------------------------------------------------------
// z = floor (x) for double complex
//------------------------------------------------------------------------------

inline GxB_FC64_t GB_cfloor (GxB_FC64_t x)
{
    return (GxB_CMPLX (floor (creal (x)), floor (cimag (x)))) ;
}

//------------------------------------------------------------------------------
// z = round (x) for float complex
//------------------------------------------------------------------------------

inline GxB_FC32_t GB_croundf (GxB_FC32_t x)
{
    return (GxB_CMPLXF (roundf (crealf (x)), roundf (cimagf (x)))) ;
}

//------------------------------------------------------------------------------
// z = round (x) for double complex
//------------------------------------------------------------------------------

inline GxB_FC64_t GB_cround (GxB_FC64_t x)
{
    return (GxB_CMPLX (round (creal (x)), round (cimag (x)))) ;
}

//------------------------------------------------------------------------------
// z = trunc (x) for float complex
//------------------------------------------------------------------------------

inline GxB_FC32_t GB_ctruncf (GxB_FC32_t x)
{
    return (GxB_CMPLXF (truncf (crealf (x)), truncf (cimagf (x)))) ;
}

//------------------------------------------------------------------------------
// z = trunc (x) for double complex
//------------------------------------------------------------------------------

inline GxB_FC64_t GB_ctrunc (GxB_FC64_t x)
{
    return (GxB_CMPLX (trunc (creal (x)), trunc (cimag (x)))) ;
}

//------------------------------------------------------------------------------
// z = exp2 (x) for float complex
//------------------------------------------------------------------------------

inline GxB_FC32_t GB_cexp2f (GxB_FC32_t x)
{
    if (fpclassify (cimagf (x)) == FP_ZERO)
    {
        // x is real, use exp2f
        return (GxB_CMPLXF (exp2f (crealf (x)), 0)) ;
    }
    return (GB_cpowf (GxB_CMPLXF (2,0), x)) ;     // z = 2^x
}

//------------------------------------------------------------------------------
// z = exp2 (x) for double complex
//------------------------------------------------------------------------------

inline GxB_FC64_t GB_cexp2 (GxB_FC64_t x)
{
    if (fpclassify (cimag (x)) == FP_ZERO)
    {
        // x is real, use exp2
        return (GxB_CMPLX (exp2 (creal (x)), 0)) ;
    }
    return (GB_cpow (GxB_CMPLX (2,0), x)) ;      // z = 2^x
}

//------------------------------------------------------------------------------
// z = expm1 (x) for float complex
//------------------------------------------------------------------------------

inline GxB_FC32_t GB_cexpm1f (GxB_FC32_t x)
{
    // FUTURE: this is not accurate
    // z = cexpf (x) - 1
    GxB_FC32_t z = cexpf (x) ;
    return (GxB_CMPLXF (crealf (z) - 1, cimagf (z))) ;
}

//------------------------------------------------------------------------------
// z = expm1 (x) for double complex
//------------------------------------------------------------------------------

inline GxB_FC64_t GB_cexpm1 (GxB_FC64_t x)
{
    // FUTURE: this is not accurate
    // z = cexp (x) - 1
    GxB_FC32_t z = cexp (x) ;
    return (GxB_CMPLX (creal (z) - 1, cimag (z))) ;
}

//------------------------------------------------------------------------------
// z = log1p (x) for float complex
//------------------------------------------------------------------------------

inline GxB_FC32_t GB_clog1pf (GxB_FC32_t x)
{
    // FUTURE: this is not accurate
    // z = log (1+x)
    return (clogf (GxB_CMPLXF (1 + crealf (x), cimagf (x)))) ;
}

//------------------------------------------------------------------------------
// z = log1p (x) for double complex
//------------------------------------------------------------------------------

inline GxB_FC64_t GB_clog1p (GxB_FC64_t x)
{
    // FUTURE: this is not accurate
    // z = log (1+x)
    return (clog (GxB_CMPLX (creal (x) + 1, cimag (x)))) ;
}

//------------------------------------------------------------------------------
// z = log10 (x) for float complex
//------------------------------------------------------------------------------

// log_e (10) in single precision
#define GB_LOG10EF 2.3025851f

inline GxB_FC32_t GB_clog10f (GxB_FC32_t x)
{
    // z = log (x) / log (10)
    return (GB_FC32_div (clogf (x), GxB_CMPLXF (GB_LOG10EF, 0))) ;
}

//------------------------------------------------------------------------------
// z = log10 (x) for double complex
//------------------------------------------------------------------------------

// log_e (10) in double precision
#define GB_LOG10E 2.302585092994045901

inline GxB_FC64_t GB_clog10 (GxB_FC64_t x)
{
    // z = log (x) / log (10)
    return (GB_FC64_div (clog (x), GxB_CMPLX (GB_LOG10E, 0))) ;
}

//------------------------------------------------------------------------------
// z = log2 (x) for float complex
//------------------------------------------------------------------------------

// log_e (2) in single precision
#define GB_LOG2EF 0.69314718f

inline GxB_FC32_t GB_clog2f (GxB_FC32_t x)
{
    // z = log (x) / log (2)
    return (GB_FC32_div (clogf (x), GxB_CMPLXF (GB_LOG2EF, 0))) ;
}

//------------------------------------------------------------------------------
// z = log2 (x) for double complex
//------------------------------------------------------------------------------

// log_e (2) in double precision
#define GB_LOG2E 0.693147180559945286

inline GxB_FC64_t GB_clog2 (GxB_FC64_t x)
{
    // z = log (x) / log (2)
    return (GB_FC64_div (clog (x), GxB_CMPLX (GB_LOG2E, 0))) ;
}

//------------------------------------------------------------------------------
// z = isinf (x) for float complex
//------------------------------------------------------------------------------

inline bool GB_cisinff (GxB_FC32_t x)
{
    return (isinf (crealf (x)) || isinf (cimagf (x))) ;
}

//------------------------------------------------------------------------------
// z = isinf (x) for double complex
//------------------------------------------------------------------------------

inline bool GB_cisinf (GxB_FC64_t x)
{
    return (isinf (creal (x)) || isinf (cimag (x))) ;
}

//------------------------------------------------------------------------------
// z = isnan (x) for float complex
//------------------------------------------------------------------------------

inline bool GB_cisnanf (GxB_FC32_t x)
{
    return (isnan (crealf (x)) || isnan (cimagf (x))) ;
}

//------------------------------------------------------------------------------
// z = isnan (x) for double complex
//------------------------------------------------------------------------------

inline bool GB_cisnan (GxB_FC64_t x)
{
    return (isnan (creal (x)) || isnan (cimag (x))) ;
}

//------------------------------------------------------------------------------
// z = isfinite (x) for float complex
//------------------------------------------------------------------------------

inline bool GB_cisfinitef (GxB_FC32_t x)
{
    return (isfinite (crealf (x)) && isfinite (cimagf (x))) ;
}

//------------------------------------------------------------------------------
// z = isfinite (x) for double complex
//------------------------------------------------------------------------------

inline bool GB_cisfinite (GxB_FC64_t x)
{
    return (isfinite (creal (x)) && isfinite (cimag (x))) ;
}

#endif
