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
#include "Class.hpp"
#include "Context.hpp"
#include "Struct.hpp"
#include "Array.hpp"
#include "Algorithms.hpp"

// The meta-info for a class
struct UserClassMetaInfo {
  StringVector fieldNames;
  StringVector parentClasses;
};

// Class information is mapped to a class template through an ID number
static QMutex classMutex;
static QMap<QString,UserClassMetaInfo> classTable;

// some behavioral observations on classes.
//  The first call to "class" is the definitive one.
//  The exact order of the structure fieldnames must be the same 
//     for all objects
//  The list of parent objects must also be the same for all objects
//  So, classes are stored as the following:
//     class UserClass {
//         StringVector fieldNames;
//         StringVector parentClasses;
//     }
//  Also, somewhere we require a table that
//  tracks the hierarchy relationship of the classes.
// 
// To Do:
//   change grepping code to look for classes
//   change function eval code to handle classes
//
// These are both done.  Next is the issue of parent classes.
// What does it mean when we have one or more parent classes?
// The structure is simple enough (simply add a new field with
// the name of the parent class).  But what about methods?
// When we call a method of the parent class on the current class
// what does it get passed?
//
//  The answer:
//   Suppose g is of class1, and inherits class2, and class3.
//   Then g has fields class2 and class3.
//   When we call 
//     method(g), where method is a member function of class2, then
//   effectively, the output is the same as
//     method(g.class2)
//     p = g
//     p.class2 = method(g.class2)
//   Odd - that's not quite right...  it must be more complicated than that
// Class related issues
//    rhs subscripting
//    assignment
//
// What about function pointers?   - done
//
// Need overload capability for
//   And
//   Or
//   Uplus
//   a(s1,s2,...,sn) - subsref
//   a(s1, ..., sn) - subsasgn
//   b(a) - subsindex
//   [a b] - horzcat
//   [a; b] - vertcat
//   Colon
//   end (!)
//
// More ideas on overloading classes...
//
// What happens when the parent classes are different sizes - resolved
//    force parent classes to be the same size as the created object
//
// In c++, polymorphism is done through the notion of a pointer and
// type casting.  But we can't do exactly the same thing... Because
// when we type-cast, only methods and fields from the type-cast
// object are present... 
//
// What we want is 
//   a.class1.class2.foo = 32
// In this case, a is of some class (e.g., class3).  But we want to
// call some method on a that belongs to class2.  now, inside the
// method, we want something like
//    x.foo = 32
// but _x_ has to be tagged with prefix information, because _x_ is
// really of class class3.  The tag has to be on the object because
// if there are multiple arguments to the function, they can be
// typecast at different levels.  Also, it tracks only the _instance_
// of the array, not the core array itself.  So the information has
// to be tagged on the array somehow.
//
// One idea is to replace the class name with the class path.  So if
// a is of type class3, but we want to access it as a type class2,
// we "cast" it to type class3:class1:class2.  Then, when accessing
// members of "a", we use the class list to determine the indexing
// sequence.  This casting operation can be done at the dispatch
// level.  Because the "struct" operation simply strips the class name
// from the object, it will still return the intact data array.
//
// To track precedence...
// 1.  Assume that inheritance and precedence do not interact (only the
// outermost class determines precedence).
// 2.  For each class, a list of superior classes is provided.
// 3.  A inf B <--> B sup A
// Precedence is then a simple question of testing interactions.

//
// To revisit the class implementation... 
//
// The fundamental issue is that I stored the parent classes
// as fields within the base class.  I'm not sure why I did this, 
// but it makes everything more complicated.  I have now replaced
// it with a simpler mechanism that simply merges the parent class
// fields into the fields of the child.
//
// This means that the following is no longer valid:
//   a.class1.class2.foo = 32
//  But I don't really see the point for it. 

