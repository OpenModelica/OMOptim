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

    @file MOOptVector.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#ifndef OMS_H
#define OMS_H

#include <exception>

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#include <QtCore/QThread>
#include <QtCore/QProcess>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QMutex>
#endif


#include "omc_communication.h"
#include "OMCHelper.h"
#include "StringHandler.h"
#include "OMOptimModelica.h"
#include "omc_communicator.h"
#include "VariableType.h"

class Project;
class Variable;

/**
  * MOomc is dedicated to communication with OpenModelica through CORBA.
  * Its main functions are to start OMC and call OMC API functions :
  *     - load models
  *     - simulate a model
  *     - read its contents
  *     - read connections
  *     - modify models (connections, add components...)
  *     -...
  *
  */
class MOomc : public QObject
{
    Q_OBJECT

public:
    MOomc(QString appName,bool start = true);
    ~MOomc();


public :

    //Thread management for MO
    void addUsingThread(QThread*,QString);
    void removeUsingThread(QThread*);
    QList<QThread*> getThreads();
    QStringList getThreadsNames();

    //Modelica functions
    QString getWholeText(bool includeMSL);
    QString getText(QString className);
    QStringList getClassNames(QString parentClass = "");
    QStringList getPackages(QString parentClass);
    QStringList getClasses(QString parentClass);
    QStringList getModels(QString parentClass);
    QStringList getRecords(QString parentClass);
    QStringList getElementInfos(QString parentClass);

    QStringList getParameterNames(QString parentClass, bool includeInherited=false);
    QString getParameterValue(QString parentClass, QString parameterName);
    QVariant getParameterValue(QString parentClass, QString parameterName,VariableType type, QVariant defaultValue = QVariant());

    QStringList getInheritedClasses(QString inheritingClass);
    QStringList getComponentModifierNames(QString componentName);
    QString getFlattenedModifierValue(const QString & modelName,const QString & shortComponentName,const QString & modifierName,QString & flattenedModel);
    QString getFlattenedModifierUnit(const QString & modelName,const QString & shortComponentName,const QString & modifierName,QString & flattenedModel);

    QString getFlattenedModel(const QString & modelName);
    QString getFlattenedModifierValue(const QString & modelName,const QString & componentName,const QString & modifierName);
    QStringList getFlattenedModifierVectorValue(const QString &modelName, const QString &componentName, const QString &modifierName, int size, QString &flattenedModel);
    QString getComponentModifierValue(QString modelName,QString shortComponentName,QString modifierName);
    bool setComponentModifiers(QString compName,QString model, QStringList modNames,QStringList modValues);

    QString getAnnotation(QString compName,QString compClass,QString model);
    QString getComment(QString modelName,QString compName);

    int getConnectionNumber(QString className);
    QMap<QString,QString> getConnections(const QString &curComp);
    bool deleteConnection(const QString & shortOrg,const QString &  shortDest,const QString &  model);
    bool deleteConnections(const QStringList &  shortOrgs,const QStringList &  shortDests,const QString &  model);
    bool deleteConnections(const QStringList &  shortOrgs,const QList<QStringList> & dests,const QString &  model);

    bool addConnection(QString org, QString dest);
    bool addConnections(QStringList orgs, QStringList dests);
    bool addConnections(QStringList orgs, QList<QStringList> dests);

    bool inherits(QString childClass, QString parentClass);
    void getInheritedComponents(QString parentClass, QStringList & names, QStringList & classes);
    void getContainedComponents(QString parentClass, QStringList & compNames,QStringList & compClasses,bool includeInherited=true);

    void readElementInfos(QString parentClass,QStringList &packagesClasses,QStringList &modelsClasses,QStringList &recordsNames,QStringList &compsNames,QStringList &compsClasses);
    static QString getValueFromElementInfo(QString elementInfoLine,QString fieldName);

    void loadModel(QString filename,bool force,bool &ok,QString & Error);

    QStringList getDependenciesPaths(QString fileName,bool commentImportPaths);
    void loadStandardLibrary(QString path = QString());


    bool isConnector(QString ClassName);
    bool isModel(QString ClassName);
    bool isRecord(QString ClassName);
    bool isClass(QString ClassName);
    bool isPackage(QString ClassName);
    bool isPrimitive(QString ClassName);
    bool isComponent(QString name);
    QString getPrimitiveClass(QString className);
    bool isPrimitivelyInteger(QString className);
    bool isPrimitivelyReal(QString className);
    bool isPrimitivelyBoolean(QString className);
    VariableType getPrimitiveDataType(QString className);
    QString getComponentClass(QString parameter);
    Modelica::ClassRestr getClassRestriction(QString ClassName);

    bool translateModel(QString model);
    bool buildModel(QString model, QString &exeFile, QString &initFile);

    bool deleteComponent(QString compName);
    bool deleteClass(QString className);
    bool save(QString model);
    bool addComponent(QString name,QString className, QString modelName,QString annotation);

    // added functions
    QFileInfo getFileOfClass(QString);
    QStringList getClassesOfFile(QString);
    QString runScript(QString);
    QString changeDirectory(QString directory);
    QString getWorkingDirectory();
    QString getTempDirectory();
    void initTempDirectory();
    QString getResult();
    bool isStarted();

    //Communication functions
    QString evalCommand(QString comm,QString &errorString);
    QString evalCommand(QString comm);
    //void setCommand(QString comm);
    //    void evalCommand();
    void exit();

    void stopServer();
    void clear();


    QString loadFileWThread(QString filePath);


    bool initCommandLineOptions();
    bool clearCommandLineOptions();


public slots :
    QStringList loadFiles(const QStringList & filePath);
    QString loadFile(const QString & filePath);
    bool startServer();



signals:
    void startOMCThread(QString);
    void finishOMCThread(QString);
    void loadedFile(QString,QString);


private:
    //    void exceptionInEval(std::exception &e);

    //    QString command;
    int nbCalls;

    //OmcCommunicator* mCommunicator;


    QString omc_version_;

    bool mHasInitialized;
    QString mName;
    QString mResult;
    QString mObjectRefFile;
    OmcCommunication_var mOMC;

    QList<QThread*> threads;
    QStringList threadsNames;
    QMutex evalMutex; /// used to prevent eval command while quitting moommc


};

#endif
