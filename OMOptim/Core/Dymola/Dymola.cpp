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


bool Dymola::firstRun(QString moPath,QString modelToConsider,QString storeFolder)
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

    // loading libraries
    /*for(int i=0; i < libraries->items.size(); i ++)
	{
	QString _libPath = libraries->items.at(i)->filePath();
	QString _libName = libraries->items.at(i)->name(Modelica::FULL);
	if(QFileInfo(_libPath).exists() && (_libName!="Modelica"))
	scriptText.append("openModel(\""+_libPath+"\")\n");
	}*/


    scriptText.append("openModel(\""+moPath+"\")\n");
    scriptText.append("cd "+storeFolder+"\n");
    scriptText.append("experimentSetupOutput(textual=true)\n");
    scriptText.append("checkModel(\""+modelToConsider+"\",simulate=true)\n");
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
        QString command("\" \""+dymolaPath+"\" \""+filePath+"\" \"");
        //QString command("\""+filePath+"\"");
        command.replace("/","\\");
        //command.replace("\"","\\\"");
        //command.append(" >> C:\\log.txt");

        char* ccommand = command.toLatin1().data();
        std::cout << ccommand;
        char* ccommand2 = command.toAscii().data();
        std::cout << ccommand2;


        // delete previous dymosim.exe
        QFile dymoFile(storeFolder+QDir::separator()+"dymosim.exe");
        if(dymoFile.exists())
            dymoFile.remove();

        // launch script
        system(command.toAscii().data());

        // move file to storeFolder (dymola always compile in .mo folder)


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
    QString command("\" \""+dymolaPath+"\" \""+filePath+"\" \"");
    command.replace("/","\\");

    char* ccommand = command.toLatin1().data();
    std::cout << ccommand;
    char* ccommand2 = command.toAscii().data();
    std::cout << ccommand2;


    // delete previous dsin file
    QFile dsinFile(folder+QDir::separator()+"dsin.txt");
    if(dsinFile.exists())
        dsinFile.remove();

    // launch script
    system(command.toAscii().data());

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


void Dymola::start(QString path)
{
#ifdef WIN32
    bool cdOk = SetCurrentDirectory(path.utf16());
		
//    STARTUPINFO si;
//    PROCESS_INFORMATION pi;

//    ZeroMemory( &si, sizeof(si) );
//    si.cb = sizeof(si);
//    si.cb = sizeof(STARTUPINFO);
//    si.dwFlags = STARTF_USESHOWWINDOW;
//    si.wShowWindow = SW_HIDE;

//    ZeroMemory( &pi, sizeof(pi) );

   QString appPath = "\""+path+"\\"+"Dymosim.exe\"";
//	//appPath = appPath.replace("/","\\");
//	LPSTR lAppPath = (LPSTR)VQTConvert::QString_To_LPCSTR(appPath);

//    // Start the child process.
//    if( !CreateProcess( NULL,   // No module name (use command line)
//                        (LPWSTR)appPath.utf16(),        // Command line
//                        NULL,           // Process handle not inheritable
//                        NULL,           // Thread handle not inheritable
//                        FALSE,          // Set handle inheritance to FALSE
//                        CREATE_NO_WINDOW,              // No creation flags
//                        NULL,           // Use parent's environment block
//                        NULL,           // Use parent's starting directory
//                        &si,            // Pointer to STARTUPINFO structure
//                        &pi )           // Pointer to PROCESS_INFORMATION structure
//        )
//	{
//		QString msg("CreateProcess failed (%d).\n");
//        msg.sprintf(msg.toLatin1().data(),GetLastError());
//        infoSender.debug(msg);
//        return;
//    }

//    // Wait until child process exits.
//    WaitForSingleObject( pi.hProcess, INFINITE );

//    // Close process and thread handles.
//    CloseHandle( pi.hProcess );
//    CloseHandle( pi.hThread );
    QProcess simProcess;
    simProcess.start(appPath, QStringList());
    bool ok = simProcess.waitForFinished();
    if(!ok)
    {
                    QString msg("CreateProcess failed (%d).\n");
                    msg.sprintf(msg.toLatin1().data(),GetLastError());
                    infoSender.debug(msg);
                    return;
    }


    // when finish using wText dont forget to delete it
//    delete []wAppPath;
#endif
}

//QString Dymola::getPreambleFromDsin(QTextStream *text)
//{
//	QString str = text->readAll(); 
//	int indStartVariables = str.indexOf("#    Names of initial variables");
//	return str.left(indStartVariables);
//}

