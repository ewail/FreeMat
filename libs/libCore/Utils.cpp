/*
* Copyright (c) 2002-2006 Samit Basu
* 
* QFileReadFloat, QFileReadInteger, and QFileRedUnsigned functions are:
* Copyright (C) 1992-2008 Trolltech ASA. All rights reserved.
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

#include "Utils.hpp"
#include "Exception.hpp"
#include "IEEEFP.hpp"
#include <math.h>
#include <QTextCodec>
#include "Algorithms.hpp"

bool contains(StringVector& list, QString s, bool regexpmode) {
    QRegExp t;
    for (int i=0;i<list.size();i++) {
	if (regexpmode) {
	    t = QRegExp(list[i]);
	    if (t.exactMatch(s) &&
		(t.matchedLength() == s.size())) return true;
	} else {
	    if (list[i] == s) return true;
	}
    }
    return false;
}

NTuple ArrayVectorAsDimensions(const ArrayVector &arg) {
    NTuple dims;
    if (arg.size() == 0)
	return NTuple(1,1);
    for (int i=0;i<arg.size();i++)
	if (arg[i].isString())
	    throw Exception("Cannot use strings as arguments where dimensions are expected");
    // Case 1 - all of the entries are scalar
    bool allScalars;
    allScalars = true;
    for (int i=0;i<arg.size();i++)
	allScalars &= arg[i].isScalar();
    if (allScalars) {
	if (arg.size() == 1) {
	    // If all scalars and only one argument - we want a square matrix
	    if (!IsFinite(arg[0].asDouble()))
		throw Exception("Expecting finite arguments for dimensions");
	    dims.set(0,arg[0].asInteger());
	    dims.set(1,arg[0].asInteger());
	} else {
	    // If all scalars and and multiple arguments, we count dimensions
	    for (int i=0;i<arg.size();i++)  {
		if (!IsFinite(arg[i].asDouble()))
		    throw Exception("Expecting finite arguments for dimensions");
		dims.set(i,arg[i].asInteger());
	    }
	}
    } else {
	if (arg.size() > 1)
	    throw Exception("Dimension arguments must be either all scalars or a single vector");
	if (AnyNotFinite(arg[0]))
	    throw Exception("Dimension arguments must be finite");
	Array t = arg[0].toClass(UInt32);
	const BasicArray<uint32> &td(t.constReal<uint32>());
	for (index_t i=1;i<=t.length();i++)
	    dims.set(qMax(0,int(i-1)),td.get(i));
    }
    for (int i=0;i<NDims;i++) 
	if (!IsFinite(dims[i]))
	    throw Exception("Dimension arguments must be finite");
    return dims;
}


double ArrayRange(const Array& dp) {
    const Array &tmp(dp.asDenseArray().toClass(Double));
    const BasicArray<double> &rp(tmp.constReal<double>());
    if (rp.length() == 0) return 0;
    double result = 0;
    bool init = false;
    for (index_t i=1;i<=rp.length();i++) {
	if (!IsNaN(rp[i])) {
	    if (!init) {
		init = true;
		result = fabs(rp[i]);
	    } else {
		if (fabs(rp[i]) > result) 
		    result = fabs(rp[i]);
	    }
	}
    }
    if (!init) return NaN();
    return result;
}

double ArrayMin(const Array& dp) {
    const Array &tmp(dp.asDenseArray().toClass(Double));
    const BasicArray<double> &rp(tmp.constReal<double>());
    if (rp.length() == 0) return 0;
    double result = 0;
    bool init = false;
    for (index_t i=1;i<=rp.length();i++) {
	if (!IsNaN(rp[i])) {
	    if (!init) {
		init = true;
		result = rp[i];
	    } else {
		if (rp[i] < result) 
		    result = rp[i];
	    }
	}
    }
    if (!init) return NaN();
    return result;
}

double ArrayMax(const Array& dp) {
    const Array &tmp(dp.asDenseArray().toClass(Double));
    const BasicArray<double> &rp(tmp.constReal<double>());
    if (rp.length() == 0) return 0;
    double result = 0;
    bool init = false;
    for (index_t i=1;i<=rp.length();i++) {
	if (!IsNaN(rp[i])) {
	    if (!init) {
		init = true;
		result = rp[i];
	    } else {
		if (rp[i] > result) 
		    result = rp[i];
	    }
	}
    }
    if (!init) return NaN();
    return result;
}

Array DoubleVectorFromQList(QList<uint32> &ref) {
    BasicArray<double> retvec(NTuple(1,ref.size()));
    for (int i=0;i<ref.size();i++)
	retvec[i+1] = ref[i];
    return Array(retvec);
}  

void SwapBuffer(char* cp, int count, int elsize) {
    char tmp;
    for (int i=0;i<count;i++)
	for (int j=0;j<elsize/2;j++) {
	    tmp = cp[i*elsize+j];
	    cp[i*elsize+j] = cp[i*elsize+elsize-1-j];
	    cp[i*elsize+elsize-1-j] = tmp;
	}    
}

QString ReadQStringFromFile(QFile *fp) {
    QByteArray ret(fp->readLine());
    return (QTextCodec::codecForLocale()->toUnicode(ret));
}

void WriteQStringToFile(QFile *fp, QString txt) {
    QByteArray dat(QTextCodec::codecForLocale()->fromUnicode(txt));
    fp->write(dat);
}

static void SkipWhiteSpace(QFile *fp) {
    char w;
    while ((fp->getChar(&w)) && isspace(w));
    fp->ungetChar(w);
}

int digitvalue(char x) {
    return (x-'0');
}

double QFileReadInteger(QFile *fp, int base, int nMaxDigits) {
    nMaxDigits = (nMaxDigits>0)? nMaxDigits : 1024;
    SkipWhiteSpace(fp);
    // detect int encoding
    if (base == 0) {
	char ch;
	if (!fp->getChar(&ch))
	    return 0;
	if (ch == '0') {
	    char ch2;
	    if (!fp->getChar(&ch2)) {
		// Result is the number 0
		return 0;
	    }
	    ch2 = tolower(ch2);
	    if (ch2 == 'x') {
		base = 16;
	    } else if (ch2 == 'b') {
		base = 2;
	    } else if (isdigit(ch2) && 
		digitvalue(ch2) >= 0 &&
		digitvalue(ch2) <= 7) {
		    base = 8;
	    } else {
		base = 10;
	    }
	    fp->ungetChar(ch2);
	} else if (ch == '-' || ch == '+' || isdigit(ch)) {
	    base = 10;
	} else {
	    //fp->ungetChar(ch);
	    throw Exception("Scanf can't interpret numerical value");
	    return 0;
	}
	fp->ungetChar(ch);
	// State of the stream is now the same as on entry
	// (cursor is at prefix),
	// and local variable 'base' has been set appropriately.
    }
    double val=0;
    switch (base) {
		  case 2: {
		      char pf1, pf2, dig;
		      // Parse prefix '0b'
		      if (!fp->getChar(&pf1) || pf1 != '0')
			  return 0;
		      if (!fp->getChar(&pf2) || tolower(pf2) != 'b')
			  return 0;
		      // Parse digits
		      int ndigits = 0;
		      while (ndigits < nMaxDigits && fp->getChar(&dig)) {
			  int n = tolower(dig);
			  if (n == '0' || n == '1') {
			      val *= 2;
			      val += n - '0';
			  } else {
			      fp->ungetChar(dig);
			      break;
			  }
			  ndigits++;
		      }
		      if (ndigits == 0) {
			  // Unwind the prefix and abort
			  fp->ungetChar(pf2);
			  fp->ungetChar(pf1);
			  throw Exception("Scanf can't interpret numerical value");
			  return 0;
		      }
		      break;
			  }
		  case 8: {
		      char pf, dig;
		      // Parse prefix '0'
		      if (!fp->getChar(&pf) || pf != '0')
			  return 0;
		      // Parse digits
		      int ndigits = 0;
		      while (ndigits < nMaxDigits && fp->getChar(&dig)) {
			  int n = tolower(dig);
			  if (n >= '0' && n <= '7') {
			      val *= 8;
			      val += n - '0';
			  } else {
			      fp->ungetChar(dig);
			      break;
			  }
			  ndigits++;
		      }
		      if (ndigits == 0) {
			  // Unwind the prefix and abort
			  fp->ungetChar(pf);
			  throw Exception("Scanf can't interpret numerical value");
			  return 0;
		      }
		      break;
			  }
		  case 10: {
		      // Parse sign (or first digit)
		      char sign;
		      int ndigits = 0;
		      if (!fp->getChar(&sign))
			  return 0;
		      if (sign != '-' && sign != '+') {
			  if (!isdigit(sign)) {
			      fp->ungetChar(sign);
			      throw Exception("Scanf can't interpret numerical value");
			      return 0;
			  }
			  val += digitvalue(sign);
			  ndigits++;
		      }
		      // Parse digits
		      char ch;
		      while (ndigits < nMaxDigits && fp->getChar(&ch) ) {
			  if (isdigit(ch)) {
			      val *= 10;
			      val += digitvalue(ch);
			  } else {
			      fp->ungetChar(ch);
			      break;
			  }
			  ndigits++;
		      }
		      if (ndigits == 0){
			  throw Exception("Scanf can't interpret numerical value");
			  return 0;
		      }
		      if (sign == '-') 
			  val = -val;
		      break;
			   }
		  case 16: {
		      char pf1, pf2, dig;
		      // Parse prefix ' 0x'
		      if (!fp->getChar(&pf1) || pf1 != '0')
			  return 0;
		      if (!fp->getChar(&pf2) || tolower(pf2) != 'x')
			  return 0;
		      // Parse digits
		      int ndigits = 0;
		      while (ndigits < nMaxDigits && fp->getChar(&dig) ) {
			  int n = tolower(dig);
			  if (n >= '0' && n <= '9') {
			      val *= 16;
			      val += n - '0';
			  } else if (n >= 'a' && n <= 'f') {
			      val *= 16;
			      val += 10 + (n - 'a');
			  } else {
			      fp->ungetChar(dig);
			      break;
			  }
			  ndigits++;
		      }
		      if (ndigits == 0) {
			  throw Exception("Scanf can't interpret numerical value");
			  return 0;
		      }
		      break;
			   }
		  default:
		      {
			  // Unsupported integerBase
			  throw Exception("Scanf can't interpret numerical value");
			  return 0;
		      }
    }
    return val;
}

double QFileReadFloat(QFile *fp, int nMaxDigits) {
    nMaxDigits = (nMaxDigits>0)? nMaxDigits : 1024;

    // This code comes from qtextstream.cpp
    // We use a table-driven FSM to parse floating point numbers
    // strtod() cannot be used directly since we may be reading from a
    // QIODevice.
    enum ParserState {
	Init = 0,
	Sign = 1,
	Mantissa = 2,
	Dot = 3,
	Abscissa = 4,
	ExpMark = 5,
	ExpSign = 6,
	Exponent = 7,
	Nan1 = 8,
	Nan2 = 9,
	Inf1 = 10,
	Inf2 = 11,
	NanInf = 12,
	Done = 13
    };
    enum InputToken {
	None = 0,
	InputSign = 1,
	InputDigit = 2,
	InputDot = 3,
	InputExp = 4,
	InputI = 5,
	InputN = 6,
	InputF = 7,
	InputA = 8,
	InputT = 9
    };

    static uchar table[13][10] = {
	// None InputSign InputDigit InputDot InputExp InputI    InputN    InputF    InputA    InputT
	{ 0,    Sign,     Mantissa,  Dot,     0,       Inf1,     Nan1,     0,        0,        0      }, // 0  Init
	{ 0,    0,        Mantissa,  Dot,     0,       Inf1,     Nan1,     0,        0,        0      }, // 1  Sign
	{ Done, Done,     Mantissa,  Dot,     ExpMark, 0,        0,        0,        0,        0      }, // 2  Mantissa
	{ 0,    0,        Abscissa,  0,       0,       0,        0,        0,        0,        0      }, // 3  Dot
	{ Done, Done,     Abscissa,  Done,    ExpMark, 0,        0,        0,        0,        0      }, // 4  Abscissa
	{ 0,    ExpSign,  Exponent,  0,       0,       0,        0,        0,        0,        0      }, // 5  ExpMark
	{ 0,    0,        Exponent,  0,       0,       0,        0,        0,        0,        0      }, // 6  ExpSign
	{ Done, Done,     Exponent,  Done,    Done,    0,        0,        0,        0,        0      }, // 7  Exponent
	{ 0,    0,        0,         0,       0,       0,        0,        0,        Nan2,     0      }, // 8  Nan1
	{ 0,    0,        0,         0,       0,       0,        NanInf,   0,        0,        0      }, // 9  Nan2
	{ 0,    0,        0,         0,       0,       0,        Inf2,     0,        0,        0      }, // 10 Inf1
	{ 0,    0,        0,         0,       0,       0,        0,        NanInf,   0,        0      }, // 11 Inf2
	{ Done, 0,        0,         0,       0,       0,        0,        0,        0,        0      }, // 11 NanInf
    };

    ParserState state = Init;
    InputToken input = None;

    SkipWhiteSpace(fp);

    const int BufferSize = 128;
    char buf[BufferSize];
    int i = 0;

    char c;
    while (fp->getChar(&c) && i < nMaxDigits ) {
	switch (c) {
	    case '+':
	    case '-':
		input = InputSign;
		break;
	    case '0': case '1': case '2': case '3': case '4':
	    case '5': case '6': case '7': case '8': case '9':
		input = InputDigit;
		break;
	    case '.':
		input = InputDot;
		break;
	    case 'e':
	    case 'E':
		input = InputExp;
		break;
	    case 'i': case 'I':
		input = InputI;
		break;
	    case 'n': case 'N':
		input = InputN;
		break;
	    case 'f': case 'F':
		input = InputF;
		break;
	    case 'a': case 'A':
		input = InputA;
		break;
	    case 't': case 'T':
		input = InputT;
		break;
	    default:
		input = None;
		break;
	}

	state = ParserState(table[state][input]);

	if  (state == Init || state == Done || i > (BufferSize - 5)) {
	    fp->ungetChar(c);
	    if (i > (BufferSize - 5)) { // ignore rest of digits
		while (fp->getChar(&c)) {
		    if (!isdigit(c)) {
			fp->ungetChar(c);
			break;
		    }
		}
	    }
	    break;
	}
	buf[i++] = c;
    }

    if (i == 0){
	throw Exception("Not a floating point number");
    }

    double f;
    // ### Number parsing should really be handled by QLocale.
    char c0 = buf[0] | 32; // tolower
    char c1 = buf[1] | 32; // tolower
    bool sign = true;
    if (c0 == '+' || c0 == '-') {
	sign = (c0 == '+');
	c0 = c1;
    }
    if (c0 == 'i') {
	f = sign ? Inf() : -Inf();
    } else if (c0 == 'n') {
	f = NaN();
    } else {
	buf[i] = '\0';
	f = strtod(buf, 0);
    }
    return f;
}

QString QFileReadString(QFile *fp,int nMaxChars) {
    nMaxChars = (nMaxChars>0)? nMaxChars : 65535;

    SkipWhiteSpace(fp);
    QString val("");
    int nChars = 0;
    char tmp;

    while( fp->getChar( &tmp ) && nChars < nMaxChars ){
	if( isspace( tmp ) ){
	    fp->ungetChar( tmp );
	    break;
	}
	val.append( tmp );
	++nChars;
    }
    return val;
}

char QFileReadChar(QFile *fp) {
    char tmp;
    fp->getChar( &tmp );
    return tmp;
}
