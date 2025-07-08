#include <QVBoxLayout>
#include <QSplitter>
#include <QListWidgetItem>

#include <rgl_cloud_session_widget.h>

#include "cloud_session_manager_widget.h"
#include "cloud_session_list_widget.h"

CloudSessionManagerWidget::CloudSessionManagerWidget(RCloudSessionManager *session,
                                                     RCloudConnectionHandler *connectionHandler,
                                                     RApplicationSettings *applicationSettings,
                                                     QWidget *parent)
    : QWidget{parent}
    , session{session}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *splitter = new QSplitter(Qt::Horizontal);
    mainLayout->addWidget(splitter);

    CloudSessionListWidget *sessionList = new CloudSessionListWidget(session);
    splitter->addWidget(sessionList);

    RCloudSessionWidget *sessionWidget = new RCloudSessionWidget(RCloudSessionInfo(),connectionHandler,applicationSettings);
    splitter->addWidget(sessionWidget);

    QObject::connect(sessionList,&CloudSessionListWidget::sessionSelected,sessionWidget,&RCloudSessionWidget::setSessionInfo);
    QObject::connect(sessionWidget,&RCloudSessionWidget::sessionInfoChanged,this->session,&RCloudSessionManager::insertSession);

    sessionList->selectSession(this->session->findActiveSessionName());
}
