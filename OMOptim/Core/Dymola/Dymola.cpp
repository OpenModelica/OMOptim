// $Id$
/**
@file Dymola.cpp
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version


*/


#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>


#include "Dymola.h"
#include "MOSettings.h"
#include "Variable.h"
#include "Variables.h"
#include "InfoSender.h"
#include "LowTools.h"
#include "DymolaParameters.h"

Dymola::Dymola(void)
{
}

Dymola::~Dymola(void)
{
}

void Dymola::verifyInstallation()
{

    // check EI.mo exists
    char * folder = getenv("MOLIBRARIES");

    if (!folder)
    {
    }
    else
    {
    }

}


bool Dymola::compile(QFileInfo moPath,QString modelToConsider,QDir storeFolder,QFileInfo logFile,
                      const QFileInfoList & moDeps, QFileInfoList neededFiles)
{
    // Create Dymola script
    QString filePath = storeFolder.absoluteFilePath("MOFirstRun.mos");
    QFile file(filePath);
    if(file.exists())
    {
        file.remove();
    }
    file.open(QIODevice::WriteOnly);

    QString scriptText;
    QString curPath;

    // load dependencies and model
    QFileInfoList moToLoad;
    moToLoad.append(moDeps);
    moToLoad.push_back(moPath);
    LowTools::removeDuplicates(moToLoad);

    for(int i=0;i<moToLoad.size();i++)
    {
        curPath = QDir::fromNativeSeparators(moToLoad.at(i).absoluteFilePath());
        scriptText.append("openModel(\""+curPath+"\",false)\n");
    }


    QString strFolder = QDir::fromNativeSeparators(storeFolder.absolutePath());
    QString logFilePath = QDir::fromNativeSeparators(logFile.absoluteFilePath());

    scriptText.append("cd "+strFolder+"\n");
    scriptText.append("experimentSetupOutput(textual=true)\n");
    scriptText.append("Advanced.StoreProtectedVariables:=true;\n");
    //scriptText.append("checkModel(\""+modelToConsider+"\",simulate=true)\n");
    scriptText.append("translateModel(\""+modelToConsider+"\")\n");
    scriptText.append("compile()\n");
    scriptText.append("savelog(\""+logFilePath+"\")\n");
    scriptText.append("exit\n");

    QTextStream ts( &file );
    ts << scriptText;
    file.close();

    // Copy needed files
    LowTools::copyFilesInFolder(neededFiles,storeFolder);

    // Run script
    QString dymolaPath = MOSettings::value("dymolaExe").toString();
    QFileInfo dymolaBin(dymolaPath);
    if(!dymolaBin.exists())
    {
        InfoSender::instance()->send(Info("Dymola executable not found. Please verify path in Settings",ListInfo::ERROR2));
        return false;
    }
    else
    {
        // delete previous dymosim.exe
        QFile dymoFile(storeFolder.absoluteFilePath("dymosim.exe"));
        if(dymoFile.exists())
            dymoFile.remove();

        // delete previous dsin file
        QFile dsinFile(storeFolder.absoluteFilePath("dsin.txt"));
        if(dsinFile.exists())
            dsinFile.remove();

        // launch script
        QProcess scriptProcess;
        QStringList args;
        args.push_back(filePath);

        //start process
        InfoSender::sendCurrentTask("Launching Dymola...");
        scriptProcess.start(dymolaPath,args);
        bool ok = scriptProcess.waitForFinished(-1);
        if(!ok)
        {
            QString msg("CreateProcess failed");
            InfoSender::instance()->debug(msg);
            return false;
        }

        //look if it succeed
        bool success = dymoFile.exists();
        InfoSender::eraseCurrentTask();
        return success;
    }
}

