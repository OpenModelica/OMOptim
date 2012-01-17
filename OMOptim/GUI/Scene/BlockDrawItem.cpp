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

 	@file BlockDrawItem.cpp
 	@brief Comments for file documentation.
 	@author Hubert Thieriot, hubert.thieriot@mines-paristech.fr
 	Company : CEP - ARMINES (France)
 	http://www-cep.ensmp.fr/english/
 	@version 

*/

#include "BlockDrawItem.h"

BlockDrawItem::BlockDrawItem(ModItem* _comp,int _type) : QWidget()
{
	type = _type;
	component = _comp;

	build();

	label->setText(_comp->name(Modelica::SHORT));


	////rectangle
	//rectangle = new DiagramItem(DiagramItem::Rectangle,0,this);
	//rectangle->setBrush(QBrush(Qt::white,Qt::SolidPattern));
	//rectangle->setPen(QPen(Qt::black, 4));
	//rectangle->setHeight(40);

	//setGeometry(rectangle->boundingRect());
	//setMinimumSize(rectangle->boundingRect().size());
	//
	//// add text
	//text = new QGraphicsTextItem(_comp->name(ModItem::SHORT),this);
	//text->setPos(pos());
	//text->setZValue(zValue()+1);

	// resize rectangle
	/*QRectF _textRect = text->boundingRect();
	QRectF _rectRect = rectangle->boundingRect();


	if(_textRect.width()>_rectRect.width())
	{
	rectangle->setWidth(_textRect.width());
	setMinimumSize(rectangle->boundingRect().size());
	}*/


}

void BlockDrawItem::build()
{
	resize(100,40);

	
	QFont font;
	font.setStyleStrategy(QFont::PreferAntialias);
	setFont(font);
	blockLayout = new QHBoxLayout(this);
	setLayout(blockLayout);
	blockLayout->setMargin(0);
	
	label = new QLabel();
	label->setFont(font);
	label->setFrameShape(QFrame::StyledPanel);
	label->setFrameShadow(QFrame::Plain);
	label->setLineWidth(3);
	label->setScaledContents(false);
	label->setAlignment(Qt::AlignCenter);
	label->setMargin(5);


	blockLayout->addWidget(label);
}
BlockDrawItem::~BlockDrawItem(void)
{
	
	//delete text;
}

//void BlockDrawItem::setPos(QPointF &_pos)
//{
//	((DiagramItem*)this)->setPos(_pos);
//	text->setPos(_pos);
//}
//
//void BlockDrawItem::setPos(QPoint &_pos)
//{
//	rectangle->setPos(_pos);
//	text->setPos(_pos);
//}

void BlockDrawItem::activate()
{
	QColor myLightGreen("#73d216");

	QPalette pal = label->palette();
	pal.setColor(label->backgroundRole(), myLightGreen);
	label->setPalette(pal);
	label->setAutoFillBackground(true);
	label->show();

	//rectangle->setBrush(QBrush(myLightGreen));
}

void BlockDrawItem::desactivate()
{
	QPalette pal = label->palette();
	pal.setColor(label->backgroundRole(), Qt::white);
	label->setPalette(pal);
	label->setAutoFillBackground(true);
	label->show();

	//rectangle->setBrush(QBrush(Qt::white,Qt::SolidPattern));
}
