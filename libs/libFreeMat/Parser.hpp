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
#ifndef __Parser_hpp__
#define __Parser_hpp__

#include <QString>
#include "Scanner.hpp"
#include "Tree.hpp"

class Parser {
  Scanner &m_lex;
  unsigned lastpos;
  QString lasterr;
  bool octCompat;
  void serror(QString);
  bool match(TokenValueType t);
  void consume();
  const Token& expect(TokenValueType t, QString because = "");
  const Token& next();
  bool matchNumber();
  void flushSeperators();
private:
  Tree transposeFixup(Tree base);
  Tree forStatement();
  Tree functionDefinition();
  Tree forIndexExpression();
  Tree singletonStatement();
  Tree dBStepOrTraceStatement();
  Tree whileStatement();
  Tree ifStatement();
  Tree switchStatement();
  Tree tryStatement();
  Tree expression();
  Tree statement();
  Tree declarationStatement();
  Tree statementSeperator();
  Tree identifier();
  Tree assignmentStatement();
  void indexingExpressions(Tree&, bool blankRefOK);
  Tree variableDereference(bool blankRefOK = true);
  Tree multiFunctionCall();
  Tree matDef(TokenValueType basetok, TokenValueType closebracket);
  Tree matrixDefinition();
  Tree rowVectorDefinition();
  Tree specialFunctionCall();
  Tree keyword();
  Tree exp(unsigned p);
  Tree primaryExpression();
  Tree anonymousFunction();
  Tree statementList();
public:
  Parser(Scanner& lex, bool octaveCompatibility = false);
  Tree process();
  Tree processStatementList();
  QString lastErr() {return lasterr;}
  unsigned lastPos() {return lastpos;}
  void dump(); 

  friend Tree ParseExpressionString(QString);
};

Tree ParseString(QString arg);
Tree ParseExpressionString(QString arg);

#endif
