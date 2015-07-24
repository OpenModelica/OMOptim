// $Id$
/**
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-CurrentYear, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF GPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL).
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE
 * OF THE OSMC PUBLIC LICENSE OR THE GPL VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the Open Source Modelica
 * Consortium (OSMC) Public License (OSMC-PL) are obtained
 * from OSMC, either from the above address,
 * from the URLs: http://www.ida.liu.se/projects/OpenModelica or
 * http://www.openmodelica.org, and in the OpenModelica distribution.
 * GNU version 3 is obtained from: http://www.gnu.org/copyleft/gpl.html.
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of  MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

     @file InfoSender.h
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#if !defined(_INFOSENDER_H)
#define _INFOSENDER_H

#include "Info.h"

#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#include <QTextStream>
#else
#include <QTextStream>
#include <QTime>
#include <QAbstractItemModel>
#include <QStringList>
#endif

class Infos : public QAbstractTableModel
{

public:

    enum type
    {
        INFO,
        INFOWARNING,
        INFOERROR
    };

    enum destination
    {
        DESTOMC,
        DESTNORMAL,
        DESTDEBUG
    };

    Infos(QObject* parent);
    ~Infos();

    void addInfo(const QString & info,const type & infoType,const destination & infoDestination);
    void clear();


    int _maximumInfoLines;
    int _linesToRemove;


protected:
    QStringList _text;
    QList<type> _type; // normal,
    QList<int> _dest; // Destination tab: OMC, NORMAL, DEBUG

    QVariant data(const QModelIndex &index, int role) const;

    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const;
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;
    bool removeRows ( int row, int count, const QModelIndex & parent = QModelIndex() );
     void removeFirstNormalInfos(int count);


};


/** InfoSender is used to send information to GUI or to console.
It contains a single instance shared within entire project*/

class InfoSender : public QObject
{
    Q_OBJECT


public:
    static InfoSender* instance();
    static void destroy();

    void setLogStream(QTextStream* logStream,QList<ListInfo::InfoType> types = QList<ListInfo::InfoType>());
    void addLogStream(QTextStream* logStream,QList<ListInfo::InfoType> types = QList<ListInfo::InfoType>());
    ~InfoSender(void);

    static void eraseCurrentTask();
    static void sendCurrentTask(QString msg);

public slots :
    void send(const Info &);
    void debug(QString msg){send(Info(msg,ListInfo::INFODEBUG));}
    void sendWarning(QString msg){send(Info(msg,ListInfo::WARNING2));}
    void sendError(QString msg){send(Info(msg,ListInfo::ERROR2));}
    void sendNormal(QString msg){send(Info(msg,ListInfo::NORMAL2));}


    // try to go through signal/slots in order to avoid multi-threading problems
    void onReceivedInfo(const Info&);


    Infos* infosNormal(){return _infosNormal;}
    Infos* infosOM(){return _infosOM;}
    Infos* infosDebug(){return _infosDebug;}
    Infos* infosScript(){return _infosScript;}

signals :
    void receivedInfo(const Info&);
    void sent(const Info &);
    void setCurrentTask(QString);
    void increaseTaskProgress();
    void noCurrentTask();

private :
    QMultiMap<ListInfo::InfoType,QTextStream*> _logStreams;
    //QTextStream* _logStream;
    InfoSender();
    static InfoSender* _instance;
    Infos* _infosNormal;
    Infos* _infosOM;
    Infos* _infosDebug;
    Infos* _infosScript;

    QList<ListInfo::InfoType> _logStreamInfoTypes;
};



#endif
