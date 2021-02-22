// Test out the feasibility of doing a generic package building tool
// for FreeMat.  In general, downloading of files, and file manipulation
// and macro definitions are straightforward with Qt.  The tricky
// bit appears to be dealing with archives (.zip and .tar.gz).

#include <QtCore>
#include <QtScript>
#include <QtNetwork>
#include <QtGui>
#include "pkg_builder.hpp"
#include "quazip.h"
#include "quazipfile.h"

QTextBrowser *console;
static bool shouldErase = false;
static bool wantsToQuit;

void OutputConsoleText(QString txt) {
  txt.replace("\r\n","\n");
  for (int i=0;i<txt.size();i++) {
    if (txt.at(i) == '\r') {
      console->moveCursor(QTextCursor::StartOfLine);
      shouldErase = true;
    } else if (txt.at(i) == '\n') {
      console->append("");
      shouldErase = false;
    } else {
      if (shouldErase) {
	QTextCursor cursor(console->textCursor());
	cursor.movePosition(QTextCursor::EndOfLine,
			    QTextCursor::KeepAnchor);
	
	cursor.removeSelectedText();
	shouldErase = false;
      }
      console->textCursor().insertText(txt.at(i));
    }
  }
}

#define BLOCKSIZE 512
struct posix_header
{				/* byte offset */
  char name[100];		/*   0 */
  char mode[8];			/* 100 */
  char uid[8];			/* 108 */
  char gid[8];			/* 116 */
  char size[12];		/* 124 */
  char mtime[12];		/* 136 */
  char chksum[8];		/* 148 */
  char typeflag;		/* 156 */
  char linkname[100];		/* 157 */
  char magic[6];		/* 257 */
  char version[2];		/* 263 */
  char uname[32];		/* 265 */
  char gname[32];		/* 297 */
  char devmajor[8];		/* 329 */
  char devminor[8];		/* 337 */
  char prefix[155];		/* 345 */
				/* 500 */
};

#define TMAGIC   "ustar"	/* ustar and a null */
#define TMAGLEN  6
#define TVERSION "00"		/* 00 and no null */
#define TVERSLEN 2

/* Values used in typeflag field.  */
#define REGTYPE	 '0'		/* regular file */
#define AREGTYPE '\0'		/* regular file */
#define LNKTYPE  '1'		/* link */
#define SYMTYPE  '2'		/* reserved */
#define CHRTYPE  '3'		/* character special */
#define BLKTYPE  '4'		/* block special */
#define DIRTYPE  '5'		/* directory */
#define FIFOTYPE '6'		/* FIFO special */
#define CONTTYPE '7'		/* reserved */

#define XHDTYPE  'x'            /* Extended header referring to the
				   next file in the archive */
#define XGLTYPE  'g'            /* Global extended header */

/* Bits used in the mode field, values in octal.  */
#define TSUID    04000		/* set UID on execution */
#define TSGID    02000		/* set GID on execution */
#define TSVTX    01000		/* reserved */
				/* file permissions */
#define TUREAD   00400		/* read by owner */
#define TUWRITE  00200		/* write by owner */
#define TUEXEC   00100		/* execute/search by owner */
#define TGREAD   00040		/* read by group */
#define TGWRITE  00020		/* write by group */
#define TGEXEC   00010		/* execute/search by group */
#define TOREAD   00004		/* read by other */
#define TOWRITE  00002		/* write by other */
#define TOEXEC   00001		/* execute/search by other */

union block
{
  char buffer[BLOCKSIZE];
  struct posix_header header;
};

static int octToInt(const char *t) {
  return QString(t).toInt(NULL,8);
}

static bool emptyBlock(block &t) {
  for (int i=0;i<BLOCKSIZE;i++) 
    if (t.buffer[i] != 0) return false;
  return true;
}

