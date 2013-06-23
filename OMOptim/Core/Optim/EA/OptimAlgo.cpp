// $Id$
/**
 * This file is part of MinEIT Mines Energy Integration Tool.
 *
 * Developed by ARMINES - Center for Energy and Processes
 * Paris, France
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

     @file OptimAlgo.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#include "OptimAlgo.h"
#include "MOParameter.h"
#include "OptimAlgosList.h"
#include "Problem.h"

OptimAlgo::OptimAlgo(void)
{
    _problem = NULL;
    _project = NULL;
    _parameters = new MOParameters();
}

OptimAlgo::~OptimAlgo(void)
{
    delete _parameters;
}

OptimAlgo::OptimAlgo(const OptimAlgo & algo)
{
    _project = algo._project;
    _problem = algo._problem;
    _parameters = new MOParameters(*algo._parameters);
}

void OptimAlgo::setProblem(Problem* problem)
{
    _problem = problem;
    //setParent(problem);
}

QDomElement OptimAlgo::toXmlData(QDomDocument &doc)
{
    QDomElement cOptimAlgo = doc.createElement(className());
    QDomElement cParams;

    cOptimAlgo.setAttribute("name",this->name());
    cParams = this->parameters()->toXmlData(doc,"Parameters");

    cOptimAlgo.appendChild(cParams);
    return cOptimAlgo;
}

OptimAlgos::OptimAlgos(Project* project,Problem* problem)
    :QObject(problem)
{

    _project = project;
    _problem = problem;


    for(int i=0;i<OptimAlgosList::nbAlgos;i++)
    {
        OptimAlgo* newAlgo;
        newAlgo = OptimAlgosList::getNewAlgo(project,problem,(OptimAlgosList::Type)i);
        this->insertAlgo(newAlgo->name(),newAlgo);
    }

    if(keys().size()>0)
        setCurrentAlgo(keys().at(0));
}

OptimAlgos::OptimAlgos(Project* project,Problem* problem,const QDomElement & cAlgos)
 :QObject(problem)
{
    _project = project;
    _problem = problem;

    for(int i=0;i<OptimAlgosList::nbAlgos;i++)
    {
        OptimAlgo* newAlgo;
        newAlgo = OptimAlgosList::getNewAlgo(project,problem,(OptimAlgosList::Type)i);
        this->insertAlgo(newAlgo->name(),newAlgo);
    }


    if(!cAlgos.isNull() && cAlgos.tagName()==className())
    {
        // Controler type
        this->setCurrentAlgo(cAlgos.attribute("curAlgo",""));

        // Update controler parameters
        QDomNodeList domAlgosList = cAlgos.elementsByTagName(OptimAlgo::className());

        QDomElement cParams;
        QDomElement cAlgo;
        OptimAlgo* curAlgo;
        QString curAlgoName;
        for(int iC=0;iC<domAlgosList.size();iC++)
        {
            cAlgo = domAlgosList.at(iC).toElement();
            curAlgoName = cAlgo.attribute("name","");
            curAlgo = this->value(curAlgoName);

            if(curAlgo)
            {
                cParams = cAlgo.firstChildElement("Parameters");
                if(!cParams.isNull())
                {
                    curAlgo->parameters()->update(cParams);
                }
            }
        }
    }
}

OptimAlgos::~OptimAlgos()
{
    for(int i=0;i<values().size();i++)
    {
        delete values().at(i);
    }
}

OptimAlgos* OptimAlgos::clone()
{
    OptimAlgos* cloned = new OptimAlgos(_project,_problem);
    cloned->setParent(NULL);

    // first remove automatically created
    for(int i=0;i<cloned->values().size();i++)
    {
        delete cloned->values().at(i);
    }
    cloned->clear();

    // then add cloned
    for(int i=0;i<keys().size();i++)
    {
        cloned->insertAlgo(this->keys().at(i),this->value(keys().at(i))->clone());
    }

    cloned->_curAlgoName = _curAlgoName;

    return cloned;
}

void OptimAlgos::setProblem(Problem * problem)
{
    _problem = problem;
    for(int i=0;i<values().size();i++)
    {
        values().at(i)->setProblem(problem);
    }
//    setParent(problem);
}



QDomElement OptimAlgos::toXmlData(QDomDocument & doc)
{
    QDomElement cOptimAlgos = doc.createElement(className());
    cOptimAlgos.setAttribute("curAlgo",currentAlgoName());

    QDomElement ccurAlgo;
    OptimAlgo* curAlgo;

    for(int iAlgo=0;iAlgo<this->values().size();iAlgo++)
    {
        curAlgo = this->values().at(iAlgo);
        ccurAlgo = curAlgo->toXmlData(doc);
        cOptimAlgos.appendChild(ccurAlgo);
    }

    return cOptimAlgos;
}

OptimAlgo* OptimAlgos::currentAlgo() const
{
    return this->value(_curAlgoName,NULL);
}


QString OptimAlgos::currentAlgoName() const
{
    return currentAlgo()->name();
}


bool OptimAlgos::setCurrentAlgo(QString curAlgoName)
{
    if(!keys().contains(curAlgoName))
        return false;

    if(curAlgoName != _curAlgoName)
    {
        _curAlgoName = curAlgoName;
        emit modified();
        emit algoChanged(_curAlgoName);
    }
    return true;
}

void OptimAlgos::setFromOtherAlgos(const OptimAlgos &newAlgos)
{
    // clear content
    for(int i=0;i<values().size();i++)
    {
        delete values().at(i);
    }
    this->clear();

    // add clones
    QString curAlgoName;
    OptimAlgo* newAlgo;
    for(int i=0;i<newAlgos.keys().size();i++)
    {
        curAlgoName = newAlgos.keys().at(i);
        newAlgo = newAlgos.value(curAlgoName)->clone();
        this->insert(curAlgoName,newAlgo);
    }
    this->setCurrentAlgo(newAlgos._curAlgoName);
}

QStringList OptimAlgos::getNames() const
{
    QStringList result;
    // clear content
    for(int i=0;i<values().size();i++)
    {
        result.push_back(values().at(i)->name());
    }

    return result;
}

void OptimAlgos::insertAlgo(QString name,OptimAlgo* algo)
{
    this->insert(name,algo);
    connect(algo,SIGNAL(modified()),this,SIGNAL(modified()));
}

