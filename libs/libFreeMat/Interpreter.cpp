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

#include "Interpreter.hpp"
#include <math.h>
#include <stdio.h>
#include "Exception.hpp"
#include "Math.hpp"
#include "Array.hpp"
#include "Struct.hpp"
#include "Parser.hpp"
#include "Scanner.hpp"
#include "Token.hpp"
#include "Module.hpp"
#include "File.hpp"
#include <signal.h>
#include "Class.hpp"
#include "Print.hpp"
#include "MemPtr.hpp"
#include <qeventloop.h>
#include <QtCore>
#include <fstream>
#include <stdarg.h>
#include "JITFactory.hpp"
#include "JITInfo.hpp"
#include "IEEEFP.hpp"
#include "Algorithms.hpp"
#include "GetSet.hpp"
#include "FuncPtr.hpp"
#include "AnonFunc.hpp"
#include "Stats.hpp"
#include <QtGui>
#include "DebugStream.hpp"
#include "CArray.hpp"

#ifdef _WIN32
#define PATHSEP ";"
#else
#define PATHSEP ":"
#endif

#ifdef _WIN32
    #define EXPORT __declspec(dllexport)
#else
    #define EXPORT
#endif

const int max_line_count = 1000000;

/**
 * The database of compiled code segments
 */
QMap<int,JITInfo> m_codesegments;


void ClearJITCache() {
  m_codesegments.clear();
}

/**
 * The file system watcher -- watches for changes to the file system
 * Only one interpreter thread should use this watcher at a time.
 */
//QFileSystemWatcher m_watch;


#define SaveEndInfo  \
  ArrayReference oldEndRef = endRef; \
  int oldEndCount = endCount; \
  int oldEndTotal = endTotal;

#define RestoreEndInfo \
  endRef = oldEndRef; \
  endCount = oldEndCount; \
  endTotal = oldEndTotal;  \

QString TildeExpand(QString path) {
  if ((path.size() > 0) && (path[0] == '~')) {
    path.remove(0,1);
    return QDir::homePath() + path;
  }
  return path;
}

class CLIDisabler {
  Interpreter *p;
  bool CLIFlagSave;
public:
  CLIDisabler(Interpreter *q) : p(q) {
    CLIFlagSave = p->inCLI();
    p->setInCLI(false);
  }
  ~CLIDisabler() {
    p->setInCLI(CLIFlagSave);
  }
};

void Interpreter::setPath(QString path) {
  if (path == m_userCachePath) return;
  QStringList pathset(path.split(PATHSEP,QString::SkipEmptyParts));
  m_userPath.clear();
  for (int i=0;i<pathset.size();i++)
    if (pathset[i] != ".") {
      QDir tpath(TildeExpand(pathset[i]));
      m_userPath << tpath.absolutePath();
    }
  setupWatcher();
  //  rescanPath();
  updateFileTool();
  m_userCachePath = path;
}

QString Interpreter::getTotalPath() {
  QString retpath;
  QStringList totalPath(QStringList() << m_basePath << m_userPath);
  for (int i=0;i<totalPath.size()-1;i++)
    retpath = retpath + totalPath[i] + PATHSEP;
  if (totalPath.size() > 0)
    retpath = retpath + totalPath[totalPath.size()-1];
  return retpath;
}

QString Interpreter::getPath() {
  QString retpath;
  QStringList totalPath(m_userPath);
  for (int i=0;i<totalPath.size()-1;i++)
    retpath = retpath + totalPath[i] + PATHSEP;
  if (totalPath.size() > 0)
    retpath = retpath + totalPath[totalPath.size()-1];
  return retpath;
}

void Interpreter::setLiveUpdateFlag(bool t) {
  m_liveUpdateFlag = t;
  if (t) {
    //    connect(&m_watch,SIGNAL(directoryChanged(const QString &)),
    // 	    this,SLOT(updateFileTool(const QString &)));
  }
}

// static bool DirExists(const QString & path) {
//   QDir tmp(path);
//   return tmp.exists();
// }

void Interpreter::setupWatcher() {
  if (!m_liveUpdateFlag) return;
  //  QStringList pathLists(m_watch.directories());
  //  if (!pathLists.isEmpty())
  //    m_watch.removePaths(pathLists);
  //   if (!m_userPath.isEmpty()) {
  //     for (int i=0;i<m_userPath.size();i++)
  //       if (DirExists(m_userPath[i]))
  // 	m_watch.addPath(m_userPath[i]);
  //   }
  //   if (!m_basePath.isEmpty()) {
  //     for (int i=0;i<m_basePath.size();i++)
  //       if (DirExists(m_basePath[i]))
  // 	m_watch.addPath(m_basePath[i]);
  //   }
  //   m_watch.addPath(QDir::currentPath());
}

void Interpreter::changeDir(QString path) {
  if (QDir(path) == QDir::currentPath())
    return;
  if (!QDir::setCurrent(path))
    throw Exception("Unable to change to specified directory: " + path);
  if (m_liveUpdateFlag)
    emit CWDChanged(QDir::currentPath());
  setupWatcher();
  rescanPath();
  //  updateFileTool();
}

void Interpreter::updateVariablesTool() {
  if (!m_liveUpdateFlag) return;
  StringVector varList(context->listAllVariables());
  QList<QVariant> vars;
  for (int i=0;i<varList.size();i++) {
    QList<QVariant> entry;
    // Icon
    entry << QVariant();
    entry << QVariant(varList[i]);
    Array *dp = context->lookupVariableLocally(varList[i]);
    if (dp) {
      // class
      if (dp->allReal())
	entry << QVariant(dp->className());
      else
	entry << QVariant(dp->className() + " (complex)");
      // value
      entry << QVariant(SummarizeArrayCellEntry(*dp));
      // size
      entry << QVariant(dp->dimensions().toString());
      // bytes min, max, range, mean, var, std
      entry << QVariant(double(dp->bytes()));
      entry += ComputeVariableStats(dp);
    }
    for (int i=entry.size();i<10;i++)
      entry << QVariant();
    vars << QVariant(entry);
  }
  emit updateVarView(QVariant(vars));
}

static bool InSpecificScope(Context *context, QString name, QString detail) {
  return ((context->scopeName() == name) &&
	  (context->scopeDetailString() == detail));
}

static bool InKeyboardScope(Context *context) {
  return InSpecificScope(context,"keyboard","keyboard");
}

static QString GetStackToolDescription(Context *context) {
  int line = int(LineNumber(context->scopeTokenID()));
  if (line > 0)
    return QString(context->scopeDetailString() + QString("(%1)").arg(line));
  else
    return context->scopeDetailString();
}

void Interpreter::updateStackTool() {
  QStringList stackInfo;
  // Do a complete dump...
  // Suppose we start with
  int f_depth = context->scopeDepth();
  context->restoreBypassedScopes();
  int t_depth = context->scopeDepth();
  for (int i=f_depth;i<t_depth;i++) {
    if (!InKeyboardScope(context) && !context->scopeDetailString().isEmpty())
      stackInfo << GetStackToolDescription(context);
    context->bypassScope(1);
  }
  bool firstline = true;
  for (int i=0;i<f_depth;i++) {
    if (!InKeyboardScope(context) &&
	!InSpecificScope(context,"docli","builtin") &&
	!context->scopeDetailString().isEmpty()) {
      if (firstline) {
	stackInfo << QString("*") + GetStackToolDescription(context);
	firstline = false;
      } else
	stackInfo << GetStackToolDescription(context);
    }
    context->bypassScope(1);
  }
  context->restoreBypassedScopes();
  while (context->scopeDepth() > f_depth) context->bypassScope(1);
  emit updateStackView(stackInfo);
}

void Interpreter::updateFileTool(const QString &) {
  updateFileTool();
}

