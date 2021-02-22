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

#include <QImage>
#include <QImageWriter>
#include "Array.hpp"
#include "Algorithms.hpp"
#include <QtGui>
#include "PathSearch.hpp"
#include "Interpreter.hpp"
#include "Math.hpp"

//@@Signature
//sgfunction imread ImReadFunction
//inputs filename
//outputs A map alpha
//DOCBLOCK io_imread
static ArrayVector imreadHelperIndexed(QImage img) {
  QVector<QRgb> colorTable(img.colorTable());
  int numcol = colorTable.size();
  BasicArray<double> ctable_dp(NTuple(numcol,3));
  for (int i=0;i<numcol;i++) {
    QColor c(colorTable[i]);
    ctable_dp[i+1] = (double) c.redF();
    ctable_dp[i+numcol+1] = (double) c.greenF();
    ctable_dp[i+2*numcol+1] = (double) c.blueF();
  }
  Array ctable(ctable_dp);
  BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width()));
  for (int row=0;row<img.height();row++) {
    uchar *p = img.scanLine(row);
    for (int col=0;col<img.width();col++) 
      img_data_dp[row+col*img.height()+1] = p[col];
  }
  Array A(img_data_dp);
  QImage alpha(img.alphaChannel());
  BasicArray<uint8> img_alpha_dp(NTuple(img.height(),img.width()));
  for (int row=0;row<alpha.height();row++) {
    uchar *p = alpha.scanLine(row);
    for (int col=0;col<alpha.width();col++)
      img_alpha_dp[row+col*img.height()+1] = p[col];
  }
  Array trans(img_alpha_dp);
  return ArrayVector() << A << ctable << trans;
}

static ArrayVector imreadHelperRGB32(QImage img) {
  if (img.allGray()) {
    BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width()));
    for (int row=0;row<img.height();row++) {
      QRgb *p = (QRgb*) img.scanLine(row);
      for (int col=0;col<img.width();col++) {
	int ndx = row+col*img.height();
	img_data_dp[ndx+1] = qGray(p[col]);
      }
    }
    return ArrayVector() << Array(img_data_dp)
			 << EmptyConstructor() 
			 << EmptyConstructor();
  }
  else {
    BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width(),3));
    int imgcnt = img.height()*img.width();
    for (int row=0;row<img.height();row++) {
      QRgb *p = (QRgb*) img.scanLine(row);
      for (int col=0;col<img.width();col++) {
	int ndx = row+col*img.height();
	img_data_dp[ndx+1] = qRed(p[col]);
	img_data_dp[ndx+1*imgcnt+1] = qGreen(p[col]);
	img_data_dp[ndx+2*imgcnt+1] = qBlue(p[col]);
      }
    }
    return ArrayVector() << Array(img_data_dp)
			 << EmptyConstructor() 
			 << EmptyConstructor();
  }
}

static ArrayVector imreadHelperARGB32(QImage img) {
  BasicArray<uint8> img_alpha_dp(NTuple(img.height(),img.width()));
  if (img.allGray()) {
    BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width()));
    for (int row=0;row<img.height();row++) {
      QRgb *p = (QRgb*) img.scanLine(row);
      for (int col=0;col<img.width();col++) {
	int ndx = row+col*img.height();
	img_data_dp[ndx+1] = qGray(p[col]);
	img_alpha_dp[ndx+1] = qAlpha(p[col]);
      }
    }
    return ArrayVector() << Array(img_data_dp)
			 << EmptyConstructor() 
			 << Array(img_alpha_dp);
  } else {
    BasicArray<uint8> img_data_dp(NTuple(img.height(),img.width(),3));
    int imgcnt = img.height()*img.width();
    for (int row=0;row<img.height();row++) {
      QRgb *p = (QRgb*) img.scanLine(row);
      for (int col=0;col<img.width();col++) {
	int ndx = row+col*img.height();
	img_data_dp[ndx+1] = qRed(p[col]);
	img_data_dp[ndx+1*imgcnt+1] = qGreen(p[col]);
	img_data_dp[ndx+2*imgcnt+1] = qBlue(p[col]);
	img_alpha_dp[ndx+1] = qAlpha(p[col]);
      }
    }
    return ArrayVector() << Array(img_data_dp)
			 << EmptyConstructor() 
			 << Array(img_alpha_dp);
  }
}

