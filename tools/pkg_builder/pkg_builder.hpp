#include <QtNetwork>
#include <QtCore>
#include <QtGui>

class URLRetriever : public QObject
{
  Q_OBJECT

  QUrl m_url;
  QFile *m_file;
  double m_timeout;
  int m_httpGetId;
  QEventLoop m_event;
  QHttp m_http;
  bool m_err;
  bool m_gotFollowResponse;
  QString m_forwardAddress;
  QProgressDialog m_pd;
  int m_totalLength;
  int m_currentLength;
public:
  URLRetriever(QUrl url, QFile *file, double timeout) :
    m_url(url), m_file(file), m_timeout(timeout), m_err(true) {
    qDebug() << "URL: " << url;
    qDebug() << "  valid: " << url.isValid();
    qDebug() << " string: " << url.toString();
  }
  bool error() {
    return m_err;
  }
  bool follow() {
    return m_gotFollowResponse;
  }
  QString forwardAddress() {
    return m_forwardAddress;
  }
  void run() {
    m_gotFollowResponse = false;
    qDebug() << "Fetch of " << m_url;
    m_http.setHost(m_url.host(), m_url.port() != -1 ? m_url.port() : 80);
    if (!m_url.userName().isEmpty())
      m_http.setUser(m_url.userName(), m_url.password());
    m_httpGetId = m_http.get(m_url.path(), 0);
    QTimer::singleShot((int)m_timeout, &m_http, SLOT(abort()));
    connect(&m_http, SIGNAL(done(bool)), &m_event, SLOT(quit()));
    connect(&m_http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
	    this, SLOT(readResponseHeader(const QHttpResponseHeader &)));
    connect(&m_http, SIGNAL(requestFinished(int, bool)), this, 
	    SLOT(requestFinished(int, bool)));
    connect(&m_http, SIGNAL(readyRead(const QHttpResponseHeader &)),
	    this, SLOT(readyRead(const QHttpResponseHeader &)));
    m_event.exec();
  }
public slots:
  void cancel() {
    m_http.abort();
    m_err = true;
  }
  void readyRead(const QHttpResponseHeader& responseHeader) {
    if (responseHeader.statusCode() == 200) {
      QByteArray bytes(m_http.readAll());
      if (m_file->write(bytes) != bytes.count()) {
	m_err = true;
	m_http.abort();
      }
      m_currentLength += bytes.count();
      m_pd.setValue(m_currentLength);
    }
  }
  void readResponseHeader(const QHttpResponseHeader& responseHeader) {
    if ((responseHeader.statusCode() >= 300) &&
	(responseHeader.statusCode() < 400) &&
	(responseHeader.hasKey("Location"))) {
      m_gotFollowResponse = true;
      m_forwardAddress = responseHeader.value("Location");
      m_http.abort();
      return;
    }
    if (responseHeader.statusCode() != 200) {
      QMessageBox::information(NULL, tr("HTTP"),
			       tr("Download failed: %1.")
			       .arg(responseHeader.reasonPhrase()));
      m_err = true;
      //      progressDialog->hide();
      m_http.abort();
      return;
    }
    m_totalLength = 0;
    if (responseHeader.hasKey("content-length"))  {
      m_totalLength = responseHeader.value("content-length").toInt();
      m_currentLength = 0;
    }
    m_pd.setLabelText("Fetching " + m_url.toString());
    m_pd.setMinimum(0);
    m_pd.setCancelButtonText("Cancel");
    connect(&m_pd, SIGNAL(canceled()), this, SLOT(cancel()));
    m_pd.setMaximum(m_totalLength);
  }
  void requestFinished(int id, bool err) {
    qDebug() << QString("Request ID %1").arg(id);
    if (id != m_httpGetId)
      return;
    //     if ((m_http.lastResponse().statusCode() >= 300) &&
    // 	(m_http.lastResponse().statusCode() < 400) &&
    // 	m_http.lastResponse().hasKey("Location")) {
    //       m_gotFollowResponse = true;
    //       m_forwardAddress = m_http.lastResponse().value("Location");
    //       qDebug() << "Follow:" << m_forwardAddress;
    //       return;
    //     }
    m_err = err;
    if (err) 
      qDebug() << QString("Download failed");
    else
      qDebug() << QString("Download succeeded");
  }
};
