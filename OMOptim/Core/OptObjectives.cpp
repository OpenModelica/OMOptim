#include "OptObjectives.h"

#include "ModModelPlus.h"

OptObjectives::OptObjectives(bool owner, ModModelPlus* modModelPlus)
    :MOVector<OptObjective>(owner)
{
    _modModelPlus = modModelPlus;
    _displayShort = true;
}

QVariant OptObjectives::data(const QModelIndex &index, int role) const
{
    if(index.isValid()
            && _modModelPlus
            && (index.column()== OptObjective::NAME)
            && (role==Qt::DisplayRole)
            && _displayShort)
    {
        QString shortName = items.at(index.row())->name();
        shortName = shortName.remove(QRegExp("^"+_modModelPlus->modModelName()+"."));
        return QVariant(shortName);
    }
    else
        return MOVector<OptObjective>::data(index,role);
}

void OptObjectives::setModModelPlus(ModModelPlus* modModelPlus)
{
    _modModelPlus = modModelPlus;
}

OptObjectives* OptObjectives::clone() const
{
    OptObjectives* newVector = new OptObjectives(_modModelPlus);

    int i;
    OptObjective* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new OptObjective(*items.at(i));
        newVector->addItem(newItem);
    }
    return newVector;
}
