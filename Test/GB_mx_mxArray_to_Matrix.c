//------------------------------------------------------------------------------
// GB_mx_mxArray_to_Matrix
//------------------------------------------------------------------------------

// SuiteSparse:GraphBLAS, Timothy A. Davis, (c) 2017-2020, All Rights Reserved.
// http://suitesparse.com   See GraphBLAS/Doc/License.txt for license.

//------------------------------------------------------------------------------

// Convert a MATLAB sparse matrix or struct to a GraphBLAS sparse matrix.  The
// mxArray is either a struct containing two terms: a sparse matrix or vector,
// and type (a string, "logical", "double", etc), or it is just a plain sparse
// matrix.  If A.class is present, it is used to typecast the MATLAB matrix
// into the corresponding type in GraphBLAS.

// That is:
// A = sparse (...) ;   % a sparse double or logical GraphBLAS matrix

// A.matrix = A ; A.class = 'int8' ; Represents  A MATLAB sparse matrix that
// represents a GraphBLAS int8 matrix.  On input, the MATLAB sparse matrix is
// typecasted.

// The MATLAB matrix or struct is not modified.  If deep_copy is true, the
// GraphBLAS matrix is always a deep copy and can be modified by GraphBLAS.
// Otherwise, its pattern (A->p, A->h, and A->i) may be a shallow copy, and
// A->x is a shallow copy if the MATLAB matrix is 'logical' or 'double'. 

// If the MATLAB matrix is double complex, it becomes a GraphBLAS
// Complex or GxB_FC64 matrix.

// A->x is always a deep copy for other types, since it must be typecasted from
// MATLAB to GraphBLAS.

// Like GB_mx_Matrix_to_mxArray, this could be done using only user-callable
// GraphBLAS functions, but the method used here is faster.

#include "GB_mex.h"

#define FREE_ALL            \
{                           \
    GB_MATRIX_FREE (&A) ;   \
}

