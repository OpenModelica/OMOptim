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

  @file CCTools.cpp
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#include "CCTools.h"
#include <QtCore/QtAlgorithms>
#include <cmath>
#include "LowTools.h"
#include "MOCCCurve.h"
#include "assert.h"

using namespace EI;

CCTools::CCTools(void)
{
}

CCTools::~CCTools(void)
{
}

void CCTools::buildCCfromStreams(const QList<METemperature> & Tk,
                                 const QList<QList<MEQflow> > & Qik,
                                 MOCCCurve *hotCurve,
                                 MOCCCurve *coldCurve)
{

    if(Tk.size()==0)
    {
        hotCurve->clear();
        coldCurve->clear();
        return;
    }


    QList<METemperature> Tall;
    QList<MEQflow> HCold,HHot;
    MEQflow curQik;

    HCold.push_back(MEQflow(0,0));
    HHot.push_back(MEQflow(0,0));

    MEQflow curHCold, curHHot;

    curHHot=MEQflow(0,0);
    curHCold=MEQflow(0,0);

    for(int iK=0;iK<Tk.size()-1;iK++)
    {
        Tall.push_back(Tk.at(iK));

        for(int iS=0;iS<Qik.size();iS++)
        {
            curQik = Qik.at(iS).at(iK);

            if(curQik>MEQflow(0,0))
                curHCold += curQik;
            else
                curHHot -= curQik;
        }
        HHot.push_back(curHHot);
        HCold.push_back(curHCold);
    }
    Tall.push_back(Tk.at(Tk.size()-1));

    // looking for Tpinch
    METemperature TPinch;
    MEQflow dHPinch;
    getPinch(Tall,HCold,HHot,TPinch,dHPinch);


    // offsetting Cold curve (in order to make curves cross at pinch)
    for(int iT=0;iT<Tall.size();iT++)
    {
        HCold[iT] = HCold[iT]-dHPinch;
    }

    // set results
    hotCurve->setData(HHot,Tall);
    coldCurve->setData(HCold,Tall);

    hotCurve->setType(MOCCCurve::CCHOT);
    coldCurve->setType(MOCCCurve::CCCOLD);
}

void CCTools::buildGCCfromStreams(const QList<METemperature> & Tk,
                                  const QList<QList<MEQflow> > & Qik,
                                  MOCCCurve *gccCurve)
{

    MOCCCurve *hotCurve = new MOCCCurve(MOCCCurve::CCHOT);
    MOCCCurve *coldCurve = new MOCCCurve(MOCCCurve::CCCOLD);
    buildCCfromStreams(Tk,Qik,hotCurve,coldCurve);


    QList<MEQflow> dH;
    QList<METemperature> Tall;

    for(int i=0;i<Tk.size();i++)
    {
        Tall.push_back(Tk.at(i));
        dH.push_back(coldCurve->XData().at(i)-hotCurve->XData().at(i));
    }

    gccCurve->setData(dH,Tall);
    gccCurve->setType(MOCCCurve::GCC);
}

void CCTools::buildICCfromStreams(const EITree &eiTree,
                               MOCCCurve *iccProcess,
                               MOCCCurve *iccUtilities,
                                  bool includeUtilities)
{

    // get information
    QList<EIStream*> eiProcessStreams;
    QList<QList<MEQflow> > Qpk; //.at(iStream).at(iDTk)
    QList<EIStream*> eiUtilityStreams;
    QList<QList<MEQflow> > Quk;//.at(iStream).at(iDTk)
    QMultiMap<EIGroupFact*,EIStream*> factStreamMap; // multimap <unit multiplier, Streams concerned>,
    QMap<EIGroupFact*,EIGroupFact*> factsRelation; // map<child unit multiplier, parent unit multiplier> for constraint (e.g. fchild <= fparent * fchildmax)
    QMap<EIGroupFact*,EIGroup*> factGroupMap;
    QList<METemperature> Tk;

    EITools::getTkQpkQuk(NULL,
                         eiTree.rootElement(),Tk,
                         eiProcessStreams,Qpk,
                         eiUtilityStreams,Quk,
                         factStreamMap,
                         factsRelation,
                         factGroupMap,
                         true);


    // build gcc with process
    CCTools::buildGCCfromStreams(Tk,Qpk,iccProcess);

    // build gcc with utilities
    if(!includeUtilities)
    {
        iccUtilities = new MOCCCurve(MOCCCurve::ICCUTILITIES);
    }
    else
    {
        // first create normal one
        QList<QList<MEQflow> > QukFact ; // Quk * multiplication factor obtained from Target
        QList<MEQflow> curQList;
        EIGroupFact* curFact;
        double mult;
        for(int iS=0;iS<eiUtilityStreams.size();iS++)
        {
            curFact = factStreamMap.key(eiUtilityStreams.at(iS),NULL);
            if(curFact)
                mult = curFact->value;
            else
                mult = 0;

            curQList = Quk.at(iS);
            for(int j=0;j<curQList.size();j++)
            {
                curQList.replace(j,curQList.at(j)*mult);
            }
            QukFact.push_back(curQList);

        }

        CCTools::buildGCCfromStreams(Tk,QukFact,iccUtilities);
        // mirror effect now
        QList<MEQflow>  revXData = iccUtilities->XData();
        for(int i=0;i<revXData.size();i++)
        {
            revXData.replace(i,MEQflow(0,0)-revXData.at(i));
        }

        // horizontal translate
        METemperature TPinch;
        MEQflow dHPinch;
        MEQflow translateH;
        getPinch(Tk,iccProcess->XData(),TPinch,dHPinch);
        int iTPinch = Tk.indexOf(TPinch);
        if(iTPinch>-1)
        {
            translateH = MEQflow(0,0) - revXData.at(iTPinch);
        }
        for(int i=0;i<revXData.size();i++)
        {
            revXData.replace(i,revXData.at(i)+translateH);
        }
        iccUtilities->setData(revXData,iccUtilities->YData());
    }


    // set types
    iccProcess->setType(MOCCCurve::ICCPROCESS);
    iccUtilities->setType(MOCCCurve::ICCUTILITIES);
}


