/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef SCRIPTPARSER_H
#define SCRIPTPARSER_H

#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QMap>
#include <QStringList>

class ProjectBase;

class ScriptFunction
{
public:
    ScriptFunction(){};
    ScriptFunction(QString functionName,QStringList argsNames,QString description = QString());

    QString name;
    QStringList args;
    QString description;

    QString toTxt() const;
};


class ScriptParser : public QObject
{
    Q_OBJECT
public:
    ScriptParser(){_continue = true;}
    static bool parseFile(QFileInfo fileInfo,QStringList &commands,QMap<QString,QString> &options);
    static bool parseFile(const QString & text,QStringList &commands,QMap<QString,QString> &options);

    bool executeCommand(QString command);
    bool executeCommands( QStringList commands);
    virtual QString helpText(){return QString();}

public slots:
    void stop(){_continue = false;}
protected :


 protected:
    ScriptFunction findFunction(QString functionName,int nbArgs, bool &ok);

    virtual bool launchFunction(QString function, QStringList args, bool & foundFunction)=0;
    virtual void initFunctions() = 0;
     virtual QStringList functionsList();
    QMap<QString,ScriptFunction> _functions;

    bool _continue;
};

class ScriptParserOMOptimBasis : public ScriptParser
{
    Q_OBJECT
public:
    ScriptParserOMOptimBasis(ProjectBase* project);

    virtual QString helpText();

    QString annexHelpText();

public slots:
    void onProblemFinished();
protected :
    virtual bool launchFunction(QString function, QStringList args, bool & foundFunction);
    virtual void initFunctions();


    ProjectBase* _projectBase;
    bool _problemThreadFinished;
};


#endif // SCRIPTPARSER_H
