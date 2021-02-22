#ifndef __OctaveScanner_hpp__
#define __OctaveScanner_hpp__

#include "Scanner.hpp"
#include "Token.hpp"
#include <QString>
#include <QStack>

class OctaveScanner : public Scanner {
protected:
  virtual void fetch();
  virtual void fetchOther();
  virtual void fetchIdentifier();
  virtual void fetchWhitespace();
  virtual void fetchOctString();
public:
  OctaveScanner(QString buf, QString fname);
  virtual ~OctaveScanner() {}
};

#endif
