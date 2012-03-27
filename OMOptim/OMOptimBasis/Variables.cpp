#include "Variables.h"

#include <QMimeData>

Variables::Variables(bool owner)
    :MOVector<Variable>(owner)
{
}

QVariant Variables::data(const QModelIndex &index, int role) const
{
        return MOVector<Variable>::data(index,role);
}


Variables* Variables::clone() const
{
    // owner should always be true
    Variables* newVector = new Variables(true);

    int i;
    Variable* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new Variable(*items.at(i));
        newVector->addItem(newItem);
    }  

    return newVector;
}


Qt::DropActions Variables::supportedDropActions() const{
    return Qt::CopyAction | Qt::MoveAction;
}

Variable *Variables::findVariable(QString model, QString varName)
{
    int iVar=0;
    bool found = 0;
    while(!found && (iVar<this->size()))
    {
        found = ((this->at(iVar)->name()==varName) && (this->at(iVar)->model()==model));
        if(!found)
            iVar++;
    }
    if(found)
        return this->at(iVar);
    else
        return NULL;
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
        mimeData->setText("Variable::"+item->name(Variable::FULL));
    }

//    mimeData->setData("application/vnd.text.list", encodedData);
    return mimeData;
}



OptVariables::OptVariables(bool owner)
    :MOVector<OptVariable>(owner)
{

}

QVariant OptVariables::data(const QModelIndex &index, int role) const
{
        return MOVector<OptVariable>::data(index,role);
}



OptVariables* OptVariables::clone() const
{
     // owner should always be true
    OptVariables* newVector = new OptVariables(true);

    int i;
    OptVariable* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new OptVariable(*items.at(i));
        newVector->addItem(newItem);
    }

    return newVector;
}



ScannedVariables::ScannedVariables(bool owner)
    : MOVector<ScannedVariable>(owner)
{
}

QVariant ScannedVariables::data(const QModelIndex &index, int role) const
{
        return MOVector<ScannedVariable>::data(index,role);
}

ScannedVariables* ScannedVariables::clone() const
{
     // owner should always be true
    ScannedVariables* newVector = new ScannedVariables(true);

    int i;
    ScannedVariable* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new ScannedVariable(*items.at(i));
        newVector->addItem(newItem);
    }


    return newVector;
}
