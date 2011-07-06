#include "EIConns.h"

EIConns::EIConns()
{
}

EIConns::~EIConns()
{
}

void EIConns::setItems(QDomElement & domEl,EITree* eiTree)
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
