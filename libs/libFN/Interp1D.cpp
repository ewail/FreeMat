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
#include "Array.hpp"
#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include "Exception.hpp"

template <class T>
static int interv(const T *xt, int lxt, T x, int *left, int *mflag)
{

  /* Initialized data */

  static int ilo = 1;
  
  static int ihi, istep, middle;
  
  /*  from  * a practical guide to splines *  by C. de Boor */
  /* Computes  left = max( i :  xt(i) .lt. xt(lxt) .and.  xt(i) .le. x )  . */

  /* ******  i n p u t  ****** */
  /*  xt.....a real sequence, of length  lxt , assumed to be nondecreasing */
  /*  lxt.....number of terms in the sequence  xt . */
  /*  x.....the point whose location with respect to the sequence  xt  is */
  /*        to be determined. */

  /* ******  o u t p u t  ****** */
  /*  left, mflag.....both ints, whose value is */

  /*   1     -1      if               x .lt.  xt(1) */
  /*   i      0      if   xt(i)  .le. x .lt. xt(i+1) */
  /*   i      0      if   xt(i)  .lt. x .eq. xt(i+1) .eq. xt(lxt) */
  /*   i      1      if   xt(i)  .lt.        xt(i+1) .eq. xt(lxt) .lt. x */

  /*        In particular,  mflag = 0  is the 'usual' case.  mflag .ne. 0 */
  /*        indicates that  x  lies outside the CLOSED interval */
  /*        xt(1) .le. y .le. xt(lxt) . The asymmetric treatment of the */
  /*        intervals is due to the decision to make all pp functions cont- */
  /*        inuous from the right, but, by returning  mflag = 0  even if */
  /*        x = xt(lxt), there is the option of having the computed pp function */
  /*        continuous from the left at  xt(lxt) . */

  /* ******  m e t h o d  ****** */
  /*  The program is designed to be efficient in the common situation that */
  /*  it is called repeatedly, with  x  taken from an increasing or decrea- */
  /*  sing sequence. This will happen, e.g., when a pp function is to be */
  /*  graphed. The first guess for  left  is therefore taken to be the val- */
  /*  ue returned at the previous call and stored in the  l o c a l  varia- */
  /*  ble  ilo . A first check ascertains that  ilo .lt. lxt (this is nec- */
  /*  essary since the present call may have nothing to do with the previ- */
  /*  ous call). Then, if  xt(ilo) .le. x .lt. xt(ilo+1), we set  left = */
  /*  ilo  and are done after just three comparisons. */
  /*     Otherwise, we repeatedly double the difference  istep = ihi - ilo */
  /*  while also moving  ilo  and  ihi  in the direction of  x , until */
  /*                      xt(ilo) .le. x .lt. xt(ihi) , */
  /*  after which we use bisection to get, in addition, ilo+1 = ihi . */
  /*  left = ilo  is then returned. */

  /* Parameter adjustments */
  --xt;

  /* Function Body */
  ihi = ilo + 1;
  if (ihi < lxt) {
    goto L20;
  }
  if (x >= xt[lxt]) {
    goto L110;
  }
  if (lxt <= 1) {
    goto L90;
  }
  ilo = lxt - 1;
  ihi = lxt;

 L20:
  if (x >= xt[ihi]) {
    goto L40;
  }
  if (x >= xt[ilo]) {
    goto L100;
  }

  /*              **** now x .lt. xt(ilo) . decrease  ilo  to capture  x . */
  istep = 1;
 L31:
  ihi = ilo;
  ilo = ihi - istep;
  if (ilo <= 1) {
    goto L35;
  }
  if (x >= xt[ilo]) {
    goto L50;
  }
  istep <<= 1;
  goto L31;
 L35:
  ilo = 1;
  if (x < xt[1]) {
    goto L90;
  }
  goto L50;
  /*              **** now x .ge. xt(ihi) . increase  ihi  to capture  x . */
 L40:
  istep = 1;
 L41:
  ilo = ihi;
  ihi = ilo + istep;
  if (ihi >= lxt) {
    goto L45;
  }
  if (x < xt[ihi]) {
    goto L50;
  }
  istep <<= 1;
  goto L41;
 L45:
  if (x >= xt[lxt]) {
    goto L110;
  }
  ihi = lxt;

  /*           **** now xt(ilo) .le. x .lt. xt(ihi) . narrow the interval. */
 L50:
  middle = (ilo + ihi) / 2;
  if (middle == ilo) {
    goto L100;
  }
  /*     note. it is assumed that middle = ilo in case ihi = ilo+1 . */
  if (x < xt[middle]) {
    goto L53;
  }
  ilo = middle;
  goto L50;
 L53:
  ihi = middle;
  goto L50;
  /* **** set output and return. */
 L90:
  *mflag = -1;
  *left = 1;
  return 0;
 L100:
  *mflag = 0;
  *left = ilo;
  return 0;
 L110:
  *mflag = 1;
  if (x == xt[lxt]) {
    *mflag = 0;
  }
  *left = lxt;
 L111:
  if (*left == 1) {
    return 0;
  }
  --(*left);
  if (xt[*left] < xt[lxt]) {
    return 0;
  }
  goto L111;
} /* interv_ */

