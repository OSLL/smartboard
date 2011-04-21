// smartboard.cpp

#include <QApplication>
#include <QtGui>
#include <QModelIndex>
#include <QGraphicsItem>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>

#include "smartboard.h"
#include "ui_smartboard.h"

#include "q_whiteboard_node.h"
#include "q_sib_info.h"
#include "q_sib_discovery.h"
#include "triple.h"
#include "templatequery.h"
#include "templatesubscription.h"

#include "finddialog.h"
#include "viewboard.h"

SmartBoard::SmartBoard(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::SmartBoard),
    m_directory("/home"), m_fileDialog(this),
    m_findDialog(0), m_sticker(new Sticker(":/images/sticker.svg"))
{
    ui->setupUi(this);

    createActions();
    createToolBars();

    ui->menuBar->hide();
    ui->m_optionsFrame->hide();
    ui->m_publishFrame->hide();
    ui->mainToolBar->setMovable(false);
    ui->subscribeBoardAction->setEnabled(false);
    ui->publishAction->setEnabled(false);
    ui->searchAction->setEnabled(false);
    ui->unsubscribeAction->setEnabled(false);
    ui->viewAction->setEnabled(false);
    ui->m_sibConnectGrp->hide();

    ui->m_triplesView->setColumnWidth(0,70);
    ui->m_triplesView->setColumnWidth(1,90);
    ui->m_triplesView->setColumnWidth(2,185);
    ui->m_triplesView->setColumnWidth(3,10);
    QStringList headers;
    headers << tr("Author") << tr("Theme") << tr("Message") << tr("View") ;
    ui->m_triplesView->setHeaderLabels(headers);
    ui->m_sibStatus->setStyleSheet("QLabel { color : red; }");

    m_viewBoard = new ViewBoard;
    //ui->m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //ui->m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    ui->m_graphicsView->setRenderHint(QPainter::Antialiasing, true);
    ui->m_graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    ui->m_graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    ui->m_graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter |
                                             QGraphicsView::DontSavePainterState |
                                             QGraphicsView::DontAdjustForAntialiasing);
    ui->m_graphicsView->setMaximumSize(401, 441);
    ui->m_graphicsView->setScene(m_viewBoard);
    ui->m_graphicsView->hide();

    setWindowIcon(QIcon(":/images/icon.png"));
}

SmartBoard::~SmartBoard()
{
    QHash<QString, QWhiteBoardNode *>::iterator it= m_nodes.begin();
    while(it != m_nodes.end())
    {
        QWhiteBoardNode *node = m_nodes.take(it.key());
        node->deleteLater();
        it++;
    }

    delete ui;
}

// create menu actions
void SmartBoard::createActions()
{
    ui->optionsAction->setIcon(QIcon(":/images/options.png"));
    ui->optionsAction->setShortcut(tr("Ctrl+O"));
    ui->optionsAction->setStatusTip(tr("SmartBoard options"));
    connect(ui->optionsAction, SIGNAL(triggered()), this, SLOT(boardOptions()));

    ui->exitAction->setIcon(QIcon(":/images/exit.png"));
    ui->exitAction->setShortcut(tr("Ctrl+Q"));
    ui->exitAction->setStatusTip(tr("Exit program"));
    connect(ui->exitAction, SIGNAL(triggered()), this, SLOT(close()));

    ui->publishAction->setIcon(QIcon(":/images/publish.png"));
    ui->publishAction->setShortcut(tr("Ctrl+P"));
    ui->publishAction->setStatusTip(tr("Publish announsment"));
    connect(ui->publishAction, SIGNAL(triggered()), this, SLOT(publishAnnouncements()));

    ui->searchAction->setIcon(QIcon(":/images/search.png"));
    ui->searchAction->setShortcut(tr("Ctrl+F"));
    ui->searchAction->setStatusTip(tr("Search information"));
    connect(ui->searchAction, SIGNAL(triggered()), this, SLOT(searchAnnouncements()));

    ui->subscribeBoardAction->setIcon(QIcon(":/images/subscribe.png"));
    ui->subscribeBoardAction->setShortcut(tr("Ctrl+S"));
    ui->subscribeBoardAction->setStatusTip(tr("Subscribe to SmartBoard"));
    connect(ui->subscribeBoardAction, SIGNAL(triggered()), this, SLOT(subscribeBoard()));

    ui->unsubscribeAction->setIcon(QIcon(":/images/unsubscribe.png"));
    ui->unsubscribeAction->setShortcut(tr("Ctrl+U"));
    ui->unsubscribeAction->setStatusTip(tr("Unsibscribe from SmartBoard"));

    ui->viewAction->setIcon(QIcon(":/images/view.png"));
    ui->viewAction->setShortcut(tr("Ctrl+V"));
    ui->viewAction->setStatusTip(tr("SmartBoard view"));
    connect(ui->viewAction, SIGNAL(triggered()), this, SLOT(showViewBoard()));

    ui->helpAction->setIcon(QIcon(":/images/help.png"));
    ui->helpAction->setShortcut(tr("Ctrl+H"));
    ui->helpAction->setStatusTip(tr("SmartBoard help"));
    connect(ui->helpAction, SIGNAL(triggered()), this, SLOT(boardHelp()));

    ui->aboutSBAction->setIcon(QIcon(":/images/about.png"));
    ui->aboutSBAction->setShortcut(tr("Ctrl+A"));
    ui->aboutSBAction->setStatusTip(tr("About SmartBoard"));
    connect(ui->aboutSBAction, SIGNAL(triggered()), this, SLOT(boardAbout()));
}

