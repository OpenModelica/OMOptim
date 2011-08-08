#ifndef OMPROCESS_H
#define OMPROCESS_H

#include <QProcess>
#include <QString>
#include "InfoSender.h"


/**
  * OMProcess inherits QProcess. It allows to forward stdout information to infosender.
  * Therefore, user can see command results.
  */
class OMProcess : public QProcess
{
    Q_OBJECT
public:
    explicit OMProcess(QString appName, bool dispOut, QObject *parent = 0);

signals:

public slots:
    void onReadyRead();

private :
    bool _dispOut;
    QString _appName;

};

#endif // OMPROCESS_H