void Interpreter::updateFileTool() {
  // Build the info to send to the file tool
  QDir dir(QDir::currentPath());
  dir.setFilter(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
  QFileInfoList list(dir.entryInfoList());
  QList<QVariant> files;
  QList<QVariant> entry;
  entry << QVariant(QString("dir"));
  entry << QVariant(" .. (Parent Folder)");
  entry << QVariant();
  entry << QVariant();
  entry << QVariant("Folder");
  files << QVariant(entry);
  for (int i=0;i<((int)list.size());i++) {
    QList<QVariant> entry;
    QFileInfo fileInfo(list.at(i));
    if (fileInfo.isDir())
      entry << QVariant(QString("dir"));
    else
      entry << QVariant(QString("file"));
    entry << QVariant(fileInfo.fileName());
    entry << QVariant(fileInfo.size());
    entry << QVariant(fileInfo.lastModified());
    if (fileInfo.isDir())
      entry << QVariant(QString("Folder"));
    else if (fileInfo.suffix().isEmpty())
      entry << QVariant("File");
    else
      entry << QVariant(QString(fileInfo.suffix() + " File"));
    files << QVariant(entry);
  }
  emit updateDirView(QVariant(files));
}

void Interpreter::rescanPath() {
  if (m_disablerescan) return;
  if (!context) return;
  context->flushTemporaryGlobalFunctions();
  for (int i=0;i<m_basePath.size();i++)
    scanDirectory(m_basePath[i],false,"");
  for (int i=0;i<m_userPath.size();i++)
    scanDirectory(m_userPath[i],false,"");
  // Scan the current working directory.
  scanDirectory(QDir::currentPath(),true,"");
  updateFileTool();
}


void Interpreter::setBasePath(QStringList pth) {
  m_basePath = pth;
}

void Interpreter::setUserPath(QStringList pth) {
  m_userPath = pth;
}

static QString mexExtension() {
#ifdef Q_OS_LINUX
  return "fmxglx";
#endif
#ifdef Q_OS_MACX
  return "fmxmac";
#endif
#ifdef Q_OS_WIN32
  return "fmxw32";
#endif
  return "fmx";
}

void Interpreter::scanDirectory(QString scdir, bool tempfunc,
				QString prefix) {
  QDir dir(scdir);
  dir.setFilter(QDir::Files|QDir::Dirs|QDir::NoDotAndDotDot);
  dir.setNameFilters(QStringList() << "*.m" << "*.p"
		     << "@*" << "private" << "*."+mexExtension());
  QFileInfoList list(dir.entryInfoList());
  for (int i=0;i<((int)list.size());i++) {
    QFileInfo fileInfo(list.at(i));
    QString fileSuffix(fileInfo.suffix());
    QString fileBaseName(fileInfo.baseName());
    QString fileAbsoluteFilePath(fileInfo.absoluteFilePath());
    if (fileSuffix == "m" || fileSuffix == "M")
      if (prefix.isEmpty())
	procFileM(fileBaseName,fileAbsoluteFilePath,tempfunc);
      else
	procFileM(prefix + ":" + fileBaseName,fileAbsoluteFilePath,tempfunc);
    else if (fileSuffix == "p" || fileSuffix == "P")
      if (prefix.isEmpty())
	procFileP(fileBaseName,fileAbsoluteFilePath,tempfunc);
      else
	procFileP(prefix + ":" + fileBaseName,fileAbsoluteFilePath,tempfunc);
    else if (fileBaseName[0] == '@')
      scanDirectory(fileAbsoluteFilePath,tempfunc,fileBaseName);
    else if (fileBaseName == "private")
      scanDirectory(fileAbsoluteFilePath,tempfunc,fileAbsoluteFilePath);
    else
      procFileMex(fileBaseName,fileAbsoluteFilePath,tempfunc);
  }
}

void Interpreter::procFileM(QString fname, QString fullname, bool tempfunc) {
  MFunctionDef *adef;
  adef = new MFunctionDef();
  adef->name = fname;
  adef->fileName = fullname;
  adef->temporaryFlag = tempfunc;
  FuncPtr val;
  if (context->lookupFunction(fname,val))
    if (val->type() == FM_BUILT_IN_FUNCTION)
      warningMessage("built in function " + fname + " will be shadowed by the script " + fullname);
  context->insertFunction(adef, tempfunc);
}

void Interpreter::procFileP(QString fname, QString fullname, bool tempfunc) {
  throw Exception("P-files are not supported in this version of FreeMat");
}

void Interpreter::procFileMex(QString fname, QString fullname, bool tempfunc) {
  MexFunctionDef *adef;
  adef = new MexFunctionDef(fullname);
  adef->name = fname;
  if (adef->LoadSuccessful())
    context->insertFunction((MFunctionDef*)adef,tempfunc);
  else
    delete adef;
}

void Interpreter::RegisterGfxResults(ArrayVector m) {
  mutex.lock();
  gfx_buffer.push_back(m);
  gfxBufferNotEmpty.wakeAll();
  mutex.unlock();
}

void Interpreter::RegisterGfxError(QString msg) {
  mutex.lock();
  gfxError = msg;
  gfxErrorOccured = true;
  gfxBufferNotEmpty.wakeAll();
  mutex.unlock();
}

ArrayVector Interpreter::doFunction(FuncPtr f, ArrayVector& m,
				    int narg_out, VariableTable *vtable) {
  CLIDisabler dis(this);
  PopContext saver(context,0);
  context->pushScope(f->functionName(),f->detailedName(),false);
  if (f->graphicsFunction) {
    gfxErrorOccured = false;
    QMutexLocker lock(&mutex);
    emit doGraphicsCall(this,f,m,narg_out);
    if (!gfxErrorOccured && gfx_buffer.empty()) {
      gfxBufferNotEmpty.wait(&mutex);
    } else {
      dbout << "Wha??\n";
    }
    if (gfxErrorOccured) {
      throw Exception(gfxError);
    }
    if (gfx_buffer.empty())
      dbout << "Warning! graphics empty on return\n";
    ArrayVector ret(gfx_buffer.front());
    gfx_buffer.erase(gfx_buffer.begin());
    return ret;
  } else {
    ArrayVector ret(f->evaluateFunc(this,m,narg_out,vtable));
    if (context->scopeStepTrap() >= 1) {
      tracetrap = 1;
      tracecurrentline = 0;
      warningMessage("dbstep beyond end of function " + context->scopeDetailString() +
		     " -- setting single step mode\n");
      context->setScopeStepTrap(0);
    }
    return ret;
  }
}

void Interpreter::setTerminalWidth(int ncols) {
  mutex.lock();
  m_ncols = ncols;
  mutex.unlock();
}

int Interpreter::getTerminalWidth() {
  return m_ncols;
}

QString TranslateString(QString x) {
  return x.replace("\n","\r\n");
}

void Interpreter::diaryMessage(QString msg) {
  QFile file(m_diaryFilename);
  if (file.open(QIODevice::WriteOnly | QIODevice::Append)) {
    QTextStream os(&file);
    os << msg;
  }
}


void Interpreter::outputMessage(QString msg) {
  if (m_diaryState) diaryMessage(msg);
  if (m_captureState)
    m_capture += msg;
  else
    if (m_quietlevel < 2)
      emit outputRawText(TranslateString(msg));
}

void Interpreter::outputMessage(const char* format,...) {
  char buffer[4096];
  va_list ap;
  va_start(ap,format);
  vsnprintf(buffer,4096,format,ap);
  va_end(ap);
  outputMessage(QString(buffer));
}

void Interpreter::errorMessage(QString msg) {
  if (m_diaryState) diaryMessage("Error: " + msg + "\n");
  if (m_captureState)
    m_capture += "Error: " + msg + "\n";
  else
    if (m_quietlevel < 2)
      emit outputRawText(TranslateString("Error: " + msg + "\r\n"));
}

void Interpreter::warningMessage(QString msg) {
  static QString lastWarning;
  static bool lastWarningRepeat = false;
  if (!m_enableWarnings) return;
  if (m_diaryState) diaryMessage("Warning: " + msg + "\n");
  if (m_captureState)
    m_capture += "Warning: " + msg + "\n";
  else
    if (m_quietlevel < 2) {
      if (lastWarning != msg) {
	emit outputRawText(TranslateString("Warning: " +msg + "\r\n"));
	lastWarningRepeat = false;
	lastWarning = msg;
      } else {
	if (!lastWarningRepeat) {
	  emit outputRawText(TranslateString("Warning: Last warning repeats... suppressing more of these\r\n"));
	  lastWarningRepeat = true;
	}
      }
    }
}

static bool isMFile(QString arg) {
  // Not completely right...
  return (((arg[arg.size()-1] == 'm') ||
	   (arg[arg.size()-1] == 'p')) &&
	  (arg[arg.size()-2] == '.'));
}

QString TrimFilename(QString arg) {
  int ndx = arg.lastIndexOf(QDir::separator());
  if (ndx>=0)
    arg.remove(0,ndx+1);
  return arg;
}

QString TrimExtension(QString arg) {
  if (arg.size() > 2 && arg[arg.size()-2] == '.')
    arg.remove(arg.size()-2,arg.size());
  return arg;
}

static QString PrivateMangleName(QString currentFunctionPath, QString fname) {
  if (currentFunctionPath.isEmpty()) return "";
  // First look to see if we are already a private function
  QString separator("/");
  int ndx1 = currentFunctionPath.lastIndexOf(separator + "private" + separator);
  if (ndx1>=0) {
    // The current function is already in a private directory
    // In that case, try to find a private function in the same directory
    currentFunctionPath.remove(ndx1+1,currentFunctionPath.size());
    return currentFunctionPath + "private:" + fname;
  }
  int ndx;
  ndx = currentFunctionPath.lastIndexOf(separator);
  if (ndx>=0)
    currentFunctionPath.remove(ndx+1,currentFunctionPath.size());
  return currentFunctionPath + "private:" + fname;
}

static QString LocalMangleName(QString currentFunctionPath, QString fname) {
  int ndx = currentFunctionPath.lastIndexOf("/");
  if (ndx >= 0)
    currentFunctionPath.remove(ndx,currentFunctionPath.size());
  QString tmp = currentFunctionPath + "/" + fname;
  return currentFunctionPath + "/" + fname;
}

static QString NestedMangleName(QString cfunc, QString fname) {
  return cfunc + "/" + fname;
}

QString Interpreter::getVersionString() {
  return QString("FreeMat v" FREEMAT_VERSION);
}

// Run the thread function
void Interpreter::run() {
  if (m_threadFunc) {
    try {
      m_threadFuncRets = doFunction(m_threadFunc,m_threadFuncArgs,m_threadNargout);
    } catch (Exception &e) {
      m_threadErrorState = true;
      lasterr = e.msg();
    } catch (InterpreterQuitException &e) {
      m_threadErrorState = true;
      lasterr = "'quit' called in non-main thread";
    } catch (InterpreterKillException &e) {
      m_kill = false;
    } catch (InterpreterRetallException &e) {
    } catch (exception& e) {
      m_threadErrorState = true;
      lasterr = "thread crashed!! - you have encountered a bug in FreeMat - please file bug report describing what happened";
    } catch (...) {
      m_threadErrorState = true;
      lasterr = "thread crashed!! - you have encountered a bug in FreeMat - please file bug report describing what happened";
    }
  }
}

void Interpreter::doCLI() {
  //  rescanPath();
  emit CWDChanged(QDir::currentPath());
  updateFileTool();
  if (!m_skipflag)
    sendGreeting();
  try {
    while (1) {
      int scope_stackdepth = context->scopeDepth();
      try {
	evalCLI();
      } catch (InterpreterRetallException) {
      } catch (InterpreterReturnException &e) {
      }
      while (context->scopeDepth() > scope_stackdepth) context->popScope();
    }
  } catch (InterpreterQuitException &e) {
    emit QuitSignal();
  } catch (std::exception& e) {
    qDebug() << "crash: " << e.what();
    emit CrashedSignal();
  } catch (...) {
    emit CrashedSignal();
  }
}

void Interpreter::sendGreeting() {
  outputMessage(" " + getVersionString() + "\n");
  outputMessage(" Copyright (c) 2002-2008 by Samit Basu\n");
  outputMessage(" Licensed under the GNU Public License (GPL)\n");
  outputMessage(" Type <help license> to find out more\n");
  outputMessage("      <helpwin> for online help\n");
  outputMessage("      <pathtool> to set or change your path\n");
  outputMessage(" Use <dbauto on/off> to control stop-on-error behavior\n");
  outputMessage(" Use ctrl-b to stop execution of a function/script\n");
  outputMessage(" JIT is enabled by default - use jitcontrol to change it\n");
  outputMessage(" Use <rootpath gui> to set/change where the FreeMat toolbox is installed\n");
  outputMessage("");
}

bool Interpreter::inMFile() const {
  return (isMFile(context->activeScopeName()));
}

void Interpreter::debugDump() {
  int depth = context->scopeDepth();
  qDebug() << "******************************\n";
  for (int i=0;i<depth;i++) {
    if (context->isScopeActive())
      qDebug() << "In " << context->scopeName() << " (" << context->scopeDetailString() << ")*";
    else
      qDebug() << "In " << context->scopeName() << " (" << context->scopeDetailString() << ")";
    context->bypassScope(1);
  }
  context->restoreScope(depth);
}

//DOCBLOCK debug_dbup
void Interpreter::dbup() {
  // The stack should look like --
  // base, foo, keyboard, dbup
  // so to do a dbup, we have to save the top two of the
  // stack, move foo to the backup stack, and then restore
  //
  // Consider the following.  The stack looks like this:
  // main: base foo1 keyboard foo2 keyboard dbup
  // bypass: <empty>
  //
  // Now we do a dbup
  // main: base foo1 keyboard keyboard dbup
  // bypass foo2
  //
  // Suppose we
  // We need the "keyboard" states on the stack because they
  // capture the context updates for the command line routines.
  //
  if (InSpecificScope(context,"docli","builtin"))
    return;
  context->reserveScope();
  while (InKeyboardScope(context))
    context->bypassScope(1);
  if (!InSpecificScope(context,"base","base") &&
      !InSpecificScope(context,"docli","builtin")) {
    // Bypass a single non-keyboard context
    context->bypassScope(1);
  }
  while (InKeyboardScope(context))
    context->bypassScope(1);
  context->unreserveScope();
}

//DOCBLOCK debug_dbdown
void Interpreter::dbdown() {
  // Save the one for the "dbdown" command
  // Save the one for the "keyboard" command that we are currently in
  context->reserveScope();
  // Restore until we get a non-"keyboard" scope
  context->restoreScope(1);
  while (InKeyboardScope(context))
    context->restoreScope(1);
  context->unreserveScope();
  dbdown_executed = true;
}

QString Interpreter::getLocalMangledName(QString fname) {
  QString ret;
  if (isMFile(context->activeScopeName()))
    ret = LocalMangleName(context->activeScopeDetailString(),fname);
  else
    ret = fname;
  return ret;
}

QString Interpreter::getPrivateMangledName(QString fname) {
  QString ret;
  if (isMFile(context->scopeName()))
    ret = PrivateMangleName(context->scopeName(),fname);
  else {
    ret = QDir::currentPath() +
      QString(QDir::separator()) +
      QString("private:" + fname);
  }
  return ret;
}

QString Interpreter::getMFileName() {
  if (isMFile(context->scopeName()))
    return TrimFilename(TrimExtension(context->scopeName()));
  // TESTME
  //   for (int i=cstack.size()-1;i>=0;i--)
  //     if (isMFile(cstack[i].cname))
  //       return TrimFilename(TrimExtension(cstack[i].cname));
  return QString("");
}

// called by editor
QString Interpreter::getInstructionPointerFileName() {
  if (!InCLI) return QString("");
  ParentScopeLocker lock(context);
  QString filename(context->scopeName());
  if (isMFile(filename))
    return filename;
  return QString("");
}

Array Interpreter::DoBinaryOperator(const Tree & t, BinaryFunc fnc,
				    QString funcname) {
  Array a(expression(t.first()));
  Array b(expression(t.second()));
  if (!(a.isUserClass() || b.isUserClass()))
    return fnc(a,b);
  return ClassBinaryOperator(a,b,funcname,this);
}

Array Interpreter::DoUnaryOperator(const Tree & t, UnaryFunc fnc,
				   QString funcname) {
  Array a(expression(t.first()));
  if (!a.isUserClass())
    return fnc(a);
  return ClassUnaryOperator(a,funcname,this);
}

void Interpreter::setPrintLimit(int lim) {
  printLimit = lim;
}

int Interpreter::getPrintLimit() {
  return(printLimit);
}

//DOCBLOCK variables_matrix
//Works
Array Interpreter::matrixDefinition(const Tree & t) {
  ArrayMatrix m;
  if (t.numChildren() == 0)
    return EmptyConstructor();
  for (int i=0;i<t.numChildren();i++) {
    const Tree & s(t.child(i));
    ArrayVector n;
    for (int j=0;j<s.numChildren();j++)
      multiexpr(s.child(j),n);
    m.push_back(n);
  }
  // Check if any of the elements are user defined classes
  bool anyuser = false;
  for (int i=0;i<m.size() && !anyuser;i++)
    for (int j=0;j<m[i].size() && !anyuser;j++)
      if (m[i][j].isUserClass())
	anyuser = true;
  if (!anyuser)
    return MatrixConstructor(m);
  else
    return ClassMatrixConstructor(m,this);
}

//DOCBLOCK variables_cell
//Works
Array Interpreter::cellDefinition(const Tree & t) {
  ArrayMatrix m;
  if (t.numChildren() == 0)
    return Array(CellArray);
  for (int i=0;i<t.numChildren();i++) {
    const Tree & s(t.child(i));
    ArrayVector n;
    for (int j=0;j<s.numChildren();j++)
      multiexpr(s.child(j),n);
    m.push_back(n);
  }
  return CellConstructor(m);
}

Array Interpreter::ShortCutOr(const Tree &  t) {
  Array a(expression(t.first()));
  Array retval;
  if (!a.isScalar())
    retval = DoBinaryOperator(t,Or,"or");
  else {
    // A is a scalar - is it true?
    if (a.toClass(Bool).constRealScalar<bool>())
      retval = a.toClass(Bool);
    else
      retval = DoBinaryOperator(t,Or,"or");
  }
  return retval;
}

Array Interpreter::ShortCutAnd(const Tree & t) {
  context->setScopeTokenID(t.context());
  Array a(expression(t.first()));
  context->setScopeTokenID(t.context());
  Array retval;
  if (!a.isScalar()) {
    retval = DoBinaryOperator(t,And,"and");
  } else {
    // A is a scalar - is it false?
    if (!a.toClass(Bool).constRealScalar<bool>())
      retval = a.toClass(Bool);
    else
      retval = DoBinaryOperator(t,And,"and");
  }
  return retval;
}

//Works
// Need to take care

ArrayVector Interpreter::handleReindexing(const Tree & t, const ArrayVector &p) {
  if (t.numChildren() > 2)
    if (p.size() > 1)
      throw Exception("reindexing of function expressions not allowed when multiple values are returned by the function");
    else {
      Array r;
      if (p.size() == 1)
	r = p[0];
      else
	r = EmptyConstructor();
      for (int index = 2;index < t.numChildren();index++)
	deref(r,t.child(index));
      return ArrayVector() << r;
    }
  else
    return p;
}

void Interpreter::multiexpr(const Tree & t, ArrayVector &q, index_t lhsCount, bool output_optional) {
  if (t.is(TOK_VARIABLE)) {
    ArrayReference ptr(context->lookupVariable(t.first().text()));
    if (!ptr.valid()) {
      ArrayVector p;
      functionExpression(t,int(lhsCount),output_optional,p);
      q += handleReindexing(t,p);
      return;
    }
    if (t.numChildren() == 1) {
      q.push_back(*ptr);
      return;
    }
    if (ptr->isUserClass() && !stopoverload) {
      q += ClassRHSExpression(*ptr,t,this);
      return;
    }
    Array r(*ptr);
    for (int index = 1;index < t.numChildren()-1;index++)
      deref(r,t.child(index));
    SaveEndInfo;
    endRef = &r;
    const Tree & s(t.last());
    if (s.is(TOK_PARENS)) {
      ArrayVector m;
      endTotal = s.numChildren();
      if (s.numChildren() == 0)
	q.push_back(r);
      else {
	for (int p = 0;p < s.numChildren(); p++) {
	  endCount = m.size();
	  multiexpr(s.child(p),m);
	}
	subsindex(m);
	if (m.size() == 1)
	  q.push_back(r.get(m.front()));
	else
	  q.push_back(r.get(m));
      }
    } else if (s.is(TOK_BRACES)) {
      ArrayVector m;
      endTotal = s.numChildren();
      for (int p = 0;p < s.numChildren(); p++) {
	endCount = m.size();
	multiexpr(s.child(p),m);
      }
      subsindex(m);
      if (m.size() == 1)
	q += ArrayVectorFromCellArray(r.get(m.front()));
      else
	q += ArrayVectorFromCellArray(r.get(m));
    } else if (s.is('.')) {
      q += r.get(s.first().text());
    } else if (s.is(TOK_DYN)) {
      QString field;
      try {
	Array fname(expression(s.first()));
	field = fname.asString();
      } catch (Exception &e) {
	throw Exception("dynamic field reference to structure requires a string argument");
      }
      q += r.get(field);
    }
    RestoreEndInfo;
  } else if (!t.is(TOK_KEYWORD))
    q.push_back(expression(t));
}

Array Interpreter::expression(const Tree & t) {
  switch(t.token()) {
  case TOK_VARIABLE:
    return rhs(t);
  case TOK_REAL:
  case TOK_IMAG:
  case TOK_REALF:
  case TOK_IMAGF:
  case TOK_STRING:
    return t.array();
  case TOK_REINDEX:
    {
      Array r = expression(t.first());
      for (int index = 1;index < t.numChildren();index++)
	deref(r,t.child(index));
      return r;
    }
  case TOK_INCR_PREFIX:
    {
      Array dummy = rhs(t.first());
      Array ret = Add(dummy,Array((double)(1)));
      assignment(t.first(),false,ret);
      return ret;
    }
  case TOK_DECR_PREFIX:
    {
      Array dummy = rhs(t.first());
      Array ret = Subtract(dummy,Array((double)(1)));
      assignment(t.first(),false,ret);
      return ret;
    }
  case TOK_INCR_POSTFIX:
    {
      Array dummy = rhs(t.first());
      Array ret = Add(dummy,Array((double)(1)));
      assignment(t.first(),false,ret);
      return dummy;
    }
  case TOK_DECR_POSTFIX:
    {
      Array dummy = rhs(t.first());
      Array ret = Subtract(dummy,Array((double)(1)));
      assignment(t.first(),false,ret);
      return dummy;
    }
  case TOK_END:
    if (!endRef.valid())
      throw Exception("END keyword not allowed for undefined variables");
    if (endTotal == 1)
      return Array(double(endRef->length()));
    else
      return Array(double(endRef->dimensions()[endCount]));
  case ':':
    if (t.numChildren() == 0) {
      return Array(QString(":"));
    } else if (t.first().is(':')) {
      return doubleColon(t);
    } else {
      return unitColon(t);
    }
    break;
  case TOK_MATDEF:
    return matrixDefinition(t);
    break;
  case TOK_CELLDEF:
    return cellDefinition(t);
    break;
  case '+':
    return DoBinaryOperator(t,Add,"plus");
    break;
  case '-':
    return DoBinaryOperator(t,Subtract,"minus");
    break;
  case '*':
    return DoBinaryOperator(t,Multiply,"mtimes");
    break;
  case '/':
    return DoBinaryOperator(t,RightDivide,"mrdivide");
    break;
  case '\\':
    return DoBinaryOperator(t,LeftDivide,"mldivide");
    break;
  case TOK_SOR:
    return ShortCutOr(t);
    break;
  case '|':
    return DoBinaryOperator(t,Or,"or");
    break;
  case TOK_SAND:
    return ShortCutAnd(t);
    break;
  case '&':
    return DoBinaryOperator(t,And,"and");
  case '<':
    return DoBinaryOperator(t,LessThan,"lt");
    break;
  case TOK_LE:
    return DoBinaryOperator(t,LessEquals,"le");
    break;
  case '>':
    return DoBinaryOperator(t,GreaterThan,"gt");
    break;
  case TOK_GE:
    return DoBinaryOperator(t,GreaterEquals,"ge");
    break;
  case TOK_EQ:
    return DoBinaryOperator(t,Equals,"eq");
    break;
  case TOK_NE:
    return DoBinaryOperator(t,NotEquals,"ne");
    break;
  case TOK_DOTTIMES:
    return DoBinaryOperator(t,DotMultiply,"times");
    break;
  case TOK_DOTRDIV:
    return DoBinaryOperator(t,DotRightDivide,"rdivide");
    break;
  case TOK_DOTLDIV:
    return DoBinaryOperator(t,DotLeftDivide,"ldivide");
    break;
  case TOK_UNARY_MINUS:
    return DoUnaryOperator(t,Negate,"uminus");
    break;
  case TOK_UNARY_PLUS:
    return DoUnaryOperator(t,Plus,"uplus");
    break;
  case '~':
    return DoUnaryOperator(t,Not,"not");
    break;
  case '^':
    return DoBinaryOperator(t,Power,"mpower");
    break;
  case TOK_DOTPOWER:
    return DoBinaryOperator(t,DotPower,"power");
    break;
  case '\'':
    return DoUnaryOperator(t,Hermitian,"ctranspose");
    break;
  case TOK_DOTTRANSPOSE:
    return DoUnaryOperator(t,Transpose,"transpose");
    break;
  case '@':
    return FunctionPointer(t);
  default:
    throw Exception("Unrecognized expression!");
  }
}

Array Interpreter::FunctionPointer(const Tree & t) {
   if (t.first().is(TOK_ANONYMOUS_FUNC)) {
     return AnonFuncConstructor(this,t.first());
   } else {
     FuncPtr val;
     if (!lookupFunction(t.first().text(),val))
       throw Exception("unable to resolve " + t.first().text() +
		       " to a function call");
     return FuncPtrConstructor(this,val);
   }
}

//DOCBLOCK operators_colon
//Works
Array Interpreter::unitColon(const Tree & t) {
  Array a, b;
  a = expression(t.first());
  b = expression(t.second());
  if (!(a.isUserClass() || b.isUserClass()))
    return UnitColon(a,b);
  else
    return ClassBinaryOperator(a,b,"colon",this);
}

void Interpreter::deleteHandleClass(StructArray *ap)
{
  // We need to call the destructor on
  Array b(*ap);
  delete ap;
  FuncPtr val;
  if (b.isUserClass() && ClassResolveFunction(this,b,"delete",val))
    {
      val->updateCode(this);
      ArrayVector args(b);
      doFunction(val,args,1);
    }
}

//Works
Array Interpreter::doubleColon(const Tree & t) {
  Array a, b, c;
  a = expression(t.first().first());
  b = expression(t.first().second());
  c = expression(t.second());
  if (!(a.isUserClass() || b.isUserClass() || c.isUserClass()))
    return DoubleColon(a,b,c);
  else
    return ClassTrinaryOperator(a,b,c,"colon",this);
}

/**
 * This somewhat strange test is used by the switch statement.
 * If x is a scalar, and we are a scalar, this is an equality
 * test.  If x is a string and we are a string, this is a
 * strcmp test.  If x is a scalar and we are a cell-array, this
 * test is applied on an element-by-element basis, looking for
 * any matches.  If x is a string and we are a cell-array, then
 * this is applied on an element-by-element basis also.
 */
bool Interpreter::testCaseStatement(const Tree & t, Array s) {
  Array r(expression(t.first()));
  bool caseMatched = TestForCaseMatch(s,r);
  if (caseMatched)
    block(t.second());
  return caseMatched;
}

//DOCBLOCK flow_try
//Works
void Interpreter::tryStatement(const Tree & t) {
  // Turn off autostop for this statement block
  bool autostop_save = autostop;
  autostop = false;
  bool intryblock_save = intryblock;
  intryblock = true;
  // Get the state of the IDnum stack and the
  // contextStack and the cnameStack
  int stackdepth = context->scopeDepth();
  try {
    block(t.first());
  } catch (Exception &e) {
    while (context->scopeDepth() > stackdepth) context->popScope();
    if (t.numChildren()>1) {
      autostop = autostop_save;
      block(t.second().first());
    }
  }
  autostop = autostop_save;
  intryblock = intryblock_save;
}


//DOCBLOCK flow_switch
//Works
void Interpreter::switchStatement(const Tree & t) {
  Array switchVal;
  // First, extract the value to perform the switch on.
  switchVal = expression(t.first());
  // Assess its type to determine if this is a scalar switch
  // or a string switch.
  if (!switchVal.isScalar() && !switchVal.isString())
    throw Exception("Switch statements support scalar and string arguments only.");
  int n=1;
  while (n < t.numChildren() && t.child(n).is(TOK_CASE)) {
    if (testCaseStatement(t.child(n),switchVal))
      return;
    n++;
  }
  if (t.last().is(TOK_OTHERWISE))
    block(t.last().first());
}

//DOCBLOCK flow_if
//Works
void Interpreter::ifStatement(const Tree & t) {
  bool condtest = RealAllNonZeros(expression(t.first()));
  if (condtest) {
    block(t.second());
    return;
  } else {
    int n=2;
    while (n < t.numChildren() && t.child(n).is(TOK_ELSEIF)) {
      if (RealAllNonZeros(expression(t.child(n).first()))) {
	block(t.child(n).second());
	return;
      }
      n++;
    }
  }
  if (t.last().is(TOK_ELSE))
    block(t.last().first());
}

static bool compileJITBlock(Interpreter *interp, const Tree & t, JITInfo & ref, JITControlFlag jitflag) {
  delete ref.JITFunction();
  ref.setJITState(JITInfo::FAILED);
  JITFuncBase *cg = JITFactory::GetJITFunc(interp);
  if (!cg) return false;
  bool success = false;
  try {
    if (!cg->compile(t,jitflag))
      {
	delete cg;
	success = false;
	ref.setJITState(JITInfo::FAILED);
	return success;
      }
    success = true;
    ref.setJITState(JITInfo::SUCCEEDED);
    ref.setJITFunction(cg);
    interp->incrementJITCounter();
    dbout << "Block JIT compiled at line "
	  << LineNumber(interp->getContext()->scopeTokenID())
	  << " of " << interp->getContext()->scopeName() << "\n";
  } catch (Exception &e) {
    dbout << "JIT compile failed:" << e.msg() << " at line "
      	  << LineNumber(interp->getContext()->scopeTokenID())
	  << " of " << interp->getContext()->scopeName() << "\n";
    delete cg;
    success = false;
    ref.setJITState(JITInfo::FAILED);
  }
  return success;
}

bool Interpreter::tryJitCode(const Tree & t) {
    // Try to compile this block to an instruction stream
    if (jitcontrol) {
        int UID = t.node().UID();
        JITInfo & ref = m_codesegments[UID];
        try{
            if (ref.JITState() == JITInfo::UNTRIED) {
                bool success = compileJITBlock(this,t,ref,jitcontrol);
                if (success)
                {
                    if (ref.JITFunction()->run() == CJIT_Success)
                    {
                        ref.setJITState(JITInfo::SUCCEEDED);
                        return true;
                    }
                    ref.setJITState(JITInfo::FAILED);
                    return false;
                }
            } else if (ref.JITState() == JITInfo::SUCCEEDED) {
                int stat = ref.JITFunction()->run();
                if (stat == CJIT_Success)
                    return true;
                // If the prep stage failed, we can try to recompile
                dbout << "Prep failed for JIT block retrying\n";
                if (stat == CJIT_Prepfail)
                {
                    bool success = compileJITBlock(this,t,ref,jitcontrol);
                    if (success)
                    {
                        if (ref.JITFunction()->run() == CJIT_Success)
			  {
                            return true;
			  }
                    }
                }
            }
        }catch(Exception &e){
            errorMessage(e.msg());
            //errorMessage("Fatal Error. Please restart FreeMat.");
        }

        ref.setJITState(JITInfo::FAILED);
    }
    return false;
}

//DOCBLOCK flow_while
//Works
void Interpreter::whileStatement(const Tree & t) {
  if (tryJitCode(t)) return;
  const Tree & testCondition(t.first());
  const Tree & codeBlock(t.second());
  bool breakEncountered = false;
  Array condVar(expression(testCondition));
  bool conditionTrue = RealAllNonZeros(condVar);
  context->enterLoop();
  breakEncountered = false;
  while (conditionTrue && !breakEncountered) {
    try {
      block(codeBlock);
    } catch (InterpreterContinueException& e) {
    } catch (InterpreterBreakException& e) {
      breakEncountered = true;
    } catch (InterpreterReturnException& e) {
      context->exitLoop();
      throw;
    } catch (InterpreterRetallException& e) {
      context->exitLoop();
      throw;
    }
    if (!breakEncountered) {
      condVar = expression(testCondition);
      conditionTrue = RealAllNonZeros(condVar);
    }
  }
  context->exitLoop();
}

//Helper functions for FOR loops.  This template function
//handles the index variable with the correct type.  Reducing
//the net loop time

class ContextLoopLocker {
  Context* m_context;
public:
  ContextLoopLocker(Context* a): m_context(a) {m_context->enterLoop();}
  ~ContextLoopLocker() {m_context->exitLoop();}
};

inline bool IsIntegerDataClass( const Array& a )
{
    return (a.dataClass() >= Int8) && (a.dataClass() <= UInt64);
}

template <class T>
void ForLoopHelper(const Tree & codeBlock, const Array& indexSet,
		   index_t count, const QString& indexName, Interpreter *eval) {
  for (index_t m=1;m<=count;m++) {
    Array *vp = eval->getContext()->lookupVariableLocally( indexName );
    if ((!vp) || (!vp->isScalar())) {
	eval->getContext()->insertVariableLocally(indexName,Array());
	vp = eval->getContext()->lookupVariableLocally(indexName);
    }

    *vp = indexSet.get(m);
    try {
      eval->block(codeBlock);
    } catch (InterpreterContinueException &) {
    } catch (InterpreterBreakException &) {
      break;
    }
  }
}

void ForLoopIterator( const Tree & codeBlock, QString indexName,
		     Array& first, Array& last, Array& step, Interpreter *eval)
{
    int nsteps;

    if( !( first.isScalar() && last.isScalar() && step.isScalar() ) )
	throw Exception("Loop parameters must be scalar.");

    Array *vp = eval->getContext()->lookupVariableLocally( indexName );
    if ((!vp) || (!vp->isScalar())) {
	eval->getContext()->insertVariableLocally(indexName,Array());
	vp = eval->getContext()->lookupVariableLocally(indexName);
    }

    bool bIntLoop = (IsIntegerDataClass(first) || IsIntegerDataClass(last) || IsIntegerDataClass(step));

    if( bIntLoop ){
	//integer loop path
	Array temp1;

	temp1 = DotRightDivide( Subtract( last, first ), step );    //( ( l - f ) / s )
	nsteps = temp1.asInteger() + 1;	    //( ( l - f ) / s )+1
	if( nsteps < 0 ) return;

	for (int m=0;m<nsteps;m++) {
	    *vp = Add( first, DotMultiply( Array(m), step ) );
	    try {
		eval->block(codeBlock);
	    }
	    catch (InterpreterContinueException &e) {
	    }
	    catch (InterpreterBreakException &e) {
		break;
	    }
	}
    }
    else{
	//floating point loop path
	bool bFloatLoop = ( first.dataClass() == Float || last.dataClass() == Float || step.dataClass() == Float );
	double f = first.asDouble();
	double l = last.asDouble();
	double s = step.asDouble();

	if( bFloatLoop )
	    nsteps = num_for_loop_iter_f(f, s, l);
	else
	    nsteps = num_for_loop_iter(f, s, l);

	for (double m=0;m<nsteps;m++) { //array variable should be of type double for correct typing of DotMultiply
	    *vp = Add( first, DotMultiply( Array(m), step ) );
	    try {
		eval->block(codeBlock);
	    }
	    catch (InterpreterContinueException &) {}
	    catch (InterpreterBreakException &) {
		break;
	    }
	}
    }
}

extern "C" EXPORT
float num_for_loop_iter_f( float first, float step, float last )
{
    int signum = (step > 0) - (step < 0);
    int nsteps = (int) floor( ( last - first ) / step ) + 1;
    if( nsteps < 0 )
	return 0;

    float mismatch = signum*(first + nsteps*step - last);
    if( (mismatch > 0) && ( mismatch < 3.*fepsf(last) ) && ( step != rint(step) ) ) //allow overshoot by 3 eps in some cases
	nsteps++;

    return nsteps;
}

extern "C" EXPORT
double num_for_loop_iter( double first, double step, double last )
{
    int signum = (step > 0) - (step < 0);
    int nsteps = (int) floor( ( last - first ) / step ) + 1;
    if( nsteps < 0 )
	return 0;

    double mismatch = signum*(first + nsteps*step - last);
    if( (mismatch > 0) && ( mismatch < 3.*feps(last) ) && ( step != rint(step) ) ) //allow overshoot by 3 eps in some cases
	nsteps++;

    return nsteps;
}

//DOCBLOCK flow_for
//Works

void Interpreter::forStatement(const Tree & t) {
  if (tryJitCode(t)) return;
  Array indexSet;
  QString indexVarName;
    /* Get the name of the indexing variable */
    if( !t.first().is('=') )
	throw Exception( "Incorrect format of for operator" );

    indexVarName = t.first().first().text();

    if( t.first().second().is(TOK_MATDEF) ||
	t.first().second().is(TOK_VARIABLE) )   {
	  //case "for j=[1:10]"
	  //case "for j=K" skb
    	/* Evaluate the index set */
	indexSet = expression(t.first().second());

	/* Get the code block */
	const Tree & codeBlock(t.second());
	index_t elementCount = indexSet.length();
	DataClass loopType(indexSet.dataClass());
	ContextLoopLocker lock(context);
	switch(loopType) {
	  case Invalid:
  	      throw Exception("Invalid arrays not supported");
  	  case Struct:
	      throw Exception("Structure arrays are not supported as for loop index sets");
	  case CellArray:
	      ForLoopHelper<Array>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Bool:
	      ForLoopHelper<logical>(codeBlock, indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case UInt8:
	      ForLoopHelper<uint8>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Int8:
	      ForLoopHelper<int8>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case UInt16:
	      ForLoopHelper<uint16>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Int16:
	      ForLoopHelper<int16>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case UInt32:
	      ForLoopHelper<uint32>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Int32:
	      ForLoopHelper<int32>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case UInt64:
	      ForLoopHelper<uint64>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Int64:
	      ForLoopHelper<int64>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Float:
	      ForLoopHelper<float>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case Double:
	      ForLoopHelper<double>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	  case StringArray:
	      ForLoopHelper<uint8>(codeBlock,indexSet,
		  elementCount,indexVarName,this);
	      break;
	}
    }
    else if( t.first().second().token() == ':' ){
	if (t.first().second().numChildren() == 0)
	    throw Exception( "Incorrect format of loop operator parameters" );

	Array first, step, last;
	const Tree & codeBlock(t.second());
	ContextLoopLocker lock(context);

	if (t.first().second().first().is(':')) {
	  first = expression(t.first().second().first().first());
	  step = expression(t.first().second().first().second());
	  last = expression(t.first().second().second());
	  ForLoopIterator( codeBlock, indexVarName, first, last, step, this);
	    //return doubleColon(t);
	}
	else {
	  first = expression(t.first().second().first());
	  last = expression(t.first().second().second());
	  Array tmp(BasicArray<double>(1));
	  ForLoopIterator( codeBlock, indexVarName, first, last, tmp, this);
	    //return unitColon(t);
	}
    }
}

//DOCBLOCK variables_global
void Interpreter::globalStatement(const Tree & t) {
  for (int i=0;i<t.numChildren();i++) {
    QString name = t.child(i).text();
    context->addGlobalVariable(name);
    if (!context->lookupVariable(name).valid())
      context->insertVariable(name,EmptyConstructor());
  }
}

//DOCBLOCK variables_persistent
void Interpreter::persistentStatement(const Tree & t) {
  for (int i=0;i<t.numChildren();i++) {
    QString name = t.child(i).text();
    context->addPersistentVariable(name);
    if (!context->lookupVariable(name).valid())
      context->insertVariable(name,EmptyConstructor());
  }
}

//DOCBLOCK flow_continue

//DOCBLOCK flow_break

//DOCBLOCK flow_return

//DOCBLOCK freemat_quit

//DOCBLOCK flow_retall

//DOCBLOCK flow_keyboard

void Interpreter::doDebugCycle() {
  depth++;
  PopContext saver(context,0);
  {
    context->pushScope("keyboard","keyboard");
    PopContext saver2(context,0);
    context->setScopeActive(false);
    try {
      evalCLI();
    } catch (InterpreterContinueException& e) {
    } catch (InterpreterBreakException& e) {
    } catch (InterpreterReturnException& e) {
    } catch (InterpreterRetallException& e) {
      depth--;
      throw;
    }
  }
  depth--;
}

void Interpreter::displayArray(Array b) {
  // Check for a user defined class
  FuncPtr val;
  if (b.isUserClass() && ClassResolveFunction(this,b,"display",val)) {
    if (val->updateCode(this)) refreshBreakpoints();
    ArrayVector args(b);
    ArrayVector retvec(doFunction(val,args,1));
  } else
    PrintArrayClassic(b,printLimit,this);
}

//Works
void Interpreter::expressionStatement(const Tree & s, bool printIt) {
  ArrayVector m;
  if (!s.is(TOK_EXPR)) throw Exception("Unexpected statement type!");
  const Tree & t(s.first());
  // There is a special case to consider here - when a
  // function call is made as a statement, we do not require
  // that the function have an output.
  Array b;
  ArrayReference ptr;
  if (t.is(TOK_VARIABLE)) {
    ptr = context->lookupVariable(t.first().text());
    if (!ptr.valid()) {
      functionExpression(t,0,true,m);
      m = handleReindexing(t,m);
      bool emptyOutput = false;
      if (m.size() == 0) {
	b = EmptyConstructor();
	emptyOutput = true;
      } else
	b = m[0];
      if (printIt && (!emptyOutput)) {
	outputMessage(QString("\nans = \n"));
	displayArray(b);
      }
    } else {
      multiexpr(t,m);
      if (m.size() == 0)
	b = EmptyConstructor();
      else {
	b = m[0];
	if (printIt) {
	  outputMessage(QString("\nans = \n"));
	  for (int j=0;j<m.size();j++) {
	    char buffer[1000];
	    if (m.size() > 1) {
	      sprintf(buffer,"\n%d of %d:\n",j+1,m.size());
	      outputMessage(QString(buffer));
	    }
	    displayArray(m[j]);
	  }
	}
      }
    }
  } else {
    b = expression(t);
    if (printIt) {
      outputMessage(QString("\nans = \n"));
      displayArray(b);
    }
  }
  context->insertVariable("ans",b);
}

void Interpreter::multiassign(ArrayReference r, const Tree & s, ArrayVector &data) {
  SaveEndInfo;
  endRef = r;
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numChildren();
    if (s.numChildren() == 0)
      throw Exception("The expression A() is not legal unless A is a function");
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r->set(m[0],data[0]);
    else
      r->set(m,data[0]);
    data.pop_front();
  } else if (s.is(TOK_BRACES)) {
    ArrayVector m;
    endTotal = s.numChildren();
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      SetCellContents(*r,m[0],data);
    else
      SetCellContents(*r,m,data);
  } else if (s.is('.')) {
    r->set(s.first().text(),data);
  } else if (s.is(TOK_DYN)) {
    QString field;
    try {
      Array fname(expression(s.first()));
      field = fname.asString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    r->set(field,data);
  }
  RestoreEndInfo;
}

void Interpreter::assign(ArrayReference r, const Tree & s, Array &data) {
  SaveEndInfo;
  endRef = r;
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numChildren();
    if (s.numChildren() == 0)
      throw Exception("The expression A() is not legal unless A is a function");
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r->set(m[0],data);
    else
      r->set(m,data);
  } else if (s.is(TOK_BRACES)) {
    ArrayVector datav(data);
    ArrayVector m;
    endTotal = s.numChildren();
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      SetCellContents(*r,m[0],datav);
    else
      SetCellContents(*r,m,datav);
  } else if (s.is('.')) {
    ArrayVector datav(data);
    r->set(s.first().text(),datav);
  } else if (s.is(TOK_DYN)) {
    QString field;
    try {
      Array fname(expression(s.first()));
      field = fname.asString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    ArrayVector datav(data);
    r->set(field,datav);
  }
  RestoreEndInfo;
}


ArrayReference Interpreter::createVariable(QString name) {
  FuncPtr p;
  PopContext saver(context,0);
  // This is annoying.
  //  if (context->lookupFunction(name,p) && (name.size() > 1))
  //    warningMessage("Newly defined variable " + name + " shadows a function of the same name.  Use clear " + name + " to recover access to the function");
  // Are we in a nested scope?
  if (!context->isCurrentScopeNested() || context->variableLocalToCurrentScope(name)) {
    // if not, just create a local variable in the current scope, and move on.
    context->insertVariable(name,EmptyConstructor());
  } else {
    // if so - walk up the scope chain until we are no longer nested
    QString localScopeName = context->scopeName();
    context->bypassScope(1);
    while (context->currentScopeNests(localScopeName))
      context->bypassScope(1);
    context->restoreScope(1);
    // We wre now pointing to the highest scope that contains the present
    // (nested) scope.  Now, we start walking down the chain looking for
    // someone who accesses this variable
    while (!context->currentScopeVariableAccessed(name) &&
	   context->scopeName() != localScopeName)
      context->restoreScope(1);
    // Either we are back in the local scope, or we are pointing to
    // a scope that (at least theoretically) accesses a variable with
    // the given name.
    context->insertVariable(name,EmptyConstructor());
  }
  ArrayReference np(context->lookupVariable(name));
  if (!np.valid())
    throw Exception("error creating variable name " + name +
		    " with scope " + context->scopeName());
  return np;
}

//Works
    // The case of a(1,2).foo.goo{3} = rhs
    // The tree looks like this:
    // Variable
    //    Identifier
    //    ()
    //      etc
    //    .
    //      foo
    //    .
    //      goo
    //    {}
    //      3
    //
    // We have to do:
    //   a1 = id       data = id
    //   a2 = id(etc)  stack[0] = id(etc)
    //   a3 = a2.foo   stack[1] = stack[0].foo
    //   a4 = a3.goo   stack[2] = stack[1].goo
    //   a3{3} = rhs   data{3} = rhs
    //   a2.foo = a3
    //   id(etc) = a2;

//DOCBLOCK array_assign
void Interpreter::assignment(const Tree & var, bool printIt, Array &b) {
  QString name(var.first().text());
  ArrayReference ptr(context->lookupVariable(name));
  if (!ptr.valid())
    ptr = createVariable(name);
  if (var.numChildren() == 1) {
    (*ptr) = b;
  } else if (ptr->isUserClass() &&
	     !inMethodCall(ptr->className()) &&
	     !stopoverload) {
    ClassAssignExpression(ptr,var,b,this);
  } else if (var.numChildren() == 2)
    assign(ptr,var.second(),b);
  else {
    ArrayVector stack;
    Array data(*ptr);
    int varCount = var.numChildren();
    for (int index=1;index<varCount-1;index++) {
      if (!data.isEmpty()) {
	try {
	  deref(data,var.child(index));
	} catch (Exception &e) {
	  data = EmptyConstructor();
	}
      }
      stack.push_back(data);
    }
    assign(&data,var.child(varCount-1),b);
    Array rhs(data);
    if (stack.size() > 0) {
      stack.pop_back();
      int ptr = 0;
      while (stack.size() > 0) {
	data = stack.back();
	assign(&data,var.child(varCount-2-ptr),rhs);
	rhs = data;
	stack.pop_back();
	ptr++;
      }
    }
    assign(ptr,var.child(1),rhs);
  }
  if (printIt) {
    outputMessage("\n");
    outputMessage(name);
    outputMessage(" = \n");
    displayArray(*ptr);
  }
}

void Interpreter::processBreakpoints(const Tree & t) {
  for (int i=0;i<bpStack.size();i++) {
    if ((bpStack[i].cname == context->scopeName()) &&
	((LineNumber(context->scopeTokenID()) == bpStack[i].tokid))) {
      doDebugCycle();
      context->setScopeTokenID(t.context());
    }
  }
  if (tracetrap > 0) {
    if ((LineNumber(context->scopeTokenID()) != tracecurrentline)) {
      tracetrap--;
      if (tracetrap == 0)
	doDebugCycle();
    }
  }
  if (context->scopeStepTrap() > 0) {
    if ((LineNumber(context->scopeTokenID())) !=
	context->scopeStepCurrentLine()) {
      context->setScopeStepTrap(context->scopeStepTrap()-1);
      if (context->scopeStepTrap() == 0)
	doDebugCycle();
    }
  }
}

void Interpreter::statementType(const Tree & t, bool printIt) {
  // check the debug flag
  context->setScopeTokenID(t.context());
  processBreakpoints(t);
  switch(t.token()) {
  case '=':
    {
      Array b(expression(t.second()));
      assignment(t.first(),printIt,b);
    }
    break;
  case TOK_MULTI:
    multiFunctionCall(t,printIt);
    break;
  case TOK_SPECIAL:
    specialFunctionCall(t,printIt);
    break;
  case TOK_FOR:
    forStatement(t);
    break;
  case TOK_WHILE:
    whileStatement(t);
    break;
  case TOK_IF:
    ifStatement(t);
    break;
  case TOK_BREAK:
    if (context->inLoop())
      throw InterpreterBreakException();
    break;
  case TOK_CONTINUE:
    if (context->inLoop())
      throw InterpreterContinueException();
    break;
  case TOK_DBSTEP:
    dbstepStatement(t);
    emit RefreshBPLists();
    throw InterpreterReturnException();
    break;
  case TOK_DBTRACE:
    dbtraceStatement(t);
    emit RefreshBPLists();
    throw InterpreterReturnException();
    break;
  case TOK_DBUP:
    dbup();
    break;
  case TOK_DBDOWN:
    dbdown();
    break;
  case TOK_RETURN:
    throw InterpreterReturnException();
    break;
  case TOK_SWITCH:
    switchStatement(t);
    break;
  case TOK_TRY:
    tryStatement(t);
    break;
  case TOK_QUIT:
    throw InterpreterQuitException();
    break;
  case TOK_RETALL:
    throw InterpreterRetallException();
    break;
  case TOK_KEYBOARD:
    doDebugCycle();
    break;
  case TOK_GLOBAL:
    globalStatement(t);
    break;
  case TOK_PERSISTENT:
    persistentStatement(t);
    break;
  case TOK_EXPR:
    expressionStatement(t,printIt);
    break;
  case TOK_NEST_FUNC:
    break;
  default:
    throw Exception("Unrecognized statement type");
  }
}


//Trapping at the statement level is much better! - two
//problems... try/catch and multiline statements (i.e.,atell.m)
//The try-catch one is easy, I think...  When a try occurs,
//we capture the stack depth... if an exception occurs, we
//unwind the stack to this depth..
//The second one is trickier - suppose we have a conditional
//statement
//if (a == 3)
//    bfunc
//else
//    cfunc
//end
//this is represented in the parse tree as a single construct...
//

//
//Works
void Interpreter::statement(const Tree & t) {
  try {
    if (t.is(TOK_QSTATEMENT))
      statementType(t.first(),false);
    else if (t.is(TOK_STATEMENT))
      statementType(t.first(),m_quietlevel == 0);
    else
      throw Exception("Unexpected statement type!\n");
  } catch (Exception& e) {
    if (autostop && !InCLI) {
      errorCount++;
      e.printMe(this);
      stackTrace();
      doDebugCycle();
    } else  {
      if (!e.wasHandled() && !InCLI && !intryblock) {
	stackTrace();
	e.markAsHandled();
      }
      throw;
    }
  }
}

//Works
void Interpreter::block(const Tree & t) {
  try {
    const TreeList statements(t.children());
    for (TreeList::const_iterator i=statements.begin();
	 i!=statements.end();i++) {
      if (m_kill)
	throw InterpreterKillException();
      if (m_interrupt) {
	outputMessage("Interrupt (ctrl-b) encountered\n");
	stackTrace();
	m_interrupt = false;
	doDebugCycle();
      } else
	statement(*i);
    }
  } catch (Exception &e) {
    lasterr = e.msg();
    throw;
  }
}

// I think this is too complicated.... there should be an easier way
// Works
index_t Interpreter::countLeftHandSides(const Tree & t) {
  Array lhs;
  ArrayReference ptr(context->lookupVariable(t.first().text()));
  if (!ptr.valid())
    lhs = EmptyConstructor();
  else
    lhs = *ptr;
  if (t.numChildren() == 1) return 1;
  if (t.last().is(TOK_PARENS)) return 1;
  for (int index = 1;index < t.numChildren()-1;index++) {
    try {
      deref(lhs,t.child(index));
    } catch (Exception& e) {
      lhs = EmptyConstructor();
    }
  }
  if (t.last().is(TOK_BRACES)) {
    const Tree & s(t.last());
    ArrayVector m;
    for (int p = 0; p < s.numChildren(); p++)
      multiexpr(s.child(p),m);
    subsindex(m);
    if (m.size() == 0)
      throw Exception("Expected indexing expression!");
    if (m.size() == 1) {
      // m[0] should have only one element...
      if (IsColonOp(m[0]))
	return (lhs.length());
      return (IndexArrayFromArray(m[0]).length());
    } else {
      int i=0;
      index_t outputCount=1;
      while (i<m.size()) {
	if (IsColonOp(m[i]))
	  outputCount *= lhs.dimensions()[i];
	else {
	  outputCount *= IndexArrayFromArray(m[i]).length();
	}
	i++;
      }
      return (outputCount);
    }
  }
  if (t.last().is('.'))
    return std::max((index_t)1,lhs.length());
  return 1;
}

Array Interpreter::AllColonReference(Array v, int index, int count) {
  if (v.isUserClass()) return EmptyConstructor();
  return Array(QString(":"));
}

//test
void Interpreter::specialFunctionCall(const Tree & t, bool printIt) {
  ArrayVector m;
  StringVector args;
  for (int index=0;index < t.numChildren();index++)
    args.push_back(t.child(index).text());
  if (args.empty()) return;
  ArrayVector n;
  for (int i=1;i<args.size();i++)
    n.push_back(Array(args[i]));
  FuncPtr val;
  if (!lookupFunction(args[0],val,n))
    throw Exception("unable to resolve " + args[0] + " to a function call");
  if (val->updateCode(this)) refreshBreakpoints();
  m = doFunction(val,n,0);
}

void Interpreter::setBreakpoint(stackentry bp, bool enableFlag) {
  FuncPtr val;
  bool isFun = lookupFunction(bp.detail,val);
  if (!isFun) {
    warningMessage(QString("unable to find function ") +
		   bp.detail + " to set breakpoint");
    return;
  }
  if (val->type() != FM_M_FUNCTION) {
    warningMessage("function " + bp.detail +
		   " is not an m-file, and does not support breakpoints");
    return;
  }
  //  try {
  //     //    ((MFunctionDef*)val)->SetBreakpoint(bp.tokid,enableFlag);
  //   } catch (Exception &e) {
  //     e.printMe(this);
  //   }
}

void Interpreter::addBreakpoint(stackentry bp) {
  bpStack.push_back(bp);
  refreshBreakpoints();
  emit RefreshBPLists();
}

void Interpreter::refreshBreakpoints() {
  for (int i=0;i<bpStack.size();i++)
    setBreakpoint(bpStack[i],true);
}

//Some notes on the multifunction call...  This one is pretty complicated, and the current logic is hardly transparent.  Assume we have an expression of the form:
//
//[expr1 expr2 ... exprn] = fcall
//
//where fcall is a function call (obviously).  Now, we want to determine how many output arguments fcall should have.  There are several interesting cases to consider:
//
//expr_i is an existing numeric variable -- lhscount += 1
//
//expr_i is an existing cell array -- lhscount += size(expr_i)
//
//expr_i is an existing struct array -- lhscount += size(expr_i)
//
//expr_i does not exist -- lhscount += 1
//
//Where this will fail is in one case.  If expr_i is a cell reference for a variable that does not exist, and has a sized argument, something like
//[eg{1:3}]
//in which case the lhscount += 3, even though eg does not exist.
// WORKS
void Interpreter::multiFunctionCall(const Tree & t, bool printIt) {
  ArrayVector m;
  TreeList s;
  Array c;
  index_t lhsCount;

  if (!t.first().is(TOK_BRACKETS))
    throw Exception("Illegal left hand side in multifunction expression");
  s = t.first().children();
  // We have to make multiple passes through the LHS part of the AST.
  // The first pass is to count how many function outputs are actually
  // being requested.
  // Calculate how many lhs objects there are
  lhsCount = 0;
  for (int ind=0;ind<s.size();ind++)
    lhsCount += countLeftHandSides(s[ind]);

  multiexpr(t.second(),m,lhsCount);

  int index;
  for (index=0;(index<s.size()) && (m.size() > 0);index++) {
    const Tree & var(s[index]);
    QString name(var.first().text());
    ArrayReference ptr(context->lookupVariable(name));
    if (!ptr.valid())
      ptr = createVariable(name);
    if (ptr->isUserClass() &&
	!inMethodCall(ptr->className()) &&
	!stopoverload && (var.numChildren() > 1)) {
      ClassAssignExpression(ptr,var,m.front(),this);
      m.pop_front();
      return;
    }
    if (var.numChildren() == 1) {
      (*ptr) = m.front();
      m.pop_front();
    } else if (var.numChildren() == 2)
      multiassign(ptr,var.second(),m);
    else {
      ArrayVector stack;
      Array data(*ptr);
      int varCount = var.numChildren();
      for (int index=1;index<varCount-1;index++) {
	if (!data.isEmpty()) {
	  try {
	    deref(data,var.child(index));
	  } catch (Exception &e) {
	    data = EmptyConstructor();
	  }
	}
	stack.push_back(data);
      }
      multiassign(&data,var.child(varCount-1),m);
      Array rhs(data);
      if (stack.size() > 0) {
	stack.pop_back();
	int ptr = 0;
	while (stack.size() > 0) {
	  data = stack.back();
	  assign(&data,var.child(varCount-2-ptr),rhs);
	  rhs = data;
	  stack.pop_back();
	  ptr++;
	}
      }
      assign(ptr,var.child(1),rhs);
    }
    if (printIt) {
      outputMessage(name);
      outputMessage(" = \n");
      displayArray(*ptr);
    }
  }
  if (index < s.size())
    warningMessage("one or more outputs not assigned in call.");
}

int getArgumentIndex(StringVector list, QString t) {
  bool foundArg = false;
  QString q;
  int i = 0;
  while (i<list.size() && !foundArg) {
    q = list[i];
    if (q[0] == '&')
      q.remove(0,1);
    foundArg = (q == t);
    if (!foundArg) i++;
  }
  if (foundArg)
    return i;
  else
    return -1;
}

//DOCBLOCK functions_function


//DOCBLOCK functions_anonymous

//DOCBLOCK functions_keywords

//DOCBLOCK functions_varargin

//DOCBLOCK functions_varargout

//DOCBLOCK functions_script

//DOCBLOCK functions_special
void Interpreter::collectKeywords(const Tree & q, ArrayVector &keyvals,
				  TreeList &keyexpr, StringVector &keywords) {
  // Search for the keyword uses -
  // To handle keywords, we make one pass through the arguments,
  // recording a list of keywords used and using ::expression to
  // evaluate their values.
  for (int index=0;index < q.numChildren();index++) {
    if (q.child(index).is(TOK_KEYWORD)) {
      keywords.push_back(q.child(index).first().text());
      if (q.child(index).numChildren() > 1) {
	keyvals.push_back(expression(q.child(index).second()));
	keyexpr.push_back(q.child(index).second());
      } else {
	keyvals.push_back(Array(bool(true)));
	keyexpr.push_back(Tree());
      }
    }
  }
}

int* Interpreter::sortKeywords(ArrayVector &m, StringVector &keywords,
			       StringVector arguments, ArrayVector keyvals) {
  // If keywords were used, we have to permute the
  // entries of the arrayvector to the correct order.
  int *keywordNdx = new int[keywords.size()];
  int maxndx;
  maxndx = 0;
  // Map each keyword to an argument number
  for (int i=0;i<keywords.size();i++) {
    int ndx;
    ndx = getArgumentIndex(arguments,keywords[i]);
    if (ndx == -1)
      throw Exception("out-of-order argument /" + keywords[i] + " is not defined in the called function!");
    keywordNdx[i] = ndx;
    if (ndx > maxndx) maxndx = ndx;
  }
  // Next, we have to determine how many "holes" there are
  // in the argument list - we get the maximum list
  int holes;
  holes = maxndx + 1 - keywords.size();
  // At this point, holes is the number of missing arguments
  // If holes > m.size(), then the total number of arguments
  // is just maxndx+1.  Otherwise, its
  // maxndx+1+(m.size() - holes)
  int totalCount;
  if (holes > m.size())
    totalCount = maxndx+1;
  else
    totalCount = maxndx+1+(m.size() - holes);
  // Next, we allocate a vector to hold the values
  ArrayVector toFill;
  for (int i=0;i<totalCount;i++)
    toFill.push_back(Array());
  //  ArrayVector toFill(totalCount);
  bool *filled = new bool[totalCount];
  int *argTypeMap = new int[totalCount];
  for (int i=0;i<totalCount;i++) {
    filled[i] = false;
    argTypeMap[i] = -1;
  }
  // Finally...
  // Copy the keyword values in
  for (int i=0;i<keywords.size();i++) {
    toFill[keywordNdx[i]] = keyvals[i];
    filled[keywordNdx[i]] = true;
    argTypeMap[keywordNdx[i]] = i;
  }
  // Fill out the rest of the values from m
  int n = 0;
  int p = 0;
  while (n < m.size()) {
    if (!filled[p]) {
      toFill[p] = m[n];
      filled[p] = true;
      argTypeMap[p] = -2;
      n++;
    }
    p++;
  }
  // Finally, fill in empty matrices for the
  // remaining arguments
  for (int i=0;i<totalCount;i++)
    if (!filled[i])
      toFill[i] = EmptyConstructor();
  // Clean up
  delete[] filled;
  delete[] keywordNdx;
  // Reassign
  m = toFill;
  return argTypeMap;
}

// arguments is exactly what it should be - the vector of arguments
// m is vector of argument values
// keywords is the list of values passed as keywords
// keyexpr is the
void Interpreter::handlePassByReference(Tree q, StringVector arguments,
					ArrayVector m,StringVector keywords,
					TreeList keyexpr, int* argTypeMap) {
  Tree p;
  // M functions can modify their arguments
  int maxsearch = m.size();
  if (maxsearch > arguments.size()) maxsearch = arguments.size();
  int qindx = 0;
  for (int i=0;i<maxsearch;i++) {
    // Was this argument passed out of order?
    if ((keywords.size() > 0) && (argTypeMap[i] == -1)) continue;
    if ((keywords.size() > 0) && (argTypeMap[i] >=0)) {
      p = keyexpr[argTypeMap[i]];
    } else {
      p = q.second().child(qindx);
      qindx++;
      if (qindx >= q.second().numChildren())
	qindx = q.second().numChildren()-1;
    }
    QString args(arguments[i]);
    if (args[0] == '&') {
      args.remove(0,1);
      // This argument was passed by reference
      if (!p.valid() || !(p.is(TOK_VARIABLE)))
	throw Exception("Must have lvalue in argument passed by reference");
      assignment(p,false,m[i]);
    }
  }
}

//Test
void Interpreter::functionExpression(const Tree & t,
				     int narg_out,
				     bool outputOptional,
				     ArrayVector &output) {
  ArrayVector m, n;
  StringVector keywords;
  ArrayVector keyvals;
  TreeList keyexpr;
  FuncPtr funcDef;
  int* argTypeMap;
  // Because of the introduction of user-defined classes, we have to
  // first evaluate the keywords and the arguments, before we know
  // which function to call.
  // First, check for arguments
  if ((t.numChildren()>=2) && t.second().is(TOK_PARENS)) {
    // Collect keywords
    collectKeywords(t.second(),keyvals,keyexpr,keywords);
    // Evaluate function arguments
    try {
      const Tree & s(t.second());
      for (int p=0;p<s.numChildren();p++)
	multiexpr(s.child(p),m);
    } catch (Exception &e) {
      // Transmute the error message about illegal use of ':'
      // into one about undefined variables.  Its crufty,
      // but it works.
      if (e.matches("Illegal use of the ':' operator"))
	throw Exception("Undefined variable " + t.text());
      else
	throw;
    }
  }
  // Now that the arguments have been evaluated, we have to
  // find the dominant class
  if (!lookupFunction(t.first().text(),funcDef,m))
    throw Exception("Undefined function or variable " +
		    t.first().text());
  if (funcDef->updateCode(this)) refreshBreakpoints();
  if (funcDef->scriptFlag) {
    if (t.numChildren()>=2)
      throw Exception(QString("Cannot use arguments in a call to a script."));
    if ((narg_out > 0) && !outputOptional)
      throw Exception(QString("Cannot assign outputs in a call to a script."));
    context->pushScope(((MFunctionDef*)funcDef)->fileName,
		       ((MFunctionDef*)funcDef)->name,false);
    context->setScopeActive(false);
    block(((MFunctionDef*)funcDef)->code);
    if (context->scopeStepTrap() >= 1) {
      tracetrap = 1;
      tracecurrentline = 0;
      warningMessage("dbstep beyond end of script " + context->scopeDetailString() +
		     " -- setting single step mode\n");
      context->setScopeStepTrap(0);
    }
    context->popScope();
  } else {
    // We can now adjust the keywords (because we know the argument list)
    // Apply keyword mapping
    if (!keywords.empty())
      argTypeMap = sortKeywords(m,keywords,funcDef->arguments,keyvals);
    else
      argTypeMap = NULL;
    if ((funcDef->inputArgCount() >= 0) &&
	(m.size() > funcDef->inputArgCount()))
      throw Exception(QString("Too many inputs to function ")+t.first().text());
    if ((funcDef->outputArgCount() >= 0) &&
	(narg_out > funcDef->outputArgCount() && !outputOptional))
      throw Exception(QString("Too many outputs to function ")+t.first().text());
    n = doFunction(funcDef,m,narg_out);
    // Check for any pass by reference
    if (t.hasChildren() && (funcDef->arguments.size() > 0))
      handlePassByReference(t,funcDef->arguments,m,keywords,keyexpr,argTypeMap);
  }
  // Some routines (e.g., min and max) will return more outputs
  // than were actually requested... so here we have to trim
  // any elements received that we didn't ask for.
  // preserve one output if we were called as an expression (for ans)
  if (outputOptional) narg_out = (narg_out == 0) ? 1 : narg_out;
  while (n.size() > narg_out)
    n.pop_back();
  output += n;
}


void Interpreter::toggleBP(QString fname, int lineNumber) {
  if (isBPSet(fname,lineNumber)) {
    QString fname_string(fname);
    for (int i=0;i<bpStack.size();i++)
      if ((bpStack[i].cname == fname_string) &&
	  (LineNumber(bpStack[i].tokid) == lineNumber)) {
	deleteBreakpoint(bpStack[i].number);
	return;
      }
  } else {
    addBreakpoint(fname,lineNumber);
  }
}

MFunctionDef* Interpreter::lookupFullPath(QString fname) {
  StringVector allFuncs(context->listAllFunctions());
  FuncPtr val;
  for (int i=0;i<allFuncs.size();i++) {
    bool isFun = context->lookupFunction(allFuncs[i],val);
    if (!isFun || !val) return NULL;
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr;
      mptr = (MFunctionDef *) val;
      if (mptr->fileName ==  fname) return mptr;
    }
  }
  return NULL;
}

static int bpList = 1;
// Add a breakpoint - name is used to track to a full filename
void Interpreter::addBreakpoint(QString name, int line) {
  FuncPtr val;
  // Map the name argument to a full file name.
  QString fullFileName;
  if (context->lookupFunction(name,val) && (val->type() == FM_M_FUNCTION))
    fullFileName = ((MFunctionDef*) val)->fileName;
  else
    fullFileName = name;
  // Get a list of all functions
  StringVector allFuncs(context->listAllFunctions());
  // We make one pass through the functions, and update
  // those functions that belong to the given filename
  for (int i=0;i<allFuncs.size();i++) {
    bool isFun = context->lookupFunction(allFuncs[i],val);
    if (!isFun || !val) throw Exception("Cannot add breakpoint to " + name + " :  it does not appear to be a valid M file.");
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr = (MFunctionDef *) val;
      if (mptr->fileName == fullFileName)
	mptr->updateCode(this);
    }
  }
  // Refresh the list of all functions
  allFuncs = context->listAllFunctions();
  // Search through the list for any function defined  - for each function,
  // record the line number closest to it
  MemBlock<int> line_dist_block(allFuncs.size());
  int *line_dist = &line_dist_block;
  for (int i=0;i<allFuncs.size();i++) line_dist[i] = 2*max_line_count;
  for (int i=0;i<allFuncs.size();i++) {
    bool isFun = context->lookupFunction(allFuncs[i],val);
    if (!isFun || !val) throw Exception("Cannot add breakpoint to " + name + " :  it does not appear to be a valid M file.");
    if (val->type() == FM_M_FUNCTION) {
      MFunctionDef *mptr = (MFunctionDef *) val;
      if (mptr->fileName == fullFileName) {
	try {
	  int dline = mptr->ClosestLine(line);
	  line_dist[i] = dline;
	} catch (Exception& e) {
	}
      }
    }
  }
  // Second pass through it - find the function with a line number closest to the
  // desired one, but not less than it
  int best_func = -1;
  int best_dist = 2*max_line_count;
  for (int i=0;i<allFuncs.size();i++) {
    if ((line_dist[i] >= line) && ((line_dist[i]-line) < best_dist)) {
      best_func = i;
      best_dist = line_dist[i]-line;
    }
  }
  if (best_dist > max_line_count)
//    warningMessage(QString("Cannot set breakpoint at line ")+line+" of file "+name + ".  \r\nThis can be caused by an illegal line number, or a function that is not on the path or in the current directory.");
    emit IllegalLineOrCurrentPath(name, line);
  else {
    addBreakpoint(stackentry(fullFileName,allFuncs[best_func],best_dist+line,bpList++));
  }
}

