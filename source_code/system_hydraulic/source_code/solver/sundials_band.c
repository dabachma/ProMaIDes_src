/*
 * -----------------------------------------------------------------
 * $Revision: 1.6 $
 * $Date: 2009/02/17 02:42:29 $
 * -----------------------------------------------------------------
 * Programmer(s): Alan C. Hindmarsh and Radu Serban @ LLNL
 * -----------------------------------------------------------------
 * Copyright (c) 2002, The Regents of the University of California.
 * Produced at the Lawrence Livermore National Laboratory.
 * All rights reserved.
 * For details, see the LICENSE file.
 * -----------------------------------------------------------------
 * This is the implementation file for a generic BAND linear
 * solver package.
 * -----------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>

#include <sundials_band.h>
#include <sundials_math.h>
#include <omp.h>



#define ZERO RCONST(0.0)
#define ONE  RCONST(1.0)

#define ROW(i,j,smu) (i-j+smu)

/*
 * -----------------------------------------------------
 * Functions working on DlsMat
 * -----------------------------------------------------
 */

int BandGBTRF(DlsMat A, int *p)
{
  return(bandGBTRF(A->cols, A->M, A->mu, A->ml, A->s_mu, p));
}
void BandGBTRS(DlsMat A, int *p, realtype *b)
{
  bandGBTRS(A->cols, A->M, A->s_mu, A->ml, p, b);
}


void BandCopy(DlsMat A, DlsMat B, int copymu, int copyml)
{
  bandCopy(A->cols, B->cols, A->M, A->s_mu, B->s_mu, copymu, copyml);
}

//Changed Daniel 26.6.2013
//copy optimized jacobi-matrix
void BandCopyJ(DlsMat A, DlsMat B, int copymu, int copyml,int *r_id, int neq, realtype c)
{
  bandCopyJ(A->cols, B->cols, A->M, A->s_mu, B->s_mu, copymu, copyml,r_id, neq, c);
}
//Changed Daniel 22.10.2014
//copy optimized jacobi-matrix to ilu-matrix
void BandCopyJ_ilu(DlsMat A, DlsMat B, int copymu, int copyml,int *r_id, int neq, realtype c)
{
  bandCopyJ_ilu(A->cols, B->cols, A->M, A->s_mu, B->s_mu, copymu, copyml,r_id, neq, c);
}

void BandScale(realtype c, DlsMat A)
{
  bandScale(c, A->cols, A->M, A->mu, A->ml, A->s_mu);
}

//Changed Daniel 26.6.2013
//scale optimized jacobi-matrix
void BandScaleJ(realtype c, DlsMat A)
{
  bandScaleJ(c, A->cols, A->M, A->mu, A->ml, A->s_mu);
}


/*
 * -----------------------------------------------------
 * Functions working on realtype**
 * -----------------------------------------------------
 */

