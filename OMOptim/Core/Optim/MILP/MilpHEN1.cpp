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
 * Main contributor 2011, Hubert Thierot, CEP - ARMINES (France)

  @file MilpHEN1.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "MilpHEN1.h"
#ifdef WIN32
#include <windows.h>
#endif
#include <cmath>

#include "CERESInfo.h"

bool GlpkCallBack::_stop;

MilpHEN1::MilpHEN1(const EITree &eiTree,const MOParameters &parameters,const EIConnConstrs &connConstrs,
                   const MOOptVector &variables,QDir folder,METemperature TPinch)
{
    _eiTree = new EITree(eiTree);
    _variables = new MOOptVector(variables);
    _connConstrs = new EIConnConstrs(connConstrs);
    _parameters = new MOParameters(parameters);

    _folder = folder;
    _TPinch = TPinch;

    _modFilePath = getModFilePath();


    _logFileName = "MILPLog.txt";
    _resFileName = "MILPResult.txt";
    _sensFileName= "MILPSens.txt";
    _dataFileName = "EI.dat";
    _mpsFileName = "problem.mps";


    _QFlowUnit = MEQflow::W;
    _TempUnit = METemperature::K;
    _MassFlowUnit = MEMassFlow::KG_S;
    _surfaceUnit = MESurface::M2;
    _htUnit = MEHTCoeff::W_M2_K;
    _specCpUnit = MESpecHeatCapacity::J_KG_K;


    _glpProblem = NULL;
}

QString MilpHEN1::getModFilePath()
{

#ifdef DEBUG
    // if debug, uses directly file in MILP folder
    // this allows to modify .mo and reload it without recompiling
    QDir modDir(QApplication::applicationDirPath());
    modDir.cdUp();
    modDir.cd("Core");
    modDir.cd("Optim");
    modDir.cd("MILP");
    QString modFilePath = modDir.absoluteFilePath("MilpHEN.mod");
    return modFilePath;

#else
    // if not debug, .mo file is first copied from resource and then, its filepath is returned
    QFile moFile(":/MILP/MilpHEN.mod");
    qDebug(moFile.fileName().toLatin1().data());
    QString newPath = QApplication::applicationDirPath()+QDir::separator()+QFileInfo(moFile).fileName();
    QFileInfo newInfo(newPath);
    if(newInfo.exists())
        newInfo.absoluteDir().remove(newInfo.fileName());

    moFile.copy(newPath);

    return newPath;
#endif

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


    if(msg.right(1)=="\n")
        msg.remove(msg.length()-1,1);
    infoSender.send(Info("GLPK : "+msg));
    return 1;
}

EIHEN1Parameters::Solver MilpHEN1::solver()
{
    return (EIHEN1Parameters::Solver)_parameters->value(EIHEN1Parameters::SOLVER).toInt();
}

bool MilpHEN1::shouldSplitTPinch()
{
    return _parameters->value((int)EIHEN1Parameters::SPLITPINCH).toBool();
}

EIHEN1Result * MilpHEN1::launch()
{
    switch(_parameters->value(EIHEN1Parameters::PRESET).toInt())
    {
    case EIHEN1Parameters::FAST :
        return autoLaunchFast();

    case EIHEN1Parameters::PRECISE :
        return autoLaunchPrecise();

    case EIHEN1Parameters::CUSTOM :
        return launchCustom();

    }
}

EIHEN1Result * MilpHEN1::launchCustom()
{
    QString msg;

    QList<METemperature> Tk;

    bool dataOk = prepareData(Tk);
    bool launchOk;

    if(!dataOk)
        return NULL;

    switch(solver())
    {
    case EIHEN1Parameters::GLPK :
        launchOk = launchGLPK(msg);
        if(launchOk)
            return  getGLPKResult(Tk);
        else
        {
            infoSender.send(Info("Problem failed : "+msg,ListInfo::ERROR2));
            return NULL;
        }
    case EIHEN1Parameters::CBC :
        launchOk = launchCBC(msg);
        if(launchOk)
            return  getCBCResult(Tk);
        else
        {
            infoSender.send(Info("Problem failed : "+msg,ListInfo::ERROR2));
            return NULL;
        }
    default:
        return NULL;
    }
}

void MilpHEN1::stop()
{
    if(solver()==EIHEN1Parameters::CBC)
    {
        _cbcCtrl->stop();
    }

    if(solver()==EIHEN1Parameters::GLPK)
    {
        _glpkCtrl->stop();
    }
}

void MilpHEN1::prepareTk( const QList<EIStream*> &allStreams,
                          QList<METemperature> &Tk)
{

    //configuration
    bool splitTinterval = _parameters->value((int)EIHEN1Parameters::SPLITDT,QVariant(true)).toBool();
    double splitTstep = _parameters->value((int)EIHEN1Parameters::SPLITDTSTEP,QVariant(true)).toDouble();


    // Temperature interval sorting and spliting
    if(splitTinterval)
    {
        splitTk(Tk,splitTstep);
    }
    else
    {
        //sort Tk in descending order !!! (check)
        qSort(Tk.begin(),Tk.end(),METemperature::ThoterThan); //T(0) > T(1) > T(2)...
    }

    QList<int> tkConcerned,tkConcernedA,tkConcernedB;
    QList<METemperature> TU;
    QList<METemperature> TL;
    METemperature curStreamTPinch;

    // be sure each stream at least holds in three intervals
    for(int i=0;i<allStreams.size();i++)
    {
        tkConcerned = EIReader::getTIntervalsConcerned(Tk,allStreams.at(i),false);
        TU.clear();
        TL.clear();
        for(int j=0;j<tkConcerned.size();j++)
        {
            TU.push_back(Tk.at(tkConcerned.at(j)));
            TL.push_back(Tk.at(tkConcerned.at(j)+1));
        }

        if(!shouldSplitTPinch())
        {

            if(tkConcerned.size()<3)
            {

                Q_ASSERT(tkConcerned.size()>0);

                if(tkConcerned.size()==1)
                {
                    Tk.insert(tkConcerned.at(0)+1,TL[0]+(TU[0]-TL[0])*1/3);
                    Tk.insert(tkConcerned.at(0)+1,TL[0]+(TU[0]-TL[0])*2/3);
                }
                if(tkConcerned.size()==2)
                {
                    if((TU[0]-TL[0])>(TU[1]-TL[1]))
                        Tk.insert(tkConcerned.at(0)+1,TL[0]+(TU[0]-TL[0])*1/2);
                    else
                        Tk.insert(tkConcerned.at(1)+1,TL[1]+(TU[1]-TL[1])*1/2);
                }
            }
        }
        else
        {
            // adding all streams TPinch (where TCorrected = TPinch)
            if(allStreams.at(i)->isHot())
                curStreamTPinch = _TPinch+allStreams.at(i)->_DTmin2;
            else
                curStreamTPinch =  _TPinch-allStreams.at(i)->_DTmin2;

            if(!Tk.contains(curStreamTPinch))
            {
                Tk.push_back(curStreamTPinch);
                qSort(Tk.begin(),Tk.end(),METemperature::ThoterThan); //T(0) > T(1) > T(2)...
            }

            // check in each zone, streams holds in at least 3 intervals
            tkConcernedA.clear();
            tkConcernedB.clear();
            //divide zones
            for(int iT=0;iT<tkConcerned.size();iT++)
            {
                if(Tk.at(tkConcerned.at(iT)) > curStreamTPinch) //use corrected T
                {
                    tkConcernedA.push_back(tkConcerned.at(iT));
                }
                else
                {
                    tkConcernedB.push_back(tkConcerned.at(iT));
                }
            }

            // divide in zone A
            if(tkConcernedA.size()==1)
            {
                Tk.insert(tkConcernedA.at(0)+1,TL[0]+(TU[0]-TL[0])*1/3);
                Tk.insert(tkConcernedA.at(0)+1,TL[0]+(TU[0]-TL[0])*2/3);
            }
            if(tkConcernedA.size()==2)
            {
                if((TU[0]-TL[0])>(TU[1]-TL[1]))
                    Tk.insert(tkConcernedA.at(0)+1,TL[0]+(TU[0]-TL[0])*1/2);
                else
                    Tk.insert(tkConcernedA.at(1)+1,TL[1]+(TU[1]-TL[1])*1/2);
            }

            // divide in zone B
            if(tkConcernedB.size()==1)
            {
                Tk.insert(tkConcernedB.at(0)+1,TL[0]+(TU[0]-TL[0])*1/3);
                Tk.insert(tkConcernedB.at(0)+1,TL[0]+(TU[0]-TL[0])*2/3);
            }
            if(tkConcernedB.size()==2)
            {
                if((TU[0]-TL[0])>(TU[1]-TL[1]))
                    Tk.insert(tkConcernedB.at(0)+1,TL[0]+(TU[0]-TL[0])*1/2);
                else
                    Tk.insert(tkConcernedB.at(1)+1,TL[1]+(TU[1]-TL[1])*1/2);
            }
        }
    }


    //sort Tk in descending order !!!
    qSort(Tk.begin(),Tk.end(),METemperature::ThoterThan); //T(0) > T(1) > T(2)...
}


