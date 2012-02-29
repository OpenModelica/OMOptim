#ifndef VARIABLES_H
#define VARIABLES_H

#include "MOVector.h"
#include "Variable.h"


/**
  * Variables is a container for several variables. It contains a
  * modelname concerned in order to improve display. Except this function,
  * it acts as MOVector<Variable>
  */
class Variables : public MOVector<Variable>
{
public:
    Variables(bool owner,QString modelName = QString());

    QVariant data(const QModelIndex &index, int role) const;
    void setModelName(QString);
    Variables* clone() const;

    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;


private :
    QString _modelName;
    bool _displayShort;
};

/**
  * OptVariables is a container for several variables. It contains a
  * modelname concerned in order to improve display. Except this function,
  * it acts as MOVector<OptVariable>
  */
class OptVariables : public MOVector<OptVariable>
{
public:
    OptVariables(bool owner,QString modelName = QString());

    QVariant data(const QModelIndex &index, int role) const;
    void setModelName(QString);
    OptVariables* clone() const;

private :
    QString _modelName;
    bool _displayShort;
};


/**
  * ScannedVariables is a container for several variables. It contains a
  * modelname concerned in order to improve display. Except this function,
  * it acts as MOVector<ScannedVariable>
  */
class ScannedVariables : public MOVector<ScannedVariable>
{
public:
    ScannedVariables(bool owner,QString modelName = QString());

    QVariant data(const QModelIndex &index, int role) const;
    void setModelName(QString);
    ScannedVariables* clone() const;

private :
    QString _modelName;
    bool _displayShort;
};




#endif // VARIABLES_H
