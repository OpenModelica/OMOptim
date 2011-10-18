#ifndef CBCCTRLCCTRL_H
#define CBCCTRLCCTRL_H

#include <QtCore/QString>
#include <QtCore/QDir>
#include "MOParameter.h"
#include "OMProcess.h"
#include "EIHEN1Parameters.h"

class CbcCtrl
{
public:
    CbcCtrl(QDir folder, QString mpsFileName,QString solFileName,MOParameters* parameters);
    bool run(QString &msg);
    void stop();

private :
    QString _mpsFileName;
    QString _solFileName;
    QDir _folder;
    MOParameters* _parameters;
    OMProcess _process;

    bool success(QString &msg);

};

#endif // CBCCTRLCCTRL_H
