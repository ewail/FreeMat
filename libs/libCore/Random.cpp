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
#include <math.h>
#include <stdio.h>
#include "RanLib.hpp"
#include "Operators.hpp"
#include "Utils.hpp"

static bool initialized = false;

//@@Signature
//function seed SeedFunction jitsafe
//inputs s t
//outputs none
//DOCBLOCK random_seed
ArrayVector SeedFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("Seed function requires a two integer arguments");
  uint32 seedval1;
  uint32 seedval2;
  seedval1 = (uint32) arg[0].asInteger();
  seedval2 = (uint32) arg[1].asInteger();
  setall(seedval1,seedval2);
  init_genrand(seedval1);
  initialized = true;
  return ArrayVector();
}

//@@Signature
//function randbeta RandBetaFunction jitsafe
//inputs alpha beta
//outputs y
//DOCBLOCK random_randbeta

struct OpRandBeta {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,float>(genbet(float(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T& ar, const T& ai, const T& br, const T& bi, T& cr, T& ci) {
    throw Exception("randbeta is not supported for complex arguments");
  }
};

ArrayVector RandBetaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randbeta requires two parameter arguments");
  return DotOp<OpRandBeta>(arg[0],arg[1]);
}

//@@Signature
//function randi RandIFunction jitsafe
//inputs low high
//outputs y
//DOCBLOCK random_randi
struct OpRandI {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,int32>(ignuin(long(v1),long(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randi is not supported for complex arguments");
  }
};

ArrayVector RandIFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randi requires two parameter arguments");
  return DotOp<OpRandI>(arg[0],arg[1]);
}
  
//@@Signature
//function randchi RandChiFunction jitsafe
//inputs n
//outputs y
//DOCBLOCK random_randchi
struct OpRandChi {
  template <typename T>
  static inline T func(const T& v1) {
    if (v1 <= 0) throw Exception("argument to randchi must be positive");
    return CastConvert<T,float>(genchi(float(v1)));
  }
  template <typename T>
  static inline void func(const T&, const T&, T&, T&) {
    throw Exception("randchi is not supported for complex arguments");
  }
};

ArrayVector RandChiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randchi requires exactly one parameter (the vector of degrees of freedom)");
  return UnaryOp<OpRandChi>(arg[0]);
}

//@@Signature
//function randexp RandExpFunction jitsafe
//inputs lambda
//outputs y
//DOCBLOCK random_randexp
struct OpRandExp {
  template <typename T>
  static inline T func(const T& v1) {
    return CastConvert<T,float>(genexp(float(v1)));
  }
  template <typename T>
  static inline void func(const T&, const T&, T&, T&) {
    throw Exception("randexp is not supported for complex arguments");
  }
};

ArrayVector RandExpFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randexp requires exactly one parameter (the vector of means)");
  return UnaryOp<OpRandExp>(arg[0]);
}

//@@Signature
//function randp RandPoissonFunction jitsafe
//inputs n
//outputs y
//DOCBLOCK random_randp
struct OpRandPoisson {
  template <typename T>
  static inline T func(const T& v1) {
    return CastConvert<T,int32>(ignpoi(float(v1)));
  }
  template <typename T>
  static inline void func(const T&, const T&, T&, T&) {
    throw Exception("randp is not supported for complex arguments");
  }
};

ArrayVector RandPoissonFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("randp requires exactly one parameter (the vector of means)");
  return UnaryOp<OpRandPoisson>(arg[0]);
}

