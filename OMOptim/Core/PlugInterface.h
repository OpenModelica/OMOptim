#ifndef PLUGINTERFACE_H
#define PLUGINTERFACE_H



#include <QString>
#include <QDomDocument>
#include <QObject>

class PlugInterface
{


public:
    PlugInterface(){}
    virtual ~PlugInterface(){}
    virtual QString name() const =0;
    virtual QString outputFileName() const =0;
    virtual QStringList dllFileNamesUsedByExeFile() const =0;
    virtual void readVarsFromInitFile(QString fileName, QDomDocument & doc) const =0;
    virtual void writeInInitFile(QString fileName, QDomDocument & doc) const =0;
    virtual void readResultsFromOutputFile(QString fileName, QDomDocument & doc) const =0;
};


Q_DECLARE_INTERFACE(PlugInterface, "com.interfaceIdentifier/1.0")


#endif // PLUGINTERFACE_H
