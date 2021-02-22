/*
 * Copyright (c) 2009 Samit Basu
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
#ifndef __Complex_hpp__
#define __Complex_hpp__

#include "IEEEFP.hpp"

template <typename T>
static inline void complex_divide(const T& ar, const T& ai,
				  const T& br, const T& bi,
				  T& c0, T& c1) {
  double ratio, den;
  double abr, abi, cr;
  if ((ai == 0) && (bi == 0)) {
    c1 = 0;
    c0 = ar/br;
    return;
  }
  if (bi == 0) {
    c0 = ar/br;
    c1 = ai/br;
    return;
  }
  if ((ar == 0) && (bi == 0)) {
    c0 = 0;
    c1 = ai/br;
    return;
  }
  if ((ai == 0) && (br == 0)) {
    c0 = 0;
    c1 = -ar/bi;
    return;
  }
    
//   if (ar == 0) {
//     c0 = (ai*bi)/(br*br + bi*bi);
//     c1 = (ai*br)/(br*br + bi*bi);
//     return;
//   }
  if ((ar == br) && (ai == bi)) {
    c0 = 1; c1 = 0;
    return;
  }
  if( (abr = br) < 0.)
    abr = - abr;
  if( (abi = bi) < 0.)
    abi = - abi;
  if( abr <= abi )
    {
      if(abi == 0) {
	if (ai != 0 || ar != 0)
	  abi = 1.;
	c1 = c0 = abi / abr;
	return;
      }
      ratio = br / bi ;
      den = bi * (1 + ratio*ratio);
      cr = (ar*ratio + ai) / den;
      c1 = (ai*ratio - ar) / den;
    }
  else
    {
      ratio = bi / br ;
      den = br * (1 + ratio*ratio);
      cr = (ar + ai*ratio) / den;
      c1 = (ai - ar*ratio) / den;
    }
  c0 = cr;
}

template <typename T>
static inline void complex_recip(const T& ar, const T& ai, T& cr, T& ci) {
  if (ai == 0) {
    ci = 0;
    cr = 1/ar;
    return;
  }
  complex_divide<T>(1,0,ar,ai,cr,ci);
}

template <class T>
inline T complex_abs(T real, T imag) {
  T swap;
  if(real < 0)
    real = -real;
  if(imag < 0)
    imag = -imag;
  if(imag > real){
    swap = real;
    real = imag;
    imag = swap;
  }
  if((real+imag) == real)
    return(real);
  double temp = double(imag)/double(real);
  temp = real*sqrt(1.0 + temp*temp);  /*overflow!!*/
  return(T(temp));
}

template <typename T>
inline T complex_phase(const T &ar, const T &ai) {
  return T(atan2(double(ai),double(ar)));
}

template <typename T>
inline void complex_multiply(const T &ar, const T &ai, 
			     const T &br, const T &bi, 
			     T &cr, T &ci) {
  if ((ai == 0) && (bi == 0)) {
    cr = ar * br;
    ci = 0;
  } else if (ai == 0) {
    cr = ar * br;
    ci = ar * bi;
  } else if (bi == 0) {
    cr = br * ar;
    ci = br * ai;
  } else {
    cr = ar * br - ai * bi;
    ci = ar * bi + ai * br;
  }
}

template <typename T>
inline bool complex_lt(const T &ar, const T &ai, 
		const T &br, const T &bi) {
  T mag_a = complex_abs(ar,ai);
  T mag_b = complex_abs(br,bi);
  if ((mag_b-mag_a) > feps(mag_a)*4) return true;
  if ((mag_a-mag_b) > feps(mag_b)*4) return false;
  return (complex_phase(ar,ai) < complex_phase(br,bi));
}

template <typename T>
inline bool complex_gt(const T &ar, const T &ai, 
		const T &br, const T &bi) {
  T mag_a = complex_abs(ar,ai);
  T mag_b = complex_abs(br,bi);
  if ((mag_b-mag_a) > feps(mag_a)*4) return false;
  if ((mag_a-mag_b) > feps(mag_b)*4) return true;
  return (complex_phase(ar,ai) > complex_phase(br,bi));
}

template <typename T>
inline bool complex_eq(const T & ar, const T & ai, 
		       const T & br, const T & bi) {
  return ((ar == br) && (ai == bi));
}

template <typename T>
inline bool complex_ne(const T & ar, const T & ai,
		       const T & br, const T & bi) {
  return ((ar != br) || (ai != bi));
}

template <typename T>
inline bool complex_le(const T & ar, const T & ai, 
		       const T & br, const T & bi) {
  return complex_eq(ar,ai,br,bi) || complex_lt(ar,ai,br,bi);
}

template <typename T>
inline bool complex_ge(const T & ar, const T & ai, 
		const T & br, const T & bi) {
  return complex_eq(ar,ai,br,bi) || complex_gt(ar,ai,br,bi);
}

template <typename T>
inline void complex_log(T xr, T xi, T &yr, T &yi) {
  yr = log(complex_abs(xr,xi));
  yi = atan2(xi,xr);
}

template <typename T>
inline void complex_exp(T xr, T xi, T &yr, T &yi) {
  yr = exp(xr)*cos(xi);
  yi = exp(xr)*sin(xi);
}

template <typename T>
inline void complex_sqrt(T xr, T xi, T &yr, T &yi) {
  T tr, ti;
  if ((xr >= 0) && (xi == 0)) {
    yr = sqrt(xr);
    yi = 0;
    return;
  }
  if ((xr < 0) && (xi == 0)) {
    yi = sqrt(-xr);
    yr = 0;
    return;
  }
  complex_log(xr,xi,tr,ti);
  tr /= 2.0;
  ti /= 2.0;
  complex_exp(tr,ti,yr,yi);
}

template <typename T>
inline void complex_square(T xr, T xi, T &yr, T &yi) {
  yr = xr*xr - xi*xi;
  yi = 2*xr*xi;
}

#endif
