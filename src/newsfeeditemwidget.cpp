#include <QDebug>

#include "newsfeedpost.h"
#include "facebookaccount.h"
#include "newsfeedmodel.h"
#include "newsfeeditemwidget.h"
#include "ui_newsfeeditemwidget.h"

NewsFeedItemWidget::NewsFeedItemWidget(QAbstractItemModel *model, QModelIndex index) :
    QtModelWidget(model, index),
    m_ui(new Ui::NewsFeedItemWidget)
{
    m_ui->setupUi(this);
}

NewsFeedItemWidget::~NewsFeedItemWidget()
{
    delete m_ui;
}

void NewsFeedItemWidget::dataChanged()
{
    NewsFeedPost *np = static_cast<NewsFeedPost *>(model()->data(index(), NewsFeedModel::PostRole).value<void *>());
    m_ui->userName->setText(np->author()->name());
    m_ui->date->setText(np->timeAsString());
    m_ui->avatar->setPixmap(np->author()->avatar());
    m_ui->post->setText(np->message());

    if (np->hasAttachment()) {
        m_ui->attachmentContainer->show();

        if (np->attachmentName().length() == 0) {
            m_ui->attachmentName->hide();
        } else {
            m_ui->attachmentName->setText(np->attachmentName());
        }

        m_ui->attachmentDescription->setText(np->description());
        m_ui->attachment->setPixmap(np->thumbnail());
    } else {
        m_ui->attachmentContainer->hide();
    }

    updateGeometry();
    update(contentsRect());
}
