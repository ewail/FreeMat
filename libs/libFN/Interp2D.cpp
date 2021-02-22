/*
 * Copyright (c) 2010 Chuong Nguyen, Samit Basu
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
static void DoMultiInterpolation2DReal(const T* z, int zrows, int zcols,
                       const T* xi, const T* yi, int xicount,
                       T* zi, int method, double xtrapval) {

    int ii, jj, nn, xifloor, yifloor;
    register T dx, dy;

    switch (method) {
        default:
            throw Exception("unhandled type in argument to DoMultiInterpolation2DReal");
        case 0: //nearest
            for (ii = 0; ii< xicount; ii++) {
                if (xi[ii] < 1 || xi[ii] > zcols || yi[ii] < 1 || yi[ii] > zrows )
                    zi[ii] = xtrapval;
                else {
                    dx = xi[ii];
                    dy = yi[ii];
                    xifloor = int(dx);
                    yifloor = int(dy);
                    dx = dx - xifloor;
                    dy = dy - yifloor;
                    if (dx >= 0.5)
                        xifloor++;
                    if (dy >= 0.5)
                        yifloor++;
                    nn = (xifloor-1)*zrows + yifloor-1;
                    zi[ii] = z[nn];
                }
            }
            break;
        case 1: //linear
            for (ii = 0; ii< xicount; ii++) {
                if (xi[ii] < 1 || xi[ii] > zcols || yi[ii] < 1 || yi[ii] > zrows )
                    zi[ii] = xtrapval;
                else {
                    dx = xi[ii];
                    dy = yi[ii];
                    xifloor = int(dx);
                    yifloor = int(dy);
                    dx = dx - xifloor;
                    dy = dy - yifloor;
                    nn = (xifloor-1)*zrows + yifloor-1;
                    if (dx == 0)
                        zi[ii] = z[nn]*(1-dy) + z[nn+1]*dy;
                    else if (dy == 0)
                        zi[ii] = z[nn]*(1-dx) + z[nn+zrows]*dx;
                    else {
                        zi[ii] = (z[nn      ]*(1-dy) + z[nn+1      ]*dy)*(1-dx) +
                                 (z[nn+zrows]*(1-dy) + z[nn+1+zrows]*dy)*dx;
                    }
                }
            }
            break;
        case 2: //cubic
            double xw0, xw1, xw2, xw3;
            double yw0, yw1, yw2, yw3;
            T const *pz;
            register T *pzz;
            //create a temporary array zz to hold z and extra boundary elements
            int zzrows = zrows+2;
            int zzcols = zcols+2;
            T *zz = (T*) malloc(sizeof(T)*(zzrows)*(zzcols));
            if (zz == NULL)
                throw Exception("unable to locate memory in DoMultiInterpolation2DReal");
            pz  = z;
            pzz = zz;
            pzz+=zzrows; //skip the first column of zz
            // copy z into the center of zz
            for (ii = 0; ii< zcols; ii++) {
                pzz++; //skip top element of zz
                for (jj = 0; jj< zrows; jj++) {
                    *(pzz++) = *(pz++);
                }
                pzz++; //skip bottom element of zz
            }
            #define data(i,j)   (zz[(i)*zzrows+(j)])
            //filling extra elements on the boundary
            for (ii = 0; ii< zzcols; ii++) {
                //top and bottom elements
                data(ii,0)        = 3*data(ii,       1) - 3*data(ii,       2) + data(ii,       3);
                data(ii,zzrows-1) = 3*data(ii,zzrows-2) - 3*data(ii,zzrows-3) + data(ii,zzrows-4);
            }
            for (jj = 0; jj< zzrows; jj++) {
                //left and right elements
                data(0,jj)        = 3*data(1,       jj) - 3*data(2,       jj) + data(3,       jj);
                data(zzcols-1,jj) = 3*data(zzcols-2,jj) - 3*data(zzcols-3,jj) + data(zzcols-4,jj);
            }
            #undef data
            //interpolate at given points
            for (ii = 0; ii< xicount; ii++) {
                if (xi[ii] < 1 || xi[ii] > zcols || yi[ii] < 1 || yi[ii] > zrows )
                    //assign to xtrapval for out of range elements
                    zi[ii] = xtrapval;
                else {
                    dx = xi[ii];
                    dy = yi[ii];
                    xifloor = int(dx);
                    yifloor = int(dy);
                    dx = dx - xifloor;
                    dy = dy - yifloor;
                    //generate interpolation coefficients
                    if (dx != 0) {
                        xw0 = ((2.0-dx)*dx-1.0)*dx;
                        xw1 = (3.0*dx-5.0)*dx*dx+2.0;
                        xw2 = ((4.0-3.0*dx)*dx+1.0)*dx;
                        xw3 = (dx-1.0)*dx*dx;
                    }
                    if (dy != 0) {
                        yw0 = ((2.0-dy)*dy-1.0)*dy;
                        yw1 = (3.0*dy-5.0)*dy*dy+2.0;
                        yw2 = ((4.0-3.0*dy)*dy+1.0)*dy;
                        yw3 = (dy-1.0)*dy*dy;
                    }
                    //obtain interpolated value
                    pzz = zz + xifloor*zzrows + yifloor;
                    # define data(i,j) (pzz[(i)*zzrows+(j)])
                    if (dx == 0)
                        if (dy == 0)
                            zi[ii] = data(0,0);
                        else
                            zi[ii] = (yw0*data(0,-1) + yw1*data(0,0) + yw2*data(0,1) + yw3*data(0,2))*0.5;
                    else if (dy == 0)
                        zi[ii] = (xw0*data(-1,0) + xw1*data(0,0) + xw2*data(1,0) + xw3*data(2,0))*0.5;
                    else
                        zi[ii] = (( xw0*data(-1,-1) + xw1*data(0,-1) + xw2*data(1,-1) + xw3*data(2,-1) )*yw0 +
                                  ( xw0*data(-1, 0) + xw1*data(0, 0) + xw2*data(1, 0) + xw3*data(2, 0) )*yw1 +
                                  ( xw0*data(-1, 1) + xw1*data(0, 1) + xw2*data(1, 1) + xw3*data(2, 1) )*yw2 +
                                  ( xw0*data(-1, 2) + xw1*data(0, 2) + xw2*data(1, 2) + xw3*data(2, 2) )*yw3)*0.25;
                    #undef data
                }
            }
            //free temporary array
            if (zz != NULL){
                free(zz);
                zz = NULL;
            }
            pzz = NULL;
            break;
    }//end of switch
}//end of DoMultiInterpolation2DReal

template <typename T>
static BasicArray<T> Minterp2(const BasicArray<T> &z, const BasicArray<T> &xi, const BasicArray<T> &yi, int method, double xtrapflag) {
  BasicArray<T> zi(xi.dimensions());
  DoMultiInterpolation2DReal(z.constData(),int(z.rows()),int(z.cols()),
                            xi.constData(),yi.constData(),int(xi.length()),
                            zi.data(), method, xtrapflag);
  return zi;
}

//@@Signature
//function interp2 Interp2Function jitsafe
//inputs z xi yi method extrapflag
//outputs zi
//DOCBLOCK curvefit_interp2
ArrayVector Interp2Function(int nargout, const ArrayVector& arg) {
  if (arg.size() < 3)
    throw Exception("interp2 requires at least three arguments (z,xi,yi)");
  Array z(arg[0].toClass(Double));
  Array xi(arg[1].toClass(Double));
  Array yi(arg[2].toClass(Double));
  z  = z.asDenseArray();
  xi = xi.asDenseArray();
  yi = yi.asDenseArray();

  if (xi.isComplex() || yi.isComplex())
    throw Exception("xi and yi cannot be complex in interp2");
  // Make sure xi and yi are the same size
  if (xi.cols() != yi.cols() || xi.rows() != yi.rows())
    throw Exception("vectors xi and yi must be the same size");
  // Check for extrapolation flag
  int method = 1; //default linear
  if (arg.size() >= 4) {
    if (!arg[3].isString())
      throw Exception("method must be a string");
    QString method_c = arg[3].asString();
    if (method_c=="nearest")
      method = 0;
    else if (method_c=="linear")
      method = 1;
    else if (method_c=="cubic")
      method = 2;
    else
      throw Exception("unrecognized method to routine interp2");
  }
  // Check for extrapolation value
  double xtrap = NaN();
  if (arg.size() >= 5) {
    if (!arg[4].isScalar())
      throw Exception("extrapolation value must be a number");
    xtrap = arg[4].asDouble();
  }
  if (z.allReal()) {
    return ArrayVector(Array(Minterp2(z.constReal<double>(),xi.constReal<double>(),yi.constReal<double>(),method,xtrap)));
  }
  else {
    return ArrayVector(Array(Minterp2(z.constReal<double>(),xi.constReal<double>(),yi.constReal<double>(),method,xtrap),
                             Minterp2(z.constImag<double>(),xi.constReal<double>(),yi.constReal<double>(),method,xtrap)));
  }
}
