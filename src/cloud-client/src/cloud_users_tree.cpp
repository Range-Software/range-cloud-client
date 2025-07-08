#include <QVBoxLayout>
#include <QTreeWidgetItem>
#include <QTimer>
#include <QMenu>

#include <rbl_logger.h>

#include <rgl_message_box.h>

#include "application.h"
#include "cloud_auth_token_manager_dialog.h"
#include "cloud_user_add_dialog.h"
#include "cloud_user_groups_dialog.h"
#include "cloud_users_tree.h"

CloudUsersTree::CloudUsersTree(QWidget *parent)
    : QWidget{parent}
{
    this->cloudClient = qobject_cast<Application*>(Application::instance())->getCloudConnectionHandler()->createPrivateClient(this);
    this->cloudClient->setBlocking(false);

    QIcon addIcon(":/icons/cloud/pixmaps/range-add.svg");
    QIcon removeIcon(":/icons/cloud/pixmaps/range-remove.svg");
    QIcon editIcon(":/icons/cloud/pixmaps/range-edit.svg");
    QIcon refreshIcon(":/icons/cloud/pixmaps/range-refresh.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    QTreeWidgetItem *headerItem = new QTreeWidgetItem();
    for (int type=ColumnName;type<NunberOfColumns;type++)
    {
        headerItem->setText(type,CloudUsersTree::getColumnName(ColumnType(type)));
    }

    this->usersTree = new QTreeWidget;
    this->usersTree->setColumnCount(NunberOfColumns);
    this->usersTree->setSortingEnabled(true);
    this->usersTree->setHeaderItem(headerItem);
    mainLayout->addWidget(this->usersTree);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);

    this->addButton = new QPushButton(addIcon,tr("Add"));
    buttonsLayout->addWidget(this->addButton);

    this->editButton = new QPushButton(editIcon,tr("Edit"));
    this->editButton->setDisabled(true);
    buttonsLayout->addWidget(this->editButton);

    QMenu *editMenu = new QMenu;
    this->editButton->setMenu(editMenu);

    QAction *editGroupsAction = new QAction(tr("Edit groups"));
    editMenu->addAction(editGroupsAction);

    QAction *editTokensAction = new QAction(tr("Edit tokens"));
    editMenu->addAction(editTokensAction);

    buttonsLayout->addWidget(new QWidget, 1);

    this->refreshButton = new QPushButton(refreshIcon,tr("Refresh"));
    buttonsLayout->addWidget(this->refreshButton);

    buttonsLayout->addWidget(new QWidget, 1);

    this->removeButton = new QPushButton(removeIcon,tr("Remove"));
    this->removeButton->setDisabled(true);
    buttonsLayout->addWidget(this->removeButton);

    QObject::connect(this->usersTree,&QTreeWidget::itemSelectionChanged,this,&CloudUsersTree::onSelectionChanged);

    QObject::connect(this->cloudClient,&RCloudClient::configurationChanged,this,&CloudUsersTree::onClientConfigurationChanged);
    QObject::connect(this->cloudClient,&RCloudClient::userListAvailable,this,&CloudUsersTree::onUserListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::userAdded,this,&CloudUsersTree::onUserAdded);
    QObject::connect(this->cloudClient,&RCloudClient::userUpdated,this,&CloudUsersTree::onUserUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::userRemoved,this,&CloudUsersTree::onUserRemoved);

    QObject::connect(this->addButton,&QPushButton::clicked,this,&CloudUsersTree::onAddButtonClicked);
    QObject::connect(this->removeButton,&QPushButton::clicked,this,&CloudUsersTree::onRemoveButtonClicked);
    QObject::connect(this->refreshButton,&QPushButton::clicked,this,&CloudUsersTree::onRefreshButtonClicked);

    QObject::connect(editGroupsAction,&QAction::triggered,this,&CloudUsersTree::onEditGroupsButtonTriggered);
    QObject::connect(editTokensAction,&QAction::triggered,this,&CloudUsersTree::onEditTokensButtonTriggered);

    const RApplicationSettings *applicationSettings = qobject_cast<Application*>(Application::instance())->getApplicationSettings();

    this->refreshTimer = new QTimer(this);
    QObject::connect(this->refreshTimer,&QTimer::timeout,this,&CloudUsersTree::onRefreshTimeout);
    QObject::connect(applicationSettings,&RApplicationSettings::cloudRefreshTimeoutChanged,this,&CloudUsersTree::onRefreshTimeoutChanged);
    this->onRefreshTimeoutChanged(applicationSettings->getCloudRefreshTimeout());

    this->refreshCloudUsers();
}

void CloudUsersTree::populateTree(const QList<RUserInfo> &userInfoList, QTreeWidget *treeWidget)
{
    QList<RUserInfo> _userInfoList = userInfoList;

    for (int i=treeWidget->topLevelItemCount()-1; i>=0; i--)
    {
        const QString userName = treeWidget->topLevelItem(i)->text(ColumnName);
        bool userInList = false;
        for (auto iter = _userInfoList.cbegin(), end = _userInfoList.cend(); iter != end; ++iter)
        {
            if (userName == iter->getName())
            {
                CloudUsersTree::updateTreeItem(treeWidget->topLevelItem(i),*iter);
                _userInfoList.erase(iter);
                userInList = true;
                break;
            }
        }
        if (!userInList)
        {
            treeWidget->takeTopLevelItem(i);
        }
    }
    for (const RUserInfo &userInfo : _userInfoList)
    {
        CloudUsersTree::updateTreeItem(new QTreeWidgetItem(treeWidget),userInfo);
    }
    treeWidget->resizeColumnToContents(ColumnName);
}

