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

#include "Exception.hpp"
#include "Array.hpp"
#include "Operators.hpp"
#include <math.h>
#if HAVE_FFTW | HAVE_FFTWF
#include "fftw3.h"
#endif

#if HAVE_FFTWF
class WrapFFTWF
{
  fftwf_complex *inf, *outf;
  fftwf_plan pf_forward, pf_backward;
  int N;
public:
  WrapFFTWF(int Narg)
  {
    inf = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*Narg);
    outf = (fftwf_complex*) fftwf_malloc(sizeof(fftwf_complex)*Narg);
    pf_forward = fftwf_plan_dft_1d(Narg,inf,outf,FFTW_FORWARD,FFTW_ESTIMATE);
    pf_backward = fftwf_plan_dft_1d(Narg,inf,outf,FFTW_BACKWARD,FFTW_ESTIMATE);
    N = Narg;
  }
  ~WrapFFTWF()
  {
    fftwf_destroy_plan(pf_forward);
    fftwf_destroy_plan(pf_backward);
    fftwf_free(inf);
    fftwf_free(outf);
  }
  void Forward(float *dp)
  {
    memcpy(inf,dp,sizeof(float)*N*2);
    fftwf_execute(pf_forward);
    memcpy(dp,outf,sizeof(float)*N*2);
  }
  void Backward(float *dp)
  {
    memcpy(inf,dp,sizeof(float)*N*2);
    fftwf_execute(pf_backward);
    memcpy(dp,outf,sizeof(float)*N*2);
    for (int i=0;i<(2*N);i++)
      dp[i] /= ((float) N);
  }
};
#endif

#if HAVE_FFTW
class WrapFFTW
{
  fftw_complex *inf, *outf;
  fftw_plan pf_forward, pf_backward;
  int N;
public:
  WrapFFTW(int Narg)
  {
    inf = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*Narg);
    outf = (fftw_complex*) fftw_malloc(sizeof(fftw_complex)*Narg);
    pf_forward = fftw_plan_dft_1d(Narg,inf,outf,FFTW_FORWARD,FFTW_ESTIMATE);
    pf_backward = fftw_plan_dft_1d(Narg,inf,outf,FFTW_BACKWARD,FFTW_ESTIMATE);
    N = Narg;
  }
  ~WrapFFTW()
  {
    fftw_destroy_plan(pf_forward);
    fftw_destroy_plan(pf_backward);
    fftw_free(inf);
    fftw_free(outf);
  }
  void Forward(double *dp)
  {
    memcpy(inf,dp,sizeof(double)*N*2);
    fftw_execute(pf_forward);
    memcpy(dp,outf,sizeof(double)*N*2);
  }
  void Backward(double *dp)
  {
    memcpy(inf,dp,sizeof(double)*N*2);
    fftw_execute(pf_backward);
    memcpy(dp,outf,sizeof(double)*N*2);
    for (int i=0;i<(2*N);i++)
      dp[i] /= ((double) N);
  }
};
#endif


class OpVecFFT {
  WrapFFTWF fft_float;
  WrapFFTW  fft_double;
public:
  OpVecFFT(int N) : fft_float(N), fft_double(N) {}
  template <typename T>
  inline void func(const ConstSparseIterator<T> & src, SparseSlice<T>& dest) {
    throw Exception("fft not supported for sparse matrices");
  }
  template <typename T>
  inline void func(const ConstComplexSparseIterator<T> & src, 
		   SparseSlice<T>& dest_real,
		   SparseSlice<T>& dest_imag) {
    throw Exception("fft not supported for sparse matrices");
  }
  template <typename T>
  inline void func(const BasicArray<T> & src, BasicArray<T>& dest) {
    throw Exception("fft not defined for real arrays");
  }
  // We want to take a length N FFT.  N is the length of the output
  // P is the length of the input.
  //
  //  if P>N, we take only the first N values from the input, into a 
  //     buffer of size N
  //  if P<N, we take all P values from the input into a buffer of size
  //     N
  inline void func(const BasicArray<float> & src_real,
		   const BasicArray<float> & src_imag,
		   BasicArray<float>& dest_real,
		   BasicArray<float>& dest_imag) {
    if (src_real.length() == 0) return;
    int N = dest_real.length();
    int P = src_real.length();
    QVector<float> tbuf(N*2);
    int L = qMin(P,N);
    for (index_t i=1;i<=L;i++) {
      tbuf[int(2*i-1)-1] = src_real[i];
      tbuf[int(2*i)-1] = src_imag[i];
    }
    fft_float.Forward(tbuf.data());
    for (index_t i=1;i<=dest_real.length();i++) {
      dest_real[i] = tbuf[int(2*i-1)-1];
      dest_imag[i] = tbuf[int(2*i)-1];
    }
  }  
  inline void func(const BasicArray<double> & src_real,
		   const BasicArray<double> & src_imag,
		   BasicArray<double>& dest_real,
		   BasicArray<double>& dest_imag) {
    if (src_real.length() == 0) return;
    int N = dest_real.length();
    int P = src_real.length();
    QVector<double> tbuf(N*2);
    int L = qMin(P,N);
    for (index_t i=1;i<=L;i++) {
      tbuf[int(2*i-1)-1] = src_real[i];
      tbuf[int(2*i)-1] = src_imag[i];
    }
    fft_double.Forward(tbuf.data());
    for (index_t i=1;i<=dest_real.length();i++) {
      dest_real[i] = tbuf[int(2*i-1)-1];
      dest_imag[i] = tbuf[int(2*i)-1];
    }
  }  
};

