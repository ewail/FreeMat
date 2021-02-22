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
#include "HandleProperty.hpp"
#include "HandleList.hpp"
#include "HandleObject.hpp"
#include "HandleCommands.hpp"

static const char *auto_manual_dict[3] = {"auto","manual",0};
static const char *on_off_dict[3] = {"on","off",0};
static const char *on_inactive_off_dict[4] = {"on","active","off",0};
static const char *font_angle_dict[4] = {"normal","italic","oblique",0};
static const char *font_units_dict[6] = {"points","normalized","inches",
					 "centimeters","pixels",0};
static const char *font_weight_dict[5] = {"normal","bold","light","demi",0};
static const char *line_style_dict[6] = {"-","--",":","-.","none",0};
static const char *top_bottom_dict[3] = {"top","bottom",0};
static const char *left_right_dict[3] = {"left","right",0};
static const char *normal_reverse_dict[3] = {"normal","reverse",0};
static const char *linear_log_dict[3] = {"linear","log",0};
static const char *next_plot_dict[4] = {"add","replace","replacechildren",0};
static const char *projection_mode_dict[3] = {"orthographic","perspective",0};
static const char *in_out_dict[3] = {"in","out",0};
static const char *units_dict[8] = {"inches","centimeters","normalized",
				    "points","pixels","characters","data",0};
static const char *position_dict[3] = {"outerposition","position",0};
static const char *horiz_dict[4] = {"left","center","right",0};
static const char *vert_dict[6] = {"top","cap","middle","baseline","bottom",0};
static const char *symb_dict[19] = {"+","o","*",".","x","square","s","diamond","d","^","v",">","<","pentagram","p","hexagram","h","none",0};
static const char *mapmode_dict[4] = {"none","direct","scaled",0};
static const char *datamapmode_dict[3] = {"scaled","direct",0};
static const char *rowcols_dict[4] = {"both","rows","cols",0};
static const char *autoflat_dict[5] = {"none","auto","flat","colorspec",0};
static const char *lightingmode_dict[5] = {"none","flat","gouraud","phong",0};
static const char *interpmode_dict[5] = {"none","flat","interp","colorspec",0};
static const char *facealpha_dict[5] = {"flat","interp","texturemap","scalar",0};
static const char *backface_dict[4] = {"unlit","lit","reverselit",0};
static const char *edgealpha_dict[4] = {"flat","interp","scalar",0};
static const char *render_dict[3] = {"painters","opengl",0};
static const char *widget_dict[11] = {"pushbutton","toggle","radiobutton","checkbox",
				      "edit","text","slider","frame","listbox","popupmenu",0};
static const char *autocolor_dict[4] = {"none","auto","colorspec",0};
  
HPAutoManual::HPAutoManual() : HPConstrainedString(auto_manual_dict) {}
HPOnOff::HPOnOff() : HPConstrainedString(on_off_dict) {}
HPOnOffInactive::HPOnOffInactive() : HPConstrainedString(on_inactive_off_dict) {}
HPFontAngle::HPFontAngle() : HPConstrainedString(font_angle_dict) {}
HPFontUnits::HPFontUnits() : HPConstrainedString(font_units_dict) {}
HPFontWeight::HPFontWeight() : HPConstrainedString(font_weight_dict) {}
HPLineStyle::HPLineStyle() : HPConstrainedString(line_style_dict) {}
HPTopBottom::HPTopBottom() : HPConstrainedString(top_bottom_dict) {}
HPLeftRight::HPLeftRight() : HPConstrainedString(left_right_dict) {}
HPNormalReverse::HPNormalReverse() : HPConstrainedString(normal_reverse_dict) {}
HPLinearLog::HPLinearLog() : HPConstrainedString(linear_log_dict) {}
HPNextPlotMode::HPNextPlotMode() : HPConstrainedString(next_plot_dict) {}
HPProjectionMode::HPProjectionMode() : HPConstrainedString(projection_mode_dict) {}
HPInOut::HPInOut() : HPConstrainedString(in_out_dict) {}
HPUnits::HPUnits() : HPConstrainedString(units_dict) {}
HPPosition::HPPosition() : HPConstrainedString(position_dict) {}
HPAlignHoriz::HPAlignHoriz() : HPConstrainedString(horiz_dict) {}
HPAlignVert::HPAlignVert() : HPConstrainedString(vert_dict) {}
HPSymbol::HPSymbol() : HPConstrainedString(symb_dict) {}
HPMappingMode::HPMappingMode() : HPConstrainedString(mapmode_dict) {}
HPDataMappingMode::HPDataMappingMode() : HPConstrainedString(datamapmode_dict) {}
HPLineStyleOrder::HPLineStyleOrder() : HPConstrainedStringSet(line_style_dict) {}
HPRowColumns::HPRowColumns() : HPConstrainedString(rowcols_dict) {}
HPLightingMode::HPLightingMode() : HPConstrainedString(lightingmode_dict) {}
HPAutoFlatColor::HPAutoFlatColor() : HPConstrainedStringColor(autoflat_dict) {}
HPAutoColor::HPAutoColor() : HPConstrainedStringColor(autocolor_dict) {}
HPColorInterp::HPColorInterp() : HPConstrainedStringColor(interpmode_dict) {}
HPFaceAlpha::HPFaceAlpha() : HPConstrainedStringScalar(facealpha_dict) {}
HPBackFaceLighting::HPBackFaceLighting() : HPConstrainedString(backface_dict) {}
HPWidgetString::HPWidgetString() : HPConstrainedString(widget_dict) {}
HPEdgeAlpha::HPEdgeAlpha() : HPConstrainedStringScalar(edgealpha_dict) {}
HPRenderMode::HPRenderMode() : HPConstrainedString(render_dict) {}

