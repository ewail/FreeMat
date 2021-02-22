
//#include <iostream>
#include <string.h>
#include "MemPtr.hpp"
#include <stdio.h>

#define MAX(a,b) (((a) > (b)) ? (a) : (b))
extern "C"
{
  void zheev_(char *JOBZ, char *UPLO, int *N, double *A, int *LDA, 
	      double *W, double *WORK, int *LWORK, double *RWORK, int *INFO, int len);
  void cheev_(char *JOBZ, char *UPLO, int *N, float *A, int *LDA, 
	      float *W, float *WORK, int *LWORK, float *RWORK, int *INFO, int len);
}

void toogo() {
  for (int n=2;n<10;n++) {
    double *a = new double[2*n*n];
    memset(a,0,sizeof(double)*2*n*n);
    a[0] = 1;
    a[2] = 1;
    a[3] = -1;
    a[4] = 1;
    a[5] = 1;
    a[6] = 1;
    char JOBZ = 'N';
    char UPLO = 'U';
    int N = n;
    int LDA = n;
    MemBlock<double> RWORK(MAX(1,3*N-2));
    int LWORK;
    int INFO;
    double WORKSZE[2];
    LWORK = -1;
    double *d = new double[n];
    zheev_(&JOBZ,&UPLO,&N,a,&LDA,d,WORKSZE,&LWORK,&RWORK,&INFO,1);
    LWORK = (int) WORKSZE[0];
    printf("LWORK = %d\n",LWORK);
    MemBlock<double> WORK(2*LWORK);
    zheev_(&JOBZ,&UPLO,&N,a,&LDA,d,&WORK,&LWORK,&RWORK,&INFO,1);
    for (int i=0;i<n;i++)  {
      printf("eig[%d] = %f\n",i,d[i]);
    }
    delete a;
    delete d;
  }
}

void toogo2() {
  for (int n=2;n<10;n++) {
    float *a = new float[2*n*n];
    memset(a,0,sizeof(float)*2*n*n);
    a[0] = 1;
    a[2] = 1;
    a[3] = -1;
    a[4] = 1;
    a[5] = 1;
    a[6] = 1;
    char JOBZ = 'N';
    char UPLO = 'U';
    int N = n;
    int LDA = n;
    MemBlock<float> RWORK(MAX(1,3*N-2));
    int LWORK;
    int INFO;
    float WORKSZE[2];
    LWORK = -1;
    float *d = new float[n];
    cheev_(&JOBZ,&UPLO,&N,a,&LDA,d,WORKSZE,&LWORK,&RWORK,&INFO,1);
    LWORK = (int) WORKSZE[0];
    printf("LWORK = %d\n",LWORK);
    MemBlock<float> WORK(2*LWORK);
    cheev_(&JOBZ,&UPLO,&N,a,&LDA,d,&WORK,&LWORK,&RWORK,&INFO,1);
    for (int i=0;i<n;i++)  {
      printf("eig[%d] = %f\n",i,d[i]);
    }
    delete a;
    delete d;
  }
}

int main(int argc, char*argv[]) {
  toogo2();
  toogo();
  return 0;
}
