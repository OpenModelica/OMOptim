#include "OMProcess.h"

OMProcess::OMProcess(QString appName, bool dispOut, QObject *parent) :
    QProcess(parent)
{
    _appName = appName;
    _dispOut = dispOut;

    setProcessChannelMode(MergedChannels);
    if(_dispOut)
        connect(this,SIGNAL(readyReadStandardOutput()),this,SLOT(onReadyRead()));
}

void OMProcess::onReadyRead()
{
    QString msg = "["+_appName+"] : "+QString(readAllStandardOutput());
    infoSender.send(Info(msg));
}
