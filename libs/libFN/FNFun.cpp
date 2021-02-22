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

#include "FN.hpp"
#include "Exception.hpp"
#include "Array.hpp"
#include <math.h>
#include "Operators.hpp"
#include "mathfunc5.hpp"

#if defined(_MSC_VER )
    float erff(float x);
    float erfcf(float x);
    double erf(double x);
    double erfc(double x);
    double tgamma(double x);
    float tgammaf(float x);
    double lgamma(double x);
    float lgammaf(float x);
    double trunc( double x );
    float truncf( float x );
#endif 

//@@Signature
//function erfc ErfcFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_erfc

struct OpErfc {
  static inline float func(float x) {return erfcf(x);}
  static inline double func(double x) {return erfc(x);}
  static inline void func(float, float, float&, float&) 
  { throw Exception("erfc not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("erfc not defined for complex types");}
};

ArrayVector ErfcFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("erfc requires at least one argument");
  return ArrayVector(UnaryOp<OpErfc>(arg[0]));
}

JitScalarFunc1(erfc,OpErfc::func);

//@@Signature
//function erf ErfFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_erf

struct OpErf {
  static inline float func(float x) {return erff(x);}
  static inline double func(double x) {return erf(x);}
  static inline void func(float, float, float&, float&) 
  { throw Exception("erf not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("erf not defined for complex types");}
};

ArrayVector ErfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("erf requires at least one argument");
  return ArrayVector(UnaryOp<OpErf>(arg[0]));
}

JitScalarFunc1(erf,OpErf::func);
  
//@@Signature
//function erfinv ErfInvFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_erfinv

struct OpErfInv{
  static inline float func(float x) {return erfinv(x);}
  static inline double func(double x) {return erfinv(x);}
  static inline void func(float, float, float&, float&) 
  { throw Exception("erf not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("erf not defined for complex types");}
};

ArrayVector ErfInvFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("erf requires at least one argument");
  return ArrayVector(UnaryOp<OpErfInv>(arg[0]));
}

JitScalarFunc1(erfinv,OpErfInv::func);

//@@Signature
//function gamma GammaFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_gamma

struct OpGamma {
  static inline float func(float x) {
    if ((x < 0) && (x == truncf(x))) return Inf();
    return tgammaf(x);
  }
  static inline double func(double x) {
    if ((x < 0) && (x == trunc(x))) return Inf();
    return tgamma(x);
  }
  static inline void func(float, float, float&, float&) 
  { throw Exception("gamma not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("gamma not defined for complex types");}
};

ArrayVector GammaFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("gamma requires at least one argument");
  
  ArrayVector ret;
  try{
	ret = UnaryOp<OpGamma>(arg[0]);
  }
  catch(...){
	throw Exception("Error evaluating gamma function");
  }
  return ret;
}

JitScalarFunc1(gamma,OpGamma::func);

//@@Signature
//function gammaln GammaLnFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK mathfunctions_gammaln

struct OpGammaLn {
  static inline float func(float x) {
    if (x < 0) return Inf();
    return lgammaf(x);
  }
  static inline double func(double x) {
    if (x < 0) return Inf();
    return lgamma(x);
  }
  static inline void func(float, float, float&, float&) 
  { throw Exception("gammaln not defined for complex types");}
  static inline void func(float, float, double&, double&) 
  { throw Exception("gammaln not defined for complex types");}
};

ArrayVector GammaLnFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 1)
    throw Exception("gammaln requires at least one argument");
  return ArrayVector(UnaryOp<OpGammaLn>(arg[0]));
}

JitScalarFunc1(gammaln,OpGammaLn::func);

//@@Signature
//function betainc BetaIncFunction
//inputs varargin
//outputs y
//DOCBLOCK mathfunctions_betainc
#ifdef HAVE_BOOST
#include <boost/math/special_functions/beta.hpp>

ArrayVector BetaIncFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("betainc requires at least three arguments");
  ArrayVector retVec;
  Array X( arg[0] );
  Array Y( arg[1] );
  Array Z( arg[2] );
  int maxLen = std::max( X.length(), std::max( Y.length(), Z.length() ) );
  NTuple retDims = max( X.dimensions(), max( Y.dimensions(), Z.dimensions() ) );
  
