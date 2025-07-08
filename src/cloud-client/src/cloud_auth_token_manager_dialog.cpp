#include <QVBoxLayout>
#include <QPushButton>
#include <QDialogButtonBox>

#include "cloud_auth_token_manager_dialog.h"
#include "cloud_auth_token_manager_widget.h"

CloudAuthTokenManagerDialog::CloudAuthTokenManagerDialog(const QString &resourceName, QWidget *parent)
    : QDialog{parent}
    , userName{resourceName}
{
    QIcon closeIcon(":/icons/action/pixmaps/range-close.svg");

    this->setWindowTitle(tr("Manage authentication tokens"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    CloudAuthTokenManagerWidget *authTokenManagerWidget = new CloudAuthTokenManagerWidget(resourceName);
    mainLayout->addWidget(authTokenManagerWidget);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *closeButton = new QPushButton(closeIcon, tr("Close"));
    buttonBox->addButton(closeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}
