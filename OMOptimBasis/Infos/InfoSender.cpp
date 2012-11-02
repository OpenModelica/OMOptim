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
 * Main contributor 2010, Hubert Thierot, CEP - ARMINES (France)

     @file InfoSender.cpp
     @brief Comments for file documentation.
     @author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
     Company : CEP - ARMINES (France)
     http://www-cep.ensmp.fr/english/
     @version

  */
#include "InfoSender.h"
#include "MOThreads.h"
#include <QApplication>

// Global static pointer used to ensure a single instance of the class.
InfoSender* InfoSender::_instance = NULL;

InfoSender::InfoSender()
{
    _logStream = NULL;
    _infosNormal = new Infos(this);
    _infosOM = new Infos(this);
    _infosDebug = new Infos(this);

    connect(this, SIGNAL(receivedInfo(const Info&)),this, SLOT(onReceivedInfo(const Info&)));
}

void InfoSender::setLogStream(QTextStream* logStream)
{
    _logStream = logStream;
}


InfoSender::~InfoSender(void)
{
    delete _infosNormal;
    delete _infosOM;
    delete _infosDebug;
}

InfoSender* InfoSender::instance()
{
    // instance pointer is stored in qapp properties : allows to share with plugins
    // Otherwise, plugins create a new instance
    // A correct way would be to build an OMOptim shared lib, that exe and plugin would share
    if(!_instance)
    {
        if(qApp->property("InfoSender").isValid())
        {
            _instance = dynamic_cast<InfoSender*>(qApp->property("InfoSender").value<QObject*>());
        }
        else
        {
            _instance = new InfoSender();
            qApp->setProperty("InfoSender",qVariantFromValue(qobject_cast<QObject*>(_instance)));
        }
    }
    return _instance;
}

void InfoSender::destroy()
{
    if(_instance)
    {
        delete _instance;
        _instance = NULL;
        qApp->setProperty("InfoSender",QVariant());
    }
}

void InfoSender::send(const Info &info)
{

    if(_logStream)
    {
        *_logStream << QTime::currentTime().toString().toAscii().data();
        *_logStream << "\t";
        *_logStream << info.infoMsg;
        *_logStream << "\n";
    }
    if(info.infoType==ListInfo::INFODEBUG)
        qDebug(info.infoMsg.toLatin1().data());


    emit receivedInfo(info);
}

void InfoSender::onReceivedInfo(const Info &info)
{
    switch(info.infoType)
    {
    case ListInfo::OMCERROR2:
        _infosOM->addInfo(info.infoMsg,Infos::INFOERROR,Infos::DESTOMC);
        break;
    case ListInfo::OMCNORMAL2:
        _infosOM->addInfo(info.infoMsg,Infos::INFO,Infos::DESTOMC);
        break;
    case ListInfo::OMCWARNING2:
        _infosOM->addInfo(info.infoMsg,Infos::INFOWARNING,Infos::DESTOMC);
        break;
    case ListInfo::NORMAL2:
    case ListInfo::TASK:
        if(!info.infoMsg.isEmpty())
            _infosNormal->addInfo(info.infoMsg,Infos::INFO,Infos::DESTNORMAL);
        break;
    case ListInfo::WARNING2:
        _infosNormal->addInfo(info.infoMsg,Infos::INFOWARNING,Infos::DESTNORMAL);
        break;
    case ListInfo::ERROR2:
        _infosNormal->addInfo(info.infoMsg,Infos::INFOERROR,Infos::DESTNORMAL);
        break;
    case ListInfo::INFODEBUG:
        _infosDebug->addInfo(info.infoMsg,Infos::INFO,Infos::DESTDEBUG);
        break;
    }

    emit sent(info);
}

void InfoSender::sendCurrentTask(QString msg)
{
    instance()->send(Info(msg,ListInfo::TASK));
}

void InfoSender::eraseCurrentTask()
{
    instance()->sendCurrentTask(QString());
}

Infos::Infos(QObject* parent)
    :QAbstractTableModel(parent)
{
    _maximumLines = 500;
    _linesToRemove = _maximumLines/10;
}

Infos::~Infos()
{
    clear();
}

void Infos::addInfo(const QString &info, const type &infoType, const destination &infoDestination)
{
    if(info.isEmpty())
        return;

    // if reached full size
    if(_text.size()>= _maximumLines)
    {
        removeRows(0,_linesToRemove,QModelIndex());
    }

    // qDebug("Started adding Info");
    // add info
    beginInsertRows(QModelIndex(),_text.size(),_text.size());
    _text.push_back(info);
    _type.push_back(infoType);
    _dest.push_back(infoDestination);
    endInsertRows();
    // qDebug("Ended adding Info");
}

int Infos::rowCount(const QModelIndex &parent) const
{
    //return 0;
    return _text.size();
}

bool Infos::removeRows(int position, int rows, const QModelIndex &index)
{
    Q_UNUSED(index);
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) {
        _text.removeAt(position);
        _type.removeAt(position);
        _dest.removeAt(position);
    }

    endRemoveRows();
    return true;
}

Qt::ItemFlags Infos::flags(const QModelIndex &index) const
{
    return  Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int Infos::columnCount(const QModelIndex &parent) const
{
    return 2;
    // fist column : type
    // second column : text
}

void Infos::clear()
{
    beginResetModel();
    _text.clear();
    _type.clear();
    _dest.clear();
    endResetModel();
}

QVariant Infos::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if(role!=Qt::DisplayRole)
        return QVariant();

    if (index.row() >= _text.size())
        return QVariant();

    // type
    if(index.column()==0)
    {
        // type
        //        if(role == Qt::UserRole)
        //            return _type.at(index.row());
        if(role == Qt::DisplayRole)
        {
            switch (_type.at(index.row()))
            {
            case INFOERROR :
                return "Error :";
            case INFOWARNING:
                return "Warning :";
            case INFO:
                return "Info :";
            }
        }
    }

    // text
    if((index.column()==1) && (role==Qt::DisplayRole))
        return _text.at(index.row());

    return QVariant();
}

QVariant Infos::headerData(int section, Qt::Orientation orientation,
                           int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
        return QString("Column %1").arg(section);
    else
        return QString("Row %1").arg(section);
}


