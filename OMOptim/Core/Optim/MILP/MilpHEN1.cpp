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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

 	@file MilpHEN1.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 0.9 

  */
#include "MilpHEN1.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <cmath>


MilpHEN1::MilpHEN1(EITree* eiTree,MOParameters *parameters,EIConnConstrs *connConstrs,
                       MOOptVector *variables,QDir folder,QString modFileName, QString dataFileName)
{
    _eiTree = new EITree(*eiTree);
    _variables = new MOOptVector(*variables);
    _connConstrs = new EIConnConstrs(*connConstrs);
    _parameters = new MOParameters(*parameters);

    _folder = folder;
    _modFileName =  modFileName;
    _dataFileName = dataFileName;


    _logFileName = "MILPLog.txt";
    _resFileName = "MILPResult.txt";
    _sensFileName= "MILPSens.txt";


    _QFlowUnit = MEQflow::W;
    _TempUnit = METemperature::K;
}

MilpHEN1::~MilpHEN1(void)
{
    delete _eiTree;
    delete _variables;
    delete _parameters;
    delete _connConstrs;
}

int MilpHEN1::hook(void *info, const char *s)
{
    QString msg(s);
    infoSender.send(Info("GLPK : "+msg));
    return 1;
}


EIHEN1Result * MilpHEN1::launch()
{
    //***********************
    // get data
    //***********************
    // get temperature intervals, flows, factors...
    QList<EIStream*> eiProcessStreams;
    QList<QList<MEQflow> > Qpk; //.at(iStream).at(iDTk)
    QList<EIStream*> eiUtilityStreams;
    QList<QList<MEQflow> > Quk;//.at(iStream).at(iDTk)
    QMultiMap<EIGroupFact*,EIStream*> factStreamMap; // multimap <unit multiplier, Streams concerned>,
    QMap<EIGroupFact*,EIGroupFact*> factsRelation; // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
    QMap<EIGroupFact*,EIGroup*> factGroupMap;


    EITools::getTkQpkQuk(_variables,
                         _eiTree->rootElement(),_Tk,
                         eiProcessStreams,Qpk,
                         eiUtilityStreams,Quk,
                         factStreamMap,
                         factsRelation,
                         factGroupMap);

    // write data
    QString dataFilePath = _folder.absoluteFilePath(_dataFileName);
    DataToFile(dataFilePath,eiProcessStreams,Qpk,eiUtilityStreams,Quk,
               factStreamMap,factsRelation,factGroupMap);


    glp_prob* glpProblem = launchGLPK();
    EIHEN1Result* result;
    if(!glpProblem)
    {
        result = new EIHEN1Result();
        result->setEITree(_eiTree);
        result->setSuccess(false);
    }
    else
    {
        result = readResult(glpProblem);
        glp_delete_prob(glpProblem);
    }

    result->_logFileName = _logFileName;
    result->_resFileName = _resFileName;
    result->_sensFileName = _sensFileName;

    return result;
}



