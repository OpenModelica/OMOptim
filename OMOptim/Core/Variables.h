#ifndef VARIABLES_H
#define VARIABLES_H

#include "MOVector.h"
#include "Variable.h"


class ModModelPlus;

/**
  * Variables is a container for several variables. It contains a reference to
  * modModelPlus concerned in order to improve display. Except this function,
  * it acts as MOVector<Variable>
  */
class Variables : public MOVector<Variable>
{
public:
    Variables(ModModelPlus* modModelPlus=NULL);

    QVariant data(const QModelIndex &index, int role) const;
    void setModModelPlus(ModModelPlus*);
    Variables* clone() const;

private :
    ModModelPlus* _modModelPlus;
    bool _displayShort;
};

/**
  * OptVariables is a container for several variables. It contains a reference to
  * modModelPlus concerned in order to improve display. Except this function,
  * it acts as MOVector<OptVariable>
  */
class OptVariables : public MOVector<OptVariable>
{
public:
    OptVariables(ModModelPlus* modModelPlus=NULL);

    QVariant data(const QModelIndex &index, int role) const;
    void setModModelPlus(ModModelPlus*);
    OptVariables* clone() const;

private :
    ModModelPlus* _modModelPlus;
    bool _displayShort;
};


/**
  * ScannedVariables is a container for several variables. It contains a reference to
  * modModelPlus concerned in order to improve display. Except this function,
  * it acts as MOVector<ScannedVariable>
  */
class ScannedVariables : public MOVector<ScannedVariable>
{
public:
    ScannedVariables(ModModelPlus* modModelPlus=NULL);

    QVariant data(const QModelIndex &index, int role) const;
    void setModModelPlus(ModModelPlus*);
    ScannedVariables* clone() const;

private :
    ModModelPlus* _modModelPlus;
    bool _displayShort;
};




#endif // VARIABLES_H
