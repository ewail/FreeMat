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
#include "HandleObject.hpp"
#include "HandleAxis.hpp"
#include "HandleList.hpp"
#include "HandleFigure.hpp"
#include "HandleCommands.hpp"
#include "IEEEFP.hpp"

bool HandleObject::IsType(QString name) {
  HPString* sp = (HPString*) LookupProperty("type");
  return (sp->Is(name));
}

void HandleObject::ToManual(QString name) {
  HPAutoManual *qp = (HPAutoManual*) LookupProperty(name);
  qp->Data("manual");
}

bool HandleObject::HasChanged(StringVector names) {
  HandleProperty *hp;
  for (int i=0;i<names.size();i++) {
    hp = LookupProperty(names[i]);
    if (hp->isModified()) return true;
  }
  return false;
}

bool HandleObject::HasChanged(QString name) {
  StringVector names;
  names.push_back(name);
  return HasChanged(names);
}

void HandleObject::ClearAllChanged() {
  StringVector names(m_properties.getCompletions(""));
  ClearChanged(names);
}

void HandleObject::ClearChanged(StringVector names) {
  HandleProperty *hp;
  for (int i=0;i<names.size();i++) {
    hp = LookupProperty(names[i]);
    hp->ClearModified();
  }    
}

void HandleObject::ClearChanged(QString name) {
  HandleProperty *hp;
  hp = LookupProperty(name);
  hp->ClearModified();
}

HandleObject::HandleObject() {
  ref_count = 1;
  m_stateDirty = false;
}

HandleObject::~HandleObject() {
  // Loop through our children
  HPHandles *hp = (HPHandles*) LookupProperty("children");
  HandleObject *gp;
  QVector<unsigned> my_children(hp->Data());
  for (int i=0;i<my_children.size();i++) {
    unsigned handle = my_children[i];
    if (handle >= HANDLE_OFFSET_OBJECT) {
      gp = LookupHandleObject(handle);
      gp->Dereference();
      if (gp->RefCount() <= 0) {
	//	  qDebug("Deleting handle %d\n",handle);
	FreeHandleObject(handle);
	delete gp;
      }
    }
  }
  // Delete our properties also
  StringVector propSet(m_properties.getCompletions(""));
  for (int i=0;i<propSet.size();i++) {
    HandleProperty** hp = m_properties.findSymbol(propSet[i]);
    if (hp) delete (*hp);
  }
}

HandleProperty* HandleObject::LookupProperty(QString name) {
  name = name.toLower();
  // First look for the property to match (avoids the problem
  // with prefix names)
  HandleProperty** hp = m_properties.findSymbol(name);
  if (hp) return (*hp);
  //complete it
  StringVector completes(m_properties.getCompletions(name));
  if (completes.empty())
    throw Exception("invalid property " + name);
  if (completes.size() != 1)
    throw Exception("ambiguous property name " + name);
  hp = m_properties.findSymbol(completes[0]);
  return *hp;
}

void HandleObject::SetPropertyHandle(QString name, unsigned value) {
  HPHandles* hp = (HPHandles*) LookupProperty(name);
  QVector<unsigned> newval;
  newval.push_back(value);
  hp->Data(newval);
}

HandleAxis* HandleObject::GetParentAxis() {
  // Get our parent - must be an axis
  HPHandles *parent = (HPHandles*) LookupProperty("parent");
  if (parent->Data().empty()) return NULL;
  unsigned parent_handle = parent->Data()[0];
  HandleObject *fp = LookupHandleObject(parent_handle);
  HPString *name = (HPString*) fp->LookupProperty("type");
  if (!name) return NULL;
  if (!name->Is("axes")) return NULL;
  HandleAxis *axis = (HandleAxis*) fp;
  return axis;
}

HandleFigure* HandleObject::GetParentFigure() {
  HandleAxis* hp;
  if (StringCheck("type","axes")) 
    hp = (HandleAxis*) this;
  else
    hp = GetParentAxis();
  // Get our parent - must be an axis
  HPHandles *parent = (HPHandles*) hp->LookupProperty("parent");
  if (parent->Data().empty()) return NULL;
  unsigned parent_handle = parent->Data()[0];
  return LookupHandleFigureNoCreate(parent_handle);
}

QString HandleObject::StringPropertyLookup(QString name) {
  HPString* sp = (HPString*) LookupProperty(name);
  return (sp->Data());
}

QVector<double> HandleObject::VectorPropertyLookup(QString name) {
  HPVector* sp = (HPVector*) LookupProperty(name);
  return (sp->Data());
}

Array HandleObject::ArrayPropertyLookup(QString name) {
  HPArray* hp = (HPArray*) LookupProperty(name);
  return (hp->Data());
}
  
