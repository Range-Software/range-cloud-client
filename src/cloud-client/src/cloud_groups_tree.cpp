#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <QTimer>

#include <rbl_logger.h>

#include <rgl_message_box.h>

#include "application.h"
#include "cloud_group_add_dialog.h"
#include "cloud_groups_tree.h"

CloudGroupsTree::CloudGroupsTree(QWidget *parent)
    : QWidget{parent}
{
    this->cloudClient = qobject_cast<Application*>(Application::instance())->getCloudConnectionHandler()->createPrivateClient(this);
    this->cloudClient->setBlocking(false);

    QIcon addIcon(":/icons/cloud/pixmaps/range-add.svg");
    QIcon removeIcon(":/icons/cloud/pixmaps/range-remove.svg");
    QIcon refreshIcon(":/icons/cloud/pixmaps/range-refresh.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    QTreeWidgetItem *headerItem = new QTreeWidgetItem();
    for (int type=ColumnName;type<NunberOfColumns;type++)
    {
        headerItem->setText(type,CloudGroupsTree::getColumnName(ColumnType(type)));
    }

    this->groupsTree = new QTreeWidget;
    this->groupsTree->setColumnCount(NunberOfColumns);
    this->groupsTree->setSortingEnabled(true);
    this->groupsTree->setHeaderItem(headerItem);
    mainLayout->addWidget(this->groupsTree);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);

    this->addButton = new QPushButton(addIcon,tr("Add"));
    buttonsLayout->addWidget(this->addButton);

    buttonsLayout->addWidget(new QWidget, 1);

    this->refreshButton = new QPushButton(refreshIcon,tr("Refresh"));
    buttonsLayout->addWidget(this->refreshButton);

    buttonsLayout->addWidget(new QWidget, 1);

    this->removeButton = new QPushButton(removeIcon,tr("Remove"));
    this->removeButton->setDisabled(true);
    buttonsLayout->addWidget(this->removeButton);

    QObject::connect(this->groupsTree,&QTreeWidget::itemSelectionChanged,this,&CloudGroupsTree::onSelectionChanged);

    QObject::connect(this->cloudClient,&RCloudClient::configurationChanged,this,&CloudGroupsTree::onClientConfigurationChanged);
    QObject::connect(this->cloudClient,&RCloudClient::groupListAvailable,this,&CloudGroupsTree::onGroupListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::groupAdded,this,&CloudGroupsTree::onGroupAdded);
    QObject::connect(this->cloudClient,&RCloudClient::groupRemoved,this,&CloudGroupsTree::onGroupRemoved);

    QObject::connect(this->addButton,&QPushButton::clicked,this,&CloudGroupsTree::onAddButtonClicked);
    QObject::connect(this->removeButton,&QPushButton::clicked,this,&CloudGroupsTree::onRemoveButtonClicked);
    QObject::connect(this->refreshButton,&QPushButton::clicked,this,&CloudGroupsTree::onRefreshButtonClicked);

    const RApplicationSettings *applicationSettings = qobject_cast<Application*>(Application::instance())->getApplicationSettings();

    this->refreshTimer = new QTimer(this);
    QObject::connect(this->refreshTimer,&QTimer::timeout,this,&CloudGroupsTree::onRefreshTimeout);
    QObject::connect(applicationSettings,&RApplicationSettings::cloudRefreshTimeoutChanged,this,&CloudGroupsTree::onRefreshTimeoutChanged);
    this->onRefreshTimeoutChanged(applicationSettings->getCloudRefreshTimeout());

    this->refreshCloudGroups();
}

void CloudGroupsTree::populateTree(const QList<RGroupInfo> &groupInfoList, QTreeWidget *treeWidget)
{
    QList<RGroupInfo> _groupInfoList = groupInfoList;

    for (int i=treeWidget->topLevelItemCount()-1; i>=0; i--)
    {
        const QString groupName = treeWidget->topLevelItem(i)->text(ColumnName);
        bool groupInList = false;
        for (auto iter = _groupInfoList.cbegin(), end = _groupInfoList.cend(); iter != end; ++iter)
        {
            if (groupName == iter->getName())
            {
                CloudGroupsTree::updateTreeItem(treeWidget->topLevelItem(i),*iter);
                _groupInfoList.erase(iter);
                groupInList = true;
                break;
            }
        }
        if (!groupInList)
        {
            treeWidget->takeTopLevelItem(i);
        }
    }
    for (const RGroupInfo &groupInfo : _groupInfoList)
    {
        CloudGroupsTree::updateTreeItem(new QTreeWidgetItem(treeWidget),groupInfo);
    }
}

void CloudGroupsTree::updateTreeItem(QTreeWidgetItem *listItem, const RGroupInfo &groupInfo)
{
    listItem->setText(ColumnName,groupInfo.getName());
}

QString CloudGroupsTree::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnName: return tr("Name");
        default:         return QString();
    }
}

void CloudGroupsTree::refreshCloudGroups()
{
    try
    {
        this->cloudClient->requestListGroups();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of user groups from Cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("Cloud transfer failed"),tr("Retrieving list of user groups from Cloud has failed."));
    }
}

void CloudGroupsTree::onGroupListAvailable(QList<RGroupInfo> groupInfoList)
{
    CloudGroupsTree::populateTree(groupInfoList,this->groupsTree);
    emit this->groupListChanged(groupInfoList);
}

void CloudGroupsTree::onGroupAdded(RGroupInfo groupInfo)
{
    RLogger::info("User group was added \'%s\'\n",groupInfo.getName().toUtf8().constData());
    RMessageBox::information(this,tr("User group was added"),tr("User group was added successfully."));
    this->refreshCloudGroups();
}

void CloudGroupsTree::onGroupRemoved(QString groupName)
{
    RLogger::info("User group was removed \'%s\'\n",groupName.toUtf8().constData());
    RMessageBox::information(this,tr("User group was removed"),tr("User group was removed successfully."));
    this->refreshCloudGroups();
}

void CloudGroupsTree::onAddButtonClicked()
{
    CloudGroupAddDialog userGroupDialog(this);
    if (userGroupDialog.exec() == QDialog::Accepted)
    {
        try
        {
            this->cloudClient->requestGroupAdd(userGroupDialog.getGroupName());
        }
        catch (const RError &rError)
        {
            RLogger::error("Failed to request to add new user group to Cloud. %s\n",rError.getMessage().toUtf8().constData());
            RMessageBox::critical(this,tr("Adding new user group failed"),tr("Adding new user group to Cloud has failed."));
        }
    }
}

void CloudGroupsTree::onRemoveButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->groupsTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        QString questionText = tr("Are you sure you want to remove user group") + " <b>" + selectedItem->text(ColumnName) + "</b>?";
        if (RMessageBox::question(this,tr("Remove user group?"),questionText) == RMessageBox::Yes)
        {
            try
            {
                this->cloudClient->requestGroupRemove(selectedItem->text(ColumnName));
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request to remove user group from Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("Removing user group failed"),tr("Removing user group from Cloud has failed."));
            }
        }
    }
}

void CloudGroupsTree::onRefreshButtonClicked()
{
    this->refreshCloudGroups();
}

void CloudGroupsTree::onRefreshTimeout()
{
    this->refreshCloudGroups();
}

void CloudGroupsTree::onRefreshTimeoutChanged(uint refreshTimeout)
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

void CloudGroupsTree::onClientConfigurationChanged()
{
    this->refreshCloudGroups();
}

void CloudGroupsTree::onSelectionChanged()
{
    uint nSelected = this->groupsTree->selectedItems().count();
    this->removeButton->setEnabled(nSelected);
}
