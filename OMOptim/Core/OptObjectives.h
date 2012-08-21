#ifndef OPTOBJECTIVES_H
#define OPTOBJECTIVES_H

#include "OptObjective.h"
#include "MOVector.h"

class ModelPlus;
class ModModelPlus;

/**
  * OptObjectives is a container for several variables. It contains a reference to
  * modModelPlus concerned in order to improve display. Except this function,
  * it acts as MOVector<OptObjective>
  */
class OptObjectives : public MOVector<OptObjective>
{
public:
    OptObjectives(bool owner);

    QVariant data(const QModelIndex &index, int role) const;

    OptObjectives* clone() const;


};


#endif // OPTOBJECTIVES_H
