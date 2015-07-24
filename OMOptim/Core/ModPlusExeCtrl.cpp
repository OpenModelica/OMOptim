

#include "ModPlusExeCtrl.h"
#include "Project.h"
#include "MOParameter.h"
#include "Variable.h"
#include "ModItemsTree.h"
#include "ModPlusOMCtrl.h"
#include "PlugInterface.h"
#include <QDebug>
#include "LowTools.h"
#include "ModExePlus.h"

ModPlusExeCtrl::ModPlusExeCtrl(Project* project,ModelPlus *modExePlus)
               :ModPlusCtrl(project,modExePlus,project->moomc())
{
    _copyAllMoOfFolder = true;
}

ModPlusExeCtrl::~ModPlusExeCtrl(void)
{

}


void ModPlusExeCtrl::setDefaultParameters()
{
//    _parameters->addItem(new MOParameter(STOPTIME,"stopTime","Stop time",1,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));

//    QMap<int,QString> mapSolvers;
//    mapSolvers.insert(ModPlusOMCtrl::DASSL,"dassl");
//    mapSolvers.insert(ModPlusOMCtrl::EULER,"euler");

//    _parameters->addItem( new MOParameterListed((int)SOLVER,"solver","Solver",ModPlusOMCtrl::DASSL,mapSolvers));

//    _parameters->addItem(new MOParameter(TOLERANCE,"tolerance","Tolerance",1e-6,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
//    _parameters->addItem(new MOParameter(STEPSIZE,"stepSize","Step size",0.002,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));
//    _parameters->addItem(new MOParameter(STARTTIME,"startTime","Start time",0,MOParameter::DOUBLE,0,std::numeric_limits<int>::max()));

 //   _parameters->addItem(new MOParameter((int)MAXSIMTIME,"MaxSimTime","Maximum time allowed for simulation [sec] (-1 : no limit)",-1,MOParameter::INT,-1,std::numeric_limits<int>::max()));

//    QMap<int,QString> mapOutput;
//    mapOutput.insert(ModPlusOMCtrl::MAT,"mat");
//    mapOutput.insert(ModPlusOMCtrl::CSV,"csv");

    //    _parameters->addItem( new MOParameterListed((int)OUTPUT,"outputFormat","Output",ModPlusOMCtrl::MAT,mapOutput));

}




QString ModPlusExeCtrl::name()
{
    return "Executable";
}


//ModPlusCtrl *ModPlusExeCtrl::clone()
//{
//    ModPlusExeCtrl* cloned = new ModPlusExeCtrl(_project, _ModelPlus);

//    cloned->_initFile          = _initFile;
//    cloned->_exeFile           = _exeFile;
//    cloned->_copyAllMoOfFolder = _copyAllMoOfFolder;
//    cloned->_parameters        = _parameters->clone();

//    return cloned;
//}







//bool ModPlusExeCtrl::readInitialVariables(MOVector<Variable> *initVariables, ModPlusCtrl::Type type, bool forcerecompile, QString initialFile)
//{
//    QString initFile;

//    ModItem* item = _ModelPlus->getProject()->modItemsTree()->findItem(_ModelPlus->name());
//    ExeModel* exeMod = dynamic_cast<ExeModel*> (item);
//    if( ! exeMod )
//        return false;
//    else
//    {
//        initFile = exeMod->getInputFilePath();
//  //      if(initFile.isEmpty() || initFile.right(3)!= "xml" || !QFile::exists(initFile))
//        if(initFile.isEmpty() || !QFile::exists(initFile))
//            return false;
//        else if(type == ModPlusCtrl::EXECUTABLE)
//          {
//            readInitialVarsFromPluginFunc(initFile, _ModelPlus->modelName(), initVariables);
//            return true;
//          }
//        else
//        {
//            initVariables->clear();
//            getInputVariablesFromXmlFile(initFile, _ModelPlus->modelName(), initVariables);
//            return true;
//        }
//    }
//          }


