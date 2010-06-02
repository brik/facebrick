/*
 * Copyright (C) 2010 Kamilla Bremeraunet <kamilla@bremeraunet.com>
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

#include <QObject>
#include <QWidget>

class KeyHandler : public QObject
{
Q_OBJECT
public:
    static KeyHandler *instance();
    void grabKeyEvents(WId);
    void emitIncreaseSignal();
    void emitDecreaseSignal();
    int fontSizeDifference();
signals:
    void decreaseKeyPressed();
    void increaseKeyPressed();
private:
    KeyHandler();
    int m_fontDifference;
};
