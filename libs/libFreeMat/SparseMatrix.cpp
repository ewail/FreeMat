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
#include "SparseMatrix.hpp"
#include "BasicArray.hpp"
#include "Interpreter.hpp"

template <typename T>
void PrintMe(const SparseMatrix<T> &ar, Interpreter* io) {
  ConstSparseIterator<T> source(&ar);
  while (source.isValid()) {
    while (source.moreInSlice()) {
      NTuple pos(source.pos());
      io->outputMessage(QString(" %1 %2 %3\n").arg(pos[1]).arg(pos[0]).arg(source.value()));
      source.next();
    }
    source.nextSlice();
  }
}

template <typename T>
void PrintMe(const SparseMatrix<T> &ar, const SparseMatrix<T> &ai, Interpreter* io) {
  ConstComplexSparseIterator<T> source(&ar,&ai);
  while (source.isValid()) {
    while (source.moreInSlice()) {
      NTuple pos(source.pos());
      if (source.imagValue() < 0)
	io->outputMessage(QString(" %1 %2 %3%4 i\n").arg(pos[1])
			  .arg(pos[0])
			  .arg(source.realValue())
			  .arg(source.imagValue()));
      else 
	io->outputMessage(QString(" %1 %2 %3+%4 i\n").arg(pos[1])
			  .arg(pos[0])
			  .arg(source.realValue())
			  .arg(source.imagValue()));
      source.next();
    }
    source.nextSlice();
  }
}
