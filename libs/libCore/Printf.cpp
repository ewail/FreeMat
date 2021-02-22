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
#include "Printf.hpp"
#include "Print.hpp"
#include "HandleList.hpp"
#include "Algorithms.hpp"
#include "Utils.hpp"
#include <QTextCodec>
#include <QFile>
#include <QTextStream>

extern HandleList<FilePtr*> fileHandles;

class PrintfStream{
public:
  virtual PrintfStream& operator <<( const char* data ) = 0;
};

class PrintfFileStream : public PrintfStream{
private:
  QFile* fp;
  QTextStream ts;
public:
  PrintfFileStream( QFile* fp_ ) : fp( fp_ ) { ts.setDevice( fp ); };
  virtual PrintfFileStream& operator <<( const char* data ){
    ts << data;
    return *this;
  };
  ~PrintfFileStream() { fp->flush(); };
};

class PrintfStringStream : public PrintfStream{
private:
  QString* str;
public:
  PrintfStringStream( QString* str_ ) : str(str_){};
  virtual PrintfStringStream& operator <<( const char* data ){
    (*str) += QString( data );
    return *this;
  };
};


bool isEscape(char *dp) {
  return ((dp[0] == '\\') &&
	  ((dp[1] == 'n') ||
	   (dp[1] == 't') ||
	   (dp[1] == 'r') ||
	   (dp[1] == '\\')));
}

static int flagChar(char c) {
  return ((c == '#') ||  (c == '0') || (c == '-') ||  
	  (c == ' ') ||  (c == '+'));
}

static int convspec(char c) {
  return ((c == 'd') || (c == 'i') || (c == 'o') || 
	  (c == 'u') || (c == 'x') || (c == 'X') ||
	  (c == 'e') || (c == 'E') || (c == 'f') || 
	  (c == 'F') || (c == 'g') || (c == 'G') ||
	  (c == 'a') || (c == 'A') || (c == 'c') || 
	  (c == 's'));
}

static char* validateFormatSpec(char* cp) {
  if (*cp == '%') return cp+1;
  while ((*cp) && flagChar(*cp)) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  while ((*cp) && (*cp == '.')) cp++;
  while ((*cp) && isdigit(*cp)) cp++;
  if ((*cp) && convspec(*cp)) 
    return cp+1;
  else
    return 0;
}

QString ConvertEscapeSequences(const QString &src) {
  QString dest;
  int i=0;
  while (i < src.size()) {
    if ((src[i] != '\\') || (i == (src.size()-1))) {
      dest += src[i];
    } else {
      if (src[i+1] == 'n') {
	dest += '\n';
	i++;
      } else if (src[i+1] == 't') {
	dest += '\t';
	i++;
      } else if (src[i+1] == 'r') {
	dest += '\r';
	i++;
      } else {
	dest += src[i+1];
	i++;
      }
    }
    i++;
  }
  return dest;
}

class PrintfDataServer{
private:
  const ArrayVector arg;
  int vec_ind;
  int elem_ind;
  bool hasMoreData;
  bool dataUsed;
  void IncDataPtr(void){
    if( ++elem_ind >= arg[ vec_ind ].length() ){
      if( ++vec_ind < arg.size() ){
	elem_ind = 0;
      }
      else{
	hasMoreData = false;
      }
    }
    dataUsed = true;
  };

public:
  PrintfDataServer( const ArrayVector& arg_ ):arg(arg_),vec_ind(1),elem_ind(0){
    //vec_ind starts with 1, because zeroth argument is format string
    hasMoreData = (arg.size() > 1); //( (arg.size() > 1) && (arg[1].getLength() > 0));
    dataUsed = false;
  };

  void GetNextAsDouble(double& data){
    if( !hasMoreData )
      throw Exception("Error: no more data");
    if( arg[vec_ind].isEmpty()) {
      data = 0;
    } else {
      if( arg[ vec_ind ].isScalar() )
	data = arg[ vec_ind ].asDouble();
      else
	data = arg[ vec_ind ].get( elem_ind+1 ).asDouble();
    }
    IncDataPtr();
  };

