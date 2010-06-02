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

#include "keyhandler.h"

#include <QDebug>
#include <QAbstractEventDispatcher>
#include <QKeySequence>
#include <QtGui>
#include <QX11Info>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>

bool myEventFilter(void *message)
{
    XEvent *event = static_cast<XEvent *>(message);
    if (!event)
        return false;

    if (event->type == KeyPress) {
        if (event->xkey.keycode == QKeySequence::ZoomOut) {
            KeyHandler::instance()->emitDecreaseSignal();
            return true;
        }
        else if (event->xkey.keycode == QKeySequence::ZoomIn) {
            KeyHandler::instance()->emitIncreaseSignal();
            return true;
        }
    }
    return false;
}

KeyHandler *KeyHandler::instance()
{
    static KeyHandler *v = 0;

    if (!v)
        v = new KeyHandler();

    return v;
}

KeyHandler::KeyHandler() : QObject(0)
{
    QAbstractEventDispatcher::instance()->setEventFilter(myEventFilter);
    m_fontDifference = 0;
}

void KeyHandler::grabKeyEvents(WId windowId)
{
    // Just returning here as we need a settings dialogue for this first.
    return;
    // Tell maemo-status-volume to gran/ungrab increase/decrease keys
    unsigned long val = (true) ? 1 : 0;

    Atom atom;
    atom = XInternAtom(QX11Info::display(), "_HILDON_ZOOM_KEY_ATOM", 0);

    qDebug() << XChangeProperty(QX11Info::display(),
                    windowId,
                    atom,
                    XA_INTEGER,
                    32,
                    PropModeReplace,
                    (unsigned char *)  &val,
                    1);
}

void KeyHandler::emitIncreaseSignal()
{
    m_fontDifference++;
    emit increaseKeyPressed();
}

void KeyHandler::emitDecreaseSignal()
{
    if (m_fontDifference > -10)
        m_fontDifference--;

    emit decreaseKeyPressed();
}

int KeyHandler::fontSizeDifference()
{
    return m_fontDifference;
}
