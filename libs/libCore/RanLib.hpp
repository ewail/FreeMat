/*
 * Copyright (c) 2002-2006 Samit Basu
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#ifndef __RanLib_hpp__
#define __RanLib_hpp__

void advnst(long k);
float genbet(float aa,float bb);
float genchi(float df);
float genexp(float av);
float genf(float dfn, float dfd);
float gengam(float a,float r);
void genmn(float *parm,float *x,float *work);
void genmul(long n,float *p,long ncat,long *ix);
float gennch(float df,float xnonc);
float gennf(float dfn, float dfd, float xnonc);
float gennor(float av,float sd);
void genprm(long *iarray,int larray);
float genunf(float low,float high);
void getsd(long *iseed1,long *iseed2);
void gscgn(long getset,long *g);
long ignbin(long n,float pp);
long ignnbn(long n,float p);
long ignlgi(void);
long ignpoi(float mu);
long ignuin(long low,long high);
void initgn(long isdtyp);
long mltmod(long a,long s,long m);
void phrtsd(char* phrase,long* seed1,long* seed2);
float ranf(void);
void setall(long iseed1,long iseed2);
void setant(long qvalue);
void setgmn(float *meanv,float *covm,long p,float *parm);
void setsd(long iseed1,long iseed2);
float sexpo(void);
float sgamma(float a);
float snorm(void);
void init_genrand(unsigned long s);
void init_by_array(unsigned long init_key[], unsigned long key_length);
unsigned long genrand_int32(void);
long genrand_int31(void);
double genrand_real1(void);
double genrand_real2(void);
double genrand_real3(void);
double genrand_res53(void);
void GetRandStateVect(unsigned int*);
void SetRandStateVect(unsigned int*);
#endif
