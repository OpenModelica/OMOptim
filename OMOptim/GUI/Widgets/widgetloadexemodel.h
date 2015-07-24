#ifndef WIDGETLOADEXEMODEL_H
#define WIDGETLOADEXEMODEL_H

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QDialog>
#include <QModelIndex>
#include <QDirModel>
#include <QFileInfo>
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#endif

#include "Project.h"

namespace Ui {
class WidgetLoadExeModel;
}

class WidgetLoadExeModel : public QDialog
{
    Q_OBJECT

public:
    explicit WidgetLoadExeModel(Project *project, QWidget *parent);
    ~WidgetLoadExeModel();
    QString name();
    QFileInfo exeFileInfo();
    QFileInfo inputFileInfo();


private:
    Ui::WidgetLoadExeModel *ui;
    Project* _project;
    //QList<ModPlusCtrl*> _ctrls;

public slots:
    void openExe();
    void openInput();
    void validate();


};

#endif // WIDGETLOADEXEMODEL_H
