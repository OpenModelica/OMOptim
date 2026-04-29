/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef PROBLEMINTERFACE_H
#define PROBLEMINTERFACE_H


#include <QtPlugin>
#include <QtCore>
#include <QObject>
#include <QDomElement>
#include <QWidget>


class ProjectBase;
class Problem;
class Result;
class MOParameters;

/**
  * ProblemInterface is used by Project class to manage problems.
  * Every kind of problem (e.g. OneSimulation, Optimization) should be managed by one ProblemInterface.
  * It allows to easily extend OMOptim or other OMOptimBasis based tools with new problems.
  *
  * One ProblemInterface may managed several problems.
  *
  * ProblemInterface offers several functions for each problem :
  * - create a new instance of problem or result
  * - save/load those problem/result
  * - build GUI for problem/result
*/

class ProblemInterface : public QObject
{
    Q_OBJECT

public :

    ProblemInterface();
    virtual ~ProblemInterface(){}

    //******************
    // Create instance
    //******************
    /**
      * Creates a new instance of a problem.
      *
      * @param problemType Type of the problem (QString corresponding to Problem::className())
      * @param modelNames Contains list of model names selected by user. May be empty if no model is required (@sa modelNeeds(QString))
      * @return A pointer to problem created.
      *
      */
    virtual Problem* createNewProblem(ProjectBase*,const QStringList modelNames,QString problemType) = 0;

    //******************
    // Information
    //******************
    /**
      * Returns a list of problem types managed by this interface.
      * Each QString correspond to a Problem::className()
      *
      */
    virtual QStringList problemTypes() = 0;
    virtual QStringList resultTypes() = 0;

    /**
      * Returns the name of the ProblemInterface.
      */
    virtual QString name() = 0;

    //******************
    // GUI
    //******************
    /**
      * Returns a graphical problem tab (cf. QTab class) which contains a set of
      * widget dedicated to problem specification by user.
      */
    virtual QWidget* createProblemTab(Problem *,QWidget* parent) = 0;

    /**
      * Returns a graphical result tab (cf. QTab class) which contains a set of
      * widget dedicated to result display.
      */
    virtual QWidget* createResultTab(Result*,QWidget* parent) = 0;

    //******************
    // Save / Load
    //******************
    /**
      * Save the problem in its saving file, in a xml format.
      * A basic version is implemented, consisting in writing result of Problem::toXmlData function.
      * In most cases, this is suited. Should be extended if one wants to save problem differently.
      */
    virtual bool saveProblem(Problem*);

    /**
      * Save the result in its saving file, in a xml format.
      * A basic version is implemented, consisting in writing result of Result::toXmlData function.
      * In most cases, this is suited. Should be extended if one wants to save result differently.
      */
    virtual bool saveResult(Result*);


    /**
      * Load a problem from its saved file.
      * @param domOMCase XML node containing problem information.
      * @param ProjectBase Pointer to the project that will own this problem.
      * Returns NULL if load failed.
      */
    virtual Problem* loadProblem(QFileInfo loadedFile,const QDomElement & domOMCase,ProjectBase*) = 0;


    /**
      * Load a result from its saved file.
      * @param domOMCase XML node containing result information.
      * @param ProjectBase Pointer to the project that will own this result.
      * Returns NULL if load failed.
      */
    virtual Result* loadResult(QFileInfo loadedFile,const QDomElement & domOMCase,ProjectBase*) = 0;

    //******************
    // Parameters
    //******************
    enum ModelNeeds
    {
        NOMODEL,
        ONEMODEL,
        SEVERALMODELS
    };

    /**
      * For a problemType, returns wether its initialization require one model, several models, or no one.
      * If one or several are required, createNewProblem function will bring them through modelNames parameter.
      * GUI automatically asks the user for models if needed.
      * @sa createNewProblem()
      */
    virtual ModelNeeds modelNeeds(QString problemType) = 0;


    virtual void initParameters();
    virtual MOParameters* parameters();

protected :
    MOParameters *_parameters; /// Parameters of the interface. Automatically added in Settings window.
 };


#ifdef ISAPLUGIN
    Q_DECLARE_INTERFACE(ProblemInterface,"com.OMOptim.ProblemInterface/0.1")
#endif





#endif // PROBLEMINTERFACE_H
