// finddialog.cpp

#include <QtGui>

#include "finddialog.h"

FindDialog::FindDialog(QWidget *parent)
    : QDialog(parent)
{
    m_label = new QLabel(tr("Find:"));
    m_lineEdit = new QLineEdit;
    m_label->setBuddy(m_lineEdit);

    m_caseCheckBox = new QCheckBox(tr("Match &case"));

    m_findButton = new QPushButton(tr("&Find"));
    m_findButton->setDefault(true);
    m_findButton->setEnabled(false);

    m_closeButton = new QPushButton(tr("Close"));

    connect(m_lineEdit, SIGNAL(textChanged(const QString &)), this, SLOT(enableFindButton(const QString &)));
    connect(m_findButton, SIGNAL(clicked()), this, SLOT(findClicked()));
    connect(m_closeButton, SIGNAL(clicked()), this, SLOT(close()));

    QHBoxLayout *topLeftLayout = new QHBoxLayout;
    topLeftLayout->addWidget(m_label);
    topLeftLayout->addWidget(m_lineEdit);

    QVBoxLayout *leftLayout = new QVBoxLayout;
    leftLayout->addLayout(topLeftLayout);
    leftLayout->addWidget(m_caseCheckBox);

    QVBoxLayout *rightLayout = new QVBoxLayout;
    rightLayout->addWidget(m_findButton);
    rightLayout->addWidget(m_closeButton);
    rightLayout->addStretch();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    setWindowTitle(tr("Find"));
    setFixedHeight(sizeHint().height());
}

void FindDialog::findClicked()
{
    QString text = m_lineEdit->text();
    Qt::CaseSensitivity cs = m_caseCheckBox->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive;
    emit findNext(text, cs);
}

void FindDialog::enableFindButton(const QString &text)
{
    m_findButton->setEnabled(!text.isEmpty());
}