GrB_Matrix GB_mx_mxArray_to_Matrix     // returns GraphBLAS version of A
(
    const mxArray *A_matlab,            // MATLAB version of A
    const char *name,                   // name of the argument
    bool deep_copy,                     // if true, return a deep copy
    const bool empty    // if false, 0-by-0 matrices are returned as NULL.
                        // if true, a 0-by-0 matrix is returned.
)
{

    //--------------------------------------------------------------------------
    // check for empty matrix
    //--------------------------------------------------------------------------

    GB_WHERE ("mxArray_to_Matrix") ;

    GrB_Matrix A = NULL ;

    if (A_matlab == NULL)
    {
        // input is not present; this is not an error if A is an
        // optional input
        return (NULL) ;
    }

    if ((mxGetM (A_matlab) == 0) && (mxGetN (A_matlab) == 0))
    {
        // input is "[ ]", zero-by-zero.
        if (empty)
        {
            // treat as a sparse 0-by-0 matrix, not NULL
            GrB_Matrix_new (&A, GrB_FP64, 0, 0) ;
            ASSERT_MATRIX_OK (A, "got A = [ ] from MATLAB", GB0) ;
            return (A) ;
        }
        else
        {
            // Treat as NULL in GraphBLAS.  Useful for mask matrices
            return (NULL) ;
        }
    }

    //--------------------------------------------------------------------------
    // get the matrix
    //--------------------------------------------------------------------------


    const mxArray *Amatrix = NULL ;
    GrB_Type atype_in, atype_out ;
    GB_Type_code atype_in_code, atype_out_code ;

    if (mxIsStruct (A_matlab))
    {
        // look for A.matrix
        int fieldnumber = mxGetFieldNumber (A_matlab, "matrix") ;
        if (fieldnumber >= 0)
        {
            Amatrix = mxGetFieldByNumber (A_matlab, 0, fieldnumber) ;
        }
        else
        {
            // A.matrix not present, try A.vector
            fieldnumber = mxGetFieldNumber (A_matlab, "vector") ;
            if (fieldnumber < 0)
            {
                FREE_ALL ;
                mexWarnMsgIdAndTxt ("GB:warn", "invalid matrix/vector struct") ;
                return (NULL) ;
            }
            Amatrix = mxGetFieldByNumber (A_matlab, 0, fieldnumber) ;
            if (mxGetN (Amatrix) != 1)
            {
                FREE_ALL ;
                mexWarnMsgIdAndTxt ("GB:warn", "vector must be n-by-1") ;
                return (NULL) ;
            }
        }

        // get the type
        ASSERT (Amatrix != NULL) ;

        atype_in = GB_mx_Type (Amatrix) ;
        atype_out = atype_in ;
        fieldnumber = mxGetFieldNumber (A_matlab, "class") ;
        if (fieldnumber >= 0)
        {
            mxArray *s = mxGetFieldByNumber (A_matlab, 0, fieldnumber) ;
            atype_out = GB_mx_string_to_Type (s, atype_in) ;
        }
    }
    else
    {
        // just a matrix
        Amatrix = A_matlab ;
        atype_in = GB_mx_Type (Amatrix) ;
        atype_out = atype_in ;
    }

    if (!mxIsSparse (Amatrix))
    {
        FREE_ALL ;
        mexWarnMsgIdAndTxt ("GB:warn", "input matrix must be sparse") ;
        return (NULL) ;
    }

    //--------------------------------------------------------------------------
    // get the matrix type
    //--------------------------------------------------------------------------

    atype_in_code  = atype_in->code ;
    atype_out_code = atype_out->code ;

    //--------------------------------------------------------------------------
    // get the size and content of the MATLAB matrix
    //--------------------------------------------------------------------------

    int64_t nrows = mxGetM (Amatrix) ;
    int64_t ncols = mxGetN (Amatrix) ;
    int64_t *Mp = (int64_t *) mxGetJc (Amatrix) ;
    int64_t *Mi = (int64_t *) mxGetIr (Amatrix) ;
    int64_t anz = Mp [ncols] ;
    GB_void *Mx = mxGetData (Amatrix) ;     // OK:any type
    int64_t anzmax = mxGetNzmax (Amatrix) ;

    //--------------------------------------------------------------------------
    // look for A.values
    //--------------------------------------------------------------------------

    if (mxIsStruct (A_matlab))
    {
        // this is used for int64 and uint64 only
        int fieldnumber = mxGetFieldNumber (A_matlab, "values") ;
        if (fieldnumber >= 0)
        {
            mxArray *values = mxGetFieldByNumber (A_matlab, 0, fieldnumber) ;
            if (mxIsComplex (values))
            {
                mexErrMsgTxt ("A.values must be real") ;
            }
            if (mxGetNumberOfElements (values) >= anz)
            {
                Mx = mxGetData (values) ;       // OK:any type
                atype_in = GB_mx_Type (values) ;
                atype_in_code = atype_in->code ;
                anzmax = mxGetNumberOfElements (values) ;
            }
        }
    }

    ASSERT_TYPE_OK (atype_in,  "A type in", GB0) ;
    ASSERT_TYPE_OK (atype_out, "A type out", GB0) ;

    if (atype_in == NULL || atype_out == NULL)
    {
        FREE_ALL ;
        mexWarnMsgIdAndTxt ("GB:warn", "types must be numeric") ;
        return (NULL) ;
    }

    GrB_Info info ;

    // MATLAB matrices are non-hypersparse CSC
    bool is_csc = true ;
    bool is_hyper = false ;

    //--------------------------------------------------------------------------
    // get the pattern of A
    //--------------------------------------------------------------------------

    if (deep_copy)
    {

        // create the GraphBLAS matrix
        info = GB_new (&A, atype_out, (GrB_Index) nrows, (GrB_Index) ncols,
            GB_Ap_calloc, is_csc, is_hyper, GB_HYPER_DEFAULT, 0, Context) ;
        if (info != GrB_SUCCESS)
        {
            FREE_ALL ;
            mexWarnMsgIdAndTxt ("GB:warn", "new deep matrix failed") ;
            return (NULL) ;
        }

        // A is a deep copy and can be modified by GraphBLAS
        info = GB_ix_alloc (A, anz, true, Context) ;
        if (info != GrB_SUCCESS)
        {
            FREE_ALL ;
            mexWarnMsgIdAndTxt ("GB:warn", "out of memory") ;
            return (NULL) ;
        }

        memcpy (A->p, Mp, (ncols+1) * sizeof (int64_t)) ;
        memcpy (A->i, Mi, anz * sizeof (int64_t)) ;
        A->magic = GB_MAGIC ;

    }
    else
    {

        // the GraphBLAS pattern (A->p and A->i) are pointers into the
        // MATLAB matrix and must not be modified.

        // [ create the GraphBLAS matrix, do not allocate A->p
        info = GB_new (&A, atype_out, (GrB_Index) nrows, (GrB_Index) ncols,
            GB_Ap_null, is_csc, is_hyper, GB_HYPER_DEFAULT, 0, Context) ;
        if (info != GrB_SUCCESS)
        {
            FREE_ALL ;
            mexWarnMsgIdAndTxt ("GB:warn", "new shallow matrix failed") ;
            return (NULL) ;
        }

        A->p = Mp ;
        A->magic = GB_MAGIC ;       // A->p now initialized ]
        A->i = Mi ;
        A->p_shallow = true ;
        A->h_shallow = false ;      // A->h is NULL
        A->i_shallow = true ;
    }

    //--------------------------------------------------------------------------
    // copy the numerical values from MATLAB to the GraphBLAS matrix
    //--------------------------------------------------------------------------

    A->x_shallow = (!deep_copy &&
           ((atype_out_code == GB_BOOL_code ||
             atype_out_code == GB_FP64_code ||
             atype_out_code == GB_FC64_code)
         && (atype_out_code == atype_in_code))) ;

    if (A->x_shallow)
    {
        // the MATLAB matrix and GraphBLAS matrix have the same type; (logical,
        // double, or double complex), and a deep copy is not requested.  Just
        // make a shallow copy.
        A->nzmax = anzmax ;
        A->x = Mx ;
    }
    else
    {
        if (!deep_copy)
        {
            // allocate new space for the GraphBLAS values
            A->nzmax = GB_IMAX (anz, 1) ;
            A->x = GB_MALLOC (A->nzmax * atype_out->size, GB_void) ;
            if (A->x == NULL)
            {
                FREE_ALL ;
                mexWarnMsgIdAndTxt ("GB:warn", "out of memory") ;
                return (NULL) ;
            }
        }

        GB_cast_array (
            A->x,
            (atype_out_code == GB_UDT_code) ? GB_FC64_code : atype_out_code,
            Mx,
            (atype_in_code == GB_UDT_code) ? GB_FC64_code : atype_in_code,
            (atype_in_code == GB_UDT_code) ? sizeof(GxB_FC64_t) : atype_in->size,
            anz, 1) ;
    }

    //--------------------------------------------------------------------------
    // look for CSR/CSC and hyper/non-hyper format
    //--------------------------------------------------------------------------

    bool has_hyper_ratio = false ;
    double hyper_ratio = GB_HYPER_DEFAULT ;

    if (mxIsStruct (A_matlab))
    {
        // look for A.is_csc
        int fieldnumber = mxGetFieldNumber (A_matlab, "is_csc") ;
        if (fieldnumber >= 0)
        {
            is_csc = mxGetScalar (mxGetFieldByNumber (A_matlab,
                0, fieldnumber)) ;
        }

        // look for A.is_hyper (ignored if hyper_ratio present)
        fieldnumber = mxGetFieldNumber (A_matlab, "is_hyper") ;
        if (fieldnumber >= 0)
        {
            is_hyper = mxGetScalar (mxGetFieldByNumber (A_matlab,
                0, fieldnumber)) ;
        }

        // look for A.hyper_ratio
        fieldnumber = mxGetFieldNumber (A_matlab, "hyper_ratio") ;
        if (fieldnumber >= 0)
        {
            has_hyper_ratio = true ;
            hyper_ratio = mxGetScalar (mxGetFieldByNumber (A_matlab,
                0, fieldnumber)) ;
        }
    }

    //--------------------------------------------------------------------------
    // compute the # of non-empty vectors in A only when needed
    //--------------------------------------------------------------------------

    A->nvec_nonempty = -1 ; // compute when needed; see also GxB_Matrix_import

    ASSERT_MATRIX_OK (A, "got natural A from MATLAB", GB0) ;
    ASSERT (!A->is_hyper) ;

    //--------------------------------------------------------------------------
    // convert to CSR if requested
    //--------------------------------------------------------------------------

    int64_t nrows_old = GB_NROWS (A) ;
    int64_t ncols_old = GB_NCOLS (A) ;

    if (!is_csc)
    {
        // this might convert A to hypersparse
        GxB_Matrix_Option_set_(A, GxB_FORMAT, GxB_BY_ROW) ;
        // so convert it back; hypersparsity is defined below
        GB_to_nonhyper (A, Context) ;
        ASSERT (!A->is_csc) ;
    }

    ASSERT_MATRIX_OK (A, "conformed from MATLAB", GB0) ;
    ASSERT (!A->is_hyper) ;
    ASSERT (A->is_csc == is_csc) ;

    //--------------------------------------------------------------------------
    // convert to hypersparse or set hypersparse ratio, if requested
    //--------------------------------------------------------------------------

    if (has_hyper_ratio)
    {
        // this sets the hyper_ratio and then conforms the matrix to its
        // desired hypersparsity.  It may stay non-hypersparse.
        GxB_Matrix_Option_set_(A, GxB_HYPER, hyper_ratio) ;
    }
    else if (is_hyper)
    {
        // this forces the matrix to be always hypersparse
        GxB_Matrix_Option_set_(A, GxB_HYPER, GxB_ALWAYS_HYPER) ;
        if (A->vdim > 1)
        {
            ASSERT (A->is_hyper == is_hyper) ;
        }
        else
        {
            // column vectors are never hypersparse
            ASSERT (!A->is_hyper) ;
        }
    }

    ASSERT_MATRIX_OK (A, "final hyper/nonhyper", GB0) ;
    ASSERT (A->is_csc == is_csc) ;
    ASSERT (nrows_old == GB_NROWS (A)) ;
    ASSERT (ncols_old == GB_NCOLS (A)) ;

    //--------------------------------------------------------------------------
    // return the GraphBLAS matrix
    //--------------------------------------------------------------------------

    ASSERT_MATRIX_OK (A, "got A from MATLAB", GB0) ;
    return (A) ;
}

