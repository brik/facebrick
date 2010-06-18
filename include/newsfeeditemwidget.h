#ifndef NEWSFEEDITEMWIDGET_H
#define NEWSFEEDITEMWIDGET_H

#include "qtmodelwidget.h"

namespace Ui {
    class NewsFeedItemWidget;
}

class NewsFeedItemWidget : public QtModelWidget
{
    Q_OBJECT

public:
    Q_INVOKABLE
    explicit NewsFeedItemWidget(QAbstractItemModel *model, QModelIndex index);
    ~NewsFeedItemWidget();
private:
    void dataChanged();

private:
    Ui::NewsFeedItemWidget *m_ui;
};

#endif // NEWSFEEDITEMWIDGET_H
