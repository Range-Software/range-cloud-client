#include <QVBoxLayout>
#include <QFormLayout>
#include <QListWidget>
#include <QDialogButtonBox>

#include <rcl_user_info.h>

#include "cloud_user_groups_dialog.h"

CloudUserGroupsDialog::CloudUserGroupsDialog(const RUserInfo &userInfo, const QList<RGroupInfo> &groupInfoList, QWidget *parent)
    : QDialog{parent}
    , userInfo{userInfo}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-add.svg");

    this->setWindowTitle(tr("Set user groups"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QListWidget *listWidget = new QListWidget;
    mainLayout->addWidget(listWidget);

    for (const RGroupInfo &groupInfo : groupInfoList)
    {
        QListWidgetItem *listItem = new QListWidgetItem(groupInfo.getName(),listWidget);
        listItem->setCheckState(this->userInfo.getGroupNames().contains(groupInfo.getName()) ? Qt::Checked : Qt::Unchecked);
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->okButton = new QPushButton(okIcon, tr("OK"));
    this->okButton->setDisabled(true);
    buttonBox->addButton(this->okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(listWidget,&QListWidget::itemChanged,this,&CloudUserGroupsDialog::onListItemChanged);
}

void CloudUserGroupsDialog::onListItemChanged(QListWidgetItem *item)
{
    if (item->checkState() == Qt::Checked)
    {
        this->userInfo.getGroupNames().append(item->text());
    }
    else
    {
        this->userInfo.getGroupNames().removeAll(item->text());
    }
    this->okButton->setEnabled(true);
}
