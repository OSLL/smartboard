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
    m_sticker(new Sticker(":/images/sticker.svg")), m_findDialog(0)
{
    ui->setupUi(this);

    createForms();
    createActions();
    createToolBars();

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

// crete gui forms
void SmartBoard::createForms()
{
    // options frame
    m_optionsFrame = new QFrame(ui->centralWidget);

    m_optionsLbl = new QLabel("Options", m_optionsFrame);
    m_font.setPointSize(20);
    m_font.setBold(true);
    m_font.setWeight(75);
    m_optionsLbl->setFont(m_font);

    m_sibLbl = new QLabel("SIB's", m_optionsFrame);
    m_sibList = new QListWidget(m_optionsFrame);
    connect(m_sibList, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(sibList_doubleClicked()));
    m_sibStatusLbl = new QLabel("Status: ", m_optionsFrame);
    m_sibStatus = new QLabel(m_optionsFrame);
    m_backOptions = new QPushButton("Back", m_optionsFrame);
    connect(m_backOptions, SIGNAL(clicked()), this, SLOT(backOptions_clicked()));

    m_optionsVboxLayout = new QVBoxLayout;
    m_optionsHboxLayout = new QHBoxLayout;
    m_optionsHboxLayout->addWidget(m_sibStatusLbl);
    m_optionsHboxLayout->setSpacing(1);
    m_optionsHboxLayout->addWidget(m_sibStatus);

    m_optionsVboxLayout->setSpacing(5);
    m_optionsVboxLayout->addWidget(m_optionsLbl);
    m_optionsVboxLayout->addWidget(m_sibLbl);
    m_optionsVboxLayout->addWidget(m_sibList);
    m_optionsVboxLayout->addLayout(m_optionsHboxLayout);
    m_optionsVboxLayout->addWidget(m_backOptions);

    m_optionsFrame->setLayout(m_optionsVboxLayout);

    // publish frame
    m_publishFrame = new QFrame(ui->centralWidget);

    m_publishLbl = new QLabel(m_publishFrame);
    m_font.setPointSize(20);
    m_font.setBold(true);
    m_font.setWeight(75);
    m_publishLbl->setFont(m_font);

    m_authorLbl = new QLabel("Author", m_publishFrame);
    m_authorTxt = new QLineEdit(m_publishFrame);
    m_themeLbl = new QLabel("Theme", m_publishFrame);
    m_themeTxt = new QLineEdit(m_publishFrame);
    m_bulletinLbl = new QLabel("Announcement", m_publishFrame);
    m_bulletinTxt = new QTextEdit(m_publishFrame);
    m_publishStatusLbl = new QLabel(m_publishFrame);
    m_publishBtn = new QPushButton("Publish", m_publishFrame);
    connect(m_publishBtn, SIGNAL(clicked()), this, SLOT(publishBtn_clicked()));
    m_backPublish = new QPushButton("Back", m_publishFrame);
    connect(m_backPublish, SIGNAL(clicked()), this, SLOT(backPublish_clicked()));

    m_publishVboxLayout = new QVBoxLayout;
    m_publishHboxLayout = new QHBoxLayout;
    m_publishHboxLayout->addWidget(m_publishBtn);
    m_publishHboxLayout->addStretch(1);
    m_publishHboxLayout->addWidget(m_backPublish);

    m_publishVboxLayout->setSpacing(5);
    m_publishVboxLayout->addWidget(m_publishLbl);
    m_publishVboxLayout->addWidget(m_authorLbl);
    m_publishVboxLayout->addWidget(m_authorTxt);
    m_publishVboxLayout->addWidget(m_themeLbl);
    m_publishVboxLayout->addWidget(m_themeTxt);
    m_publishVboxLayout->addWidget(m_bulletinLbl);
    m_publishVboxLayout->addWidget(m_bulletinTxt);
    m_publishVboxLayout->addWidget(m_publishStatusLbl);
    m_publishVboxLayout->addLayout(m_publishHboxLayout);

    m_publishFrame->setLayout(m_publishVboxLayout);

    // triples view
    m_triplesView = new QTreeWidget(ui->centralWidget);

    QTreeWidgetItem *tripleItem = new QTreeWidgetItem();
    tripleItem->setText(3, QString::fromUtf8("4"));
    tripleItem->setText(2, QString::fromUtf8("3"));
    tripleItem->setText(1, QString::fromUtf8("2"));
    tripleItem->setText(0, QString::fromUtf8("1"));

    m_triplesView->setHeaderItem(tripleItem);
    m_triplesView->setMouseTracking(false);
    m_triplesView->setHeaderHidden(false);
    m_triplesView->setExpandsOnDoubleClick(true);
    m_triplesView->setColumnCount(4);
    m_triplesView->header()->setVisible(true);
    m_triplesView->header()->setCascadingSectionResizes(false);
    m_triplesView->header()->setHighlightSections(false);

    // stickers view
    m_graphicsView = new QGraphicsView(ui->centralWidget);

    // central widget layout
    m_gridLayout = new QGridLayout;
    m_gridLayout->setMargin(0);
    m_gridLayout->addWidget(m_optionsFrame, 0, 0);
    m_gridLayout->addWidget(m_publishFrame, 0, 0);
    m_gridLayout->addWidget(m_triplesView, 0, 0);
    m_gridLayout->addWidget(m_graphicsView, 0, 0);

    ui->centralWidget->setLayout(m_gridLayout);

    // menu
    ui->menuBar->hide();
    m_optionsFrame->hide();
    m_publishFrame->hide();
    ui->mainToolBar->setMovable(false);
    ui->subscribeBoardAction->setEnabled(false);
    ui->publishAction->setEnabled(false);
    ui->searchAction->setEnabled(false);
    ui->unsubscribeAction->setEnabled(false);
    ui->viewAction->setEnabled(false);

    this->setTabOrder(m_authorTxt, m_themeTxt);
    this->setTabOrder(m_themeTxt, m_bulletinTxt);

    m_triplesView->setColumnWidth(0,70);
    m_triplesView->setColumnWidth(1,90);
    m_triplesView->setColumnWidth(2,275);
    m_triplesView->setColumnWidth(3,10);
    QStringList headers;
    headers << tr("Author") << tr("Theme") << tr("Message") << tr("View");
    m_triplesView->setHeaderLabels(headers);
    m_sibStatus->setStyleSheet("QLabel { color : red; }");

    // graphics view
    m_viewBoard = new ViewBoard;
    //m_graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //m_graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_graphicsView->setRenderHint(QPainter::Antialiasing, true);
    m_graphicsView->setCacheMode(QGraphicsView::CacheBackground);
    m_graphicsView->setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
    m_graphicsView->setOptimizationFlags(QGraphicsView::DontClipPainter |
                                             QGraphicsView::DontSavePainterState |
                                             QGraphicsView::DontAdjustForAntialiasing);
    m_graphicsView->setScene(m_viewBoard);
    m_graphicsView->hide();
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
    if (m_publishFrame->isVisible()) m_publishFrame->hide();
    if (m_graphicsView->isVisible()) m_graphicsView->hide();
    if (m_triplesView->isVisible()) m_triplesView->hide();

    if (m_optionsFrame->isVisible())
    {
        m_optionsFrame->hide();
        m_triplesView->show();
    }
    else m_optionsFrame->show();
}

// hide options frame
void SmartBoard::backOptions_clicked()
{
    m_optionsFrame->hide();
    m_triplesView->show();
}

//
void SmartBoard::showViewBoard()
{
    if (m_optionsFrame->isVisible()) m_optionsFrame->hide();
    if (m_publishFrame->isVisible()) m_publishFrame->hide();
    if (m_triplesView->isVisible()) m_triplesView->hide();

    if (m_graphicsView->isVisible())
    {
        m_graphicsView->hide();
        m_triplesView->show();
        m_viewBoard->clear();
    }
    else
    {
        m_viewBoard->clear();
        m_graphicsView->show();
    }

    // paint all existing bulletin boxes
    for (int i = 0; i < m_triplesView->topLevelItemCount(); i++)
    {
        QTreeWidgetItem *item =  m_triplesView->topLevelItem(i);
        //int rand = qrand() % 350;

        if(item->checkState(3) == Qt::Checked)
        {
            Sticker *sticker = new Sticker(":/images/sticker.svg");
            sticker->setStickerTheme(sticker, item->text(1));
            sticker->setToolTip("<b>" + item->text(0) + "</b>" + "<p>" + item->text(2) + "</p>");
            sticker->translate(qrand() % 300, qrand() % 300);
            m_viewBoard->addItem(sticker);
        }
    }
}

// show add bulletin dialog
void SmartBoard::publishAnnouncements()
{
    if (m_optionsFrame->isVisible()) m_optionsFrame->hide();
    if (m_graphicsView->isVisible()) m_graphicsView->hide();

    m_authorTxt->clear();
    m_themeTxt->clear();
    m_bulletinTxt->clear();
    m_publishStatusLbl->clear();

    m_publishLbl->setText(tr("Publish announsment"));
    m_triplesView->hide();

    if (m_publishFrame->isVisible())
    {
        m_publishFrame->hide();
        m_triplesView->show();
    }
    else
    {
        m_publishFrame->show();
    }

    if (m_publishBtn->isHidden())       m_publishBtn->show();
    if (m_publishStatusLbl->isHidden()) m_publishStatusLbl->show();
    if (m_authorTxt->isReadOnly())      m_authorTxt->setReadOnly(false);
    if (m_themeTxt->isReadOnly())       m_themeTxt->setReadOnly(false);
    if (m_bulletinTxt->isReadOnly())    m_bulletinTxt->setReadOnly(false);
}

// hide publish data frame
void SmartBoard::backPublish_clicked()
{
    m_publishFrame->hide();
    m_triplesView->show();

    m_publishStatusLbl->clear();
    m_authorTxt->clear();
    m_themeTxt->clear();
    m_bulletinTxt->clear();
}

// subscribe button
void SmartBoard::subscribeBoard()
{
    if (m_optionsFrame->isVisible()) m_optionsFrame->hide();
    if (m_publishFrame->isVisible()) m_publishFrame->hide();
    if (m_graphicsView->isVisible()) m_publishFrame->hide();
    if (m_triplesView->isHidden()) m_triplesView->show();

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
                       "<p><b>Options</b> - connect to the SIB"
                       "<p><b>Publish</b> - publish new announsment."
                       "<p><b>Subscribe</b> - subscribe to announsments."
                       "<p><b>Unsubscribe</b> - unsubscribe from announsments."
                       "<p><b>View</b> - graphical view of selected announsments."
                       "<p><b>Double click</b> on table item - get full information."
                       "<p>See <b>README</b> file for full process description."));
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
        m_sibStatus->setText(tr("Not joined to SIB ")  + m_sibUri);
        m_sibStatus->setStyleSheet("QLabel {color : red;}");
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
            m_sibStatus->setText(tr("Not joined to SIB ")  + m_sibUri);
            m_sibStatus->setStyleSheet("QLabel {color : red;}");
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
            m_sibStatus->setText(tr("Not joined to SIB ")  + m_sibUri);
            m_sibStatus->setStyleSheet("QLabel { color : red; }");
            enableMenuItem(false);
        }
        else
        {
            m_sibStatus->setText(tr("Joined to SIB ")  + m_sibUri);
            m_sibStatus->setStyleSheet("QLabel { color : green; }");
            enableMenuItem(true);
        }
    }
    else
    {
        QMessageBox::critical(this, "Join error","Internal Join error");
        m_sibStatus->setText(tr("Not joined to SIB ")  + m_sibUri);
        m_sibStatus->setStyleSheet("QLabel { color : red; }");
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

        if(m_sibList->findItems(label, Qt::MatchStartsWith).count() == 0)
            m_sibList->addItem(label);
    }
}

