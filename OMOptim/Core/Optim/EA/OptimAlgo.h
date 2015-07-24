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

        @file OptimAlgo.h
  @brief Comments for file documentation.
  @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
  Company : CEP - ARMINES (France)
  http://www-cep.ensmp.fr/english/
  @version

  */
#if !defined(_OPTIMALGO_H)
#define _OPTIMALGO_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#include <QMap>
#endif

#include "OptimAlgosList.h"

class Problem;
class Result;
class Project;
class MOParameters;

class OptimAlgo : public QObject
{
    Q_OBJECT

public:
    OptimAlgo(void);
    virtual ~OptimAlgo(void);
    OptimAlgo(const OptimAlgo &);
    static QString className(){return "OptimAlgo";}

    virtual OptimAlgo* clone() const = 0;
    virtual Result* launch(QString tempDir) = 0;

    virtual QString name() =0;
    virtual void setProblem(Problem* problem);
    virtual QDomElement toXmlData(QDomDocument & doc);

    MOParameters* parameters(){return _parameters;}

    /**
          * \brief Determines if algorithm accepts or not several objectives.
          * Determines if algorithm accepts or not several objectives. Pure virtual function.
          */
    virtual bool acceptMultiObjectives() = 0;

    /**
    * Set parameters that should be defined
    */
    virtual void setDefaultParameters() = 0;
    virtual void quickEnd(){}
    virtual void stop(){}

signals:
    void modified();

protected :
    Project* _project;
    Problem* _problem;
    MOParameters *_parameters;
};


class OptimAlgos :public QObject, public QMap<QString,OptimAlgo*>
{
    Q_OBJECT

public :
    OptimAlgos(Project* project,Problem* problem);
    OptimAlgos(Project* project,Problem* problem,const QDomElement &);
    virtual ~OptimAlgos();

    static QString className(){return "OptimAlgos";}

    OptimAlgos* clone();
    void setProblem(Problem *);
    QDomElement toXmlData(QDomDocument & doc);


    OptimAlgo* currentAlgo() const;
    QString currentAlgoName() const;
    bool setCurrentAlgo(QString curAlgo);
    void setFromOtherAlgos(const OptimAlgos &);

    QStringList getNames() const;

signals :
    void modified();
    void algoChanged(QString);


private :
    Project *_project;
    Problem* _problem;
    OptimAlgo* _optimAlgo;
    QString _curAlgoName;
    void insertAlgo(QString name,OptimAlgo*);

};

#endif
