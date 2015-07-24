// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

  @file ProjectBase.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#if !defined(_PROJECTBASE_H)
#define _PROJECTBASE_H

#include <stdio.h>
#include <QtCore/QThread>
#include <QtCore/QMap>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtXml/QDomDocument>
#include <QtCore/QSettings>
#include <QString>
#include <QFileSystemWatcher>
#include <QPluginLoader>


#include "MOItem.h"
#include "ProblemInterfaces.h"


class ProblemInterface;
class Problems;
class Results;
class Problem;
class Result;
class OMCase;
class ScriptParser;

namespace MOThreads
{
    class ProblemThread;
}
/**
 * Main class managing problems, results, models,
 * paths information, save/load main functions, threads.
 * So far, only one project is run at a time.
 */
class ProjectBase: public MOItem
{
    Q_OBJECT

    //****************************
    // Attributes
    //****************************
public :

    //Threads management
    QMutex _componentMutex;
    QMutex _connectionMutex;
    QMutex _problemLaunchMutex;
    QMutex _saveLoadMutex;
    QMap<Problem*,MOThreads::ProblemThread *> _problemsThreads;


protected:
    //Misc
    QString _filePath;
    bool _isdefined;
    bool _isSaved; /// defines whether project has been modified since last save

    // Plugins
    QMap<QString,QString> _pluginsLoaded; //loaded plugins <name,filePath>

    Problems* _problems;
    Results* _results;

    // Problems interfaces
    ProblemInterfaces _problemsInterfaces; /// List of problemInterface pluged to project

    // Parser
    ScriptParser* _scriptParser;

 public:
    ProjectBase();
    virtual ~ProjectBase();

    virtual QString getClassName() const {return "ProjectBase";}
    virtual void clear();
    virtual QString getFieldName(int iField,int role) const;
    virtual unsigned getNbFields() const ;

    virtual bool isSaved();
    virtual void setSaved(bool);



    //****************************
    //Problem managment
    //****************************
    ProblemInterfaces problemsInterfaces(){return _problemsInterfaces;}
    void addProblemInterface(ProblemInterface* problemInterface);
    void removeProblemInterface(QString interfaceName);
    void addNewProblem(ProblemInterface* itf, QStringList modelsList,QString problemType);
    void addResult(Result *);
    void addProblem(Problem *);
    bool addOMCase(QString filePath);
    void addOMCases(QDir folder);
    MOThreads::ProblemThread*  launchProblem(Problem*,bool useSeparateThread = true);
    MOThreads::ProblemThread*  launchProblem(QString,bool useSeparateThread = true);
    void launchProblems(QList<Problem*>,bool useSeparateThread = true);
    void removeResult(OMCase *, bool saveProject = true);
    void removeAllResults();
    void removeProblem(Problem*);
    void removeCases(QList<OMCase*>);
    void renameCase(OMCase*, QString);
    OMCase* findOMCase(QString name);


    // scripts
    virtual bool launchScript(QFileInfo scriptFile){return false;}
    virtual bool launchScript(const QString & text){return false;}

    //****************************
    // Get/Set functions
    //****************************
    bool isDefined(){return _isdefined;}
    QString problemsFolder();
    QString resultsFolder();
    QString filePath();
    QDir folder();
    QString tempPath();
    void setName(QString);
    void setFilePath(QString);
    void setIsDefined(bool);
    Problem* curLaunchedProblem();
    QMap<QString,QString> pluginsLoaded();
    void setScriptParser(ScriptParser *parser);
    ScriptParser* scriptParser() const {return _scriptParser;}


    Problems* problems(){return _problems;}
    Results* results(){return _results;}
    QFileInfoList casesFiles();


    //****************************
    // Save/ Load functions
    //****************************
    virtual void save(bool saveAllOMCases);
    virtual void save(Problem*);
    virtual void save(Result*);
    virtual bool load(QString);

    //****************************
    // Plugins
    //****************************
    virtual bool loadPlugin(QString filePath, bool storePath=true,bool forceLoad=true);
    virtual bool unloadPlugin(QString pluginPath);
    virtual bool unloadPlugins();

    //****************************
    // Misc
    //****************************
    void terminateProblemsThreads();
    bool createTempDir();
    virtual bool checkConfiguration();

    //****************************
    // Slots
    //****************************
public slots :


    // problems
    Problem* restoreProblemFromResult(int numSolved);
    Problem* restoreProblemFromResult(Result* result);
    bool renameProblem(Problem*, QString);
    void onProblemFinished(Problem*,Result*);
    void onProblemStopAsked(Problem*);
    void onProjectChanged();

    // others
    bool renameResult(Result*, QString);


    // openfolder
    void openOMCaseFolder();

    //****************************
    // Signals
    //****************************
signals:
    void sendProgress(float);
    void sendProgress(float,int,int);
    void projectAboutToBeReset();
    void projectChanged();

    void addedProblem(Problem*);
    void addedResult(Result*);

    void beforeRemoveResult(Result*);
    void beforeRemoveProblem(Problem*);

    void problemBegun(Problem*);
    void newProblemProgress(float);
    void newProblemProgress(float,int,int);
    void problemFinished(Problem*,Result*);
    void forgetProblems();// for script event loop, not to be blocked once application should be terminated

    void interfacesModified();

};




#endif