// toolbar items
void SmartBoard::createToolBars()
{
    ui->mainToolBar->addAction(ui->publishAction);
    ui->mainToolBar->addAction(ui->subscribeBoardAction);
    ui->mainToolBar->addAction(ui->unsubscribeAction);
    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(ui->viewAction);
    ui->mainToolBar->addAction(ui->searchAction);
    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(ui->optionsAction);
    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(ui->helpAction);
    ui->mainToolBar->addAction(ui->aboutSBAction);
    ui->mainToolBar->addSeparator();

    ui->mainToolBar->addAction(ui->exitAction);
}

// show options dialog
void SmartBoard::boardOptions()
{
    if (ui->m_publishFrame->isVisible()) ui->m_publishFrame->hide();
    if (ui->m_graphicsView->isVisible()) ui->m_graphicsView->hide();
    if (ui->m_triplesView->isVisible()) ui->m_triplesView->hide();

    if (ui->m_optionsFrame->isVisible())
    {
        ui->m_optionsFrame->hide();
        ui->m_triplesView->show();
    }
    else ui->m_optionsFrame->show();
}

// hide options frame
void SmartBoard::on_m_backOptions_clicked()
{
    ui->m_optionsFrame->hide();
    ui->m_triplesView->show();
}

//
void SmartBoard::showViewBoard()
{
    if (ui->m_optionsFrame->isVisible()) ui->m_optionsFrame->hide();
    if (ui->m_publishFrame->isVisible()) ui->m_publishFrame->hide();
    if (ui->m_triplesView->isVisible()) ui->m_triplesView->hide();

    if (ui->m_graphicsView->isVisible())
    {
        ui->m_graphicsView->hide();
        ui->m_triplesView->show();
        m_viewBoard->clear();
    }
    else
    {
        m_viewBoard->clear();
        ui->m_graphicsView->show();
    }

    // paint all existing bulletin boxes
    for (int i = 0; i < ui->m_triplesView->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item =  ui->m_triplesView->topLevelItem(i);
        int rand = qrand() % 350;

        if(item->checkState(3) == Qt::Checked)
        {
            Sticker *sticker = new Sticker(":/images/sticker.svg");
            sticker->setStickerTheme(sticker, item->text(1));
            sticker->setToolTip("<b>" + item->text(0) + "</b>" + "<p>" + item->text(2) + "</p>");
            sticker->translate(rand, rand);
            m_viewBoard->addItem(sticker);
        }
    }
}