  if( !(X.isScalar()) && retDims != X.dimensions() )
    throw Exception("wrong size of the first argument");
  if( !(Y.isScalar()) && retDims != Y.dimensions() )
    throw Exception("wrong size of the second argument");
  if( !(Z.isScalar()) && retDims != Z.dimensions() )
    throw Exception("wrong size of the third argument");
  
  if( X.dataClass() == Double && Y.dataClass() == Double && Z.dataClass() == Double ){
    BasicArray< double > result( retDims );
    for( int i = 1; i <= maxLen; ++i ){
      double x,y,z,r;
      x = (X.isScalar()) ? X.constRealScalar<double>() : X.real<double>()[i];
      y = (Y.isScalar()) ? Y.constRealScalar<double>() : Y.real<double>()[i];
      z = (Z.isScalar()) ? Z.constRealScalar<double>() : Z.real<double>()[i];
	  try{
		result[i] = boost::math::ibeta( y, z, x );
	  }
	  catch(...){
		throw Exception("Error evaluating ibeta");
      }
	}
    retVec.push_back( result );
  }
  else if( X.dataClass() == Float && Y.dataClass() == Float && Z.dataClass() == Float ){
    BasicArray< float > result( retDims );
    for( int i = 1; i <= maxLen; ++i ){
      float x,y,z,r;
      x = (X.isScalar()) ? X.realScalar<float>() : X.real<float>()[i];
      y = (Y.isScalar()) ? Y.realScalar<float>() : Y.real<float>()[i];
      z = (Z.isScalar()) ? Z.realScalar<float>() : Z.real<float>()[i];
	  try{
		result[i] = boost::math::ibeta( y, z, x );
	  }
	  catch(...){
		throw Exception("Error evaluating ibeta");
	  }
    }
    retVec.push_back( result );
  }
  else{
    throw Exception("Inputs must be either double or single");
  }
  return retVec;
}
#else
ArrayVector BetaIncFunction(int nargout, const ArrayVector& arg) {
    throw Exception("FreeMat must be compiled with boost to enable betainc");
    return ArrayVector();
}
#endif

//@@Signature
//function legendre LegendreFunction
//inputs varargin
//outputs y
//DOCBLOCK mathfunctions_legendre

#ifdef HAVE_BOOST
#include <boost/math/special_functions/legendre.hpp>
ArrayVector LegendreFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() < 2)
    throw Exception("Legendre function requires at least two argument");
  Array x( arg[1] );
  Array n( arg[0] );
  NTuple retDims;
  ArrayVector retVec;
  
  if( x.isComplex() )
    throw Exception("Second argument must be real");

  if( n.isComplex() )
    throw Exception("First argument must be real integer");
  
  if( x.dimensions().count() > 1 && !n.isScalar() )
    throw Exception("If second argument is a not a vector or scalar, then first argument must be scalar");
  
  if( (n.dimensions().count() == 2 && n.dimensions()[1] != x.length()) || (n.dimensions().count() > 2) )
    throw Exception("Incompatible dimensions between first and second argument");
  
  if( n.isScalar() )
    retDims = x.dimensions();
  else
    retDims = n.dimensions();
  
  if( x.dataClass() == Double ){
    BasicArray< double > result( retDims );
    if( n.isScalar() ){
      for( int i=1; i<=x.length(); ++i ){
	double xt = (x.isScalar()) ? x.constRealScalar<double>() : x.real<double>()[i];
	try{
		result[i]=boost::math::legendre_p<double>(n.constRealScalar<double>(), xt);
	}
	catch(...){
		throw Exception("Error evaluating legendre");
	}
      }
    }
    retVec.push_back(result); 
  }
  else if( x.dataClass() == Float ){
    BasicArray< float > result( retDims );
    if( n.isScalar() ){
      for( int i=1; i<=x.length(); ++i ){
	float xt = (x.isScalar()) ? x.constRealScalar<float>() : x.real<float>()[i];
	try{
		result[i]=boost::math::legendre_p<float>(n.constRealScalar<float>(), xt);
	}
	catch(...){
		throw Exception("Error evaluating legendre");
	}
      }
    }
    retVec.push_back(result); 
  }
  else
    throw Exception("Second argument must be double or single");
  return retVec;
}
#else
ArrayVector LegendreFunction(int nargout, const ArrayVector& arg) {
    throw Exception("FreeMat must be compiled with boost to enable legendre");
    return ArrayVector();
}
#endif