bool Dymola::createDsin(QFileInfo moFile,QString modelToConsider,QDir folder,
                        const QFileInfoList & moDeps,QFileInfoList neededFiles)
{
    // Create Dymola script
    QFile file(folder.absoluteFilePath("MOFirstRun.mos"));
    if(file.exists())
    {
        file.remove();
    }
    file.open(QIODevice::WriteOnly);

    QString strFolder = QDir::fromNativeSeparators(folder.absolutePath());

    QString scriptText;
    QString curPath;

    // load dependencies and model
    QFileInfoList moToLoad;
    moToLoad.append(moDeps);
    moToLoad.push_back(moFile);
    LowTools::removeDuplicates(moToLoad);

    for(int i=0;i<moToLoad.size();i++)
    {
        curPath = QDir::fromNativeSeparators(moToLoad.at(i).absoluteFilePath());
        scriptText.append("openModel(\""+curPath+"\")\n");
    }


    scriptText.append("cd "+strFolder+"\n");
    scriptText.append("translateModel(\""+modelToConsider+"\")\n");
    scriptText.append("exportInitialDsin(\"dsin.txt\")\n");
    scriptText.append("savelog(\"buildlog.txt\")\n");
    scriptText.append("exit\n");

    QTextStream ts( &file );
    ts << scriptText;
    file.close();

    // Copy needed files
    LowTools::copyFilesInFolder(neededFiles,folder);

    // Run script
    QString dymolaPath = MOSettings::value("dymolaExe").toString();


    QProcess simProcess;
    QStringList args;
    args.push_back(QFileInfo(file).absoluteFilePath());


    // delete previous dsin file
    QFile dsinFile(folder.absoluteFilePath("dsin.txt"));
    if(dsinFile.exists())
        dsinFile.remove();

    // launch script
    InfoSender::instance()->send(Info("Launching Dymola..."));
    simProcess.start(dymolaPath, args);
    bool ok = simProcess.waitForFinished(-1);
    if(!ok)
    {
        QString msg("CreateProcess failed");
        InfoSender::instance()->debug(msg);
        return false;
    }

    //look if it succeed
    bool success = dsinFile.exists();
    return success;
}


QString Dymola::getExecutablePath()
{
    QString path;
#ifdef WIN32
    HKEY hKey = 0;
    char buf[255] = {0};
    DWORD dwType = 0;
    DWORD dwBufSize = sizeof(buf);


    QString subkey("SOFTWARE\\Classes\\Applications\\Dymola.exe\\shell\\Run\\command");

    if( RegOpenKey(HKEY_LOCAL_MACHINE,VQTConvert::QString_To_LPCTSTR(subkey),&hKey) == ERROR_SUCCESS)
    {
        dwType = REG_SZ;
        if( RegQueryValueEx(hKey,NULL,NULL, &dwType, (BYTE*)buf, &dwBufSize) == ERROR_SUCCESS)
        {
            path = QString(buf);
        }
        RegCloseKey(hKey);
    }

    path.remove("\"");
    path.remove("%1");
    //path.remove(" ");
#endif
    return path;
}


bool Dymola::start(QDir folder,QProcess &simProcess,QString & errMsg,int maxNSec)
{
#ifdef WIN32
    simProcess.setWorkingDirectory(folder.absolutePath());

    QString appPath = folder.absoluteFilePath("Dymosim.exe");
    if(!QFile::exists(appPath))
    {
        errMsg = "Cannot find Dymosim.exe in folder " + folder.absolutePath();
        return false;
    }
    simProcess.start(appPath, QStringList());

    int nmsec;
    if(maxNSec==-1)
        nmsec = -1;
    else
        nmsec = maxNSec*1000;

    bool ok = simProcess.waitForFinished(nmsec);
    if(!ok)
    {
        errMsg = "Simulation process failed or time limit reached";
        simProcess.close();
    }

    return ok;
#else
    return false;
#endif
}

