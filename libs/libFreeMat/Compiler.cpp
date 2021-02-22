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
// Still need:
//  Type promotion on loop entry.
//  Range checking.
//  Dynamic array resizing.
//

//
// 
//

//
// A prototype compiler for FreeMat.  Consider the simplest function
//
//  function y = add(a,b)
//     y = a + b;
// The corresponding C++ code is
//
//  ArrayVector addFunction(const ArrayVector& args) {
//     Array a(args[0]);
//     Array b(args[1]);
//     Array y(Add(a,b));
//     ArrayVector varargout;
//     varargout.push_back(y);
//     return varargout;
//  }
//
// 
//
// Just for fun, mind you....
//

#include "Compiler.hpp"
#include "Tree.hpp"
#include <QString>
#include <QTextStream>
#include <QFile>
#include <set>

static int indent_level = 0;
static QFile *fp;
static QTextStream *ts;
static Interpreter *eval;
StringVector localIdents;
StringVector returnVals;
int forDepth;

bool IsVariableDefined(string varname) {
  for (int i=0;i<localIdents.size();i++)
    if (localIdents[i] == varname)
      return true;
  return false;
}

void EmitIndent() {
  for (int i=0;i<indent_level;i++)
    (*ts) << "   ";
}

void Emit(QString t) {
  (*ts) << t;
}

void EmitExpression(const tree &t);

void EmitMultiExpression(const tree &t);

void EmitSingleDeref(const tree &t) {
  if (t.is(TOK_PARENS)) {
    Emit(".getNDimSubset(");
    Emit("ArrayVector()");
    for (int p=0;p<t.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(t.child(p));
    }
    Emit(")");
  } else if (t.is(TOK_BRACES)) {
    Emit(".getNDimContents(");
    Emit("ArrayVector()");
    for (int p=0;p<t.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(t.child(p));
    }
    Emit(")");
  } else if (t.is('.')) {
    Emit(QString(".getField(") + QString::fromStdString(t.first().text()) + ")");
  } else if (t.is(TOK_DYN)) {
    Emit(".getField(ArrayToString(");
    EmitExpression(t.first());
    Emit("))");
  }
}

void EmitMultiDeref(const tree &t) {
  if (t.is(TOK_PARENS)) {
    Emit(".getNDimSubset(");
    Emit("ArrayVector()");
    for (int p=0;p<t.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(t.child(p));
    }
    Emit(")");
  } else if (t.is(TOK_BRACES)) {
    Emit(".getNDimContentsAsList(");
    Emit("ArrayVector()");
    for (int p=0;p<t.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(t.child(p));
    }
    Emit(")");
  } else if (t.is('.')) {
    Emit(QString(".getFieldAsList(") + QString::fromStdString(t.first().text()) + ")");
  } else if (t.is(TOK_DYN)) {
    Emit(".getFieldAsList(ArrayToString(");
    EmitExpression(t.first());
    Emit("))");
  }
}

void EmitMultiExpressionVariable(const tree &t) {
  Emit(QString::fromStdString(t.first().text()));
  for (int index=1;index<t.numchildren()-1;index++)
    EmitSingleDeref(t.child(index));
  if (t.numchildren() > 1)
    EmitMultiDeref(t.last());
}

void EmitMultiExpression(const tree &t) {
  if (t.is(TOK_VARIABLE)) {
    if (IsVariableDefined(t.first().text()))
      EmitMultiExpressionVariable(t);
//     else
//       EmitMultiExpressionFunction(t);
  } else if (!t.is(TOK_KEYWORD))
    EmitExpression(t);
}

void EmitBinaryOperator(const tree &t, QString opname) {
  Emit(QString("%1").arg(opname));
  Emit("(");
  EmitExpression(t.first());
  Emit(",");
  EmitExpression(t.second());
  Emit(")");
}

void EmitUnaryOperator(const tree &t, QString opname) {
  Emit(QString("%1").arg(opname));
  Emit("(");
  EmitExpression(t.first());
  Emit(")");
}

void EmitRHS(const tree &t) {
  // Check our list of defined variables
  if (IsVariableDefined(t.first().text())) {
    Emit(QString::fromStdString(t.first().text()));
    for (int index=1;index<t.numchildren();index++)
      EmitSingleDeref(t.child(index));
  } else {
    Emit("ToSingleArray(");
    Emit(QString("%1Function(").arg(QString::fromStdString(t.first().text())));
    Emit("1,");
    Emit("ArrayVector()");
    for (int i=0;i<t.second().numchildren();i++) {
      Emit(" << ");
      EmitMultiExpression(t.second().child(i));
    }
    Emit("))");
  }
}

void EmitExpression(const tree &t) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    EmitRHS(t);
    return;
  case TOK_INTEGER:
    Emit("Array::int32Constructor(");
    Emit(QString::fromStdString(t.text()));
    Emit(")");
    return;
  case TOK_FLOAT:
    Emit("Array::floatConstructor(");
    Emit(QString::fromStdString(t.text()));
    Emit(")");
    return;
  case TOK_DOUBLE:
    Emit("Array::doubleConstructor(");
    Emit(QString::fromStdString(t.text()));
    Emit(")");
    return;
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
    throw Exception("Unimplemented");
  case TOK_STRING:
    Emit(QString("\"%1\"").arg(QString::fromStdString(t.text())));
    return;
  case TOK_END:
    throw Exception("Unimplemented");
  case ':':
    if (t.numchildren() == 0) {
      Emit(QString("Array::stringConstructor(\":\")"));
    } else if (t.first().is(':')) {
      Emit("DoubleColon(");
      EmitExpression(t.first().first());
      Emit(",");
      EmitExpression(t.first().second());
      Emit(",");
      EmitExpression(t.second());
      Emit(")");
    } else {
      Emit("UnitColon(");
      EmitExpression(t.first());
      Emit(",");
      EmitExpression(t.second());
      Emit(")");
    }
    break;
  case TOK_MATDEF: 
    //    return matrixDefinition(t); 
    break;
  case TOK_CELLDEF: 
    //    return cellDefinition(t); 
    break;
  case '+': 
    EmitBinaryOperator(t,"Add");
    break;
  case '-': 
    EmitBinaryOperator(t,"Subtract");
    break;
  case '*': 
    EmitBinaryOperator(t,"Multiply");
    break;
  case '/': 
    EmitBinaryOperator(t,"RightDivide");
    break;
  case '\\': 
    EmitBinaryOperator(t,"LeftDivide");
    break;
  case TOK_SOR: 
  case '|': 
    throw Exception("Unimplemented");    
    break;
  case TOK_SAND: 
  case '&':  
    throw Exception("Unimplemented");    
    break;
  case '<': 
    EmitBinaryOperator(t,"LessThan");
    break;
  case TOK_LE: 
    EmitBinaryOperator(t,"LessEquals");
    break;
  case '>': 
    EmitBinaryOperator(t,"GreaterThan");
    break;
  case TOK_GE: 
    EmitBinaryOperator(t,"GreaterEquals");
    break;
  case TOK_EQ: 
    EmitBinaryOperator(t,"Equals");
    break;
  case TOK_NE: 
    EmitBinaryOperator(t,"NotEquals");
    break;
  case TOK_DOTTIMES: 
    EmitBinaryOperator(t,"DotMultiply");
    break;
  case TOK_DOTRDIV: 
    EmitBinaryOperator(t,"DotRightDivide");
    break;
  case TOK_DOTLDIV: 
    EmitBinaryOperator(t,"DotLeftDivide");
    break;
  case TOK_UNARY_MINUS: 
    EmitUnaryOperator(t,"Negate");
    break;
  case TOK_UNARY_PLUS: 
    EmitUnaryOperator(t,"Plus");
    break;
  case '~': 
    EmitUnaryOperator(t,"Not");
    break;
  case '^': 
    EmitBinaryOperator(t,"Power");
    break;
  case TOK_DOTPOWER: 
    EmitBinaryOperator(t,"DotPower");
    break;
  case '\'': 
    EmitUnaryOperator(t,"Transpose");
    break;
  case TOK_DOTTRANSPOSE: 
    EmitUnaryOperator(t,"DotTranspose");
    break;
  case '@':
    throw Exception("Unimplemented");    
  default:
    throw Exception("Unrecognized expression!");
  }
}