ArrayVector ImReadFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  PathSearcher psearch(eval->getTotalPath());
  if (arg.size() == 0)
    throw Exception("imread requires a filename to read.");
  QString filename(arg[0].asString());
  QString completename;

  completename = psearch.ResolvePath(filename);
  if( completename.isNull() )
	throw Exception("unable to find file " + completename);

  // Construct the QImage object
  QImage img(completename);
  if (img.isNull())
    throw Exception("unable to read file " + completename);
  if (img.format() == QImage::Format_Invalid)
    throw Exception("file " + completename + " is invalid");
  if (img.format() == QImage::Format_Indexed8) return imreadHelperIndexed(img);
  if (img.format() == QImage::Format_RGB32) return imreadHelperRGB32(img);
  if (img.format() == QImage::Format_ARGB32) return imreadHelperARGB32(img);
  throw Exception("unsupported image format - only 8 bit indexed and 24 bit RGB and 32 bit ARGB images are supported");
  return ArrayVector();
}

//@@Signature
//sgfunction imwrite ImWriteFunction
//inputs filename A map alpha
//outputs none
//DOCBLOCK io_imwrite
static QImage imwriteHelperIndexed(Array A, Array ctable, Array trans) {
  QImage img(int(A.columns()), int(A.rows()), QImage::Format_Indexed8);
  uint8 *img_data_dp = A.real<uint8>().data();
  uint8 *ctable_dp = ctable.real<uint8>().data();
  uint8 *img_alpha_dp = trans.real<uint8>().data();

  for (int row=0;row<img.height();row++) {
    uchar *p = img.scanLine(row);
    for (int col=0;col<img.width();col++) 
      p[col] = img_data_dp[row+col*img.height()];
  }
  
  if (ctable_dp) {
    QVector<QRgb> colorTable(int(ctable.length()/3));
    int numcol = colorTable.size();
    for (int i=0;i<numcol;i++)
      colorTable[i] = qRgb(int(ctable_dp[i]),
			   int(ctable_dp[i+numcol]),
			   int(ctable_dp[i+2*numcol]));
    img.setColorTable(colorTable);
  }
  else {
    int numrow = 256;
    QVector<QRgb> colorTable(numrow);
    for (int i=0;i<numrow;i++)
      colorTable[i] = qRgb(i, i, i);
    img.setColorTable(colorTable);
  }
  
  if (img_alpha_dp) {
    QImage alpha(int(A.columns()), int(A.rows()), QImage::Format_Indexed8);
    for (int row=0;row<alpha.height();row++) {
      uchar *p = alpha.scanLine(row);
      for (int col=0;col<alpha.width();col++)
	p[col] = img_alpha_dp[row+col*img.height()];
    }
    img.setAlphaChannel(alpha);
  }
  return img;
}

static QImage imwriteHelperRGB32(Array A) {
  uint8 *img_data_dp = A.real<uint8>().data();
  QImage img(int(A.columns()), int(A.rows()), QImage::Format_RGB32);
  int imgcnt = img.height()*img.width();
  for (int row=0;row<img.height();row++) {
    QRgb *p = (QRgb*) img.scanLine(row);
    for (int col=0;col<img.width();col++) {
      int ndx = row+col*img.height();
      p[col] = qRgb(img_data_dp[ndx], 
		    img_data_dp[ndx+1*imgcnt], 
		    img_data_dp[ndx+2*imgcnt]);
    }
  }
  return img;
}

static QImage imwriteHelperARGB32(Array A, Array trans) {
  QImage img(int(A.columns()), int(A.rows()), QImage::Format_ARGB32);
  uint8 *img_data_dp = A.real<uint8>().data();
  uint8 *img_alpha_dp = trans.real<uint8>().data();
  int imgcnt = img.height()*img.width();
  for (int row=0;row<img.height();row++) {
    QRgb *p = (QRgb*) img.scanLine(row);
    for (int col=0;col<img.width();col++) {
      int ndx = row+col*img.height();
      p[col] = qRgba(img_data_dp[ndx], 
		     img_data_dp[ndx+1*imgcnt], 
		     img_data_dp[ndx+2*imgcnt], 
		     img_alpha_dp[ndx]);
    }
  }
  return img;
}

static Array convert2uint8(Array A) {
  if (A.dataClass() ==  UInt8) return A;
  if (A.dataClass() == Bool) A = A.toClass(Double);
  if ((A.dataClass() == Float) || (A.dataClass() == Double))
    return DotMultiply(A,Array(double(255.0))).toClass(UInt8);
  return A.toClass(UInt8);
}  

