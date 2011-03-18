#include "EIConnConstrs.h"

EIConnConstrs::EIConnConstrs(void)
{
}

EIConnConstrs::~EIConnConstrs(void)
{
}

EIConnConstrs* EIConnConstrs::clone()
{
	EIConnConstrs* cloned = new EIConnConstrs();
	for(int i=0;i<items.size();i++)
	{
		cloned->addItem(items.at(i)->clone());
	}
	return cloned;
}


EIConnConstrs::EIConnConstrs(QDomElement & domList, EIItem* rootEI, EIReader* eiReader)
{
	QDomNode n = domList.firstChild();
	while( !n.isNull() )
	{
		QDomElement domItem = n.toElement();
		QDomAttr domAttr;
		if( !domItem.isNull() )
		{
			EIConnConstr* newItem = new EIConnConstr(domItem,rootEI,eiReader);
			addItem(newItem);
		}
		n = n.nextSibling();
	}
}

QMultiMap<EIStream*,EIStream*> EIConnConstrs::getMapStreams(EIReader* eiReader,MOOptVector *variables)
{
	QMultiMap<EIStream*,EIStream*> result;
	QMultiMap<EIStream*,EIStream*> curMap;
	for(int iMap=0;iMap<items.size();iMap++)
	{
		curMap = items.at(iMap)->getMapStreams(eiReader,variables);
		for(int i=0;i<curMap.keys().size();i++)
			for(int j=0;j<curMap.values(curMap.keys().at(i)).size();j++)
			result.insert(curMap.keys().at(i),curMap.values(curMap.keys().at(i)).at(j));
	}
	return result;
}