#include "OptObjectives.h"

#include "ModModelPlus.h"

OptObjectives::OptObjectives(bool owner)
    :MOVector<OptObjective>(owner)
{
}

QVariant OptObjectives::data(const QModelIndex &index, int role) const
{
        return MOVector<OptObjective>::data(index,role);
}

OptObjectives* OptObjectives::clone() const
{
    OptObjectives* newVector = new OptObjectives(true);

    int i;
    OptObjective* newItem;
    for(i=0;i<items.size();i++)
    {
        newItem = new OptObjective(*items.at(i));
        newVector->addItem(newItem);
    }
    return newVector;
}
