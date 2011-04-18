// smartboard.h

#ifndef SMARTBOARD_H
#define SMARTBOARD_H

#include <QMainWindow>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include <QDir>
#include <QFileDialog>

#include "q_whiteboard_node.h"
#include "q_sib_info.h"
#include "triple.h"

#include "finddialog.h"
#include "viewboard.h"
#include "sticker.h"


class TemplateQuery;
class TemplateSubscription;

namespace Ui {
    class SmartBoard;
}

class SmartBoard : public QMainWindow
{
    Q_OBJECT

public:
    explicit SmartBoard(QWidget *parent = 0);
    ~SmartBoard();

private:
    Ui::SmartBoard *ui;

    QString m_sibUri;
    QHash<QString, QWhiteBoardNode *> m_nodes;
    QList<Triple *> m_graphList;

    QHash<QString, TemplateSubscription *> m_subscriptions;
    QString m_activeSubscription;

    QDir m_directory;
    QFileDialog m_fileDialog;

    FindDialog *m_findDialog;

    ViewBoard *m_viewBoard;
    Sticker *m_sticker;


public slots:
    void joinComplete(int success);
    void sibList(const QList<QSibInfo *> &siblist);
    void sibInserted(const QSibInfo *sib);
    void sibRemoved(QString uri);
    void joinSelectedSib();
    void leaveSelectedSib();

    void sibSubscribe(Triple *triple);
    void sibUnsubscribe();

    void subscriptionFinished();
    void subscriptionIndication();

    void findData(const QString &str, Qt::CaseSensitivity cs);

private slots:
    void createActions();
    void createToolBars();
    void boardOptions();
    void publishAnnouncements();
    void searchAnnouncements();
    void subscribeBoard();
    void showViewBoard();
    void boardHelp();
    void boardAbout();

    bool addNode(QString sib);

    void fullBulletinInfo(QTreeWidgetItem *item);
    void enableMenuItem(bool option);
    QString getSelectedItemUri();

    void on_m_sibList_doubleClicked();
    void on_m_backOptions_clicked();

    void on_m_searchBtn_clicked();

    void on_m_publishBtn_clicked();
    void on_m_backPublish_clicked();

signals:
    void getSibs();

};

#endif // SMARTBOARD_H
