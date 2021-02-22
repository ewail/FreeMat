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
#include "Stats.hpp"
#include "ArrayPrivate.hpp"
#include "Complex.hpp"

QVariant ComplexVariant(double real, double imag) {
  if (imag == 0) 
    return QVariant(real);
  if (imag > 0)
    return QVariant(QString("%1+%2i").arg(real).arg(imag));
  else
    return QVariant(QString("%1%2i").arg(real).arg(imag));
}

template <typename T>
static QList<QVariant> TComputeVariableStatsReal(const BasicArray<T> &dp) {
  QVariant vmin, vmax, vrange, vmean, vstd, vvar;
  T dmin, dmax;
  double dmean;
  bool init = false;
  index_t len = dp.length();
  double norm = 1.0/len;
  for (index_t i=1;i<=len;i++) {
    T val = dp[i];
    if (!IsNaN(val)) {
      if (!init) {
	init = true;
	dmin = val;
	dmax = val;
	dmean = val*norm;
      } else {
	if (val < dmin) dmin = val;
	if (val > dmax) dmax = val;
	dmean += val*norm;
      }
    }
  }
  if (init) {
    vmin = QVariant(dmin);
    vmax = QVariant(dmax);
    vrange = QVariant(dmax - dmin);
    vmean = QVariant(dmean);
  }
  if (init && (len > 1)) {
    double vnorm = 1.0/(len-1.0);
    double dvar = 0;
    for (index_t i=1;i<=len;i++) {
      T val = dp[i] - dmean;
      dvar += val*val*vnorm;
    }
    vvar = QVariant(dvar);
    vstd = QVariant(sqrt(dvar));
  }					       
  return QList<QVariant>() << vmin << vmax << vrange << vmean << vstd << vvar;
}

template <typename T>
static QList<QVariant> TComputeVariableStatsComplex(const BasicArray<T> &dp_real, 
						    const BasicArray<T> &dp_imag) {
  QVariant vmin, vmax, vrange, vmean, vstd, vvar;
  T dmin_real = 0, dmax_real = 0; double dmean_real = 0;
  T dmin_imag = 0, dmax_imag = 0; double dmean_imag = 0;
  index_t len = dp_real.length();
  bool init = false;
  double norm = 1.0/len;
  for (index_t i=1;i<=len;i++) {
    T val_real = dp_real[i];
    T val_imag = dp_imag[i];
    if (!IsNaN(val_real) && !IsNaN(val_imag)) {
      if (!init) {
	init = true;
	dmin_real = val_real; dmin_imag = val_imag;
	dmax_real = val_real; dmax_imag = val_imag;
	dmean_real = val_real*norm; dmean_imag = val_imag*norm;
      } else {
	if (complex_lt(val_real,val_imag,dmin_real,dmin_imag)) {
	  dmin_real = val_real;
	  dmin_imag = val_imag;
	}
	if (complex_gt(val_real,val_imag,dmax_real,dmax_imag)) {
	  dmax_real = val_real;
	  dmax_imag = val_imag;
	}
	dmean_real += val_real*norm; dmean_imag += val_imag*norm;
      }
    }
  }
  if (init) {
    vmin = ComplexVariant(dmin_real,dmin_imag);
    vmax = ComplexVariant(dmax_real,dmax_imag);
    vrange = ComplexVariant(dmax_real - dmin_real,dmax_imag - dmin_imag);
    vmean = ComplexVariant(dmean_real,dmean_imag);
  }
  if (init && (len > 1)) {
    double vnorm = 1.0/(len-1.0);
    double dvar = 0;
    for (index_t i=1;i<=len;i++) {
      T val_real = dp_real[i] - dmean_real;
      T val_imag = dp_imag[i] - dmean_imag;
      dvar += complex_abs(val_real,val_imag)*vnorm;
    }
    vvar = QVariant(dvar);
    vstd = QVariant(sqrt(dvar));
  }					       
  return QList<QVariant>() << vmin << vmax << vrange << vmean << vstd << vvar;  
}

template <typename T>
static QList<QVariant> TComputeVariableStats(const Array *x) {
  if (x->allReal()) {
    return TComputeVariableStatsReal<T>(x->constReal<T>());
  } else {
    return TComputeVariableStatsComplex<T>(x->constReal<T>(),
					   x->constImag<T>());
  }
}

#define MacroTCompute(ctype,cls)  \
  case cls: return TComputeVariableStats<ctype>(x);

QList<QVariant> ComputeVariableStats(const Array *x) {
  QList<QVariant> empty;
  empty << QVariant() << QVariant() << QVariant() << QVariant() << QVariant() << QVariant();
  if (x->isSparse() || x->isString() ||
      x->isReferenceType() || x->isUserClass() ||
      x->isEmpty() || x->length() > 5000 || x->isScalar()) {
    return empty;
  } else {
    switch (x->dataClass()) {
    default:
      return empty;
      MacroExpandCasesNoBool(MacroTCompute);
    }
  }
}
