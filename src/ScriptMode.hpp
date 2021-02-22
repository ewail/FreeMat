#ifndef __ScriptMode_hpp__
#define __ScriptMode_hpp__

#include <QObject>
#include <QString>

class ScriptMode : public QObject {
  Q_OBJECT
public: 
  ScriptMode(QString filename);
public slots:
  void Fire();
signals:
  void SendCommand(QString);
private:
  QString fname;
};

#endif