void EmitArgumentAliases(MFunctionDef *m_def) {
  localIdents.clear();
  EmitIndent();
  Emit("Array ");
  for (int i=0;i<m_def->arguments.size();i++) {
    Emit(QString::fromStdString(m_def->arguments[i]));
    if (i < m_def->arguments.size()-1)
      Emit(",");
  }
  Emit(";\n");
  for (int i=0;i<m_def->arguments.size();i++) {
    EmitIndent();
    Emit(QString("if (args.size() >= %1) %2 = args[%1];\n").arg(i).arg(QString::fromStdString(m_def->arguments[i])));
    localIdents.push_back(m_def->arguments[i]);
  }
  returnVals = m_def->returnVals;
}

// a(1,2).foo.goo{3} = rhs
//
// t1_ = a(1,2)
// t2_ = t1_.foo
// t3_ = t2_.goo
// t3_{3} = rhs
// 
//
//  t_ = a(1,2).foo.goo
//  t_{3} = rhs;
//  a(1,2).foo.goo = t_;
//
//

void EmitAssign(string name, const tree &s) {
  if (s.is(TOK_PARENS)) {
    Emit(QString("%1.setNDimSubset(").arg(QString::fromStdString(name)));
    Emit("ArrayVector()");
    for (int p=0;p<s.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(s.child(p));
    }
    Emit(",_rhs)");
  } else if (s.is(TOK_BRACES)) {
    Emit(QString("%1.setNDimContentsAsList(").arg(QString::fromStdString(name)));
    Emit("ArrayVector()");
    for (int p=0;p<s.numchildren();p++) {
      Emit(" << ");
      EmitMultiExpression(s.child(p));
    }
    Emit(",_rhs)");
  } else if (s.is('.')) {
    Emit(QString("%1.setFieldAsList(").arg(QString::fromStdString(name)));
    Emit(QString("%2,_rhs)").arg(QString::fromStdString(s.first().text())));
  } else if (s.is(TOK_DYN)) {
  }
}

void EmitAssignment(const tree &t, bool printIt) {
  const tree &var(t.first());
  const tree &rhs(t.second());
  string name(var.first().text());
  if (!IsVariableDefined(name)) {
    Emit(QString("Array %1;\n").arg(QString::fromStdString(name)));
    localIdents.push_back(name);
    EmitIndent();
  }
  if (var.numchildren() == 1) {
    Emit(QString("%1 = ").arg(QString::fromStdString(name)));
    EmitExpression(rhs);
    Emit(";\n");
    return;
  }
  if (var.numchildren() == 2) {
    Emit("{\n");
    indent_level++;
    EmitIndent();
    Emit(QString("Array _rhs("));
    EmitExpression(rhs);
    Emit(");\n");
    EmitIndent();
    EmitAssign(name,var.second());
    Emit(";\n");
    indent_level--;
    EmitIndent();
    Emit("}\n");
    return;
  }
}

void EmitBlock(const tree &code);

void EmitReturn() {
  Emit("return ArrayVector()");
  for (int i=0;i<returnVals.size();i++) {
    Emit(" << ");
    if (IsVariableDefined(returnVals[i]))
      Emit(QString::fromStdString(returnVals[i]));
    else
      Emit("Array::emptyConstructor()");
  }
  Emit(";\n");
}

void EmitForStatement(const tree &t) {
  QString loopvar = QString("_loop%1").arg(forDepth);
  QString indexvar = QString("_index%1").arg(forDepth);
  forDepth++;
  QString myloopvar = QString::fromStdString(t.first().first().text());
  Emit(QString("Array %1 = ").arg(indexvar));
  EmitExpression(t.first().second());
  Emit(";\n");
  EmitIndent();
  Emit(QString("for (int %1=1;%1<=%2.getLength();%1++) {\n").arg(loopvar).arg(indexvar));
  indent_level++;
  EmitIndent();
  Emit(QString("Array %1(%2.getVectorSubset(Array::int32Constructor(%3)));\n").arg(myloopvar).arg(indexvar).arg(loopvar));
  localIdents.push_back(t.first().first().text());
  EmitBlock(t.second());
  indent_level--;
  EmitIndent();
  Emit("}\n");
  forDepth--;
}

void EmitWhileStatement(const tree &t) {
  Emit("while (!(");
  EmitExpression(t.first());
  Emit(").isRealAllZeros())\n");
  EmitIndent();
  Emit("{\n");
  indent_level++;
  EmitBlock(t.second());
  indent_level--;
  EmitIndent();
  Emit("}\n");
}

