/*
 * Copyright (c) 2002-2006 Samit Basu
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
#include <QtGui>
#include "highlighter.hpp"
#include "Interpreter.hpp"

Highlighter::Highlighter(QTextDocument *parent)
  : QSyntaxHighlighter(parent)
{
  QSettings settings("FreeMat", Interpreter::getVersionString());

  keywordColor = settings.value("editor/syntax_colors/keyword",Qt::darkBlue).value<QColor>();
  commentColor = settings.value("editor/syntax_colors/comments",Qt::darkRed).value<QColor>();
  stringColor = settings.value("editor/syntax_colors/strings",Qt::darkGreen).value<QColor>();
  untermStringColor = settings.value("editor/syntax_colors/untermstrings",Qt::darkRed).value<QColor>();


  QTextCharFormat keywordFormat;
  keywordFormat.setForeground(keywordColor);
  keywordFormat.setFontWeight(QFont::Bold);

  QStringList keywordPatterns;
  keywordPatterns << "\\bbreak\\b" <<
    "\\bcase\\b" <<
    "\\bcatch\\b" <<
    "\\bcontinue\\b" <<
    "\\belse\\b" <<
    "\\belseif\\b" <<
    "\\bend\\b" <<
    "\\bfor\\b" <<
    "\\bfunction\\b" <<
    "\\bglobal\\b" <<
    "\\bif\\b" <<
    "\\bkeyboard\\b" <<
    "\\botherwise\\b" <<
    "\\bpersistent\\b" <<
    "\\bquit\\b" <<
    "\\bretall\\b" <<
    "\\breturn\\b" <<
    "\\bswitch\\b" <<
    "\\btry\\b" <<
    "\\bwhile\\b";
  foreach (QString pattern, keywordPatterns)
    mappings[pattern] = keywordFormat;

  singleLineCommentFormat.setForeground(commentColor);
  stringFormat.setForeground(stringColor);
  untermStringFormat.setForeground(untermStringColor);

  highlightingEnabled = settings.value("editor/syntax_enable",true).toBool();

  //  mappings["[^'\\]\\)\\}A-Za-z0-9]'[^']*'"] = stringFormat;
}

void Highlighter::highlightBlock(const QString &text)
{
  if (text.isEmpty() || (!highlightingEnabled)) return;
  foreach (QString pattern, mappings.keys()) {
    QRegExp expression(pattern);
    int index = text.indexOf(expression);
    while (index >= 0) {
      int length = expression.matchedLength();
      setFormat(index, length, mappings[pattern]);
      index = text.indexOf(expression, index + length);
    }
  }

  QRegExp sutest("[^'\\]\\)\\}A-Za-z0-9]'[^']*");
  QRegExp droptest("[\\[\\(\\{A-Za-z0-9]");
  int index = text.indexOf(sutest);
  while (index >=0) {
    int length = sutest.matchedLength();
    if (length>0) {
      QString first(text[index]); 
      int q = first.indexOf(droptest);
      if (q >= 0) {
	index++;
	length--;
      }
    }
    setFormat(index+1, length-1, untermStringFormat);
    index = text.indexOf(sutest, index + length);
  }

    enum
    {
        NORMAL = -1,
        NORMAL_WITH_VAR,
        NORMAL_IN_PAR,
        NORMAL_IN_PAR_WITH_VAR,
        COMMENT,
        STRING
    };
   int state = NORMAL;
   int start = 0;
   int nPar = 0;
   for (int i = 0; i < text.count(); i++)
    {
        QChar ch = text.at(i);
        switch (state)
        {
            case NORMAL:
                if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || 
                    (ch >= '0' && ch <= '9') || (ch == ']'))
                    state = NORMAL_WITH_VAR;                    
                else if ((ch == '(') ) {
                    state = NORMAL_IN_PAR;
                    nPar++;
                }
                else if (ch == '%') 
                {
                    state = COMMENT;
                    start = i;
                    // it looks like each text line is a block
                    // so format until the end of line and return
                     setFormat(start, text.size()-start, singleLineCommentFormat);
                    return;
                }
                else if (ch == '\'' )
                {
                    state = STRING;
                    start = i;
                }
                break;
            case NORMAL_WITH_VAR:
                if ((ch == ' ') || (ch == ',') || (ch == '[') ) {
                    if (nPar <= 0)
                        state = NORMAL;
                    else
                        state = NORMAL_IN_PAR;
                }                   
                else if ((ch == '(') ) {
                    state = NORMAL_IN_PAR;
                    nPar++;
                }
                else if (ch == '%')
                {
                    state = COMMENT;
                    start = i;
                    setFormat(start, text.size()-start, singleLineCommentFormat);
                    return;
                }
                break;
            case NORMAL_IN_PAR:
                if (ch == ')') {
                    nPar--;
                    if (nPar <= 0)
                        state = NORMAL_WITH_VAR;
                }
                else if (ch == '%') 
                {
                    state = COMMENT;
                    start = i;
                    setFormat(start, text.size()-start, singleLineCommentFormat);
                    return;
                }
                else if ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z') || 
                    (ch >= '0' && ch <= '9') || (ch == ']'))
                    state = NORMAL_IN_PAR_WITH_VAR;                    
                else if (ch == '\'' )
                {
                    state = STRING;
                    start = i;
                }
                break;
            case NORMAL_IN_PAR_WITH_VAR:
                if (ch == ')') {
                    nPar--;
                    if (nPar <= 0)
                        state = NORMAL_WITH_VAR;
                }
                else if (ch == '%') 
                {
                    state = COMMENT;
                    start = i;
                    setFormat(start, text.size()-start, singleLineCommentFormat);
                    return;
                }
                else if ((ch == ' ')  || (ch == ',')  || (ch == '\'') ) {
                    state = NORMAL_IN_PAR;
                }
                break;
            case STRING:
                if (ch == '\'')
                {
                   if (nPar <= 0)
                        state = NORMAL;
                   else
                        state = NORMAL_IN_PAR;
                   setFormat(start, i-start+1, stringFormat);
                }
                break;
        }
    }
}

