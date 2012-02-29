#include "OptObjectives.h"

#include "ModModelPlus.h"

OptObjectives::OptObjectives(bool owner, QString modelName)
    :MOVector<OptObjective>(owner)
{
    _modelName = modelName;
    _displayShort = true;
}

QVariant OptObjectives::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && (index.column()== OptObjective::NAME)
            && (role==Qt::DisplayRole)
            && _displayShort)
    {
        QString shortName = items.at(index.row())->name();
        shortName = shortName.remove(QRegExp("^"+_modelName+"."));
        return QVariant(shortName);
    }
    else
        return MOVector<OptObjective>::data(index,role);
}

void OptObjectives::setModelName(QString modelName)
{
    _modelName = modelName;
}

OptObjectives* OptObjectives::clone() const
{
    OptObjectives* newVector = new OptObjectives(true,_modelName);

    int i;
    OptObjective* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new OptObjective(*items.at(i));
        newVector->addItem(newItem);
    }
    return newVector;
}
