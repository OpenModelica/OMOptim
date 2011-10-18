#include "EIConns.h"

namespace EI
{
EIConns::EIConns()
{
}

EIConns::~EIConns()
{
}

void EIConns::setItems(QDomElement & domEl,const EITree & eiTree)
{
    QDomNode n;
    QDomElement domItem;
    EIConn* newEIConn=NULL;

    n = domEl.firstChild();
    while( !n.isNull() )
    {
        // curLevel elements
        domItem = n.toElement();

        QString node = domItem.nodeName();

        if(node.compare("EIConn",Qt::CaseInsensitive)==0)
        {
            newEIConn = new EIConn(domItem,eiTree);
            addItem(newEIConn);
        }

        n = n.nextSibling();
    }
}

/**
  * Returns list of all streams concerned in connections
  */
QStringList EIConns::allStreams() const
{
    QStringList result;

    for(int i=0;i<items.size();i++)
    {
        result.push_back(items.at(i)->aName());
        result.push_back(items.at(i)->bName());
    }
    result.removeDuplicates();

    return result;
}


/**
  * Returns list of temperature concerning a stream : each inlet and outlet of all its connections.
  */
QList<METemperature> EIConns::TList(QString stream) const
{
    QList<METemperature> result;

    for(int i=0;i<items.size();i++)
    {
        if(items.at(i)->aName()==stream)
        {
            result.push_back(items.at(i)->TinA());
            result.push_back(items.at(i)->ToutA());
        }
        if(items.at(i)->bName()==stream)
        {
            result.push_back(items.at(i)->TinB());
            result.push_back(items.at(i)->ToutB());
        }
    }

    removeDuplicates<METemperature>::apply(result);
    return result;
}


/**
  * Returns list of EIConn that concerning a stream
  */
QList<EIConn> EIConns::filter(QString stream) const
{
    QList<EIConn> result;

    for(int i=0;i<items.size();i++)
    {
        if((items.at(i)->aName()==stream)
                ||(items.at(i)->bName()==stream))
            result.push_back(*items.at(i));
    }
    return result;
}
/**
  * Returns list of EIConn that concern a stream and in which inlet OR outlet temperature equals T
  */
QList<EIConn> EIConns::filterT(QString stream,METemperature T) const
{
    QList<EIConn> result;

    for(int i=0;i<items.size();i++)
    {
        if(items.at(i)->aName()==stream)
        {
            if((items.at(i)->TinA()==T)||(items.at(i)->ToutA()==T))
                result.push_back(*items.at(i));
        }

        if(items.at(i)->bName()==stream)
        {
            if((items.at(i)->TinB()==T)||(items.at(i)->ToutB()==T))
                result.push_back(*items.at(i));
        }

    }
    return result;
}
/**
  * Returns list of EIConn that concern a stream and in which inlet temperature equals Tin
  */
QList<EIConn> EIConns::filterTin(QString stream,METemperature Tin) const
{
    QList<EIConn> result;

    for(int i=0;i<items.size();i++)
    {
        if(items.at(i)->aName()==stream)
        {
            if(items.at(i)->TinA()==Tin)
                result.push_back(*items.at(i));
        }

        if(items.at(i)->bName()==stream)
        {
            if(items.at(i)->TinB()==Tin)
                result.push_back(*items.at(i));
        }

    }
    return result;
}

/**
  * Returns list of EIConn that concern a stream and in which outlets temperature equals Tout
  */
QList<EIConn> EIConns::filterTout(QString stream,METemperature Tout) const
{
    QList<EIConn> result;

    for(int i=0;i<items.size();i++)
    {
        if(items.at(i)->aName()==stream)
        {
            if(items.at(i)->ToutA()==Tout)
                result.push_back(*items.at(i));
        }

        if(items.at(i)->bName()==stream)
        {
            if(items.at(i)->ToutB()==Tout)
                result.push_back(*items.at(i));
        }
    }
    return result;
}
}
