#include "Variables.h"

#include "ModModelPlus.h"

Variables::Variables(bool owner,ModModelPlus* modModelPlus)
    :MOVector<Variable>(owner)
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

    newVector->_owner = _owner; // indeed, should always be true

    return newVector;
}


Qt::DropActions Variables::supportedDropActions() const{
    return Qt::CopyAction | Qt::MoveAction;
}

QStringList Variables::mimeTypes () const
{
    QStringList types;
    types << "text/plain";
    return types;
}

QMimeData* Variables::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    if(indexes.size()==1)
    {
        Variable* item = (Variable*)indexes.at(0).internalPointer();
        mimeData->setText("Variable::"+item->name(Modelica::FULL));
    }

//    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}



OptVariables::OptVariables(bool owner,ModModelPlus* modModelPlus)
    :MOVector<OptVariable>(owner)
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

    newVector->_owner = _owner; // indeed, should always be true

    return newVector;
}



ScannedVariables::ScannedVariables(bool owner, ModModelPlus* modModelPlus)
    : MOVector<ScannedVariable>(owner)
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

    newVector->_owner = _owner; // indeed, should always be true

    return newVector;
}
