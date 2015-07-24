#ifndef EXEMODEL_H
#define EXEMODEL_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QFileInfo>
#include <QString>
#endif

#include "ModItem.h"
#include "OMOptimModelica.h"

class ExeModel : public ModItem
{
private:
    QFileInfo _inputFile;
    QFileInfo _exeFile;

public:
    ExeModel(QString name,QFileInfo exeFile, QFileInfo inputFilePath);
    Modelica::ClassRestr getClassRestr(){return Modelica::EXECUTABLE;}
    virtual QString getClassName() const {return "ExeModel";}
    virtual QString getStrToolTip();

    QFileInfo inputFile(){return _inputFile;}
    QFileInfo exeFile(){return _exeFile;}
};

#endif // EXEMODEL_H
