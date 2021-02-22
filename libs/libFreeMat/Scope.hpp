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

#ifndef __Scope_hpp__
#define __Scope_hpp__

/**
 * A Scope is a combination of a variable hashtable and a function hashtable.
 */
#include <string>
#include <QMutex>
#include <QHash>
#include <algorithm>

#include "Array.hpp"
#include "Types.hpp"
#include "SymbolTable.hpp"

typedef FM::SymbolTable<Array> VariableTable;


/**
 * A Scope is a collection of functions and variables all visible
 * at some point in the execution.  The scope also keeps track of
 * the loop level, and a list of the global and persistent variables
 * relevant to the current scope.
 */
class Scope {
  /**
   * This is a mutex to protect the scope when multiple threads
   * have access to the scope.  For all scopes (except the global
   * one, this mutex is unused.
   */
  QMutex *mutex;
  /**
   * The reference count for this Scope
   */
  int refcount;
  /**
   * This is the hash-table of Array pointers that forms the
   * symbol table.  Each variable has a name associated with
   * it that must be unique to the Scope.  The Scope owns the
   * variables in the symbol table, and is responsible for
   * destructing them when destroyed.
   */
  VariableTable symTab;
  /**
   * The name of the scope.
   */
  QString name;
  /**
   * Detailed information that indicates where the scope is from.
   */
  QString detail;
  /**
   * The context information (currently executed token ID)
   */
  int tokid;
  /**
   * The line number for the current step trap (from dbstep)
   */
  int steptrap;
  /**
   * The current line number from which we are stepping
   */
  int stepcurrentline;
  /**
   * Flag that indicates if this scope is active (if it is actually
   * used to store local variables -- for scripts, the corresponding 
   * scope is not active).
   */
  bool active;
  /**
   * The loop level.  This is used to track the depth of nested
   * loops.
   */
  int loopLevel;
  /**
   * These are the global variables as defined in the current
   * scope.  Global variables are not stored in this Scope, but
   * are deferred to the top scope in the Context.
   */
  StringVector globalVars;
  /**
   * Persistent variables are similar to global variables in that
   * they are deferred to the top scope in the Context.  However,
   * unlike global variables, persistent variables are mangled
   * with the name of the scope before being indexed into the global 
   * scope.
   */
  StringVector persistentVars;
  /**
   * This string vector contains the names of variables accessed (potentially)
   * in this scope.
   */
  StringVector variablesAccessed;
  /**
   * This string vector contains the names of variables that must be local to this
   * scope.
   */
  StringVector localVariables;
  /**
   * On every call to modify the scope, we have to check the global/persistent
   * variable table.  This is generally expensive, so we cache information
   * about these tables being empty (the usual case).
   */
  bool anyPersistents;
  bool anyGlobals;
  bool isNested;
  /**
   * The number of input/output arguments - used by M-functions that need to know
   */
  int numargin;
  int numargout;
public:
  /**
   * Construct a scope with the given name.
   */
  Scope(QString scopeName, bool nested) : mutex(NULL), refcount(0),
					  name(scopeName), tokid(0), steptrap(0), 
					  stepcurrentline(0), active(true), 
					  loopLevel(0), anyPersistents(false), anyGlobals(false),
					  isNested(nested)
  {}
  inline bool isActive() const {
    return active;
  }
  inline void setActive(bool x) {
    active = x;
  }
  inline int stepTrap() const {
    return steptrap;
  }
  inline void setStepTrap(int x) {
    steptrap = x;
  }
  inline int stepCurrentLine() const {
    return stepcurrentline;
  }
  inline void setStepCurrentLine(int x) {
    stepcurrentline = x;
  }
  inline int tokenID() const {
    return tokid;
  }
  inline void setTokenID(int x) {
    tokid = x;
  }
  inline int nargin() const {
    return numargin;
  }
  inline void setNargin(int x) {
    numargin = x;
  }
  inline int nargout() const {
    return numargout;
  }
  inline void setNargout(int x) {
    numargout = x;
  }
  inline QString detailString() {
    return detail;
  }
  inline void setDetailString(QString x) {
    detail = x;
  }
  inline void setVariablesAccessed(StringVector varList) {
    variablesAccessed = varList;
  }
  inline bool variableAccessed(QString varName) {
    for (int i=0;i<variablesAccessed.size();i++)
      if (variablesAccessed[i] == varName) return true;
    return false;
  }
  inline StringVector getVariablesAccessedList() {
    return variablesAccessed;
  }
  inline void setLocalVariables(StringVector varList) {
    localVariables = varList;
  }
  inline StringVector getLocalVariablesList() {
    return localVariables;
  }
  inline bool variableLocal(QString varName) {
    for (int i=0;i<localVariables.size();i++) 
      if (localVariables[i] == varName) return true;
    return false;
  }
  inline bool isnested() {
    return isNested; 
  }
  inline void setNestingFlag(bool x) {
    isNested = x;
  }
  inline bool nests(QString peerName) {
    // Nesting requires that our peer have a strictly more 
    // qualified (longer) name, and that our name is a prefix
    // of that name.
    return ((name.size() < peerName.size()) && 
	    (peerName.left(name.size()) == name));
  }
  static bool nests(QString name, QString peerName) {
    // Nesting requires that our peer have a strictly more 
    // qualified (longer) name, and that our name is a prefix
    // of that name.
    return ((name.size() < peerName.size()) && 
	    (peerName.left(name.size()) == name));
  }
  /**
   * Lock the scope's mutex
   */
  inline void lock() {
    if (mutex) mutex->lock();
  }
  /**
   * Unlock the scope's mutex
   */
  inline void unlock() {
    if (mutex) mutex->unlock();
  }
  /**
   * Initialize the scope's mutex - only needed in Global
   * scope.
   */
  inline void mutexSetup() {
    if (mutex) 
      delete mutex;
    mutex = new QMutex(QMutex::Recursive);
  }
  /**
   * Insert a variable with the given name.  If the variable
   * already exists in the Scope, then the previous definition
   * is replaced with the given one.
   */
  inline void insertVariable(const QString& varName, const Array& val) {
    symTab.insertSymbol(varName,val);
  }
  /**
   * Lookup a variable.  Return a pointer to the variable in the symbol 
   * table if found and NULL otherwise.  Different than lookupFunction
   * because in write-back assignments (e.g., A(:,346) = b) it is critical 
   * to manage the number of copies.
   */
  inline Array* lookupVariable(const QString& varName) {
    return symTab.findSymbol(varName);
  }
  /**
   * Add a variable name to the global variables list.
   */
  inline void addGlobalVariablePointer(QString varName) {
    if (!isVariableGlobal(varName)) {
      globalVars.push_back(varName);
      anyGlobals = true;
    }
  }
  inline StringVector getGlobalVariablesList() {
    return globalVars;
  }
  /**
   * Delete a variable name from the global variables list.
   */
  inline void deleteGlobalVariablePointer(QString varName) {
    StringVector::iterator i = std::find(globalVars.begin(),
					 globalVars.end(),
					 varName);
    if (*i == varName)
      globalVars.erase(i);
    if (globalVars.empty()) anyGlobals = false;
  }
  /**
   * Check to see if a variable is globally defined.
   */
  inline bool isVariableGlobal(const QString& varName) {
    if (!anyGlobals) return false;
    bool foundName = false;
    int i = 0;
    if (globalVars.empty()) return false;
    while (i<globalVars.size() && !foundName) {
      foundName = (globalVars[i] == varName);
      if (!foundName) i++;
    }
    return foundName;
  }
  /**
   * Add a variable name to the persistent variables list.
   */
  inline void addPersistentVariablePointer(QString varName) {
    if (!isVariablePersistent(varName)) {
      persistentVars.push_back(varName);
      anyPersistents = true;
    }
  }
  inline StringVector getPersistentVariablesList() {
    return persistentVars;
  }
  /**
   * Delete a variable name from the persistent variables list.
   */
  inline void deletePersistentVariablePointer(QString varName) {
    StringVector::iterator i = std::find(persistentVars.begin(),
					 persistentVars.end(),
					 varName);
    if (*i == varName)
      persistentVars.erase(i);
    if (persistentVars.empty()) anyPersistents = false;
  }
  /**
   * Check to see if a variable is defined in the persistent
   * list.
   */
  inline bool isVariablePersistent(const QString& varName) {
    if (!anyPersistents) return false;
    bool foundName = false;
    int i = 0;
    if (persistentVars.empty()) return false;
    while (i<persistentVars.size() && !foundName) {
      foundName = (persistentVars[i] == varName);
      if (!foundName) i++;
    }
    return foundName;
  }
  /**
   * Mangle the name of a variable by prepending
   * a "_scopename_" to the name of the variable.
   */
  inline QString getMangledName(QString varName) {
    return (QString("_") + name + QString("_") + varName);
  }
  /**
   * Get the name of the scope.
   */
  inline QString getName() {
    return name;
  }
  /**
   * Increment the loop counter.
   */
  inline void enterLoop() {
    loopLevel++;
  }
  /**
   * Decrement the loop counter.
   */
  inline void exitLoop() {
    loopLevel--;
  }
  /**
   * Test the loop counter.
   */
  inline bool inLoop() {
    return (loopLevel>0);
  }
  /**
   * Get a list of all possible completions of the given
   * string.
   */
   inline StringVector getCompletions(const QString& prefix) {
     return symTab.getCompletions(prefix);
   }
  /**
   * Returns a list of all currently defined variables
   * in the active scope.
   */
  inline StringVector listAllVariables() {
    StringVector names(symTab.getCompletions(""));
    for (int i=0;i<globalVars.size();i++)
      names.push_back(globalVars[i]);
    for (int i=0;i<persistentVars.size();i++)
      names.push_back(persistentVars[i]);
    return names;
  }
  /**
   * Delete a variable in this scope.  It does not simply
   * replace the variable with an empty variable, but deletes
   * the variable from the symbol table completely.
   */
  inline void deleteVariable(QString var) {
    symTab.deleteSymbol(var);
  }
  /**
   * Clear the list of global variable names
   */
  inline void clearGlobalVariableList() {
    globalVars.clear();
    anyGlobals = false;
  }
  /**
   * Clear the list of persistent variable names
   */
  inline void clearPersistentVariableList() {
    persistentVars.clear();
    anyPersistents = false;
  }
};

typedef Scope* ScopePtr;

#endif
