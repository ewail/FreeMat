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
#ifndef __HandleProperty_hpp__
#define __HandleProperty_hpp__

#include "Array.hpp"
#include "Algorithms.hpp"
#include <string>
#include <QColor>

class HandleProperty {
  bool modified;
public:
  HandleProperty() {modified = false;}
  virtual ~HandleProperty() {}
  virtual Array Get() = 0;
  virtual void Set(Array) {modified = true;}
  void ClearModified() {modified = false;}
  bool isModified() {return modified;}
};
  
class HPArray : public HandleProperty {
protected:
  Array data;
public:
  HPArray() : data(EmptyConstructor()) {}
  virtual ~HPArray() {}
  virtual Array Get() {return data;}
  virtual void Set(Array m) {HandleProperty::Set(m); data = m;}
  Array Data() {return data;}
  void Data(Array m) {data = m;}    
};

class HPHandles : public HandleProperty {
protected:
  QVector<unsigned> data;
public:
  HPHandles();
  virtual ~HPHandles() {}
  virtual Array Get();
  virtual void Set(Array);
  QVector<unsigned> Data() {return data;}
  void Data(QVector<unsigned> m) {data = m;}
};

class HPVector : public HandleProperty {
protected:
  QVector<double> data;
public:
  HPVector() {}
  virtual ~HPVector() {}
  virtual Array Get();
  virtual void Set(Array);
  QVector<double> Data() {return data;}
  void Data(QVector<double> m) {data = m;}
  void Data(double m) {QVector<double> n; n.push_back(m); data = n;}
  double& operator[](int ndx);
  double& At(int ndx);
};

class HPFixedVector : public HPVector {
protected:
  int m_len;
public:
  HPFixedVector(int len) : m_len(len) {
    for (int i=0;i<len;i++) data.push_back(0);
  }
  virtual ~HPFixedVector() {}
  virtual void Set(Array);    
};

class HPString : public HandleProperty {
protected:
  QString data;
public:
  HPString() {}
  virtual ~HPString() {}
  virtual Array Get();
  virtual void Set(Array);
  QString Data() {return data;}
  void Data(QString m) {data = m;}
  void Value(QString m) {data = m;}
  bool Is(QString m) {return (data == m);}
};

class HPStringSet : public HandleProperty {
protected:
  StringVector data;
public:
  HPStringSet() {}
  virtual ~HPStringSet() {}
  virtual Array Get();
  virtual void Set(Array);
  StringVector Data() {return data;}
  void Data(StringVector m) {data = m;}
};

class HPConstrainedString : public HPString {
protected:
  StringVector m_dictionary;
public:
  HPConstrainedString(StringVector dict) : m_dictionary(dict) {
    data = dict[0];
  }
  HPConstrainedString(const char **dict) {
    while (*dict) {
      m_dictionary.push_back(*dict);
      dict++;
    }
  }
  virtual void Set(Array);    
};

class HPConstrainedStringSet : public HPStringSet {
  StringVector m_dictionary;
public:
  HPConstrainedStringSet(const char **dict) {
    while (*dict) {
      m_dictionary.push_back(*dict);
      dict++;
    }
  }
  virtual ~HPConstrainedStringSet() {}
  virtual void Set(Array);
};
  
class HPTwoVector : public HPFixedVector {
public:
  HPTwoVector() : HPFixedVector(2) {}
  virtual ~HPTwoVector() {}
  void Value(double x, double y) {At(0) = x; At(1) = y;}
};

class HPThreeVector : public HPFixedVector {
public:
  HPThreeVector() : HPFixedVector(3) {}
  virtual ~HPThreeVector() {}
  void Value(double x, double y, double z) {At(0) = x; At(1) = y; At(2) = z;}
};
  
class HPFourVector : public HPFixedVector {
public:
  HPFourVector() : HPFixedVector(4) {}
  virtual ~HPFourVector() {}
  void Value(double x, double y, double z, double w) {At(0) = x; At(1) = y; At(2) = z; At(3) = w;}
};

class HPSixVector : public HPFixedVector {
public:
  HPSixVector() : HPFixedVector(6) {}
  virtual ~HPSixVector() {}
  void Value(double x1, double x2,
	     double y1, double y2,
	     double z1, double z2) {
    At(0) = x1; At(1) = x2; 
    At(2) = y1; At(3) = y2;
    At(4) = z1; At(5) = z2;}
};

class HPAutoManual : public HPConstrainedString {
public:
  HPAutoManual();
  virtual ~HPAutoManual() {}
};

class HPColor : public HPFixedVector {
public:
  HPColor() : HPFixedVector(3) {}
  virtual ~HPColor() {}
  virtual void Set(Array);    
  Array Get();    
  bool IsNone() {return (data[0]<0);}
  QColor asQColor() {return QColor((int)(data[0]*255),(int)(data[1]*255),(int)(data[2]*255));}
};

class HPColorVector : public HPVector {
public:
  HPColorVector() : HPVector() {}
  virtual ~HPColorVector() {}
  virtual void Set(Array);    
  Array Get();
};

class HPOnOff : public HPConstrainedString {
public:
  HPOnOff();
  virtual ~HPOnOff() {}
  bool AsBool() {return Is("on");}
};

