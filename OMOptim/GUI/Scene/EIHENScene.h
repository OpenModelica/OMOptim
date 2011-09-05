#ifndef EIHENSCENE_H
#define EIHENSCENE_H

#include <QGraphicsScene>
#include "EIHENGraphicNode.h"
#include "EIHENGraphicEdge.h"
#include "EIHEN.h"
#include <QMap>
#include <cmath>

class EIHENScene : public QGraphicsScene
{
    Q_OBJECT
public:
    EIHENScene(const EIHEN &, QObject *parent = 0);
    ~EIHENScene();


protected :
    void initialize();
    void advance(EIHENGraphicNode* curGrNode,QList<EIHENGraphicNode*> &pendingGrNodes,QRectF& streamRect);
    void spreadAlongY();
    void attributeChildOffset(const QMap<QString,QStringList> & ,const QMap<QString,QString> & ,QString ,QMap<QString,double> &,double & );


    EIHEN _eiHEN;
    QMap<EIHEN_Node*,EIHENGraphicNode*> _mapNodes;
    QList<EIHENGraphicEdge*> _allEdges;

    EIHENGraphicNode* addGrNode(EIHEN_Node* node);
    EIHENGraphicEdge* addGrEdge(EIHENGraphicNode* from,EIHENGraphicNode* to);
    EIHENGraphicQEdge* addGrQEdge(EIHENGraphicNode* a,EIHENGraphicNode* b);

    double _TXFactor;
    QMap<EIHENGraphicNode*,QString> _nodesSections; //ylevels for graphic nodes


};

#endif // EIHENSCENE_H
