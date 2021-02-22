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
#include <math.h>
#include "Utils.hpp"
#include "IEEEFP.hpp"
#include "Operators.hpp"
#include "Complex.hpp"

#define DEG2RAD M_PI/180.0

//@@Signature
//function cos CosFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_cos

struct OpCos {
  static inline float func(float x) {
    return cosf(x);
  }
  static inline double func(double x) {
    return cos(x);
  }
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = cosf(xr)*coshf(xi);
    yi = -sinf(xr)*sinhf(xi);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = cos(xr)*cosh(xi);
    yi = -sin(xr)*sinh(xi);
  }
};

ArrayVector CosFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Cosine Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpCos>(arg[0]));
}

JitScalarFunc1(cos,OpCos::func);

//@@Signature
//function cosd CosdFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_cosd
struct OpCosd {
  template <typename T>
  static inline T func(T x) {
    return OpCos::func(x*DEG2RAD);
  }
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    OpCos::func(xr*DEG2RAD,xi*DEG2RAD,yr,yi);
  }
};

ArrayVector CosdFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Cosd Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpCosd>(arg[0]));
}

JitScalarFunc1(cosd,OpCosd::func);

//@@Signature 
//function sin SinFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_sin

struct OpSin {
  static inline float func(float x) {
    return sinf(x);
  }
  static inline double func(double x) {
    return sin(x);
  }
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = sinf(xr)*coshf(xi);
    yi = cosf(xr)*sinhf(xi);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = sin(xr)*cosh(xi);
    yi = cos(xr)*sinh(xi);
  }
};

ArrayVector SinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Sin Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpSin>(arg[0]));
}

JitScalarFunc1(sin,OpSin::func);

//@@Signature
//function sind SindFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_sind

struct OpSind {
  template <typename T>
  static inline T func(T x) {
    return OpSin::func(x*DEG2RAD);
  }
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    OpSin::func(xr*DEG2RAD,xi*DEG2RAD,yr,yi);
  }
};

ArrayVector SindFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Sind Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpSind>(arg[0]));
}

JitScalarFunc1(sind,OpSind::func);

//@@Signature
//function tan TanFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_tan

struct OpTan {
  static inline float func(float x) {
    return tanf(x);
  }
  static inline double func(double x) {
    return tan(x);
  }
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    T sinr, sini;
    T cosr, cosi;
    OpSin::func(xr,xi,sinr,sini);
    OpCos::func(xr,xi,cosr,cosi);
    complex_divide(sinr,sini,cosr,cosi,yr,yi);
  }
};

ArrayVector TanFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Tangent Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpTan>(arg[0]));
}

JitScalarFunc1(tan,OpTan::func);

//@@Signature
//function tand TandFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_tand
struct OpTand {
  template <typename T>
  static inline T func(T x) {
    return OpTan::func(x*DEG2RAD);
  }
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    OpTan::func(T(xr*DEG2RAD),T(xi*DEG2RAD),yr,yi);
  }
};

ArrayVector TandFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Tand Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpTand>(arg[0]));
}

JitScalarFunc1(tand,OpTand::func);

//@@Signature
//function csc CscFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_csc

struct OpCsc {
  static inline float func(float x) {
    return 1.0f/OpSin::func(x);
  }
  static inline double func(double x) {
    return 1.0/OpSin::func(x);
  }
  static inline void func(float xr, float xi, float &yr, float &yi) {
    float zr, zi;
    OpSin::func(xr,xi,zr,zi);
    complex_recip(zr,zi,yr,yi);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    double zr, zi;
    OpSin::func(xr,xi,zr,zi);
    complex_recip(zr,zi,yr,yi);
  }
};

ArrayVector CscFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Cosecant Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpCsc>(arg[0]));
}

JitScalarFunc1(csc,OpCsc::func);

//@@Signature
//function cscd CscdFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_cscd

struct OpCscd {
  static inline float func(float x) {
    return 1.0f/OpSind::func(x);
  }
  static inline double func(double x) {
    return 1.0/OpSind::func(x);
  }
  static inline void func(float xr, float xi, float &yr, float &yi) {
    float zr, zi;
    OpSind::func(xr,xi,zr,zi);
    complex_recip(zr,zi,yr,yi);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    double zr, zi;
    OpSind::func(xr,xi,zr,zi);
    complex_recip(zr,zi,yr,yi);
  }
};

ArrayVector CscdFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("cscd Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpCscd>(arg[0]));
}