int bandGBTRF(realtype **a, int n, int mu, int ml, int smu, int *p)
{
  int c, r, num_rows, n_minus;
  int i, j, k, l, storage_l, storage_k, last_col_k, last_row_k;
  realtype *a_c, *col_k, *diag_k, *sub_diag_k, *col_j, *kptr, *jptr;
  realtype max, temp, mult, a_kj;
  booleantype swap;
  realtype abs_buff;
 
  /* zero out the first smu - mu rows of the rectangular array a */

  n_minus=n-1;
  num_rows = smu - mu;
  if (num_rows > 0) {
    for (c=0; c < n; c++) {
      a_c = a[c];
      for (r=0; r < num_rows; r++) {
		a_c[r] = ZERO;
      }
    }
  }

  /* k = elimination step number */
 
  for (k=0; k < n_minus; k++, p++) {
    
    col_k     = a[k];
    diag_k    = col_k + smu;
    sub_diag_k = diag_k + 1;
    last_row_k = MIN(n_minus,k+ml);

    /* find l = pivot row number */

    l=k;
    max = ABS(*diag_k);
	
    for (i=k+1, kptr=sub_diag_k; i <= last_row_k; i++, kptr++) {
		
	  abs_buff=ABS(*kptr);
	  if (abs_buff > max) {
		l=i;
		max = abs_buff;
      }
    }
    storage_l = ROW(l, k, smu);
    *p = l;
    
    /* check for zero pivot element */

    if (col_k[storage_l] == ZERO) return(k+1);
    
    /* swap a(l,k) and a(k,k) if necessary */
    
    if ( (swap = (l != k) )) {
      temp = col_k[storage_l];
      col_k[storage_l] = *diag_k;
      *diag_k = temp;
    }

    /* Scale the elements below the diagonal in         */
    /* column k by -1.0 / a(k,k). After the above swap, */
    /* a(k,k) holds the pivot element. This scaling     */
    /* stores the pivot row multipliers -a(i,k)/a(k,k)  */
    /* in a(i,k), i=k+1, ..., MIN(n-1,k+ml).            */
    
    mult = -ONE / (*diag_k);
    for (i=k+1, kptr = sub_diag_k; i <= last_row_k; i++, kptr++){
		if((*kptr)!=0.0){
			(*kptr) *= mult;
		}

	}


    /* row_i = row_i - [a(i,k)/a(k,k)] row_k, i=k+1, ..., MIN(n-1,k+ml) */
    /* row k is the pivot row after swapping with row l.                */
    /* The computation is done one column at a time,                    */
    /* column j=k+1, ..., MIN(k+smu,n-1).                               */
    
    last_col_k = MIN(k+smu,n_minus);
    for (j=k+1; j <= last_col_k; j++) {
      
      col_j = a[j];
      storage_l = ROW(l,j,smu); 
      storage_k = ROW(k,j,smu); 
      a_kj = col_j[storage_l];

      /* Swap the elements a(k,j) and a(k,l) if l!=k. */
      if (swap) {
		col_j[storage_l] = col_j[storage_k];
		col_j[storage_k] = a_kj;
      }

      /* a(i,j) = a(i,j) - [a(i,k)/a(k,k)]*a(k,j) */
      /* a_kj = a(k,j), *kptr = - a(i,k)/a(k,k), *jptr = a(i,j) */

	  if (a_kj != ZERO) {
		for (i=k+1, kptr=sub_diag_k, jptr=col_j+ROW(k+1,j,smu);
			 i <= last_row_k;
			 i++, kptr++, jptr++)
				(*jptr) += a_kj * (*kptr);

	  }
	}  
  }
  
  
  
  /* set the last pivot row to be n-1 and check for a zero pivot */

  *p = n_minus; 
  if (a[n_minus][smu] == ZERO) return(n);

  /* return 0 to indicate success */

  return(0);
}

void bandGBTRS(realtype **a, int n, int smu, int ml, int *p, realtype *b)
{
  int k, l, i, first_row_k, last_row_k, n_minus;
  realtype mult, *diag_k;
  n_minus=n-1;
  
  /* Solve Ly = Pb, store solution y in b */
  
  for (k=0; k < n_minus; k++) {
    l = p[k];
    mult = b[l];
    if (l != k) {
      b[l] = b[k];
      b[k] = mult;
    }
	if(mult!=0.0){//optimized by Bachmann
		diag_k = a[k]+smu;
		last_row_k = MIN(n_minus,k+ml);
		for (i=k+1; i <= last_row_k; i++)
		b[i] += mult * diag_k[i-k];
	}
  }
  
  /* Solve Ux = y, store solution x in b */
  for (k=n_minus; k >= 0; k--) {
	if(b[k]!=0.0){//optimized by Bachmann
		diag_k = a[k]+smu;
		first_row_k = MAX(0,k-smu);
		b[k] /= (*diag_k);
		mult = -b[k];
		for (i=first_row_k; i <= k-1; i++)
		 b[i] += mult*diag_k[i-k];
	}
  }
}