void EmitIfStatement(const tree &t) {
  Emit("if (!(");
  EmitExpression(t.first());
  Emit(").isRealAllZeros())\n");
  EmitIndent();
  Emit("{\n");
  indent_level++;
  EmitBlock(t.second());
  indent_level--;
  unsigned n=2;
  while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
    EmitIndent();
    Emit("}\n");
    EmitIndent();
    Emit("else if (!(");
    EmitExpression(t.child(n).first());
    Emit(").isRealAllZeros()))\n");
    EmitIndent();
    Emit("{\n");
    indent_level++;
    EmitBlock(t.child(n).second());
    indent_level--;
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    EmitIndent();
    Emit("}\n");
    EmitIndent();
    Emit("else\n");
    EmitIndent();
    Emit("{\n");
    EmitIndent();
    EmitBlock(t.last().first());
    EmitIndent();
    Emit("}\n");
  } else {
    EmitIndent();
    Emit("}\n");
  }
}

void EmitStatementType(const tree &t, bool printIt) {
  switch(t.token()) {
  case '=': 
    EmitAssignment(t,printIt);
    break;
  case TOK_MULTI:
    //     multiFunctionCall(t,printIt);
    break;
  case TOK_SPECIAL:
    //     specialFunctionCall(t,printIt);
    break;
  case TOK_FOR:
    EmitForStatement(t);
    break;
  case TOK_WHILE:
    EmitWhileStatement(t);
    break;
  case TOK_IF:
    EmitIfStatement(t);
    break;
  case TOK_BREAK:
    Emit("break;\n");
    break;
  case TOK_CONTINUE:
    Emit("continue;\n");
    break;
  case TOK_DBSTEP:
    break;
  case TOK_DBTRACE:
    break;
  case TOK_RETURN:
    EmitReturn();
    break;
  case TOK_SWITCH:
    //    switchStatement(t);
    break;
  case TOK_TRY:
    //    tryStatement(t);
    break;
  case TOK_QUIT:
    break;
  case TOK_RETALL:
    break;
  case TOK_KEYBOARD:
    break;
  case TOK_GLOBAL:
    //    globalStatement(t);
    break;
  case TOK_PERSISTENT:
    //    persistentStatement(t);
    break;
  case TOK_EXPR:
    //    expressionStatement(t,printIt);
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void EmitStatement(const tree &t) {
  if (t.is(TOK_QSTATEMENT))
    EmitStatementType(t.first(),false);
  else if (t.is(TOK_STATEMENT))
    EmitStatementType(t.first(),true);
  Emit(";\n");
}

void EmitBlock(const tree &code) {
  for (int i=0;i<code.numchildren();i++) {
    EmitIndent();
    EmitStatement(code.child(i));
  }
}

void ProtoCompile(Interpreter *m_eval, MFunctionDef *m_def, string filename) {
  m_def->updateCode(m_eval);
  fp = new QFile(QString::fromStdString(filename));
  eval = m_eval;
  if (!fp->open(QIODevice::WriteOnly))
    throw Exception("Unable to open file " + filename + " for writing.");
  ts = new QTextStream(fp);
  EmitIndent();
  Emit(QString("ArrayVector %1Function(int nargout, const ArrayVector& args) {\n").arg(QString::fromStdString(m_def->name)));
  forDepth = 0;
  indent_level++;
  // Emit aliases for the arguments
  EmitArgumentAliases(m_def);
  EmitBlock(m_def->code);
  EmitIndent();
  EmitReturn();
  indent_level--;
  EmitIndent();
  Emit(QString("}\n"));
  fp->close();
  delete fp;
}

ArrayVector fccFunction(int nargout, const ArrayVector& args, Interpreter* m_eval) {
  for (int i=0;i<args.size();i++) {
    FuncPtr val;
    string name(ArrayToString(args[i]));
    if (!m_eval->lookupFunction(name,val))
      throw Exception("Unable to resolve " + name + " to a function");
    if (!(val->type() == FM_M_FUNCTION))
      throw Exception("Function " + name + " is not an M file (and cannot be compiled");
    ProtoCompile(m_eval,(MFunctionDef*) val,name+".cpp");
  }
  return ArrayVector();
}

static void VariableReferencesList(const tree & t, StringVector& idents) {
  if (t.is(TOK_VARIABLE)) {
    bool exists = false;
    for (int i=0;(i<idents.size());i++) {
      exists = (idents[i] == t.first().text());
      if (exists) break;
    }
    if (!exists)
      idents.push_back(t.first().text());
  }
  for (int i=0;i<t.numchildren();i++)
    VariableReferencesList(t.child(i),idents);
}

class LoopSignature {
  std::set<string> scalars;
  std::set<string> matrices;
public:
  std::set<string> get_scalars() {
    return scalars;
  }
  std::set<string> get_matrices() {
    return matrices;
  }
  void add_scalar(string scalar) {
    scalars.insert(scalar);
  }
  void add_matrix(string matrix) {
    matrices.insert(matrix);
  }
  void remove_scalar(string scalar) {
    scalars.erase(scalar);
  }
  void remove_matrix(string matrix) {
    matrices.erase(matrix);
  }
  void print() {
    for (std::set<string>::const_iterator i = scalars.begin();
	 i!=scalars.end();i++) {
      std::cout << "scalar " << *i << "\r\n";
    }
    for (std::set<string>::const_iterator i = matrices.begin();
	 i!=matrices.end();i++) {
      std::cout << "matrix " << *i << "\r\n";
    }
  }
};


static inline registerType RTUnsigned(unsigned x) {
  registerType ret;
  ret.u = x;
  return ret;
}

static inline registerType RTBoolean(bool x) {
  registerType ret;
  ret.b = x;
  return ret;
}

static inline registerType RTInteger(int x) {
  registerType ret;
  ret.i = x;
  return ret;
}

static inline registerType RTFloat(float x) {
  registerType ret;
  ret.f = x;
  return ret;  
}

static inline registerType RTDouble(double x) {
  registerType ret;
  ret.d = x;
  return ret;  
}

static inline registerType RTPointer(void* x) {
  registerType ret;
  ret.p = x;
  return ret;  
}

std::string RegisterName(opcodeClass t, registerType topval) {
  char buffer[512];
  sprintf(buffer,"unknown");
  switch(t) {
  case t_boolean: 
    sprintf(buffer,"%u",topval.b);
    break;
  case unsigned_integer:
    sprintf(buffer,"%u",topval.u);
    break;
  case integer:
    sprintf(buffer,"%d",topval.i);
    break;
  case single_float:
    sprintf(buffer,"%g",topval.f);
    break;
  case double_float:
    sprintf(buffer,"%g",topval.d);
    break;
  case pointer:
    sprintf(buffer,"%x",(unsigned)topval.p);
  }
  return buffer;
}

std::string OpCodeName(opcodeType topcode) {
  switch(topcode) {
  case ADD:   return "ADD  ";
  case SUB:   return "SUB  ";
  case SET:   return "SET  ";
  case NOP:   return "NOP  ";
  case LOAD:  return "LOAD ";
  case STORE: return "STORE";
  case MUL:   return "MUL  ";
  case RDIV:  return "RDIV ";
  case LDIV:  return "LDIV "; 
  case OR:    return "OR   ";
  case AND:   return "AND  ";
  case XOR:   return "XOR  ";
  case LT:    return "LT   ";
  case LE:    return "LE   ";
  case EQ:    return "EQ   ";
  case NEQ:   return "NEQ  ";
  case GE:    return "GE   ";
  case GT:    return "GT   ";
  case JIT:   return "JIT  ";
  case JIF:   return "JIF  ";
  case JMP:   return "JMP  ";
  case COPY:  return "COPY ";
  case CASTB: return "CASTB";
  case CASTI: return "CASTI";
  case CASTU: return "CASTU";
  case CASTF: return "CASTF";
  case CASTD: return "CASTD";
  case NEG:   return "NEG  ";
  case POS:   return "POS  ";
  case NOT:   return "NOT  ";
  case DEC:   return "DEC  ";
  case RET:   return "RET  ";
  }
}

std::string OpCodeClass(opcodeClass topclass) {
  switch(topclass) {
  case t_boolean: return "BOL";
  case unsigned_integer: return "USN";
  case integer: return "INT";
  case single_float: return "FLT";
  case double_float: return "DBL";
  case pointer: return "PNT";
  }
}

void VMInstruction::print(std::ostream& o) {
  if (opcode == NOP) {
    o << OpCodeName(opcode) << "\r\n";
  } else if (opcode == SET) {
    o << OpCodeName(opcode) << " <" << OpCodeClass(opclass) << "> \t";
    o << "r" << dest << "," << RegisterName(opclass,literal) << "\r\n";
  } else if ((opcode == JIT) || (opcode == JIF)) {
    o << OpCodeName(opcode) << "       \t";
    o << "r" << dest << "," << RegisterName(opclass,literal) << "\r\n";
  } else if (opcode == JMP) {
    o << OpCodeName(opcode) << "       \t";
    o << RegisterName(opclass,literal) << "\r\n";
  } else if (opcode == DEC) {
    o << OpCodeName(opcode) << "       \t";
    o << "r" << dest << "\r\n";
  } else if ((opcode == CASTI) || (opcode == CASTU) || 
	     (opcode == CASTF) || (opcode == CASTD) ||
	     (opcode == NEG) || (opcode == POS) || 
	     (opcode == NOT)) {
    o << OpCodeName(opcode) << " <" << OpCodeClass(opclass) << "> \t";
    o << "r" << dest << ",";
    o << "r" << arg1 << "\r\n";
  } else {
    o << OpCodeName(opcode) << " <" << OpCodeClass(opclass) << "> \t";
    o << "r" << dest << ",";
    o << "r" << arg1 << ",";
    o << "r" << arg2 << "\r\n";
  }
}

void VMStream::Run(Interpreter* m_eval) {
  // Fill out the arguments
  StringVector syms(get_symbol_names());
  for (int i=0;i<syms.size();i++) {
    SymbolInformation *psym = find_symbol(syms[i]);
    if (psym && psym->isArgument()) {
      ArrayReference ptr(m_eval->getContext()->lookupVariable(syms[i]));
      if (!ptr.valid())
	throw Exception("Undefined variable reference: " + syms[i]);
      if (psym->isScalar() && ptr->isScalar()) {
	switch (ptr->dataClass()) {
	case FM_FUNCPTR_ARRAY:
	case FM_CELL_ARRAY:
	case FM_STRUCT_ARRAY:
	case FM_UINT8:
	case FM_INT8:
	case FM_UINT16:
	case FM_INT16:
	case FM_UINT64:
	case FM_INT64:
	case FM_COMPLEX:
	case FM_DCOMPLEX:
	case FM_STRING:
	  throw Exception("Cannot JIT code because of type of :" + syms[i]);
	case FM_LOGICAL:
	  if (psym->m_type != t_boolean)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].b = ((const logical*)(ptr->getDataPointer()))[0];
	  break;
	case FM_UINT32:
	  if (psym->m_type != unsigned_integer)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].u = ((const uint32*)(ptr->getDataPointer()))[0];
	  break;
	case FM_INT32:
	  if (psym->m_type != integer)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].i = ((const int32*)(ptr->getDataPointer()))[0];
	  break;
	case FM_FLOAT:
	  if (psym->m_type != single_float)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].f = ((const float*)(ptr->getDataPointer()))[0];
	  break;
	case FM_DOUBLE:
	  if (psym->m_type != double_float)
	    throw Exception("Expected " + syms[i] + " to be a different type than it is");
	  reg[psym->m_base.index()].d = ((const double*)(ptr->getDataPointer()))[0];
	}
      } else {
	if (psym->isReadOnly()) {
	  reg[psym->m_base.index()].p = (void*) ptr->getDataPointer();
	} else {
	  reg[psym->m_base.index()].p = ptr->getReadWriteDataPointer();
	}
	reg[psym->m_rows.index()].u = ptr->rows();
	reg[psym->m_cols.index()].u = ptr->columns();
      }      
    }
  }
  
  bool term = false;
  unsigned ip = 0;
  unsigned ops_max = p_data.size();
  while (ip < ops_max) {
    VMInstruction *op = p_data[ip++];
    switch(op->opcode) {
    case ADD:
      AddOp(op);
      break;
    case SET:
      SetOp(op);
      break;
    case NOP:
      break;
    case SUB:
      SubOp(op);
      break;
    case MUL:
      MulOp(op);
      break;
    case RDIV:
      RDivOp(op);
      break;
    case LDIV:
      LDivOp(op);
      break;
    case LT:
      LTOp(op);
      break;
    case LE:
      LEOp(op);
      break;
    case EQ:
      EQOp(op);
      break;
    case NEQ:
      NEQOp(op);
      break;
    case GE:
      GEOp(op);
      break;
    case GT:
      GTOp(op);
      break;
    case OR:
      OrOp(op);
      break;
    case AND:
      AndOp(op);
      break;
    case XOR:
      XorOp(op);
      break;
    case LOAD:
      LoadOp(op);
      break;
    case STORE:
      StoreOp(op);
      break;
    case JIT:
      if (reg[op->dest].b)
	ip = op->literal.u;
      break;
    case JIF:
      if (!reg[op->dest].b)
	ip = op->literal.u;
      break;
    case COPY:
      CopyOp(op);
      break;
    case CASTB:
      CastBOp(op);
      break;
    case CASTU:
      CastUOp(op);
      break;
    case CASTI:
      CastIOp(op);
      break;
    case CASTF:
      CastFOp(op);
      break;
    case CASTD:
      CastDOp(op);
      break;
    case NEG:
      NegOp(op);
      break;
    case POS:
      break;
    case NOT:
      NotOp(op);
      break;
    case DEC:
      DecOp(op);
      break;
    case RET:
      return;
    }
  }
}