//@@Signature
//function randbin RandBinFunction jitsafe
//inputs N p
//outputs y
//DOCBLOCK random_randbin
struct OpRandBin {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,int32>(ignbin(uint32(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randbin is not supported for complex arguments");
  }
};

ArrayVector RandBinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randbin requires two parameter arguments");
  return DotOp<OpRandBin>(arg[0],arg[1]);
}

//@@Signature
//function randnbin RandNBinFunction jitsafe
//inputs r p
//outputs y
//DOCBLOCK random_randnbin
struct OpRandNBin {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,int32>(ignnbn(uint32(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randnbin is not supported for complex arguments");
  }
};

ArrayVector RandNBinFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randnbin requires two parameter arguments");
  return DotOp<OpRandNBin>(arg[0],arg[1]);
}

//@@Signature
//function randf RandFFunction jitsafe
//inputs n m
//outputs y
//DOCBLOCK random_randf
struct OpRandF {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    if ((v1 <= 0) || (v2 <= 0)) throw Exception("randf requires positive arguments");
    return CastConvert<T,float>(genf(float(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randf is not supported for complex arguments");
  }  
};

ArrayVector RandFFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randf requires two parameter arguments");
  return DotOp<OpRandF>(arg[0],arg[1]);
}

//@@Signature
//function randgamma RandGammaFunction jitsafe
//inputs a r
//outputs y
//DOCBLOCK random_randgamma
struct OpRandGamma {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    return CastConvert<T,float>(gengam(float(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randgamma is not supported for complex arguments");
  }  
};

ArrayVector RandGammaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randgamma requires two parameter arguments");
  return DotOp<OpRandGamma>(arg[0],arg[1]);
}

//@@Signature
//function randmulti RandMultiFunction jitsafe
//inputs N pvec
//outputs y
//DOCBLOCK random_randmulti
ArrayVector RandMultiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randmulti requires two parameter arguments");
  if (arg[0].isEmpty() || arg[1].isEmpty())
    return ArrayVector(EmptyConstructor());
  int N = arg[0].asInteger();
  if (N<0) 
    throw Exception("number of events to generate for randmulti must be a nonnegative integer");
  Array arg2 = arg[1].toClass(Float);
  if (arg2.isSparse()) throw Exception("randmulti does not work with sparse arguments");
  BasicArray<float> &dp(arg2.real<float>());
  float Psum = 0;
  for (index_t i=1;i<=arg2.length();i++) {
    if ((dp[i] < 0) || (dp[i] > 1))
      throw Exception("probability vector argument to randmulti must have all elements between 0 and 1");
    Psum += dp[i];
  }
  for (index_t i=1;i<=arg2.length();i++) 
    dp[i] /= Psum;
  NTuple outDims(arg2.dimensions());
  BasicArray<int32> rp(outDims);
  genmul(N,dp.data(),int(arg2.length()),(long*)rp.data());
  return ArrayVector(Array(rp).toClass(Double));
}
  
//@@Signature
//function randnchi RandNChiFunction jitsafe
//inputs n mu
//outputs y
//DOCBLOCK random_randnchi
struct OpRandNChi {
  template <typename T>
  static inline T func(const T& v1, const T& v2) {
    if (v1 <= 1) throw Exception("degrees of freedom argument must be > 1");
    if (v2 < 0) throw Exception("noncentrality parameter must be positive");
    return CastConvert<T,float>(gennch(float(v1),float(v2)));
  }
  template <typename T>
  static inline void func(const T&, const T&, const T&, const T&, T&, T&) {
    throw Exception("randnchi is not supported for complex arguments");
  }  
};

ArrayVector RandNChiFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 2)
    throw Exception("randnchi requires two parameter arguments");
  return DotOp<OpRandNChi>(arg[0],arg[1]);
}

//@@Signature
//function randnf RandNFFunction jitsafe
//inputs n m c
//outputs y
//DOCBLOCK random_randnf
ArrayVector RandNFFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 3)
    throw Exception("randnf requires three parameter arguments");
  Array arg1(arg[0].asDenseArray().toClass(Float));
  Array arg2(arg[1].asDenseArray().toClass(Float));
  Array arg3(arg[2].asDenseArray().toClass(Float));
  if (arg1.isEmpty() || arg2.isEmpty() || arg3.isEmpty()) 
    return ArrayVector(EmptyConstructor());
  int arg1_advance = (arg1.isScalar()) ? 0 : 1;
  int arg2_advance = (arg2.isScalar()) ? 0 : 1;
  int arg3_advance = (arg3.isScalar()) ? 0 : 1;
  if (arg1_advance && arg2_advance && (arg1.dimensions() != arg2.dimensions()))
    throw Exception("vector arguments to randnf must be the same size");
  if (arg1_advance && arg3_advance && (arg1.dimensions() != arg3.dimensions()))
    throw Exception("vector arguments to randnf must be the same size");
  if (arg2_advance && arg3_advance && (arg2.dimensions() != arg3.dimensions()))
    throw Exception("vector arguments to randnf must be the same size");
  // Output dimension is the larger of the two
  NTuple outDims;
  if ((arg1.length() > arg2.length()) && (arg1.length() > arg3.length()))
    outDims = arg1.dimensions();
  else if ((arg2.length() > arg1.length()) && (arg2.length() > arg3.length()))
    outDims = arg2.dimensions();
  else if ((arg3.length() > arg1.length()) && (arg3.length() > arg2.length()))
    outDims = arg3.dimensions();
  if (arg1.isScalar()) arg1 = Uniform(outDims,arg1.constRealScalar<float>());
  if (arg2.isScalar()) arg2 = Uniform(outDims,arg2.constRealScalar<float>());
  if (arg3.isScalar()) arg3 = Uniform(outDims,arg3.constRealScalar<float>());
  BasicArray<float> dp(outDims);
  const BasicArray<float> &p1(arg1.constReal<float>());
  const BasicArray<float> &p2(arg2.constReal<float>());
  const BasicArray<float> &p3(arg3.constReal<float>());
  for (index_t i=1;i<=dp.length();i++) {
    if (p1[i] <= 1.0) throw Exception("numerator degrees of freedom must be > 1.0");
    if (p2[i] <= 0.0) throw Exception("denominator degrees of freedom must be positive");
    if (p3[i] < 0.0) throw Exception("noncentrality parameter must be non-negative");
    dp[i] = gennf(p1[i],p2[i],p3[i]);
  }
  return ArrayVector(Array(dp));
}

