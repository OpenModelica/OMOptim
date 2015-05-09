#ifndef EXEMODEL_H
#define EXEMODEL_H

#include "ModItem.h"
#include "OMOptimModelica.h"
#include <QFileInfo>
#include <QString>

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