std::ostream& operator <<(std::ostream& o, VMStream& t) {
  StringVector syms(t.get_symbol_names());
  for (int i=0;i<syms.size();i++) {
    SymbolInformation *psym = t.find_symbol(syms[i]);
    if (psym && psym->isArgument()) {
      std::cout << "ARG: " << syms[i] << " : ";
      std::cout << "Base: " << psym->m_base.index() << "\r\n";
    }
  }
  for (int i=0;i<t.size();i++) {
    char buffer[1000];
    sprintf(buffer,"%03u: ",i);
    o << buffer;
    t[i]->print(o);
  }
  return o;
}

VMStream& operator <<(VMStream& o, VMInstruction* t) {
  o.push_back(t);
  return o;
}

void JITBlock(VMStream& o, tree t, Interpreter* m_eval);
RegisterReference JITExpression(VMStream& o, tree t, Interpreter* m_eval);

RegisterReference JITPromote(VMStream& o, RegisterReference a, opcodeClass c) {
  RegisterReference out(o.GetReg(),c);
  if (a.type() == t_boolean) {
    switch(c) {
    case t_boolean:
      return a;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,t_boolean,out.index(),a.index());
      return out;
    case integer:
      o << new VMInstruction(CASTI,t_boolean,out.index(),a.index());
      return a;
    case single_float:
      o << new VMInstruction(CASTF,t_boolean,out.index(),a.index());
      return out;
    case double_float:
      o << new VMInstruction(CASTD,t_boolean,out.index(),a.index());
      return out;
    }
  }
  if (a.type() == integer) {
    switch(c) {
    case t_boolean:
      o << new VMInstruction(CASTB,integer,out.index(),a.index());
      return out;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,integer,out.index(),a.index());
      return out;
    case integer:
      return a;
    case single_float:
      o << new VMInstruction(CASTF,integer,out.index(),a.index());
      return out;
    case double_float:
      o << new VMInstruction(CASTD,integer,out.index(),a.index());
      return out;
    }
  } else if (a.type() == unsigned_integer) {
    switch(c) {
    case t_boolean:
      o << new VMInstruction(CASTB,unsigned_integer,out.index(),a.index());
      return out;      
    case unsigned_integer:
      return a;
    case integer:
      o << new VMInstruction(CASTI,unsigned_integer,out.index(),a.index());
      return out;
    case single_float:
      o << new VMInstruction(CASTF,unsigned_integer,out.index(),a.index());
      return out;
    case double_float:
      o << new VMInstruction(CASTD,unsigned_integer,out.index(),a.index());
      return out;
    }    
  } else if (a.type() == single_float) {
    switch(c) {
    case t_boolean:
      o << new VMInstruction(CASTB,single_float,out.index(),a.index());
      return out;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,single_float,out.index(),a.index());
      return out;
    case integer:
      o << new VMInstruction(CASTI,single_float,out.index(),a.index());
      return out;
    case single_float:
      return a;
    case double_float:
      o << new VMInstruction(CASTD,single_float,out.index(),a.index());
      return out;
    }        
  } else if (a.type() == double_float) {
    switch(c) {
    case t_boolean:
      o << new VMInstruction(CASTB,double_float,out.index(),a.index());
      return out;      
    case unsigned_integer:
      o << new VMInstruction(CASTU,double_float,out.index(),a.index());
      return out;
    case integer:
      o << new VMInstruction(CASTI,double_float,out.index(),a.index());
      return out;
    case single_float:
      o << new VMInstruction(CASTF,double_float,out.index(),a.index());
      return out;
    case double_float:
      return a;
    }            
  }
  throw Exception("Illegal type promotion call.");
}