class HPScalar : public HPFixedVector {
public:
  HPScalar() : HPFixedVector(1) {}
  virtual ~HPScalar() {}
  void Value(double x);
};

    
class HPFontAngle : public HPConstrainedString {
public:
  HPFontAngle();
  virtual ~HPFontAngle() {}
};
  
  
class HPFontUnits : public HPConstrainedString {
public:
  HPFontUnits();
  virtual ~HPFontUnits() {}
};
  

class HPFontWeight : public HPConstrainedString {
public:
  HPFontWeight();
  virtual ~HPFontWeight() {}
};


class HPLineStyle : public HPConstrainedString {
public:
  HPLineStyle();
  virtual ~HPLineStyle() {}
};


class HPTopBottom : public HPConstrainedString {
public:
  HPTopBottom();
  virtual ~HPTopBottom() {}
};


class HPLeftRight : public HPConstrainedString {
public:
  HPLeftRight();
  virtual ~HPLeftRight() {}
};


class HPNormalReverse : public HPConstrainedString {
public:
  HPNormalReverse();
  virtual ~HPNormalReverse() {}
};


class HPLinearLog : public HPConstrainedString {
public:
  HPLinearLog();
  virtual ~HPLinearLog() {}
};
  

class HPNextPlotMode : public HPConstrainedString {
public:
  HPNextPlotMode();
  virtual ~HPNextPlotMode() {}
};


class HPProjectionMode : public HPConstrainedString {
public:
  HPProjectionMode();
  virtual ~HPProjectionMode() {}
};
  

class HPInOut : public HPConstrainedString {
public:
  HPInOut();
  virtual ~HPInOut() {}
};

  
class HPUnits : public HPConstrainedString {
public:
  HPUnits();
  virtual ~HPUnits() {}
};
  
  
class HPPosition : public HPConstrainedString {
public:
  HPPosition();
  virtual ~HPPosition() {}
};

class HPAlignHoriz : public HPConstrainedString {
public:
  HPAlignHoriz();
  virtual ~HPAlignHoriz() {}
};

class HPAlignVert : public HPConstrainedString {
public:
  HPAlignVert();
  virtual ~HPAlignVert() {}
};

class HPSymbol : public HPConstrainedString {
public:
  HPSymbol();
  virtual ~HPSymbol() {}
};

class HPLineStyleOrder : public HPConstrainedStringSet {
public:
  HPLineStyleOrder();
  virtual ~HPLineStyleOrder() {}
};

class HPMappingMode : public HPConstrainedString {
public:
  HPMappingMode();
  virtual ~HPMappingMode() {}
};

class HPDataMappingMode : public HPConstrainedString {
public:
  HPDataMappingMode();
  virtual ~HPDataMappingMode() {}
};

class HPOnOffInactive : public HPConstrainedString {
public:
  HPOnOffInactive();
  virtual ~HPOnOffInactive() {}
};

class HPRowColumns : public HPConstrainedString {
public:
  HPRowColumns();
  virtual ~HPRowColumns() {}
};

class HPLightingMode : public HPConstrainedString {
public:
  HPLightingMode();
  virtual ~HPLightingMode() {}
};

class HPRenderMode : public HPConstrainedString {
public:
  HPRenderMode();
  virtual ~HPRenderMode() {}
};

class HPConstrainedStringScalar : public HPConstrainedString {
  double scalar;
public:
  HPConstrainedStringScalar(const char **dict) : 
    HPConstrainedString(dict) {}
  virtual ~HPConstrainedStringScalar() {}
  virtual void Set(Array);
  Array Get();
  double Scalar();
  void Scalar(double val);
};

class HPConstrainedStringColor : public HPConstrainedString {
  QVector<double> colorspec;
public:
  HPConstrainedStringColor(const char **dict) : 
    HPConstrainedString(dict) {}
  virtual ~HPConstrainedStringColor() {}
  virtual void Set(Array);
  Array Get();
  QVector<double> ColorSpec();
  void ColorSpec(QVector<double> col);
  void ColorSpec(double r, double g, double b);
};

class HPAutoColor : public HPConstrainedStringColor {
public:
  HPAutoColor();
  virtual ~HPAutoColor() {}
};

class HPAutoFlatColor : public HPConstrainedStringColor {
public:
  HPAutoFlatColor();
  virtual ~HPAutoFlatColor() {}
};
  
class HPColorInterp : public HPConstrainedStringColor {
public:
  HPColorInterp();
  virtual ~HPColorInterp() {}
};

class HPWidgetString : public HPConstrainedString {
public:
  HPWidgetString();
  virtual ~HPWidgetString() {}
};

class HPFaceAlpha : public HPConstrainedStringScalar {
public:
  HPFaceAlpha();
  virtual ~HPFaceAlpha() {}
};

class HPBackFaceLighting : public HPConstrainedString {
public:
  HPBackFaceLighting();
  virtual ~HPBackFaceLighting() {}
};
  
class HPEdgeAlpha : public HPConstrainedStringScalar {
public:
  HPEdgeAlpha();
  virtual ~HPEdgeAlpha() {}
};

#endif
