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

#ifndef __SYMBOLTABLE_HPP__
#define __SYMBOLTABLE_HPP__
#include <QHash>
#include "Types.hpp"

namespace FM { //have to use namespace to avoid conflict with llvm::SymbolTable

	template <class T>
	class SymbolTable {
	  QHash<QString,T> map;
	public:
	  SymbolTable() { }
	  ~SymbolTable() { }
	  T* findSymbol(const QString& key) {
		if (map.contains(key)) {
		  return &(map[key]);
		} 
		return 0;
	  }
	  bool hasSymbol(const QString& key) {
	    return map.contains(key);
	  }
	  void deleteSymbol(const QString& key) {
		map.remove(key);
	  }
	  void insertSymbol(const QString& key, const T& val) {
		map.insert(key,val);
	  }
	  StringVector getCompletions(const QString& prefix) {
		StringVector retvec;
		for (typename QHash<QString,T>::const_iterator i=map.constBegin();i != map.constEnd();++i) {
		  if ((i.key().size() > prefix.size()) &&
		  (i.key().left(prefix.size()) == prefix))
		retvec << i.key();
		}
		return retvec;
	  }
	};
}

#endif
