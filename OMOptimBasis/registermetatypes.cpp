#include "registermetatypes.h"
#include "Info.h"

#include <QtCore>

RegisterMetaTypes::RegisterMetaTypes()
{
}

void RegisterMetaTypes::init()
{
    // Register classes as a metaType

    // Needed for Info communication between threads
    // or even for simple connect functions
    qRegisterMetaType<Info>();

    qRegisterMetaType< QList<int> >( "QList<int>" );

}