RegisterReference JITBooleanOrOperator(VMStream& o, tree t, Interpreter* m_eval) {
  RegisterReference result(o.GetReg(),t_boolean);
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference atest(JITPromote(o,a,t_boolean));
  VMInstruction *jump_a_true = new VMInstruction(JIT,unsigned_integer,
						 atest.index(),
						 RTUnsigned(0));
  o << jump_a_true;
  RegisterReference b(JITExpression(o,t.second(),m_eval));
  RegisterReference btest(JITPromote(o,b,t_boolean));
  VMInstruction *jump_b_true = new VMInstruction(JIT,unsigned_integer,
						 btest.index(),
						 RTUnsigned(0));
  o << jump_b_true;
  o << new VMInstruction(SET,t_boolean,result.index(),RTBoolean(false));
  VMInstruction *jump_done = new VMInstruction(JMP,unsigned_integer,
					       RTUnsigned(o.size()+2));
  o << jump_done;
  jump_a_true->literal = RTUnsigned(o.size());
  jump_b_true->literal = RTUnsigned(o.size());
  o << new VMInstruction(SET,t_boolean,result.index(),RTBoolean(true));
  o << new VMInstruction(NOP);
  return result;
}

RegisterReference JITBooleanNotOperator(VMStream& o, tree t, Interpreter* m_eval) {
  RegisterReference result(o.GetReg(),t_boolean);
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference atest(JITPromote(o,a,t_boolean));
  o << new VMInstruction(NOT,t_boolean,result.index(),atest.index());
  return result;
}

RegisterReference JITBooleanAndOperator(VMStream& o, tree t, Interpreter* m_eval) {
  RegisterReference result(o.GetReg(),t_boolean);
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference atest(JITPromote(o,a,t_boolean));
  VMInstruction *jump_a_false = new VMInstruction(JIF,unsigned_integer,
						  atest.index(),
						  RTUnsigned(0));
  o << jump_a_false;
  RegisterReference b(JITExpression(o,t.second(),m_eval));
  RegisterReference btest(JITPromote(o,b,t_boolean));
  VMInstruction *jump_b_false = new VMInstruction(JIF,unsigned_integer,
						  btest.index(),
						  RTUnsigned(0));
  o << jump_b_false;
  o << new VMInstruction(SET,t_boolean,result.index(),RTBoolean(true));
  VMInstruction *jump_done = new VMInstruction(JMP,unsigned_integer,
					       RTUnsigned(o.size()+2));
  o << jump_done;
  jump_a_false->literal = RTUnsigned(o.size());
  jump_b_false->literal = RTUnsigned(o.size());
  o << new VMInstruction(SET,t_boolean,result.index(),RTBoolean(false));
  o << new VMInstruction(NOP);
  return result;
}