// show add bulletin dialog
void SmartBoard::publishAnnouncements()
{
    if (ui->m_optionsFrame->isVisible()) ui->m_optionsFrame->hide();
    if (ui->m_graphicsView->isVisible()) ui->m_graphicsView->hide();

    ui->m_authorTxt->clear();
    ui->m_themeTxt->clear();
    ui->m_bulletinTxt->clear();
    ui->m_publishStatusLbl->clear();

    ui->m_publishLbl->setText(tr("Publish announsment"));
    ui->m_triplesView->hide();

    if (ui->m_publishFrame->isVisible())
    {
        ui->m_publishFrame->hide();
        ui->m_triplesView->show();
    }
    else
    {
        ui->m_publishFrame->show();
    }

    if (ui->m_publishBtn->isHidden())       ui->m_publishBtn->show();
    if (ui->m_publishStatusLbl->isHidden()) ui->m_publishStatusLbl->show();
    if (ui->m_authorTxt->isReadOnly())      ui->m_authorTxt->setReadOnly(false);
    if (ui->m_themeTxt->isReadOnly())       ui->m_themeTxt->setReadOnly(false);
    if (ui->m_bulletinTxt->isReadOnly())    ui->m_bulletinTxt->setReadOnly(false);
}

// hide publish data frame
void SmartBoard::on_m_backPublish_clicked()
{
    ui->m_publishFrame->hide();
    ui->m_triplesView->show();

    ui->m_publishStatusLbl->clear();
    ui->m_authorTxt->clear();
    ui->m_themeTxt->clear();
    ui->m_bulletinTxt->clear();
}

// subscribe button
void SmartBoard::subscribeBoard()
{
    if (ui->m_optionsFrame->isVisible()) ui->m_optionsFrame->hide();
    if (ui->m_publishFrame->isVisible()) ui->m_publishFrame->hide();
    if (ui->m_graphicsView->isVisible()) ui->m_publishFrame->hide();
    if (ui->m_triplesView->isHidden()) ui->m_triplesView->show();

    sibSubscribe(new Triple(TripleElement("sib:any", TripleElement::ElementTypeURI),
                            TripleElement("sib:any"),
                            TripleElement("sib:any", TripleElement::ElementTypeURI)));

    ui->searchAction->setEnabled(true);
    ui->viewAction->setEnabled(true);
}

// smart board help
void SmartBoard::boardHelp()
{
    QMessageBox::about(this, tr("Smart Board help"),
                       tr("<h2>Smart Board</h2>"
                       "<p><b>Follow this instructions:</b> "
                       "<p><b>Options</b> - connect to the SIB, point SIB IP"
                       "<p><b>Publish</b> - publish new announsment."
                       "<p><b>Subscribe</b> - subscribe to announsments."
                       "<p><b>Unsubscribe</b> - unsubscribe from announsments."
                       "<p><b>View</b> - graphical view of selected announsments."
                       "<p><b>Search</b> - search information in announsments."
                       "<p><b>Double click</b> on table item - get full information."));
}

// about Smart Board
void SmartBoard::boardAbout()
{
    QMessageBox::about(this, tr("About Smart Board"),
                       tr("<h2>Smart Board</h2>"
                       "<p>Smart Board is a client application that "
                       "work with announsment board. Smart Board is part "
                       "of a common smart space where users can share "
                       "announcements, publish and monitor their timely "
                       "appearance."));
}

// leave selected sib
void SmartBoard::leaveSelectedSib()
{
    m_sibUri = getSelectedItemUri();

    if(m_nodes.contains(m_sibUri))
    {
        if(m_nodes[m_sibUri]->leave() < 0)
        {
            QMessageBox::critical(this, "Leave error", "Leave failed from SIB " + m_sibUri);
            enableMenuItem(false);
        }

        QWhiteBoardNode *node = m_nodes.take(m_sibUri);
        node->deleteLater();
        ui->m_sibStatus->setText(tr("Not joined to SIB ")  + m_sibUri);
        ui->m_sibStatus->setStyleSheet("QLabel {color : red;}");
        enableMenuItem(false);
    }
}

// join to selected list sib
void SmartBoard::joinSelectedSib()
{
    m_sibUri = getSelectedItemUri();

    if(addNode(m_sibUri))
    {
        connect(m_nodes[m_sibUri], SIGNAL(joinComplete(int)), this, SLOT(joinComplete(int)));

        if(m_nodes[m_sibUri]->join(m_sibUri) < 0)
        {
            QMessageBox::critical(this, "Join error","Join failed to SIB "  + m_sibUri);
            ui->m_sibStatus->setText(tr("Not joined to SIB ")  + m_sibUri);
            ui->m_sibStatus->setStyleSheet("QLabel {color : red;}");
            enableMenuItem(false);
        }
    }
}


// add new node, if not created
bool SmartBoard::addNode(QString sib)
{
    if(!m_nodes.contains(sib))
    {
        m_nodes[sib]= new QWhiteBoardNode();
        return true;
    }
    else
        return false;
}

