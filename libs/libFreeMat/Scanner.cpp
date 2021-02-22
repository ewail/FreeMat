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
#include "Scanner.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Exception.hpp"
#include <algorithm>
#include <QMutexLocker>
#include <QMutex>

static QHash<QString, TokenValueType> fm_reserved;

static bool fm_reserved_initialized = false;

QMutex lock;
static int TokenID = 1;

void InitializeReservedTable() {
  if (fm_reserved_initialized) return;
  fm_reserved["break"] = TOK_BREAK;
  fm_reserved["case"] = TOK_CASE;
  fm_reserved["catch"] = TOK_CATCH;
  fm_reserved["continue"] = TOK_CONTINUE;
  fm_reserved["dbstep"] = TOK_DBSTEP;
  fm_reserved["dbtrace"] = TOK_DBTRACE;
  fm_reserved["dbdown"] = TOK_DBDOWN;
  fm_reserved["dbup"] = TOK_DBUP;
  fm_reserved["else"] = TOK_ELSE;
  fm_reserved["elseif"] = TOK_ELSEIF;
  fm_reserved["end"] = TOK_END;
  fm_reserved["for"] = TOK_FOR;
  fm_reserved["function"] = TOK_FUNCTION;
  fm_reserved["global"] = TOK_GLOBAL;
  fm_reserved["if"] = TOK_IF;
  fm_reserved["keyboard"] = TOK_KEYBOARD;
  fm_reserved["otherwise"] = TOK_OTHERWISE;
  fm_reserved["persistent"] = TOK_PERSISTENT;
  fm_reserved["quit"] = TOK_QUIT;
  fm_reserved["retall"] = TOK_RETALL;
  fm_reserved["return"] = TOK_RETURN;
  fm_reserved["switch"] = TOK_SWITCH;
  fm_reserved["try"] = TOK_TRY;
  fm_reserved["while"] = TOK_WHILE;
  fm_reserved_initialized = true;
}

static bool isalnumus(TokenValueType a) {
  return (isalnum(a) || (a=='_'));
}

static bool isablank(TokenValueType a) {
  return (a==' ' || a=='\t' || a=='\r');
}

unsigned Scanner::contextNum() {
  return (m_ptr << 16 | m_linenumber);
}

void Scanner::setToken(TokenValueType tok, QString text) {
  m_tok = Token(tok,m_ptr << 16 | m_linenumber,text);
  QMutexLocker locker(&lock);
  m_tok.setUID(TokenID++);
}

bool Scanner::done() {
  return (m_ptr >= (int)(m_text.size()));
}

bool Scanner::peek(int chars, TokenValueType tok) {
  return (ahead(chars) == tok);
}

Scanner::Scanner(QString buf, QString fname) {
  InitializeReservedTable();
  m_text = buf;
  m_filename = fname;
  m_ptr = 0;
  m_linenumber = 1;
  m_tokValid = false;
  m_inContinuationState = false;
  m_bracketDepth = 0;
  m_strlen = buf.size();
  m_ignorews.push(true);
  m_debugFlag = false;
  m_blobFlag = false;
}

void Scanner::fetchContinuation() {
  m_ptr += 3;
  while ((current() != '\n') && (m_ptr < m_strlen))
    m_ptr++;
  if (current() == '\n') {
    m_linenumber++;
    m_ptr++;
  }
  m_inContinuationState = true;
}

void Scanner::fetch() {
  if (m_ptr >= m_strlen)
    setToken(TOK_EOF);
  else if (current() == '%') {
    fetchComment();
    return;
  } else if ((current() == '.') && 
	     (ahead(1) == '.') &&
	     (ahead(2) == '.')) {
    fetchContinuation();
    return;
  } else if (m_blobFlag && !isablank(current()) && 
	     (current() != '\n') && (current() != ';') &&
	     (current() != ',') && (current() != '\'') &&
	     (current() != '%')) 
    fetchBlob();
  else if (isalpha(current()))
    fetchIdentifier();
  else if (isdigit(current()) || ((current() == '.') && isdigit(ahead(1))))
    fetchNumber();
  else if (isablank(current())) {
    fetchWhitespace();
    if (m_ignorews.top()) return;
  } else if ((current() == '\'') && !((previous() == '\'') ||
				      (previous() == ')') ||
				      (previous() == ']') ||
				      (previous() == '}') ||
				      (isalnumus(previous())))) {
    fetchString();
  } else
    fetchOther();
  m_tokValid = true;
}

