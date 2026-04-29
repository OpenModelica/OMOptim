/*
 * This file is part of OpenModelica.
 *
 * Copyright (c) 1998-2026, Open Source Modelica Consortium (OSMC),
 * c/o Linköpings universitet, Department of Computer and Information Science,
 * SE-58183 Linköping, Sweden.
 *
 * All rights reserved.
 *
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF AGPL VERSION 3 LICENSE OR
 * THIS OSMC PUBLIC LICENSE (OSMC-PL) VERSION 1.8.
 * ANY USE, REPRODUCTION OR DISTRIBUTION OF THIS PROGRAM CONSTITUTES
 * RECIPIENT'S ACCEPTANCE OF THE OSMC PUBLIC LICENSE OR THE GNU AGPL
 * VERSION 3, ACCORDING TO RECIPIENTS CHOICE.
 *
 * The OpenModelica software and the OSMC (Open Source Modelica Consortium)
 * Public License (OSMC-PL) are obtained from OSMC, either from the above
 * address, from the URLs:
 * http://www.openmodelica.org or
 * https://github.com/OpenModelica/ or
 * http://www.ida.liu.se/projects/OpenModelica,
 * and in the OpenModelica distribution.
 *
 * GNU AGPL version 3 is obtained from:
 * https://www.gnu.org/licenses/licenses.html#GPL
 *
 * This program is distributed WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE, EXCEPT AS EXPRESSLY SET FORTH
 * IN THE BY RECIPIENT SELECTED SUBSIDIARY LICENSE CONDITIONS OF OSMC-PL.
 *
 * See the full OSMC Public License conditions for more details.
 *
 */

#ifndef __QScienceSpinBox_H__
#define __QScienceSpinBox_H__

// QT Headers
#include <QtGlobal>
#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QtWidgets>
#else
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QDoubleValidator>
#include <QtGui/QLineEdit>
#include <QtCore/QVariant>
#include <QtCore/QDebug>
#include <QtCore/QString>
#endif

class QScienceSpinBox : public QDoubleSpinBox
{
    Q_OBJECT
public:
    QScienceSpinBox(QWidget * parent = 0);

    int decimals() const;
    void setDecimals(int value);

    QString textFromValue ( double value ) const;
    double valueFromText ( const QString & text ) const;

    static bool isIntermediateValueHelper(qint64 num, qint64 minimum, qint64 maximum, qint64 *match = 0);


private:
    int dispDecimals;
    QChar delimiter, thousand;
    QDoubleValidator * v;


private:
    void initLocalValues(QWidget *parent);
    bool isIntermediateValue(const QString &str) const;
    QVariant validateAndInterpret(QString &input, int &pos, QValidator::State &state) const;
    QValidator::State validate(QString &text, int &pos) const;
    void fixup(QString &input) const;
    QString stripped(const QString &t, int *pos) const;
    double round(double value) const;
    void stepBy(int steps);

public slots:
    void stepDown();
    void stepUp();

};


#endif