HPHandles::HPHandles() {
}
  
Array HPHandles::Get() {
  Array ret(UInt32,NTuple(1,data.size()));
  uint32 *dp = ret.real<uint32>().data();
  for (int i=0;i<data.size();i++)
    dp[i] = data[i];
  return ret;
}

void HPHandles::Set(Array arg) {
  if (arg.isEmpty()) {
    data.clear();
    HandleProperty::Set(arg);
    return;
  }
  arg = arg.asDenseArray().toClass(UInt32);
  const uint32 *dp = arg.constReal<uint32>().constData();
  // make sure they are all valid handles
  for (int i=0;i<int(arg.length());i++) 
    ValidateHandle(dp[i]);
  data.clear();
  for (int i=0;i<int(arg.length());i++) 
    data.push_back(dp[i]);
  HandleProperty::Set(arg); 
}

Array HPString::Get() {
  return Array(data);
}

void HPString::Set(Array arg) {
  HandleProperty::Set(arg);
  if (!arg.isString())
    throw Exception("Expecting a string for property ");
  data = arg.asString();
}
  
Array HPVector::Get() {
  Array ret(Double,NTuple(1,data.size()));
  double *dp = ret.real<double>().data();
  for (int i=0;i<data.size();i++)
    dp[i] = data[i];
  return ret;
}
  
void HPVector::Set(Array num) {
  HandleProperty::Set(num);
  num = num.asDenseArray().toClass(Double);
  const double *dp = num.constReal<double>().constData();
  data.clear();
  for (int i=0;i<int(num.length());i++)
    data.push_back(dp[i]);
}
  
void HPFixedVector::Set(Array num) {
  HandleProperty::Set(num);
  num = num.asDenseArray().toClass(Double);
  const double *dp = num.constReal<double>().constData();
  data.clear();
  int m = qMin(m_len,int(num.length()));
  for (int i=0;i<m;i++)
    data.push_back(dp[i]);
  for (int i=m;i<m_len;i++)
    data.push_back(0);
}

double& HPVector::At(int ndx) {
  return data[ndx];
}

double& HPVector::operator[](int ndx) {
  return data[ndx];
}

void HPColorVector::Set(Array arg) {
  HandleProperty::Set(arg);
  arg = arg.asDenseArray().toClass(Double);
  if ((!arg.is2D()) || (arg.cols() != 3))
    throw Exception("Expect an m x 3 matrix for color orders");
  const double *dp = arg.constReal<double>().constData();
  int n = int(arg.length());
  for (int i=0;i<n;i++) 
    if ((dp[i] < 0) || (dp[i] > 1.0))
      throw Exception("Color vector must be between 0 and 1");
  data.clear();
  int rows = n/3;
  for (int i=0;i<rows;i++) 
    for (int j=0;j<3;j++)
      data.push_back(dp[i+j*rows]);
}

Array HPColorVector::Get() {
  int count = data.size();
  int rows = count/3;
  Array ret(Double,NTuple(rows,3));
  double *rp = ret.real<double>().data();
  for (int i=0;i<rows;i++)
    for (int j=0;j<3;j++)
      rp[i+j*rows] = data[3*i+j];
  return ret;
}

