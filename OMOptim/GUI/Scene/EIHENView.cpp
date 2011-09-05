#include "EIHENView.h"
#include "EIHENGraphicNode.h"
#include <math.h>

 EIHENView::EIHENView(QWidget *parent)
     : QGraphicsView(parent)
 {

     setCacheMode(CacheBackground);
     setViewportUpdateMode(BoundingRectViewportUpdate);
     setRenderHint(QPainter::Antialiasing);
     setTransformationAnchor(AnchorUnderMouse);
     scale(qreal(0.8), qreal(0.8));
     setMinimumSize(400, 400);
     setWindowTitle(tr("Heat exchanger network"));

 }

 void EIHENView::itemMoved()
 {

 }

 void EIHENView::keyPressEvent(QKeyEvent *event)
 {
     switch (event->key()) {
     case Qt::Key_Up:
         translate(0, -20);
         break;
     case Qt::Key_Down:
         translate(0, 20);
         break;
     case Qt::Key_Left:
         translate(-20, 0);
         break;
     case Qt::Key_Right:
         translate(20, 0);
         break;
     case Qt::Key_Plus:
         scaleView(qreal(1.2));
         break;
     case Qt::Key_Minus:
         scaleView(1 / qreal(1.2));
         break;
     case Qt::Key_Space:
     case Qt::Key_Enter:
         foreach (QGraphicsItem *item, scene()->items()) {
             if (qgraphicsitem_cast<EIHENGraphicNode *>(item))
                 item->setPos(-150 + qrand() % 300, -150 + qrand() % 300);
         }
         break;
     default:
         QGraphicsView::keyPressEvent(event);
     }
 }

// void GraphWidget::timerEvent(QTimerEvent *event)
// {
//     Q_UNUSED(event);

//     QList<Node *> nodes;
//     foreach (QGraphicsItem *item, scene()->items()) {
//         if (Node *node = qgraphicsitem_cast<Node *>(item))
//             nodes << node;
//     }

//     foreach (Node *node, nodes)
//         node->calculateForces();

//     bool itemsMoved = false;
//     foreach (Node *node, nodes) {
//         if (node->advance())
//             itemsMoved = true;
//     }

//     if (!itemsMoved) {
//         killTimer(timerId);
//         timerId = 0;
//     }
// }

 void EIHENView::wheelEvent(QWheelEvent *event)
 {
     scaleView(pow((double)2, event->delta() / 240.0));
 }

// void EIHENView::drawBackground(QPainter *painter, const QRectF &rect)
// {
//     Q_UNUSED(rect);

//     // Shadow
//     QRectF sceneRect = this->sceneRect();
//     QRectF rightShadow(sceneRect.right(), sceneRect.top() + 5, 5, sceneRect.height());
//     QRectF bottomShadow(sceneRect.left() + 5, sceneRect.bottom(), sceneRect.width(), 5);
//     if (rightShadow.intersects(rect) || rightShadow.contains(rect))
//         painter->fillRect(rightShadow, Qt::darkGray);
//     if (bottomShadow.intersects(rect) || bottomShadow.contains(rect))
//         painter->fillRect(bottomShadow, Qt::darkGray);

//     // Fill
//     QLinearGradient gradient(sceneRect.topLeft(), sceneRect.bottomRight());
//     gradient.setColorAt(0, Qt::white);
//     gradient.setColorAt(1, Qt::lightGray);
//     painter->fillRect(rect.intersect(sceneRect), gradient);
//     painter->setBrush(Qt::NoBrush);
//     painter->drawRect(sceneRect);

//     // Text
//     QRectF textRect(sceneRect.left() + 4, sceneRect.top() + 4,
//                     sceneRect.width() - 4, sceneRect.height() - 4);
//     QString message(tr("Click and drag the nodes around, and zoom with the mouse "
//                        "wheel or the '+' and '-' keys"));

//     QFont font = painter->font();
//     font.setBold(true);
//     font.setPointSize(14);
//     painter->setFont(font);
//     painter->setPen(Qt::lightGray);
//     painter->drawText(textRect.translated(2, 2), message);
//     painter->setPen(Qt::black);
//     painter->drawText(textRect, message);
// }

 void EIHENView::scaleView(qreal scaleFactor)
 {
     qreal factor = transform().scale(scaleFactor, scaleFactor).mapRect(QRectF(0, 0, 1, 1)).width();
     if (factor < 0.07 || factor > 100)
         return;

     scale(scaleFactor, scaleFactor);
 }
