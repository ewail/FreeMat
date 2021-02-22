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
#ifndef __HandleObject_hpp__
#define __HandleObject_hpp__

#include <string>
#include "SymbolTable.hpp"
#include "HandleProperty.hpp"
#include "RenderEngine.hpp"


class HandleAxis;
class HandleFigure;

double VecMin(QVector<double> &v);
double VecMax(QVector<double> &v);
double ArrayMin(Array x);
double ArrayMax(Array x);

class HandleObject {
  FM::SymbolTable<HandleProperty*> m_properties;
  unsigned ref_count;
  bool m_stateDirty;
public:
  HandleObject();
  virtual ~HandleObject();
  void Reference() {ref_count++;}
  void Dereference() {if (ref_count > 0) ref_count--;}
  unsigned RefCount() {return ref_count;}
  bool isDirty() {return m_stateDirty;}
  void markDirty() {m_stateDirty = true;}
  void markClean() {m_stateDirty = false;}
  virtual void RegisterProperties() {}
  virtual void UpdateState() {}
  virtual QVector<double> GetLimits() {return QVector<double>();};
  bool HasChanged(StringVector names);
  bool HasChanged(QString name);
  void ToManual(QString name);
  bool IsType(QString name);
  void ClearChanged(StringVector names);
  void ClearChanged(QString name);
  void ClearAllChanged();
  void AddProperty(HandleProperty* prop, QString name);
  HandleProperty* LookupProperty(QString name);
  double ScalarPropertyLookup(QString name);
  unsigned HandlePropertyLookup(QString name);
  QVector<double> VectorPropertyLookup(QString name);
  Array ArrayPropertyLookup(QString name);
  QString StringPropertyLookup(QString name);
  bool StringCheck(QString name, QString value);
  void SetConstrainedStringDefault(QString name, QString value);
  void SetConstrainedStringSetDefault(QString name, QString value);
  void SetConstrainedStringScalarDefault(QString name, QString value, double scalar);
  void SetConstrainedStringColorDefault(QString name, QString value, double r, double g, double b);
  void SetTwoVectorDefault(QString name, double x, double y);
  void SetThreeVectorDefault(QString name, double x, double y, double z);
  void SetFourVectorDefault(QString name, double x, double y, 
			    double z, double w);
  void SetStringDefault(QString name, QString value);
  void SetScalarDefault(QString name, double value);
  void SetPropertyHandle(QString name, unsigned value);
  bool IsAuto(QString mode);
  virtual void PaintMe(RenderEngine &gc) = 0;
  HandleAxis* GetParentAxis();
  HandleFigure* GetParentFigure();
};

#endif
