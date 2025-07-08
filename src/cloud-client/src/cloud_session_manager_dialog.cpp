#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "cloud_session_manager_dialog.h"
#include "cloud_session_manager_widget.h"

CloudSessionManagerDialog::CloudSessionManagerDialog(RCloudSessionManager *session, RCloudConnectionHandler *connectionHandler, RApplicationSettings *applicationSettings, QWidget *parent)
    : QDialog{parent}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Session manager"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    CloudSessionManagerWidget *cloudSessionManagerWidget = new CloudSessionManagerWidget(session,connectionHandler,applicationSettings);
    mainLayout->addWidget(cloudSessionManagerWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::close);
}
