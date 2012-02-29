#include "Variables.h"

#include <QMimeData>

Variables::Variables(bool owner,QString modelName)
    :MOVector<Variable>(owner)
{
    _modelName = modelName;
    _displayShort = true;
}

QVariant Variables::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && (index.column()== Variable::NAME)
            && (role==Qt::DisplayRole)
            && _displayShort)
    {
        QString shortName = items.at(index.row())->name();
        shortName = shortName.remove(QRegExp("^"+_modelName+"."));
        return QVariant(shortName);
    }
    else
        return MOVector<Variable>::data(index,role);
}

void Variables::setModelName(QString modelName)
{
    _modelName = modelName;
}

Variables* Variables::clone() const
{
    // owner should always be true
    Variables* newVector = new Variables(true,_modelName);

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



OptVariables::OptVariables(bool owner,QString modelName)
    :MOVector<OptVariable>(owner)
{
    _modelName = modelName;
    _displayShort = true;
}

QVariant OptVariables::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && (index.column()== OptVariable::NAME)
            && (role==Qt::DisplayRole)
            && _displayShort)
    {
        QString shortName = items.at(index.row())->name();
        shortName = shortName.remove(QRegExp("^"+_modelName+"."));
        return QVariant(shortName);
    }
    else
        return MOVector<OptVariable>::data(index,role);
}


void OptVariables::setModelName(QString modelName)
{
    _modelName = modelName;
}

OptVariables* OptVariables::clone() const
{
     // owner should always be true
    OptVariables* newVector = new OptVariables(true,_modelName);

    int i;
    OptVariable* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new OptVariable(*items.at(i));
        newVector->addItem(newItem);
    }

    return newVector;
}



ScannedVariables::ScannedVariables(bool owner, QString modelName)
    : MOVector<ScannedVariable>(owner)
{
    _modelName = modelName;
    _displayShort = true;
}

QVariant ScannedVariables::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && (index.column()== ScannedVariable::NAME)
            && (role==Qt::DisplayRole)
            && _displayShort)
    {
        QString shortName = items.at(index.row())->name();
        shortName = shortName.remove(QRegExp("^"+_modelName+"."));
        return QVariant(shortName);
    }
    else
        return MOVector<ScannedVariable>::data(index,role);
}

void ScannedVariables::setModelName(QString modelName)
{
    _modelName = modelName;
}

ScannedVariables* ScannedVariables::clone() const
{
     // owner should always be true
    ScannedVariables* newVector = new ScannedVariables(true,_modelName);

    int i;
    ScannedVariable* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new ScannedVariable(*items.at(i));
        newVector->addItem(newItem);
    }


    return newVector;
}