void CloudUsersTree::updateTreeItem(QTreeWidgetItem *listItem, const RUserInfo &userInfo)
{
    listItem->setText(ColumnName,userInfo.getName());
    listItem->setText(ColumnGroups,userInfo.getGroupNames().join(','));
}

QString CloudUsersTree::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnName:   return tr("Name");
        case ColumnGroups: return tr("Groups");
        default:           return QString();
    }
}

void CloudUsersTree::refreshCloudUsers()
{
    try
    {
        this->cloudClient->requestListUsers();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of users from Cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("Cloud transfer failed"),tr("List of users from Cloud has failed."));
    }
}

void CloudUsersTree::onGroupListChanged(QList<RGroupInfo> groupInfoList)
{
    this->groupInfoList = groupInfoList;
}

void CloudUsersTree::onUserListAvailable(QList<RUserInfo> userInfoList)
{
    CloudUsersTree::populateTree(userInfoList,this->usersTree);
    emit this->userListChanged(userInfoList);
}

void CloudUsersTree::onUserAdded(RUserInfo userInfo)
{
    RLogger::info("Cloud user was added \'%s\'\n",userInfo.getName().toUtf8().constData());
    RMessageBox::information(this,tr("User was added"),tr("User was added successfully."));
    this->refreshCloudUsers();
}

void CloudUsersTree::onUserUpdated(RUserInfo userInfo)
{
    RLogger::info("Cloud user was updated \'%s\'\n",userInfo.getName().toUtf8().constData());
    RMessageBox::information(this,tr("User was updated"),tr("User was updated successfully."));
    this->refreshCloudUsers();
}

void CloudUsersTree::onUserRemoved(QString userName)
{
    RLogger::info("Cloud user was removed \'%s\'\n",userName.toUtf8().constData());
    RMessageBox::information(this,tr("User was removed"),tr("User was removed successfully."));
    this->refreshCloudUsers();
}

void CloudUsersTree::onAddButtonClicked()
{
    CloudUserAddDialog userAddDialog(this);
    if (userAddDialog.exec() == QDialog::Accepted)
    {
        try
        {
            this->cloudClient->requestUserAdd(userAddDialog.getUserName());
        }
        catch (const RError &rError)
        {
            RLogger::error("Failed to request to add new user to Cloud. %s\n",rError.getMessage().toUtf8().constData());
            RMessageBox::critical(this,tr("Adding new user failed"),tr("Adding new user to Cloud has failed."));
        }
    }
}

void CloudUsersTree::onRemoveButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->usersTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        QString questionText = tr("Are you sure you want to remove user") + " <b>" + selectedItem->text(ColumnName) + "</b>?";
        if (RMessageBox::question(this,tr("Remove user?"),questionText) == RMessageBox::Yes)
        {
            try
            {
                this->cloudClient->requestUserRemove(selectedItem->text(ColumnName));
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request to remove user from Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("Removing user failed"),tr("Removing user from Cloud has failed."));
            }
        }
    }
}

void CloudUsersTree::onRefreshButtonClicked()
{
    this->refreshCloudUsers();
}

void CloudUsersTree::onEditGroupsButtonTriggered()
{
    QList<QTreeWidgetItem*> selectedItems = this->usersTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RUserInfo userInfo;
        userInfo.setName(selectedItem->text(ColumnName));
        userInfo.setGroupNames(selectedItem->text(ColumnGroups).split(','));

        CloudUserGroupsDialog userGroupsDialog(userInfo,this->groupInfoList,this);

        if (userGroupsDialog.exec() == RMessageBox::Accepted)
        {
            try
            {
                RUserInfo userInfo = userGroupsDialog.getUserInfo();
                this->cloudClient->requestUserUpdate(userInfo.getName(),userInfo);
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request to update user on Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("Updating user failed"),tr("Updating user on Cloud has failed."));
            }
        }
    }
}

void CloudUsersTree::onEditTokensButtonTriggered()
{
    QList<QTreeWidgetItem*> selectedItems = this->usersTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        CloudAuthTokenManagerDialog dialog(selectedItem->text(ColumnName),this);
        dialog.exec();
    }
}

void CloudUsersTree::onRefreshTimeout()
{
    this->refreshCloudUsers();
}

void CloudUsersTree::onRefreshTimeoutChanged(uint refreshTimeout)
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

void CloudUsersTree::onClientConfigurationChanged()
{
    this->refreshCloudUsers();
}

void CloudUsersTree::onSelectionChanged()
{
    uint nSelected = this->usersTree->selectedItems().count();
    this->editButton->setEnabled(nSelected);
    this->removeButton->setEnabled(nSelected);
}
