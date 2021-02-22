/*
 * Copyright (c) 2011 Samit Basu
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
#include "OctaveScanner.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "Exception.hpp"
#include <algorithm>

static QHash<QString, TokenValueType> fm_oct_reserved;

static bool fm_oct_reserved_initialized = false;

static int TokenID = 1;

// TODO - static, until, do

static void InitializeReservedTable() {
  if (fm_oct_reserved_initialized) return;
  fm_oct_reserved["break"] = TOK_BREAK;
  fm_oct_reserved["case"] = TOK_CASE;
  fm_oct_reserved["catch"] = TOK_CATCH;
  fm_oct_reserved["continue"] = TOK_CONTINUE;
  fm_oct_reserved["dbstep"] = TOK_DBSTEP;
  fm_oct_reserved["dbtrace"] = TOK_DBTRACE;
  fm_oct_reserved["dbdown"] = TOK_DBDOWN;
  fm_oct_reserved["dbup"] = TOK_DBUP;
  fm_oct_reserved["else"] = TOK_ELSE;
  fm_oct_reserved["elseif"] = TOK_ELSEIF;
  fm_oct_reserved["end"] = TOK_END;
  fm_oct_reserved["endwhile"] = TOK_END;
  fm_oct_reserved["endswitch"] = TOK_END;
  fm_oct_reserved["endif"] = TOK_END;
  fm_oct_reserved["endfor"] = TOK_END;
  fm_oct_reserved["endfunction"] = TOK_END;
  fm_oct_reserved["end_try_catch"] = TOK_END;
  fm_oct_reserved["end_unwind_protect"] = TOK_END;
  fm_oct_reserved["for"] = TOK_FOR;
  fm_oct_reserved["function"] = TOK_FUNCTION;
  fm_oct_reserved["global"] = TOK_GLOBAL;
  fm_oct_reserved["if"] = TOK_IF;
  fm_oct_reserved["keyboard"] = TOK_KEYBOARD;
  fm_oct_reserved["otherwise"] = TOK_OTHERWISE;
  fm_oct_reserved["persistent"] = TOK_PERSISTENT;
  fm_oct_reserved["quit"] = TOK_QUIT;
  fm_oct_reserved["retall"] = TOK_RETALL;
  fm_oct_reserved["return"] = TOK_RETURN;
  fm_oct_reserved["switch"] = TOK_SWITCH;
  fm_oct_reserved["try"] = TOK_TRY;
  fm_oct_reserved["unwind_protect"] = TOK_TRY;
  fm_oct_reserved["unwind_protect_cleanup"] = TOK_CATCH;
  fm_oct_reserved["while"] = TOK_WHILE;
  fm_oct_reserved_initialized = true;
}

static bool isalnumus(TokenValueType a) {
  return (isalnum(a) || (a=='_'));
}

static bool isablank(TokenValueType a) {
  return (a==' ' || a=='\t' || a=='\r');
}

OctaveScanner::OctaveScanner(QString buf, QString fname) : Scanner(buf,fname) {
  InitializeReservedTable();
}

void OctaveScanner::fetchWhitespace() {
  int len = 0;
  while (isablank(ahead(len)) || ((ahead(len) == '\n') && !m_ignorews.top())) len++;
  setToken(TOK_SPACE);
  m_ptr += len;
}

void OctaveScanner::fetchOther() {
  if (current() == '.') {
    if (tryFetchBinary(".*",TOK_DOTTIMES)) return;
    if (tryFetchBinary("./",TOK_DOTRDIV)) return;
    if (tryFetchBinary(".\\",TOK_DOTLDIV)) return;
    if (tryFetchBinary(".^",TOK_DOTPOWER)) return;
    if (tryFetchBinary(".'",TOK_DOTTRANSPOSE)) return;
  }
  if (tryFetchBinary("++",TOK_INCR)) return;
  if (tryFetchBinary("--",TOK_DECR)) return;
  if (tryFetchBinary("+=",TOK_PLUS_EQ)) return;
  if (tryFetchBinary("-=",TOK_MINUS_EQ)) return;
  if (tryFetchBinary("<=",TOK_LE)) return;
  if (tryFetchBinary(">=",TOK_GE)) return;
  if (tryFetchBinary("==",TOK_EQ)) return;
  if (tryFetchBinary("!=",TOK_NE)) return;
  if (tryFetchBinary("&&",TOK_SAND)) return;
  if (tryFetchBinary("||",TOK_SOR)) return;
  if (current() == '!')
    {
      setToken('~'); 
      m_ptr++;
      return;
    }
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

void OctaveScanner::fetchIdentifier() {
  int len = 0;
  while (isalnumus(ahead(len))) len++;
  // Collect the identifier into a string
  QString ident(m_text.mid(m_ptr,len));
  if (fm_oct_reserved.contains(ident))
    setToken(fm_oct_reserved[ident]);
  else
    setToken(TOK_IDENT,m_text.mid(m_ptr,len));
  m_ptr += len;
}

void OctaveScanner::fetchOctString() {
  int len = 0;
  // We want to advance, but skip double quotes
  while ((ahead(len+1) != '\"') && (ahead(len+1) != '\n')) {
      len++;
  }
  if (ahead(len+1) == '\n')
    throw Exception("unterminated string" + context());
  QString ret(m_text.mid(m_ptr+1,len));
  setToken(TOK_STRING,ret);
  m_ptr += len+2;
}

void OctaveScanner::fetch() {
  if (m_ptr >= m_strlen)
    setToken(TOK_EOF);
  else if ((current() == '#') || (current() == '%')) {
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
  } else if (current() == '\"') {
    fetchOctString();
  } else if ((current() == '\n') && m_ignorews.top() && (m_ignorews.size() > 1)) {
    m_ptr++;
    return;
  } else
    fetchOther();
  m_tokValid = true;
}