bool Interpreter::isBPSet(QString fname, int lineNumber) {
  for (int i=0;i<bpStack.size();i++)
    if ((bpStack[i].cname == fname) &&
	(LineNumber(bpStack[i].tokid) == lineNumber)) return true;
  return false;
}

// called by editor
bool Interpreter::isInstructionPointer(QString fname, int lineNumber) {
  if (!InCLI) return false;
  ParentScopeLocker lock(context);
  QString filename(context->scopeName());
  int token(context->scopeTokenID());
  return ((fname == filename) && ((lineNumber == LineNumber(token)) ||
				  ((lineNumber == 1) && (LineNumber(token) == 0))));
}

void Interpreter::listBreakpoints() {
  for (int i=0;i<bpStack.size();i++) {
    QString buffer = QString("%1   %2 line %3\n").arg(bpStack[i].number)
      .arg(bpStack[i].cname).arg(LineNumber(bpStack[i].tokid));
    outputMessage(buffer);
  }
}

void Interpreter::deleteBreakpoint(int number) {
  for (int i=0;i<bpStack.size();i++)
    if (bpStack[i].number == number) {
      bpStack.remove(i);
      emit RefreshBPLists();
      return;
    }
  warningMessage("Unable to delete specified breakpoint (does not exist)");
  emit RefreshBPLists();
  return;
}

