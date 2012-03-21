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
    Variables(bool owner);

    QVariant data(const QModelIndex &index, int role) const;
    Variables* clone() const;

    QStringList mimeTypes() const;
    QMimeData* mimeData(const QModelIndexList &indexes) const;
    Qt::DropActions supportedDropActions() const;

    Variable* findVariable(QString model,QString varName);


private :
};

/**
  * OptVariables is a container for several variables. It contains a
  * modelname concerned in order to improve display. Except this function,
  * it acts as MOVector<OptVariable>
  */
class OptVariables : public MOVector<OptVariable>
{
public:
    OptVariables(bool owner);
    QVariant data(const QModelIndex &index, int role) const;
    OptVariables* clone() const;

private :
};


/**
  * ScannedVariables is a container for several variables. It contains a
  * modelname concerned in order to improve display. Except this function,
  * it acts as MOVector<ScannedVariable>
  */
class ScannedVariables : public MOVector<ScannedVariable>
{
public:
    ScannedVariables(bool owner);
    QVariant data(const QModelIndex &index, int role) const;
    ScannedVariables* clone() const;

private :
};




#endif // VARIABLES_H