//
void SmartBoard::joinComplete(int success)
{
    QWhiteBoardNode *node = static_cast<QWhiteBoardNode *>(sender());

    m_sibUri = getSelectedItemUri();

    if(node)
    {
        QString sib = m_nodes.key(node);

        if(success < 0)
        {
            QMessageBox::critical(this, "Join error","Join failed to SIB "  + m_sibUri);
            ui->m_sibStatus->setText(tr("Not joined to SIB ")  + m_sibUri);
            ui->m_sibStatus->setStyleSheet("QLabel { color : red; }");
            enableMenuItem(false);
        }
        else
        {
            ui->m_sibStatus->setText(tr("Joined to SIB ")  + m_sibUri);
            ui->m_sibStatus->setStyleSheet("QLabel { color : green; }");
            enableMenuItem(true);
        }
    }
    else
    {
        QMessageBox::critical(this, "Join error","Internal Join error");
        ui->m_sibStatus->setText(tr("Not joined to SIB ")  + m_sibUri);
        ui->m_sibStatus->setStyleSheet("QLabel { color : red; }");
        enableMenuItem(false);
    }
}

// print sibs
void SmartBoard::sibList(const QList<QSibInfo *> &siblist)
{
    QList<QSibInfo *>::const_iterator it;
    qDebug() << "SmartBoard::sibList, len: " << siblist.size();

    for(it = siblist.begin(); it != siblist.end(); ++it)
    {
        qDebug() << "SIB found:";
        qDebug() << "Uri: " << (*it)->uri();
        qDebug() << "Name: " << (*it)->name();

        QStringList new_sib;
        new_sib << (*it)->uri() << (*it)->name();
        QString label = new_sib.join("--");

        if(ui->m_sibList->findItems(label, Qt::MatchStartsWith).count() == 0)
            ui->m_sibList->addItem(label);
    }
}

//
void SmartBoard::sibInserted(const QSibInfo *sib)
{
    QStringList new_sib;
    new_sib << sib->uri() << sib->name();
    QString label = new_sib.join("--");

    if( ui->m_sibList->findItems(label, Qt::MatchStartsWith).count() == 0)
        ui->m_sibList->addItem(label);
}

//
void SmartBoard::sibRemoved(QString uri)
{
    QList<QListWidgetItem *> items =  ui->m_sibList->findItems(uri, Qt::MatchStartsWith);

    QList<QListWidgetItem *>::iterator item_it;
    for( item_it = items.begin(); item_it != items.end(); item_it++)
    {
        QListWidgetItem *item = ui->m_sibList->takeItem(ui->m_sibList->row(*item_it));
        delete item;
    }
}

// join or leave sib
void SmartBoard::on_m_sibList_doubleClicked()
{
    m_sibUri = getSelectedItemUri();

    if(m_nodes.contains(m_sibUri))
        leaveSelectedSib();
    else
        joinSelectedSib();
}


// publish announsment to the SS
void SmartBoard::on_m_publishBtn_clicked()
{
    if(ui->m_authorTxt->text().isEmpty() ||
       ui->m_themeTxt->text().isEmpty() ||
       ui->m_bulletinTxt->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, "Empty fields","Please, insert some data to the fields!");
        return;
    }

    if(ui->m_authorTxt->text().contains("&", Qt::CaseInsensitive) ||
       ui->m_themeTxt->text().contains("&", Qt::CaseInsensitive)  ||
       ui->m_bulletinTxt->toPlainText().contains("&", Qt::CaseInsensitive))
    {
        QMessageBox::warning(this, "Invalid character","Entered an invalid character: &. Please check this!");
        return;
    }

    Triple *triple = new Triple(TripleElement(ui->m_authorTxt->text(), TripleElement::ElementTypeURI),
                                TripleElement(ui->m_themeTxt->text()),
                                TripleElement(ui->m_bulletinTxt->toPlainText(), TripleElement::ElementTypeURI));

    m_graphList.append(triple);

    if(ui->m_sibList->count() > 0)
    {
        m_sibUri = getSelectedItemUri();

        if(m_nodes.contains(m_sibUri) && m_nodes[m_sibUri]->insert(m_graphList) >= 0)
        {
            ui->m_publishStatusLbl->setText(tr("Announsment is published!"));
            ui->m_publishStatusLbl->setStyleSheet("QLabel { color : green; }");
        }
        else
        {
            ui->m_publishStatusLbl->setText(tr("Announsment isn't published!"));
            ui->m_publishStatusLbl->setStyleSheet("QLabel { color : red; }");
        }
    }
    else
        QMessageBox::warning(this, "SmartSpace Error", "Insert, No SmartSpaces found");

    while(m_graphList.count())
    {
        Triple *triple = m_graphList.takeFirst();
        delete triple;
    }

    // set added announcement item to scene
    m_sticker->translate(20, 20);
    m_sticker->setStickerTheme(m_sticker, ui->m_themeTxt->text());
    m_sticker->setToolTip("<b>" + ui->m_authorTxt->text() + "</b>" + "<p>" + ui->m_bulletinTxt->toPlainText() + "</p>");
    m_viewBoard->addItem(m_sticker);
}