//
void SmartBoard::sibInserted(const QSibInfo *sib)
{
    QStringList new_sib;
    new_sib << sib->uri() << sib->name();
    QString label = new_sib.join("--");

    if( m_sibList->findItems(label, Qt::MatchStartsWith).count() == 0)
        m_sibList->addItem(label);
}

//
void SmartBoard::sibRemoved(QString uri)
{
    QList<QListWidgetItem *> items =  m_sibList->findItems(uri, Qt::MatchStartsWith);

    QList<QListWidgetItem *>::iterator item_it;
    for( item_it = items.begin(); item_it != items.end(); item_it++)
    {
        QListWidgetItem *item = m_sibList->takeItem(m_sibList->row(*item_it));
        delete item;
    }
}

// join or leave sib
void SmartBoard::sibList_doubleClicked()
{
    m_sibUri = getSelectedItemUri();

    if(m_nodes.contains(m_sibUri))
        leaveSelectedSib();
    else
        joinSelectedSib();
}


// publish announsment to the SS
void SmartBoard::publishBtn_clicked()
{
    if(m_authorTxt->text().isEmpty() ||
       m_themeTxt->text().isEmpty() ||
       m_bulletinTxt->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, "Empty fields","Please, insert some data to the fields!");
        return;
    }

    if(m_authorTxt->text().contains("&", Qt::CaseInsensitive) ||
       m_themeTxt->text().contains("&", Qt::CaseInsensitive)  ||
       m_bulletinTxt->toPlainText().contains("&", Qt::CaseInsensitive))
    {
        QMessageBox::warning(this, "Invalid character","Entered an invalid character: &. Please check this!");
        return;
    }

    Triple *triple = new Triple(TripleElement(m_authorTxt->text(), TripleElement::ElementTypeURI),
                                TripleElement(m_themeTxt->text()),
                                TripleElement(m_bulletinTxt->toPlainText(), TripleElement::ElementTypeURI));

    m_graphList.append(triple);

    if(m_sibList->count() > 0)
    {
        m_sibUri = getSelectedItemUri();

        if(m_nodes.contains(m_sibUri) && m_nodes[m_sibUri]->insert(m_graphList) >= 0)
        {
            m_publishStatusLbl->setText(tr("Announsment is published!"));
            m_publishStatusLbl->setStyleSheet("QLabel { color : green; }");
        }
        else
        {
            m_publishStatusLbl->setText(tr("Announsment isn't published!"));
            m_publishStatusLbl->setStyleSheet("QLabel { color : red; }");
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
    m_sticker->setStickerTheme(m_sticker, m_themeTxt->text());
    m_sticker->setToolTip("<b>" + m_authorTxt->text() + "</b>" + "<p>" + m_bulletinTxt->toPlainText() + "</p>");
    m_viewBoard->addItem(m_sticker);
}

void SmartBoard::sibSubscribe(Triple *triple)
{
    QList<Triple *> list;
    list.append(triple);

    if(m_sibList->count() > 0)
    {
        m_sibUri = getSelectedItemUri();

        if(m_nodes.contains(m_sibUri))
        {
            TemplateSubscription *s = new TemplateSubscription(m_nodes[m_sibUri]);
            s->setObjectName("smartSubscription");
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
        QTreeWidgetItem *rootItem = m_triplesView->invisibleRootItem();

        qDebug() << "Got: " << resultsAdded.count() << " new triples";
        qDebug() << "Got: " << resultsObs.count() << " obsolete triples";

        for(it = resultsObs.begin(); it != resultsObs.end(); ++it)
        {
            for(int i = 0; i < rootItem->childCount(); i++)
            {
                QTreeWidgetItem *child = rootItem->child(i);
                if(((*it)->subject().node().compare(child->text(0)) == 0) &&
                  ((*it)->predicate().node().compare(child->text(1)) == 0) &&
                  (((*it)->object().node().isEmpty() && (child->text(2).compare("<empty>")==0)) ||
                  ((*it)->object().node().compare(child->text(2)) == 0)))
                {
                    QTreeWidgetItem *c1 = rootItem->takeChild(i);
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

            m_triplesView->addTopLevelItems(items);

            ui->unsubscribeAction->setEnabled(true);
            ui->subscribeBoardAction->setEnabled(false);

            connect(ui->unsubscribeAction, SIGNAL(triggered()), this, SLOT(sibUnsubscribe()));
            connect(m_triplesView, SIGNAL(itemDoubleClicked(QTreeWidgetItem *, int)),
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
        m_triplesView->clear();
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
    m_publishLbl->setText(tr("Announsment"));

    m_triplesView->hide();
    m_publishBtn->hide();
    m_publishStatusLbl->hide();
    m_publishFrame->show();

    m_authorTxt->setText(item->text(0));
    m_authorTxt->setReadOnly(true);
    m_themeTxt->setText(item->text(1));
    m_themeTxt->setReadOnly(true);
    m_bulletinTxt->setText(item->text(2));
    m_bulletinTxt->setReadOnly(true);
}

void SmartBoard::enableMenuItem(bool option)
{
    ui->subscribeBoardAction->setEnabled(option);
    ui->publishAction->setEnabled(option);
}

QString SmartBoard::getSelectedItemUri()
{
    QListWidgetItem *item = m_sibList->item(m_sibList->currentRow());
    QStringList txt = item->text().split("--");
    QString sib_uri = txt[0];
    return sib_uri;
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
    QTreeWidgetItem *rootItem = m_triplesView->invisibleRootItem();

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
