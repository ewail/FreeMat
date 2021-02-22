/*
 * Copyright (c) 2002-2006 Samit Basu
 * Copyright (c) 2006 Thomas Beutlich
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
#include "HandleList.hpp"
#include "Interpreter.hpp"
#include "Serialize.hpp"
#include "Print.hpp"
#include "Utils.hpp"
#include "MatIO.hpp"
#include <QtCore>
#include "Algorithms.hpp"

//@@Signature
//function format FormatFunction
//inputs format exptype
//outputs format
//DOCBLOCK io_format
ArrayVector FormatFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() > 0) {
    QString argtxt;
    for (int i=0;i<arg.size();i++) argtxt += arg[i].asString().toUpper();
    if (argtxt == "NATIVE") SetPrintFormatMode(format_native);
    else if (argtxt == "SHORT") SetPrintFormatMode(format_short);
    else if (argtxt == "LONG") SetPrintFormatMode(format_long);
    else if (argtxt == "SHORTE") SetPrintFormatMode(format_short_e);
    else if (argtxt == "LONGE") SetPrintFormatMode(format_long_e);
    else throw Exception("unrecognized argument to the format command");
  }
  if (nargout > 0) {
    switch(GetPrintFormatMode()) {
    case format_native:
      return ArrayVector(Array(QString("native")));
    case format_short:
      return ArrayVector(Array(QString("short")));
    case format_long:
      return ArrayVector(Array(QString("long")));
    case format_short_e:
      return ArrayVector(Array(QString("short e")));
    case format_long_e:
      return ArrayVector(Array(QString("long e")));
    }
    return ArrayVector(Array(QString("unknown?")));
  }
  return ArrayVector();
}


//@@Signature
//sfunction setprintlimit SetPrintLimitFunction
//inputs linecount
//outputs none
//DOCBLOCK io_setprintlimit
ArrayVector SetPrintLimitFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  if (arg.size() != 1)
    throw Exception("setprintlimit requires one, scalar integer argument");
  eval->setPrintLimit(arg[0].asInteger());
  return ArrayVector();
}

//@@Signature
//sfunction getprintlimit GetPrintLimitFunction
//inputs none
//outputs linecount
//DOCBLOCK io_getprintlimit
ArrayVector GetPrintLimitFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  return ArrayVector(Array(double(eval->getPrintLimit())));
}
  
static ArrayVector SaveNativeFunction(QString filename, StringVector names, Interpreter* eval) {
  QFile ofile(filename);
  if (!ofile.open(QIODevice::WriteOnly))
    throw Exception("Unable to open " + filename + " for saving");
  Serialize output(&ofile);
  output.handshakeServer();
  Context *cntxt = eval->getContext();
  for (int i=0;i<names.size();i++) {
    ArrayReference toWrite;
    char flags;
    if (!(names[i].compare("ans") == 0)) {
      toWrite = cntxt->lookupVariable(names[i]);
      if (!toWrite.valid())
	throw Exception(QString("unable to find variable ")+
			names[i]+" to save to file "+filename);
      flags = 'n';
      if (cntxt->isVariableGlobal(names[i]))
	flags = 'g';
      if (cntxt->isVariablePersistent(names[i]))	
	flags = 'p';
      output.putString(names[i]);
      output.putByte(flags);
      output.putArray(*toWrite);
    }
  }
  output.putString("__eof");
  return ArrayVector();
}
  
static ArrayVector SaveASCIIFunction(QString filename, StringVector names, bool tabsMode,
				     bool doubleMode, Interpreter* eval) {
  QFile fp(filename);
  if (!fp.open(QIODevice::WriteOnly))
    throw Exception("unable to open file " + filename + " for writing.");
  Context *cntxt = eval->getContext();
  for (int i=0;i<names.size();i++) {
    if (!(names[i] == "ans")) {
      ArrayReference toWrite = cntxt->lookupVariable(names[i]);
      if (!toWrite.valid())
	throw Exception("unable to find variable " + names[i] + 
			" to save to file "+filename);
      if (toWrite->isReferenceType()) {
	eval->warningMessage("variable " + names[i] + " is not numeric - cannot write it to an ASCII file");
	continue;
      }
      if (!toWrite->is2D()) {
	eval->warningMessage("variable " + names[i] + " is not 2D - cannot write it to an ASCII file");
	continue;
      }
      if (toWrite->isComplex()) 
	eval->warningMessage("variable " + names[i] + " is complex valued - only real part will be written to ASCII file");
      Array A(*toWrite); A = A.toClass(Double).asDenseArray();
      int rows = int(A.rows());
      int cols = int(A.columns());
      const BasicArray<double> &dp(A.constReal<double>());
      QTextStream out(&fp);
      if (doubleMode)
	out.setRealNumberPrecision(15);
      else
	out.setRealNumberPrecision(7);
      for (int i=0;i<rows;i++) {
	for (int j=0;j<cols;j++) {
	  out << dp[j*rows+i+1];
	  if (tabsMode && (j < (cols-1)))
	    out << "\t";
	  else
	    out << " ";
	}
	out << "\n";
      }
    }
  }
  return ArrayVector();
}

//@@Signature
//sfunction save SaveFunction
//inputs varargin
//outputs none
//DOCBLOCK io_save
ArrayVector SaveFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  ArrayVector argCopy;
  if (arg.size() == 0) return ArrayVector();
  bool asciiMode = false;
  bool tabsMode = false;
  bool doubleMode = false;
  bool matMode = false;
  bool regexpMode = false;
  for (int i=0;i<arg.size();i++) {
    if (arg[i].isString()) {
      if (arg[i].asString().toUpper() == "-MAT")
	matMode = true;
      else if (arg[i].asString().toUpper() == "-ASCII")
	asciiMode = true;
      else if (arg[i].asString().toUpper() == "-REGEXP")
	regexpMode = true;
      else if (arg[i].asString().toUpper() == "-DOUBLE")
	doubleMode = true;
      else if (arg[i].asString().toUpper() == "-TABS")
	tabsMode = true;
      else
	argCopy << arg[i];
    } else
      argCopy << arg[i];
  }
  if (argCopy.size() < 1) throw Exception("save requires a filename argument");
  QString fname(argCopy[0].asString());
  if (!asciiMode && !matMode) {
    if (fname.endsWith(".mat",Qt::CaseInsensitive))
      matMode = true;
    if (fname.endsWith(".txt",Qt::CaseInsensitive))
      asciiMode = true;
  }
  StringVector names;
  for (int i=1;i<argCopy.size();i++) {
    if (!arg[i].isString())
      throw Exception("unexpected non-string argument to save command");
    names << argCopy[i].asString();
  }
  Context *cntxt = eval->getContext();
  ParentScopeLocker lock(cntxt);
  StringVector toSave;
  if (regexpMode || (names.size() == 0)) {
    StringVector allNames = cntxt->listAllVariables();
    for (int i=0;i<(int)allNames.size();i++)
      if ((names.size() == 0) || contains(names,allNames[i],regexpMode))
	toSave << allNames[i];
  } else 
    toSave = names;
  if (matMode)
    return MatSaveFunction(fname,toSave,eval);
  else if (asciiMode)
    return SaveASCIIFunction(fname,toSave,tabsMode,doubleMode,eval);
  else
    return SaveNativeFunction(fname,toSave,eval);
}

static int ParseNumber(QString tx) {
  int lookahead = 0;
  int len = 0;
  if ((tx[len] == '+') || (tx[len] == '-'))
    len++;
  lookahead = len;
  while (tx[len].isDigit()) len++;
  lookahead = len;
  if (tx[lookahead] == '.') {
    lookahead++;
    len = 0;
    while (tx[len+lookahead].isDigit()) len++;
    lookahead+=len;
  }
  if ((tx[lookahead] == 'E') || (tx[lookahead] == 'e')) {
    lookahead++;
    if ((tx[lookahead] == '+') || (tx[lookahead] == '-')) {
      lookahead++;
    }
    len = 0;
    while (tx[len+lookahead].isDigit()) len++;
    lookahead+=len;
  }
  return lookahead;
}

static void ParseComplexValue(QString tx, double &real, double &imag) {
  int lnum = ParseNumber(tx);
  int rnum = ParseNumber(tx.mid(lnum));
  QString num1 = tx.left(lnum);
  QString num2 = tx.mid(lnum,rnum);
  if (num1.isEmpty() && num2.isEmpty()) {
    real = 0; imag = 1;
    return;
  }
  if (num1 == "+") num1 = "+1";
  if (num2 == "+") num2 = "+1";
  if (num1 == "-") num1 = "-1";
  if (num2 == "-") num2 = "-1";
  if (num2.isEmpty()) {
    real = 0;
    imag = num1.toDouble();
  } else {
    real = num1.toDouble();
    imag = num2.toDouble();
  }
}

static int DecodeSpreadsheetColumn(QString tx) {
  tx.toUpper();
  QByteArray txb(tx.toLatin1());
  for (int i=0;i<txb.count();i++) 
    txb[i] = txb[i] - 'A';
  int ret = 0;
  for (int i=0;i<txb.count();i++) 
    ret += (int) (txb.at(i)*pow(26.0,txb.count()-1-i));
  return ret;
}

static void DecodeSpreadsheetRange(QString tx, int &startrow, int &startcol,
			    int &stoprow, int &stopcol) {
  QString colstart;
  QString rowstart;
  QString colstop;
  QString rowstop;
  while (tx.at(0).isLetter()) {
    colstart += tx.left(1);
    tx = tx.mid(1);
  }
  while (tx.at(0).isDigit()) {
    rowstart += tx.left(1);
    tx = tx.mid(1);
  }
  tx = tx.mid(1);
  tx = tx.mid(1);
  while (tx.at(0).isLetter()) {
    colstop += tx.left(1);
    tx = tx.mid(1);
  }
  while (tx.at(0).isDigit()) {
    rowstop += tx.left(1);
    tx = tx.mid(1);
  }
  startrow = rowstart.toInt()-1;
  stoprow = rowstop.toInt()-1;
  startcol = DecodeSpreadsheetColumn(colstart);
  stopcol = DecodeSpreadsheetColumn(colstop);
}

//@@Signature
//function dlmread DlmReadFunction
//inputs filename delimiter startrow startcol
//outputs y
//DOCBLOCK io_dlmread
ArrayVector DlmReadFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0) 
    throw Exception("dlmread expects a filename");
  QFile ifile(arg[0].asString());
  if (!ifile.open(QFile::ReadOnly))
    throw Exception("filename " + arg[0].asString() + " could not be opened");
  bool no_delimiter = true;
  QString delimiter;
  if (arg.size() >= 2) {
    delimiter = arg[1].asString();
    no_delimiter = (delimiter.size() == 0);
  }
  int col_count = 0;
  int row_count = 0;
  QList<QList<double> > data_real;
  QList<QList<double> > data_imag;
  QTextStream str(&ifile);
  while (!str.atEnd()) {
    QString whole_line = str.readLine(0);
    QStringList line_pieces(whole_line.split("\r"));
    for (int i=0;i<line_pieces.size();i++) {
      QString line = line_pieces[i];
      QStringList elements;
      if (no_delimiter) {
	if (line.contains(QRegExp("[,;:]")))
	  elements = line.split(QRegExp("[,;:]"));
	else {
	  line = line.simplified();
	  elements = line.split(' ');
	}
      } else {
	elements = line.split(QString(delimiter)[0]);
      }
      QList<double> row_data_real;
      QList<double> row_data_imag;
      row_count++;
      for (int i=0;i<elements.size();i++) {
	QString element(elements[i]);
	element.replace(" ","");
	if (element.contains('i') || element.contains('I') ||
	    element.contains('j') || element.contains('J')) {
	  double real, imag;
	  ParseComplexValue(element,real,imag);
	  row_data_real << real;
	  row_data_imag << imag;
	} else {
	  row_data_real << element.toDouble();
	  row_data_imag << 0;
	}
      }
      col_count = qMax(col_count,elements.size());
      data_real << row_data_real;
      data_imag << row_data_imag;
    }
  }
  int startrow = 0;
  int startcol = 0;
  int stoprow = row_count-1;
  int stopcol = col_count-1;
  if (arg.size() == 4) {
    startrow = arg[2].asInteger();
    startcol = arg[3].asInteger();
  } else if (arg.size() == 3) {
    if (arg[2].isVector() && (arg[2].length() == 4)) {
      Array range(arg[2].asDenseArray().toClass(Int32));
      const BasicArray<int32> &dp(range.constReal<int32>());
      startrow = dp[1];
      startcol = dp[2];
      stoprow = dp[3];
      stopcol = dp[4];
    } else if (arg[2].isString()) 
      DecodeSpreadsheetRange(arg[2].asString(),startrow,startcol,stoprow,stopcol);
    else
      throw Exception("Unable to decode the range arguments to the dlmread function");
  }
  Array A;
  startrow = qMax(0,qMin(row_count-1,startrow));
  startcol = qMax(0,qMin(col_count-1,startcol));
  stoprow = qMax(0,qMin(row_count-1,stoprow));
  stopcol = qMax(0,qMin(col_count-1,stopcol));
  int numrows = stoprow-startrow+1;
  int numcols = stopcol-startcol+1;
  if ((numrows > 0) && (numcols > 0) && (row_count > 0) && (col_count > 0)) {
    bool anyNonzeroImaginary = false;
    for (int i=startrow;i<=stoprow;i++) 
      for (int j=0;j<=qMin(data_real[i].size()-1,stopcol);j++) 
	if (data_imag[i][j] != 0) anyNonzeroImaginary = true;
    if (!anyNonzeroImaginary) {
      A = Array(Double,NTuple(numrows,numcols));
      BasicArray<double> &dp(A.real<double>());
      for (int i=startrow;i<=stoprow;i++)
	for (int j=startcol;j<=stopcol;j++)
	  if (j <= (data_real[i].size()-1))
	    dp[i-startrow+(j-startcol)*numrows+1] = data_real[i][j];
    } else {
      A = Array(Double,NTuple(numrows,numcols));
      BasicArray<double> &dp(A.real<double>());
      BasicArray<double> &ip(A.imag<double>());
      for (int i=startrow;i<=stoprow;i++)
	for (int j=startcol;j<=stopcol;j++)
	  if (j <= (data_real[i].size()-1)) {
	    dp[i-startrow+(j-startcol)*numrows+1] = data_real[i][j];
	    ip[i-startrow+(j-startcol)*numrows+1] = data_imag[i][j];
	  }
    }
  }
  return ArrayVector(A);
}

static ArrayVector LoadASCIIFunction(int nargout, QString filename, Interpreter* eval) {
  // Hmmm...
  QFile ifile(filename);
  if (!ifile.open(QFile::ReadOnly))
    throw Exception("filename " + filename + " could not be opened");
  QTextStream str(&ifile);
  int i=0;
  int col_count = 0;
  int row_count = 0;
  QList<double> data;
  bool evenData = true;
  while (!str.atEnd() && evenData) {
    QString line = str.readLine(0);
    line = line.simplified();
    QStringList elements(line.split(' '));
    if (row_count == 0) 
      col_count = elements.size();
    else if (elements.size() != col_count)
      evenData = false;
    if (evenData) {
      row_count++;
      for (i=0;i<elements.size();i++) 
	data << elements[i].toDouble();
    }
  }
  if (!evenData)
    eval->warningMessage("data in ASCII file does not have a uniform number of entries per row");
  // Now construct the matrix
  Array A;
  if ((row_count > 0) && (col_count > 0)) {
    A = Array(Double,NTuple(row_count,col_count));
    BasicArray<double> &dp(A.real<double>());
    for (int r=0;r<row_count;r++) 
      for (int c=0;c<col_count;c++) 
	dp[r+c*row_count+1] = data.at(r*col_count+c);
  }
  if (nargout == 1)
    return ArrayVector(A);
  else {
    QFileInfo fi(filename);
    ParentScopeLocker lock(eval->getContext());
    eval->getContext()->insertVariable(fi.baseName(),A);
  }
  return ArrayVector();
}

static ArrayVector LoadNativeFunction(int nargout, QString filename,
				      StringVector names, bool regexpmode, Interpreter* eval) {
  QFile ofile(filename);
  if (!ofile.open(QIODevice::ReadOnly))
    throw Exception("Unable to open " + filename + " to read data");
  Serialize input(&ofile);
  input.handshakeClient();
  QString arrayName = input.getString();
  StringVector fieldnames;
  ArrayVector fieldvalues;
  ParentScopeLocker lock(eval->getContext());
  while (arrayName != "__eof") {
    Array toRead;
    char flag;
    flag = input.getByte();
    input.getArray(toRead);
    if ((names.size() == 0) || 
	(contains(names,arrayName,regexpmode) && (nargout == 0))) {
      switch (flag) {
      case 'n':
	break;
      case 'g':
	eval->getContext()->addGlobalVariable(arrayName);
	break;
      case 'p':
	eval->getContext()->addPersistentVariable(arrayName);
	break;
      default:
	throw Exception(QString("unrecognized variable flag ") + flag + 
			QString(" on variable ") + arrayName);
      }
      eval->getContext()->insertVariable(arrayName,toRead);
    } else {
      fieldnames << arrayName;
      fieldvalues << toRead;
    }
    arrayName = input.getString();
  }
  if (nargout == 0)
    return ArrayVector();
  else
    return ArrayVector(StructConstructor(fieldnames,fieldvalues));
}

//@@Signature
//sfunction load LoadFunction
//inputs varargin
//outputs y
//DOCBLOCK io_load
ArrayVector LoadFunction(int nargout, const ArrayVector& arg, 
			 Interpreter* eval) {
  // Process the arguments to extract the "-mat", "-ascii" and "-regexp" 
  // flags.
  ArrayVector argCopy;
  if (arg.size() == 0) return ArrayVector();
  bool asciiMode = false;
  bool matMode = false;
  bool regexpMode = false;
  for (int i=0;i<arg.size();i++) {
    if (arg[i].isString()) {
      if (arg[i].asString().toUpper() == "-MAT")
	matMode = true;
      else if (arg[i].asString().toUpper() == "-ASCII")
	asciiMode = true;
      else if (arg[i].asString().toUpper() == "-REGEXP")
	regexpMode = true;
      else
	argCopy << arg[i];
    } else
      argCopy << arg[i];
  }
  if (argCopy.size() < 1)  throw Exception("load requries a filename argument");
  QString fname(argCopy[0].asString());
  // If one of the filemode flags has not been specified, try to 
  // guess if it is an ASCII file or a MAT file
  if (!matMode && !asciiMode) {
    if (fname.endsWith(".mat",Qt::CaseInsensitive))
      matMode = true;
    if (fname.endsWith(".txt",Qt::CaseInsensitive)) {
      asciiMode = true;
    } else {
      // Could be an ASCII file - try to open it native
      try {
	QFile of(fname);
	if (!of.open(QIODevice::ReadOnly))
	  throw Exception("nope");
	Serialize input(&of);
	input.handshakeClient();
      } catch(Exception& e) {
	asciiMode = true;
      }
    }
  }
  StringVector names;
  for (int i=1;i<argCopy.size();i++) {
    if (!arg[i].isString())
      throw Exception("unexpected non-string argument to load command");
    names << argCopy[i].asString();
  }
  // Read the data file using the appropriate handler
  try {
    if (matMode)
      return MatLoadFunction(nargout,fname,names,regexpMode,eval);
    else if (asciiMode)
      return LoadASCIIFunction(nargout,fname,eval);
    else
      return LoadNativeFunction(nargout,fname,names,regexpMode,eval);
  } catch (Exception& e) {
    throw Exception("unable to read data from file " + fname + " - it may be corrupt, or FreeMat may not understand the format.  See help load for more information.  The specific error was: " + e.msg());
  }
  return ArrayVector();
}

//@@Signature
//sfunction getline GetLineFunction
//inputs prompt
//outputs string
//DOCBLOCK io_getline
ArrayVector GetLineFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  QString prompt;
  if (arg.size() < 1)
    prompt = "";
  else {
    if (!arg[0].isString())
      throw Exception("getline requires a string prompt");
    prompt = arg[0].asString();
  }
  return ArrayVector(Array(eval->getLine(prompt)));
}