ArrayVector ImWriteFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  PathSearcher psearch(eval->getTotalPath());
  if (arg.size() < 2)
    throw Exception("imwrite requires at least a filename and a matrix");
  ArrayVector argCopy;
  if (arg[0].isString())
    argCopy = arg; //Octave-style syntax
  else if (arg[1].isString()) {
    argCopy << arg[1];
    for (int i = 0; i< arg.size()-1; i++){
      if (i == 1)
        continue;
      argCopy << arg[i];
    }
  }
  else if (arg[2].isString()) {
    argCopy << arg[2];
    for (int i = 0; i< arg.size()-1; i++){
      if (i == 2)
        continue;
      argCopy << arg[i];
    }
  }
  else
    throw Exception("imwrite requires a filename");
  int hasAlpha = 0;
  for (int i = 1; i< arg.size(); i++) {
    if (argCopy[i].isString() && argCopy[0].asString().toUpper() == "ALPHA") {
      hasAlpha = 1;
      break;
    }
    i++;
  }
  QString FileName = argCopy[0].asString();
  QByteArray ImageFormat;
  ImageFormat.append(QFileInfo(FileName).suffix());
  // Construct the QImageWriter object
  QImageWriter imgWriter(FileName,ImageFormat);
  if (!imgWriter.canWrite()) {
    throw Exception("unable to write image file " + FileName);
  }

  Array A(argCopy[1]);
  if (A.dimensions().lastNotOne() == 2) { // choose QImage::Format_Indexed8
    if (argCopy.size() == 2) { // 8-bit grayscale image
      Array ctable(UInt8);
      Array trans(UInt8);
      QImage img = imwriteHelperIndexed(convert2uint8(A), ctable, trans);
      if (!imgWriter.write(QImage(img)))
	throw Exception("cannot create image file" + FileName);
    }
    else if (argCopy.size() == 3) { // 8-bit indexed color image
      Array ctable(argCopy[2]);
      if (ctable.length() != 0 && ctable.cols() != 3)
	throw Exception("color map should be a 3 columns matrix");
      Array trans(UInt8);
      QImage img = imwriteHelperIndexed(convert2uint8(A), convert2uint8(ctable), trans);
      if (!imgWriter.write(img))
	throw Exception("cannot create image file" + FileName);
    }
    else if (argCopy.size() == 4) { // 8-bit indexed color image with alpha channel
      Array ctable(argCopy[2]);
      if (ctable.length() != 0 && ctable.cols() != 3)
	throw Exception("color map should be a 3 columns matrix");
      Array trans(argCopy[3]);
      eval->warningMessage("saving alpha/transparent channel will increase file size");
      QImage img = imwriteHelperIndexed(convert2uint8(A), convert2uint8(ctable), convert2uint8(trans));
      if (!imgWriter.write(img))
	throw Exception("cannot create image file" + FileName);
    }
    else
      throw Exception("invalide input number of arguments");
  }
  else if (A.dimensions().lastNotOne() == 3) { // choose QImage::Format_RGB32 or Format_ARGB32
    if (argCopy.size() == 2) {
      QImage img = imwriteHelperRGB32(convert2uint8(A));
      if (!imgWriter.write(QImage(img)))
	throw Exception("cannot create image file" + FileName);
    }
    else if (argCopy.size() == 3) {
      Array trans(argCopy[2]);
      if (A.rows() == trans.rows() && A.columns() == trans.columns() ) {
	// the third argument is alpha channel
	QImage img = imwriteHelperARGB32(convert2uint8(A), convert2uint8(trans));
	if (!imgWriter.write(QImage(img)))
	  throw Exception("cannot create image file" + FileName);
      }
      else {
	if (trans.length() != 0)
	  eval->warningMessage("ignore colormap argument");
	QImage img = imwriteHelperRGB32(convert2uint8(A));
	if (!imgWriter.write(QImage(img)))
	  throw Exception("cannot create image file" + FileName);
      }
    }
    else if (argCopy.size() == 4 || (argCopy.size() == 5 && hasAlpha == 1)) {
      Array ctable(arg[2]);
      if (ctable.length() != 0)
	eval->warningMessage("ignore colormap argument");
      Array trans(argCopy[3+hasAlpha]);
      if (A.rows() == trans.rows() && A.columns() == trans.columns() ) {
	// the third argument is alpha/transparent channel
	QImage img = imwriteHelperARGB32(convert2uint8(A), convert2uint8(trans));
	if (!imgWriter.write(QImage(img)))
          throw Exception("alpha/transparent size is not the same as image size");
      }
      else {
	if (trans.length() != 0)
          eval->warningMessage("ignore invalid alpha/transparent argument");
	QImage img = imwriteHelperRGB32(convert2uint8(A));
	if (!imgWriter.write(QImage(img)))
	  throw Exception("cannot create image file" + FileName);
      }
    }
    else
      throw Exception("invalide input number of arguments");
  }
  else
    throw Exception("invalid matrix dimension");

  return ArrayVector();
}
