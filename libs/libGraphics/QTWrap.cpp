#include "QTWrap.hpp"
#include <QtGui>
#include "Array.hpp"
#include "Struct.hpp"
#include "Interpreter.hpp"
#include "Class.hpp"


Array MakeQTPointer(QObject *p, Interpreter *eval)
{
  if (!p) throw Exception("Function returned an empty Qt pointer!");
  StructArray *sap = new StructArray;
  sap->insert("pointer",Array(reinterpret_cast<uint64>(p)));
  const QMetaObject* mo = p->metaObject();
  sap->insert("name",Array(QString(mo->className())));
  sap->setClassName(QString(mo->className()));
  StructArray hsap(sap,eval);
  return Array(hsap);
}

//Signature
//sgfunction qtnew QtNewFunction
//input classname
//output ptr
//DOCBLOCK qt_qtnew
ArrayVector QtNewFunction(int nargout, const ArrayVector& arg, Interpreter *eval) {
  if (arg.size() == 0) return ArrayVector();
  QString classname = arg[0].asString();
  if (classname == "QPushButton")
    return MakeQTPointer(new QPushButton,eval);
  return ArrayVector();
}
