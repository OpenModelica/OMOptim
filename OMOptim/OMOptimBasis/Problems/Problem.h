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

    @file Problem.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
        @version

  */
#if !defined(_PROBLEM_H)
#define _PROBLEM_H

#include <QtCore/QObject>
#include <QtCore/QTime>
#include <QtCore/QFile>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>


#include "OMCase.h"




class Result;
class MOParameters;
class ProjectBase;
class ProblemConfig;



/**
* \brief Problem is an abstract class to be inherited by problem you want to solve.
*/
class Problem: public OMCase
{
    Q_OBJECT

protected :

    // General information
    MOParameters* _parameters;


public:

    // CTOR
    Problem(ProjectBase*);
    Problem(QDomElement domProblem,ProjectBase* project,bool &ok);
    Problem(const Problem & s);
    virtual ~Problem(void);

    virtual Problem* clone() const =0;


    // MO Item overwriting
    virtual QString getFieldName(int iField,int iRole);
    virtual unsigned getNbFields();
    virtual QString getClassName(){return "Problem";}


    // Stop functions
    virtual bool canBeStoped(){return false;}/// does the problem resolution can be stoped at any time
    virtual bool hasQuickEndOption(){return false;}/// does the problem resolution can be shortened at any time

    virtual void stop(){};

    // Infos
    static QString infos();

    // Execution
    /**
        * Launch problem resolution.
    * This function should be reimplemented for each kind of problem.
    */
    virtual Result* launch(ProblemConfig config) = 0;

    /**
        * Precomputation function : should be launched at beginning
        * of launch function
        */
    //virtual void precompute();
    /**
        * Check if problem parameters, and inputs are correct.
        * For example, check if min<max.
        * pure virtual functions : all classes inheriting Problem should have a check function.
        * @param error : contains list of errors detected.
        */
    virtual bool checkBeforeComp(QString & error) = 0;

    // Managment functions
    /**
        * Save problem information in XML form.
    */
    virtual QDomElement toXmlData(QDomDocument & doc) = 0;
    /**
        * Store problem files in destFolder. Is called when a problem resolution is finished.
        * @param destFolder destination folder path.
        * @param tempDir dir from where problem files are copied
        */
    virtual void store(QString destFolder, QString tempDir);
    void setDefaultSaveFileName();

    // Get functions
    MOParameters* parameters(){return _parameters;}


signals:
    void newProgress(float);
    void newProgress(float,int,int);

};







#endif
