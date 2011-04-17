#ifndef FINDDIALOG_H
#define FINDDIALOG_H

#include <QDialog>

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class FindDialog : public QDialog
{
    Q_OBJECT

public:
    FindDialog(QWidget *parent = 0);

signals:
    void findNext(const QString &str, Qt::CaseSensitivity cs);

private slots:
    void findClicked();
    void enableFindButton(const QString &text);

private:
    QLabel *m_label;
    QLineEdit *m_lineEdit;
    QCheckBox *m_caseCheckBox;
    QPushButton *m_findButton;
    QPushButton *m_closeButton;
};

#endif // FINDDIALOG_H
