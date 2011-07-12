// $Id$
/**
@file Dymola.cpp
@brief Comments for file documentation.
@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
Company : CEP - ARMINES (France)
http://www-cep.ensmp.fr/english/
@version


*/



#include "Dymola.h"
#include <vector>
#include "Variable.h"
#include <QtCore/QObject>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTextStream>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QProcess>
#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include "MOSettings.h"



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


bool Dymola::firstRun(QString moPath,QString modelToConsider,QString storeFolder,QString logFilePath)
{
    // Create Dymola script
    QString filePath = storeFolder+QDir::separator()+"MOFirstRun.mos";
    QFile file(filePath);
    if(file.exists())
    {
        file.remove();
    }
    file.open(QIODevice::WriteOnly);

    QString scriptText;


    scriptText.append("openModel(\""+moPath+"\")\n");
    scriptText.append("cd "+storeFolder+"\n");
    scriptText.append("experimentSetupOutput(textual=true)\n");
    scriptText.append("checkModel(\""+modelToConsider+"\",simulate=true)\n");
    scriptText.append("savelog(\""+logFilePath+"\")\n");
    scriptText.append("exit\n");

    QTextStream ts( &file );
    ts << scriptText;
    file.close();


    // Run script
    QString dymolaPath = MOSettings::getValue("path/dymolaExe").toString();
    QFileInfo dymolaBin(dymolaPath);
    if(!dymolaBin.exists())
    {
        infoSender.send(Info("Dymola executable not found. Please verify path in Settings",ListInfo::ERROR2));
        return false;
    }
    else
    {
        // delete previous dymosim.exe
        QFile dymoFile(storeFolder+QDir::separator()+"dymosim.exe");
        if(dymoFile.exists())
            dymoFile.remove();

        // launch script
        QProcess scriptProcess;
        QStringList args;
        args.push_back(filePath);

        //start process
        infoSender.send(Info("Launching Dymola..."));
        scriptProcess.start(dymolaPath,args);
        bool ok = scriptProcess.waitForFinished(-1);
        if(!ok)
        {
            QString msg("CreateProcess failed");
            infoSender.debug(msg);
            return false;
        }

        //look if it succeed
        bool success = dymoFile.exists();

        return success;
    }
}

bool Dymola::createDsin(QString moPath,QString modelToConsider,QString folder)
{
    // Create Dymola script
    QString filePath = folder+QDir::separator()+"MOFirstRun.mos";
    QFile file(filePath);
    if(file.exists())
    {
        file.remove();
    }
    file.open(QIODevice::WriteOnly);

    QString scriptText;
    scriptText.append("openModel(\""+moPath+"\")\n");
    scriptText.append("cd "+folder+"\n");
    scriptText.append("translateModel(\""+modelToConsider+"\")\n");
    scriptText.append("exportInitialDsin(\"dsin.txt\")\n");
    scriptText.append("exit\n");

    QTextStream ts( &file );
    ts << scriptText;
    file.close();

    // Run script
    QString dymolaPath = MOSettings::getValue("path/dymolaExe").toString();


    QProcess simProcess;
    QStringList args;
    args.push_back(filePath);


    // delete previous dsin file
    QFile dsinFile(folder+QDir::separator()+"dsin.txt");
    if(dsinFile.exists())
        dsinFile.remove();

    // launch script
    infoSender.send(Info("Launching Dymola..."));
    simProcess.start(dymolaPath, args);
    bool ok = simProcess.waitForFinished(-1);
    if(!ok)
    {
        QString msg("CreateProcess failed");
        infoSender.debug(msg);
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


void Dymola::start(QString path,int maxNSec)
{
#ifdef WIN32
    QProcess simProcess;
    simProcess.setWorkingDirectory(path);
		

   QString appPath = "\""+path+"\\"+"Dymosim.exe\"";


    simProcess.start(appPath, QStringList());

    int nmsec;
    if(maxNSec==-1)
        nmsec = -1;
    else
        nmsec = maxNSec*1000;

    bool ok = simProcess.waitForFinished(nmsec);
    if(!ok)
    {
        QString msg("CreateProcess failed.");
        infoSender.debug(msg);
        simProcess.close();
        return;
    }


    // when finish using wText dont forget to delete it
//    delete []wAppPath;
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


    int iPStopTime = parameters->findItem((int)Dymola::STOPTIME,MOParameter::INDEX);
	int iLStopTime = lines.indexOf(QRegExp(".* # StopTime .*"));
    if((iLStopTime>-1) && (iPStopTime>-1))
    {
		newLine =  "       "
                   +parameters->items.at(iPStopTime)->getFieldValue(MOParameter::VALUE).toString()
			+"                   # StopTime     Time at which integration stops";
        lines.replace(iLStopTime,newLine);
    }

    int iPTolerance = parameters->findItem((int)Dymola::TOLERANCE,MOParameter::INDEX);
    int iLTolerance = lines.indexOf(QRegExp(".*  # Tolerance .*"));
    if((iLTolerance>-1) && (iPTolerance>-1))
    {
        newLine =  "       "
                   +parameters->items.at(iPTolerance)->getFieldValue(MOParameter::VALUE).toString()
                   +"                   # nInterval    Relative precision of signals for \n                            #              simulation, linearization and trimming";
        lines.replace(iLTolerance,newLine);
    }

    int iPnInterval = parameters->findItem((int)Dymola::NINTERVAL,MOParameter::INDEX);
    int iLnInterval = lines.indexOf(QRegExp(".*  # nInterval .*"));
    if((iLnInterval>-1) && (iPnInterval>-1))
    {
        newLine =  "       "
                   +parameters->items.at(iPnInterval)->getFieldValue(MOParameter::VALUE).toString()
                   +"                   # nInterval    Number of communication intervals, if > 0";
        lines.replace(iLnInterval,newLine);
    }

    int iPSolver = parameters->findItem((int)Dymola::SOLVER,MOParameter::INDEX);
    int iLSolver = lines.indexOf(QRegExp(".*  # Algorithm .*"));
    if((iLSolver>-1) && (iPSolver>-1))
    {
        newLine =  "       "
                   +parameters->items.at(iPSolver)->getFieldValue(MOParameter::VALUE).toString()
                   +"                   # Algorithm    Integration algorithm as integer";
        lines.replace(iLSolver,newLine);
    }

	allDsinText = lines.join("\n");
}

void Dymola::getVariablesFromDsFile(QTextStream *text, MOVector<Variable> *variables,QString _modelName)

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
        newVariable->setName(_modelName+"."+line);
        variables->addItem(newVariable);
        line=text->readLine();
    }


    nbv=variables->items.size();


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

        variables->items[nv]->setValue(linefields[1].toDouble());
        variables->items[nv]->setDataType(linefields[5].toInt()%4); // use %4 to avoid Dymola variable definition bug

        line = text->readLine();
        nv++;
    }

    // Get variables' description
    indStartDesc = str.indexOf("initialDescription");
    text->seek(indStartDesc);
    text->readLine(); //pass through end of line

    line = text->readLine();
    nv=0;
    nbv=variables->items.size();
    while (!text->atEnd() && nv<nbv){
        variables->items[nv]->setDescription(line);
        line = text->readLine();
        nv++;
    }


}