RegisterReference JITComparisonOperator(VMStream& o, tree t, opcodeType op, Interpreter* m_eval) {
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference b(JITExpression(o,t.second(),m_eval));
  opcodeClass outputClass;
  if (a.type() > b.type()) 
    outputClass = a.type();
  else
    outputClass = b.type();
  if (a.type() != outputClass)
    a = JITPromote(o,a,outputClass);
  if (b.type() != outputClass)
    b = JITPromote(o,b,outputClass);
  RegisterReference c(o.GetReg(),t_boolean);
  o << new VMInstruction(op,t_boolean,c.index(),a.index(),b.index());
  return c;
}

RegisterReference JITUnaryOperator(VMStream& o, tree t, opcodeType op, Interpreter* m_eval) {
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference c(o.GetReg(),a.type());
  o << new VMInstruction(op,c.type(),c.index(),a.index());
  return c;
}

RegisterReference JITBinaryOperator(VMStream& o, tree t, opcodeType op, Interpreter* m_eval) {
  RegisterReference a(JITExpression(o,t.first(),m_eval));
  RegisterReference b(JITExpression(o,t.second(),m_eval));
  opcodeClass outputClass;
  if (a.type() > b.type()) 
    outputClass = a.type();
  else
    outputClass = b.type();
  if ((op == LDIV) || (op == RDIV) && (outputClass == integer))
    outputClass = double_float;
  if (a.type() != outputClass)
    a = JITPromote(o,a,outputClass);
  if (b.type() != outputClass)
    b = JITPromote(o,b,outputClass);
  RegisterReference c(o.GetReg(),outputClass);
  o << new VMInstruction(op,c.type(),c.index(),a.index(),b.index());
  return c;
}

SymbolInformation* JITAddArgument(VMStream& o, string name, Interpreter*m_eval, bool scalarVariable) {
  ArrayReference ptr(m_eval->getContext()->lookupVariable(name));
  if (!ptr.valid())
    throw Exception("Undefined variable reference:" + name);
  if (!ptr->is2D())
    throw Exception("Cannot JIT multi-dimensional array:" + name);
  if (ptr->isString() || ptr->isReferenceType())
    throw Exception("Cannot JIT strings or reference types:" + name);
  if (ptr->isComplex())
    throw Exception("Cannot JIT complex arrays:" + name);
  if (!ptr->isScalar() && scalarVariable)
    throw Exception("JIT requires " + name + " be a scalar");
  opcodeClass q_type;
  switch (ptr->dataClass()) {
  case FM_FUNCPTR_ARRAY:
  case FM_CELL_ARRAY:
  case FM_STRUCT_ARRAY:
  case FM_UINT8:
  case FM_INT8:
  case FM_UINT16:
  case FM_INT16:
  case FM_UINT64:
  case FM_INT64:
  case FM_COMPLEX:
  case FM_DCOMPLEX:
  case FM_STRING:
    throw Exception("Cannot JIT code because of type of :" + name);
  case FM_LOGICAL:
    q_type = t_boolean;
    break;
  case FM_UINT32:
    q_type = unsigned_integer;
    break;
  case FM_INT32:
    q_type = integer;
    break;
  case FM_FLOAT:
    q_type = single_float;
    break;
  case FM_DOUBLE:
    q_type = double_float;
  }
  // Allocate a symbol for the variable
  SymbolInformation sym;
  sym.m_rows = RegisterReference(o.GetReg(),unsigned_integer);
  sym.m_cols = RegisterReference(o.GetReg(),unsigned_integer);
  sym.m_type = q_type;
  if (!scalarVariable)
    sym.m_base = RegisterReference(o.GetReg(),pointer);
  else
    sym.m_base = RegisterReference(o.GetReg(),q_type);
  sym.m_argument = true;
  sym.m_scalar = scalarVariable;
  sym.m_readonly = true;
  o.add_symbol(name,sym);
  return o.find_symbol(name);
}

RegisterReference JITRHS(VMStream& o, tree t, Interpreter* m_eval) { 
  string symname(t.first().text());
  SymbolInformation *v = o.find_symbol(symname);
  if (!v) 
    // Try to look up the symbol in the current context
    v = JITAddArgument(o,symname,m_eval,t.numchildren() == 1);
  if (t.numchildren() == 1) {
    if (!v->isScalar()) 
      throw Exception("Non-scalar reference returned in scalar context!");
    return v->m_base;
  }
  if (t.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isScalar())
    throw Exception("array indexing of scalar values...");
  tree s(t.second());
  if (!s.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (s.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (s.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (s.numchildren() == 1) {
    RegisterReference offset_n(JITExpression(o,s.first(),m_eval));
    RegisterReference offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,offset_n.type(),offset_u.index(),offset_n.index());
    o << new VMInstruction(DEC,offset_u.type(),offset_u.index());
    RegisterReference val(o.GetReg(),v->m_type);
    o << new VMInstruction(LOAD,v->m_type,val.index(),v->m_base.index(),offset_u.index());
    return val;
  } else if (s.numchildren() == 2) {
    RegisterReference row_offset_n(JITExpression(o,s.first(),m_eval));
    RegisterReference row_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,row_offset_n.type(),row_offset_u.index(),row_offset_n.index());
    o << new VMInstruction(DEC,row_offset_u.type(),row_offset_u.index());
    RegisterReference col_offset_n(JITExpression(o,s.second(),m_eval));
    RegisterReference col_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,col_offset_n.type(),col_offset_u.index(),col_offset_n.index());
    o << new VMInstruction(DEC,col_offset_u.type(),col_offset_u.index());    
    RegisterReference ndx_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(MUL,unsigned_integer,ndx_offset_u.index(),
			   col_offset_u.index(),v->m_rows.index());
    o << new VMInstruction(ADD,unsigned_integer,ndx_offset_u.index(),
			   ndx_offset_u.index(),row_offset_u.index());
    RegisterReference val(o.GetReg(),v->m_type);
    o << new VMInstruction(LOAD,v->m_type,val.index(),v->m_base.index(),ndx_offset_u.index());
    return val;
  }
  throw Exception("dereference not handled yet...");
}

