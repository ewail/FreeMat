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
#ifndef __Compiler_hpp__
#define __Compiler_hpp__

#include "Interpreter.hpp"
#include "FunctionDef.hpp"
#include <string>

using namespace std;

typedef union {
  bool b;
  unsigned u;
  int i;
  float f;
  double d;
  void* p;
} registerType;

typedef enum {
  t_boolean,
  unsigned_integer,
  integer,
  single_float,
  double_float,
  pointer
} opcodeClass;

typedef enum {
  ADD,
  SUB,
  SET,
  NOP,
  LOAD,
  STORE,
  MUL,
  LDIV,
  RDIV,
  OR,
  AND,
  XOR,
  LT,
  LE,
  EQ,
  NEQ,
  GE,
  GT,
  JIT,
  JIF,
  JMP,
  CASTI,
  CASTU,
  CASTF,
  CASTD,
  CASTB,
  COPY,
  NEG,
  POS,
  NOT,
  DEC,
  RET
} opcodeType;

class VMInstruction {
public:
  opcodeType opcode;
  opcodeClass opclass;
  unsigned arg1;
  unsigned arg2;
  unsigned dest;
  registerType literal;
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest,
		unsigned targ1,
		unsigned targ2,
		registerType tlit) : opcode(topcode), opclass(topclass),
				     arg1(targ1), arg2(targ2), dest(tdest),
				     literal(tlit)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest,
		unsigned targ1,
		unsigned targ2) : opcode(topcode), opclass(topclass),
				  arg1(targ1), arg2(targ2), dest(tdest)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest,
		unsigned targ1) : opcode(topcode), opclass(topclass),
				  arg1(targ1), dest(tdest)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest,
		registerType tlit) : opcode(topcode), opclass(topclass),
				     dest(tdest), literal(tlit)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		unsigned tdest) : opcode(topcode), opclass(topclass),
				     dest(tdest)
  {}
  VMInstruction(opcodeType topcode,
		registerType tlit) : opcode(topcode), literal(tlit)
  {}
  VMInstruction(opcodeType topcode,
		opcodeClass topclass,
		registerType tlit) : opcode(topcode), opclass(topclass),
				     literal(tlit)
  {}
  VMInstruction(opcodeType topcode) : opcode(topcode)
  {}
  void print(std::ostream& o);
};

class RegisterReference {
  unsigned m_index;
  opcodeClass m_type;
public:
  RegisterReference(unsigned t, opcodeClass s) : m_index(t), m_type(s) {}
  RegisterReference() {}
  inline unsigned index() {return m_index;}
  inline opcodeClass type() {return m_type;}
};

class SymbolInformation {
public:
  RegisterReference m_rows;
  RegisterReference m_cols;
  opcodeClass m_type;
  RegisterReference m_base;
  bool m_scalar;
  bool m_argument;
  bool m_readonly;
  SymbolInformation() {}
  SymbolInformation(opcodeClass type, unsigned index) :
    m_type(type), m_base(index,type), m_scalar(true), m_argument(false) {}
  bool isScalar() {return m_scalar;}
  bool isArgument() {return m_argument;}
  bool isReadOnly() {return m_readonly;}
};