void MilpHEN1::DataToFile(QString dataFilePath,
                          QList<EIStream*> &eiProcessStreams,
                          QList<EIStream*> &eiUtilityStreams,
                          QMultiMap<EIGroupFact*,EIStream*> &factStreamMap, // multimap <unit multiplier, Streams concerned>,
                          QMap<EIGroupFact*,EIGroupFact*> &factsRelation, // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
                          QMap<EIGroupFact*,EIGroup*> &factGroupMap,
                          QList<METemperature> &Tk)
{

    //configuration
    bool allInB = _parameters->value((int)EIHEN1Parameters::ALLOWSEVERAL,QVariant(true)).toBool(); // if true, more than one heat exchanger is permitted between each couple of streams
    bool allInNI = _parameters->value((int)EIHEN1Parameters::ALLOWNI,QVariant(true)).toBool();// if true, non-isothermal connections are allowed for every streams
    bool allSplitsAllowed = _parameters->value((int)EIHEN1Parameters::ALLOWSPLITS,QVariant(true)).toBool(); // if true, splits are allowed for all streams
    int kmax = _parameters->value((int)EIHEN1Parameters::KMAX,QVariant(10)).toInt();

    bool splitTinterval = _parameters->value((int)EIHEN1Parameters::SPLITDT,QVariant(true)).toBool();
    double splitTstep = _parameters->value((int)EIHEN1Parameters::SPLITDTSTEP,QVariant(true)).toDouble();


    // get all streams
    QList<EIStream*> allStreams;
    allStreams.append(eiUtilityStreams);
    allStreams.append(eiProcessStreams);

    // initialize corresponding names (if too long, shortened version will be used)
    QStringList allNames;
    for(int i=0;i<allStreams.size();i++)
        allNames.push_back(allStreams.at(i)->name());
    for(int i=0;i<factGroupMap.values().size();i++)
        allNames.push_back(factGroupMap.values().at(i)->name());
    initCorresp(allNames);


    // be sure each stream at least holds in three intervals
    prepareTk(allStreams,Tk);


    QString zone("z1");
    QString zoneA("za"); //zone above
    QString zoneB("zb"); //zone below


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


    QList<EIStream*> hotStreamsA ;
    QList<EIStream*> hotStreamsB;
    QList<EIStream*> coldStreamsA;
    QList<EIStream*> coldStreamsB;
    QList<EIStream*> huStreamsA;
    QList<EIStream*> huStreamsB;
    QList<EIStream*> cuStreamsA;
    QList<EIStream*> cuStreamsB;

    if(shouldSplitTPinch())
    {
        //use corrected check
        hotStreamsA = EIReader::getStreamsAboveT(_TPinch,hotStreams,true);
        hotStreamsB = EIReader::getStreamsBelowT(_TPinch,hotStreams,true);
        coldStreamsA = EIReader::getStreamsAboveT(_TPinch,coldStreams,true);
        coldStreamsB = EIReader::getStreamsBelowT(_TPinch,coldStreams,true);
        huStreamsA = EIReader::getStreamsAboveT(_TPinch,huStreams,true);
        huStreamsB = EIReader::getStreamsBelowT(_TPinch,huStreams,true);
        cuStreamsA = EIReader::getStreamsAboveT(_TPinch,cuStreams,true);
        cuStreamsB = EIReader::getStreamsBelowT(_TPinch,cuStreams,true);
    }



    //*************
    // param ndt
    //*************
    MilpParam0D paramndt("ndt");
    paramndt.addItem(QString::number(Tk.size()-1));
    dataText.append(paramndt.toString());
    dataText.append("\n\n");

    //*************
    // sets
    //*************
    //Si : hot streams
    MilpSet0D setSi("Si");
    for(int i=0;i<hotStreams.size();i++)
        setSi.addItem(toShortName(hotStreams.at(i)->name()));
    dataText.append(setSi.toString());
    dataText.append("\n");


    //Sj : cold streams
    MilpSet0D setSj("Sj");
    for(int i=0;i<coldStreams.size();i++)
        setSj.addItem(toShortName(coldStreams.at(i)->name()));
    dataText.append(setSj.toString());
    dataText.append("\n");

    //HU : hot utility streams
    MilpSet0D setHU("HU");

    for(int i=0;i<huStreams.size();i++)
        setHU.addItem(toShortName(huStreams.at(i)->name()));
    dataText.append(setHU.toString());
    dataText.append("\n");

    //CU : cold utility streams
    MilpSet0D setCU("CU");

    for(int i=0;i<cuStreams.size();i++)
        setCU.addItem(toShortName(cuStreams.at(i)->name()));
    dataText.append(setCU.toString());
    dataText.append("\n");

    // Sz : zones
    MilpSet0D setSz("Sz");

    if(shouldSplitTPinch())
    {
        setSz.addItem(zoneA);
        setSz.addItem(zoneB);
    }
    else
        setSz.addItem(zone);
    dataText.append(setSz.toString());
    dataText.append("\n");

    // P : connections allowed
    // all combinations + removing forbidden ones
    // get forbidden <HotStream,ColdStream>
    QMultiMap<QString,QString> mapForbidden = _connConstrs->getForbiddenMatchs(_variables);

    MilpSet0D setP("P");
    for(int iH=0;iH<hotStreams.size();iH++)
    {
        for(int iC=0;iC<coldStreams.size();iC++)
        {
            //check if not forbidden
            if(!_connConstrs->isForbiddenMatch(hotStreams.at(iH)->name(),coldStreams.at(iC)->name(),mapForbidden))
                setP.addItem("("+toShortName(hotStreams.at(iH)->name())+","+toShortName(coldStreams.at(iC)->name())+")");
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
            setNIH.addItem(toShortName(hotStreams.at(iH)->name()));
        }
        for(int iC=0;iC<coldStreams.size();iC++)
        {
            setNIC.addItem(toShortName(coldStreams.at(iC)->name()));
        }
    }
    dataText.append(setNIH.toString()+"\n");
    dataText.append(setNIC.toString()+"\n");



    //Hz : hot streams in z
    MilpSet1D setHz("Hz");
    if(shouldSplitTPinch())
    {
        for(int i=0;i<hotStreamsA.size();i++)
            setHz.addItem(zoneA,toShortName(hotStreamsA.at(i)->name()));
        for(int i=0;i<hotStreamsB.size();i++)
            setHz.addItem(zoneB,toShortName(hotStreamsB.at(i)->name()));
    }
    else
    {
        for(int i=0;i<hotStreams.size();i++)
            setHz.addItem(zone,toShortName(hotStreams.at(i)->name()));
    }
    dataText.append(setHz.toString());
    dataText.append("\n");

    //HUz : hot utility streams in z
    MilpSet1D setHUz("HUz");
    if(shouldSplitTPinch())
    {
        for(int i=0;i<huStreamsA.size();i++)
            setHUz.addItem(zoneA,toShortName(huStreamsA.at(i)->name()));
        for(int i=0;i<huStreamsB.size();i++)
            setHUz.addItem(zoneB,toShortName(huStreamsB.at(i)->name()));
    }
    else
    {
        for(int i=0;i<huStreams.size();i++)
        {
            setHUz.addItem(zone,toShortName(huStreams.at(i)->name()));
        }
    }
    dataText.append(setHUz.toString());
    dataText.append("\n");

    //Cz : cold streams in z
    MilpSet1D setCz("Cz");
    if(shouldSplitTPinch())
    {
        for(int i=0;i<coldStreamsA.size();i++)
            setCz.addItem(zoneA,toShortName(coldStreamsA.at(i)->name()));
        for(int i=0;i<coldStreamsB.size();i++)
            setCz.addItem(zoneB,toShortName(coldStreamsB.at(i)->name()));
    }
    else
    {
        for(int i=0;i<coldStreams.size();i++)
            setCz.addItem(zone,toShortName(coldStreams.at(i)->name()));
    }
    dataText.append(setCz.toString());
    dataText.append("\n");

    //CUz : cold utility streams in z
    MilpSet1D setCUz("CUz");
    if(shouldSplitTPinch())
    {
        for(int i=0;i<cuStreamsA.size();i++)
            setCUz.addItem(zoneA,toShortName(cuStreamsA.at(i)->name()));
        for(int i=0;i<cuStreamsB.size();i++)
            setCUz.addItem(zoneB,toShortName(cuStreamsB.at(i)->name()));
    }
    else
    {
        for(int i=0;i<cuStreams.size();i++)
        {
            setCUz.addItem(zone,toShortName(cuStreams.at(i)->name()));
        }
    }
    dataText.append(setCUz.toString());
    dataText.append("\n");


    // SUtGroups
    MilpSet0D setSUtGroups("SUtGroups");
    for(int i=0;i<factGroupMap.values().size();i++)
        setSUtGroups.addItem(toShortName(factGroupMap.values().at(i)->name()));
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
            setSUtStrGroups.addItem("("+toShortName(curGroup->name())+","+toShortName(curStream->name())+") ");
        }
    }
    dataText.append(setSUtStrGroups.toString());
    dataText.append("\n");



    //Mz : list of temperature interval in zone
    MilpSet1D setMz("Mz");
    QStringList allTempIntervals;
    for(int i=0;i<Tk.size()-1;i++)
    {
        allTempIntervals.push_back(QString::number(i+1));
    }

    if(shouldSplitTPinch())
    {
        setMz.addItems(zoneA,allTempIntervals);
        setMz.addItems(zoneB,allTempIntervals);
    }
    else
    {
        setMz.addItems(zone,allTempIntervals);
    }

    dataText.append(setMz.toString());
    dataText.append("\n");


    //Mi0 : set of initial temperature interval of hot streams
    //Mif : set of final...
    //Miz temperature intervals where hot stream i is present
    //Hmz : hot streams of zone z in temperature interval m

    MilpSet1D setMi0("mi0");
    MilpSet1D setMif("mif");
    MilpSet2D setMiz("Miz");
    MilpSet2D setHmz("Hmz");
    int i0;
    int ifinal;
    QList<int> listiTconcerned;

    for(int i=0;i<hotStreams.size();i++)
    {
        // !!! HEN1 does not consider corrected temperatures !!
        // -> use false as argument in getTIntervalsConcerned
        listiTconcerned = EIReader::getTIntervalsConcerned(Tk,hotStreams.at(i),false);
        if(listiTconcerned.size()>0)
        {
            i0 = listiTconcerned.first();
            ifinal = listiTconcerned.last();

            setMi0.addItem(toShortName(hotStreams.at(i)->name()),QString::number(i0+1));
            setMif.addItem(toShortName(hotStreams.at(i)->name()),QString::number(ifinal+1));

            for(int j=0;j<listiTconcerned.size();j++)
            {
                if(shouldSplitTPinch())
                {
                    if(Tk.at(listiTconcerned.at(j))-hotStreams.at(i)->_DTmin2 > _TPinch) //use corrected T
                    {
                        setMiz.addItem(toShortName(hotStreams.at(i)->name()),zoneA,QString::number(listiTconcerned.at(j)+1));
                        setHmz.addItem(QString::number(listiTconcerned.at(j)+1),zoneA,toShortName(hotStreams.at(i)->name()));
                    }
                    else
                    {
                        setMiz.addItem(toShortName(hotStreams.at(i)->name()),zoneB,QString::number(listiTconcerned.at(j)+1));
                        setHmz.addItem(QString::number(listiTconcerned.at(j)+1),zoneB,toShortName(hotStreams.at(i)->name()));
                    }
                }
                else
                {
                    setMiz.addItem(toShortName(hotStreams.at(i)->name()),zone,QString::number(listiTconcerned.at(j)+1));
                    setHmz.addItem(QString::number(listiTconcerned.at(j)+1),zone,toShortName(hotStreams.at(i)->name()));
                }
            }
        }
    }

    dataText.append(setMi0.toString()+"\n");
    dataText.append(setMif.toString()+"\n");
    dataText.append(setMiz.toString()+"\n");
    dataText.append(setHmz.toString()+"\n");

    //Nj0 : set of initial temperature interval of cold streams
    //Njf : set of final...
    //Njz : temperature intervals where cold stream j is present
    //Cnz : cold streams of zone z in temperature interval n
    MilpSet1D setNj0("nj0");
    MilpSet1D setNjf("njf");
    MilpSet2D setNjz("Njz");
    MilpSet2D setCnz("Cnz");
    int j0;
    int jfinal;
    for(int i=0;i<coldStreams.size();i++)
    {
        // !!! HEN1 does not consider corrected temperatures !!
        // -> use false as argument in getTIntervalsConcerned
        listiTconcerned = EIReader::getTIntervalsConcerned(Tk,coldStreams.at(i),false);
        if(listiTconcerned.size()>0)
        {
            j0 = listiTconcerned.first();
            jfinal = listiTconcerned.last();

            setNj0.addItem(toShortName(coldStreams.at(i)->name()),QString::number(j0+1));
            setNjf.addItem(toShortName(coldStreams.at(i)->name()),QString::number(jfinal+1));

            for(int j=0;j<listiTconcerned.size();j++)
            {
                if(shouldSplitTPinch())
                {
                    if(Tk.at(listiTconcerned.at(j))+coldStreams.at(i)->_DTmin2 > _TPinch)
                    {
                        setNjz.addItem(toShortName(coldStreams.at(i)->name()),zoneA,QString::number(listiTconcerned.at(j)+1));
                        setCnz.addItem(QString::number(listiTconcerned.at(j)+1),zoneA,toShortName(coldStreams.at(i)->name()));
                    }
                    else
                    {
                        setNjz.addItem(toShortName(coldStreams.at(i)->name()),zoneB,QString::number(listiTconcerned.at(j)+1));
                        setCnz.addItem(QString::number(listiTconcerned.at(j)+1),zoneB,toShortName(coldStreams.at(i)->name()));
                    }
                }
                else
                {
                    setNjz.addItem(toShortName(coldStreams.at(i)->name()),zone,QString::number(listiTconcerned.at(j)+1));
                    setCnz.addItem(QString::number(listiTconcerned.at(j)+1),zone,toShortName(coldStreams.at(i)->name()));
                }
            }
        }
    }
    dataText.append(setNj0.toString()+"\n");
    dataText.append(setNjf.toString()+"\n");
    dataText.append(setNjz.toString()+"\n");
    dataText.append(setCnz.toString()+"\n");


    // authorized connections within temperature interval
    MilpSet2D setPimH("PimH");
    QStringList tempIntervals;
    QString hotStreamName,coldStreamName;

    for(int iH=0;iH<hotStreams.size();iH++)
    {
        hotStreamName = hotStreams.at(iH)->name();
        tempIntervals = setMiz.items().values(MilpKey2D(toShortName(hotStreamName),zone));
        tempIntervals.append(setMiz.items().values(MilpKey2D(toShortName(hotStreamName),zoneA)));
        tempIntervals.append(setMiz.items().values(MilpKey2D(toShortName(hotStreamName),zoneB)));
        tempIntervals.removeDuplicates();

        for(int iT=0;iT<tempIntervals.size();iT++)
        {
            for(int iC=0;iC<coldStreams.size();iC++)
            {
                coldStreamName = coldStreams.at(iC)->name();
                //check if not forbidden
                if(!_connConstrs->isForbiddenMatch(hotStreams.at(iH)->name(),coldStreams.at(iC)->name(),mapForbidden))
                    setPimH.addItem(toShortName(hotStreamName),tempIntervals.at(iT),toShortName(coldStreamName));
            }
        }
    }
    dataText.append(setPimH.toString()+"\n");

    // authorized connections within temperature interval
    MilpSet2D setPjnC("PjnC");
    for(int iC=0;iC<coldStreams.size();iC++)
    {
        coldStreamName = coldStreams.at(iC)->name();
        tempIntervals = setNjz.items().values(MilpKey2D(toShortName(coldStreamName),zone));
        tempIntervals.append(setNjz.items().values(MilpKey2D(toShortName(coldStreamName),zoneA)));
        tempIntervals.append(setNjz.items().values(MilpKey2D(toShortName(coldStreamName),zoneB)));
        tempIntervals.removeDuplicates();

        for(int iT=0;iT<tempIntervals.size();iT++)
        {
            for(int iH=0;iH<hotStreams.size();iH++)
            {
                hotStreamName = hotStreams.at(iH)->name();
                //check if not forbidden
                if(!_connConstrs->isForbiddenMatch(hotStreams.at(iH)->name(),coldStreams.at(iC)->name(),mapForbidden))
                    setPjnC.addItem(toShortName(coldStreamName),tempIntervals.at(iT),toShortName(hotStreamName));
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


    // he costs
    double fixHEValue = _parameters->value(EIHEN1Parameters::HECOSTA).toDouble();
    double varHEValue = _parameters->value(EIHEN1Parameters::HECOSTB).toDouble();
    MilpParam2D paramCijA("cijA");
    MilpParam2D paramCijF("cijF");
    MilpKey2D curKey2;
    for(int iH=0;iH<hotStreams.size();iH++)
    {
        for(int iC=0;iC<coldStreams.size();iC++)
        {
            curKey2 = MilpKey2D(toShortName(hotStreams.at(iH)->name()),toShortName(coldStreams.at(iC)->name()));
            paramCijA.addItem(curKey2.row(),curKey2.col(),QString::number(fixHEValue));
            paramCijF.addItem(curKey2.row(),curKey2.col(),QString::number(varHEValue));
        }
    }

    dataText.append(paramCijA.toString()+"\n");
    dataText.append(paramCijF.toString()+"\n");


    //Utility groups parameters


    MilpParam1D paramFixUtCost("FixUtCost");
    MilpParam1D paramVarUtCost("VarUtCost");
    MilpParam1D paramUtFactMax("UtFactMax");
    MilpParam1D paramUtFactMin("UtFactMin");

    for(int iFact=0;iFact<factGroupMap.uniqueKeys().size();iFact++)
    {
        curGroupFact = factGroupMap.uniqueKeys().at(iFact);
        curGroup = factGroupMap.value(curGroupFact);

        paramUtFactMax.addItem(toShortName(curGroup->name()),QString::number(curGroupFact->max));
        paramUtFactMin.addItem(toShortName(curGroup->name()),QString::number(curGroupFact->min));
        paramFixUtCost.addItem(toShortName(curGroup->name()),curGroup->getFieldValue(EIGroup::COSTFIX).toString());
        paramVarUtCost.addItem(toShortName(curGroup->name()),curGroup->getFieldValue(EIGroup::COSTMULT).toString());
    }

    dataText.append(paramFixUtCost.toString()+"\n");
    dataText.append(paramVarUtCost.toString()+"\n");
    dataText.append(paramUtFactMax.toString()+"\n");
    dataText.append(paramUtFactMin.toString()+"\n");

    //flowrates
    MilpParam1D paramFi("Fi");
    for(int i=0;i<hotStreams.size();i++)
    {
        curStream = hotStreams.at(i);
        paramFi.addItem(toShortName(curStream->name()),QString::number(curStream->massFlowNum().value(_MassFlowUnit),'g',20));
    }
    dataText.append(paramFi.toString()+"\n");

    MilpParam1D paramFj("Fj");
    for(int i=0;i<coldStreams.size();i++)
    {
        curStream = coldStreams.at(i);
        paramFj.addItem(toShortName(curStream->name()),QString::number(curStream->massFlowNum().value(_MassFlowUnit),'g',20));
    }
    dataText.append(paramFj.toString()+"\n");


    //DHimzH
    //Cpim
    MilpParam3D paramDHimzH("DHimzH");
    MilpParam2D paramCpim("Cpim");
    QMultiMap<MilpKey2D,QString> mizValues = setMiz.values();

    tempIntervals.append(setMz.items().values(zoneA));
    tempIntervals.append(setMz.items().values(zoneB));
    MEQflow dQ;
    double dT;
    double Cp;
    bool CpOk;
    bool inZoneA,inZoneB,inZone;
    for(int iH=0;iH<hotStreams.size();iH++)
    {
        hotStreamName = hotStreams.at(iH)->name();
        Cp = hotStreams.at(iH)->Cp(CpOk).value(_specCpUnit);

        for(int iT=0;iT<allTempIntervals.size();iT++)
        {
            dQ = EIReader::getIntervalQFlow(Tk.at(allTempIntervals.at(iT).toInt()-1),Tk.at(allTempIntervals.at(iT).toInt()),hotStreams.at(iH),false);
            dT = Tk.at(allTempIntervals.at(iT).toInt()-1)-Tk.at(allTempIntervals.at(iT).toInt());

            inZoneA = false;
            inZoneB = false;
            inZone = false;

            if(shouldSplitTPinch())
            {
                inZoneA = mizValues.contains(MilpKey2D(toShortName(hotStreamName),zoneA),allTempIntervals.at(iT));
                inZoneB = mizValues.contains(MilpKey2D(toShortName(hotStreamName),zoneB),allTempIntervals.at(iT));
                if(inZoneA)
                    paramDHimzH.addItem(toShortName(hotStreamName),allTempIntervals.at(iT),zoneA,QString::number(dQ.value(_QFlowUnit),'g',20));

                if(inZoneB)
                    paramDHimzH.addItem(toShortName(hotStreamName),allTempIntervals.at(iT),zoneB,QString::number(dQ.value(_QFlowUnit),'g',20));
            }
            else
            {
                inZone = mizValues.contains(MilpKey2D(toShortName(hotStreamName),zone),allTempIntervals.at(iT));
                if(inZone)
                    paramDHimzH.addItem(toShortName(hotStreamName),allTempIntervals.at(iT),zone,QString::number(dQ.value(_QFlowUnit),'g',20));
            }

            if(inZone || inZoneA || inZoneB)
                paramCpim.addItem(toShortName(hotStreamName),allTempIntervals.at(iT),QString::number(Cp,'g',20));

        }
    }
    dataText.append(paramDHimzH.toString()+"\n");
    dataText.append(paramCpim.toString()+"\n");




    //DHjnzC
    //Cpjn
    MilpParam3D paramDHjnzC("DHjnzC");
    MilpParam2D paramCpjn("Cpjn");
    QMultiMap<MilpKey2D,QString> njzValues = setNjz.values();
    qDebug(setNjz.toString().toLatin1().data());
    tempIntervals.append(setMz.items().values(zoneA));
    tempIntervals.append(setMz.items().values(zoneB));
    for(int iC=0;iC<coldStreams.size();iC++)
    {
        coldStreamName = coldStreams.at(iC)->name();
        Cp = coldStreams.at(iC)->Cp(CpOk).value(_specCpUnit);
        for(int iT=0;iT<allTempIntervals.size();iT++)
        {
            dQ = EIReader::getIntervalQFlow(Tk.at(allTempIntervals.at(iT).toInt()-1),Tk.at(allTempIntervals.at(iT).toInt()),coldStreams.at(iC),false);
            dT = Tk.at(allTempIntervals.at(iT).toInt()-1)-Tk.at(allTempIntervals.at(iT).toInt());

            inZoneA = false;
            inZoneB = false;
            inZone = false;

            if(shouldSplitTPinch())
            {
                inZoneA = njzValues.contains(MilpKey2D(toShortName(coldStreamName),zoneA),allTempIntervals.at(iT));
                inZoneB = njzValues.contains(MilpKey2D(toShortName(coldStreamName),zoneB),allTempIntervals.at(iT));
                if(inZoneA)
                    paramDHjnzC.addItem(toShortName(coldStreamName),allTempIntervals.at(iT),zoneA,QString::number(dQ.value(_QFlowUnit),'g',20));
                if(inZoneB)
                    paramDHjnzC.addItem(toShortName(coldStreamName),allTempIntervals.at(iT),zoneB,QString::number(dQ.value(_QFlowUnit),'g',20));
            }
            else
            {
                inZone = njzValues.contains(MilpKey2D(toShortName(coldStreamName),zone),allTempIntervals.at(iT));
                if(inZone)
                    paramDHjnzC.addItem(toShortName(coldStreamName),allTempIntervals.at(iT),zone,QString::number(dQ.value(_QFlowUnit),'g',20));
            }
            if(inZone || inZoneA || inZoneB)
                paramCpjn.addItem(toShortName(coldStreamName),allTempIntervals.at(iT),QString::number(Cp,'g',20));
        }
    }
    dataText.append(paramDHjnzC.toString()+"\n");
    dataText.append(paramCpjn.toString()+"\n");

    //DTi
    // METemperature dT;
    MilpParam1D paramDTi("DTi");
    for(int iH=0;iH<hotStreams.size();iH++)
    {
        dT = hotStreams.at(iH)->TinNum(false) - hotStreams.at(iH)->ToutNum(false);
        paramDTi.addItem(toShortName(hotStreams.at(iH)->name()),QString::number(dT,'g',20));
    }
    dataText.append(paramDTi.toString()+"\n");

    //DTj
    MilpParam1D paramDTj("DTj");
    for(int iC=0;iC<coldStreams.size();iC++)
    {
        dT = coldStreams.at(iC)->ToutNum(false) - coldStreams.at(iC)->TinNum(false);
        paramDTj.addItem(toShortName(coldStreams.at(iC)->name()),QString::number(dT,'g',20));
    }
    dataText.append(paramDTj.toString()+"\n");

    //TmL TmU
    MilpParam1D paramTmL("TmL");
    MilpParam1D paramTmU("TmU");
    for(int iT=0;iT<Tk.size()-1;iT++)
    {
        paramTmU.addItem(QString::number(iT+1),QString::number(Tk.at(iT).value(_TempUnit),'g',20));
        paramTmL.addItem(QString::number(iT+1),QString::number(Tk.at(iT+1).value(_TempUnit),'g',20));
    }
    dataText.append(paramTmL.toString()+"\n");
    dataText.append(paramTmU.toString()+"\n");

    //DTmnML
    MilpParam2D paramdTlm("DTmnML");
    double dT1;
    double dT2;
    double dTlm;
    for(int iT1=0;iT1<Tk.size()-1;iT1++)
    {

        for(int iT2=0;iT2<iT1;iT2++)
        {
            dTlm=0;
            paramdTlm.addItem(QString::number(iT1+1),QString::number(iT2+1),QString::number(dTlm));
        }

        for(int iT2=iT1;iT2<Tk.size()-1;iT2++)
        {
            dT1 = Tk.at(iT1).value()-Tk.at(iT2).value();
            dT2 = Tk.at(iT1+1).value()-Tk.at(iT2+1).value();
            dTlm = EITools::DTlm(dT1,dT2);
            paramdTlm.addItem(QString::number(iT1+1),QString::number(iT2+1),QString::number(dTlm,'g',20));
        }
    }
    dataText.append(paramdTlm.toString()+"\n");
    dataText +="end;";

    QTextStream ts( &file );
    ts << dataText;
    file.close();
}

bool MilpHEN1::prepareData( QList<METemperature> & Tk)
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


    Tk.clear();
    EITools::getTkQpkQuk(_variables,
                         _eiTree->rootElement(),Tk,
                         eiProcessStreams,Qpk,
                         eiUtilityStreams,Quk,
                         factStreamMap,
                         factsRelation,
                         factGroupMap,
                         false);

    // write data
    QString dataFilePath = _folder.absoluteFilePath(_dataFileName);
    DataToFile(dataFilePath,eiProcessStreams,eiUtilityStreams,
               factStreamMap,factsRelation,factGroupMap,Tk);


    //delete log file
    _folder.remove(_logFileName);

    //delete result file
    _folder.remove(_resFileName);

    //delete sensitivity file
    _folder.remove(_sensFileName);

    //Hook
    // redirect terminal output
    glp_term_hook(hook, NULL);



    int ret;
    _glpProblem = glp_create_prob();
    _glpTran = glp_mpl_alloc_wksp();
    ret = glp_mpl_read_model(_glpTran, _modFilePath.toLatin1().data(), 1);
    if (ret != 0)
    {
        infoSender.send(Info(ListInfo::MILPERRORMODEL));
        return false;
    }
    ret = glp_mpl_read_data(_glpTran, _folder.absoluteFilePath(_dataFileName).toLatin1().data());
    if (ret != 0)
    {
        infoSender.send(Info(ListInfo::MILPERRORDATA));
        return false;
    }
    ret = glp_mpl_generate(_glpTran, NULL);
    if (ret != 0)
    {
        infoSender.send(Info(ListInfo::MILPERRORDATA));
        return false;
    }


    bool ok=true;

    glp_mpl_build_prob(_glpTran, _glpProblem);


    if(ok && solver()==EIHEN1Parameters::CBC)
    {
        // create .mps for Cbc
        QString mpsFilePath = _folder.absoluteFilePath(_mpsFileName);
        ret = glp_write_mps(_glpProblem,GLP_MPS_FILE,NULL,mpsFilePath.toLatin1().data());
        ok = ok && (ret==0);
    }
    return ok;
}

bool MilpHEN1::launchCBC(QString &msg)
{
    _cbcCtrl = new CbcCtrl(_folder,_mpsFileName,_resFileName,_parameters);
    return _cbcCtrl->run(msg);
}

bool MilpHEN1::launchGLPK(QString &msg)
{
    _glpkCtrl = new GlpkCtrl(_glpProblem,_glpTran);
    return _glpkCtrl->run(msg);
}


void MilpHEN1::printGlpkFileResult()
{
    // get results
    int nbRows = glp_get_num_rows(_glpProblem);
    int nbCols = glp_get_num_cols(_glpProblem);

    QString resText;
    QString rowName;
    QString colName;
    double value;
    for(int iRow = 1;iRow<=nbRows; iRow++)
    {
        rowName = QString(glp_get_row_name(_glpProblem, iRow));
        value = glp_mip_row_val(_glpProblem, iRow);
        resText += "Row[" + QString::number(iRow)+"] : ";
        resText += rowName + " : ";
        resText += QString::number(value) +"\n";
    }
    for(int iCol = 1;iCol<=nbCols; iCol++)
    {
        colName = QString(glp_get_col_name(_glpProblem, iCol));
        value = glp_mip_col_val(_glpProblem, iCol);
        resText += "Col[" + QString::number(iCol)+"] : ";
        resText += colName + " : ";
        resText += QString::number(value) +"\n";
    }

    QFile resFile(_folder.absoluteFilePath(_resFileName));
    resFile.open(QIODevice::WriteOnly);
    QTextStream ts( &resFile );
    ts << resText;
    resFile.close();

}

EIHEN1Result* MilpHEN1::getGLPKResult(const QList<METemperature> & Tk)
{
    int ret;
    try
    {
        ret = glp_mpl_postsolve(_glpTran, _glpProblem, GLP_MIP);
    }
    catch(std::exception &e)
    {
        infoSender.send(Info("Error on postsolving model"));
    }

    if (ret != 0)
        fprintf(stderr, "Error on postsolving model\n");

    glp_mpl_free_wksp(_glpTran);
    glp_print_sol(_glpProblem, _folder.absoluteFilePath(_logFileName).toUtf8().data());

    bool printRanges = _parameters->value(EIHEN1Parameters::LOGSENS).toBool();
    if(printRanges)
        glp_print_ranges(_glpProblem,0,0,0,_folder.absoluteFilePath(_sensFileName).toUtf8().data());


    // print resfile
    printGlpkFileResult();


    // resume terminal output
    glp_term_hook(NULL, NULL);
    //result error hook
    glp_error_hook(NULL, NULL);




    EIHEN1Result* result;
    if(!_glpProblem)
    {
        result = new EIHEN1Result(_eiTree->project());
        result->setEITree(*_eiTree);
        result->setSuccess(false);
    }
    else
    {
        result = readGLPKResult(_glpProblem,Tk);
        glp_delete_prob(_glpProblem);
    }


    result->_logFileName = _logFileName;
    result->_resFileName = _resFileName;
    result->_sensFileName = _sensFileName;


    return result;
}



EIHEN1Result* MilpHEN1::getCBCResult(const QList<METemperature> & Tk)
{

    glp_mpl_free_wksp(_glpTran);
    // resume terminal output
    glp_term_hook(NULL, NULL);

    QFile file(_folder.absoluteFilePath(_resFileName));
    bool openOk = file.open(QIODevice::ReadOnly);
    if(!openOk)
        return NULL;

    QTextStream* in = new QTextStream(&file);
    QString txt = in->readAll();

    EIHEN1Result *result=  readCBCResult(txt,Tk);

    if(result)
    {
        result->_logFileName = _logFileName;
        result->_resFileName = _resFileName;
        result->_sensFileName = _sensFileName;
    }

    return result;
}

EIHEN1Result* MilpHEN1::readGLPKResult(glp_prob * glpProblem,const QList<METemperature> & Tk)
{

    // clone eiTree
    EIHEN1Result* result = new EIHEN1Result(_eiTree->project());
    result->setEITree(*_eiTree);

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
    GlpkTools::fill(varUtFact,glpProblem,colNames);
    QMap<QString,double> mapGroupFacMul = varUtFact.values();

    EIGroup* curGroup;
    for(int i=0;i<mapGroupFacMul.keys().size();i++)
    {
        groupName = mapGroupFacMul.keys().at(i);
        curGroup = dynamic_cast<EIGroup*>(result->eiTree()->findItem(groupName));
        if(curGroup)
        {
            value = mapGroupFacMul.value(groupName);
            curGroup->getFact()->value = value;
        }
    }

    // read all UtEnabled
    MilpVariableResult1D varUtEnabled("UtEnabled");
    GlpkTools::fill(varUtEnabled,glpProblem,colNames);
    QMap<QString,double> mapGroupEnabled = varUtEnabled.values();

    for(int i=0;i<mapGroupEnabled.keys().size();i++)
    {
        groupName = mapGroupEnabled.keys().at(i);
        curGroup =dynamic_cast<EIGroup*>(result->eiTree()->findItem(groupName));
        if(curGroup)
        {
            curGroup->setChecked((bool)mapGroupEnabled.value(groupName));
        }
    }

    // read TotalCost
    MilpVariableResult0D varTotalCost("TotalCost");
    GlpkTools::fill(varTotalCost,glpProblem,-1,colNames);
    result->_totalCost = varTotalCost.value();

    // read TotalArea
    MilpVariableResult0D varTotalArea("TotalArea");
    GlpkTools::fill(varTotalArea,glpProblem,-1,colNames);
    result->_totalArea = varTotalArea.value();


    // read HENumber
    MilpVariableResult0D varHENumber("HENumber");
    GlpkTools::fill(varHENumber,glpProblem,-1,colNames);
    result->_HENumber = varHENumber.value();


    result->setEIConns(readGLPKEIConns(glpProblem,colNames,Tk));


    return result;
}

EIHEN1Result* MilpHEN1::readCBCResult(const QString &txt,const QList<METemperature> & Tk)
{

    // clone eiTree
    EIHEN1Result* result = new EIHEN1Result(_eiTree->project());
    result->setEITree(*_eiTree);

    // read if successfull
    bool isOptimal = (txt.indexOf("Optimal")==0);
    result->setSuccess(isOptimal);

    // read all UtFact
    MilpVariableResult1D varUtFact("UtFact");
    CBCTools::fill(varUtFact,txt);

    QMap<QString,double> mapGroupFacMul = varUtFact.values();

    EIGroup* curGroup;
    QString groupName;
    double value;
    for(int i=0;i<mapGroupFacMul.keys().size();i++)
    {
        groupName = mapGroupFacMul.keys().at(i);
        curGroup = dynamic_cast<EIGroup*>(result->eiTree()->findItem(groupName));
        if(curGroup)
        {
            value = mapGroupFacMul.value(groupName);
            curGroup->getFact()->value = value;
        }
    }

    // read all UtEnabled
    MilpVariableResult1D varUtEnabled("UtEnabled");
    CBCTools::fill(varUtEnabled,txt);
    QMap<QString,double> mapGroupEnabled = varUtEnabled.values();

    for(int i=0;i<mapGroupEnabled.keys().size();i++)
    {
        groupName = mapGroupEnabled.keys().at(i);
        curGroup =dynamic_cast<EIGroup*>(result->eiTree()->findItem(groupName));
        if(curGroup)
        {
            curGroup->setChecked((bool)mapGroupEnabled.value(groupName));
        }
    }

    // read TotalCost
    MilpVariableResult0D varTotalCost("TotalCost");
    CBCTools::fill(varTotalCost,-1,txt);
    result->_totalCost = varTotalCost.value();

    // read TotalArea
    MilpVariableResult0D varTotalArea("TotalArea");
    CBCTools::fill(varTotalArea,-1,txt);
    result->_totalArea = varTotalArea.value();

    // read HENumber
    MilpVariableResult0D varHENumber("HENumber");
    CBCTools::fill(varHENumber,-1,txt);
    result->_HENumber = varHENumber.value();

    result->setEIConns(readCBCEIConns(txt,Tk));

    return result;
}

void MilpHEN1::splitTk(QList<METemperature> &Tk, double maxDT)
{
    //sort Tk in descending order
    qSort(Tk.begin(),Tk.end(),METemperature::ThoterThan); //T(0) > T(1) > T(2)...

    int i=0;
    double curDT;
    int nDT;
    double curDT2;

    while(i<Tk.size()-1)
    {
        curDT = std::abs(Tk.at(i)-Tk.at(i+1));
        nDT = ceil(curDT / maxDT);

        if(nDT<=1)
            i++;
        else
        {
            curDT2=curDT/nDT;

            for(int j=0;j<nDT-1;j++)
            {
                Tk.insert(i+1,Tk.at(i)-curDT2);
                i++;
            }
            i++;
        }
    }
}

EIConns* MilpHEN1::readGLPKEIConns(glp_prob * glpProblem,QStringList colNames,const QList<METemperature> & Tk)
{

    // KijmzH
    MilpVariableResult4D varKijmzH("KijmzH");
    GlpkTools::fill(varKijmzH,glpProblem,colNames);

    // KijmzH
    MilpVariableResult4D varKijnzC("KijnzC");
    GlpkTools::fill(varKijnzC,glpProblem,colNames);

    // KeijmzH
    MilpVariableResult4D varKeijmzH("KeijmzH");
    GlpkTools::fill(varKeijmzH,glpProblem,colNames);

    // KeijmzH
    MilpVariableResult4D varKeijnzC("KeijnzC");
    GlpkTools::fill(varKeijnzC,glpProblem,colNames);

    // QijmzH
    MilpVariableResult4D varQijmzH("qijmzH");
    GlpkTools::fill(varQijmzH,glpProblem,colNames);

    // QijmzH
    MilpVariableResult4D varQijnzC("qijnzC");
    GlpkTools::fill(varQijnzC,glpProblem,colNames);

    // massFijmz
    MilpVariableResult4D varMassFijmz("massFijmz");
    GlpkTools::fill(varMassFijmz,glpProblem,colNames);

    // massFijmz
    MilpVariableResult4D varMassFijnz("massFijnz");
    GlpkTools::fill(varMassFijnz,glpProblem,colNames);

    // aijz
    MilpVariableResult3D varAijz("Aijz");
    GlpkTools::fill(varAijz,glpProblem,colNames);

    // aijzk
    MilpVariableResult4D varAijzk("Aijzk");
    GlpkTools::fill(varAijzk,glpProblem,colNames);

    return readEIConns(Tk,varKijmzH,varKijnzC,varKeijmzH,varKeijnzC,varQijmzH,varQijnzC,
                       varMassFijmz,varMassFijnz,varAijz,varAijzk);

}



EIConns* MilpHEN1::readCBCEIConns(const QString &txt,const QList<METemperature> & Tk)
{

    // KijmzH
    MilpVariableResult4D varKijmzH("KijmzH");
    CBCTools::fill(varKijmzH,txt);

    // KijmzH
    MilpVariableResult4D varKijnzC("KijnzC");
    CBCTools::fill(varKijnzC,txt);

    // KeijmzH
    MilpVariableResult4D varKeijmzH("KeijmzH");
    CBCTools::fill(varKeijmzH,txt);

    // KeijmzH
    MilpVariableResult4D varKeijnzC("KeijnzC");
    CBCTools::fill(varKeijnzC,txt);

    // QijmzH
    MilpVariableResult4D varQijmzH("qijmzH");
    CBCTools::fill(varQijmzH,txt);

    // QijnzC
    MilpVariableResult4D varQijnzC("qijnzC");
    CBCTools::fill(varQijnzC,txt);

    // massFijmz
    MilpVariableResult4D varMassFijmz("massFijmz");
    CBCTools::fill(varMassFijmz,txt);

    // massFijmz
    MilpVariableResult4D varMassFijnz("massFijnz");
    CBCTools::fill(varMassFijnz,txt);

    // Aijz
    MilpVariableResult3D varAijz("Aijz");
    CBCTools::fill(varAijz,txt);

    // Aijz
    MilpVariableResult4D varAijzk("Aijzk");
    CBCTools::fill(varAijzk,txt);


    return readEIConns(Tk,varKijmzH,varKijnzC,varKeijmzH,varKeijnzC,varQijmzH,varQijnzC,varMassFijmz,varMassFijnz,varAijz,varAijzk);
}


EIConns* MilpHEN1::readEIConns(const QList<METemperature> & Tk,
                               const MilpVariableResult4D &varKijmzH,
                               const MilpVariableResult4D &varKijnzC,
                               const MilpVariableResult4D &varKeijmzH,
                               const MilpVariableResult4D &varKeijnzC,
                               const MilpVariableResult4D &varQijmzH,
                               const MilpVariableResult4D &varQijnzC,
                               const MilpVariableResult4D &varMassFijmz,
                               const MilpVariableResult4D &varMassFijnz,
                               const MilpVariableResult3D &varAijz,
                               const MilpVariableResult4D &varAijzK
                               )
{

    EIConns* conns = new EIConns();

    // get heat exchanger begin keys
    QList<MilpKey4D> beginHKeys = varKijmzH.values().keys(1);
    QList<MilpKey4D> beginCKeys = varKijnzC.values().keys(1);


    qDebug(varKijnzC.toString().toLatin1().data());

    // get heat exchanger endkeys
    QList<MilpKey4D> endHKeys = varKeijmzH.values().keys(1);
    QList<MilpKey4D> endCKeys = varKeijnzC.values().keys(1);


    MilpKey4D curBHKey,curEHKey,curBCKey,curECKey;
    int iEH,iBC,iEC;
    int kBH,kEH,kBC,kEC;
    bool okBH,okEH,okBC,okEC;
    bool okCp;
    bool found;
    QString strHot,strCold;
    MEQflow qflow,extQFlow;
    MESurface surface;
    METemperature TinA,ToutA,TinB,ToutB;
    MilpKey4D curKey;
    MilpKey3D curKey3;
    EIStream* hotStream;
    EIStream* coldStream;
    MEMassFlow hotMassFlow,coldMassFlow;
    QString zone;
    bool connOk;

    // for each hot beginning
    for(int iBH=0;iBH<beginHKeys.size();iBH++)
    {
        curBHKey=beginHKeys.at(iBH);

        // if qijmzh = 0 at this place, update temperature interval (i3)
        if(varQijmzH.values().value(curBHKey)==0)
            curBHKey = MilpKey4D(curBHKey.i1(),curBHKey.i2(),QString::number(curBHKey.i3().toInt()+1),curBHKey.i4());

        strHot = curBHKey.i1();
        strCold = curBHKey.i2();
        zone = curBHKey.i4();

        hotStream = dynamic_cast<EIStream*>(_eiTree->findItem(strHot));
        coldStream = dynamic_cast<EIStream*>(_eiTree->findItem(strCold));

        if(!hotStream || !coldStream)
        {
            if(!hotStream)
                infoSender.send(Info("Unable to find stream "+strHot,ListInfo::ERROR2));
            if(!coldStream)
                infoSender.send(Info("Unable to find stream "+strCold,ListInfo::ERROR2));
        }
        else
        {
            // find hot end

            iEH=0;
            found=false;
            while((iEH<endHKeys.size())&&(!found))
            {
                if((endHKeys.at(iEH).i1()==strHot)&&(endHKeys.at(iEH).i2()==strCold)&&(endHKeys.at(iEH).i4()==zone))
                    found=true;
                else
                    iEH++;
            }
            if(!found)
                infoSender.debug("Error HEN: could not found HEN hot end (i1="+strHot+",i2="+strCold+")");
            else
            {
                curEHKey = endHKeys.at(iEH);

                // if qijmzh = 0 at this place, update temperature interval (i3)
                if(varQijmzH.values().value(curEHKey)==0)
                    curEHKey = MilpKey4D(curEHKey.i1(),curEHKey.i2(),QString::number(curEHKey.i3().toInt()-1),curEHKey.i4());

                // find cold begining
                found = false;
                iBC=0;

                while((iBC<beginCKeys.size())&&(!found))
                {
                    if((beginCKeys.at(iBC).i1()==strHot)&&(beginCKeys.at(iBC).i2()==strCold)&&(beginCKeys.at(iBC).i4()==zone))
                        found=true;
                    else
                        iBC++;
                }
                if(!found)
                    infoSender.debug("Error HEN: could not found HEN cold begin (i1="+strHot+",i2="+strCold+")");
                else
                {

                    curBCKey = beginCKeys.at(iBC);



                    // if qijnzc = 0 at this place, update temperature interval (i3)
                    if(varQijnzC.values().value(curBCKey)==0)
                        curBCKey = MilpKey4D(curBCKey.i1(),curBCKey.i2(),QString::number(curBCKey.i3().toInt()+1),curBCKey.i4());

                    // find cold end
                    found = false;
                    iEC=0;

                    while((iEC<endCKeys.size())&&(!found))
                    {
                        if((endCKeys.at(iEC).i1()==strHot)&&(endCKeys.at(iEC).i2()==strCold)&&(endCKeys.at(iEC).i4()==zone))
                            found=true;
                        else
                            iEC++;
                    }
                    if(!found)
                        infoSender.debug("Error HEN: could not found HEN cold end (i1="+strHot+",i2="+strCold+")");
                    else
                    {
                        curECKey = endCKeys.at(iEC);

                        // if qijnzc = 0 at this place, update temperature interval (i3)
                        if(varQijnzC.values().value(curECKey)==0)
                            curECKey = MilpKey4D(curECKey.i1(),curECKey.i2(),QString::number(curECKey.i3().toInt()-1),curECKey.i4());

                        //Create connection
                        EIConn *newConn = new EIConn();
                        connOk = true;
                        // still does not support mass fractions -> set to 1

                        kBH = curBHKey.i3().toInt(&okBH);
                        kEH = curEHKey.i3().toInt(&okEH);
                        kBC = curBCKey.i3().toInt(&okBC);
                        kEC = curECKey.i3().toInt(&okEC);

                        Q_ASSERT(okBH && okEH && okBC && okEC);



                        //Hot mass fraction
                        hotMassFlow.setValue(0);
                        for(int k=kBH;k<=kEH;k++)
                        {
                            curKey=MilpKey4D(curBHKey.i1(),curBHKey.i2(),QString::number(k),curBHKey.i4());

                            hotMassFlow.setValue(std::max(hotMassFlow.value(),varMassFijmz.values().value(curKey,-1)),_MassFlowUnit);

                            // check that massFraction is constant in one heat hexchanger
                        }
                        // if only in one temperature interval, we consider no split.
                        if(kBH==kEH)
                            hotMassFlow = hotStream->massFlowNum();

                        //cold mass fraction
                        coldMassFlow.setValue(0);
                        for(int k=kBC;k<=kEC;k++)
                        {
                            curKey=MilpKey4D(curBCKey.i1(),curBCKey.i2(),QString::number(k),curBCKey.i4());
                            coldMassFlow.setValue(std::max(coldMassFlow.value(),varMassFijnz.values().value(curKey,-1)),_MassFlowUnit);

                            // check that massFraction is constant in one heat hexchanger

                        }
                        // if only in one temperature interval, we consider no split.
                        if(kBC==kEC)
                            coldMassFlow=coldStream->massFlowNum();


                        //Tint and Tout

                        //TinHot = TL+q/(m*cp)
                        if(!varQijmzH.values().contains(curBHKey))
                            connOk = false;

                        extQFlow = MEQflow(varQijmzH.values().value(curBHKey,-1),_QFlowUnit);
                        TinA = Tk.at(kBH)+extQFlow.value()/(hotMassFlow.value(_MassFlowUnit)*hotStream->Cp(okCp).value(_specCpUnit));
                        //Q_ASSERT(TinA.value(METemperature::K)>0);
                        //ToutHot = TU-q/(m*cp)
                        if(!varQijmzH.values().contains(curEHKey))
                            connOk = false;

                        extQFlow = MEQflow(varQijmzH.values().value(curEHKey,-1),_QFlowUnit);
                        if(kEH==kBH)
                            ToutA = Tk.at(kEH);
                        else
                            ToutA = Tk.at(kEH-1)-extQFlow.value()/(hotMassFlow.value(_MassFlowUnit)*hotStream->Cp(okCp).value(_specCpUnit));

                        //Q_ASSERT(ToutA.value(METemperature::K)>0);

                        //TinCold = TU - q/(m*cp)
                        //EC corresponds to cold size (inlet)
                        if(!varQijnzC.values().contains(curECKey))
                            connOk = false;

                        extQFlow = MEQflow(varQijnzC.values().value(curECKey,-1),_QFlowUnit);
                        TinB = Tk.at(kEC-1)-extQFlow.value()/(coldMassFlow.value(_MassFlowUnit)*coldStream->Cp(okCp).value(_specCpUnit));
                        //Q_ASSERT(TinB.value(METemperature::K)>0);

                        //ToutCold = TL+q/(m*cp)
                        //EC corresponds to hot size (outlet)
                        if(!varQijnzC.values().contains(curBCKey))
                            connOk = false;

                        extQFlow = MEQflow(varQijnzC.values().value(curBCKey,-1),_QFlowUnit);
                        if(kBC==kEC)
                            ToutB = Tk.at(kBC-1);
                        else
                            ToutB  = Tk.at(kBC)+extQFlow.value()/(coldMassFlow.value(_MassFlowUnit)*coldStream->Cp(okCp).value(_specCpUnit));
                        //Q_ASSERT(ToutB.value(METemperature::K)>0);



                        newConn->setA(hotStream->name(),TinA,ToutA,hotMassFlow);
                        newConn->setB(coldStream->name(),TinB,ToutB,coldMassFlow);

                        //calculate qflow
                        qflow.setValue(0,_QFlowUnit);
                        for(int k=kBH;k<=kEH;k++)
                        {
                            curKey=MilpKey4D(curBHKey.i1(),curBHKey.i2(),QString::number(k),curBHKey.i4());
                            qflow += MEQflow(varQijmzH.values().value(curKey,0),_QFlowUnit);
                        }
                        newConn->setQFlow(qflow);

                        // set surface
                        curKey3 = MilpKey3D(curBHKey.i1(),curBHKey.i2(),curBHKey.i4());
                        surface = MESurface(varAijz.values().value(curKey3,0),_surfaceUnit);
                        newConn->setSurface(surface);

                        if(connOk)
                            conns->addItem(newConn);
                        else
                            delete newConn;
                    }
                }
            }
        }
    }


    return conns;


}


/**
  * Since HEN could take a long time and conver/diverge depending on solver,
  * autoLaunch tries to provide automatic procedure to get result :
  * e.g. by launching HEN with different parameters, stoping if taking too much time...
  * Here procedure aims a short time resolution
  */
EIHEN1Result* MilpHEN1::autoLaunchFast()
{
     // set parameters
    _parameters->setValue(EIHEN1Parameters::SPLITPINCH,false);
    _parameters->setValue(EIHEN1Parameters::ALLOWNI,false);
    _parameters->setValue(EIHEN1Parameters::ALLOWSPLITS,true);
    _parameters->setValue(EIHEN1Parameters::ALLOWSEVERAL,false);
    _parameters->setValue(EIHEN1Parameters::SPLITDT,true);
    _parameters->setValue(EIHEN1Parameters::SPLITDTSTEP,15);
    _parameters->setValue(EIHEN1Parameters::SOLVER,EIHEN1Parameters::GLPK);

    EIHEN1Result* glpkResult =  launchCustom();
    if(!glpkResult)
    {
        infoSender.send(Info("Did not work with GLPK, CBC will now be tried"));
        _parameters->setValue(EIHEN1Parameters::SOLVER,EIHEN1Parameters::CBC);
        _parameters->setValue(EIHEN1Parameters::CBCPREPROCESS,EIHEN1Parameters::OFF);
        _parameters->setValue(EIHEN1Parameters::INTEGERTOLERANCE,10-4);
        _parameters->setValue(EIHEN1Parameters::PRIMALTOLERANCE,10-5);

        EIHEN1Result* cbcResult = launchCustom();
        return cbcResult;

    }

}

/**
  * Since HEN could take a long time and conver/diverge depending on solver,
  * autoLaunch tries to provide automatic procedure to get result :
  * e.g. by launching HEN with different parameters, stoping if taking too much time...
  * Here procedure aims a precise resolution
  */
EIHEN1Result* MilpHEN1::autoLaunchPrecise()
{
    // set parameters
   _parameters->setValue(EIHEN1Parameters::SPLITPINCH,false);
   _parameters->setValue(EIHEN1Parameters::ALLOWNI,false);
   _parameters->setValue(EIHEN1Parameters::ALLOWSPLITS,true);
   _parameters->setValue(EIHEN1Parameters::ALLOWSEVERAL,true);
   _parameters->setValue(EIHEN1Parameters::KMAX,true);
   _parameters->setValue(EIHEN1Parameters::SPLITDT,true);
   _parameters->setValue(EIHEN1Parameters::SPLITDTSTEP,5);
   _parameters->setValue(EIHEN1Parameters::SOLVER,EIHEN1Parameters::GLPK);

   return launchCustom();
}

void MilpHEN1::initCorresp(const QStringList &longNames)
{
    int maxLength = 32;
    if(longNames.isEmpty())
        return;


    QStringList commonSections;
    QStringList curSections;
    bool lengthExceed=false;

    // initialize commonSections
    commonSections = longNames.at(0).split(".");


    // extract commonSections
    for(int i=0;i<longNames.size();i++)
    {
        lengthExceed = lengthExceed || (longNames.at(i).length()>=maxLength);
        curSections = longNames.at(i).split(".");
        for(int j=0;j<commonSections.size();j++)
        {
            if((j>=curSections.size())||(curSections.at(j)!=commonSections.at(j)))
                commonSections.replace(j,QString());
        }
    }

    QString curShortName;
    QString sectionToRemove;
    int iFirstEmpty = commonSections.indexOf("");
    if(iFirstEmpty==-1)
        sectionToRemove=commonSections.join(".");
    else
    {
        sectionToRemove = QStringList(commonSections.mid(0,iFirstEmpty)).join(".");
    }
    if(longNames.size()==1)
    {
        // if only one stream, let it an id !!
        sectionToRemove = sectionToRemove.section(".",0,-2);
    }

    // if length exceeded, remove common sections
    if(lengthExceed)
    {
        for(int i=0;i<longNames.size();i++)
        {
            curShortName = longNames.at(i);
            if(curShortName==sectionToRemove)
                curShortName = sectionToRemove.section(".",-1,-1);
            else
                curShortName = curShortName.remove(sectionToRemove+".");

            _corrNames.insert(curShortName,longNames.at(i));
        }
    }
}

QString MilpHEN1::toShortName(QString longName)
{
    return _corrNames.key(longName,longName);
}

QString MilpHEN1::toLongName(QString shortName)
{
    return _corrNames.value(shortName,shortName);
}
