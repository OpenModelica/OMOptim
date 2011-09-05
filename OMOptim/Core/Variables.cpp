#include "Variables.h"

#include "ModModelPlus.h"

Variables::Variables(ModModelPlus* modModelPlus)
{
    _modModelPlus = modModelPlus;
    _displayShort = true;
}

QVariant Variables::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && _modModelPlus
            && (index.column()== Variable::NAME)
            && (role==Qt::DisplayRole)
            && _displayShort)
    {
        QString shortName = items.at(index.row())->name();
        shortName = shortName.remove(QRegExp("^"+_modModelPlus->modModelName()+"."));
        return QVariant(shortName);
    }
    else
        return MOVector<Variable>::data(index,role);
}

void Variables::setModModelPlus(ModModelPlus* modModelPlus)
{
    _modModelPlus = modModelPlus;

}

Variables* Variables::clone() const
{
    Variables* newVector = new Variables(_modModelPlus);

    int i;
    Variable* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new Variable(*items.at(i));
        newVector->addItem(newItem);
    }
    return newVector;
}

OptVariables::OptVariables(ModModelPlus* modModelPlus)
{
    _modModelPlus = modModelPlus;
    _displayShort = true;
}

QVariant OptVariables::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && _modModelPlus
            && (index.column()== OptVariable::NAME)
            && (role==Qt::DisplayRole)
            && _displayShort)
    {
        QString shortName = items.at(index.row())->name();
        shortName = shortName.remove(QRegExp("^"+_modModelPlus->modModelName()+"."));
        return QVariant(shortName);
    }
    else
        return MOVector<OptVariable>::data(index,role);
}

void OptVariables::setModModelPlus(ModModelPlus* modModelPlus)
{
    _modModelPlus = modModelPlus;

}

OptVariables* OptVariables::clone() const
{
    OptVariables* newVector = new OptVariables(_modModelPlus);

    int i;
    OptVariable* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new OptVariable(*items.at(i));
        newVector->addItem(newItem);
    }
    return newVector;
}



ScannedVariables::ScannedVariables(ModModelPlus* modModelPlus)
{
    _modModelPlus = modModelPlus;
    _displayShort = true;
}

QVariant ScannedVariables::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && _modModelPlus
            && (index.column()== ScannedVariable::NAME)
            && (role==Qt::DisplayRole)
            && _displayShort)
    {
        QString shortName = items.at(index.row())->name();
        shortName = shortName.remove(QRegExp("^"+_modModelPlus->modModelName()+"."));
        return QVariant(shortName);
    }
    else
        return MOVector<ScannedVariable>::data(index,role);
}

void ScannedVariables::setModModelPlus(ModModelPlus* modModelPlus)
{
    _modModelPlus = modModelPlus;

}

ScannedVariables* ScannedVariables::clone() const
{
    ScannedVariables* newVector = new ScannedVariables(_modModelPlus);

    int i;
    ScannedVariable* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new ScannedVariable(*items.at(i));
        newVector->addItem(newItem);
    }
    return newVector;
}
