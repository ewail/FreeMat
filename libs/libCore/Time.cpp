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
#include <QtCore>
#include "Array.hpp"

//@@Signature
//function tic TicFunction jitsafe
//inputs none
//outputs none
//DOCBLOCK freemat_tic
  
static QTime ticvalue;

ArrayVector TicFunction(int nargout, const ArrayVector& arg) {
  ticvalue.start();
  return ArrayVector();
}

//@@Signature
//function clock ClockFunction jitsafe
//inputs none
//outputs y
//DOCBLOCK freemat_clock
ArrayVector ClockFunction(int nargout, const ArrayVector& arg) {
  QDateTime ctime(QDateTime::currentDateTime());
  Array retvec(Double,NTuple(1,6));
  BasicArray<double> &dp(retvec.real<double>());
  dp[1] = ctime.date().year();
  dp[2] = ctime.date().month();
  dp[3] = ctime.date().day();
  dp[4] = ctime.time().hour();
  dp[5] = ctime.time().minute();
  dp[6] = ctime.time().second() + ctime.time().msec()/1.0e3;
  return ArrayVector(retvec);
}

//@@Signature
//function clocktotime ClockToTimeFunction jitsafe
//inputs x
//outputs y
//DOCBLOCK freemat_clocktotime
ArrayVector ClockToTimeFunction(int nargout, const ArrayVector& arg) {
  if (arg.size() != 1)
    throw Exception("clocktotime expects 1 argument - a vector in clock format: [year month day hour minute seconds]");
  Array targ(arg[0].toClass(Double));
  if (targ.length() != 6)
    throw Exception("clocktotime expects 1 argument - a vector in clock format: [year month day hour minute seconds]");
  const BasicArray<double> &dp(targ.constReal<double>());
  QDate t_date;
  t_date.setDate(int(dp[1]),int(dp[2]),int(dp[3]));
  QTime t_time;
  t_time.setHMS(int(dp[4]),int(dp[5]),int(dp[6]));
  QDateTime t_datetime(t_date,t_time);
  double retval = t_datetime.toTime_t() + (dp[6] - int(dp[6]));
  return ArrayVector(Array(retval));
}
  

//@@Signature
//function toc TocFunction jitsafe
//inputs none
//outputs y
//DOCBLOCK freemat_toc
ArrayVector TocFunction(int nargout, const ArrayVector& arg) {
  return ArrayVector(Array(ticvalue.elapsed()/1.0e3));
}
