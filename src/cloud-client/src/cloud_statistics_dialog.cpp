#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextBrowser>
#include <QDialogButtonBox>

#include <rbl_logger.h>

#include <rgl_message_box.h>

#include "application.h"
#include "cloud_statistics_dialog.h"

CloudStatisticsDialog::CloudStatisticsDialog(QWidget *parent)
    : QDialog(parent)
{
    this->cloudClient = qobject_cast<Application*>(Application::instance())->getCloudConnectionHandler()->createPrivateClient(this);
    this->cloudClient->setBlocking(false);

    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");
    QIcon refreshIcon(":/icons/cloud/pixmaps/range-refresh.svg");

    this->resize(700,500);

    this->setWindowTitle(tr("Cloud service statistics"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QLabel *titleLabel = new QLabel();
    mainLayout->addWidget(titleLabel);

    this->textBrowser = new RTextBrowser(false,this);
    mainLayout->addWidget(this->textBrowser);

    QPushButton *refreshButton = new QPushButton(refreshIcon,tr("Refresh"));
    mainLayout->addWidget(refreshButton);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);

    QObject::connect(refreshButton,&QPushButton::clicked,this,&CloudStatisticsDialog::onRefreshButtonClicked);

    QObject::connect(this->cloudClient,&RCloudClient::configurationChanged,this,&CloudStatisticsDialog::onClientConfigurationChanged);
    QObject::connect(this->cloudClient,&RCloudClient::statisticsAvailable,this,&CloudStatisticsDialog::onStatisticsAvailable);

    const RApplicationSettings *applicationSettings = qobject_cast<Application*>(Application::instance())->getApplicationSettings();

    this->refreshTimer = new QTimer(this);
    QObject::connect(this->refreshTimer,&QTimer::timeout,this,&CloudStatisticsDialog::onRefreshTimeout);
    QObject::connect(applicationSettings,&RApplicationSettings::cloudRefreshTimeoutChanged,this,&CloudStatisticsDialog::onRefreshTimeoutChanged);
    this->onRefreshTimeoutChanged(applicationSettings->getCloudRefreshTimeout());

    this->requestStatistics();
}

void CloudStatisticsDialog::requestStatistics()
{
    try
    {
        this->cloudClient->requestStatistics();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request statistics from Cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("Cloud transfer failed"),tr("List of processes from Cloud has failed."));
    }
}

void CloudStatisticsDialog::onRefreshButtonClicked()
{
    this->requestStatistics();
}

void CloudStatisticsDialog::onRefreshTimeout()
{
    this->requestStatistics();
}

void CloudStatisticsDialog::onRefreshTimeoutChanged(uint refreshTimeout)
{
    if (refreshTimeout == 0)
    {
        this->refreshTimer->stop();
    }
    else
    {
        this->refreshTimer->start(refreshTimeout);
    }
}

void CloudStatisticsDialog::onClientConfigurationChanged()
{
    this->requestStatistics();
}

void CloudStatisticsDialog::onStatisticsAvailable(const QString &statistics)
{
    QTextCursor textCursor = this->textBrowser->textCursor();
    textCursor.movePosition(QTextCursor::Start);
    this->textBrowser->setTextCursor(textCursor);

    this->textBrowser->insertPlainText(statistics);
}