void Interpreter::stackTrace(int skiplevels) {
  bool firstline = true;
  int depth = context->scopeDepth();
  context->bypassScope(skiplevels);
  for (int i=0;i<(depth-skiplevels);i++) {
    if ((context->scopeName() == "keyboard") &&
	(context->scopeDetailString() == "keyboard")) {
      context->bypassScope(1);
      continue;
    }
    if (firstline) {
      firstline = false;
    } else
      outputMessage(QString("    "));
    outputMessage(QString("In ") + context->scopeName() + "("
		  + context->scopeDetailString() + ")");
    int line = int(LineNumber(context->scopeTokenID()));
    if (line > 0)
      outputMessage(QString(" at line " +
			    QString().setNum(LineNumber(context->scopeTokenID()))));
    outputMessage("\r\n");
    context->bypassScope(1);
  }
  context->restoreScope(depth);
}

bool Interpreter::inMethodCall(QString classname) {
  if (context->scopeDetailString().isEmpty()) return false;
  if (context->scopeDetailString()[0] != '@') return false;
  return (context->scopeDetailString().mid(1,classname.size()) == classname);
}

bool Interpreter::lookupFunction(QString funcName, FuncPtr& val) {
  ArrayVector dummy;
  return(lookupFunction(funcName,val,dummy));
}