void MilpHEN1::DataToFile(QString dataFilePath,
                            QList<EIStream*> &eiProcessStreams,
                            QList<QList<MEQflow> > &Qpk, //.at(iStream).at(iDTk)
                            QList<EIStream*> &eiUtilityStreams,
                            QList<QList<MEQflow> > &Quk,//.at(iStream).at(iDTk)
                            QMultiMap<EIGroupFact*,EIStream*> &factStreamMap, // multimap <unit multiplier, Streams concerned>,
                            QMap<EIGroupFact*,EIGroupFact*> &factsRelation, // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
                            QMap<EIGroupFact*,EIGroup*> &factGroupMap)
{

    //configuration                
    bool allInB = _parameters->value((int)EIHEN1Parameters::ALLOWSEVERAL,QVariant(true)).toBool(); // if true, more than one heat exchanger is permitted between each couple of streams
    bool allInNI = _parameters->value((int)EIHEN1Parameters::ALLOWNI,QVariant(true)).toBool();// if true, non-isothermal connections are allowed for every streams
    bool allSplitsAllowed = _parameters->value((int)EIHEN1Parameters::ALLOWSPLITS,QVariant(true)).toBool(); // if true, splits are allowed for all streams
    int kmax = _parameters->value((int)EIHEN1Parameters::KMAX,QVariant(10)).toInt();



    //sort Tk in descending order !!! (check)
    qSort(_Tk.begin(),_Tk.end(),METemperature::ThoterThan); //T(0) > T(1) > T(2)...

    QString zone("z1");



    // Create Data File
    QFile file(dataFilePath);
    if(file.exists())
    {
        file.remove();
    }
    file.open(QIODevice::WriteOnly);

    // Create text
    QString dataText;
    dataText.append("data;\n");

    // filter streams
    QList<EIStream*> hotStreams = EIReader::getHotStreams(eiProcessStreams);
    hotStreams.append(EIReader::getHotStreams(eiUtilityStreams));
    QList<EIStream*> coldStreams = EIReader::getColdStreams(eiProcessStreams);
    coldStreams.append(EIReader::getColdStreams(eiUtilityStreams));
    QList<EIStream*> huStreams = EIReader::getHotStreams(eiUtilityStreams);
    QList<EIStream*> cuStreams = EIReader::getColdStreams(eiUtilityStreams);

    //*************
    // param ndt
    //*************
    MilpParam0D paramndt("ndt");
    paramndt.addItem(QString::number(_Tk.size()-1));
    dataText.append(paramndt.toString());
    dataText.append("\n\n");

    //*************
    // sets
    //*************

    //Si : hot streams
    MilpSet0D setSi("Si");
    for(int i=0;i<hotStreams.size();i++)
        setSi.addItem(hotStreams.at(i)->name());
    dataText.append(setSi.toString());
    dataText.append("\n");


    //Sj : cold streams
    MilpSet0D setSj("Sj");
    for(int i=0;i<coldStreams.size();i++)
        setSj.addItem(coldStreams.at(i)->name());
    dataText.append(setSj.toString());
    dataText.append("\n");


    //HU : hot utility streams
    MilpSet0D setHU("HU");

    for(int i=0;i<huStreams.size();i++)
        setHU.addItem(huStreams.at(i)->name());
    dataText.append(setHU.toString());
    dataText.append("\n");

    //CU : cold utility streams
    MilpSet0D setCU("CU");

    for(int i=0;i<cuStreams.size();i++)
        setCU.addItem(cuStreams.at(i)->name());
    dataText.append(setCU.toString());
    dataText.append("\n");

    // Sz : zones
    MilpSet0D setSz("Sz");
    setSz.addItem(zone);
    dataText.append(setSz.toString());
    dataText.append("\n");

    // P : connections allowed
    // all combinations + removing forbidden ones
    // get forbidden <HotStream,ColdStream>
    QMultiMap<EIStream*,EIStream*> mapForbidden = _connConstrs->getMapStreams(_variables);

    MilpSet0D setP("P");
    for(int iH=0;iH<hotStreams.size();iH++)
    {
        for(int iC=0;iC<coldStreams.size();iC++)
        {
            //check if not forbidden
            if(!mapForbidden.values(hotStreams.at(iH)).contains(coldStreams.at(iC)))
                setP.addItem("("+hotStreams.at(iH)->name()+","+coldStreams.at(iC)->name()+")");
    }
    }
    dataText.append(setP.toString());
    dataText.append("\n");

    // SB : several connexions possible between streams
    MilpSet0D setB("B");
    //if allInB, add all allowed combinations
    if(allInB)
        setB.setItems(setP.items());
    dataText.append(setB.toString());
    dataText.append("\n");

    // SH : splits are allowed for hot streams
    MilpSet0D setSH("SH");
    if(allSplitsAllowed)
        setSH.setItems(setSi.items());
    dataText.append(setSH.toString());
    dataText.append("\n");

    // SC : splits are allowed for cold streams
    MilpSet0D setSC("SC");
    if(allSplitsAllowed)
        setSC.setItems(setSj.items());
    dataText.append(setSC.toString());
    dataText.append("\n");

    // NIH and NIC : non-isothermal connexions allowed
    MilpSet0D setNIH("NIH");
    MilpSet0D setNIC("NIC");
    if(allInNI)
    {
        //if allInNI, add all streams
        for(int iH=0;iH<hotStreams.size();iH++)
        {
            setNIH.addItem(hotStreams.at(iH)->name());
    }
        for(int iC=0;iC<coldStreams.size();iC++)
        {
            setNIC.addItem(coldStreams.at(iC)->name());
        }
    }
    dataText.append(setNIH.toString()+"\n");
    dataText.append(setNIC.toString()+"\n");



    //Hz : hot streams in z
    MilpSet1D setHz("Hz");
    for(int i=0;i<hotStreams.size();i++)
        setHz.addItem(zone,hotStreams.at(i)->name());
    dataText.append(setHz.toString());
    dataText.append("\n");

    //HUz : hot utility streams in z
    MilpSet1D setHUz("HUz");
    for(int i=0;i<huStreams.size();i++)
    {
        setHUz.addItem(zone,huStreams.at(i)->name());
    }
    dataText.append(setHUz.toString());
    dataText.append("\n");

    //Hmz : hot streams of zone z in temperature interval m
    MilpSet2D setHmz("Hmz");
    QList<EIStream*> hmzStreams;
    for(int idT=0;idT<_Tk.size()-1;idT++)
    {
        hmzStreams = EIReader::getStreamsPresentInDT(_Tk.at(idT),_Tk.at(idT+1),hotStreams);
        for(int iH=0;iH<hmzStreams.size();iH++)
        {
            setHmz.addItem(QString::number(idT+1),zone,hmzStreams.at(iH)->name());
        }
    }
    dataText.append(setHmz.toString());
    dataText.append("\n");

    //Cz : cold streams in z
    MilpSet1D setCz("Cz");
    for(int i=0;i<coldStreams.size();i++)
        setCz.addItem(zone,coldStreams.at(i)->name());
    dataText.append(setCz.toString());
    dataText.append("\n");

    //CUz : cold utility streams in z
    MilpSet1D setCUz("CUz");
    for(int i=0;i<cuStreams.size();i++)
    {
        setCUz.addItem(zone,cuStreams.at(i)->name());
    }
    dataText.append(setCUz.toString());
    dataText.append("\n");

    //Cnz : cold streams of zone z in temperature interval n
    MilpSet2D setCnz("Cnz");
    QList<EIStream*> cnzStreams;
    for(int idT=0;idT<_Tk.size()-1;idT++)
    {
        cnzStreams = EIReader::getStreamsPresentInDT(_Tk.at(idT),_Tk.at(idT+1),coldStreams);
        for(int iH=0;iH<cnzStreams.size();iH++)
        {
            setCnz.addItem(QString::number(idT+1),zone,cnzStreams.at(iH)->name());
        }
    }
    dataText.append(setCnz.toString());
    dataText.append("\n");


    // SUtGroups
    MilpSet0D setSUtGroups("SUtGroups");
    for(int i=0;i<factGroupMap.values().size();i++)
        setSUtGroups.addItem(factGroupMap.values().at(i)->name());
    dataText.append(setSUtGroups.toString());
    dataText.append("\n");

    // SUtStrGroups
    MilpSet0D setSUtStrGroups("SUtStrGroups");
    EIGroupFact* curGroupFact;
    EIGroup* curGroup;
    EIStream* curStream;
    for(int iFact=0;iFact<factStreamMap.uniqueKeys().size();iFact++)
    {
        curGroupFact = factStreamMap.uniqueKeys().at(iFact);
        curGroup = factGroupMap.value(curGroupFact);
        for(int iS=0;iS<factStreamMap.values(curGroupFact).size();iS++)
        {
            curStream = factStreamMap.values(curGroupFact).at(iS);
            setSUtStrGroups.addItem("("+curGroup->name()+","+curStream->name()+") ");
        }
    }
    dataText.append(setSUtStrGroups.toString());
    dataText.append("\n");



    //Mz : list of temperature interval in zone
    MilpSet1D setMz("Mz");
    for(int i=0;i<_Tk.size()-1;i++)
        setMz.addItem(zone,QString::number(i+1));
    dataText.append(setMz.toString());
    dataText.append("\n");


    //Mi0 : set of initial temperature interval of hot streams
    //Mif : set of final...
    //Miz temperature intervals where hot stream i is present
    MilpSet1D setMi0("mi0");
    MilpSet1D setMif("mif");
    MilpSet2D setMiz("Miz");
    int i0;
    int ifinal;
    QList<int> listiTconcerned;
    for(int i=0;i<hotStreams.size();i++)
    {
        // !!! HEN1 does not consider corrected temperatures !!
        // -> use false as argument in getTIntervalsConcerned
        listiTconcerned = EIReader::getTIntervalsConcerned(_Tk,hotStreams.at(i),false);
        if(listiTconcerned.size()>0)
        {
            i0 = listiTconcerned.first();
            ifinal = listiTconcerned.last();

            setMi0.addItem(hotStreams.at(i)->name(),QString::number(i0+1));
            setMif.addItem(hotStreams.at(i)->name(),QString::number(ifinal+1));

            for(int j=0;j<listiTconcerned.size();j++)
                setMiz.addItem(hotStreams.at(i)->name(),zone,QString::number(listiTconcerned.at(j)+1));
        }
    }
    dataText.append(setMi0.toString()+"\n");
    dataText.append(setMif.toString()+"\n");
    dataText.append(setMiz.toString()+"\n");

    //Nj0 : set of initial temperature interval of cold streams
    //Njf : set of final...
    //Njz : temperature intervals where cold stream j is present
    MilpSet1D setNj0("nj0");
    MilpSet1D setNjf("njf");
    MilpSet2D setNjz("Njz");
    int j0;
    int jfinal;
    for(int i=0;i<coldStreams.size();i++)
    {
        // !!! HEN1 does not consider corrected temperatures !!
        // -> use false as argument in getTIntervalsConcerned
        listiTconcerned = EIReader::getTIntervalsConcerned(_Tk,coldStreams.at(i),false);
        if(listiTconcerned.size()>0)
        {
            j0 = listiTconcerned.first();
            jfinal = listiTconcerned.last();

            setNj0.addItem(coldStreams.at(i)->name(),QString::number(j0+1));
            setNjf.addItem(coldStreams.at(i)->name(),QString::number(jfinal+1));

            for(int j=0;j<listiTconcerned.size();j++)
                setNjz.addItem(coldStreams.at(i)->name(),zone,QString::number(listiTconcerned.at(j)+1));
        }
    }
    dataText.append(setNj0.toString()+"\n");
    dataText.append(setNjf.toString()+"\n");
    dataText.append(setNjz.toString()+"\n");


    // authorized connections within temperature interval
    MilpSet2D setPimH("PimH");
    QStringList tempIntervals;
    QString hotStreamName,coldStreamName;
    for(int iH=0;iH<hotStreams.size();iH++)
    {
        hotStreamName = hotStreams.at(iH)->name();
        tempIntervals = setMiz.items().values(MilpKey2D(hotStreamName,zone));

        for(int iT=0;iT<tempIntervals.size();iT++)
        {
            for(int iC=0;iC<coldStreams.size();iC++)
            {
                coldStreamName = coldStreams.at(iC)->name();
                //check if not forbidden
                if(!mapForbidden.values(hotStreams.at(iH)).contains(coldStreams.at(iC)))
                    setPimH.addItem(hotStreamName,tempIntervals.at(iT),coldStreamName);
        }
    }
    }
    dataText.append(setPimH.toString()+"\n");

    // authorized connections within temperature interval
    MilpSet2D setPjnC("PjnC");
    for(int iC=0;iC<coldStreams.size();iC++)
    {
        coldStreamName = coldStreams.at(iC)->name();
        tempIntervals = setNjz.items().values(MilpKey2D(coldStreamName,zone));

        for(int iT=0;iT<tempIntervals.size();iT++)
        {
            for(int iH=0;iH<hotStreams.size();iH++)
            {
                hotStreamName = hotStreams.at(iH)->name();
                //check if not forbidden
                if(!mapForbidden.values(hotStreams.at(iH)).contains(coldStreams.at(iC)))
                    setPjnC.addItem(coldStreamName,tempIntervals.at(iT),hotStreamName);
        }
    }
    }
    dataText.append(setPjnC.toString()+"\n");


    //****************************
    // Parameters
    //****************************
    //kmax
    MilpParam0D paramKmax("kmax");
    paramKmax.addItem(QString::number(kmax));
    dataText.append(paramKmax.toString()+"\n");


    //Utility groups parameters


    MilpParam1D paramFixUtCost("FixUtCost");
    MilpParam1D paramVarUtCost("VarUtCost");
    MilpParam1D paramUtFactMax("UtFactMax");
    MilpParam1D paramUtFactMin("UtFactMin");

    for(int iFact=0;iFact<factGroupMap.uniqueKeys().size();iFact++)
    {
        curGroupFact = factGroupMap.uniqueKeys().at(iFact);
        curGroup = factGroupMap.value(curGroupFact);

        paramUtFactMax.addItem(curGroup->name(),QString::number(curGroupFact->max));
        paramUtFactMin.addItem(curGroup->name(),QString::number(curGroupFact->min));
        paramFixUtCost.addItem(curGroup->name(),curGroup->getFieldValue(EIGroup::COSTFIX).toString());
        paramVarUtCost.addItem(curGroup->name(),curGroup->getFieldValue(EIGroup::COSTMULT).toString());
    }
    dataText.append(paramFixUtCost.toString()+"\n");
    dataText.append(paramVarUtCost.toString()+"\n");
    dataText.append(paramUtFactMax.toString()+"\n");
    dataText.append(paramUtFactMin.toString()+"\n");





    //DHimzH
    MilpParam3D paramDHimzH("DHimzH");
    tempIntervals = setMz.items().values(zone);
    MEQflow dQ;
    for(int iH=0;iH<hotStreams.size();iH++)
    {
        hotStreamName = hotStreams.at(iH)->name();

        for(int iT=0;iT<tempIntervals.size();iT++)
        {
            dQ = EIReader::getIntervalQFlow(_Tk.at(tempIntervals.at(iT).toInt()-1),_Tk.at(tempIntervals.at(iT).toInt()),hotStreams.at(iH));
            paramDHimzH.addItem(hotStreamName,tempIntervals.at(iT),zone,QString::number(dQ.value(_QFlowUnit)));
        }
    }
    dataText.append(paramDHimzH.toString()+"\n");

    //DHjnzC
    MilpParam3D paramDHjnzC("DHjnzC");
    tempIntervals = setMz.items().values(zone);
    for(int iC=0;iC<coldStreams.size();iC++)
    {
        coldStreamName = coldStreams.at(iC)->name();

        for(int iT=0;iT<tempIntervals.size();iT++)
        {
            dQ = EIReader::getIntervalQFlow(_Tk.at(tempIntervals.at(iT).toInt()-1),_Tk.at(tempIntervals.at(iT).toInt()),coldStreams.at(iC));
            paramDHjnzC.addItem(coldStreamName,tempIntervals.at(iT),zone,QString::number(dQ.value(_QFlowUnit)));
        }
    }
    dataText.append(paramDHjnzC.toString()+"\n");


    //DTi
    METemperature dT;
    MilpParam1D paramDTi("DTi");
    for(int iH=0;iH<hotStreams.size();iH++)
    {
        dT = hotStreams.at(iH)->TinNum - hotStreams.at(iH)->ToutNum;
        paramDTi.addItem(hotStreams.at(iH)->name(),QString::number(dT.value()));
    }
    dataText.append(paramDTi.toString()+"\n");

    //DTj
    MilpParam1D paramDTj("DTj");
    for(int iC=0;iC<coldStreams.size();iC++)
    {
        dT = coldStreams.at(iC)->ToutNum - coldStreams.at(iC)->TinNum;
        paramDTj.addItem(coldStreams.at(iC)->name(),QString::number(dT.value()));
    }
    dataText.append(paramDTj.toString()+"\n");

    //TmL TmU
    MilpParam1D paramTmL("TmL");
    MilpParam1D paramTmU("TmU");
    for(int iT=0;iT<_Tk.size()-1;iT++)
    {
        paramTmU.addItem(QString::number(iT+1),QString::number(_Tk.at(iT).value(_TempUnit)));
        paramTmL.addItem(QString::number(iT+1),QString::number(_Tk.at(iT+1).value(_TempUnit)));
    }
    dataText.append(paramTmL.toString()+"\n");
    dataText.append(paramTmU.toString()+"\n");

    //DTmnML
    MilpParam2D paramdTlm("DTmnML");
    double dT1;
    double dT2;
    double dTlm;
    for(int iT1=0;iT1<_Tk.size()-1;iT1++)
    {

        for(int iT2=0;iT2<iT1;iT2++)
        {
            dTlm=0;
            paramdTlm.addItem(QString::number(iT1+1),QString::number(iT2+1),QString::number(dTlm));
        }

        for(int iT2=iT1;iT2<_Tk.size()-1;iT2++)
        {
            dT1 = _Tk.at(iT1).value()-_Tk.at(iT2).value();
            dT2 = _Tk.at(iT1+1).value()-_Tk.at(iT2+1).value();
            dTlm = EITools::DTlm(dT1,dT2);
            paramdTlm.addItem(QString::number(iT1+1),QString::number(iT2+1),QString::number(dTlm));
        }
    }
    dataText.append(paramdTlm.toString()+"\n");
    dataText +="end;";

    QTextStream ts( &file );
    ts << dataText;
    file.close();
}