class VMStream {
  std::vector<VMInstruction*> p_data;
  SymbolTable<SymbolInformation> p_symbols;
  unsigned reglist;
  registerType reg[4096];
protected:
  inline void SetOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].b = op->literal.b;
      break;
    case unsigned_integer:
      reg[op->dest].u = op->literal.u;
      break;
    case integer:
      reg[op->dest].i = op->literal.i;
      break;
    case single_float:
      reg[op->dest].f = op->literal.f;
      break;
    case double_float:
      reg[op->dest].d = op->literal.d;
      break;
    case pointer:
      reg[op->dest].p = op->literal.p;
    }
  }
  inline void AddOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u + reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i + reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f + reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d + reg[op->arg2].d;
      break;
    }
  }
  inline void SubOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u - reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i - reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f - reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d - reg[op->arg2].d;
      break;
    }
  }
  inline void MulOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u * reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i * reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f * reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d * reg[op->arg2].d;
      break;
    }
  }
  inline void RDivOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u / reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i / reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f / reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d / reg[op->arg2].d;
      break;
    }
  }
  inline void LDivOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg2].u / reg[op->arg1].u;	
      break;
    case integer:
      reg[op->dest].i = reg[op->arg2].i / reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg2].f / reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg2].d / reg[op->arg1].d;
      break;
    }
  }
  inline void LTOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u < reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i < reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f < reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d < reg[op->arg2].d;
      break;
    }
  }
  inline void LEOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u <= reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i <= reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f <= reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d <= reg[op->arg2].d;
      break;
    }
  }
  inline void EQOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u == reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i == reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f == reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d == reg[op->arg2].d;
      break;
    }
  }
  inline void NEQOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u != reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i != reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f != reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d != reg[op->arg2].d;
      break;
    }
  }
  inline void GTOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u > reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i > reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f > reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d > reg[op->arg2].d;
      break;
    }
  }
  inline void CopyOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].b = reg[op->arg1].b;
      break;
    case unsigned_integer:
      reg[op->dest].u = reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].i = reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].f = reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].d = reg[op->arg1].d;
      break;
    }
  }
  inline void CastBOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].b = reg[op->arg1].b;
      break;
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u != 0;
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i != 0;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f != 0;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d != 0;
      break;
    }
  }
  inline void CastUOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].u = reg[op->arg1].b ? 1 : 0;
      break;
    case unsigned_integer:
      reg[op->dest].u = (unsigned) reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].u = (unsigned) reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].u = (unsigned) reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].u = (unsigned) reg[op->arg1].d;
      break;
    }
  }
  inline void CastIOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].i = reg[op->arg1].b ? 1 : 0;
      break;
    case unsigned_integer:
      reg[op->dest].i = (int) reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].i = (int) reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].i = (int) reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].i = (int) reg[op->arg1].d;
      break;
    }
  }
  inline void CastFOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].f = reg[op->arg1].b ? 1 : 0;
      break;
    case unsigned_integer:
      reg[op->dest].f = (float) reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].f = (float) reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].f = (float) reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].f = (float) reg[op->arg1].d;
      break;
    }
  }
  inline void DecOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      throw Exception("Decrement not supported for logical variables");
      break;
    case unsigned_integer:
      reg[op->dest].u--;
      break;
    case integer:
      reg[op->dest].i--;
      break;
    case single_float:
      reg[op->dest].f--;
      break;
    case double_float:
      reg[op->dest].d--;
      break;
    }
  }
  inline void CastDOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].d = reg[op->arg1].b ? 1 : 0;
      break;
    case unsigned_integer:
      reg[op->dest].d = (double) reg[op->arg1].u;
      break;
    case integer:
      reg[op->dest].d = (double) reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].d = (double) reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].d = (double) reg[op->arg1].d;
      break;
    }
  }
  inline void NegOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      throw Exception("Neg not supported for boolean arguments");
      break;
    case unsigned_integer:
      throw Exception("Neg not supported for unsigned arguments");
      break;
    case integer:
      reg[op->dest].i = -reg[op->arg1].i;
      break;
    case single_float:
      reg[op->dest].f = -reg[op->arg1].f;
      break;
    case double_float:
      reg[op->dest].d = -reg[op->arg1].d;
      break;
    }
  }
  inline void NotOp(VMInstruction *op) {
    switch (op->opclass) {
    case t_boolean:
      reg[op->dest].b = !reg[op->arg1].b;
      return;
    }
    throw Exception("NOT not supported for non-boolean arguments");
  }
  inline void GEOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].b = reg[op->arg1].u >= reg[op->arg2].u;	
      break;
    case integer:
      reg[op->dest].b = reg[op->arg1].i >= reg[op->arg2].i;
      break;
    case single_float:
      reg[op->dest].b = reg[op->arg1].f >= reg[op->arg2].f;
      break;
    case double_float:
      reg[op->dest].b = reg[op->arg1].d >= reg[op->arg2].d;
      break;
    }
  }
  inline void OrOp(VMInstruction *op) {
    reg[op->dest].b = reg[op->arg1].b | reg[op->arg2].b;
  }
  inline void AndOp(VMInstruction *op) {
    reg[op->dest].b = reg[op->arg1].b & reg[op->arg2].b;
  }
  inline void XorOp(VMInstruction *op) {
    reg[op->dest].b = reg[op->arg1].b ^ reg[op->arg2].b;
  }
  inline void LoadOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      reg[op->dest].u = ((unsigned*) reg[op->arg1].p)[reg[op->arg2].u];
      break;
    case integer:
      reg[op->dest].i = ((int*) reg[op->arg1].p)[reg[op->arg2].u];
      break;
    case single_float:
      reg[op->dest].f = ((float*) reg[op->arg1].p)[reg[op->arg2].u];
      break;
    case double_float:
      reg[op->dest].d = ((double*) reg[op->arg1].p)[reg[op->arg2].u];
      break;
    }
  }
  inline void StoreOp(VMInstruction *op) {
    switch (op->opclass) {
    case unsigned_integer:
      ((unsigned*) reg[op->dest].p)[reg[op->arg2].u] = reg[op->arg1].u;
      break;
    case integer:
      ((int*) reg[op->dest].p)[reg[op->arg2].u] = reg[op->arg1].i;
      break;
    case single_float:
      ((float*) reg[op->dest].p)[reg[op->arg2].u] = reg[op->arg1].f;
      break;
    case double_float:
      ((double*) reg[op->dest].p)[reg[op->arg2].u] = reg[op->arg1].d;
      break;
    }
  }
public:
  VMStream() : reglist(0) {}
  inline void push_back(VMInstruction *t) {p_data.push_back(t);}
  inline unsigned size() {return p_data.size();}
  VMInstruction* operator[](int i) {return p_data[i];}
  SymbolInformation* find_symbol(string name) {return p_symbols.findSymbol(name);}
  void add_symbol(const string &key, const SymbolInformation &val) {p_symbols.insertSymbol(key,val);}
  StringVector get_symbol_names() {return p_symbols.getCompletions("");}
  void Run(Interpreter* m_eval);
  inline unsigned GetReg() { return reglist++; }
};

VMStream CompileForBlock(const tree t, Interpreter* m_eval);
std::ostream& operator <<(std::ostream& o, VMStream& t);
#endif