bool IsNestedName(QString basename) {
  return (basename.lastIndexOf("/") >= 0);
}

QString StripNestLevel(QString basename) {
  int ndx = basename.lastIndexOf("/");
  if (ndx >= 0)
    basename.remove(ndx,basename.size());
  else
    basename = "";
  return basename;
}

// Look up a function by name.  Use the arguments (if available) to assist
// in resolving method calls for objects
bool Interpreter::lookupFunction(QString funcName, FuncPtr& val,
				 ArrayVector &args, bool disableOverload) {
  int passcount = 0;
  while(passcount < 2) {
    // This is the order for function dispatch according to the Matlab manual
    // Nested functions - not explicitly listed in the Matlab manual, but
    // I figure they have the highest priority in the current scope.
    if (isMFile(context->scopeName()) &&
	(context->lookupFunction(NestedMangleName(context->scopeDetailString(),funcName),val)))
      return true;
    if (InCLI && isMFile(context->activeScopeName()) &&
    	(context->lookupFunction(NestedMangleName(context->activeScopeDetailString(),funcName),val)))
      return true;
    // Not a nested function of the current scope.  We have to look for nested
    // functions of all parent scopes. Sigh.
    if (context->isCurrentScopeNested()) {
      QString basename = context->scopeDetailString();
      while (!basename.isEmpty()) {
	if (context->lookupFunction(NestedMangleName(basename,funcName),val))
	  return true;
	basename = StripNestLevel(basename);
      }
    }
    // Subfunctions
    if (inMFile() &&
	(context->lookupFunction(getLocalMangledName(funcName),val)))
      return true;
    // Private functions
    // Not sure if you have to be an M-file in the current directory
    // to access a private function...
    if (context->lookupFunction(getPrivateMangledName(funcName),val))
      return true;
    // Class constructor functions
    if (context->lookupFunction(ClassMangleName(funcName,funcName),val))
      return true;
    if (!(disableOverload || stopoverload)) {
      // Look for a class method
      // Are any of the arguments classes?
      bool anyClasses = false;
      int i=0;
      while ((!anyClasses) && (i < args.size())) {
	anyClasses = args[i].isUserClass();
	if (!anyClasses) i++;
      }
      // Yes, try and resolve the call to a method
      if (anyClasses && ClassResolveFunction(this,args[i],funcName,val))
	return true;
    }
    if (context->lookupFunction(funcName,val)) return true;
    if (passcount == 0)
      rescanPath();
    passcount++;
  }
  return false;
}

