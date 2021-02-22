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
#ifndef __Token_hpp__
#define __Token_hpp__

#include "Array.hpp"
#include <QTextStream>
#include <QChar>
#include "DebugStream.hpp"
#include <QSharedData>

typedef uint16 TokenValueType;

const TokenValueType TOK_INVALID = 0;
const TokenValueType TOK_IDENT = 130;
const TokenValueType TOK_SPACE = 132;
const TokenValueType TOK_STRING = 133;
const TokenValueType TOK_KEYWORD = 134;
const TokenValueType TOK_BREAK = 135;
const TokenValueType TOK_CASE = 136;
const TokenValueType TOK_CATCH = 137;
const TokenValueType TOK_CONTINUE = 138;
const TokenValueType TOK_DBSTEP = 139;
const TokenValueType TOK_DBTRACE = 140;
const TokenValueType TOK_ELSE = 141;
const TokenValueType TOK_ELSEIF = 142;
const TokenValueType TOK_END = 143;
const TokenValueType TOK_FOR = 144;
const TokenValueType TOK_FUNCTION = 145;
const TokenValueType TOK_GLOBAL = 146;
const TokenValueType TOK_IF = 147;
const TokenValueType TOK_KEYBOARD = 148;
const TokenValueType TOK_OTHERWISE = 149;
const TokenValueType TOK_PERSISTENT = 150;
const TokenValueType TOK_QUIT = 151;
const TokenValueType TOK_RETALL = 152;
const TokenValueType TOK_RETURN = 153;
const TokenValueType TOK_SWITCH = 154;
const TokenValueType TOK_TRY = 155;
const TokenValueType TOK_WHILE = 156;
// Generated (synthetic) token;
const TokenValueType TOK_MULTI = 157;
const TokenValueType TOK_SPECIAL = 158;
const TokenValueType TOK_VARIABLE = 159;
const TokenValueType TOK_DYN = 160;
const TokenValueType TOK_BLOCK = 161;
const TokenValueType TOK_EOF = 162;
const TokenValueType TOK_MATDEF = 163;
const TokenValueType TOK_CELLDEF = 164;
const TokenValueType TOK_PARENS = 165;
const TokenValueType TOK_BRACES = 166;
const TokenValueType TOK_BRACKETS = 167;
const TokenValueType TOK_ROWDEF = 168;
const TokenValueType TOK_UNARY_MINUS = 169;
const TokenValueType TOK_UNARY_PLUS = 170;
const TokenValueType TOK_EXPR = 171;
const TokenValueType TOK_DOTTIMES = 172;
const TokenValueType TOK_DOTRDIV = 173;
const TokenValueType TOK_DOTLDIV = 174;
const TokenValueType TOK_DOTPOWER = 175;
const TokenValueType TOK_DOTTRANSPOSE = 176;
const TokenValueType TOK_LE = 177;
const TokenValueType TOK_GE = 178;
const TokenValueType TOK_EQ = 179;
const TokenValueType TOK_NE = 180;
const TokenValueType TOK_SOR = 181;
const TokenValueType TOK_SAND = 182;
const TokenValueType TOK_QSTATEMENT = 183;
const TokenValueType TOK_STATEMENT = 184;
const TokenValueType TOK_REALF = 185;
const TokenValueType TOK_IMAGF = 186;
const TokenValueType TOK_REAL = 187;
const TokenValueType TOK_IMAG = 188;
const TokenValueType TOK_FUNCTION_DEFS = 190;
const TokenValueType TOK_SCRIPT = 191;
const TokenValueType TOK_ANONYMOUS_FUNC = 192;
const TokenValueType TOK_NEST_FUNC = 193;
const TokenValueType TOK_TYPE_DECL = 194;
const TokenValueType TOK_DBUP = 195;
const TokenValueType TOK_DBDOWN = 196;
const TokenValueType TOK_REINDEX = 197;
const TokenValueType TOK_INCR = 198;
const TokenValueType TOK_DECR = 199;
const TokenValueType TOK_INCR_PREFIX = 200;
const TokenValueType TOK_DECR_PREFIX = 201;
const TokenValueType TOK_INCR_POSTFIX = 202;
const TokenValueType TOK_DECR_POSTFIX = 203;
const TokenValueType TOK_PLUS_EQ = 204;
const TokenValueType TOK_MINUS_EQ = 205;

class Serialize;

class ParseException {
  unsigned m_pos;
  QString m_text;
public:
  ParseException(unsigned pos = 0, QString text = QString()) : m_pos(pos), m_text(text) {}
  unsigned Position() {return m_pos;}
  QString Text() {return m_text;}
};

class Token {
  TokenValueType m_tok;
  unsigned m_pos;
  QString m_text;
  Array m_array;
  int m_UID;
public:
  Token();
  Token(TokenValueType tok, unsigned pos = 0, QString text = QString());
  Token(Serialize *s);
  void freeze(Serialize *s) const;
  bool is(TokenValueType tok) const {return m_tok == tok;}
  bool isBinaryOperator() const;
  bool isUnaryOperator() const;
  unsigned precedence() const;
  bool isRightAssociative() const;
  void setUID(int uid) {m_UID = uid;}
  int UID() const {return m_UID;}
  TokenValueType value() const {return m_tok;}
  void setValue(TokenValueType a) {m_tok = a;}
  unsigned position()  const {return m_pos;}
  QString text()  const {return m_text;}
  void setText(QString txt) {m_text = txt;}
  Array array() const {return m_array;}
  void fillArray();
  void print(QTextStream& o) const;
};

QString TokenToString(const Token& b);

DebugStream& operator<<(DebugStream& o, const Token& b);

void FreezeToken(const Token& a, Serialize *s);

Token ThawToken(Serialize *s);

inline int LineNumber(int x) {return (x & 0xffff);}

#endif