void Dymola::writeParameters(QString &allDsinText,MOParameters *parameters)
{
    QString newLine;

    QStringList lines = allDsinText.split("\n");
    int iLForm = lines.indexOf(QRegExp(".* # lform .*"));
    if(iLForm>-1)
    {
        newLine =  " 0                         # lform    0/1 ASCII/Matlab-binary storage format of results";
        lines.replace(iLForm,newLine);
    }


    MOParameter* pStopTime = parameters->findItem(DymolaParameters::str(DymolaParameters::STOPTIME));
    int iLStopTime = lines.indexOf(QRegExp(".* # StopTime .*"));
    if((iLStopTime>-1) && pStopTime)
    {
        newLine =  "       "
                +pStopTime->getFieldValue(MOParameter::VALUE).toString()
                +"                   # StopTime     Time at which integration stops";
        lines.replace(iLStopTime,newLine);
    }

    MOParameter* pTolerance = parameters->findItem(DymolaParameters::str(DymolaParameters::TOLERANCE));
    int iLTolerance = lines.indexOf(QRegExp(".*  # Tolerance .*"));
    if((iLTolerance>-1) && pTolerance)
    {
        newLine =  "       "
                +pTolerance->getFieldValue(MOParameter::VALUE).toString()
                +"                   # nInterval    Relative precision of signals for \n                            #              simulation, linearization and trimming";
        lines.replace(iLTolerance,newLine);
    }

    MOParameter* pnInterval = parameters->findItem(DymolaParameters::str(DymolaParameters::NINTERVAL));
    int iLnInterval = lines.indexOf(QRegExp(".*  # nInterval .*"));
    if((iLnInterval>-1) && pnInterval)
    {
        newLine =  "       "
                +pnInterval->getFieldValue(MOParameter::VALUE).toString()
                +"                   # nInterval    Number of communication intervals, if > 0";
        lines.replace(iLnInterval,newLine);
    }

    MOParameter* pSolver = parameters->findItem(DymolaParameters::str(DymolaParameters::SOLVER));
    int iLSolver = lines.indexOf(QRegExp(".*  # Algorithm .*"));
    if((iLSolver>-1) && pSolver)
    {
        newLine =  "       "
                +pSolver->getFieldValue(MOParameter::VALUE).toString()
                +"                   # Algorithm    Integration algorithm as integer";
        lines.replace(iLSolver,newLine);
    }

    MOParameter* pFinalFile = parameters->findItem(DymolaParameters::str(DymolaParameters::FINALFILE));
    int iLineLRes = lines.indexOf(QRegExp(".*  # lres     0/1 do not/store results .*"));
    if((iLineLRes>-1) && pFinalFile)
    {
        int lRes;
        switch(pFinalFile->value().toInt())
        {
        case DymolaParameters::DSFINAL :
            lRes = 0;
            break;
        case DymolaParameters::DSRES :
        default :
            lRes = 1;
            break;
        }

        newLine =  "       "
                +QString::number(lRes)
                +"                   # lres     0/1 do not/store results on result file";
        lines.replace(iLineLRes,newLine);
    }


    allDsinText = lines.join("\n");
}

bool Dymola::getVariablesFromDsFile(QTextStream *text, MOVector<Variable> *variables,QString modelName)

{
    variables->clear();
    QString line;
    QStringList linefields;

    Variable *newVariable;
    text->seek(0);
    QString str = text->readAll();

    int indStartVariables;
    int indStartDesc;
    int nv; //current variable
    int nbv; //number of variables


    indStartVariables = str.indexOf("#    Names of initial variables");
    text->seek(indStartVariables);
    text->readLine(); //go to next line
    text->readLine(); //pass through char() line

    // Get variables' names
    line = text->readLine();
    while (!line.isEmpty()){
        newVariable = new Variable();
        newVariable->setName(line);
        newVariable->setModel(modelName);
        variables->addItem(newVariable);
        line=text->readLine();
    }


    nbv=variables->size();

    // Get variable's value, type, min, max, nature
    text->readLine(); // pass through double() line
    nv=0;
    line = text->readLine();
    while (!line.isEmpty())
    {
        linefields = line.split(" ", QString::SkipEmptyParts);
        if(linefields.size()<8)
        {
            // data has been stored on two lines
            line = text->readLine();
            linefields << line.split(" ", QString::SkipEmptyParts);
        }
        if(nv>=variables->size())
        {
            InfoSender::instance()->sendError("Corrupted dsin file. Unable to read variables. Try to regenerate dsin file.");
            variables->clear();
            return false;
        }
        if(linefields.size()<8)
        {
            InfoSender::instance()->sendWarning("Cannot read variable information ["+variables->at(nv)->name()+"]. It will not be considered");
            variables->removeRow(nv);
        }
        else
        {
            variables->at(nv)->setValue(linefields[1].toDouble());
            int dymDataType = linefields[5].toInt();
            variables->at(nv)->setDataType(convertVariableType(dymDataType));
            int dymValueType = linefields[0].toInt();
            int dymCategory = linefields[4].toInt();
            variables->at(nv)->setCausality(convertToCausality(dymValueType,dymCategory));
            nv++;
        }
        line = text->readLine();
    }

    // Get variables' description
    indStartDesc = str.indexOf("initialDescription");
    text->seek(indStartDesc);
    text->readLine(); //pass through end of line

    line = text->readLine();
    nv=0;
    nbv=variables->size();
    while (!text->atEnd() && nv<nbv)
    {
        variables->at(nv)->setDescription(line);
        line = text->readLine();
        nv++;
    }
    return true;
}