static Array ClassAux(const Array &s, QString classname, const StringVector &parentNames, 
		      const ArrayVector &parents) {
  QMutexLocker lock(&classMutex);
  if (!s.isEmpty() && (s.dataClass() != Struct))
    throw Exception("first argument to 'class' function must be a structure");
  // First look up the class ID
  UserClassMetaInfo meta;
  if (!classTable.contains(classname)) {
    meta.fieldNames = FieldNames(s);
    for (int i=0;i<parentNames.size();i++) {
      if (!classTable.contains(parentNames[i]))
	throw Exception("parent object types must be defined at time of class creation");
      meta.parentClasses.push_back(parentNames[i]);
    }
    classTable[classname] = meta;
  } else
    meta = classTable.value(classname);
  StringVector s_fields(FieldNames(s));
  if (meta.fieldNames != s_fields)
    {
      qDebug() << "Registered names " << meta.fieldNames << " vs passed " << s_fields;
      throw Exception("fieldnames of structure provided must match the fieldnames for the registered class");
    }
  // Now check to make sure all of the parent objects are the same size as the source object
  for (int i=0;i<parents.size();i++) 
    if (s.dimensions() != parents[i].dimensions())
      throw Exception("parent object much match dimensions of the structure used to make the object");
  Array retval(Struct);
  StructArray &rp(retval.structPtr());
  const StructArray &sp(s.constStructPtr());
  // First prepend the fields of the parent classes
  for (int i=0;i<parentNames.size();i++)
    {
      const StructArray &pp(parents[i].constStructPtr());
      StringVector p_fields(FieldNames(pp));
      for (int j=0;j<p_fields.size();j++)
	rp[p_fields.at(j)] = pp[p_fields.at(j)];
    }
  // Then append the class specific fields
  for (int i=0;i<s_fields.size();i++)
    rp[s_fields.at(i)] = sp[s_fields.at(i)];
  rp.setClassName(classname);
  rp.updateDims();
  return retval;
}

//DOCBLOCK class_class

//DOCBLOCK class_constructors

//DOCBLOCK class_horzcat

//DOCBLOCK class_vertcat
  
//DOCBLOCK class_or

//DOCBLOCK class_and

//DOCBLOCK class_lt

//DOCBLOCK class_gt
  
//DOCBLOCK class_le

//DOCBLOCK class_ge
  
//DOCBLOCK class_eq

//DOCBLOCK class_ne

//DOCBLOCK class_plus

//DOCBLOCK class_minus

//DOCBLOCK class_mtimes

//DOCBLOCK class_times
  
//DOCBLOCK class_rdivide

//DOCBLOCK class_ldivide

//DOCBLOCK class_mrdivide

//DOCBLOCK class_mldivide


//DOCBLOCK class_uminus

//DOCBLOCK class_uplus

//DOCBLOCK class_not

//DOCBLOCK class_mpower

//DOCBLOCK class_power

//DOCBLOCK class_ctranspose

//DOCBLOCK class_transpose

//DOCBLOCK class_colon

//DOCBLOCK class_subsref

//DOCBLOCK class_subsasgn

//@@Signature
//function class ClassFunction
//input varargin
//output classname
//DOCBLOCK class_subsindex
ArrayVector ClassFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() == 0)
    throw Exception("class function requires at least one argument");
  if (arg.size() == 1) 
    return ArrayVector(Array(arg[0].className()));
  ArrayVector parents;
  StringVector parentNames;
  for (int i=2;i<arg.size();i++) {
    if (!arg[i].isUserClass())
      throw Exception("parent objects must be user defined classes");
    parents.push_back(arg[i]);
    parentNames.push_back(arg[i].className());
  }
  return ArrayVector(ClassAux(arg[0],arg[1].asString(),parentNames,parents));
}

static QVector<int> MarkUserClasses(ArrayVector t) {
  QVector<int> set;
  for (int j=0;j<t.size();j++) 
    if (t[j].isUserClass()) set.push_back(j);
  return set;
}
 
static bool ClassSearchOverload(Interpreter* eval, ArrayVector t, 
				QVector<int> userset, FuncPtr &val,
				QString name) {
  bool overload = false;
  int k = 0;
  while (k<userset.size() && !overload) {
    overload = 
      eval->getContext()->lookupFunction(ClassMangleName(t[userset[k]].className(),name),val);
    if (!overload) k++;
  }
  return overload;
}