void SmartBoard::sibSubscribe(Triple *triple)
{
    QList<Triple *> list;
    list.append(triple);

    if(ui->m_sibList->count() > 0)
    {
        m_sibUri = getSelectedItemUri();

        if(m_nodes.contains(m_sibUri))
        {
            TemplateSubscription *s = new TemplateSubscription(m_nodes[m_sibUri]);
            s->setObjectName("subscription");
            if(!m_subscriptions.contains(s->objectName()))
                m_subscriptions[s->objectName()] = s;
            else
            {
                QMessageBox::warning(this, "Subscription error","Subscription already exists");
                return;
            }

            connect(s, SIGNAL(finished(int)), this, SLOT(subscriptionFinished()));
            connect(s, SIGNAL(indication()), this, SLOT(subscriptionIndication()));
            s->subscribe(list);
        }
    }
    while(list.count())
    {
        triple = list.takeFirst();
        delete triple;
    }

    ui->unsubscribeAction->setEnabled(true);
}

// fill table with getted announsments
void SmartBoard::subscriptionIndication()
{
    if(m_subscriptions.contains(sender()->objectName()))
    {
        TemplateSubscription *q = NULL;
        q = m_subscriptions[sender()->objectName()];

        m_activeSubscription =  sender()->objectName();

        QList<Triple *> resultsAdded = q->resultsAdded();
        QList<Triple *> resultsObs = q->resultsObsolete();
        QList<Triple *>::iterator it;
        QTreeWidgetItem *rootItem = ui->m_triplesView->invisibleRootItem();

        qDebug() << "Got: " << resultsAdded.count() << " new triples";
        qDebug() << "Got: " << resultsObs.count() << " obsolete triples";

        for(it = resultsObs.begin(); it != resultsObs.end(); ++it)
        {
            for(int ii = 0; ii < rootItem->childCount(); ii++)
            {
                QTreeWidgetItem *child = rootItem->child(ii);
                if(((*it)->subject().node().compare(child->text(0)) == 0) &&
                  ((*it)->predicate().node().compare(child->text(1)) == 0) &&
                  (((*it)->object().node().isEmpty() && (child->text(2).compare("<empty>")==0)) ||
                  ((*it)->object().node().compare(child->text(2)) == 0)))
                {
                    QTreeWidgetItem *c1 = rootItem->takeChild(ii);
                    delete c1;
                    break;
                }
            }
        }
        if(resultsAdded.count())
        {
            QList<QTreeWidgetItem *> items;
            for(it = resultsAdded.begin(); it != resultsAdded.end(); ++it)
            {
                if (!(*it)->object().node().contains("http://", Qt::CaseInsensitive))
                {
                    QStringList strings;
                    strings << (*it)->subject().node() << (*it)->predicate().node();

                    if(!(*it)->object().node().isEmpty())
                        strings << (*it)->object().node();
                    else
                        strings << "<empty>";

                    QTreeWidgetItem *item = new QTreeWidgetItem(strings);
                    item->setCheckState(3, Qt::Unchecked);
                    items.append(item);
                }
            }

            ui->m_triplesView->addTopLevelItems(items);

            ui->unsubscribeAction->setEnabled(true);
            ui->subscribeBoardAction->setEnabled(false);

            connect(ui->unsubscribeAction, SIGNAL(triggered()), this, SLOT(sibUnsubscribe()));
            connect(ui->m_triplesView, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
                    this, SLOT(fullBulletinInfo(QTreeWidgetItem *)));
        }
    }
}

