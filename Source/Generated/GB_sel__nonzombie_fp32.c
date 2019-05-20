//------------------------------------------------------------------------------
// GB_sel:  hard-coded functions for selection operators
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2019, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

#include "GB.h"
#include "GB_sel__include.h"

// The selection is defined by the following types and operators:

// phase1: GB_sel_phase1__nonzombie_fp32
// phase2: GB_sel_phase2__nonzombie_fp32

// A type:   float
// selectop: (GB_IS_NOT_ZOMBIE (Ai [p]))

// kind
#define GB_ENTRY_SELECTOR

#define GB_ATYPE \
    float

// test Ax [p]
#define GB_SELECT(p)                                    \
    (GB_IS_NOT_ZOMBIE (Ai [p]))

// get the vector index (user select operators only)
#define GB_GET_J                                        \
    ;

// workspace is a parameter to the function, not defined internally
#define GB_REDUCTION_WORKSPACE(W, ntasks) ;

// W [k] = s, no typecast
#define GB_COPY_SCALAR_TO_ARRAY(W,k,s)                  \
    W [k] = s

// W [k] = S [i], no typecast
#define GB_COPY_ARRAY_TO_ARRAY(W,k,S,i)                 \
    W [k] = S [i]

// W [k] += S [i], no typecast
#define GB_ADD_ARRAY_TO_ARRAY(W,k,S,i)                  \
    W [k] += S [i]

// no terminal value
#define GB_BREAK_IF_TERMINAL(t) ;

// ztype s = (ztype) Ax [p], with typecast
#define GB_CAST_ARRAY_TO_SCALAR(s,Ax,p)                 \
    int64_t s = GB_SELECT (p)

// s += (ztype) Ax [p], with typecast
#define GB_ADD_CAST_ARRAY_TO_SCALAR(s,Ax,p)             \
    s += GB_SELECT (p)

// Cx [pC] = Ax [pA], no typecast
#define GB_SELECT_ENTRY(Cx,pC,Ax,pA)                    \
    Cx [pC] = Ax [pA]

//------------------------------------------------------------------------------
// GB_sel_phase1__nonzombie_fp32
//------------------------------------------------------------------------------

#if 0

void GB_sel_phase1__nonzombie_fp32
(
    // output
    int64_t *restrict Zp,
    int64_t *restrict Cp,
    int64_t *restrict Wfirst,
    int64_t *restrict Wlast,
    // input
    const GrB_Matrix A,
    const int64_t *restrict kfirst_slice,
    const int64_t *restrict klast_slice,
    const int64_t *restrict pstart_slice,
    const bool flipij,
    const int64_t ithunk,
    const float *restrict xthunk,
    const GxB_select_function user_select,
    const int ntasks,
    const int nthreads
)
{
    int64_t *restrict Tx = Cp ;
    ;
    #include "GB_select_phase1.c"
}

#endif

//------------------------------------------------------------------------------
// GB_sel_exec
//------------------------------------------------------------------------------

void GB_sel_phase2__nonzombie_fp32
(
    // output
    int64_t *restrict Ci,
    float *restrict Cx,
    // input
    const int64_t *restrict Zp,
    const int64_t *restrict Cp,
    const int64_t *restrict C_pstart_slice,
    const GrB_Matrix A,
    const int64_t *restrict kfirst_slice,
    const int64_t *restrict klast_slice,
    const int64_t *restrict pstart_slice,
    const bool flipij,
    const int64_t ithunk,
    const float *restrict xthunk,
    const GxB_select_function user_select,
    const int ntasks,
    const int nthreads
)
{
    ;
    #include "GB_select_phase2.c"
}

