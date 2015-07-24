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

    @file OptObjective.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#ifndef OPTOBJECTIVE_H
#define OPTOBJECTIVE_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtCore/QObject>
#endif

#include "Variable.h"


class OptObjective : public Variable
{
public:
    enum Field
    {
        NAME,
        MODEL,
        DESCRIPTION,
        DIRECTION,
        MIN,
        MAX,
        // Scan
        SAMPLINGFUNCTION
    };

    enum Direction
    {
        MAXIMIZE=0,
        MINIMIZE=1
    };

    enum ScanFunction
    {
        NONE,
        AVERAGE,
        SUM,
        DEVIATION,
        MINIMUM,
        MAXIMUM
    };

    OptObjective();
    OptObjective(const Variable &,Direction = MINIMIZE);
    OptObjective(const OptObjective &);
    OptObjective(QDomElement &);
    ~OptObjective(void);
    virtual QString getClassName() const {return "OptObjective";};


    OptObjective* clone() const;

    void initOptExtremum();

    void setDirection(Direction);
    void setScanFunction(ScanFunction);
    void setDescription(QString);

    Direction direction();
    ScanFunction scanFunction();

    bool check(QString &error);
    bool isMinimized();
    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole) const {return OptObjective::sFieldName(i,role);};

    virtual QVariant getFieldValue(int, int role = Qt::UserRole) const;
    virtual bool setFieldValue(int ifield, QVariant value_);

    static const int nbFields = 7;
    virtual unsigned getNbFields( ) const {return nbFields;};


    double min(){return _min;};
    double max(){return _max;};

protected:
    ScanFunction _scanFunction;
    Direction _direction;
    double _min;
    double _max;



};



class OptObjectiveResult : public OptObjective
{
public:
    OptObjectiveResult();
    OptObjectiveResult(const OptObjective &);
    OptObjectiveResult(QDomElement &);
    virtual QString getClassName() const {return "OptObjectiveResult";};


    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole) const {return OptObjectiveResult::sFieldName(i,role);};

    QVariant getFieldValue(int, int role = Qt::UserRole) const;
    bool setFieldValue(int ifield, QVariant value_);




public:
    //Added functions
    std::vector<double> finalValues() const;
    bool isComputedPoint(int i) const;
    int nbPoints() const;
    double finalValue(int) const;

    void setFinalValues(const std::vector<double> &);
    void setFinalValueAtPoint(double,int);
  //  void appendFinalValue(double);

    void clearFinalValues();
    OptObjectiveResult* clone() const;

    OptObjective* equivOptObjective();

private :
    std::vector<double> _finalValues;
    std::vector<bool> _computedPoints;

};

#endif
