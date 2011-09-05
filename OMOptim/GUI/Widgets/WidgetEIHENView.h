#ifndef WIDGETEIHENVIEW_H
#define WIDGETEIHENVIEW_H

#include <QtGui/QGraphicsView>
#include <QWidget>

#include "EIHENView.h"
#include "EIHENScene.h"


namespace Ui {
    class WidgetEIHENViewClass;
}

class QErrorMessage;


class WidgetEIHENView : public QWidget
{
    Q_OBJECT

public :
    explicit WidgetEIHENView(const EIHEN &,QWidget* parent);
    virtual ~WidgetEIHENView();
    Ui::WidgetEIHENViewClass *_ui;


protected :
    EIHENView* _eiHENView;
    EIHENScene* _eiHENScene;
    EIHEN _eiHEN;


public slots:
        void onSpecifyAsked();
};



#endif // WIDGETEIHENVIEW_H
