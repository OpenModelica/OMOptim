// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linkpings universitet, Department of Computer and Information Science,
 * SE-58183 Linkping, Sweden.
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

    @file Variable.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_VARIABLE_H)
#define _VARIABLE_H

#include <vector>
#include <QtCore/QObject>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtGui/QSortFilterProxyModel>
#include <limits>
#include <QtXml/QDomDocument>

#include "MOItem.h"
#include "LowTools.h"
#include "VariableType.h"



class Variable : public MOItem
{
public :
    /*enum Type
    {
        SPECIAL_CASE = -2,
        FIXED = -1,
        FREE = 0,
        DESIRED = 1
    };*/

    enum Field
    {
        //Modelica fields
        NAME,
        MODEL,
        VALUE,
        DESCRIPTION,
        /*MIN,
        MAX,
        TYPE,
        CATEGORY,*/
        DATATYPE
    };

    /*enum Category
    {
        PARAMETER = 1,
        STATE = 2,
        STATE_DERIVATIVE = 3,
        OUTPUT = 4,
        INPUT = 5,
        AUXILIARY_VARIABLE = 6
    };*/



    enum MOVarType
    {
        NORMAL ,
        OPTIMIZED
    };

    enum NameFormat
    {
        SHORT,
        FULL
    };

public:
    Variable(void);
    Variable(QString name);
    Variable(const Variable &);
    Variable(QDomElement &);
    virtual ~Variable(void);

    virtual Variable* clone() const;

    virtual QString getClassName(){return "Variable";};

public :

    void setModel(QString);
    QString model() const;


    //Overwrited functions
    virtual QVariant getFieldValue(int, int role = Qt::UserRole) const;
    virtual bool setFieldValue(int ifield, QVariant value_);
    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole){return Variable::sFieldName(i,role);};

    static const int nbFields = 5;
    virtual unsigned getNbFields(){return nbFields;};

    //Specific functions
    void setDescription(QString);
    //void setType(int);
    void setValue(double);
    double value() const;
    //void setMin(double);
    //void setMax(double);
    //void setCategory(int);
    void setDataType(VariableType);
    QString description();
    virtual QString name(Variable::NameFormat = Variable::FULL) const;
    virtual QString getStrToolTip();



protected :
    // added fields
    QString _description;
    QString _model;

    //int type;
    //int category;
    VariableType _dataType;
    double _value;
    //double min;
    //double max;
};




class VariableResult : public Variable
{

    Q_OBJECT

public:
    VariableResult(void);
    VariableResult(const VariableResult& );
    VariableResult(const Variable &);
    VariableResult(QDomElement &);


    ~VariableResult(void);

    VariableResult* clone() const;
    virtual QString getClassName(){return "VariableResult";};

    static const int nbFields = 5;

private:
    std::vector<std::vector<double> > _finalValues;
    std::vector<std::vector<bool> > _computedPoints;

public :
    //Overwrited functions
    //VariableResult* clone() const;


    //Added functions
    std::vector<double> finalValues(int iScan) const;
    bool isComputedPoint(int iScan,int iPoint) const;
    int nbPoints() const;
    int nbScans() const;
    double finalValue(int iScan, int iPoint) const;
    std::vector<double> finalValuesAtPoint(int iPoint);
    std::vector<double> finalValuesAtScan(int iScan);
    void setFinalValuesAtScan(int iScan, const std::vector<double> &);
    void setFinalValuesAtPoint(int iPoint,const std::vector<double> &);
    void setFinalValueAtPoint(int iPoint,double );
    void setFinalValue(int iScan, int iPoint,double,bool computed = true);
//    void appendFinalValue(double, int iScan);
//    void appendScanValues(std::vector<double> _values,std::vector<bool> _computedPoints);
//    void appendScanValue(double _value,bool _computedPoint);
    void clearFinalValues();
    void clearFinalValuesAtIpoint(int iPoint);

    virtual QDomElement toXmlData(QDomDocument & doc);

};

class OptVariable : public Variable
{
public :

    enum Field
    {
        NAME,
        MODEL,
        VALUE,
        DESCRIPTION,
        /*MIN,
        MAX,
        TYPE,
        CATEGORY,*/
        DATATYPE,
        OPTMIN,
        OPTMAX
    };

public:
    OptVariable(void);
    OptVariable(const Variable &);
    ~OptVariable(void);
    OptVariable(const OptVariable &);
    OptVariable(QDomElement &);
    OptVariable* clone() const;
    virtual QString getClassName(){return "OptVariable";};


public :
    //Overwrited functions
    virtual bool check(QString &error);
    virtual QVariant getFieldValue(int, int role = Qt::UserRole) const;
    virtual bool setFieldValue(int ifield, QVariant value_);
    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole){return OptVariable::sFieldName(i,role);};
    static const int nbFields = 7;
    virtual unsigned getNbFields(){return nbFields;};

    void initOptExtremum();

    double optMin(){return _optMin;};
    double optMax(){return _optMax;};

protected :
    // added fields
    double _optMin;
    double _optMax;

};

class ScannedVariable : public Variable
{
public :

    enum Field
    {
        NAME,
        MODEL,
        VALUE,
        DESCRIPTION,
        /*MIN,
        MAX,
        TYPE,
        CATEGORY,*/
        DATATYPE,

        SCANMIN,
        SCANMAX,
        SCANSTEP
    };

public:
    ScannedVariable(void);
    ScannedVariable(const ScannedVariable &);
    ScannedVariable(const Variable &);
    ScannedVariable(QDomElement &);
    ~ScannedVariable(void);
    ScannedVariable* clone() const;
    virtual QString getClassName(){return "ScannedVariable";};

public :
    //Overwrited functions
    bool check(QString &error);
    QVariant getFieldValue(int, int role = Qt::UserRole) const;
    bool setFieldValue(int ifield, QVariant value_);
    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole){return ScannedVariable::sFieldName(i,role);};


    static const int nbFields = 8;
    virtual unsigned getNbFields(){return nbFields;};


    void initScanExtremum();
    int nbScans();
protected :
    // added fields

    double _scanMin;
    double _scanMax;
    double _scanStep;

};


#endif
