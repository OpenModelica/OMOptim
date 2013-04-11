#include "Variables.h"
#include "MOItem.h"
#include <QMimeData>

Variables::Variables(bool owner, QObject *parent)
    :MOVector<Variable>(owner)
{
//    this->setParent(parent);
}

Variables::~Variables()
{
}

QVariant Variables::data(const QModelIndex &index, int role) const
{
        return MOVector<Variable>::data(index,role);
}


Variables* Variables::clone() const
{
    // owner should always be true
    Variables* newVector = new Variables(true,NULL);

    int i;
    Variable* newItem;
    for(i=0;i<_items.size();i++)
    {
        newItem = new Variable(*this->at(i));
        newVector->addItem(newItem);
    }  

    return newVector;
}


Qt::DropActions Variables::supportedDropActions() const{
    return Qt::CopyAction | Qt::MoveAction;
}

Variable *Variables::findVariable(QString model, QString shortVarName)
{
    int iVar=0;
    bool found = 0;
    while(!found && (iVar<this->size()))
    {
        found = ((this->at(iVar)->name(Variable::SHORT)==shortVarName) && (this->at(iVar)->model()==model));
        if(!found)
            iVar++;
    }
    if(found)
        return this->at(iVar);
    else
        return NULL;
}

int Variables::findVariable(QString model, QRegExp & shortVarName, int from)
{

    int iVar=from;
    bool found = 0;
    while((!found) && (iVar<this->size()))
    {
        found = (shortVarName.exactMatch(this->at(iVar)->name(Variable::SHORT)) && (this->at(iVar)->model()==model));
        if(!found)
            iVar++;
    }
    if(found)
        return iVar;
    else
        return -1;
}


//QStringList Variables::mimeTypes () const
//{
//    QStringList types;
//    types << "text/plain";
//    return types;
//}

//QMimeData* Variables::mimeData(const QModelIndexList &indexes) const
//{
//    QMimeData *mimeData = new QMimeData();
//    QByteArray encodedData;

//    QDataStream stream(&encodedData, QIODevice::WriteOnly);
//    if(indexes.size()==1)
//    {
//        Variable* item = (Variable*)indexes.at(0).internalPointer();
//        mimeData->setData("text/XML",QString("Variable::"+item->name(Variable::FULL)).toAscii());
//    }

//    mimeData->setText(
////    mimeData->setData("application/vnd.text.list", encodedData);
//    return mimeData;
//}



OptVariables::OptVariables(bool owner,QObject* parent)
    :MOVector<OptVariable>(owner,parent)
{

}

OptVariables::~OptVariables()
{
}

QVariant OptVariables::data(const QModelIndex &index, int role) const
{
        return MOVector<OptVariable>::data(index,role);
}

OptVariables* OptVariables::clone() const
{
     // owner should always be true
    OptVariables* newVector = new OptVariables(true,NULL);

    int i;
    OptVariable* newItem;
    for(i=0;i<_items.size();i++)
    {
        newItem = new OptVariable(*this->at(i));
        newVector->addItem(newItem);
    }

    return newVector;
}



ScannedVariables::ScannedVariables(bool owner, QObject *parent)
    : MOVector<ScannedVariable>(owner,parent)
{
}

ScannedVariables::~ScannedVariables()
{
}

QVariant ScannedVariables::data(const QModelIndex &index, int role) const
{
        return MOVector<ScannedVariable>::data(index,role);
}

ScannedVariables* ScannedVariables::clone() const
{
     // owner should always be true
    ScannedVariables* newVector = new ScannedVariables(true,NULL);

    int i;
    ScannedVariable* newItem;
    for(i=0;i<_items.size();i++)
    {
        newItem = new ScannedVariable(*this->at(i));
        newVector->addItem(newItem);
    }


    return newVector;
}
