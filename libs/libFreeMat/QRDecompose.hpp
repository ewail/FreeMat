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

#ifndef __QRDecompose_hpp__
#define __QRDecompose_hpp__

void floatQRD(int nrows, int ncols, float *q, float *r, float *a);
void doubleQRD(int nrows, int ncols, double *q, double *r, double *a);
void complexQRD(int nrows, int ncols, float *q, float *r, float *a);
void dcomplexQRD(int nrows, int ncols, double *q, double *r, double *a);
void floatQRDP(int nrows, int ncols, float *q, float *r, int *p, float *a);
void doubleQRDP(int nrows, int ncols, double *q, double *r, int *p, double *a);
void complexQRDP(int nrows, int ncols, float *q, float *r, int *p, float *a);
void dcomplexQRDP(int nrows, int ncols, double *q, double *r, int *p, double *a);
#endif