//DOCBLOCK variables_functionhandles

//DOCBLOCK variables_indexing


// This has a few shortcomings that prevent it from being
// 100% correct.
//
//   1.  subsindex is not called for argument
//       expressions of user-defined classes.
//   2.  "end" no longer works.
//
// To fix "end", we should use a source transformation technique.
// The original tree looks like this
//
//   variable
//      -> t
//      -> ()
//          -> 2
//          -> end
//
// This should be translated into:
//
//   _t = end(t,2)
//
// This is done in Transform.cpp...
//
// This does not cover:
//    Function pointers
//    subsindex
//
//

//
//
void Interpreter::deref(Array &r, const Tree & s) {
  SaveEndInfo;
  endRef = &r;
  if (s.is(TOK_PARENS)) {
    ArrayVector m;
    endTotal = s.numChildren();
    if (s.numChildren() == 0) {
      r = r;
    } else {
      for (int p = 0; p < s.numChildren(); p++) {
	endCount = m.size();
	multiexpr(s.child(p),m);
      }
      subsindex(m);
      if (m.size() == 1)
	r = r.get(m[0]);
      else
	r = r.get(m);
    }
  } else if (s.is(TOK_BRACES)) {
    ArrayVector m;
    endTotal = s.numChildren();
    for (int p = 0; p < s.numChildren(); p++) {
      endCount = m.size();
      multiexpr(s.child(p),m);
    }
    subsindex(m);
    if (m.size() == 1)
      r = ArrayFromCellArray(r.get(m[0]));
    else
      r = ArrayFromCellArray(r.get(m));
  } else if (s.is('.')) {
    r = r.get(s.first().text()).front();
  } else if (s.is(TOK_DYN)) {
    QString field;
    try {
      Array fname(expression(s.first()));
      field = fname.asString();
    } catch (Exception &e) {
      throw Exception("dynamic field reference to structure requires a string argument");
    }
    r = r.get(field).front();
  }
  RestoreEndInfo;
}

 Array Interpreter::rhs(const Tree & t) {
   ArrayReference ptr(context->lookupVariable(t.first().text()));
   if (!ptr.valid()) {
     ArrayVector m;
     functionExpression(t,1,false,m);
     m = handleReindexing(t,m);
     if (m.size() >= 1)
       return m[0];
     else
       return EmptyConstructor();
   }
   if (t.numChildren() == 1)
     return *ptr;
   if (ptr->isUserClass() && !stopoverload) {
     ArrayVector m(ClassRHSExpression(*ptr,t,this));
     if (m.size() >= 1)
       return m[0];
     else
       return EmptyConstructor();
   }
   Array r(*ptr);
   for (int index = 1;index < t.numChildren();index++)
     deref(r,t.child(index));
   return r;
 }