static QScriptValue qtscript_untgz(QScriptContext *ctx, QScriptEngine *eng) {
  QString ipath = ctx->argument(0).toString();
  QString opath = ctx->argument(1).toString();
  gzFile gfile = gzopen(ipath.toLocal8Bit().constData(),"rb");
  if (!gfile) {
    OutputConsoleText("Unable to untgz file " + ipath);
    return QScriptValue(eng,false);
  }
  bool atEndOfTar = false;
  while (!atEndOfTar) {
    block b;
    if (gzread(gfile,b.buffer,BLOCKSIZE) != BLOCKSIZE) {
      OutputConsoleText("Corrupt tgz file (short read)" + ipath);
      return QScriptValue(eng,false);
    }
    if (strncmp(b.header.magic,TMAGIC,TMAGLEN-1) != 0) {
      OutputConsoleText("Corrupt tgz file (magic) " + ipath);
      return QScriptValue(eng,false);
    }
    atEndOfTar = emptyBlock(b);
    if (!atEndOfTar) {
      QString filename(opath + QString(b.header.prefix) + 
		       QString(b.header.name));
      OutputConsoleText(QString("File: ") + filename + "  :  ");
      int len = octToInt(b.header.size);
      OutputConsoleText(QString(" Size %1").arg(len));
      OutputConsoleText(QString(" Code %1\n").arg(b.header.typeflag));
      if (b.header.typeflag == DIRTYPE) {
	QDir::current().mkpath(filename);
      } else if (b.header.typeflag == REGTYPE) {
	QFile out(filename);
	if (!out.open(QFile::WriteOnly)) {
	  OutputConsoleText(QString(" Unable to open file ") + filename + 
			    " for writing");
	  return QScriptValue(eng,false);
	}
	int leftToWrite = len;
	while (leftToWrite > 0) {
	  block t;
	  if (gzread(gfile,t.buffer,BLOCKSIZE) != BLOCKSIZE) {
	    OutputConsoleText("Corrupt tar file (short read)" + ipath);
	    return QScriptValue(eng,false);
	  }
	  if (out.write(t.buffer,qMin(BLOCKSIZE,leftToWrite)) != 
	      qMin(BLOCKSIZE,leftToWrite)) {
	    OutputConsoleText(QString("Unable to write to file ") + 
			      filename);
	    return QScriptValue(eng,false);
	  }
	  leftToWrite -= BLOCKSIZE;
	}
	out.close();
      } else {
	OutputConsoleText(QString(" Unable to cope with entry type " + filename));
	return QScriptValue(eng,false);
      }
    }
  }
  gzclose(gfile);
  return QScriptValue(eng,true);
}

static QScriptValue qtscript_mkdir(QScriptContext *ctx, QScriptEngine *eng) {
  QString path = ctx->argument(0).toString();
  return QScriptValue(eng,QDir::current().mkpath(path));
}

static QScriptValue qtscript_sys(QScriptContext *ctx, QScriptEngine *eng) {
  QString cmd = ctx->argument(0).toString();
  QProcess toRun;
#ifdef Q_OS_WIN32
  char shellCmd[_MAX_PATH];
  if( !GetEnvironmentVariable("ComSpec", shellCmd, _MAX_PATH) )
    throw Exception("Unable to find command shell!");
  cmd = QString(shellCmd) + " /a /c " + QString(cmd);
#else
  cmd = QString("sh -c \"") + cmd + QString("\"");
#endif
  OutputConsoleText(cmd + "\n");
  toRun.start(cmd);
  if (!toRun.waitForStarted())
    return QScriptValue(eng,-1);
  toRun.closeWriteChannel();
  while (!toRun.waitForFinished(100)) {
    OutputConsoleText(toRun.readAllStandardOutput());
    OutputConsoleText(toRun.readAllStandardError());
    qApp->processEvents();
  }
  OutputConsoleText(toRun.readAllStandardOutput());
  OutputConsoleText(toRun.readAllStandardError());
  return QScriptValue(eng,toRun.exitCode());
}