  void GetNextAsString(std::string& str){
    if( !hasMoreData )
      throw Exception("Error: no more data");
    Array d(arg[ vec_ind ]);
    if (!d.isEmpty()) {
      if( d.isString() ){
	QString s = d.asString();
	QChar* data = s.data();
	while( elem_ind < s.length() ){
	  str.push_back((data + elem_ind++)->toAscii());
	}
      }else{
	str.push_back(d.get(elem_ind+1).asInteger());
      }
    }
    IncDataPtr();
  };
  bool HasMoreData(void){ return hasMoreData; };
  bool WasDataUsed(void) {return dataUsed;};
};

//Common routine used by sprintf,printf,fprintf.  They all
//take the same inputs, and output either to a string, the
//console or a file.  For output to a console or a file, 
//we want escape-translation on.  For output to a string, we
//want escape-translation off.  So this common routine prints
//the contents to a string, which is then processed by each 
//subroutine.
void PrintfHelperFunction(int nargout, const ArrayVector& arg, PrintfStream& output, QByteArray& errmsg, Array& ret, bool convEscape = false ) 
{
  Array format(arg[0]);
  QString frmt = format.asString();

  std::vector<char> buff( frmt.length()+1 ); //vectors are contiguous in memory. we'll use it instead of char[]
  strncpy(&buff[0], frmt.toStdString().c_str(), frmt.length()+1 );

  // Search for the start of a format subspec
  char *dp = &buff[0];
  char *np;
  char sv;
  int nprn = 0;
  int noutput = 0;
  // Buffer to hold each sprintf result
#define BUFSIZE 65534
  char nbuff[BUFSIZE+1];
  memset( nbuff, 0, BUFSIZE+1 );

  // Buffer to hold the output
  //output.clear();
  errmsg.clear();

  PrintfDataServer ps( arg );

  //do while there is still data to output or format string to save
  while( (*dp) || ps.HasMoreData() ) {
    if ( !(*dp) && ps.HasMoreData() ) //still have arguments, need to rewind format.
      {
	if (ps.WasDataUsed())
	  dp = &buff[0];
	else
	  {
	    WarningMessage("Unused data (format string had no usable format specs)");
	    break;
	  }
      }

    np = dp;
    int nbuf_ind = 0;
    //copy string upto formatting character and do escape conversion in the process
    while ((*dp) && (*dp != '%') && nbuf_ind < BUFSIZE ){ 
      if (convEscape && isEscape(dp)) {
	switch (*(dp+1)) {
	case '\\':
	  *(nbuff+nbuf_ind) = '\\';
	  break;
	case 'n':
	  *(nbuff+nbuf_ind) = '\n';
	  break;
	case 't':
	  *(nbuff+nbuf_ind) = '\t';	  break;
	case 'r':
	  *(nbuff+nbuf_ind) = '\r';
	  break;
	}
	dp += 2; ++nbuf_ind;
      } else
	*(nbuff+nbuf_ind++) = *(dp++);
    }

    // Print out the formatless segment
    *(nbuff+nbuf_ind) = 0;
    nprn = nbuf_ind; noutput += nbuf_ind;
    output << nbuff;    


    // Process the format spec
    if (*dp == '%' && *(dp+1)) {
      np = validateFormatSpec(dp+1);
      if (!np)
	throw Exception("erroneous format specification " + QString(dp));
      else {
	if (*(np-1) == '%') {
	  nprn = snprintf(nbuff,BUFSIZE,"%%"); nbuff[std::min(nprn+1,BUFSIZE-1)]='\0'; noutput += nbuf_ind;
	  output << nbuff;
	  sv=0;
	} else 
	  if( *(np-1) == 's') {
	    std::string str;
	    ps.GetNextAsString( str );
	    const char* pStr = str.c_str();
	    sv = *np;
	    *np = 0;
	    nprn = snprintf(nbuff,BUFSIZE,dp,pStr); nbuff[std::min(nprn+1,BUFSIZE-1)]='\0'; noutput += nbuf_ind;
	    output << nbuff;
	  } else{
	    sv = *np;
	    *np = 0;

	    double data;
	    ps.GetNextAsDouble( data );

	    switch (*(np-1)) 
	      {
	      case 'd':
	      case 'i':
	      case 'o':
	      case 'u':
	      case 'x':
	      case 'X':
	      case 'c':
		nprn = snprintf(nbuff,BUFSIZE,dp,(int32)data); nbuff[nprn+1]='\0'; noutput += nbuf_ind;
		output << nbuff;
		break;
	      case 'e':
	      case 'E':
	      case 'f':
	      case 'F':
	      case 'g':
	      case 'G':
		nprn = snprintf(nbuff,BUFSIZE,dp,data); nbuff[nprn+1]='\0'; noutput += nbuf_ind;
		output << nbuff;
		break;
	      }
	  }
	if( sv )
	  *np = sv;
	dp = np;
      }
    }
  }
  ret = Array( static_cast<double>(noutput) );
}