glp_prob* MilpHEN1::launchGLPK()
{

    //delete log file
    _folder.remove(_logFileName);

    //delete result file
    _folder.remove(_resFileName);

    //delete sensitivity file
    _folder.remove(_sensFileName);

    //Hook
    // redirect terminal output
    glp_term_hook(hook, NULL);



    glp_prob *glpProblem;
    glp_tran *tran;
    int ret;
    glpProblem = glp_create_prob();
    tran = glp_mpl_alloc_wksp();
    ret = glp_mpl_read_model(tran, _folder.absoluteFilePath(_modFileName).toLatin1().data(), 1);
    if (ret != 0)
    {
        infoSender.send(Info(ListInfo::MILPERRORMODEL));
        return NULL;
    }
    ret = glp_mpl_read_data(tran, _folder.absoluteFilePath(_dataFileName).toLatin1().data());
    if (ret != 0)
    {
        infoSender.send(Info(ListInfo::MILPERRORDATA));
        return NULL;
    }
    ret = glp_mpl_generate(tran, NULL);
    if (ret != 0)
    {
        infoSender.send(Info(ListInfo::MILPERRORDATA));
        return NULL;
    }

    glp_mpl_build_prob(tran, glpProblem);
    glp_simplex(glpProblem, NULL);
    glp_intopt(glpProblem, NULL);
    ret = glp_mpl_postsolve(tran, glpProblem, GLP_MIP);
    if (ret != 0)
        fprintf(stderr, "Error on postsolving model\n");

    glp_mpl_free_wksp(tran);
    glp_print_sol(glpProblem, _folder.absoluteFilePath(_logFileName).toUtf8().data());
    glp_print_ranges(glpProblem,0,0,0,_folder.absoluteFilePath(_sensFileName).toUtf8().data());


    // get results
    int nbRows = glp_get_num_rows(glpProblem);
    int nbCols = glp_get_num_cols(glpProblem);

    QString resText;
    QString rowName;
    QString colName;
    double value;
    for(int iRow = 1;iRow<=nbRows; iRow++)
    {
        rowName = QString(glp_get_row_name(glpProblem, iRow));
        value = glp_mip_row_val(glpProblem, iRow);
        resText += "Row[" + QString::number(iRow)+"] : ";
        resText += rowName + " : ";
        resText += QString::number(value) +"\n";
    }
    for(int iCol = 1;iCol<=nbCols; iCol++)
    {
        colName = QString(glp_get_col_name(glpProblem, iCol));
        value = glp_mip_col_val(glpProblem, iCol);
        resText += "Col[" + QString::number(iCol)+"] : ";
        resText += colName + " : ";
        resText += QString::number(value) +"\n";
    }

    QFile resFile(_folder.absoluteFilePath(_resFileName));
    resFile.open(QIODevice::WriteOnly);
    QTextStream ts( &resFile );
    ts << resText;
    resFile.close();

    // resume terminal output
    glp_term_hook(NULL, NULL);

    return glpProblem;
}