//Changed Daniel 22.10.2014
//for ilu-matrix
int bandGBTRF_ilu(realtype **a, int mu,  int *r_id, int neq)
{

  int  k,  id, id1;
//  For k=1,…,n?1, do (splaten)
//     For i=k+1,…,n and if (i,k)?P, do (zeielen
//        mik:=mik/mkk
//        For j=k+1,…,n and if (i,j)?P, do (spalten
//           mij:=mij?mikmkj

       for(k=0;k<neq-1;k++){//spalten
           id=r_id[k];
           if(id>=0){
           //zeilen
            //normal id=k;id1=k+1
               id1=r_id[k+1];
               if(id1>=0){
                 a[id1][1]=(a[id1])[1]/a[id][2];
                     a[id1][2]=a[id1][2]-a[id1][1]*a[id][3];
               }

               if(k+mu<neq){
                   //normal id=k;id1=k+mu
                   id1=r_id[k+mu];
                   if(id1>=0){
                     a[id1][0]=a[id1][0]/a[id][2];
                        a[id1][2]=a[id1][2]-a[id1][0]*a[id][4];
                   }

               }
           }
       }
 /* return 0 to indicate success */
  return(0);
}
//Changed Daniel 22.10.2014
//for ilu-matrix
void bandGBTRS_ilu(realtype **a,  int ml, realtype *b, int *r_id, int neq)
{
  // b=result vector
  int k, id,id1;
  realtype mult, *diag_k;


  /* Solve Ly = Pb, store solution y in b */

  for (k=0; k < neq-1; k++) {
     id=r_id[k];
     if(id>=0){
        mult = -b[id];
        if(mult!=0.0){//optimized by Bachmann
            id1=k+1;
            id1=r_id[id1];
            if(id1>=0){
                diag_k=a[id1];
                b[id1] += mult * diag_k[1];
            }
            id1=k+ml;
            if(id1<neq){
                id1=r_id[id1];
                if(id1>=0){
                    diag_k=a[id1];
                     b[id1] += mult * diag_k[0];
                }
            }
        }
     }
  }

  /* Solve Ux = y, store solution x in b */
  for (k=neq-1; k >=0; k--) {
      id=r_id[k];
      if(id>=0){
          diag_k=a[id];
          b[id]/=diag_k[2];
          mult = -b[id];
          if(mult!=0.0){//optimized by Bachmann
              id1=k-1;
              if(id1>=0){
                id1=r_id[id1];
                if(id1>=0){
                    diag_k=a[id1];
                     b[id1] += mult * diag_k[3];
                }
              }
              id1=k-ml;
              if(id1>=0){
                  id1=r_id[id1];
                  if(id1>=0){
                      diag_k=a[id1];
                       b[id1] += mult * diag_k[4];
                  }
              }
          }

       }
  }
}


void bandCopy(realtype **a, realtype **b, int n, int a_smu, int b_smu, 
              int copymu, int copyml)
{
  int i, j, copySize, a_buff, b_buff;
  realtype *a_col_j, *b_col_j;

  copySize = copymu + copyml + 1;
  a_buff=a_smu-copymu;
  b_buff=b_smu-copymu;
 
  for (j=0; j < n; j++) {
    a_col_j = a[j]+a_buff;
    b_col_j = b[j]+b_buff;
    for (i=0; i < copySize; i++)
      b_col_j[i] = a_col_j[i];
  }
}