//@@Signature
//function sprintf SprintfFunction
//inputs varargin
//outputs string
//DOCBLOCK io_sprintf
ArrayVector SprintfFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("sprintf requires at least one (string) argument");
  if (!arg[0].isString())
    throw Exception("sprintf format argument must be a string");

  QString outf;
  PrintfStringStream textstream( &outf );
  QByteArray errmsg;
  Array output;

  PrintfHelperFunction( nargout, arg, textstream, errmsg, output, true );
  ArrayVector ret;

  ret << Array( outf );
  ret << output;
  return ret;
}

//@@Signature
//sfunction printf PrintfFunction
//inputs varargin
//outpus none
//DOCBLOCK io_printf
ArrayVector PrintfFunction(int nargout, const ArrayVector& arg, 
			   Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("printf requires at least one (string) argument");
  Array format(arg[0]);
  if (!format.isString())
    throw Exception("printf format argument must be a string");
  
  QString outf;
  PrintfStringStream textstream( &outf );
  
  QByteArray errmsg;
  Array output;
  
  PrintfHelperFunction( nargout, arg, textstream, errmsg, output, true );
  ArrayVector ret;
  
  eval->outputMessage( outf );
  return ArrayVector();
}

//@@Signature
//sfunction fprintf FprintfFunction
//inputs varargin
//outputs varargout
//DOCBLOCK io_fprintf
ArrayVector FprintfFunction(int nargout, const ArrayVector& arg, 
			    Interpreter* eval) {
  if (arg.size() == 0)
    throw Exception("fprintf requires at least one (string) argument");
  ArrayVector argCopy(arg);
  int handle = 1;
  if (arg.size() > 1) {
    Array tmp(arg[0]);
    if (tmp.isScalar()) {
      handle = tmp.asInteger();
      argCopy.pop_front();
    }
    else {
      handle=1;
    }
  }
  if (handle == 1)
    return PrintfFunction(nargout,argCopy,eval);
  Array format(argCopy[0]);
  if (!format.isString())
    throw Exception("fprintf format argument must be a string");
  ArrayVector argcopy(arg);
  argcopy.pop_front();

  FilePtr *fptr=(fileHandles.lookupHandle(handle+1));

  PrintfFileStream textstream( fptr->fp );
  QByteArray errmsg;
  Array output;

  PrintfHelperFunction( nargout, argcopy, textstream, errmsg, output, true );
  return ArrayVector() << output;
}

//@@Signature
//sfunction disp DispFunction
//inputs varargin
//outputs none
//DOCBLOCK io_disp
ArrayVector DispFunction(int nargout, const ArrayVector& arg, Interpreter* eval) {
  for (int i=0;i<arg.size();i++) 
    PrintArrayClassic(arg[i],eval->getPrintLimit(),eval);
  return ArrayVector();
} 

