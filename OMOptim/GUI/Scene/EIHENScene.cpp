#include "EIHENScene.h"

#include "EIHENGraphicNode.h"


EIHENScene::EIHENScene(const EIHEN & eiHEN,QObject *parent) :
    QGraphicsScene(parent)
{
    _eiHEN = eiHEN;



    this->setItemIndexMethod(QGraphicsScene::NoIndex);
    this->setSceneRect(-200, -200, 400, 400);

    initialize();

}
EIHENScene::~EIHENScene()
{
    for(int i=0;i<_allEdges.size();i++)
        delete _allEdges.at(i);

    for(int i=0;i<_mapNodes.values().size();i++)
        delete _mapNodes.values().at(i);

}

void EIHENScene::initialize()
{


    // "gr" prefix : graphic version

    QList<EIHEN_RootNode*> roots =  _eiHEN.mapRoots().values();
    QList<EIHENGraphicNode*> grRoots;
    EIHENGraphicNode* curGrNode;
    QList<EIHENGraphicNode*> pendingGrNodes;

    QRectF newStreamRect;
    QRectF oldStreamRect;
    qreal oldBottom;

    METemperature Tmax, Tmin;
    Tmin = _eiHEN.Tmin();
    Tmax = _eiHEN.Tmax();
    _TXFactor = 1000/(Tmax.value(METemperature::K)-Tmin.value(METemperature::K));


    QList<EIHENGraphicNode*> curStreamGrNodes;

    // for each stream
    for(int i=0;i<roots.size();i++)
    {
        pendingGrNodes.clear();
        curStreamGrNodes.clear();

        // starting from roots node
        curGrNode = addGrNode(roots.at(i));
        curGrNode->setPos((Tmax.value(METemperature::K)-roots.at(i)->_outletT.value(METemperature::K))*_TXFactor,0);
        _nodesSections.insert(curGrNode,QString::number(i+1));

        grRoots.push_back(curGrNode);

        pendingGrNodes.push_back(curGrNode);

        // follow lines
        while(!pendingGrNodes.isEmpty())
        {
            if(!curStreamGrNodes.contains(pendingGrNodes.at(0)))
                curStreamGrNodes.push_back(pendingGrNodes.at(0));

            advance(pendingGrNodes.at(0),pendingGrNodes,newStreamRect);
        }

        // move new rect
        for(int iGr=0;iGr<curStreamGrNodes.size();iGr++)
        {
            curStreamGrNodes.at(iGr)->moveBy(0,oldStreamRect.bottom()+newStreamRect.height());
        }

        // reset rect
        oldBottom = oldStreamRect.bottom()+80;
        oldStreamRect = newStreamRect;
        oldStreamRect.moveBottom(oldBottom+newStreamRect.height());
        newStreamRect = QRectF();
    }

    // for each HE
    for(int i=0;i<_eiHEN.hes().size();i++)
    {
        addGrQEdge(_mapNodes.value(_eiHEN.hes().at(i)->nodeA()),_mapNodes.value(_eiHEN.hes().at(i)->nodeB()));
    }

    // spread along y
    spreadAlongY();

    // autoscale
    setSceneRect(QRectF());

}


void EIHENScene::advance(EIHENGraphicNode* startGrNode,QList<EIHENGraphicNode*> & pendingGrNodes, QRectF& streamRect)
{
    QList<EIHEN_Node*> pendingNodes = startGrNode->node()->_outlets;
    EIHENGraphicNode* newGrNode;
    EIHENGraphicEdge* newEdge;



    double DX = 100;



    for(int i=0;i<pendingNodes.size();i++)
    {
        // test if pending node already created (in case of mixer)
        if(_mapNodes.contains(pendingNodes.at(i)))
            newGrNode = _mapNodes.value(pendingNodes.at(i));
        else// create corresponding graphics nodes
            newGrNode = addGrNode(pendingNodes.at(i));

        //abscissa depending on temperature
        METemperature Taverage;
        Taverage.setValue((pendingNodes.at(i)->_inletT.value(METemperature::K)+pendingNodes.at(i)->_outletT.value(METemperature::K))/2,METemperature::K);

        METemperature TMax = _eiHEN.Tmax();


        newGrNode->setX((TMax.value(METemperature::K)-Taverage.value(METemperature::K))*_TXFactor/*-newGrNode->boundingRect().width()/2*/);

        if(pendingNodes.size()>1)
        {
            _nodesSections.insert(newGrNode,_nodesSections.value(startGrNode)+"."+QString::number(i+1));
        }
        else
        {
            // if this node has several inlets, remove last section numbering
            if(pendingNodes.at(i)->_inlets.size()>1)
                _nodesSections.insert(newGrNode,_nodesSections.value(startGrNode).section(".",0,-2));
            else
                // else keep the same
                _nodesSections.insert(newGrNode,_nodesSections.value(startGrNode));
        }
        // create an edge
        newEdge = addGrEdge(startGrNode,newGrNode);

        //add it in pendingGrNodes
        pendingGrNodes.push_back(newGrNode);

        streamRect = streamRect.united(newGrNode->boundingRect());
    }

    pendingGrNodes.removeAll(startGrNode);
}

