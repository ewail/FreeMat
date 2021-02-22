/*
 * Copyright (c) 2009 Eugene Ingerman
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

//Borrowed with small modifications from octave

#include <math.h>
#include "IEEEFP.hpp"

template <typename T> inline int signum(T val)
{
    return (val > T(0)) - (val < T(0));
}


// This algorithm is due to P. J. Acklam.
// See http://home.online.no/~pjacklam/notes/invnorm/
// The rational approximation has relative accuracy 1.15e-9 in the whole region.
// For doubles, it is refined by a single step of Higham's 3rd order method.
// For single precision, the accuracy is already OK, so we skip it to get
// faster evaluation.

static double do_erfinv (double x, bool refine)
{
  // Coefficients of rational approximation.
  static const double a[] =
    { -2.806989788730439e+01,  1.562324844726888e+02,
      -1.951109208597547e+02,  9.783370457507161e+01,
      -2.168328665628878e+01,  1.772453852905383e+00 };
  static const double b[] =
    { -5.447609879822406e+01,  1.615858368580409e+02,
      -1.556989798598866e+02,  6.680131188771972e+01,
      -1.328068155288572e+01 };
  static const double c[] =
    { -5.504751339936943e-03, -2.279687217114118e-01,
      -1.697592457770869e+00, -1.802933168781950e+00,
       3.093354679843505e+00,  2.077595676404383e+00 };
  static const double d[] =
    {  7.784695709041462e-03,  3.224671290700398e-01,
       2.445134137142996e+00,  3.754408661907416e+00 };

  static const double spi2 =  8.862269254527579e-01; // sqrt(pi)/2.
  static const double pbreak = 0.95150;
  double ax = fabs (x), y;

  // Select case.
  if (ax <= pbreak)
    {
      // Middle region.
      const double q = 0.5 * x, r = q*q;
      const double yn = (((((a[0]*r + a[1])*r + a[2])*r + a[3])*r + a[4])*r + a[5])*q;
      const double yd = ((((b[0]*r + b[1])*r + b[2])*r + b[3])*r + b[4])*r + 1.0;
      y = yn / yd;
    }
  else if (ax < 1.0)
    {
      // Tail region.
      const double q = sqrt (-2*log (0.5*(1-ax)));
      const double yn = ((((c[0]*q + c[1])*q + c[2])*q + c[3])*q + c[4])*q + c[5];
      const double yd = (((d[0]*q + d[1])*q + d[2])*q + d[3])*q + 1.0;
      y = yn / yd * signum (-x);
    }
  else if (ax == 1.0)
    return Inf() * signum (x);
  else
    return NaN();

  if (refine)
    {
      // One iteration of Halley's method gives full precision.
      double u = (erf(y) - x) * spi2 * exp (y*y);
      y -= u / (1 + y*u);
    }

  return y;
}

double erfinv (double x)
{
  return do_erfinv (x, true);
}

float erfinv (float x)
{
  return do_erfinv (x, false);
}

