#include "ExeModel.h"


ExeModel::ExeModel(QString name,QString exeFilePath, QString inputFilePath)
    :ModItem(NULL)
{
    _name = name;
    _file = QFileInfo(exeFilePath);
    _inputFile = QFileInfo(inputFilePath);
    _exeFile = QFileInfo(exeFilePath);
}

QString ExeModel::getStrToolTip()
{
    QString toolTip;
        toolTip += ("Executable Model: " + _name + "\n");
        toolTip += ("File: " + _exeFile.absoluteFilePath() + "\n");
        toolTip += ("InputFile: " + _inputFile.absoluteFilePath() + "\n");
    return toolTip;
}