bool ModPlusExeCtrl::getInputVariablesFromXmlFile(QString filePath, QString modelName, MOVector<Variable> * variables)
{
    variables->clear();
    QString error;

    QDomDocument doc;
    QFile file(filePath);
    if( !file.open( QIODevice::ReadOnly ) )
    {
        // InfoSender::instance()->send( Info(ListInfo::PROBLEMFILENOTEXISTS,filePath));
        return false;
    }
    else if(!doc.setContent(&file,&error))
    {
        file.close();
        // InfoSender::instance()->send( Info(ListInfo::PROBLEMFILECORRUPTED,error,filePath));
        return false;
    }
    file.close();

    // create problem
    return getInputVariablesFromXmlFile(doc,modelName,variables);


}



bool ModPlusExeCtrl::getInputVariablesFromXmlFile(const QDomDocument & doc , QString modelName,MOVector<Variable> * variables)
{
    variables->clear();

    QDomElement xfmi = doc.firstChildElement("fmiModelDescription");
    QDomElement xModelVars = xfmi.firstChildElement("ModelVariables");
    QDomNodeList listScalarVars = xModelVars.elementsByTagName("ScalarVariable");

    bool ok;
    for(int i=0;i<listScalarVars.size();i++)
    {
        Variable* newVar = variableFromFmi(listScalarVars.at(i).toElement(),modelName,ok);
        if(ok)
            variables->addItem(newVar);
    }
    return true;
}



Variable* ModPlusExeCtrl::variableFromFmi(const QDomElement & el,QString modelName,  bool & ok)
{

    ok = true;
    QString name = el.attribute("name");
    if(name.isEmpty())
    {
        ok = false;
        return NULL;
    }

    Variable* newVar = new Variable();
    newVar->setName(name);
    newVar->setDescription(el.attribute("description"));
    newVar->setModel(modelName);                           //added for the black box needs
//    newVar->setInputOutput(el.attribute("Input/Output"));  //added for the black box needs
    //look for type
    QDomElement elType = el.firstChildElement("Real");
    if(!elType.isNull())
    {
        newVar->setDataType(OMREAL);
        newVar->setValue(elType.attribute("start").toDouble(&ok));
    }
    elType = el.firstChildElement("Integer");
    if(!elType.isNull())
    {
        newVar->setDataType(OMINTEGER);
        newVar->setValue(elType.attribute("start").toInt(&ok));
    }
    elType = el.firstChildElement("Boolean");
    if(!elType.isNull())
    {
        newVar->setDataType(OMBOOLEAN);
        newVar->setValue(elType.attribute("start")=="true");
    }

    if(!ok)
    {
        delete newVar;
        return NULL;
    }
    else
        return newVar;
}




QList<ModelPlus::ModelType> ModPlusExeCtrl::compatibleModels()
{
    QList<ModelPlus::ModelType> result;
    result << ModelPlus::EXECUTABLE;
    return result;
}


bool ModPlusExeCtrl::createInitFile(const QStringList &moDeps)
{
    return true;
}






bool ModPlusExeCtrl::getFinalVariablesFromFile(QString fileName_, MOVector<Variable> *variables,QString _modelName)
{
    variables->clear();
    QFileInfo fileinfo = QFileInfo(fileName_);

    if (fileinfo.exists())
    {
        QFile file(fileinfo.filePath());
        file.open(QIODevice::ReadOnly);
        QTextStream* in = new QTextStream(&file);
        bool ok = getFinalVariablesFromFile(in, variables,_modelName);
        file.close();
        delete in;
        return ok;
    }
    else
        return true;
}



bool ModPlusExeCtrl::getFinalVariablesFromFile(QTextStream *text, MOVector<Variable> * variables,QString _modelName)
{
    variables->clear();
    QString line;
    QStringList varNames;
    QStringList varValues;

    Variable *newVar;
    text->seek(0);
    QString str = text->readLine();

    str.remove("\"");
    varNames = str.split(",",QString::SkipEmptyParts);

    while(!text->atEnd())
        line=text->readLine();

    varValues = line.split(",",QString::SkipEmptyParts);

    if(varValues.size()!=varNames.size())
        return false;

    for(int i=0;i<varValues.size();i++)
    {
        newVar = new Variable();
        newVar->setName(varNames.at(i));
        newVar->setModel(_modelName);
        newVar->setValue(varValues.at(i).toDouble());
        variables->addItem(newVar);
    }
    return true;
}