bool Dymola::getVariablesFromDsFile(QString fileName_, MOVector<Variable> *variables,QString _modelName)
{
    variables->clear();
    QFileInfo fileinfo = QFileInfo(fileName_);
    bool result = false;
    if (fileinfo.exists())
    {
        QFile file(fileinfo.filePath());
        file.open(QIODevice::ReadOnly);
        QTextStream* in = new QTextStream(&file);
        result = getVariablesFromDsFile(in, variables,_modelName);
        file.close();
    }
    return result;
}

bool Dymola::getFinalVariablesFromDsFile(QString fileName_, MOVector<Variable> *variables,QString modelName)
{
    variables->clear();
    QFileInfo fileinfo = QFileInfo(fileName_);
    bool result = false;
    if (fileinfo.exists())
    {
        QFile file(fileinfo.filePath());
        file.open(QIODevice::ReadOnly);
        QTextStream* in = new QTextStream(&file);
        result = getFinalVariablesFromDsFile(in, variables,modelName);
        file.close();
        delete in;
    }
    return result;
}

bool Dymola::getFinalVariablesFromDsFile(QTextStream *text, MOVector<Variable> *variables,QString _modelName)
{

    variables->clear();
    QString line,data;
    QStringList linefields;

    Variable *newVariable;
    text->seek(0);
    QString str = text->readAll();

    int indStartVariables;
    int indStartDesc;
    int indStartDataInfo, indStartData;
    int nv; //current variable
    int nbv; //number of variables
    int nbCols1,nbRows1,nbCols2,nbRows2;

    QList<int> dataInfo1;
    QList<int> dataInfo2;
    QList<int> dataInfo3;
    QList<int> dataInfo4;

    double **data1, **data2;



    // **************************
    // Names
    // **************************
    indStartVariables = str.indexOf("char name(");
    text->seek(indStartVariables);
    line = text->readLine(); //go to next line
    line = text->readLine(); //read line
    while (!line.isEmpty()){
        newVariable = new Variable();
        newVariable->setName(line);
        newVariable->setModel(_modelName);
        variables->addItem(newVariable);
        line=text->readLine();
    }
    nbv=variables->size();

    // **************************
    // Description
    // **************************
    indStartDesc = str.indexOf("char description(");
    text->seek(indStartDesc);
    line = text->readLine(); //go to next line
    line = text->readLine(); //read line

    nv=0;
    while (!line.isEmpty() && nv<nbv){
        variables->at(nv)->setDescription(line);
        line = text->readLine();
        nv++;
    }


    // **************************
    // Matrix informations
    // **************************
    indStartDataInfo = str.indexOf("int dataInfo(");
    text->seek(indStartDataInfo);
    line = text->readLine(); //go to next line
    line = text->readLine(); //read line

    QStringList curDataInfos;
    nv=0;
    while (!line.isEmpty() && nv<nbv){
        curDataInfos = line.split(" ",QString::SkipEmptyParts);
        dataInfo1.push_back(curDataInfos.at(0).toInt());
        dataInfo2.push_back(curDataInfos.at(1).toInt());
        dataInfo3.push_back(curDataInfos.at(2).toInt());
        dataInfo4.push_back(curDataInfos.at(3).toInt());
        line = text->readLine();
        nv++;
    }


    // **************************
    // Matrix Values
    // **************************
    // DATA1
    indStartData = str.indexOf("float data_1(");
    text->seek(indStartData);
    line = text->readLine();
    line=line.right(line.size()-line.indexOf("("));
    line.remove("(");
    line.remove(")");
    curDataInfos = line.split(",");
    nbRows1 = curDataInfos.at(0).toInt();
    nbCols1 = curDataInfos.at(1).toInt();

    data.clear();
    line = text->readLine(); //read line
    while (!line.isEmpty()){
        data.append(line);
        line = text->readLine();
    }
    curDataInfos = data.split(" ",QString::SkipEmptyParts);


    // fill data1 matrix
    data1 = new double* [nbRows1];
    for (int row = 0; row < nbRows1; row++)
    {
        data1[row] = new double[nbCols1];

        for(int col = 0; col < nbCols1; col++)
        {
            data1[row][col] = curDataInfos.at(row*nbCols1+col).toDouble();
        }
    }

    // DATA2
    indStartData = str.indexOf("float data_2(");
    text->seek(indStartData);
    line = text->readLine();
    line=line.right(line.size()-line.indexOf("("));
    line.remove("(");
    line.remove(")");
    curDataInfos = line.split(",");
    nbRows2 = curDataInfos.at(0).toInt();
    nbCols2 = curDataInfos.at(1).toInt();

    data.clear();
    line = text->readLine(); //read line
    while (!line.isEmpty()){
        data.append(line);
        line = text->readLine();
    }
    curDataInfos = data.split(" ",QString::SkipEmptyParts);


    // fill data2 matrix
    data2 = new double* [nbRows2];
    for (int row = 0; row < nbRows2; row++)
    {
        data2[row] = new double[nbCols2];

        for(int col = 0; col < nbCols2; col++)
        {
            data2[row][col] = curDataInfos.at(row*nbCols2+col).toDouble();
        }
    }




    // **************************
    // Variable Values
    // **************************
    for(int iV=0;iV<nbv;iV++)
    {
        if(dataInfo1.at(iV) ==0 || dataInfo1.at(iV) ==1)
        {
            if(dataInfo2.at(iV)<0)
                variables->at(iV)->setValue(data1[nbRows1-1][-dataInfo2.at(iV)-1]);
            else
                variables->at(iV)->setValue(data1[nbRows1-1][dataInfo2.at(iV)-1]);
        }
        else
        {
            if(dataInfo2.at(iV)<0)
                variables->at(iV)->setValue(data2[nbRows2-1][-dataInfo2.at(iV)-1]);
            else
                variables->at(iV)->setValue(data2[nbRows2-1][dataInfo2.at(iV)-1]);
        }
    }


    for (int i = 0; i < nbRows1; i++)
        delete [] data1[i];
    delete [] data1;

    for (int i = 0; i < nbRows2; i++)
        delete [] data2[i];
    delete [] data2;

    return true;

}


