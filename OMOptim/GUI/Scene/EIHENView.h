#ifndef EIHENVIEW_H
#define EIHENVIEW_H

#include <QGraphicsView>
#include "EIHEN.h"
#include "EIHENGraphicNode.h"

using namespace EI;

class EIHENView : public QGraphicsView
{
    Q_OBJECT

   public:
       EIHENView(QWidget *parent = 0);

       void itemMoved();

   protected:
       void keyPressEvent(QKeyEvent *event);
      // void timerEvent(QTimerEvent *event);
       void wheelEvent(QWheelEvent *event);
      // void drawBackground(QPainter *painter, const QRectF &rect);

       void scaleView(qreal scaleFactor);

   private:
       EIHENGraphicNode *_topLeftNode;
};

#endif // EIHENVIEW_H
