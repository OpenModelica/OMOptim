#include "EIConnConstrs.h"

EIConnConstrs::EIConnConstrs(void)
{
}

EIConnConstrs::~EIConnConstrs(void)
{
}

EIConnConstrs::EIConnConstrs(const EIConnConstrs &b)
{
    for(int i=0;i<b.items.size();i++)
        items.push_back(b.items.at(i)->clone());
}

EIConnConstrs* EIConnConstrs::clone()
{
        EIConnConstrs* cloned = new EIConnConstrs(*this);
	return cloned;
}


EIConnConstrs::EIConnConstrs(QDomElement & domList, EITree* eiTree)
{
	QDomNode n = domList.firstChild();
	while( !n.isNull() )
	{
		QDomElement domItem = n.toElement();
		QDomAttr domAttr;
		if( !domItem.isNull() )
		{
                        EIConnConstr* newItem = new EIConnConstr(domItem,eiTree);
			addItem(newItem);
		}
		n = n.nextSibling();
	}
}

QMultiMap<EIStream*,EIStream*> EIConnConstrs::getMapStreams(MOOptVector *variables)
{
	QMultiMap<EIStream*,EIStream*> result;
	QMultiMap<EIStream*,EIStream*> curMap;
	for(int iMap=0;iMap<items.size();iMap++)
	{
                curMap = items.at(iMap)->getMapStreams(variables);
		for(int i=0;i<curMap.keys().size();i++)
			for(int j=0;j<curMap.values(curMap.keys().at(i)).size();j++)
			result.insert(curMap.keys().at(i),curMap.values(curMap.keys().at(i)).at(j));
	}
	return result;
}