// execute after unsibscribe
void SmartBoard::subscriptionFinished()
{
    SubscriptionBase *q = NULL;

    if(m_subscriptions.contains(sender()->objectName()))
    {
        q = m_subscriptions.take(sender()->objectName());
        delete q;
    }
}

void SmartBoard::sibUnsubscribe()
{
    if(!m_activeSubscription.isEmpty())
    {
        if(m_subscriptions.contains(m_activeSubscription))
        {
            TemplateSubscription *s = m_subscriptions.take(m_activeSubscription);
            s->unsubscribe();
        }

        m_activeSubscription.clear();
        ui->m_triplesView->clear();
        ui->subscribeBoardAction->setEnabled(true);
        ui->unsubscribeAction->setEnabled(false);
        ui->searchAction->setEnabled(false);
        ui->viewAction->setEnabled(false);

        // удалить все элементы сцены
        m_viewBoard->clear();
    }
}

void SmartBoard::fullBulletinInfo(QTreeWidgetItem *item)
{
    ui->m_publishLbl->setText(tr("Announsment"));

    ui->m_triplesView->hide();
    ui->m_publishBtn->hide();
    ui->m_publishStatusLbl->hide();
    ui->m_publishFrame->show();

    ui->m_authorTxt->setText(item->text(0));
    ui->m_authorTxt->setReadOnly(true);
    ui->m_themeTxt->setText(item->text(1));
    ui->m_themeTxt->setReadOnly(true);
    ui->m_bulletinTxt->setText(item->text(2));
    ui->m_bulletinTxt->setReadOnly(true);
}

void SmartBoard::enableMenuItem(bool option)
{
    ui->subscribeBoardAction->setEnabled(option);
    ui->publishAction->setEnabled(option);
}

QString SmartBoard::getSelectedItemUri()
{
    QListWidgetItem *item = ui->m_sibList->item(ui->m_sibList->currentRow());
    QStringList txt = item->text().split("--");
    QString sib_uri = txt[0];
    return sib_uri;
}

// убрать или переработать!
void SmartBoard::on_m_searchBtn_clicked()
{
    QString sibIpFileName = m_fileDialog.getOpenFileName(this, tr("Open and change SIB IP file"),
                                                         m_directory.path());
    if (sibIpFileName.isNull() == false)
    {
        QFile sibIpFile(sibIpFileName);

        if (!sibIpFile.open(QIODevice::WriteOnly | QIODevice::Text))
            qWarning() << "Cannot open " << sibIpFile.fileName() << " file";
        else
        {
            // сделать проверки на
            // 1. Диапозон ИП адреса (16 макс)
            // 2. На запись только цифр и т.д
            if (!ui->m_sibIPTxt->text().isEmpty())
            {
                QTextStream out(&sibIpFile);
                out << ui->m_sibIPTxt->text();

                sibIpFile.close();
                ui->m_sibIPStatusLbl->setText("SIB IP successfully written.");
            }
            else
                QMessageBox::information(this, "Empty SIB IP address", "Please write your SIB IP");
        }
    }
}

// поиск объявлений
void SmartBoard::searchAnnouncements()
{
    if (!m_findDialog)
    {
        m_findDialog = new FindDialog(this);
        connect(m_findDialog, SIGNAL(findNext(const QString &, Qt::CaseSensitivity)),
                this, SLOT(findData(const QString &, Qt::CaseSensitivity)));
    }

    m_findDialog->show();
    m_findDialog->activateWindow();
}

//
void SmartBoard::findData(const QString &str, Qt::CaseSensitivity cs)
{
    QTreeWidgetItem *rootItem = ui->m_triplesView->invisibleRootItem();

    for(int i = 0; i < rootItem->childCount(); i++)
    {
        QTreeWidgetItem *child = rootItem->child(i);

        child->setBackgroundColor(0, QColor("white"));
        child->setBackgroundColor(1, QColor("white"));
        child->setBackgroundColor(2, QColor("white"));

        if (child->text(0).contains(str, cs) ||
            child->text(1).contains(str, cs) ||
            child->text(2).contains(str, cs))
        {
            child->setBackgroundColor(0, QColor("yellow"));
            child->setBackgroundColor(1, QColor("yellow"));
            child->setBackgroundColor(2, QColor("yellow"));
        }
    }
    QApplication::beep();
}
