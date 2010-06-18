/*
 * Copyright (C) 2010 Robin Burchell <robin.burchell@collabora.co.uk>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU Lesser General Public License,
 * version 2.1, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <QFontMetrics>
#include <QPainter>

#include "qtfastlabel.h"

QtFastLabel::QtFastLabel(QWidget *parent) :
    QWidget(parent)
{
}

void QtFastLabel::setText(QString text)
{
    text.replace('\n', QChar::LineSeparator);
    m_layout.setText(text);

    QFontMetrics fontMetrics = QFontMetrics(QFont());
    int leading = fontMetrics.leading();
    qreal height = 0;
    m_layout.beginLayout();
    while (1) {
        QTextLine line = m_layout.createLine();
        if (!line.isValid())
            break;

        line.setLineWidth(width());
        height += leading;
        line.setPosition(QPointF(0, height));
        height += line.height();
    }
    m_layout.endLayout();

    updateGeometry();
    update();
}

void QtFastLabel::resizeEvent()
{
    setText(m_layout.text());
}

QSize QtFastLabel::sizeHint() const
{
    return m_layout.boundingRect().size().toSize();
}

void QtFastLabel::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    m_layout.draw(&p, QPointF(0, 0));
}