EIHEN1Result* MilpHEN1::readResult(glp_prob * glpProblem)
{

    // clone eiTree
    EIHEN1Result* result = new EIHEN1Result();
    result->setEITree(_eiTree);

    // read if successfull
    int status = glp_get_status(glpProblem);
    switch(status)
    {
    case GLP_OPT :
        result->setSuccess(true);
        break;
    default:
        result->setSuccess(false);
    }


    // read all column names
    int nbCols = glp_get_num_cols(glpProblem);
    QStringList colNames;
    for(int iCol = 1;iCol<=nbCols; iCol++)
    {
        colNames.push_back(QString(glp_get_col_name(glpProblem, iCol)));
    }

    //*************************
    // extract information
    //*************************

    QRegExp regExp;
    QString colName;
    QString groupName;
    double value;
    int iCol;

    // read all UtFact
    MilpVariableResult1D varUtFact("UtFact");
    varUtFact.fill(glpProblem,colNames);
    QMap<QString,double> mapGroupFacMul = varUtFact.values();

    EIGroup* curGroup;
    for(int i=0;i<mapGroupFacMul.keys().size();i++)
    {
        groupName = mapGroupFacMul.keys().at(i);
        curGroup =result->eiTree()->findItem(groupName);
        if(curGroup)
        {
            value = mapGroupFacMul.value(groupName);
            curGroup->getFact()->value = value;
        }
    }

    // read all UtEnabled
    MilpVariableResult1D varUtEnabled("UtEnabled");
    varUtEnabled.fill(glpProblem,colNames);
    QMap<QString,double> mapGroupEnabled = varUtEnabled.values();

    for(int i=0;i<mapGroupEnabled.keys().size();i++)
    {
        groupName = mapGroupEnabled.keys().at(i);
        curGroup =result->eiTree()->findItem(groupName);
        if(curGroup)
        {
            curGroup->setChecked((bool)mapGroupEnabled.value(groupName));
        }
    }

    // read TotalCost
    MilpVariableResult0D varTotalCost("TotalCost");
    varTotalCost.fill(glpProblem,-1,colNames);
    result->_totalCost = varTotalCost.value();

    // read TotalArea
    MilpVariableResult0D varTotalArea("TotalArea");
    varTotalArea.fill(glpProblem,-1,colNames);
    result->_totalArea = varTotalArea.value();


    // read HENumber
    MilpVariableResult0D varHENumber("HENumber");
    varHENumber.fill(glpProblem,-1,colNames);
    result->_HENumber = varHENumber.value();


    result->setEIConns(readEIConns(glpProblem,colNames));


    return result;
}