//DOCBLOCK handle_colorspec
bool ParseColorSpec(Array arg, QVector<double> &data) {
  if (arg.isString()) {
    QString cp = arg.asString();
    if (cp=="none") {
      data.clear(); data.push_back(-1); 
      data.push_back(-1); data.push_back(-1);
    } else if ((cp=="y") || (cp=="yellow")) {
      data.clear(); data.push_back(.75); 
      data.push_back(.75); data.push_back(0);
    } else if ((cp=="m") || (cp=="magenta")) {
      data.clear(); data.push_back(.75); 
      data.push_back(0); data.push_back(.75);
    } else if ((cp=="c") || (cp=="cyan")) {
      data.clear(); data.push_back(0); 
      data.push_back(.75); data.push_back(.75);
    } else if ((cp=="r") || (cp=="red")) {
      data.clear(); data.push_back(1); 
      data.push_back(0); data.push_back(0);
    } else if ((cp=="g") || (cp=="green")) {
      data.clear(); data.push_back(0); 
      data.push_back(.5); data.push_back(0);
    } else if ((cp=="b") || (cp=="blue")) {
      data.clear(); data.push_back(0); 
      data.push_back(0); data.push_back(1);
    } else if ((cp=="w") || (cp=="white")) {
      data.clear(); data.push_back(1); 
      data.push_back(1); data.push_back(1);
    } else if ((cp=="k") || (cp=="black")) {
      data.clear(); data.push_back(0); 
      data.push_back(0); data.push_back(0);
    } else
      return false;
  } else {
    if (arg.length() != 3)
      return false;
    arg = arg.asDenseArray().toClass(Double);
    const double *dp = arg.constReal<double>().constData();
    if (((dp[0] < 0) || (dp[0] > 1)) ||
	((dp[1] < 0) || (dp[1] > 1)) ||
	((dp[2] < 0) || (dp[2] > 1)))
      return false;
    data.clear();
    data.push_back(dp[0]);
    data.push_back(dp[1]);
    data.push_back(dp[2]);
  }
  return true;
}

void HPColor::Set(Array arg) {
  HandleProperty::Set(arg);
  if (!ParseColorSpec(arg,data))
    throw Exception("Expecting a color spec: either a color name or a 3-vector or RGB values");
}
  
Array HPColor::Get() {
  if (data[0] == -1)
    return Array(QString("none"));
  else
    return HPVector::Get();
}
  
Array HPStringSet::Get() {
  if (data.size() == 0)
    return Array(QString(""));
  QString retval;
  for (int i=0;i<data.size()-1;i++) {
    retval.append(data[i]);
    retval.append("|");
  }
  retval.append(data.back());
  return Array(retval);
}

void HPStringSet::Set(Array arg) {
  HandleProperty::Set(arg);
  if (!arg.isString()) 
    throw Exception("expecting a '|'-delimited list of strings for property argument");
  QString args(arg.asString());
  data.clear();
  data = args.split("|");
}

double HPConstrainedStringScalar::Scalar() {
  return scalar;
}

void HPConstrainedStringScalar::Scalar(double scal) {
  scalar = scal;
}

void HPConstrainedStringScalar::Set(Array arg) {
  HandleProperty::Set(arg);
  if (arg.isString())
    HPConstrainedString::Set(arg);
  else
    scalar = arg.asDouble();
}

QVector<double> HPConstrainedStringColor::ColorSpec() {
  return colorspec;
}

void HPConstrainedStringColor::ColorSpec(QVector<double> col) {
  colorspec = col;
}

void HPConstrainedStringColor::ColorSpec(double r, double g, double b) {
  QVector<double> data;
  data.push_back(r);
  data.push_back(g);
  data.push_back(b);
  colorspec = data;
}

void HPConstrainedStringColor::Set(Array arg) {
  HandleProperty::Set(arg);
  if (!ParseColorSpec(arg,colorspec)) {
    HPConstrainedString::Set(arg);
  } else {
    data = "colorspec";
  }
  if (colorspec.size() > 1 && (colorspec[0] == -1))
    data = "none";
}

Array HPConstrainedStringColor::Get() {
  if (!Is("colorspec"))
    return HPConstrainedString::Get();
  Array ret(Double,NTuple(1,3));
  double *dp = ret.real<double>().data();
  dp[0] = colorspec[0];
  dp[1] = colorspec[1];
  dp[2] = colorspec[2];
  return ret;
}

Array HPConstrainedStringScalar::Get() {
  if (!Is("scalar"))
    return HPConstrainedString::Get();
  return Array(double(scalar));
}

void HPConstrainedString::Set(Array arg) {
  HandleProperty::Set(arg);
  if (!arg.isString())
    throw Exception("expecting a string for property");
  QString tst(arg.asString());
  if (!m_dictionary.contains(tst))
    throw Exception("illegal selection for property");
  HPString::Set(arg);
}

void HPConstrainedStringSet::Set(Array arg) {
  HandleProperty::Set(arg);
  HPStringSet::Set(arg);
  // Validate the result
  for (int i=0;i<data.size();i++)
    if (!m_dictionary.contains(data[i]))
      throw Exception("illegal selection for property");
}

void HPScalar::Value(double x) {
  At(0) = x;
}