template <class T>
bool TestForMonotonicReal(const T*dp, index_t len) {
  bool monotonic = true;
  int k = 0;
  while (monotonic && (k<len-1)) {
    monotonic = dp[k] <= dp[k+1];
    k++;
  }
  return monotonic;
}

template <class T>
static void DoLinearInterpolationReal(const T* x1, const T* y1, 
				      int x1count, const T* xi,
				      int xicount, T* yi, int xtrapflag) {
  int left, mflag;
  int k;
  T frac;

  for (k=0;k<xicount;k++) {
    interv<T>(x1,x1count,xi[k],&left,&mflag);
    if ((mflag==0) || (xtrapflag == 3)) {
      frac = (xi[k] - x1[left-1])/(x1[left]-x1[left-1]);
      yi[k] = y1[left-1] + frac*(y1[left]-y1[left-1]);
    } else {
      switch (xtrapflag) {
      case 0:
	yi[k] = atof("nan");
	break;
      case 1:
	yi[k] = 0;
	break;
      case 2:
	if (mflag == -1)
	  yi[k] = y1[0];
	else if (mflag == 1)
	  yi[k] = y1[x1count-1];
	break;
      }
    }
  }
}

static bool TestForMonotonic(Array x) {
  switch (x.dataClass()) {
  default: throw Exception("unhandled type in argument to testformonotonic");
  case Float:
    return TestForMonotonicReal<float>(x.real<float>().data(),x.length());
  case Double:
    return TestForMonotonicReal<double>(x.real<double>().data(),x.length());
  }
}

template <typename T>
static BasicArray<T> Lint(const BasicArray<T> &x1, const BasicArray<T> &y1, const BasicArray<T> &xi, int xtrapflag) {
  BasicArray<T> yi(xi.dimensions());
  DoLinearInterpolationReal(x1.constData(),y1.constData(),int(x1.length()),
			    xi.constData(),int(xi.length()),yi.data(),xtrapflag);
  return yi;
}

//@@Signature
//function interplin1 Interplin1Function jitsafe
//inputs x1 y1 xi extrapflag
//outputs yi
//DOCBLOCK curvefit_interplin1
ArrayVector Interplin1Function(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("interplin1 requires at least three arguments (x1,y1,xi)");
  Array x1(arg[0]);
  Array y1(arg[1]);
  Array xi(arg[2]);
  xi = xi.asDenseArray();

  // Verify that x1 are numerical
  if (x1.isReferenceType() || y1.isReferenceType() || xi.isReferenceType())
    throw Exception("arguments to interplin1 must be numerical arrays");
  if (x1.isComplex() || xi.isComplex())
    throw Exception("x-coordinates cannot be complex in interplin1");
  if (!((x1.dataClass() == y1.dataClass()) && (x1.dataClass() == xi.dataClass())))
    throw Exception("types of all three arguments to interplin1 must be the same");
  // Make sure x1 and y1 are the same length
  if (x1.length() != y1.length())
    throw Exception("vectors x1 and y1 must be the same length");
  if (!TestForMonotonic(x1))
    throw Exception("vector x1 must be monotonically increasing");
  // Check for extrapolation flag
  int xtrap = 0;
  if (arg.size() == 4) {
    if (!arg[3].isString())
      throw Exception("extrapolation flag must be a string");
    QString xtrap_c = arg[3].asString();
    if (xtrap_c=="nan")
      xtrap = 0;
    else if (xtrap_c=="zero")
      xtrap = 1;
    else if (xtrap_c=="endpoint")
      xtrap = 2;
    else if (xtrap_c=="extrap")
      xtrap = 3;
    else
      throw Exception("unrecognized extrapolation type flag to routine interplin1");
  }
  switch(y1.dataClass()) {
  default: throw Exception("unhandled type as argument to interplin1");
  case Float:
    if (y1.allReal()) {
      return ArrayVector(Array(Lint(x1.constReal<float>(),y1.constReal<float>(),xi.constReal<float>(),xtrap)));
    } else {
      return ArrayVector(Array(Lint(x1.constReal<float>(),y1.constReal<float>(),xi.constReal<float>(),xtrap),
			       Lint(x1.constReal<float>(),y1.constImag<float>(),xi.constReal<float>(),xtrap)));
    }
  case Double:
    if (y1.allReal()) {
      return ArrayVector(Array(Lint(x1.constReal<double>(),y1.constReal<double>(),xi.constReal<double>(),xtrap)));
    } else {
      return ArrayVector(Array(Lint(x1.constReal<double>(),y1.constReal<double>(),xi.constReal<double>(),xtrap),
			       Lint(x1.constReal<double>(),y1.constImag<double>(),xi.constReal<double>(),xtrap)));
    }
  }
}
