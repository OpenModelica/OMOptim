#ifndef OPTOBJECTIVES_H
#define OPTOBJECTIVES_H

#include "OptObjective.h"
#include "MOVector.h"

class ModModelPlus;

/**
  * OptObjectives is a container for several variables. It contains a reference to
  * modModelPlus concerned in order to improve display. Except this function,
  * it acts as MOVector<OptObjective>
  */
class OptObjectives : public MOVector<OptObjective>
{
public:
    OptObjectives(bool owner,ModModelPlus* modModelPlus=NULL);

    QVariant data(const QModelIndex &index, int role) const;
    void setModModelPlus(ModModelPlus*);
    OptObjectives* clone() const;

private :
    ModModelPlus* _modModelPlus;
    bool _displayShort;
};


#endif // OPTOBJECTIVES_H