static void InitializeRandGen() {
  unsigned long init[4]={0x923, 0x234, 0x405, 0x456}, length=4;
  init_by_array(init, length);
  initialized = true;
}

static ArrayVector RandStateControl(const ArrayVector& arg) {
  QString key = arg[0].asString().toUpper();
  if (!(key=="STATE"))
    throw Exception("expecting string 'state' as first argument");
  if (arg.size() == 1) {
    BasicArray<uint32> mp(NTuple(625,1));
    GetRandStateVect(mp.data());
    return ArrayVector(Array(mp).toClass(Double));
  } else {
    Array statevec(arg[1]);
    if ((statevec.isScalar()) && (statevec.asInteger() == 0)) {
      InitializeRandGen();
      return ArrayVector();
    } else {
      statevec = statevec.toClass(UInt32);
      if (statevec.length() != 625)
	throw Exception("illegal state vector - must be of length 625");
      SetRandStateVect(statevec.real<uint32>().data());
      return ArrayVector();
    }
  }
}

//@@Signature
//function randn RandnFunction jitsafe
//inputs varargin
//outputs y
//DOCBLOCK random_randn
ArrayVector RandnFunction(int nargout, const ArrayVector& arg) {
  if (!initialized) 
    InitializeRandGen();
  if ((arg.size() > 0) && (arg[0].isString()))
    return RandStateControl(arg);
  NTuple dim(ArrayVectorAsDimensions(arg));
  BasicArray<double> qp(dim);
  index_t len = dim.count();
  for (index_t j=1;j<=len;j+=2) {
    double x1, x2, w, y1, y2;
    do {
      x1 = 2.0*genrand_res53()-1.0;
      x2 = 2.0*genrand_res53()-1.0;
      w = x1 * x1 + x2 * x2;
    } while ( w >= 1.0 );
    w = sqrt( (-2.0 * log( w ) ) / w );
    y1 = x1 * w;
    y2 = x2 * w;
    qp[j] = y1;
    if (j<=(len-1))
      qp[j+1] = y2;
  }
  return ArrayVector(Array(qp));
}

//@@Signature
//function rand RandFunction jitsafe
//inputs varargin
//outputs y
//DOCBLOCK random_rand
ArrayVector RandFunction(int nargout, const ArrayVector& arg) {
  if (!initialized)
    InitializeRandGen();
  if ((arg.size() > 0) && (arg[0].isString()))
    return RandStateControl(arg);
  NTuple dim(ArrayVectorAsDimensions(arg));
  BasicArray<double> qp(dim);
  for (index_t j=1;j<=qp.length();j++) 
    qp[j] = genrand_res53();
  return ArrayVector(Array(qp));
}