bool Dymola::setVariablesToDsin(QString fileName, QString modelName,MOVector<Variable> *variables,MOParameters *parameters,QString & errMsg)
{

    //Reading Preamble
    QFileInfo fileinfo = QFileInfo(fileName);
    QFile file(fileinfo.filePath());
    if(!file.open(QIODevice::ReadOnly))
    {
        errMsg = "Failed to open "+fileName;
        return false;
    }
    else
    {
        QTextStream textRead(&file);
        QString allText = textRead.readAll();
        file.close();

        // change preamble
        writeParameters(allText,parameters);

        // change variable values
        QRegExp rxLine;
        int index=0;
        QString newLine1,newLine2;
        QString varName;
        int iCurVar;
        Variable* curVar;
        QStringList fields;
        QString smallText;
        QStringList capLines;
        int index2;
        int prec=MOSettings::value("MaxDigitsDsin").toInt(); //number of decimals
        QString value;
        for(int iV=0;iV<variables->size();iV++)
        {
            InfoSender::instance()->debug("Setting variable "+ varName+" in "+fileName);

            curVar = variables->at(iV);
            varName = curVar->name(Variable::SHORT);
            //varName = varName.remove(modelName+".");
            rxLine.setPattern(sciNumRx()+"\\s+"+sciNumRx()+"\\s+"+sciNumRx()+"\\s+"+sciNumRx()+"\\s+"+sciNumRx()+"\\s+"+sciNumRx()+"\\s*#\\s*("+varName+")\\s*");

            // extracting only text around varname
            // to speed-up regexp research (too long without)
            index2 = allText.indexOf(varName);
            smallText.clear();
            while(index2>-1)
            {
                smallText += allText.mid(index2-300,310+varName.size()); // must capture end of line chars -> 310>300
                index2 = allText.indexOf(varName,index2+1);
            }

            index = rxLine.indexIn(smallText);
            if(index>-1)
            {


                char format = 'E';

                value = QString::number(curVar->getFieldValue(Variable::VALUE).toDouble(),format,prec);
                fields = rxLine.capturedTexts();
                capLines = rxLine.cap(0).split("\n",QString::SkipEmptyParts);
                newLine1 = fields.at(1)+"\t"+ value +"\t";
                newLine1 += fields.at(3)+"\t"+fields.at(4);
                newLine2 = fields.at(5)+"\t"+fields.at(6)+"\t"+" # "+fields.at(7);
                // if variable def were on two lines
                if((capLines.size()>1)&& capLines.at(1).contains(QRegExp("\\S")))
                {
                    InfoSender::instance()->debug("found variable. 2 lines. Total text captured:  "+rxLine.cap(0));
                    allText = allText.replace(rxLine.cap(0),newLine1+"\n"+newLine2+"\n");
                    InfoSender::instance()->debug("New Text :  "+newLine1+"\n"+newLine2);
                }
                else
                {
                    InfoSender::instance()->debug("found variable. 1 line. Total text captured:  "+rxLine.cap(0));
                    // if variable def were on only one line
                    allText = allText.replace(rxLine.cap(0),newLine1+"\t"+newLine2+"\n");
                    InfoSender::instance()->debug("New Text :  "+newLine1+"\t"+newLine2);
                }
            }
            else
            {
                InfoSender::instance()->send(Info("Unable to set variable value (not found in init file):"+varName,ListInfo::ERROR2));
            }
        }

        fileinfo.setFile(fileName);
        file.setFileName(fileinfo.filePath());
        bool ok = file.open(QIODevice::WriteOnly);
        if(!ok)
        {
           errMsg = "Unable to open file for writing :" + fileinfo.absoluteFilePath();
           return false;
        }

        QTextStream textWrite(&file);
        textWrite<<allText;
        file.close();
    }
}

QString Dymola::sciNumRx()
{
    QString rx = "([+-]?[0-9]*\\.?[0-9]+|[+-]?[0-9]+\\.?[0-9]*[eE][+-]?[0-9]+)";
    return rx;
}

VariableType Dymola::convertVariableType(int dymDataType)
{
    dymDataType = dymDataType % 4; // use %4 to avoid Dymola variable definition 'bug'
    //# column 6: Data type of variable.
    //#           = 0: real.
    //#           = 1: boolean.
    //#           = 2: integer.
    switch(dymDataType)
    {
    case 0 :
        return OMREAL;
    case 1 :
        return OMBOOLEAN;
    case 2 :
        return OMINTEGER;
    default :
        return OMREAL;
    }
}

VariableCausality Dymola::convertToCausality(int dymValueType, int dymCategory)
{
    if((dymValueType==-1)||(dymCategory==1)||(dymCategory==5))
        return OMINPUT;

    if((dymValueType==-2)||(dymValueType==-0))
        return OMOUTPUT;

    return OMUNKNOWN;
}