void Dymola::getVariablesFromDsFile(QString fileName_, MOVector<Variable> *variables,QString _modelName)
{
    variables->clear();
    QFileInfo fileinfo = QFileInfo(fileName_);

    if (fileinfo.exists())
    {
        QFile file(fileinfo.filePath());
        file.open(QIODevice::ReadOnly);
        QTextStream* in = new QTextStream(&file);
        getVariablesFromDsFile(in, variables,_modelName);
        file.close();
    }
}

void Dymola::getFinalVariablesFromDsFile(QString fileName_, MOVector<Variable> *variables,QString _modelName)
{
    variables->clear();
    QFileInfo fileinfo = QFileInfo(fileName_);

    if (fileinfo.exists())
    {
        QFile file(fileinfo.filePath());
        file.open(QIODevice::ReadOnly);
        QTextStream* in = new QTextStream(&file);
        getFinalVariablesFromDsFile(in, variables,_modelName);
        file.close();
        delete in;
    }
}

void Dymola::getFinalVariablesFromDsFile(QTextStream *text, MOVector<Variable> *variables,QString _modelName)
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
        newVariable->setName(_modelName+"."+line);
        variables->addItem(newVariable);
        line=text->readLine();
    }
    nbv=variables->items.size();

    // **************************
    // Description
    // **************************
    indStartDesc = str.indexOf("char description(");
    text->seek(indStartDesc);
    line = text->readLine(); //go to next line
    line = text->readLine(); //read line

    nv=0;
    while (!line.isEmpty() && nv<nbv){
        variables->items[nv]->setDescription(line);
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
                variables->items.at(iV)->setValue(data1[nbRows1-1][-dataInfo2.at(iV)-1]);
            else
                variables->items.at(iV)->setValue(data1[nbRows1-1][dataInfo2.at(iV)-1]);
        }
        else
        {
            if(dataInfo2.at(iV)<0)
                variables->items.at(iV)->setValue(data2[nbRows2-1][-dataInfo2.at(iV)-1]);
            else
                variables->items.at(iV)->setValue(data2[nbRows2-1][dataInfo2.at(iV)-1]);
        }
    }


    for (int i = 0; i < nbRows1; i++)
        delete [] data1[i];
    delete [] data1;

    for (int i = 0; i < nbRows2; i++)
        delete [] data2[i];
    delete [] data2;

}


void Dymola::setVariablesToDsin(QString fileName, QString modelName,MOVector<Variable> *variables,MOParameters *parameters)
{

    //Reading Preamble
    QFileInfo fileinfo = QFileInfo(fileName);
    if (fileinfo.exists())
    {
        QFile file(fileinfo.filePath());
        file.open(QIODevice::ReadOnly);
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
        int prec=MOSettings::getValue("Dymola/MaxDigitsDsin").toInt(); //number of decimals
        QString value;
        for(int iV=0;iV<variables->items.size();iV++)
        {

            curVar = variables->items.at(iV);
            varName = curVar->name(Modelica::FULL);
            varName = varName.remove(modelName+".");
            rxLine.setPattern(sciNumRx()+"\\s+"+sciNumRx()+"\\s+"+sciNumRx()+"\\s+"+sciNumRx()+"\\s+"+sciNumRx()+"\\s+"+sciNumRx()+"\\s*#\\s*("+varName+")\\s*");

            // extracting only text around varname
            // to speed-up regexp research (too long without)
            index2 = allText.indexOf(varName);
            smallText.clear();
            while(index2>-1)
            {
                smallText += allText.mid(index2-300,300+varName.size())+"\n";
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
                if(capLines.size()>1)
                {
                    allText = allText.replace(capLines.at(0)+"\n"+capLines.at(1),newLine1+"\n"+newLine2);
                }
                else
                {
                    allText = allText.replace(capLines.at(0),newLine1+"\t"+newLine2);
                }
                qDebug(newLine1.toLatin1().data());
                qDebug(newLine2.toLatin1().data());
            }
            else
            {
                infoSender.send(Info("Warning : unable to set variable value (not found in init file):"+varName,ListInfo::ERROR2));
            }
        }
	


        fileinfo.setFile(fileName);
        file.setFileName(fileinfo.filePath());
        bool ok = file.open(QIODevice::WriteOnly);
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