double HandleObject::ScalarPropertyLookup(QString name) {
  HPScalar* sp = (HPScalar*) LookupProperty(name);
  return (sp->Data()[0]);
}

unsigned HandleObject::HandlePropertyLookup(QString name) {
  HPHandles* sp = (HPHandles*) LookupProperty(name);
  if (sp->Data().empty())
    return 0;
  else
    return (sp->Data()[0]);
}

void HandleObject::AddProperty(HandleProperty* hp, QString name) {
  m_properties.insertSymbol(name,hp);
}

void HandleObject::SetConstrainedStringScalarDefault(QString name,
						     QString value,
						     double sval) {
  HPConstrainedStringScalar *hp = 
    (HPConstrainedStringScalar*) LookupProperty(name);
  if (!hp)
    throw Exception("constrained string/scalar default failed lookup of <" + 
		    name + ">");
  hp->Value(value);
  hp->Scalar(sval);
}

void HandleObject::SetConstrainedStringColorDefault(QString name,
						    QString value,
						    double red, 
						    double green,
						    double blue) {
  HPConstrainedStringColor *hp = 
    (HPConstrainedStringColor*) LookupProperty(name);
  if (!hp)
    throw Exception("constrained string/color default failed lookup of <" +
		    name + ">");
  hp->Value(value);
  hp->ColorSpec(red,green,blue);
}
						     

void HandleObject::SetConstrainedStringDefault(QString name, QString value) {
  HPConstrainedString *hp = (HPConstrainedString*) LookupProperty(name);
  if (!hp)
    throw Exception("set constrained string default failed lookup of <" + name + ">");
  hp->Value(value);
}

void HandleObject::SetConstrainedStringSetDefault(QString name, QString values) {
  HPConstrainedStringSet *hp = (HPConstrainedStringSet*) LookupProperty(name);
  StringVector data(values.split("|"));
  ((HPStringSet*)hp)->Data(data);
}

void HandleObject::SetThreeVectorDefault(QString name, double x, double y, double z) {
  HPThreeVector *hp = (HPThreeVector*) LookupProperty(name);
  hp->Value(x,y,z);
}

void HandleObject::SetFourVectorDefault(QString name, double x, double y,
					double z, double w) {
  HPFourVector *hp = (HPFourVector*) LookupProperty(name);
  hp->Value(x,y,z,w);
}

void HandleObject::SetTwoVectorDefault(QString name, double x, double y) {
  HPTwoVector *hp = (HPTwoVector*) LookupProperty(name);
  hp->Value(x,y);
}

void HandleObject::SetStringDefault(QString name, QString value) {
  HPString *hp = (HPString*) LookupProperty(name);
  hp->Value(value);
}

void HandleObject::SetScalarDefault(QString name, double value) {
  HPScalar *hp = (HPScalar*) LookupProperty(name);
  hp->Value(value);
}

bool HandleObject::IsAuto(QString mode) {
  HPAutoManual *hp = (HPAutoManual*) LookupProperty(mode);
  return hp->Is("auto");
}
  
bool HandleObject::StringCheck(QString name, QString value) {
  HPString *hp = (HPString*) LookupProperty(name);
  return hp->Is(value);
}

double VecMin(QVector<double> &v) {
  double min = 0;
  bool first = true;
  for (int i=0;i<v.size();i++) {
    if (IsFinite(v[i]))
      {
	if (first) {
	  first = false;
	  min = v[i];
	} else if (v[i] < min) {
	  min = v[i];
	}
      }
  }
  return min;
}

double VecMax(QVector<double> &v) {
  double max = 0;
  bool first = true;
  for (int i=0;i<v.size();i++) {
    if (IsFinite(v[i]))
      {
	if (first) {
	  first = false;
	  max = v[i];
	} else if (v[i] > max) {
	  max = v[i];
	}
      }
  }
  return max;
}

double ArrayMin(Array a) {
  if (a.isEmpty()) return 0;
  a = a.toClass(Double);
  const double* v = a.constReal<double>().constData();
  int len = int(a.length());
  double min = 0;
  bool first = true;
  for (int i=0;i<len;i++) {
    if (IsFinite(v[i]))
      {
	if (first) {
	  first = false;
	  min = v[i];
	} else if (v[i] < min) {
	  min = v[i];
	}
      }
  }
  return min;
}

double ArrayMax(Array a) {
  if (a.isEmpty()) return 0;
  a = a.toClass(Double);
  const double* v = a.constReal<double>().constData();
  int len = int(a.length());
  double max = 0;
  bool first = true;
  for (int i=0;i<len;i++) {
    if (IsFinite(v[i]))
      {
	if (first) {
	  first = false;
	  max = v[i];
	} else if (v[i] > max) {
	  max = v[i];
	}
      }
  }
  return max;
}