int Interpreter::getErrorCount() {
  int retval = errorCount;
  errorCount = 0;
  return retval;
}

Interpreter::Interpreter(Context* aContext) {
  errorCount = 0;
  lasterr = QString("");
  context = aContext;
  depth = 0;
  printLimit = 1000;
  autostop = false;
  intryblock = false;
  jitcontrol = JITOff;
  jitcount = 0;
  stopoverload = false;
  m_skipflag = false;
  m_noprompt = false;
  m_liveUpdateFlag = false;
  tracetrap = 0;
  tracecurrentline = 0;
  endRef = NULL;
  m_interrupt = false;
  m_kill = false;
  m_diaryState = false;
  m_diaryFilename = "diary";
  m_captureState = false;
  m_capture = "";
  m_profile = false;
  m_quietlevel = 0;
  m_enableWarnings = true;
  m_disablerescan = false;
  context->pushScope("base","base",false);
}


Interpreter::~Interpreter() {
  delete context;
}

bool Interpreter::getDisableRescan() {
  return m_disablerescan;
}

void Interpreter::setDisableRescan(bool flag) {
  m_disablerescan = flag;
}

bool Interpreter::getStopOverload() {
  return stopoverload;
}

void Interpreter::setStopOverload(bool flag) {
  stopoverload = flag;
}

