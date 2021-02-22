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
#ifndef __Label_hpp__
#define __Label_hpp__

#include "QPWidget.hpp"
#include "Point2D.hpp"

class Box {
public:
  int width;
  int height;
  int depth;
  Box();
};

class FormulaTree {
  FormulaTree* m_supertree;
  FormulaTree* m_subtree;
  FormulaTree* m_nexttree;
  QString m_text;
  int m_size;
  int m_ascent;
public:
  FormulaTree(QString text, FormulaTree* supertree, 
	      FormulaTree* subtree, FormulaTree* nexttree);
  void PrintMe();
  Box GetBox();
  void SizeTree(int size, int ascent);
  void Render(DrawEngine& gc, Point2D& pos);
};

class TexLabel {
  std::string m_rawtext;
  QString m_processed_text; // (unicode) character stream
  FormulaTree *m_tree;
  int m_size;
  Box m_box;
  void Substitute(QString ecode, QChar rcode);
  void DoSubstitutions();
  void CompileRawText();
  void Stringify();
public:
  TexLabel(std::string text, int size);
  Point2D BoundingBox();
  Box Metrics();
  void Render(DrawEngine& gc, Point2D pos);
};

class Label : public QPWidget {
private:
  std::string m_text;
  TexLabel *m_label;
  char m_orientation;
public:
  Label(QWidget* parent, const char *name, std::string text, char orient);
  virtual ~Label();
  void DrawMe(DrawEngine& gc);
};

#endif
