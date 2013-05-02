#include "OMProcess.h"
#include "InfoSender.h"

OMProcess::OMProcess(QObject *parent) :
    QProcess(parent)
{

    _dispOut = true;

    setProcessChannelMode(MergedChannels);
    if(_dispOut)
  connect(this,SIGNAL(readyReadStandardOutput()),this,SLOT(onReadyRead()));
}

void OMProcess::onReadyRead()
{
    QString msg = "["+_appName+"] : "+QString(readAllStandardOutput());
    InfoSender::instance()->send(Info(msg));
}

void OMProcess::setAppName(QString appName)
{
    _appName = appName;
}
