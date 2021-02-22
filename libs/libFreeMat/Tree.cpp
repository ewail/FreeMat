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
#include <string>
#include <stack>
#include <vector>
#include <iostream>
#include "Tree.hpp"
#include "Serialize.hpp"

Tree::Tree() {
  d = new TreeNode;
  d->m_node = Token(TOK_INVALID);
}

Tree::Tree(const Token& tok) {
  d = new TreeNode;
  d->m_node = tok;
  d->m_node.fillArray();
}

Tree::Tree(TokenValueType token, unsigned position) {
  d = new TreeNode;
  d->m_node = Token(token,position);
}

Tree::Tree(const Token& tok, Tree child1, Tree child2) {
  d = new TreeNode;
  d->m_node = tok;
  d->m_children.push_back(child1);
  d->m_children.push_back(child2);
}

Tree::Tree(const Token& tok, Tree child1) {
  d = new TreeNode;
  d->m_node = tok;
  d->m_children.push_back(child1);
}

Tree::~Tree() {}

static int indentlevel = 0;
void Tree::print() const {
  QTextStream out(stdout);
  out << QString(indentlevel,QChar(' '));
  out << TokenToString(d->m_node) << "\n";
  out.flush();
  indentlevel+=3;
  for (int i=0;i<d->m_children.size();i++)
    d->m_children[i].print();
  indentlevel-=3;
}

void Tree::freeze(Serialize *s) const {
  s->putBool(true); // For compatibility with 3.5?
  d->m_node.freeze(s);
  s->putInt(d->m_children.size());
  for (int i=0;i<d->m_children.size();i++)
    d->m_children.at(i).freeze(s);
}

Tree::Tree(Serialize *s) {
  bool ValidTree = s->getBool();
  if (!ValidTree) return;
  d->m_node = Token(s);
  int numchildren = s->getInt();
  for (int i=0;i<numchildren;i++)
    d->m_children.push_back(Tree(s));
}

static int indentlvl = 0;
DebugStream& operator<<(DebugStream& o, const Tree& b) {
    for( int i=0; i<indentlvl; i++ )
	o << "  ";
    o << b.node();
    indentlvl += 1;
    for (int i=0;i< b.numChildren();i++){
	o << b.child(i);
    }
    indentlvl -= 1;
    return o;
}