RegisterReference JITExpression(VMStream& o, tree t, Interpreter* m_eval) {
  switch(t.token()) {
  case TOK_VARIABLE: 
    return JITRHS(o,t,m_eval);
  case TOK_INTEGER: {
    RegisterReference q(o.GetReg(),integer);
    o << new VMInstruction(SET,integer,q.index(),
			   RTInteger(ArrayToInt32(t.array())));
    return q;
  }
  case TOK_FLOAT: {
    RegisterReference q(o.GetReg(),single_float);
    o << new VMInstruction(SET,single_float,q.index(),
			   RTFloat(ArrayToDouble(t.array())));
    return q;
  }
  case TOK_DOUBLE: {
    RegisterReference q(o.GetReg(),double_float);
    o << new VMInstruction(SET,double_float,q.index(),
			   RTDouble(ArrayToDouble(t.array())));
    return q;
  }
  case TOK_COMPLEX:
  case TOK_DCOMPLEX:
  case TOK_STRING:
  case TOK_END:
  case ':':
  case TOK_MATDEF: 
  case TOK_CELLDEF: 
    throw Exception("JIT compiler does not support complex, string, END, matrix or cell defs");
  case '+': 
    return JITBinaryOperator(o,t,ADD,m_eval);
  case '-': 
    return JITBinaryOperator(o,t,SUB,m_eval);
  case '*': 
  case TOK_DOTTIMES: 
    return JITBinaryOperator(o,t,MUL,m_eval);
  case '/': 
  case TOK_DOTRDIV: 
    return JITBinaryOperator(o,t,RDIV,m_eval);
  case '\\': 
  case TOK_DOTLDIV: 
    return JITBinaryOperator(o,t,LDIV,m_eval);
  case TOK_SOR: 
  case '|': 
    return JITBooleanOrOperator(o,t,m_eval);
  case TOK_SAND: 
  case '&': 
    return JITBooleanAndOperator(o,t,m_eval);
  case '<': 
    return JITComparisonOperator(o,t,LT,m_eval);
  case TOK_LE: 
    return JITComparisonOperator(o,t,LE,m_eval);
  case '>': 
    return JITComparisonOperator(o,t,GT,m_eval);
  case TOK_GE: 
    return JITComparisonOperator(o,t,GE,m_eval);
  case TOK_EQ: 
    return JITComparisonOperator(o,t,EQ,m_eval);
  case TOK_NE: 
    return JITComparisonOperator(o,t,NEQ,m_eval);
  case TOK_UNARY_MINUS: 
    return JITUnaryOperator(o,t,NEG,m_eval);
  case TOK_UNARY_PLUS: 
    return JITUnaryOperator(o,t,POS,m_eval);
  case '~': 
    return JITBooleanNotOperator(o,t,m_eval);
    break;
  case '^': 
    throw Exception("^ is not currently handled by the JIT compiler");
    break;
  case TOK_DOTPOWER: 
    throw Exception(".^ is not currently handled by the JIT compiler");
    break;
  case '\'': 
    throw Exception("' is not currently handled by the JIT compiler");
    break;
  case TOK_DOTTRANSPOSE: 
    throw Exception(".' is not currently handled by the JIT compiler");
    break;
  case '@':
    throw Exception("@ is not currently handled by the JIT compiler");
  default:
    std::cout << "******************************************************\r\n";
    t.print();
    throw Exception("Unrecognized expression!");
  }
}

void JITBlock(VMStream& o, tree t, Interpreter* m_eval);

void JITIfStatement(VMStream& o, tree t, Interpreter* m_eval) {
  VMStream endInstructions;
  RegisterReference test(JITExpression(o,t.first(),m_eval));
  VMInstruction *jmp1 = new VMInstruction(JIF,unsigned_integer,
					  test.index(),RTUnsigned(0));
  o << jmp1;
  VMInstruction *prev_fixup = jmp1;
  JITBlock(o,t.second(),m_eval);
  VMInstruction *jmp2 = new VMInstruction(JMP,unsigned_integer,
					  RTUnsigned(0));
  o << jmp2;
  endInstructions << jmp2;
  unsigned n=2;
  while (n < t.numchildren() && t.child(n).is(TOK_ELSEIF)) {
    prev_fixup->literal = RTUnsigned(o.size());
    RegisterReference ttest(JITExpression(o,t.child(n).first(),m_eval));
    VMInstruction *jmpn = new VMInstruction(JIF,unsigned_integer,
					    ttest.index(),RTUnsigned(0));
    o << jmpn;
    prev_fixup = jmpn;
    JITBlock(o,t.child(n).second(),m_eval);
    VMInstruction *jmpp = new VMInstruction(JMP,unsigned_integer,
					    RTUnsigned(0));
    o << jmpp;
    endInstructions << jmpp;
    n++;
  }
  if (t.last().is(TOK_ELSE)) {
    prev_fixup->literal = RTUnsigned(o.size());
    JITBlock(o,t.last().first(),m_eval);
  }
  int end_address = o.size();
  for (int i=0;i<endInstructions.size();i++)
    endInstructions[i]->literal = RTUnsigned(end_address);
  o << new VMInstruction(NOP);
}

// what happens with:
//   if (b>0)
//     a = 3.1;
//   else
//     a = 5;
//   end
// ??   When we come out of the if statement, what type is a  ??