Array ClassMatrixConstructor(ArrayMatrix m, Interpreter* eval) {
  // Process the rows...
  // If a row has no user defined classes, then
  // use the standard matrixConstructor
  ArrayVector rows;
  for (int i=0;i<m.size();i++) {
    // check this row - make a list of columns that are
    // user classes
    QVector<int> userset(MarkUserClasses(m[i]));
    if (userset.empty()) {
      ArrayMatrix n;
      n.push_back(m[i]);
      rows.push_back(MatrixConstructor(n));
    } else {
      FuncPtr val;
      bool horzcat_overload = ClassSearchOverload(eval,m[i],userset,val,"horzcat");
      if (!horzcat_overload)
	throw Exception("no overloaded version of horzcat found");
      // scan through the list of user defined classes - look
      // for one that has "horzcat" overloaded
      val->updateCode(eval);
      ArrayVector p;
      p = eval->doFunction(val,m[i],1);
      if (!p.empty())
	rows.push_back(p[0]);
      else {
	eval->warningMessage("'horzcat' called for user defined class and it returned nothing.  Substituting empty array for result.");
	rows.push_back(EmptyConstructor());
      }
    }
  }
  // Check for a singleton - handle with special case
  if (rows.size() == 1)
    return rows[0];
  // At this point we have a vector arrays that have to vertically
  // concatenated.  There may not be any objects in it, so we have 
  // to rescan.
  QVector<int> userset(MarkUserClasses(rows));
  if (userset.empty()) {
    // OK - we don't have any user-defined classes anymore,
    // so we want to call matrix constructor, which needs
    // an ArrayMatrix instead of an ArrayVector.
    ArrayMatrix ref;
    for (int i=0;i<rows.size();i++)
      ref.push_back(ArrayVector(rows[i]));
    return MatrixConstructor(ref);
  } else {
    // We do have a user defined class - look for a vertcat defined
    FuncPtr val;
    bool vertcat_overload = ClassSearchOverload(eval,rows,userset,val,"vertcat");
    if (!vertcat_overload)
      throw Exception("no overloaded version of vertcat found");
    val->updateCode(eval);
    ArrayVector p;
    p = eval->doFunction(val,rows,1);
    if (!p.empty())
      return p[0];
    else
      return EmptyConstructor();
  }
  return EmptyConstructor();
}

Array ClassUnaryOperator(Array a, QString funcname, Interpreter* eval) {
  FuncPtr val;
  ArrayVector m, n;
  if (eval->getContext()->lookupFunction(ClassMangleName(a.className(),funcname),val)) {
    val->updateCode(eval);
    m.push_back(a);
    n = eval->doFunction(val,m,1);
    if (!n.empty())
      return n[0];
    else
      return EmptyConstructor();
  }
  throw Exception("Unable to find a definition of " + 
		  funcname + " for arguments of class " + a.className());
}

// ClassResolveHelper
static bool ClassResolveHelper(Interpreter* eval, QString className, QString funcName, FuncPtr& val) {
  Context *context = eval->getContext();
  if (context->lookupFunction(ClassMangleName(className,funcName),val))
    return true;
  UserClassMetaInfo einfo = classTable[className];
  // Now check each of the parent classes
  for (int i=0;i<einfo.parentClasses.size();i++) 
    if (ClassResolveHelper(eval,einfo.parentClasses.at(i),funcName,val))
      return true;
  return false;
}

bool isParentClass(QString parent, QString child)
{
  UserClassMetaInfo einfo = classTable[child];
  for (int i=0;i<einfo.parentClasses.size();i++)
    if (einfo.parentClasses.at(i) == parent) return true;
  return false;
}

bool ClassResolveFunction(Interpreter* eval, Array& args, QString funcName, FuncPtr& val) {
  Context *context = eval->getContext();
  return ClassResolveHelper(eval,args.className(),funcName,val);
}


// TODO - add "inferiorto", etc and class precedence

static Array ClassBiOp(Array a, Array b, FuncPtr val, Interpreter *eval) {
  val->updateCode(eval);
  ArrayVector m, n;
  m.push_back(a); m.push_back(b);
  n = eval->doFunction(val,m,1);
  if (!n.empty())
    return n[0];
  else
    return EmptyConstructor();
}

static Array ClassTriOp(Array a, Array b, Array c, FuncPtr val, Interpreter *eval) {
  val->updateCode(eval);
  ArrayVector m, n;
  m.push_back(a); m.push_back(b); m.push_back(c);
  n = eval->doFunction(val,m,1);
  if (!n.empty())
    return n[0];
  else
    return EmptyConstructor();
}

Array ClassTrinaryOperator(Array a, Array b, Array c, QString funcname,
			   Interpreter* eval) {
  FuncPtr val;
  if (a.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(a.className(),funcname),val)) 
      return ClassTriOp(a,b,c,val,eval);
    throw Exception(QString("Unable to find a definition of ") + funcname + 
		    " for arguments of class " + a.className());
  } else if (b.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(b.className(),funcname),val)) 
      return ClassTriOp(a,b,c,val,eval);
    throw Exception(QString("Unable to find a definition of ") + funcname + 
		    " for arguments of class " + b.className());
  } else if (c.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(c.className(),funcname),val)) 
      return ClassTriOp(a,b,c,val,eval);
    throw Exception(QString("Unable to find a definition of ") + funcname + 
		    " for arguments of class " + c.className());
  }
  throw Exception("unexpected argument types for classtrinaryoperator");
}

