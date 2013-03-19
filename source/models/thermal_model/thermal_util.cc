
#include <math.h>
#include <cassert>

#include "source/models/thermal_model/thermal_util.h"
#include "source/misc/misc.h"
#include "libutil/LibUtil.h"

#define LOWER(a, i, j)		((i > j) ? a[i][j] : 0)
#define UPPER(a, i, j)		((i <= j) ? a[i][j] : 0)

using std::vector;

namespace Thermal
{
    
    // LUP decomposition from the pseudocode given in the CLR 
    // 'Introduction to Algorithms' textbook. The matrix 'a' is
    // transformed into an in-place lower/upper triangular matrix
    // and the vector'p' carries the permutation vector such that
    // Pa = lu, where 'P' is the matrix form of 'p'.
    void ThermalUtil::lupDecomposition(vector< vector<double> >& a, int n, vector<int>& p)
    {
        int i, j, k, pivot=0;
        double max = 0;
    
        /* start with identity permutation  */
        for (i=0; i < n; i++)
            p[i] = i;
    
        for (k=0; k < n-1; k++)  
        {
            max = 0;
            for (i = k; i < n; i++) 
            {
                if (fabs(a[i][k]) > max) 
                {
                    max = fabs(a[i][k]);
                    pivot = i;
                }
            }   
            if (Misc::eq (max, 0))
                LibUtil::Log::printFatalLine(std::cerr, "\nsingular matrix in LUP Decomposition\n");
    
            // bring pivot element to position
            Misc::swapIntValue (&p[k], &p[pivot]);
            for (i=0; i < n; i++)
                Misc::swapDoubleValue(&a[k][i], &a[pivot][i]);
    
            for (i=k+1; i < n; i++) 
            {
                a[i][k] /= a[k][k];
                for (j=k+1; j < n; j++)
                    a[i][j] -= a[i][k] * a[k][j];
            }
        }
    }

    // LU forward and backward substitution from the pseudocode given
    // in the CLR 'Introduction to Algorithms' textbook. It solves ax = b
    // where, 'a' is an in-place lower/upper triangular matrix. The vector
    // 'x' carries the solution vector. 'p' is the permutation vector.
    void ThermalUtil::luSolve(vector< vector<double> >& a, int n, vector<int>& p, vector<double>& b, vector<double>& x)
    {
        int i, j;
        vector<double> y (n, 0);
        double sum;
    
        /* forward substitution - solves ly = pb    */
        for (i=0; i < n; i++) {
            for (j=0, sum=0; j < i; j++)
                sum += y[j] * LOWER(a, i, j);
            y[i] = b[p[i]] - sum;
        }
    
        /* backward substitution - solves ux = y    */
        for (i=n-1; i >= 0; i--) {
            for (j=i+1, sum=0; j < n; j++)
                sum += x[j] * UPPER(a, i, j);
            x[i] = (y[i] - sum) / UPPER(a, i, i);
        }
    } // luSolve
    
} // namespace Thermal