static QScriptValue qtscript_disp(QScriptContext *ctx, QScriptEngine *eng) {
  QString str = ctx->argument(0).toString();
  OutputConsoleText(str);
  qApp->processEvents();
  return eng->undefinedValue();
}

static QScriptValue qtscript_unzip(QScriptContext *ctx, QScriptEngine *eng) {
  QString filename(ctx->argument(0).toString());
  QString basedir(ctx->argument(1).toString());
  QuaZip zip(filename);
  if(!zip.open(QuaZip::mdUnzip)) {
    OutputConsoleText("Unable to open file " + filename + "\n");
    return QScriptValue(eng,false);
  }
  zip.setFileNameCodec("IBM866");
  OutputConsoleText(QString("Unzipping ") + filename + "\n");
  OutputConsoleText(QString("  Archive contains %1 entries\n").arg(zip.getEntriesCount()));
  OutputConsoleText(QString("  Comment:" + zip.getComment() + "\n"));
  QuaZipFileInfo info;
  QuaZipFile file(&zip);
  QFile out;
  QString name;
  char c;
  for(bool more=zip.goToFirstFile(); more; more=zip.goToNextFile()) {
    if(!zip.getCurrentFileInfo(&info)) {
      OutputConsoleText(QString("  Corrupted zip file!\n"));
      return QScriptValue(eng,false);
    }
    if(!file.open(QIODevice::ReadOnly)) {
      OutputConsoleText(QString("  Corrupted zip file!\n"));
      return QScriptValue(eng,false);
    }
    name=file.getActualFileName();
    if(file.getZipError()!=UNZ_OK) {
      OutputConsoleText(QString("  Corrupted zip file!\n"));
      return QScriptValue(eng,false);
    }
    out.setFileName(basedir+QDir::separator()+name);
    OutputConsoleText(QString("   Unpacking " + name + "\n"));
    // this will fail if "name" contains subdirectories, but we don't mind that
    out.open(QIODevice::WriteOnly);
    // Slow like hell (on GNU/Linux at least), but it is not my fault.
    // Not ZIP/UNZIP package's fault either.
    // The slowest thing here is out.putChar(c).
    while(file.getChar(&c)) out.putChar(c);
    out.close();
    if(file.getZipError()!=UNZ_OK) {
      OutputConsoleText(QString("  Corrupted zip file!\n"));
      return QScriptValue(eng,false);
    }
    if(!file.atEnd()) {
      OutputConsoleText(QString("  Corrupted zip file!\n"));
      return QScriptValue(eng,false);
    }
    file.close();
    if(file.getZipError()!=UNZ_OK) {
      OutputConsoleText(QString("  Corrupted zip file!\n"));
      return QScriptValue(eng,false);
    }
  }
  zip.close();
  if(zip.getZipError()!=UNZ_OK) {
    OutputConsoleText(QString("  Corrupted zip file!\n"));
    return QScriptValue(eng,false);
  }
  return QScriptValue(eng,true);
}


static QScriptValue qtscript_fetch(QScriptContext *ctx, QScriptEngine *eng) {
  QString url = ctx->argument(0).toString();
  QScriptValue filename = ctx->argument(1);
  QScriptValue timeout = ctx->argument(2);
  //   curl = curl_easy_init();
  //   if (curl) {
  //     curl_easy_setopt(curl, CURLOPT_URL, url.toString().toAscii().constData());
  //     curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
  //     res = curl_easy_perform(curl);
  //     curl_easy_cleanup(curl);
  //   }
  QFile file(filename.toString());
  if (!file.open(QIODevice::WriteOnly))
    throw QString("unable to open output file ") + filename.toString();
  int retry_count = 0;
  bool success = false;
  bool explicitFailure = false;
  while (!explicitFailure && !success && (retry_count < 5)) {
    qDebug() << "URL:" << url;
    URLRetriever p_url(QUrl(url),&file,timeout.toNumber());
    p_url.run();
    if (p_url.follow()) {
      url = p_url.forwardAddress();
    } else {
      explicitFailure = p_url.error();
      success = !explicitFailure;
    }
    retry_count++;
  }
  if (!success) 
    qDebug() << "Failed\n";
  else
    qDebug() << "Success\n";
  //  return QScriptValue(eng,!p_url.error());
  return QScriptValue(eng,false);
}