EIConns* MilpHEN1::readEIConns(glp_prob * glpProblem,QStringList colNames)
    {
    EIConns* conns = new EIConns();

    // KijmzH
    MilpVariableResult4D varKijmzH("KijmzH");
    varKijmzH.fill(glpProblem,colNames);

    // KijmzH
    MilpVariableResult4D varKijnzC("KijnzC");
    varKijnzC.fill(glpProblem,colNames);

    // KeijmzH
    MilpVariableResult4D varKeijmzH("KeijmzH");
    varKeijmzH.fill(glpProblem,colNames);

    // KeijmzH
    MilpVariableResult4D varKeijnzC("KeijnzC");
    varKeijnzC.fill(glpProblem,colNames);

    // QijmzH
    MilpVariableResult4D varQijmzH("QijmzH");
    varQijmzH.fill(glpProblem,colNames);


    // get heat exchanger begin keys
    QList<MilpKey4D> beginHKeys = varKijmzH.values().keys(1);
    QList<MilpKey4D> beginCKeys = varKijnzC.values().keys(1);


    // get heat exchanger endkeys
    QList<MilpKey4D> endHKeys = varKeijmzH.values().keys(1);
    QList<MilpKey4D> endCKeys = varKeijnzC.values().keys(1);





    MilpKey4D curBHKey,curEHKey,curBCKey,curECKey;
    int iEH,iBC,iEC;
    int kBH,kEH,kBC,kEC;
    bool okBH,okEH,okBC,okEC;
    bool found;
    QString strHot,strCold;
    MEQflow qflow;
    // for each hot beginning
    for(int iBH=0;iBH<beginHKeys.size();iBH++)
        {
        curBHKey=beginHKeys.at(iBH);
        strHot = curBHKey.i1();
        strCold = curBHKey.i2();

        // find hot end

        iEH=0;
        found=false;
        while((iEH<endHKeys.size())&&(!found))
            {
               if((endHKeys.at(iEH).i1()==strHot)&&(endHKeys.at(iEH).i2()==strCold))
                   found=true;
               else
                   iEH++;
        }
        if(!found)
            infoSender.debug("Error HEN: could not found HEN hot end (i1="+strHot+",i2="+strCold+")");
        else
        {
            curEHKey = endHKeys.at(iEH);

            // find cold begining
            found = false;
            iBC=0;

            while((iBC<beginCKeys.size())&&(!found))
                {
                   if((beginCKeys.at(iBC).i1()==strHot)&&(beginCKeys.at(iBC).i2()==strCold))
                       found=true;
                   else
                       iBC++;
                }
            if(!found)
                infoSender.debug("Error HEN: could not found HEN cold begin (i1="+strHot+",i2="+strCold+")");
            else
            {
                // find cold end
                found = false;
                iEC=0;

                while((iEC<endCKeys.size())&&(!found))
                {
                       if((endCKeys.at(iEC).i1()==strHot)&&(endCKeys.at(iEC).i2()==strCold))
                           found=true;
                       else
                           iEC++;
            }
                if(!found)
                    infoSender.debug("Error HEN: could not found HEN cold end (i1="+strHot+",i2="+strCold+")");
                else
                {
                    //Create connection


                    EIConn *newConn = new EIConn(_eiTree);
                    // still does not support mass fractions -> set to 1

                    kBH = curBHKey.i3().toInt(&okBH);
                    kEH = curEHKey.i3().toInt(&okEH);
                    kBC = curBCKey.i3().toInt(&okBC);
                    kEC = curECKey.i3().toInt(&okEC);

                    Q_ASSERT(okBH && okEH && okBC && okEC);

                    Q_ASSERT(kBC<=kEC); // to check, don't know order yet!

                    newConn->setA(_eiTree->findItem(strHot),_Tk.at(kBH-1),_Tk.at(kEH),1);
                    newConn->setB(_eiTree->findItem(strCold),_Tk.at(kBC),_Tk.at(kEC-1),1);

                    //calculate qflow
                    MilpKey4D curKey;
                    qflow.setValue(0);
                    for(int k=kBH;k<=kEH;k++)
                    {
                        curKey=MilpKey4D(curBHKey.i1(),curBHKey.i2(),QString::number(k),curBHKey.i4());
                        qflow += MEQflow(varQijmzH.values().value(curKey,0),_QFlowUnit);
        }
                    newConn->setQFlow(qflow);
                    conns->addItem(newConn);
    }
}
        }
    }


    return conns;
}