JitScalarFunc1(cscd,OpCscd::func);

//@@Signature
//function sec SecFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_sec

struct OpSec {
  static inline float func(float x) {
    return 1.0f/OpCos::func(x);
  }
  static inline double func(double x) {
    return 1.0/OpCos::func(x);
  }
  static inline void func(float xr, float xi, float &yr, float &yi) {
    float zr, zi;
    OpCos::func(xr,xi,zr,zi);
    complex_recip(zr,zi,yr,yi);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    double zr, zi;
    OpCos::func(xr,xi,zr,zi);
    complex_recip(zr,zi,yr,yi);
  }
};

ArrayVector SecFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Secant Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpSec>(arg[0]));
}

JitScalarFunc1(sec,OpSec::func);

//@@Signature
//function secd SecdFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_secd

struct OpSecd {
  static inline float func(float x) {
    return OpSec::func(x*DEG2RAD);
  }
  static inline double func(double x) {
    return OpSec::func(x*DEG2RAD);
  }
  static inline void func(float xr, float xi, float &yr, float &yi) {
    OpSec::func(xr*DEG2RAD,xi*DEG2RAD,yr,yi);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    OpSec::func(xr*DEG2RAD,xi*DEG2RAD,yr,yi);
  }
};

ArrayVector SecdFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Secd Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpSecd>(arg[0]));
}

JitScalarFunc1(secd,OpSecd::func);

//@@Signature
//function cot CotFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_cot

struct OpCot {
  template <typename T>
  static inline T func(T x) {
    return 1.0f/OpTan::func(x);
  }
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    T sinr, sini;
    T cosr, cosi;
    OpSin::func(xr,xi,sinr,sini);
    OpCos::func(xr,xi,cosr,cosi);
    complex_divide(cosr,cosi,sinr,sini,yr,yi);
  }
};

ArrayVector CotFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Cotangent Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpCot>(arg[0]));
}

JitScalarFunc1(cot,OpCot::func);

//DOCBLOCK mathfunctions_cotd

struct OpCotd {
  static inline float func(float x) {return 1.0f/tanf(x*DEG2RAD);}
  static inline double func(double x) {return 1.0/tan(x*DEG2RAD);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = sinf(xr*DEG2RAD)/(-cosf(xr*DEG2RAD)+coshf(xi*DEG2RAD));
    yi = -sinhf(xi*DEG2RAD)/(-cosf(xr*DEG2RAD)+coshf(xi*DEG2RAD));
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = sin(xr*DEG2RAD)/(-cos(xr*DEG2RAD)+cosh(xi*DEG2RAD));
    yi = -sinh(xi*DEG2RAD)/(-cos(xr*DEG2RAD)+cosh(xi*DEG2RAD));
  }
};

ArrayVector CotdFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("cotd function takes exactly one argument");
  return ArrayVector(UnaryOp<OpCotd>(arg[0]));
}

JitScalarFunc1(cotd,OpCotd::func);

//@@Signature
//function acos ArccosFunction
//inputs x
//outputs y
//DOCBLOCK mathfunctions_acos

struct OpAcos {
  static inline float func(float x) {return acosf(x);}
  static inline double func(double x) {return acos(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    if (IsInfinite(xr) && (xi == 0)) {
      if (xr > 0) {
	  yr = 0;
	  yi = Inf();
      } else {
	yr = 4.0*atan(1.0);
	yi = -Inf();
      }
      return;
    }
    T xsq_real, xsq_imag;
    // Compute x^2
    complex_square(xr,xi,xsq_real,xsq_imag);
    // Compute 1-x^2
    xsq_real = 1.0 - xsq_real;
    xsq_imag = -xsq_imag;
    if (xi == 0) xsq_imag = 0;
    T xrt_real, xrt_imag;
    // Compute sqrt(1-x^2)
    complex_sqrt(xsq_real,xsq_imag,xrt_real,xrt_imag);
    // Add i*x = i*(a+b*i) = -b+i*a
    xrt_real -= xi;
    xrt_imag += xr;
    // Take the complex log
    T xlg_real, xlg_imag;
    complex_log(xrt_real,xrt_imag,xlg_real,xlg_imag);
    // Answer = pi/2
    yr = 2.0*atan(1.0) - xlg_imag;
    yi = xlg_real;
  }
};

ArrayVector ArccosFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Arccosine Function takes exactly one argument");
  Array input(arg[0]);
  if (input.allReal() && (ArrayRange(input) > 1))
    input.forceComplex();
  return ArrayVector(UnaryOp<OpAcos>(input));
}

