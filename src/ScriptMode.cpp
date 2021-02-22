#include "ScriptMode.hpp"
#include <QFile>
#include <QIODevice>
#include <QTextStream>

ScriptMode::ScriptMode(QString filename) : fname(filename) {}

void ScriptMode::Fire()
{
  QFile file(fname);
  if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
      QTextStream in(&file);
      QString line = in.readLine();
      if (!line.startsWith("#"))
	emit SendCommand(line);
      while (!in.atEnd())
	emit SendCommand(in.readLine());
    }
  emit SendCommand("quit");
}