bool Scanner::tryFetchBinary(const char* op, TokenValueType tok) {
  if ((current() == op[0]) && (ahead(1) == op[1])) {
    setToken(tok);
    m_ptr += 2;
    return true;
  }
  return false;
}

void Scanner::fetchComment() {
  while ((current() != '\n') && (m_ptr < m_strlen))
    m_ptr++;
}

void Scanner::fetchOther() {
  if (current() == '.') {
    if (tryFetchBinary(".*",TOK_DOTTIMES)) return;
    if (tryFetchBinary("./",TOK_DOTRDIV)) return;
    if (tryFetchBinary(".\\",TOK_DOTLDIV)) return;
    if (tryFetchBinary(".^",TOK_DOTPOWER)) return;
    if (tryFetchBinary(".'",TOK_DOTTRANSPOSE)) return;
  }
  if (tryFetchBinary("<=",TOK_LE)) return;
  if (tryFetchBinary(">=",TOK_GE)) return;
  if (tryFetchBinary("==",TOK_EQ)) return;
  if (tryFetchBinary("~=",TOK_NE)) return;
  if (tryFetchBinary("&&",TOK_SAND)) return;
  if (tryFetchBinary("||",TOK_SOR)) return;
  setToken(m_text[m_ptr].unicode());
  if (m_text[m_ptr] == '[')
    m_bracketDepth++;
  if (m_text[m_ptr] == ']')
    m_bracketDepth = qMin(0,m_bracketDepth-1);
  if (m_text[m_ptr] == '{')
    m_bracketDepth++;
  if (m_text[m_ptr] == '}')
    m_bracketDepth = qMin(0,m_bracketDepth-1);
  m_ptr++;
}

void Scanner::fetchString() {
  int len = 0;
  // We want to advance, but skip double quotes
  //  while ((next() != ') || ((next() == ') && (next(2) == ')) && (next() != '\n')
  while (((ahead(len+1) != '\'') ||
	  ((ahead(len+1) == '\'') && (ahead(len+2) == '\''))) &&
	 (ahead(len+1) != '\n')) {
    if ((ahead(len+1) == '\'') &&
	(ahead(len+2) == '\'')) len+=2;
    else
      len++;
  }
  if (ahead(len+1) == '\n')
    throw Exception("unterminated string" + context());
  QString ret(m_text.mid(m_ptr+1,len));
  ret.replace("''","'");
  setToken(TOK_STRING,ret);
  m_ptr += len+2;
}

void Scanner::fetchWhitespace() {
  int len = 0;
  while (isablank(ahead(len))) len++;
  setToken(TOK_SPACE);
  m_ptr += len;
}


//A number consists of something like:
//{integer}.{integer}E{sign}{integer}
//   s1   s2   s3   s4  s5    s6
// .{integer}E{sign}{integer}
//
// <Float><Exponent>
// <Float>
// <Integer>
//
// flags - int, float, double, complex
//

void Scanner::fetchNumber() {
  int len = 0;
  int lookahead = 0;
  bool imagnumber = false;
  bool singleprecision = false;

  while (isdigit(ahead(len))) len++;
  lookahead = len;
  if (ahead(lookahead) == '.') {
    lookahead++;
    len = 0;
    while (isdigit(ahead(len+lookahead))) len++;
    lookahead+=len;
  }
  if ((ahead(lookahead) == 'E') ||
      (ahead(lookahead) == 'e')) {
    lookahead++;
    if ((ahead(lookahead) == '+') ||
	(ahead(lookahead) == '-')) {
      lookahead++;
    }
    len = 0;
    while (isdigit(ahead(len+lookahead))) len++;
    lookahead+=len;
  }
  if ((ahead(lookahead) == 'f') || (ahead(lookahead) == 'F')) {
    singleprecision = true;
    lookahead++;
  } 
  if ((ahead(lookahead) == 'd') || (ahead(lookahead) == 'D')) {
    lookahead++;
  }
  // Recognize the complex constants, but strip the "i" off
  if ((ahead(lookahead) == 'i') ||
      (ahead(lookahead) == 'I') ||
      (ahead(lookahead) == 'j') ||
      (ahead(lookahead) == 'J')) {
    imagnumber = true;
  }
  // Back off if we aggregated a "." from "..." into the number
  if (((ahead(lookahead-1) == '.') &&
       (ahead(lookahead) == '.') &&
       (ahead(lookahead+1) == '.') &&
       (ahead(lookahead+2) != '.')) ||
      ((ahead(lookahead-1) == '.') &&
       ((ahead(lookahead) == '*') ||
	(ahead(lookahead) == '/') ||
	(ahead(lookahead) == '\\') ||
	(ahead(lookahead) == '^') ||
	(ahead(lookahead) == '\'')))) lookahead--;
  QString numtext(m_text.mid(m_ptr,lookahead));
  m_ptr += lookahead;
  if (imagnumber)
    m_ptr++;
  if (!imagnumber) {
    if (singleprecision)
      setToken(TOK_REALF,numtext);
    else
      setToken(TOK_REAL,numtext);
  } else {
    if (singleprecision)
      setToken(TOK_IMAGF,numtext);
    else
      setToken(TOK_IMAG,numtext);
  }
}