Array ClassBinaryOperator(Array a, Array b, QString funcname,
			  Interpreter* eval) {
  FuncPtr val;
  if (a.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(a.className(),funcname),val)) 
      return ClassBiOp(a,b,val,eval);
    throw Exception(QString("Unable to find a definition of ") + funcname + 
		    " for arguments of class " + a.className());
  } else if (b.isUserClass()) {
    if (eval->getContext()->lookupFunction(ClassMangleName(b.className(),funcname),val)) 
      return ClassBiOp(a,b,val,eval);
    throw Exception(QString("Unable to find a definition of ") + funcname + 
		    " for arguments of class " + b.className());
  }
  throw Exception("unexpected argument types for classbinaryoperator");
}

Array IndexExpressionToStruct(Interpreter* eval, const Tree & t, Array r) {
  StringVector fNames;
  fNames.push_back("type");
  fNames.push_back("subs");
  ArrayVector type_args;
  ArrayVector subs_args;
  for (int index=1;index < t.numChildren();index++) {
    if (t.child(index).is(TOK_PARENS)) {
      ArrayVector m;
      const Tree & s(t.child(index));
      for (int p=0;p<s.numChildren();p++)
	eval->multiexpr(s.child(p),m);
      eval->subsindex(m);
      //        m = eval->varExpressionList(t[index].children(),r);
      //        // Scan through the expressions... adjust for "colon" calls
      //        AdjustColonCalls(m,t[index].children());

      // Removing to fix bug 3354203
      //      if (m.size() == 0) 
      //	throw Exception("Expected indexing expression!");

      // Take the arguments and push them into a cell array...
      ArrayMatrix q;	q.push_back(m);
      type_args.push_back(Array(QString("()")));
      subs_args.push_back(CellConstructor(q));
    }
    if (t.child(index).is(TOK_BRACES)) {
      ArrayVector m;
      const Tree & s(t.child(index));
      for (int p=0;p<s.numChildren();p++)
	eval->multiexpr(s.child(p),m);
      eval->subsindex(m);
      //        m = eval->varExpressionList(t[index].children(),r);
      //        AdjustColonCalls(m,t[index].children());
      if (m.size() == 0) 
	throw Exception("Expected indexing expression!");
      // Take the arguments and push them into a cell array...
      ArrayMatrix q;	q.push_back(m);
      type_args.push_back(Array(QString("{}")));
      subs_args.push_back(CellConstructor(q));
    }
    if (t.child(index).is('.')) {
      type_args.push_back(Array(QString(".")));
      subs_args.push_back(Array(t.child(index).first().text()));
    }
  }
  Array retval(Struct);
  StructArray& rp(retval.structPtr());
  rp["type"] = ArrayVectorToBasicArray(type_args);
  rp["subs"] = ArrayVectorToBasicArray(subs_args);
  rp.updateDims();
  return retval;
}
  
static ArrayVector ClassSubsrefCall(Interpreter* eval, const Tree & t, Array r, FuncPtr val) {
  ArrayVector p;
  p.push_back(r);
  p.push_back(IndexExpressionToStruct(eval,t, r));
  val->updateCode(eval);
  ArrayVector n = eval->doFunction(val,p,1);
  return n;
}

bool ClassHandleMethod(Array r, QString funcName, Interpreter* eval)
{
  FuncPtr p;
  if (!ClassResolveFunction(eval,r,funcName,p)) return false;
  p->updateCode(eval);
  ArrayVector q(r);
  eval->doFunction(p,q,1);
  return true;
}

