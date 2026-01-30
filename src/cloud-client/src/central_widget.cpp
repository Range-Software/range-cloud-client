#include <QApplication>
#include <QVBoxLayout>
#include <QScrollBar>

#include <QTabWidget>
#include <QTabBar>

#include <rgl_cloud_file_manager_widget.h>
#include <rgl_logger_handler.h>
#include <rgl_message_box.h>

#include "application.h"
#include "central_widget.h"
#include "cloud_user_manager_widget.h"

CentralWidget::CentralWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->tabWidget = new QTabWidget;
    this->tabWidget->setTabPosition(QTabWidget::South);
    mainLayout->addWidget(this->tabWidget);

    RCloudFileManagerWidget *cloudFileManager = new RCloudFileManagerWidget(qobject_cast<Application*>(Application::instance())->getCloudConnectionHandler(),
                                                                            qobject_cast<Application*>(Application::instance())->getApplicationSettings(),
                                                                            true);
    this->fileManasgerTabPosition = this->tabWidget->addTab(cloudFileManager,QString());

    CloudUserManagerWidget *cloudUserManager = new CloudUserManagerWidget;
    this->userManagerTabPosition = this->tabWidget->addTab(cloudUserManager,QString());

    this->applicationOutputBrowser = new RTextBrowser(true);
    this->outputTabPosition = this->tabWidget->addTab(this->applicationOutputBrowser,QString());

    this->setTabTitle(CentralWidget::FileManager);
    this->setTabTitle(CentralWidget::UserManager);
    this->setTabTitle(CentralWidget::Output);

    QObject::connect(this->tabWidget,&QTabWidget::currentChanged,this,&CentralWidget::onCurrentChanged);

    // Logger signals
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::infoPrinted,this,&CentralWidget::onInfoPrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::noticePrinted,this,&CentralWidget::onNoticePrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::warningPrinted,this,&CentralWidget::onWarningPrinted);
    QObject::connect(&RLoggerHandler::getInstance(),&RLoggerHandler::errorPrinted,this,&CentralWidget::onErrorPrinted);
}

void CentralWidget::setTabTitle(TabType tabType, RMessage::Type messageType, const QString &additionalText)
{
    int tabPosition = 0;
    if (tabType == CentralWidget::FileManager)
    {
        tabPosition = this->fileManasgerTabPosition;
    }
    else if (tabType == CentralWidget::UserManager)
    {
        tabPosition = this->userManagerTabPosition;
    }
    else if (tabType == CentralWidget::Output)
    {
        tabPosition = this->outputTabPosition;
    }
    else
    {
        return;
    }

    QString defaultText = CentralWidget::getTabName(tabType);
    if (!additionalText.isEmpty())
    {
        defaultText += " (" + additionalText + ")";
    }

    QColor tabTextColor = QApplication::palette().text().color();
    if (messageType == RMessage::Type::Info)
    {
        defaultText.prepend("* ");
        tabTextColor = Qt::darkCyan;
    }
    else if (messageType == RMessage::Type::Warning)
    {
        defaultText.prepend("! ");
        tabTextColor = Qt::red;
    }
    else if (messageType == RMessage::Type::Error)
    {
        defaultText.prepend("!!! ");
        defaultText.append(" !!!");
        tabTextColor = Qt::red;
    }

    this->tabWidget->tabBar()->setTabTextColor(tabPosition,tabTextColor);
    this->tabWidget->setTabText(tabPosition,defaultText);
}

void CentralWidget::onInfoPrinted(const RMessage &message)
{
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    int scrollBarValue = sb->value();
    bool scrollBarAtMax = ((sb->maximum() - scrollBarValue) <= 10);

    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextBackgroundColor(QApplication::palette().base().color());
    this->applicationOutputBrowser->setTextColor(QApplication::palette().text().color());
    this->applicationOutputBrowser->insertPlainText(RMessage::messageToLogString(message));
    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    if (scrollBarAtMax)
    {
        scrollBarValue = sb->maximum();
    }
    sb->setValue(scrollBarValue);
    this->setTabTitle(CentralWidget::Output,RMessage::Type::Info);
}

void CentralWidget::onNoticePrinted(const RMessage &message)
{
    RMessageBox::information(this,tr("Notice"),QString(RMessage::messageToLogString(message)).replace("NOTICE: ",""));
}

void CentralWidget::onWarningPrinted(const RMessage &message)
{
    QTextCharFormat charFormat = this->applicationOutputBrowser->currentCharFormat();

    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextBackgroundColor(QApplication::palette().base().color());
    this->applicationOutputBrowser->setTextColor(QColor(170,0,0));
    this->applicationOutputBrowser->insertPlainText(RMessage::messageToLogString(message));
    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextColor(QApplication::palette().text().color());
    this->applicationOutputBrowser->setCurrentCharFormat(charFormat);
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
    this->setTabTitle(CentralWidget::Output,RMessage::Type::Warning);
}

void CentralWidget::onErrorPrinted(const RMessage &message)
{
    QTextCharFormat charFormat = this->applicationOutputBrowser->currentCharFormat();

    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextBackgroundColor(QColor(170,0,0));
    this->applicationOutputBrowser->setTextColor(QColor(255,255,255));
    this->applicationOutputBrowser->insertPlainText(RMessage::messageToLogString(message));
    this->applicationOutputBrowser->moveCursor(QTextCursor::End);
    this->applicationOutputBrowser->setTextBackgroundColor(QApplication::palette().base().color());
    this->applicationOutputBrowser->setTextColor(QApplication::palette().text().color());
    this->applicationOutputBrowser->setCurrentCharFormat(charFormat);
    QScrollBar *sb = this->applicationOutputBrowser->verticalScrollBar();
    sb->setValue(sb->maximum());
    this->setTabTitle(CentralWidget::Output,RMessage::Type::Error);
}

void CentralWidget::onCurrentChanged(int tabPosition)
{
    if (tabPosition == this->outputTabPosition)
    {
        this->setTabTitle(CentralWidget::Output);
    }
}

QString CentralWidget::getTabName(TabType tabType)
{
    if (tabType == CentralWidget::FileManager)
    {
        return tr("Files");
    }
    else if (tabType == CentralWidget::UserManager)
    {
        return tr("Users");
    }
    else if (tabType == CentralWidget::Output)
    {
        return tr("Output");
    }
    else
    {
        return QString();
    }
}