void Scanner::fetchIdentifier() {
  int len = 0;
  while (isalnumus(ahead(len))) len++;
  // Collect the identifier into a string
  QString ident(m_text.mid(m_ptr,len));
  if (fm_reserved.contains(ident))
    setToken(fm_reserved[ident]);
  else
    setToken(TOK_IDENT,m_text.mid(m_ptr,len));
  m_ptr += len;
}

// A Blob is either:
//   1.  A regular string (with quote delimiters)
//   2.  A sequence of characters with either a whitespace
//       a comma or a colon.
void Scanner::fetchBlob() {
  if (current() == '\'') {
    fetchString();
    m_tokValid = true;
  } else {
    int len = 0;
    while ((ahead(len) != '\n') && (!isablank(ahead(len))) && 
	   (ahead(len) != '%') && (ahead(len) != ',') &&
	   (ahead(len) != ';')) len++;
    if (len > 0) {
      setToken(TOK_STRING,m_text.mid(m_ptr,len));
      m_ptr += len;
      m_tokValid = true;    
    } 
  }
}

const Token& Scanner::next() {
  while (!m_tokValid) {
    fetch();
    if ((m_ptr < m_strlen) && (current() == '\n'))
      m_linenumber++;
  }
  if (m_inContinuationState && m_tokValid && !m_tok.is(TOK_EOF))
    m_inContinuationState = false;
  return m_tok;
}

bool Scanner::inContinuationState() {
  return m_inContinuationState;
}

bool Scanner::inBracket() {
  return (m_bracketDepth>0);
}

void Scanner::consume() {
  m_tokValid = false;
}

TokenValueType Scanner::current() {
  if (m_ptr < m_strlen)
    return m_text.at(m_ptr).unicode();
  else
    return 0;
}

TokenValueType Scanner::previous() {
  if (m_ptr)
    return m_text.at(m_ptr-1).unicode();
  else
    return 0;
}

void Scanner::pushWSFlag(bool ignoreWS) {
  m_ignorews.push(ignoreWS);
}

void Scanner::popWSFlag() {
  m_ignorews.pop();
}

TokenValueType Scanner::ahead(int n) {
  if ((m_ptr+n) >= (int)(m_text.size()))
    return 0;
  else
    return m_text.at(m_ptr+n).unicode();
}

QString Scanner::context() {
  return context(contextNum());
}

QString Scanner::snippet(unsigned pos1, unsigned pos2) {
  unsigned ptr1 = pos1 >> 16;
  unsigned ptr2 = pos2 >> 16;
  return m_text.mid(ptr1,ptr2-ptr1-1);
}

QString Scanner::context(unsigned pos) {
  pos = pos >> 16;
  int line_start = 0;
  int linenumber = 1;
  int line_stop = m_text.indexOf("\n");
  QString prevline;
  while ((int(pos) > line_stop) && (linenumber < 10000)) {
    prevline = m_text.mid(line_start,line_stop-line_start);
    line_start = line_stop+1;
    line_stop = m_text.indexOf("\n",line_start);
    linenumber++;
  }
  QString retstring;
  if ((m_filename.size() > 0) && (linenumber < 10000)) {
    retstring = " at line number: " + QString::number(linenumber);
    retstring += " of file " + m_filename + "\n";
  }  else
    retstring += "\n";
  retstring += "     " + prevline + "\n";
  retstring += "     " + m_text.mid(line_start,line_stop-line_start);
  int offset = pos-line_start-1;
  if (offset < 0) offset = 0;
  retstring += "\n     " + QString(offset,' ') + "^";
  return(retstring);
}