// stackentry& Interpreter::activeDebugStack() {
//   if (cstack.isEmpty()) throw Exception("Debug stack is corrupted -- please file a bug report that reproduces this problem!");
//   if (cstack.size() < 2) return cstack[0];
//   return cstack[cstack.size()-2];
// }

// const stackentry& Interpreter::activeDebugStack() const {
//   if (cstack.isEmpty()) throw Exception("Debug stack is corrupted -- please file a bug report that reproduces this problem!");
//   if (cstack.size() < 2) return cstack[0];
//   return cstack[cstack.size()-2];
// }


// We want dbstep(n) to cause us to advance n statements and then
// stop.  we execute statement-->set step trap,
void Interpreter::dbstepStatement(const Tree & t) {
  int lines = 1;
  if (t.hasChildren()) {
    Array lval(expression(t.first()));
    lines = lval.asInteger();
  }
  // Get the current function
  FuncPtr val;
  if (context->scopeName() == "base") return;
  ParentScopeLocker lock(context);
  if (!lookupFunction(context->scopeDetailString(),val)) {
    warningMessage(QString("unable to find function ") + context->scopeDetailString() + " to single step");
    return;
  }
  context->setScopeStepTrap(lines);
  context->setScopeStepCurrentLine(LineNumber(context->scopeTokenID()));
}

void Interpreter::dbtraceStatement(const Tree & t) {
  int lines = 1;
  if (t.hasChildren()) {
    Array lval(expression(t.first()));
    lines = lval.asInteger();
  }
  // Get the current function
  FuncPtr val;
  if (context->scopeDetailString() == "base") return;
  ParentScopeLocker lock(context);
  if (!lookupFunction(context->scopeDetailString(),val)) {
    warningMessage(QString("unable to find function ") + context->scopeDetailString() + " to single step");
    return;
  }
  tracetrap = lines;
  tracecurrentline = LineNumber(context->scopeTokenID());
}

// static QString EvalPrep(QString line) {
//   QString buf1 = line;
//   if (buf1.endsWith('\n'))
//     buf1.chop(1);
//   if (buf1.endsWith('\r'))
//     buf1.chop(1);
//   if (buf1.size() > 20)
//     buf1 = buf1.left(20) + "...";
//   return buf1;
// }

void Interpreter::ExecuteLine(QString txt) {
  mutex.lock();
  cmd_buffer.push_back(txt);
  bufferNotEmpty.wakeAll();
  mutex.unlock();
  if (m_diaryState) diaryMessage(txt);
}

//PORT
void Interpreter::evaluateString(QString line, bool propogateExceptions) {
  Tree b;
  Tree t;
  m_interrupt = false;
  Scanner S(line,"");
  Parser P(S);
  try{
    b = P.process();
    t = b;
    if (!t.is(TOK_SCRIPT))
      throw Exception("Function definition unexpected!");
    t = t.first();
  } catch(Exception &e) {
    if (propogateExceptions)
      throw;
    errorCount++;
    e.printMe(this);
    return;
  }
  try {
    block(t);
  } catch(Exception &e) {
    if (propogateExceptions) throw;
    errorCount++;
    e.printMe(this);
  }
}

QString Interpreter::getLastErrorString() {
  return lasterr;
}

void Interpreter::setLastErrorString(QString txt) {
  lasterr = txt;
}

void Interpreter::setGreetingFlag(bool skip) {
  m_skipflag = skip;
}

void Interpreter::setNoPromptFlag(bool noprompt) {
  m_noprompt = noprompt;
}

bool NeedsMoreInput(Interpreter *eval, QString txt) {
  // Check for ... or an open []
  try {
    Scanner S(txt,"");
    while (!S.next().is(TOK_EOF))
      S.consume();
    if (S.inContinuationState() || S.inBracket()) return true;
  } catch (Exception &e) {
  }
  try {
    Scanner S(txt,"");
    Parser P(S);
    P.process();
    return false;
  } catch (Exception &e) {
    if (e.msg().left(13) == "Expecting end") {
      return true;
    }
  }
  return false;
}

void Interpreter::sleepMilliseconds(unsigned long msecs) {
  QThread::msleep(msecs);
}

QString Interpreter::getLine(QString prompt) {
  if (!m_noprompt) emit SetPrompt(prompt);
  if (m_diaryState) diaryMessage(prompt);
  QString retstring;
  emit EnableRepaint();
  mutex.lock();
  if (cmd_buffer.isEmpty())
    bufferNotEmpty.wait(&mutex);
  retstring = cmd_buffer.front();
  cmd_buffer.erase(cmd_buffer.begin());
  mutex.unlock();
  emit DisableRepaint();
  return retstring;
}

// This is a "generic" CLI routine.  The user interface (non-debug)
// version of this is "docli"
void Interpreter::evalCLI() {
  QString prompt;
  bool rootCLI;
  setupWatcher();
  while(1) {
    QString fname;
    int line = 0;
    if ((depth == 0) || (context->scopeDepth() < 2)) {
      prompt = "--> ";
      rootCLI = true;
    } else {
      int bypasscount = 0;
      while (InKeyboardScope(context)) {
	bypasscount++;
	context->bypassScope(1);
      }
      fname = context->scopeName();
      line = LineNumber(context->scopeTokenID());
      QString scopename = context->scopeDetailString();
      if (scopename == "builtin")
	scopename = context->scopeName();
      if (scopename == "docli")
	scopename = "base";
      prompt = QString("[%1,%2]--> ").arg(scopename).arg(line);
      context->restoreScope(bypasscount);
      rootCLI = false;
    }
    if (rootCLI) {
      tracetrap = 0;
      context->setScopeStepTrap(0);
    }
    if (m_captureState)
      m_capture += prompt;
    else {
      if (!m_noprompt) emit SetPrompt(prompt);
      if (m_diaryState) diaryMessage(prompt);
    }
    if (m_liveUpdateFlag) {
      updateVariablesTool();
      updateStackTool();
      emit ShowActiveLine(fname,line);
    }
    QString cmdset;
    QString cmdline;
    emit EnableRepaint();
    mutex.lock();

    while ((cmdset.isEmpty() ||
	    NeedsMoreInput(this,cmdset)) && (!m_interrupt)) {
      if (cmd_buffer.isEmpty())
	bufferNotEmpty.wait(&mutex);
      cmdline = cmd_buffer.front();
      cmd_buffer.erase(cmd_buffer.begin());
      cmdset += cmdline;
      if (m_captureState)
	m_capture += cmdline;
    }
    mutex.unlock();
    emit DisableRepaint();
    if (m_interrupt) {
      m_interrupt = false;
      continue;
    }
    int scope_stackdepth = context->scopeDepth();
    setInCLI(true);
    dbdown_executed = false;
    evaluateString(cmdset,false);
    if (!dbdown_executed) {
      while (context->scopeDepth() > scope_stackdepth) context->popScope();
    }
  }
}


//
// Convert a list of variable into indexing expressions
//  - for user defined classes, we call subsindex for
//  - the object
Array Interpreter::subsindex(const Array &m) {
  if (m.isUserClass() && !stopoverload) {
    Array t(ClassUnaryOperator(m,"subsindex",this));
    return Add(t.toClass(Double),Array(index_t(1)));
  }
  return m;
}

 void Interpreter::subsindex(ArrayVector& m) {
   for (int p=0;p<((int)m.size());p++)
     m[p] = subsindex(m[p]);
 }
