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

    @file Variable.h
    @brief Comments for file documentation.
    @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
    Company : CEP - ARMINES (France)
    http://www-cep.ensmp.fr/english/
    @version

  */
#if !defined(_VARIABLE_H)
#define _VARIABLE_H

#include <limits>

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#include <QTextStream>
#else
#include <QtCore/QVector>
#include <QtCore/QObject>
#include <QtCore/QAbstractTableModel>
#include <QtCore/QTextStream>
#include <QtCore/QStringList>
#include <QtXml/QDomDocument>
#endif

#include "MOItem.h"
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
        DATATYPE,
        CAUSALITY
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

    Variable & operator=(const Variable &);

    virtual Variable* clone() const;

    virtual QString getClassName() const {return "Variable";};

public :

    void setModel(QString);
    QString model() const;


    //Overwrited functions
    virtual QVariant getFieldValue(int, int role = Qt::UserRole) const;
    virtual bool setFieldValue(int ifield, QVariant value_);
    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole) const {return Variable::sFieldName(i,role);};

    static const int nbFields = 6;
    virtual unsigned getNbFields( ) const {return nbFields;};

    //Specific functions
    void setDescription(QString);
    void setValue(double);
    void setValue(QVariant);
    double doubleValue(bool *ok= NULL) const;
    QVariant value() const;
    void setDataType(VariableType);
    VariableType getDataType()const{return _dataType;}
    void setCausality(VariableCausality causality);
    VariableCausality causality();
    QString description();
    virtual QString name(Variable::NameFormat = Variable::FULL) const;
    virtual QString getStrToolTip();
    bool isBoolean() const;

    virtual bool valueDefined(){return _filledFields.contains(Variable::VALUE);}


protected :
    // added fields
    QString _description;
    QString _model;

    //int type;
    //int category;
    VariableType _dataType;
    VariableCausality _causality;
    QVariant _value;
};



class VariableResult : public Variable
{

    Q_OBJECT

public:
    VariableResult(void);
    VariableResult(const VariableResult& );
    VariableResult(const Variable &);
    VariableResult(QDomElement &);

    Variable extractPoint(int iPoint,int iScan = 0);

    ~VariableResult(void);

    VariableResult* clone() const;
    virtual QString getClassName() const {return "VariableResult";};


    static const int nbFields = 6;

private:
    QVector<QVector<double> > _finalValues;
    QVector<QVector<bool> > _computedPoints;

public :
    //Overwrited functions
    //VariableResult* clone() const;


    //Added functions
    QVector<double> finalValues(int iScan) const;
    bool isComputedPoint(int iScan,int iPoint) const;
    int nbPoints() const;
    int nbScans() const;
    double finalValue(int iScan, int iPoint) const;
    QVector<double> finalValuesAtPoint(int iPoint);
    QVector<double> finalValuesAtScan(int iScan);
    void setFinalValuesAtScan(int iScan, const QVector<double> &);
    void setFinalValuesAtPoint(int iPoint,const QVector<double> &);
    void setFinalValueAtPoint(int iPoint,double );
    void setFinalValue(int iScan, int iPoint,double,bool computed = true);
//    void appendFinalValue(double, int iScan);
//    void appendScanValues(QVector<double> _values,QVector<bool> _computedPoints);
//    void appendScanValue(double _value,bool _computedPoint);
    void clearFinalValues();
    void clearFinalValuesAtIpoint(int iPoint);

    virtual QDomElement toXmlData(QDomDocument & doc);
    QString valuesToCSV();
    void updateValuesFromCsv(QString text);
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
        CAUSALITY,
        OPTMIN,
        OPTMAX
    };

public:
    OptVariable(void);
    OptVariable(QString name);
    OptVariable(const Variable &);
    ~OptVariable(void);
    OptVariable(const OptVariable &);
    OptVariable(QDomElement &);
    OptVariable* clone() const;
    OptVariable & operator=(const OptVariable &);
    virtual QString getClassName() const {return "OptVariable";};


public :
    //Overwrited functions
    virtual bool check(QString &error);
    virtual QVariant getFieldValue(int, int role = Qt::UserRole) const;
    virtual bool setFieldValue(int ifield, QVariant value_);
    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole) const {return OptVariable::sFieldName(i,role);};
    static const int nbFields = 8;
    virtual unsigned getNbFields( ) const {return nbFields;};

    void initOptExtremum();

    double optMin(){return _optMin;};
    double optMax(){return _optMax;};

protected :
    // added fields
    double _optMin;
    double _optMax;

};

/**
  * Variable used to do sampling computations :
  * A variable will take successively several values,
  * between  ScanMin and ScanMax with ScanStep increase.
  * @warning ScannedVariable is sometimes called SamplingVariable
  */
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
        CAUSALITY,
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
    virtual QString getClassName() const {return "ScannedVariable";};

public :
    //Overwrited functions
    bool check(QString &error);
    QVariant getFieldValue(int, int role = Qt::UserRole) const;
    bool setFieldValue(int ifield, QVariant value_);
    static QString sFieldName(int field, int role);
    virtual QString getFieldName(int i, int role = Qt::DisplayRole) const {return ScannedVariable::sFieldName(i,role);};


    static const int nbFields = 9;
    virtual unsigned getNbFields( ) const {return nbFields;};


    void initScanExtremum();
    int nbScans();
protected :
    // added fields

    double _scanMin;
    double _scanMax;
    double _scanStep;

};


#endif