void JITAssignment(VMStream& o, tree t, Interpreter* m_eval) {
  tree s(t.first());
  string symname(s.first().text());
  SymbolInformation *v = o.find_symbol(symname);
  if (!v) 
    v = JITAddArgument(o,symname,m_eval,s.numchildren() == 1);
  v->m_readonly = false;
  // evaluate the RHS of the assignment
  RegisterReference RHS(JITExpression(o,t.second(),m_eval));
  if (v->m_type != RHS.type())
    throw Exception("polymorphic assignment to scalar detected.");
  if (s.numchildren() == 1) {
    if (!v->isScalar())
      throw Exception("scalar assignment to array variable.");
    o << new VMInstruction(COPY,RHS.type(),v->m_base.index(),RHS.index());
  }
  if (s.numchildren() > 2)
    throw Exception("multiple levels of dereference not handled yet...");
  if (v->isScalar())
    throw Exception("array indexing of scalar values...");
  tree q(s.second());
  if (!q.is(TOK_PARENS))
    throw Exception("non parenthetical dereferences not handled yet...");
  if (q.numchildren() == 0)
    throw Exception("Expecting at least 1 array reference for dereference...");
  if (q.numchildren() > 2)
    throw Exception("Expecting at most 2 array references for dereference...");
  if (q.numchildren() == 1) {
    RegisterReference offset_n(JITExpression(o,q.first(),m_eval));
    RegisterReference offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,offset_n.type(),offset_u.index(),offset_n.index());
    o << new VMInstruction(DEC,offset_u.type(),offset_u.index());
    o << new VMInstruction(STORE,v->m_type,v->m_base.index(),RHS.index(),offset_u.index());
  } else if (s.numchildren() == 2) {
    RegisterReference row_offset_n(JITExpression(o,q.first(),m_eval));
    RegisterReference row_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,row_offset_n.type(),row_offset_u.index(),row_offset_n.index());
    o << new VMInstruction(DEC,row_offset_u.type(),row_offset_u.index());
    RegisterReference col_offset_n(JITExpression(o,q.second(),m_eval));
    RegisterReference col_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(CASTU,col_offset_n.type(),col_offset_u.index(),col_offset_n.index());
    o << new VMInstruction(DEC,col_offset_u.type(),col_offset_u.index());    
    RegisterReference ndx_offset_u(o.GetReg(),unsigned_integer);
    o << new VMInstruction(MUL,unsigned_integer,ndx_offset_u.index(),
			   col_offset_u.index(),v->m_rows.index());
    o << new VMInstruction(ADD,unsigned_integer,ndx_offset_u.index(),
			   ndx_offset_u.index(),row_offset_u.index());
    o << new VMInstruction(STORE,v->m_type,v->m_base.index(),RHS.index(),ndx_offset_u.index());
  }
}

void JITForLoop(VMStream& o, tree t, Interpreter* m_eval);

void JITStatementType(VMStream& o, tree t, Interpreter* m_eval) {
  switch(t.token()) {
  case '=': 
    JITAssignment(o,t,m_eval);
    break;
  case TOK_MULTI:
    throw Exception("multi function calls do not JIT compile");
  case TOK_SPECIAL:
    throw Exception("special function calls do not JIT compile");
  case TOK_FOR:
    JITForLoop(o,t,m_eval);
    break;
  case TOK_WHILE:
    throw Exception("nested while loops do not JIT compile");
  case TOK_IF:
    JITIfStatement(o,t,m_eval);
    break;
  case TOK_BREAK:
    throw Exception("break is not currently handled by the JIT compiler");
    break;
  case TOK_CONTINUE:
    throw Exception("continue is not currently handled by the JIT compiler");
    break;
  case TOK_DBSTEP:
    throw Exception("dbstep is not currently handled by the JIT compiler");
    break;
  case TOK_DBTRACE:
    throw Exception("dbtrace is not currently handled by the JIT compiler");
    break;
  case TOK_RETURN:
    throw Exception("return is not currently handled by the JIT compiler");
    break;
  case TOK_SWITCH:
    throw Exception("switch is not currently handled by the JIT compiler");
    break;
  case TOK_TRY:
    throw Exception("try is not currently handled by the JIT compiler");
    break;
  case TOK_QUIT:
    throw Exception("quit is not currently handled by the JIT compiler");
    break;
  case TOK_RETALL:
    throw Exception("retall is not currently handled by the JIT compiler");
    break;
  case TOK_KEYBOARD:
    throw Exception("keyboard is not currently handled by the JIT compiler");
    break;
  case TOK_GLOBAL:
    throw Exception("global is not currently handled by the JIT compiler");
    break;
  case TOK_PERSISTENT:
    throw Exception("persistent is not currently handled by the JIT compiler");
    break;
  case TOK_EXPR:
    JITExpression(o,t.first(),m_eval);
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}

void JITStatement(VMStream& o, tree t, Interpreter* m_eval) {
  // ignore tok_qstatement/tok_statement
  JITStatementType(o,t.first(),m_eval);
}

void JITBlock(VMStream& o, tree t, Interpreter* m_eval) {
  const treeVector &statements(t.children());
  for (treeVector::const_iterator i=statements.begin();
       i!=statements.end();i++) 
    JITStatement(o,*i,m_eval);
}

void JITForLoop(VMStream& o, tree t, Interpreter* m_eval) {
  if (!(t.first().is('=') && t.first().second().is(':') &&
	t.first().second().first().is(TOK_INTEGER) &&
	t.first().second().second().is(TOK_INTEGER))) 
    throw Exception("For loop cannot be compiled - need integer bounds");
  int loop_start(atoi(t.first().second().first().text().c_str()));
  int loop_stop(atoi(t.first().second().second().text().c_str()));
  string loop_index(t.first().first().text());
  unsigned loop_index_register = o.GetReg();
  o.add_symbol(loop_index,
		     SymbolInformation(integer,loop_index_register));
  o << new VMInstruction(SET,integer,loop_index_register,
			       RTInteger(loop_start));
  unsigned loop_max_register = o.GetReg();
  o << new VMInstruction(SET,integer,loop_max_register,
			       RTInteger(loop_stop));
  unsigned loop_increment_register = o.GetReg();
  o << new VMInstruction(SET,integer,loop_increment_register,
			       RTInteger(1));
  unsigned loop_test_register = o.GetReg();
  o << new VMInstruction(SET,t_boolean,loop_test_register,RTBoolean(false));
  unsigned loop_start_instruction = o.size();
  JITBlock(o,t.second(),m_eval);
  o << new VMInstruction(ADD,integer,loop_index_register,
			       loop_index_register,loop_increment_register);
  o << new VMInstruction(LE,integer,loop_test_register,
			       loop_index_register,loop_max_register);
  o << new VMInstruction(JIT,unsigned_integer,
			       loop_test_register,
			       RTUnsigned(loop_start_instruction));
}

bool ScalarRequirements(tree t, LoopSignature &sig) {
  if (t.is(TOK_VARIABLE)) {
    if (t.numchildren() > 2) return false;
    if (t.numchildren() == 1) {
      sig.add_scalar(t.first().text());
    } else {
      if (!t.second().is(TOK_PARENS)) 
	return false;
      else {
	tree s(t.second());
	for (int i=0;i<s.numchildren();i++)
	  if (s.child(i).is(':'))
	    return false;
      }
      sig.add_matrix(t.first().text());
      if (!ScalarRequirements(t.second(),sig))
	return false;
    }
  } else {
    for (int i=0;i<t.numchildren();i++) 
      if (!ScalarRequirements(t.child(i),sig))
	return false;
  }
  return true;
}

VMStream CompileForBlock(const tree t, Interpreter* m_eval) {
  VMStream o;
  JITForLoop(o,t,m_eval);
  return o;
}

void LoadCompileFunction(Context* context) {
  context->addSpecialFunction("fcc",fccFunction,-1,0); 
}
