

#include "ExeModel.h"

ExeModel::ExeModel(QString name,QFileInfo exeFilePath, QFileInfo inputFilePath)
    :ModItem(NULL)
{
    _name = name;
    _file = exeFilePath;
    _inputFile = inputFilePath;
    _exeFile = exeFilePath;
}

QString ExeModel::getStrToolTip()
{
    QString toolTip;
        toolTip += ("Executable Model: " + _name + "\n");
        toolTip += ("File: " + _exeFile.absoluteFilePath() + "\n");
        toolTip += ("InputFile: " + _inputFile.absoluteFilePath() + "\n");
    return toolTip;
}

