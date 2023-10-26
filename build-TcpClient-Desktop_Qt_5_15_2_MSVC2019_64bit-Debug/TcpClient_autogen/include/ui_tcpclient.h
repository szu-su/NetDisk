/********************************************************************************
** Form generated from reading UI file 'tcpclient.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TCPCLIENT_H
#define UI_TCPCLIENT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TcpClient
{
public:
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *name_lab;
    QLineEdit *name_input;
    QHBoxLayout *horizontalLayout;
    QLabel *pwd_lab;
    QLineEdit *pwd_input;
    QPushButton *login_pb;
    QVBoxLayout *verticalLayout;
    QPushButton *regist_pb;
    QPushButton *cancel_pb;

    void setupUi(QWidget *TcpClient)
    {
        if (TcpClient->objectName().isEmpty())
            TcpClient->setObjectName(QString::fromUtf8("TcpClient"));
        TcpClient->resize(350, 251);
        verticalLayout_3 = new QVBoxLayout(TcpClient);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        name_lab = new QLabel(TcpClient);
        name_lab->setObjectName(QString::fromUtf8("name_lab"));
        QFont font;
        font.setPointSize(14);
        name_lab->setFont(font);

        horizontalLayout_2->addWidget(name_lab);

        name_input = new QLineEdit(TcpClient);
        name_input->setObjectName(QString::fromUtf8("name_input"));

        horizontalLayout_2->addWidget(name_input);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        pwd_lab = new QLabel(TcpClient);
        pwd_lab->setObjectName(QString::fromUtf8("pwd_lab"));
        pwd_lab->setFont(font);

        horizontalLayout->addWidget(pwd_lab);

        pwd_input = new QLineEdit(TcpClient);
        pwd_input->setObjectName(QString::fromUtf8("pwd_input"));
        pwd_input->setEchoMode(QLineEdit::Password);

        horizontalLayout->addWidget(pwd_input);


        verticalLayout_2->addLayout(horizontalLayout);

        login_pb = new QPushButton(TcpClient);
        login_pb->setObjectName(QString::fromUtf8("login_pb"));
        QFont font1;
        font1.setPointSize(16);
        login_pb->setFont(font1);

        verticalLayout_2->addWidget(login_pb);


        horizontalLayout_3->addLayout(verticalLayout_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        regist_pb = new QPushButton(TcpClient);
        regist_pb->setObjectName(QString::fromUtf8("regist_pb"));

        verticalLayout->addWidget(regist_pb);

        cancel_pb = new QPushButton(TcpClient);
        cancel_pb->setObjectName(QString::fromUtf8("cancel_pb"));

        verticalLayout->addWidget(cancel_pb);


        horizontalLayout_3->addLayout(verticalLayout);


        verticalLayout_3->addLayout(horizontalLayout_3);


        retranslateUi(TcpClient);

        QMetaObject::connectSlotsByName(TcpClient);
    } // setupUi

    void retranslateUi(QWidget *TcpClient)
    {
        TcpClient->setWindowTitle(QCoreApplication::translate("TcpClient", "TcpClient", nullptr));
        name_lab->setText(QCoreApplication::translate("TcpClient", "\347\224\250\346\210\267\345\220\215\357\274\232", nullptr));
        pwd_lab->setText(QCoreApplication::translate("TcpClient", "\345\257\206\347\240\201\357\274\232", nullptr));
        login_pb->setText(QCoreApplication::translate("TcpClient", "\347\231\273\345\275\225", nullptr));
        regist_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\345\206\214", nullptr));
        cancel_pb->setText(QCoreApplication::translate("TcpClient", "\346\263\250\351\224\200", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TcpClient: public Ui_TcpClient {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TCPCLIENT_H
