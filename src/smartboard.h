// smartboard.h

#ifndef SMARTBOARD_H
#define SMARTBOARD_H

#include <QMainWindow>
#include <QModelIndex>
#include <QTreeWidgetItem>
#include <QDir>
#include <QFileDialog>
#include <QListWidget>
#include <QTextEdit>
#include <QGridLayout>

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

    ViewBoard *m_viewBoard;
    Sticker *m_sticker;
    FindDialog *m_findDialog;

    QFont m_font;

    // graphics view
    QGraphicsView *m_graphicsView;

    // triples list
    QTreeWidget *m_triplesView;

    // options frame
    QFrame *m_optionsFrame;
    QListWidget *m_sibList;
    QLabel *m_optionsLbl, *m_sibLbl, *m_sibStatus, *m_sibStatusLbl;
    QPushButton *m_backOptions;

    // publish frame
    QFrame *m_publishFrame;
    QLabel *m_publishLbl, *m_authorLbl, *m_bulletinLbl, *m_themeLbl, *m_publishStatusLbl;
    QLineEdit *m_authorTxt, *m_themeTxt;
    QTextEdit *m_bulletinTxt;
    QPushButton *m_publishBtn, *m_backPublish;

    QGridLayout *m_gridLayout;
    QVBoxLayout *m_publishVboxLayout, *m_optionsVboxLayout;
    QHBoxLayout *m_publishHboxLayout, *m_optionsHboxLayout;

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
    void createForms();
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

    void sibList_doubleClicked();
    void backOptions_clicked();

    void publishBtn_clicked();
    void backPublish_clicked();

signals:
    void getSibs();

};

#endif // SMARTBOARD_H