//@@Signature
//function asin ArcsinFunction
//inputs x
//outputs y
//DOCBLOCK mathfunctions_asin

struct OpAsin {
  static inline float func(float x) {return asinf(x);}
  static inline double func(double x) {return asin(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    if (IsInfinite(xr) && (xi == 0)) {
      yr = NaN();
      yi = -Inf();
      return;
    }
    T xsq_real, xsq_imag;
    // Compute x^2
    complex_square(xr,xi,xsq_real,xsq_imag);
    // Compute 1-x^2
    xsq_real = 1.0 - xsq_real;
    xsq_imag = -xsq_imag;
    if (xi == 0) xsq_imag = 0;
    T xrt_real, xrt_imag;
    // Compute sqrt(1-x^2)
    complex_sqrt(xsq_real,xsq_imag,xrt_real,xrt_imag);
    // Add i*x = i*(a+b*i) = -b+i*a
    xrt_real -= xi;
    xrt_imag += xr;
    // Take the complex log
    T xlg_real, xlg_imag;
    complex_log(xrt_real,xrt_imag,xlg_real,xlg_imag);
    // Answer = pi/2
    yr = xlg_imag;
    yi = -xlg_real;
  }
};

ArrayVector ArcsinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Arcsine Function takes exactly one argument");
  Array input(arg[0]);
  if (input.allReal() && (ArrayRange(input) > 1))
    input.forceComplex();
  return ArrayVector(UnaryOp<OpAsin>(input));
}

//@@Signature
//function atan ArcTanFunction
//inputs x
//outputs y
//DOCBLOCK mathfunctions_atan

struct OpAtan {
  static inline float func(float x) {return atanf(x);}
  static inline double func(double x) {return atan(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T&yi) {
    T a_real, a_imag;
    T b_real, b_imag;
    complex_log(1 + xi,-xr,a_real,a_imag);
    complex_log(1 - xi,xr,b_real,b_imag);
    yr = -0.5*(a_imag-b_imag);
    yi = 0.5*(a_real-b_real);
  }
};

ArrayVector ArcTanFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Arctan Function takes exactly one argument");
  return ArrayVector(UnaryOp<OpAtan>(arg[0]));
}

JitScalarFunc1(atan,OpAtan::func);

//@@Signature
//function atan2 Arctan2Function jitsafe
//inputs y x
//outputs z
//DOCBLOCK mathfunctions_atan2

struct OpAtan2 {
  static inline float func(float y, float x) {return atan2f(y,x);}
  static inline double func(double y, double x) {return atan2(y,x);}
  template <typename T>
  static inline void func(T y_real, T y_imag, T x_real, T x_imag, T &z_real, T &z_imag) {
    T a_real, a_imag;
    a_real = x_real - y_imag;
    a_imag = x_imag + y_real;
    // compute x_squared and y_squared
    T xsqr_real, xsqr_imag;
    T ysqr_real, ysqr_imag;
    complex_square(x_real,x_imag,xsqr_real,xsqr_imag);
    complex_square(y_real,y_imag,ysqr_real,ysqr_imag);
    T den_real, den_imag;
    den_real = xsqr_real + ysqr_real;
    den_imag = xsqr_imag + ysqr_imag;
    T den_sqrt_real, den_sqrt_imag;
    complex_sqrt(den_real,den_imag,den_sqrt_real,den_sqrt_imag);
    // compute the log of the numerator
    T log_num_real, log_num_imag;
    complex_log(a_real,a_imag,log_num_real,log_num_imag);
    // compute the log of the denominator
    T log_den_real, log_den_imag;
    complex_log(den_sqrt_real,den_sqrt_imag,log_den_real,log_den_imag);
    // compute the num - den
    log_num_real -= log_den_real;
    log_num_imag -= log_den_imag;
    // compute -i * (c_r + i * c_i) = c_i - i * c_r
    z_real = log_num_imag;
    z_imag = -log_num_real;
  }
};

ArrayVector Arctan2Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("Arctan2 Function takes exactly two arguments");
  Array y(arg[0]);
  Array x(arg[1]);
  return ArrayVector(DotOp<OpAtan2>(y,x));
}

JitScalarFunc2(atan2,OpAtan2::func);