void Dymola::modifyPreamble(QString &allDsinText,MOVector<ModModelParameter> *parameters)
{
	QString newLine;

    QStringList lines = allDsinText.split("\n");
    int iLForm = lines.indexOf(QRegExp(".* # lform .*"));
    if(iLForm>-1)
    {
        newLine =  " 0                         # lform    0/1 ASCII/Matlab-binary storage format of results";
        lines.replace(iLForm,newLine);
    }


	int iPStopTime = parameters->findItem((int)Dymola::STOPTIME,ModModelParameter::INDEX);
	int iLStopTime = lines.indexOf(QRegExp(".* # StopTime .*"));
    if((iLStopTime>-1) && (iPStopTime>-1))
    {
		newLine =  "       "
			+parameters->items.at(iPStopTime)->getFieldValue(ModModelParameter::VALUE).toString()
			+"                   # StopTime     Time at which integration stops";
        lines.replace(iLStopTime,newLine);
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

        //		variables->items[nv]->setType(linefields[0].toInt());
        variables->items[nv]->setValue(linefields[1].toDouble());
        //		variables->items[nv]->setMin(linefields[2].toDouble());
        //		variables->items[nv]->setMax(linefields[3].toDouble());
        //		variables->items[nv]->setCategory(linefields[4].toInt());
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


void Dymola::setVariablesToDsin(QString fileName, QString modelName,MOVector<Variable> *variables,MOVector<ModModelParameter> *parameters)
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
        modifyPreamble(allText,parameters);

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
                fields = rxLine.capturedTexts();
                capLines = rxLine.cap(0).split("\n");
                newLine1 = fields.at(1)+"\t"+curVar->getFieldValue(Variable::VALUE).toString() +"\t";
                newLine1 += fields.at(3)+"\t"+fields.at(4);
                newLine2 = fields.at(5)+"\t"+fields.at(6)+"\t"+" # "+fields.at(7);
                int test1 = allText.indexOf(capLines.at(0));
                int test2 = allText.indexOf(capLines.at(1));
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
	
        //varNamesString += (curVar->name(Modelica::DYMOLA)

        ////Delete file
        ////QFile::remove(fileName_);


        ////Creating new content
        //for(nv=0;nv<variables->items.size();nv++)
        //{
        //	curVar=variables->items.at(nv);

        //	varNamesString += (curVar->name(Modelica::DYMOLA) + "\n");
        //	if (curVar->name().size()>maxNameSize)
        //	{maxNameSize=curVar->name().size();}

        //	varDescString += (curVar->getDesc() + "\n");
        //	if (curVar->getDesc().size()>maxDescSize)
        //	{maxDescSize=curVar->getDesc().size();}

        //	if(nv==80)
        //	{
        //		int a=2;
        //	}
        //	varValuesString += (variableToValueLines(curVar) + "\n");
        //}

        ////First lines of each part
        //varNamesPreamble = "#    Names of initial variables \n";
        //varNamesPreamble += "char initialName("+QString::number(variables->items.size());
        //varNamesPreamble += ","+QString::number(maxNameSize)+")\n";

        //varDescPreamble += "\nchar initialDescription("+QString::number(variables->items.size());
        //varDescPreamble += ","+QString::number(maxDescSize)+")\n";

        //varValuesPreamble += "\ndouble initialValue("+QString::number(variables->items.size());
        //varValuesPreamble += ",6)\n";


        ////Writing in file
        //newTextString = "";
        //newTextString += preambleString;
        //newTextString += varNamesPreamble;
        //newTextString += varNamesString;
        //newTextString += varValuesPreamble;
        //newTextString += varValuesString;
        //newTextString += varDescPreamble;
        //newTextString += varDescString;

		

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


//QString Dymola::variableToValueLines(Variable* variable)
//{
//	QString line = "";
//
//	//First line
//	line += QString::number(variable->getFieldValue(Variable::TYPE).toInt());
//	line += "       ";
//	line += QString::number(variable->getFieldValue(Variable::VALUE).toDouble());
//	line += "                  ";
//	line += QString::number(variable->getFieldValue(Variable::MIN).toDouble());
//	line += "  ";
//	double max = variable->getFieldValue(Variable::MAX).toDouble();
//	if(max == 1e100)
//		line += "1.0E+100";
//	else
//		line += QString::number(max);
//	line += "\n";
//
//	//Second line
//	line += QString::number(variable->getFieldValue(Variable::CATEGORY).toInt());
//	line += "  ";
//	line += QString::number(variable->getFieldValue(Variable::DATATYPE).toInt());
//	line += "  ";
//	line += ("# " + variable->name(Modelica::DYMOLA));
//
//	return line;
//}