void CCTools::getPinch(const QList<METemperature>  &Tall,const QList<MEQflow>  &HCold,const QList<MEQflow>  &HHot, METemperature & TPinch, MEQflow & dHPinch)
{

    QList<MEQflow> HColdAll, HHotAll;

    if(Tall.size()==0)
    {
        TPinch = METemperature();
        dHPinch = MEQflow();
        return;
    }

    // Getting pinch point
    if(Tall.size()>1)
    {
        TPinch = Tall[0];
        dHPinch = HCold[0] - HHot[0];
        for(int iT=1;iT<Tall.size();iT++)
        {
            if(/*!LowTools::isNan(HCold[iT]) && !LowTools::isNan(HHot[iT]) &&*/ ((HCold[iT] - HHot[iT]) < dHPinch))
            {
                dHPinch = HCold[iT] - HHot[iT];
                TPinch = Tall[iT];
            }
        }
    }
    else
        TPinch = METemperature();
}

/**
  * Finding pinch coordinates from dH = hCold - hHot
  */
void CCTools::getPinch(const QList<METemperature>  &Tall,const QList<MEQflow>  &dH, METemperature & TPinch, MEQflow & dHPinch)
{


    if(Tall.size()==0)
    {
        TPinch = METemperature();
        dHPinch = MEQflow();
        return;
    }

    // Getting pinch point
    if(Tall.size()>1)
    {
        TPinch = Tall[0];
        dHPinch = dH.at(0);
        for(int iT=1;iT<Tall.size();iT++)
        {
            if(/*!LowTools::isNan(HCold[iT]) && !LowTools::isNan(HHot[iT]) &&*/ (dH.at(iT) < dHPinch))
            {
                dHPinch = dH.at(iT);
                TPinch = Tall[iT];
            }
        }
    }
    else
        TPinch = METemperature();
}

void CCTools::getValues(const QList<METemperature> & Tk,
                        const QList<QList<MEQflow> > & Qik,
                        METemperature &TPinch, MEQflow &MER, MEQflow &MERCold)
{

    MOCCCurve *hotCurve = new MOCCCurve(MOCCCurve::CCHOT);
    MOCCCurve *coldCurve = new MOCCCurve(MOCCCurve::CCCOLD);

    buildCCfromStreams(Tk,Qik,hotCurve,coldCurve);

    MEQflow dHPinch;

    QList<MEQflow> HHot = hotCurve->XData();
    QList<MEQflow> HCold = coldCurve->XData();
    QList<METemperature> Tall = hotCurve->YData();
    QList<METemperature> Tall2 = coldCurve->YData();
    assert(Tall==Tall2);

    getPinch(Tall,HCold,HHot,TPinch,dHPinch);


    MER.setValue(0);
    MERCold.setValue(0);

    if((HHot.size()==0)&&(HCold.size()>0))
    {
        MER = HCold.at(HCold.size()-1)-HCold.at(0);
        MERCold = MEQflow(0,0);
        return;
    }

    if((HHot.size()>0)&&(HCold.size()==0))
    {
        MER.setValue(0);
        MERCold = HHot.at(HHot.size()-1)-HHot.at(0);
        return;
    }

    if((HHot.size()>0)&&(HCold.size()>0))
    {
        MERCold = HCold.at(0)-HHot.at(0);
        MER = HCold.at(HCold.size()-1)-HHot.at(HHot.size()-1);
        return;
    }


    return;
}

void CCTools::projectHonT(QList<METemperature> &newT,QList<MEQflow> &newH,const QList<METemperature> &oldT,const QList<MEQflow> &oldH)
{
    int j;
    bool found;

    newH.clear();
    double up,down;

    for(int i=0;i<newT.size();i++)
    {

        // getting newH value
        if(oldT.size()==0)
            newH.push_back(MEQflow(0,0));
        else
        {
            if(newT[i]<oldT[0])
                newH.push_back(oldH[0]);
            else
            {
                if(newT[i]>=oldT[oldT.size()-1])
                    newH.push_back(oldH[oldH.size()-1]);
                else
                {
                    j=0;
                    found = false;
                    while((j<oldT.size()-1) && (!found))
                    {
                        if(newT[i]>=oldT[j] && newT[i]<oldT[j+1])
                        {
                            found = true;
                            up = newT[i].value(METemperature::K)-oldT[j].value(METemperature::K);
                            down = oldT[j+1].value(METemperature::K)-oldT[j].value(METemperature::K);
                            newH.push_back(oldH[j] + (oldH[j+1]-oldH[j])*(up/down));
                        }

                        j++;
                    }
                }
            }
        }
    }


}

double CCTools::getNumericalFieldValue(EIStream* stream,int iField,MOOptVector* variables,bool &ok)
{

    QString variableName = stream->getFieldValue(iField).toString();

    int iVar = variables->findItem(variableName);

    if(iVar==-1)
    {
        ok = false;
        return -1;
    }
    else
    {
        ok = true;
        return variables->at(iVar)->finalValue(variables->curScan(),variables->curPoint());
    }
}
