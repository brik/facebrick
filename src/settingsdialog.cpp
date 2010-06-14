#include <QDebug>
#include <QtGui>
#include <qapplication.h>

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "newsfeedmodel.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    m_ui(new Ui::SettingsDialog)
{
    m_ui->setupUi(this);

    QSettings settings("FaceBrick", "FaceBrick");
    settings.beginGroup("settings");
    int fontSize = settings.value("fontSize", fontSize).toInt();
    settings.endGroup();
    m_ui->fontSizeSlider->setValue(fontSize);;
    connect(this, SIGNAL(accepted()), this, SLOT(onDoneButtonClicked()));
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

void SettingsDialog::onDoneButtonClicked()
{
    int fontSize = m_ui->fontSizeSlider->value();

    /* Save the new font size difference in settings */
    QSettings settings("FaceBrick", "FaceBrick");
    settings.beginGroup("settings");
    settings.setValue("fontSize", fontSize);
    settings.endGroup();

    for (int i = 0; i < NewsFeedModel::getNewsFeedModelList().size(); i++)
    {
        NewsFeedModel::getNewsFeedModelList()[i]->fontSizeChanged();
    }
}
