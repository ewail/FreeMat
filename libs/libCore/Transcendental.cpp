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
#include "Math.hpp"
#include "Operators.hpp"
#include "Complex.hpp"
#include "IEEEFP.hpp"


//@@Signature
//function log1p Log1PFunction
//inputs x
//output y
//DOCBLOCK mathfunctions_log1p

struct OpLog1P {
  static inline float func(float x) {return log1pf(x);}
  static inline double func(double x) {return log1p(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = logf(complex_abs<float>(xr+1,xi));
    yi = atan2f(xi,xr+1);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = log(complex_abs<double>(xr+1,xi));
    yi = atan2(xi,xr+1);
  }
};

ArrayVector Log1PFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Log1p function takes exactly one argument");
  Array input(arg[0]);
  if (ArrayMin(input) < -1) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpLog1P>(input).toClass(input.dataClass()));
}

//@@Signature
//function log LogFunction
//inputs x
//outputs y
//DOCBLOCK mathfunctions_log

struct OpLog {
  static inline float func(float x) {return logf(x);}
  static inline double func(double x) {return log(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = logf(complex_abs<float>(xr,xi));
    yi = atan2f(xi,xr);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = log(complex_abs<double>(xr,xi));
    yi = atan2(xi,xr);
  }
};

ArrayVector LogFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Log function takes exactly one argument");
  Array input(arg[0]);
  if (!IsNonNegativeOrNaN(input)) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpLog>(input));
}

//@@Signature
//function sqrt SqrtFunction
//inputs x
//outputs y
//DOCBLOCK mathfunctions_sqrt

struct OpSqrt {
  static inline float func(float x) {return sqrtf(x);}
  static inline double func(double x) {return sqrt(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    complex_sqrt<float>(xr,xi,yr,yi);
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    complex_sqrt<double>(xr,xi,yr,yi);
  }
};

ArrayVector SqrtFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Sqrt function takes exactly one argument");
  Array input(arg[0]);
  if (!IsNonNegativeOrNaN(input)) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpSqrt>(input));
}


//@@Signature
//function tanh TanhFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_tanh

//  e^r*c(i)+e^(-r)*c(-i)/2 + i

struct OpTanh {
  static inline float func(float x) {return tanhf(x);}
  static inline double func(double x) {return tanh(x);}
  static void func(float xr, float xi, float &yr, float &yi) {
    float cr = (expf(xr)*cosf(xi)+expf(-xr)*cosf(-xi))/2;
    float ci = (expf(xr)*sinf(xi)+expf(-xr)*sinf(-xi))/2;
    float sr = (expf(xr)*cosf(xi)-expf(-xr)*cosf(-xi))/2;
    float si = (expf(xr)*sinf(xi)-expf(-xr)*sinf(-xi))/2;
    complex_divide(sr,si,cr,ci,yr,yi);
  }
  static void func(double xr, double xi, double &yr, double &yi) {
    double cr = (exp(xr)*cos(xi)+exp(-xr)*cos(-xi))/2;
    double ci = (exp(xr)*sin(xi)+exp(-xr)*sin(-xi))/2;
    double sr = (exp(xr)*cos(xi)-exp(-xr)*cos(-xi))/2;
    double si = (exp(xr)*sin(xi)-exp(-xr)*sin(-xi))/2;
    //     double cr = (exp(xr)*cos(xi)+exp(-xr)*cos(-xi))/2;
    //     double ci = (exp(xr)*sin(xi)+exp(-xr)*sin(-xi))/2;
    //     double sr = (exp(xr)*cos(xi)-exp(-xr)*cos(-xi))/2;
    //     double si = (exp(xr)*sin(xi)-exp(-xr)*sin(-xi))/2;
    complex_divide(sr,si,cr,ci,yr,yi);
  }
};

ArrayVector TanhFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Tanh function takes exactly one argument");
  return ArrayVector(UnaryOp<OpTanh>(arg[0]));
}

JitScalarFunc1(tanh,OpTanh::func);

//@@Signature
//function acosh ArccoshFunction 
//inputs x
//outputs y
//DOCBLOCK mathfunctions_acosh

// log(x+sqrt(x^2-1))
struct OpArccosh {
  static inline float func(float x) {return acoshf(x);}
  static inline double func(double x) {return acosh(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    if (xr == -Inf() && xi == 0) {
      yr = Inf();
      yi = M_PI;
      return;
    }
    T xrt_real1, xrt_imag1;
    T xrt_real2, xrt_imag2;
    complex_sqrt(xr+1,xi,xrt_real1,xrt_imag1);
    complex_sqrt(xr-1,xi,xrt_real2,xrt_imag2);
    T y_real, y_imag;
    complex_multiply(xrt_real1,xrt_imag1,
		     xrt_real2,xrt_imag2,
		     y_real,y_imag);
    y_real += xr;
    y_imag += xi;
    complex_log(y_real,y_imag,yr,yi);
  }
};

ArrayVector ArccoshFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("acosh function takes exactly one argument");
  Array input(arg[0]);
  if (input.allReal() && (ArrayMin(input) <= 1)) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpArccosh>(input));
}

//@@Signature
//function asinh ArcsinhFunction 
//inputs x
//outputs y
//DOCBLOCK mathfunctions_asinh

// log(x+sqrt(x^2-1))
struct OpArcsinh {
  static inline float func(float x) {return asinhf(x);}
  static inline double func(double x) {return asinh(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    T xsq_real, xsq_imag;
    complex_square(xr,xi,xsq_real,xsq_imag);
    xsq_real += 1;
    T xrt_real, xrt_imag;
    complex_sqrt(xsq_real,xsq_imag,xrt_real,xrt_imag);
    xrt_real += xr;
    xrt_imag += xi;
    complex_log(xrt_real,xrt_imag,yr,yi);
  }
};

ArrayVector ArcsinhFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("acosh function takes exactly one argument");
  return ArrayVector(UnaryOp<OpArcsinh>(arg[0]));
}


