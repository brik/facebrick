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

#ifndef NEWSFEEDPOSTVIEW_H
#define NEWSFEEDPOSTVIEW_H

#include <QDialog>

namespace Ui {
    class NewsFeedPostView;
}

class NewsFeedPost;
class FBSession;

class NewsFeedPostView : public QDialog {
    Q_OBJECT
public:
    explicit NewsFeedPostView(QWidget *parent, FBSession *session);
    ~NewsFeedPostView();

    void setPost(const NewsFeedPost * const post);

protected:
    void changeEvent(QEvent *e);
private slots:
    void setupUi();
private:
    Ui::NewsFeedPostView *m_ui;
    const NewsFeedPost * m_post;
    const FBSession * const m_session;
};

#endif // NEWSFEEDPOSTVIEW_H
