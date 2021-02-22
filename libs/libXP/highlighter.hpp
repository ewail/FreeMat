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
#ifndef __HIGHLIGHTER_HPP__
#define __HIGHLIGHTER_HPP__

#include <QSyntaxHighlighter>
#include <QHash>
#include <QTextCharFormat>

class QTextDocument;

class Highlighter : public QSyntaxHighlighter {
  Q_OBJECT

public:
  Highlighter(QTextDocument *parent = 0);
protected:
  void highlightBlock(const QString &text);
private:
  QHash<QString,QTextCharFormat> mappings;
  QTextCharFormat singleLineCommentFormat;
  QTextCharFormat untermStringFormat;
  QTextCharFormat stringFormat;

  QColor keywordColor;
  QColor commentColor;
  QColor stringColor;
  QColor untermStringColor;
  bool highlightingEnabled;
};

#endif