class OpVecIFFT {
  WrapFFTWF fft_float;
  WrapFFTW fft_double;
public:
  OpVecIFFT(int N) : fft_float(N), fft_double(N) {}
  template <typename T>
  inline void func(const ConstSparseIterator<T> & src, SparseSlice<T>& dest) {
    throw Exception("ifft not supported for sparse matrices");
  }
  template <typename T>
  inline void func(const ConstComplexSparseIterator<T> & src, 
			  SparseSlice<T>& dest_real,
			  SparseSlice<T>& dest_imag) {
    throw Exception("ifft not supported for sparse matrices");
  }
  template <typename T>
  inline void func(const BasicArray<T> & src, BasicArray<T>& dest) {
    throw Exception("ifft not defined for real arrays");
  }
  inline void func(const BasicArray<float> & src_real,
			  const BasicArray<float> & src_imag,
			  BasicArray<float>& dest_real,
			  BasicArray<float>& dest_imag) {
    if (src_real.length() == 0) return;
    int N = dest_real.length();
    int P = src_real.length();
    QVector<float> tbuf(N*2);
    int L = qMin(P,N);
    for (index_t i=1;i<=L;i++) {
      tbuf[int(2*i-1)-1] = src_real[i];
      tbuf[int(2*i)-1] = src_imag[i];
    }
    fft_float.Backward(tbuf.data());
    for (index_t i=1;i<=dest_real.length();i++) {
      dest_real[i] = tbuf[int(2*i-1)-1];
      dest_imag[i] = tbuf[int(2*i)-1];
    }
  }
  inline void func(const BasicArray<double> & src_real,
			  const BasicArray<double> & src_imag,
			  BasicArray<double>& dest_real,
			  BasicArray<double>& dest_imag) {
    if (src_real.length() == 0) return;
    int N = dest_real.length();
    int P = src_real.length();
    QVector<double> tbuf(N*2);
    int L = qMin(P,N);
    for (index_t i=1;i<=L;i++) {
      tbuf[int(2*i-1)-1] = src_real[i];
      tbuf[int(2*i)-1] = src_imag[i];
    }
    fft_double.Backward(tbuf.data());
    for (index_t i=1;i<=dest_real.length();i++) {
      dest_real[i] = tbuf[int(2*i-1)-1];
      dest_imag[i] = tbuf[int(2*i)-1];
    }
  }
};

//@@Signature
//function fft FFTFunction
//inputs x len dim
//outputs y
//@@Signature
//function ifft IFFTFunction
//inputs x len dim
//outputs y
//DOCBLOCK transforms_fft
ArrayVector FFTFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("FFT requires at least one argument");
  // Get the length of the zero pad
  int FFTLength = -1;
  if ((arg.size() > 1) && (!arg[1].isEmpty())) {
    FFTLength = arg[1].asInteger();
    if (FFTLength <= 0)
      throw Exception("Length argument to FFT should be positive");
  }
  // Get the dimension to FFT along.
  int FFTDim;
  if (arg.size() > 2) {
    FFTDim = arg[2].asInteger() - 1;
    if (FFTDim < 0)
      throw Exception("Dimension argument to FFT should be positive");
  } else {
    if (arg[0].isScalar())
      FFTDim = 1;
    else
      FFTDim = arg[0].dimensions().firstNonsingular();
  }
  if (FFTLength == -1)
    FFTLength = int(arg[0].dimensions()[FFTDim]);
  Array arg0(arg[0]);
  if (arg0.dataClass() != Float)
    arg0 = arg0.toClass(Double);
  if (arg0.dimensions() == NTuple(0,0))
    return arg0;
  arg0.forceComplex();
  OpVecFFT op(FFTLength);
  return ArrayVector(VectorOpDynamic<OpVecFFT>(arg0,FFTLength,FFTDim,op));
}

ArrayVector IFFTFunction(int nargout, const ArrayVector& arg) {
  // Get the data argument
  if (arg.size() < 1)
    throw Exception("IFFT requires at least one argument");
  // Get the length of the zero pad
  int FFTLength = -1;
  if ((arg.size() > 1) && (!arg[1].isEmpty())) {
    FFTLength = arg[1].asInteger();
    if (FFTLength <= 0)
      throw Exception("Length argument to IFFT should be positive");
  }
  // Get the dimension to FFT along.
  int FFTDim;
  if (arg.size() > 2) {
    FFTDim = arg[2].asInteger() - 1;
    if (FFTDim < 0)
      throw Exception("Dimension argument to IFFT should be positive");
  } else {
    if (arg[0].isScalar())
      FFTDim = 1;
    else
      FFTDim = arg[0].dimensions().firstNonsingular();
  }
  if (FFTLength == -1)
    FFTLength = int(arg[0].dimensions()[FFTDim]);
  Array arg0(arg[0]);
  if (arg0.dataClass() != Float)
    arg0 = arg0.toClass(Double);
  if (arg0.dimensions() == NTuple(0,0))
    return arg0;
  arg0.forceComplex();
  OpVecIFFT op(FFTLength);
  return ArrayVector(VectorOpDynamic<OpVecIFFT>(arg0,FFTLength,FFTDim,op));
}