//Changed Daniel 26.6.2013
//copy optimized jacobi-matrix
void bandCopyJ(realtype **a, realtype **b, int n, int a_smu, int b_smu, 
              int copymu, int copyml, int *r_id, int neq, realtype c)
{
  int i, j, copySize, b_buff, id, id1;
  realtype *a_col_j, *b_col_j;

  copySize = copymu + copyml + 1;
  //a_buff=a_smu-copymu;
  b_buff=b_smu-copymu;
 
  //set to zero
  for (j=0; j < n; j++) {
    b_col_j = b[j]+b_buff;
    for (i=0; i < copySize; i++)
      b_col_j[i] = ZERO;
  }

  //set values
  for (j=0; j < neq; j++) {
      id=r_id[j];
     if(id>=0){
        a_col_j = a[id];
        b_col_j = b[id]+b_buff;

        b_col_j[copyml] = a_col_j[2]*c;

        id1=j-copyml;
        if(id1>=0){
            id1=r_id[id1];
            if(id1>=0){
                b_col_j[copyml-(id-id1)] = a_col_j[0]*c;
            }
        }

        id1=j-1;
        if(id1>=0){
            id1=r_id[id1];
            if(id1>=0){
                 b_col_j[copyml-1] = a_col_j[1]*c;
            }
        }
        id1=j+1;
        if(id1<neq){
            id1=r_id[id1];
            if(id1>=0){
                 b_col_j[copyml+1] = a_col_j[3]*c;
            }
        }

        id1=j+copyml;
        if(id1<neq){
            id1=r_id[id1];
            if(id1>=0){
                 b_col_j[copyml+(id1-id)] = a_col_j[4]*c;
            }
        }
     }

  }
//  for (j=0; j < n; j++) {
//    a_col_j = a[j];
//    b_col_j = b[j]+b_buff;
//    //for (i=0; i < 5; i++)
//      //b_col_j[i] = a_col_j[i];
//    b_col_j[0] = a_col_j[0];
//    b_col_j[copyml-1] = a_col_j[1];
//    b_col_j[copyml] = a_col_j[2];
//    b_col_j[copyml+1] = a_col_j[3];
//    b_col_j[copySize-1] = a_col_j[4];

//  }




}
//Changed Daniel 22.10.2014
//copy optimized jacobi-matrix to ilu-matrix
void bandCopyJ_ilu(realtype **a, realtype **b, int n, int a_smu, int b_smu,
              int copymu, int copyml, int *r_id, int neq, realtype c)
{
  int i, j, copySize, b_buff, id, id1;
  realtype *a_col_j, *b_col_j;

  //copySize = copymu + copyml + 1;
  //a_buff=a_smu-copymu;
  //b_buff=b_smu-copymu;

  //set to zero
//  for (j=0; j < n; j++) {
//    b_col_j = b[j];
//    for (i=0; i < 5; i++)
//      b_col_j[i] = ZERO;
//    }
//  omp_set_num_threads(3);
// #pragma omp parallel
//  {
// #pragma omp for
  for (j=0; j < n; j++) {

    b_col_j = b[j];
    a_col_j = a[j];
    for (i=0; i < 5; i++){
      b_col_j[i] = a_col_j[i]*c;
    }
      b_col_j[2] += ONE;
  }
  //}


  //set values
//  for (j=0; j < neq; j++) {
//      id=r_id[j];
//     if(id>=0){
//        a_col_j = a[id];
//        b_col_j = b[id];
//        //for (i=0; i < 5; i++)
//          //b_col_j[i] = a_col_j[i];
//        b_col_j[2] = a_col_j[2]*c;
//        //b_col_j[0] = a_col_j[0];
//        //b_col_j[copyml-1] = a_col_j[1];
//        //b_col_j[copyml+1] = a_col_j[3];
//        //b_col_j[copySize-1] = a_col_j[4];

//        id1=j-copyml;
//        if(id1>=0){
//            id1=r_id[id1];
//            if(id1>=0){
//                b_col_j[0] = a_col_j[0]*c;
//            }
//        }

//        id1=j-1;
//        if(id1>=0){
//            id1=r_id[id1];
//            if(id1>=0){
//                 b_col_j[1] = a_col_j[1]*c;
//            }
//        }
//        id1=j+1;
//        if(id1<neq){
//            id1=r_id[id1];
//            if(id1>=0){
//                 b_col_j[3] = a_col_j[3]*c;
//            }
//        }

//        id1=j+copyml;
//        if(id1<neq){
//            id1=r_id[id1];
//            if(id1>=0){
//                 b_col_j[4] = a_col_j[4]*c;
//            }
//        }
//     }

//  }
}


void bandScale(realtype c, realtype **a, int n, int mu, int ml, int smu)
{
  int i, j, colSize, s_buff;
  realtype *col_j;

  colSize = mu + ml + 1;
  s_buff=smu-mu;

  for(j=0; j < n; j++) {
    col_j = a[j]+s_buff;
    for (i=0; i < colSize; i++){
    	 col_j[i] *= c;
	}
  }
}
//Changed Daniel 26.6.2013
//scale optimized jacobi-matrix
void bandScaleJ(realtype c, realtype **a, int n, int mu, int ml, int smu)
{
  int i, j, buff, copySize;
  realtype *col_j;

   copySize = mu + ml + 1;
  //a_buff=a_smu-copymu;
  buff=smu-mu;
 



  //set values
  for (j=0; j < n; j++) {
    col_j = a[j]+buff;
    //for (i=0; i < 5; i++)
      //b_col_j[i] = a_col_j[i];
	col_j[0] *= c;
	col_j[ml-1] *= c;
	col_j[ml] *= c;
	col_j[ml+1] *= c;
	col_j[copySize-1] *= c;

  }

}
void bandAddIdentity(realtype **a, int n, int smu)
{
  int j;
 
  for(j=0; j < n; j++)
    a[j][smu] += ONE;
}
