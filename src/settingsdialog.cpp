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

#include <QDebug>
#include <QtGui>
#include <qapplication.h>

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "newsfeedmodel.h"

SettingsDialog *SettingsDialog::instance()
{
    static SettingsDialog *dialog = 0;

    if (!dialog)
        dialog = new SettingsDialog(0);

    return dialog;
}

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

    QSettings settings("FaceBrick", "FaceBrick");
    settings.beginGroup("settings");
    int fontSize = settings.value("fontSize").toInt();
    int updateInterval = settings.value("updateInterval").toInt();
    settings.endGroup();
    m_ui->fontSizeSlider->setValue(fontSize);
    m_ui->updateIntervalSlider->setValue(updateInterval);

    updateIntervalLabel();

    connect(this, SIGNAL(accepted()), this, SLOT(onDoneButtonClicked()));
    //connect(this, SIGNAL(updateIntervalChanged()), MainWindow::instance(), SLOT(updateInterval()));
    connect(m_ui->updateIntervalSlider, SIGNAL(valueChanged(int)), SLOT(updateIntervalLabel()));
}

SettingsDialog::~SettingsDialog()
{
    delete m_ui;
}

void SettingsDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        m_ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void SettingsDialog::updateIntervalLabel()
{
    if (m_ui->updateIntervalSlider->value() == 0)
        m_ui->intervalLabel->setText("off");
    else
        m_ui->intervalLabel->setText(QString::number(m_ui->updateIntervalSlider->value())+ " min");
}

void SettingsDialog::onDoneButtonClicked()
{
    int fontSize = m_ui->fontSizeSlider->value();
    int updateInterval = m_ui->updateIntervalSlider->value();

    /* Save the new font size difference in settings */
    QSettings settings("FaceBrick", "FaceBrick");
    settings.beginGroup("settings");
    settings.setValue("fontSize", fontSize);
    settings.setValue("updateInterval", updateInterval);
    settings.endGroup();

    for (int i = 0; i < NewsFeedModel::getNewsFeedModelList().size(); i++)
        NewsFeedModel::getNewsFeedModelList()[i]->fontSizeChanged();

    emit updateIntervalChanged();
}