static QScriptValue qtscript_quit(QScriptContext *, QScriptEngine *eng) {
  qApp->exit();
  return eng->undefinedValue();
}

static void interactive(QScriptEngine &eng)
{
  QScriptValue global = eng.globalObject();
  QScriptValue quitFunction = eng.newFunction(qtscript_quit);
  QScriptValue fetchFunction = eng.newFunction(qtscript_fetch);
  if (!global.property(QLatin1String("exit")).isValid())
    global.setProperty(QLatin1String("exit"), quitFunction);
  if (!global.property(QLatin1String("quit")).isValid())
    global.setProperty(QLatin1String("quit"), quitFunction);
  if (!global.property(QLatin1String("fetch")).isValid())
    global.setProperty(QLatin1String("fetch"), fetchFunction);
  wantsToQuit = false;

  QTextStream qin(stdin, QFile::ReadOnly);

  const char *qscript_prompt = "qs> ";
  const char *dot_prompt = ".... ";
  const char *prompt = qscript_prompt;

  QString code;

  forever {
    QString line;

    printf("%s", prompt);
    fflush(stdout);

    line = qin.readLine();
    if (line.isNull())
      break;

    code += line;
    code += QLatin1Char('\n');

    if (line.trimmed().isEmpty()) {
      continue;

    } else if (! eng.canEvaluate(code)) {
      prompt = dot_prompt;

    } else {
      QScriptValue result = eng.evaluate(code, QLatin1String("typein"));

      code.clear();
      prompt = qscript_prompt;

      if (! result.isUndefined())
	fprintf(stderr, "%s\n", qPrintable(result.toString()));

      if (wantsToQuit)
	break;
    }
  }
}

void RegisterFunction(QString name, QScriptEngine &eng, 
		      QScriptEngine::FunctionSignature fun) {
  QScriptValue global(eng.globalObject());
  QScriptValue ffun = eng.newFunction(fun);
  if (!global.property(name).isValid()) global.setProperty(name, ffun);
}

int main(int argc, char *argv[])
{
  QApplication qapp(argc,argv);
  if (argc < 2) {
    return 1;
  }
  QString filename(argv[1]);
  QScriptEngine eng;
  RegisterFunction("disp",eng,qtscript_disp);
  RegisterFunction("sys",eng,qtscript_sys);  
  RegisterFunction("fetch",eng,qtscript_fetch);  
  RegisterFunction("quit",eng,qtscript_quit);
  RegisterFunction("unzip",eng,qtscript_unzip);
  RegisterFunction("mkdir",eng,qtscript_mkdir);
  RegisterFunction("untgz",eng,qtscript_untgz);

  console = new QTextBrowser;
  console->setFont(QFont("Courier",10));
  console->resize(640,480);
  console->show();

  QFile file(filename);
  QString contents;
  if (file.open(QFile::ReadOnly)) {
    QTextStream stream(&file);
    contents = stream.readAll();
    file.close();
  }
  if (contents.isEmpty()) 
    return qapp.exec();
  QScriptValue r = eng.evaluate(contents, filename);
  if (eng.hasUncaughtException()) {
    QStringList backtrace(eng.uncaughtExceptionBacktrace());
    OutputConsoleText(r.toString() + "\n" + 
		      backtrace.join("\n"));
  }
  //  interactive(eng);
  return qapp.exec();
  //   QScriptEngine eng;
  //   qDebug() << "starting interpreter...";
  //   interactive(eng);
 }