QFileInfo ModPlusExeCtrl::initFile() const
{
    ModExePlus* modExePlus =dynamic_cast<ModExePlus*> (_ModelPlus);
    if(modExePlus)
    {
        return modExePlus->inputFile();
    }
    else
        return QFileInfo();
}

QFileInfo ModPlusExeCtrl::exeFile() const
{
    ModExePlus* modExePlus =dynamic_cast<ModExePlus*> (_ModelPlus);
    if(modExePlus)
    {
        return modExePlus->exeFile();
    }
    else
        return QFileInfo();
}








void ModPlusExeCtrl::setInputVariablesXml(QDomDocument & doc, QString modelName, MOVector<Variable> *variables)
{
    QDomElement xfmi = doc.firstChildElement("fmiModelDescription");
    QDomElement oldxfmi = xfmi;

    QDomElement xModelVars = xfmi.firstChildElement("ModelVariables");
    QDomElement oldxModelVars = xModelVars;

    QDomNodeList listScalarVars = xModelVars.elementsByTagName("ScalarVariable");


    // filling map
    QMap<QString,int> mapScalarVars; //<name,index in listScalarVars>
    QMap<QDomElement,QDomElement> mapNewScalarVars; // <old node,new node>
    QDomElement curVar;
    QDomElement oldVar;
    QDomElement newVar;
    int index;
    QDomElement oldType;
    QDomElement newType;
    QString localVarName;

    // create map for index looking
    for(int i=0;i<listScalarVars.size();i++)
    {
        curVar = listScalarVars.at(i).toElement();
        mapScalarVars.insert(curVar.attribute("name"),i);
    }

    // change variables values
    for(int i=0;i<variables->size();i++)
    {
        // getting local var name (name in init file does not contain model name)
        localVarName = variables->at(i)->name(Variable::SHORT);
        if(localVarName.contains(modelName))
          localVarName = localVarName.remove(modelName+".");

        index = mapScalarVars.value(localVarName,-1);
        if(index>-1)
        {
            oldVar = listScalarVars.at(index).toElement();
            newVar = oldVar;

            oldType = newVar.firstChildElement("Real");
            if(oldType.isNull())
                oldType = newVar.firstChildElement("Integer");
            if(oldType.isNull())
                oldType = newVar.firstChildElement("Boolean");

            if(!oldType.isNull())
            {
                newType = oldType;
                newType.setAttribute("start",variables->at(i)->value().toString());
                newVar.replaceChild(newType,oldType);
                xModelVars.replaceChild(newVar,oldVar);
            }
            xModelVars.replaceChild(newVar,oldVar);
        }
    }

    // update xfmi with new vars
    xfmi.replaceChild(xModelVars,oldxModelVars);
    doc.replaceChild(xfmi,oldxfmi);
}




bool ModPlusExeCtrl::start(QString exeFile, int maxnsec)
{
    QFileInfo exeFileInfo(exeFile);
    QString exeDir = exeFileInfo.absolutePath();

    QProcess simProcess;
    simProcess.setWorkingDirectory(exeDir);

#ifdef WIN32
    QString appPath = "\""+exeFile+"\"";

    //start process
    simProcess.start(appPath, QStringList());
#else
    QStringList arguments;
    arguments << "-c";
    arguments << "./"+exeFileInfo.fileName() << " > log.txt";
    simProcess.start("sh", arguments);
#endif

    int nmsec;
    if(maxnsec==-1)
        nmsec = -1;
    else
        nmsec = maxnsec*1000;
    bool ok = simProcess.waitForFinished(nmsec);
    if(!ok)
    {
        QString msg("CreateProcess failed (%d).");
        InfoSender::instance()->debug(msg);
        return false;
    }
    QString output(simProcess.readAllStandardOutput());
    InfoSender::instance()->send(Info(output,ListInfo::OMCNORMAL2));
    return ok;
}
