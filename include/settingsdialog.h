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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    static SettingsDialog *instance();
    ~SettingsDialog();
signals:
    void updateIntervalChanged();
private slots:
    void onDoneButtonClicked();
    void updateIntervalLabel();

protected:
    void changeEvent(QEvent *e);

private:
    explicit SettingsDialog(QWidget *parent = 0);
    Ui::SettingsDialog *m_ui;
};

#endif // SETTINGSDIALOG_H