//@@Signature
//function asech ArcsechFunction
//inputs x
//outputs y
//DOCBLOCK mathfunctions_asech
struct OpArcsech {
  template <typename T>
  static inline T func(T x) {
    if (x == 0)
      return Inf();
    return log(sqrt(1/x-1)*sqrt(1/x+1)+1/x);
  }
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
    if ((xr == 0) && (xi == 0)) {
      yr = Inf();
      yi = 0;
      return;
    }
    T xrp_real, xrp_imag;
    complex_recip(xr,xi,xrp_real,xrp_imag);
    OpArccosh::func(xrp_real,xrp_imag,yr,yi);
  }
};

ArrayVector ArcsechFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("asech function takes exactly one argument");
  Array input(arg[0]);
  if (input.allReal() && ((ArrayMin(input) < 0) || (ArrayMax(input) > 1))) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpArcsech>(input));
}



//@@Signature
//function atanh ArctanhFunction
//inputs x
//outputs y
//DOCBLOCK mathfunctions_atanh
struct OpArctanh {
  static inline float func(float x) {return atanhf(x);}
  static inline double func(double x) {return atanh(x);}
  template <typename T>
  static inline void func(T xr, T xi, T &yr, T &yi) {
//     if ((xr == 1) && (xi == 0)) {
//       yr = Inf();
//       yi = 0;
//       return;
//     }
    if ((xr == Inf()) && (xi == 0)) {
      yr = 0;
      yi = M_PI/2.0;
      return;
    }
    if ((xr == -Inf()) && (xi == 0)) {
      yr = 0;
      yi = -M_PI/2.0;
      return;
    }
    T xa, xb;
    T ya, yb;
    complex_log(xr+1,xi,xa,xb);
    complex_log(1-xr,-xi,ya,yb);
    yr = (xa-ya)/2;
    yi = (xb-yb)/2;
  }
};

ArrayVector ArctanhFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("atanh function takes exactly one argument");
  Array input(arg[0]);
  if (input.allReal() && (ArrayRange(input) >= 1)) {
    if (input.dataClass() != Float) 
      input = input.toClass(Double);
    input.forceComplex();
  }
  return ArrayVector(UnaryOp<OpArctanh>(input));
}

//@@Signature
//function cosh CoshFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_cosh

struct OpCosh {
  static inline float func(float x) {return coshf(x);}
  static inline double func(double x) {return cosh(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = (expf(xr)*cosf(xi)+expf(-xr)*cosf(-xi))/2;
    yi = (expf(xr)*sinf(xi)+expf(-xr)*sinf(-xi))/2;
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = (exp(xr)*cos(xi)+exp(-xr)*cos(-xi))/2;
    yi = (exp(xr)*sin(xi)+exp(-xr)*sin(-xi))/2;    
  }
};

ArrayVector CoshFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Cosh function takes exactly one argument");
  return ArrayVector(UnaryOp<OpCosh>(arg[0]));
}

JitScalarFunc1(cosh,OpCosh::func);

//@@Signature
//function sinh SinhFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_sinh

struct OpSinh {
  static inline float func(float x) {return sinhf(x);}
  static inline double func(double x) {return sinh(x);}
  static inline void func(float xr, float xi, float &yr, float &yi) {
    yr = (expf(xr)*cosf(xi)-expf(-xr)*cosf(-xi))/2;
    yi = (expf(xr)*sinf(xi)-expf(-xr)*sinf(-xi))/2;    
  }
  static inline void func(double xr, double xi, double &yr, double &yi) {
    yr = (exp(xr)*cos(xi)-exp(-xr)*cos(-xi))/2;
    yi = (exp(xr)*sin(xi)-exp(-xr)*sin(-xi))/2; 
  }
};

ArrayVector SinhFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Sinh function takes exactly one argument");
  return ArrayVector(UnaryOp<OpSinh>(arg[0]));
}

JitScalarFunc1(sinh,OpSinh::func);

//@@Signature
//function exp ExpFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_exp

struct OpExp {
  static inline float func(float x) {return expf(x);}
  static inline double func(double x) {return exp(x);}
  static void func(float xr, float xi, float &yr, float &yi) {
    yr = expf(xr)*cosf(xi);
    yi = expf(xr)*sinf(xi);
  }
  static void func(double xr, double xi, double &yr, double &yi) {
    yr = exp(xr)*cos(xi);
    yi = exp(xr)*sin(xi);
  }
};

ArrayVector ExpFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("Exp function takes exactly one argument");
  return ArrayVector(UnaryOp<OpExp>(arg[0]));
}

JitScalarFunc1(exp,OpExp::func);

//@@Signature
//function expm1 ExpM1Function jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_expm1

// exp(x)-1 for x = xr+i*xi
// exp(xr+i*xi) - 1 = exp(xr)*cos(xi) + i*exp(xr)*sin(xi) - 1

struct OpExpM1 {
  static inline float func(float x) {return expm1f(x);}
  static inline double func(double x) {return expm1(x);}
  static void func(float xr, float xi, float &yr, float &yi) {
    yr = expf(xr)*cosf(xi) - 1;
    yi = expf(xr)*sinf(xi);
  }
  static void func(double xr, double xi, double &yr, double &yi) {
    yr = exp(xr)*cos(xi) - 1;
    yi = exp(xr)*sin(xi);
  }
};

ArrayVector ExpM1Function(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("ExpM1 function takes exactly one argument");
  return ArrayVector(UnaryOp<OpExpM1>(arg[0]));
}

JitScalarFunc1(expm1,OpExpM1::func);