// What is special here...  Need to be able to do field indexing
// 
ArrayVector ClassRHSExpression(Array r, const Tree & t, Interpreter* eval) 
{
  Array q;
  Array n, p;
  FuncPtr val;
   
  // New handle-type classes support normal C++ syntax for 
  // method invokation... so foo.bar(args) is really
  // bar(foo,args).  Lovely.
  if (!eval->inMethodCall(r.className()) &&
      r.constStructPtr().isHandleClass() &&
      t.second().is('.') &&
      (((t.numChildren() == 2) ||
	((t.numChildren() == 3) && (t.child(2).is(TOK_PARENS))))))
    {
      FuncPtr val;
      if (ClassResolveFunction(eval,r,t.second().first().text(),val))
	{
	  val->updateCode(eval);
	  ArrayVector q(r);
	  if (t.numChildren() == 2)
	    return eval->doFunction(val,q,1);
	  else
	    {
	      const Tree &s(t.child(2));
	      for (int p=0;p<s.numChildren();p++)
		eval->multiexpr(s.child(p),q);
	      return eval->doFunction(val,q,1);
	    }
	}
    }

  // Try and look up subsref, _unless_ we are already in a method 
  // of this class
  if (!eval->inMethodCall(r.className()))
    if (ClassResolveFunction(eval,r,"subsref",val)) {
      // Overloaded subsref case
      return ClassSubsrefCall(eval,t,r,val);
    }
  ArrayVector rv;
  for (int index=1;index < t.numChildren();index++) {
    if (!rv.empty()) 
      throw Exception("Cannot reindex an expression that returns multiple values.");
    if (t.child(index).is(TOK_PARENS)) {
      ArrayVector m;
      const Tree & s(t.child(index));
      for (int p=0;p<s.numChildren();p++)
	eval->multiexpr(s.child(p),m);
      eval->subsindex(m);
      //     m = eval->varExpressionList(t->child(index).children(),r);
      if (m.size() == 0) 
	throw Exception("Expected indexing expression!");
      else if (m.size() == 1) {
	q = r.get(m.front());
	r = q;
      } else {
	q = r.get(m);
	r = q;
      }
    }
    if (t.child(index).is(TOK_BRACES)) {
      ArrayVector m;
      const Tree & s(t.child(index));
      for (int p=0;p<s.numChildren();p++)
	eval->multiexpr(s.child(p),m);
      eval->subsindex(m);
      //     m = eval->varExpressionList(t->child(index).children(),r);
      if (m.size() == 0) 
	throw Exception("Expected indexing expression!");
      else if (m.size() == 1)
	rv = ArrayVectorFromCellArray(r.get(m.front()));
      else
	rv = ArrayVectorFromCellArray(r.get(m));
      if (rv.size() == 1) {
	r = rv[0];
	rv = ArrayVector();
      } else if (rv.size() == 0) {
	throw Exception("Empty expression!");
	r = EmptyConstructor();
      }
    }
    if (t.child(index).is('.')) {
      rv = r.get(t.child(index).first().text());
      if (rv.size() <= 1) {
	r = rv[0];
	rv = ArrayVector();
      }
    }
    if (t.child(index).is(TOK_DYN)) {
      QString field;
      try {
	Array fname(eval->expression(t.child(index).first()));
	field = fname.asString();
      } catch (Exception &e) {
	throw Exception("dynamic field reference to structure requires a string argument");
      }
      rv = r.get(field);
      if (rv.size() <= 1) {
	r = rv[0];
	rv = ArrayVector();
      }      
    }
  }
  if (rv.empty())
    rv.push_back(r);
  return rv;
}

void ClassAssignExpression(ArrayReference dst, const Tree & t, const Array& value, Interpreter* eval) {
  FuncPtr val;
  if (!ClassResolveFunction(eval,*dst,"subsasgn",val))
    throw Exception(QString("The method 'subsasgn' is not defined for objects of class ") + 
		    dst->className());
  ArrayVector p;
  p.push_back(*dst);
  p.push_back(IndexExpressionToStruct(eval,t, *dst));
  p.push_back(value);
  val->updateCode(eval);
  bool overload(eval->getStopOverload());
  eval->setStopOverload(true);
  ArrayVector n = eval->doFunction(val,p,1);
  eval->setStopOverload(overload);
  if (!n.empty())
    *dst = n[0];
  else
    eval->warningMessage(QString("'subsasgn' for class ") + dst->className() + 
			 QString(" did not return a value... operation has no effect."));
}

// Ideally, this would be the only place where the class name is mangled.
// However, currently, the same op is repeated in the Interface implementation
// code.
QString ClassMangleName(QString className, QString funcName) {
  return "@" + className + ":" + funcName;
}

void clearUserClasses() {
  classTable.clear();
}

void defineHierarchyForClass(QString classname, StringVector parents)
{
  QMutexLocker lock(&classMutex);
  UserClassMetaInfo info;
  info.parentClasses = parents;
  classTable[classname] = info;
}

bool isUserClassDefined(QString classname)
{
  QMutexLocker lock(&classMutex);
  return (classTable.contains(classname));
}

//@@Signature
//sfunction makehandleclass MakeHandleClassFunction
//input x
//output handle
ArrayVector MakeHandleClassFunction(int nargout, const ArrayVector &arg, 
			  Interpreter *eval) 
{
  if (arg.size() == 0) return ArrayVector();
  Array t = arg[0];
  StructArray& sa = t.structPtr();
  StructArray* mc = new StructArray(t.structPtr());
  StructArray handle(mc,eval);
  return Array(handle);
}
