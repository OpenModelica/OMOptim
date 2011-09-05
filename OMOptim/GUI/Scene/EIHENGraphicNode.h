#ifndef EIHENGRAPHICNODE_H
#define EIHENGRAPHICNODE_H

#include <QGraphicsItem>
#include "EIHEN.h"


 class EIHENGraphicEdge;
 class QGraphicsSceneMouseEvent;

 using namespace EI;

class EIHENGraphicNode : public QGraphicsWidget
{

    Q_OBJECT
     public:
         EIHENGraphicNode(EIHEN_Node *node,double TXFactor);

         void addEdge(EIHENGraphicEdge *edge);
         QList<EIHENGraphicEdge *> edges() const;

         EIHEN_Node *node(){return _node;}
         QRectF boundingRect() const;
         QPainterPath shape() const;
         void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

     protected:
         QVariant itemChange(GraphicsItemChange change, const QVariant &value);

         void mousePressEvent(QGraphicsSceneMouseEvent *event);
         void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

         QString nodeToolTip();

         QRectF HERect(EIHEN_HENode* node) const;
         QRectF classicNodeRect(EIHEN_Node* node,bool withText=false,Qt::AnchorPoint textPos=Qt::AnchorTop) const;

         QPainterPath HEPainter(EIHEN_HENode* node) const;
         QPainterPath classicNodePainter(EIHEN_Node* node,bool withText=false,Qt::AnchorPoint textPos=Qt::AnchorTop) const;


     private:
         QList<EIHENGraphicEdge *> _edges;
         QPointF _newPos;
         EIHEN_Node *_node;
         double _TXfactor; //DX = DT * TXfactor



};

#endif // EIHENGRAPHICNODE_H