EIHENGraphicNode* EIHENScene::addGrNode(EIHEN_Node* node)
{
    EIHENGraphicNode* grNode = new EIHENGraphicNode(node,_TXFactor);
    grNode->setZValue(1);
    this->addItem(grNode);

    _mapNodes.insert(node,grNode);
    return grNode;
}

EIHENGraphicEdge* EIHENScene::addGrEdge(EIHENGraphicNode* from,EIHENGraphicNode* to)
{
    EIHENGraphicEdge* grEdge = new EIHENGraphicEdge(from,to);
    grEdge->setZValue(0);
    this->addItem(grEdge);
    _allEdges.push_back(grEdge);
    return grEdge;
}

EIHENGraphicQEdge* EIHENScene::addGrQEdge(EIHENGraphicNode* a,EIHENGraphicNode* b)
{
    if(a && b)
    {
        EIHENGraphicQEdge* grqEdge = new EIHENGraphicQEdge(a,b);
        grqEdge->setZValue(0);
        this->addItem(grqEdge);
        _allEdges.push_back(grqEdge);
        return grqEdge;
    }
    else
        return NULL;
}

void EIHENScene::spreadAlongY()
{
    // attribute an y to each section
    QStringList sections = _nodesSections.values();
    sections.removeDuplicates();
    sections.sort();


    QStringList rootSections = sections.filter(QRegExp("^[\\d]+$"));


    // create scale of y
    QMap<QString,double> sectionY;


    QMap<QString,QStringList> descendants;
    QMap<QString,QStringList> children;
    QMap<QString,QString> parents;
    QStringList curDescendants;
    QStringList curChildren;
    // filling first maps
    for(int i=0;i<sections.size();i++)
    {
        curDescendants = sections.filter(QRegExp("^"+sections.at(i)+"."));
        descendants.insert(sections.at(i),curDescendants);

        curChildren = sections.filter(QRegExp("^"+sections.at(i)+".[\\d]*"));
        children.insert(sections.at(i),curChildren);

        for(int iC=0;iC<curChildren.size();iC++)
            parents.insert(curChildren.at(iC),sections.at(i));
    }

    double lastY=0;
    for(int iR=0;iR<rootSections.size();iR++)
    {
        attributeChildOffset(children,parents,rootSections.at(iR),sectionY,lastY);
    }

    // attribute Y
    EIHENGraphicNode* curNode;
    double DY = 60;
    for(int i=0;i<_nodesSections.uniqueKeys().size();i++)
    {
        curNode = _nodesSections.uniqueKeys().at(i);
        curNode->setY(sectionY.value(_nodesSections.value(curNode))*DY);
    }
}

void EIHENScene::attributeChildOffset(const QMap<QString,QStringList> & children,const QMap<QString,QString> & parents,QString curSection,QMap<QString,double> &sectionY,double & lastY)
{
    QStringList curChildren = children.value(curSection);

    if(!curChildren.isEmpty())
    {
        double average=0;
        for(int iC=0;iC<curChildren.size();iC++)
        {
            attributeChildOffset(children,parents, curChildren.at(iC),sectionY,lastY);
            average+=sectionY.value(curChildren.at(iC));
        }
        average = average / curChildren.size();

        // set this y : average of children
        sectionY.insert(curSection,average);
    }
    else
    {
        QStringList brothers = children.value(parents.value(curSection));
        for(int iB=0;iB<brothers.size();iB++)
        {
            if(!sectionY.keys().contains(brothers.at(iB)))
            {
                sectionY.insert(brothers.at(iB),lastY+1);
                lastY = lastY+1;
            }
        }
        if(brothers.isEmpty())
        {
            sectionY.insert(curSection,lastY+1);
            lastY = lastY+1;
        }
    }
}

