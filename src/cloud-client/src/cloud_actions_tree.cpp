#include <QVBoxLayout>
#include <QMenu>
#include <QTimer>

#include <rbl_logger.h>

#include <rgl_access_owner_dialog.h>
#include <rgl_access_mode_dialog.h>
#include <rgl_message_box.h>

#include "application.h"
#include "cloud_actions_tree.h"

CloudActionsTree::CloudActionsTree(QWidget *parent)
    : QWidget{parent}
{
    this->cloudClient = qobject_cast<Application*>(Application::instance())->getCloudConnectionHandler()->createPrivateClient(this);
    this->cloudClient->setBlocking(false);

    QIcon editIcon(":/icons/cloud/pixmaps/range-edit.svg");
    QIcon refreshIcon(":/icons/cloud/pixmaps/range-refresh.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    QTreeWidgetItem *headerItem = new QTreeWidgetItem();
    for (int type=ColumnName;type<NunberOfColumns;type++)
    {
        headerItem->setText(type,CloudActionsTree::getColumnName(ColumnType(type)));
    }

    this->actionsTree = new QTreeWidget;
    this->actionsTree->setColumnCount(NunberOfColumns);
    this->actionsTree->setSortingEnabled(true);
    this->actionsTree->setHeaderItem(headerItem);
    mainLayout->addWidget(this->actionsTree);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);

    this->editButton = new QPushButton(editIcon,tr("Edit"));
    this->editButton->setDisabled(true);
    buttonsLayout->addWidget(this->editButton);

    QMenu *editMenu = new QMenu;
    this->editButton->setMenu(editMenu);

    QAction *cloudAccessModeAction = new QAction(tr("Access mode"));
    editMenu->addAction(cloudAccessModeAction);

    QAction *cloudAccessOwnerAction = new QAction(tr("Owner"));
    editMenu->addAction(cloudAccessOwnerAction);

    buttonsLayout->addWidget(new QWidget, 1);

    this->refreshButton = new QPushButton(refreshIcon,tr("Refresh"));
    buttonsLayout->addWidget(this->refreshButton);

    buttonsLayout->addWidget(new QWidget, 1);

    QObject::connect(this->actionsTree,&QTreeWidget::itemSelectionChanged,this,&CloudActionsTree::onSelectionChanged);

    QObject::connect(this->cloudClient,&RCloudClient::configurationChanged,this,&CloudActionsTree::onClientConfigurationChanged);
    QObject::connect(this->cloudClient,&RCloudClient::actionListAvailable,this,&CloudActionsTree::onActionListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::actionAccessOwnerUpdated,this,&CloudActionsTree::onActionAccessOwnerUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::actionAccessModeUpdated,this,&CloudActionsTree::onActionAccessModeUpdated);

    QObject::connect(this->refreshButton,&QPushButton::clicked,this,&CloudActionsTree::onRefreshButtonClicked);

    QObject::connect(cloudAccessModeAction,&QAction::triggered,this,&CloudActionsTree::onAccessModeActionTriggered);
    QObject::connect(cloudAccessOwnerAction,&QAction::triggered,this,&CloudActionsTree::onAccessOwnerActionTriggered);

    const RApplicationSettings *applicationSettings = qobject_cast<Application*>(Application::instance())->getApplicationSettings();

    this->refreshTimer = new QTimer(this);
    QObject::connect(this->refreshTimer,&QTimer::timeout,this,&CloudActionsTree::onRefreshTimeout);
    QObject::connect(applicationSettings,&RApplicationSettings::cloudRefreshTimeoutChanged,this,&CloudActionsTree::onRefreshTimeoutChanged);
    this->onRefreshTimeoutChanged(applicationSettings->getCloudRefreshTimeout());

    this->refreshCloudActions();
}

void CloudActionsTree::populateTree(const QList<RCloudActionInfo> &actionInfoList, QTreeWidget *treeWidget)
{
    QList<RCloudActionInfo> _actionInfoList = actionInfoList;

    for (int i=treeWidget->topLevelItemCount()-1; i>=0; i--)
    {
        const QString actionName = treeWidget->topLevelItem(i)->text(ColumnName);
        bool actionInList = false;
        for (auto iter = _actionInfoList.cbegin(), end = _actionInfoList.cend(); iter != end; ++iter)
        {
            if (actionName == iter->getName())
            {
                CloudActionsTree::updateTreeItem(treeWidget->topLevelItem(i),*iter);
                _actionInfoList.erase(iter);
                actionInList = true;
                break;
            }
        }
        if (!actionInList)
        {
            treeWidget->takeTopLevelItem(i);
        }
    }
    for (const RCloudActionInfo &actionInfo : _actionInfoList)
    {
        CloudActionsTree::updateTreeItem(new QTreeWidgetItem(treeWidget),actionInfo);
    }
    treeWidget->resizeColumnToContents(ColumnName);
}

void CloudActionsTree::updateTreeItem(QTreeWidgetItem *listItem, const RCloudActionInfo &actionInfo)
{
    listItem->setText(ColumnName,actionInfo.getName());
    listItem->setText(ColumnAccessMode,actionInfo.getAccessRights().getMode().toHuman());
    listItem->setText(ColumnAccessOwner,actionInfo.getAccessRights().getOwner().toString(':'));
}

QString CloudActionsTree::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnName:             return tr("Name");
        case ColumnAccessMode:       return tr("Access rights");
        case ColumnAccessOwner:      return tr("Owner");
        default:                     return QString();
    }
}

void CloudActionsTree::refreshCloudActions()
{
    try
    {
        this->cloudClient->requestListActions();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of actions from Cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("Cloud transfer failed"),tr("List of actions from Cloud has failed."));
    }
}

void CloudActionsTree::onUserListChanged(QList<RUserInfo> userInfoList)
{
    this->userInfoList = userInfoList;
}

void CloudActionsTree::onGroupListChanged(QList<RGroupInfo> groupInfoList)
{
    this->groupInfoList = groupInfoList;
}

void CloudActionsTree::onActionListAvailable(QList<RCloudActionInfo> actionInfoList)
{
    CloudActionsTree::populateTree(actionInfoList,this->actionsTree);
}

void CloudActionsTree::onActionAccessOwnerUpdated(RCloudActionInfo)
{
    this->refreshCloudActions();
}

void CloudActionsTree::onActionAccessModeUpdated(RCloudActionInfo)
{
    this->refreshCloudActions();
}

void CloudActionsTree::onAccessOwnerActionTriggered()
{
    QList<QTreeWidgetItem*> selectedItems = this->actionsTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RCloudActionInfo actionInfo;
        actionInfo.setName(selectedItem->text(ColumnName));

        RAccessRights accessRights;
        accessRights.setOwner(RAccessOwner::fromString(selectedItem->text(ColumnAccessOwner),':'));

        actionInfo.setAccessRights(accessRights);

        RAccessOwnerDialog cloudAccessOwnerDialog(QUuid(),actionInfo.getName(),actionInfo.getAccessRights().getOwner(),this);
        if (cloudAccessOwnerDialog.exec() == QDialog::Accepted)
        {
            try
            {
                this->cloudClient->requestActionUpdateAccessOwner(actionInfo.getName(),cloudAccessOwnerDialog.getAccessOwner());
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request action access owner update in Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("Action access owner update failed"),tr("Action access owner update in Cloud has failed."));
            }
        }
    }
}

void CloudActionsTree::onAccessModeActionTriggered()
{
    QList<QTreeWidgetItem*> selectedItems = this->actionsTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RCloudActionInfo actionInfo;
        actionInfo.setName(selectedItem->text(ColumnName));

        RAccessRights accessRights;
        accessRights.setMode(RAccessMode::fromHuman(selectedItem->text(ColumnAccessMode)));

        actionInfo.setAccessRights(accessRights);

        RAccessModeDialog cloudAccessModeDialog(QUuid(),actionInfo.getName(),actionInfo.getAccessRights().getMode(),RAccessMode::X,this);
        if (cloudAccessModeDialog.exec() == QDialog::Accepted)
        {
            try
            {
                this->cloudClient->requestActionUpdateAccessMode(actionInfo.getName(),cloudAccessModeDialog.getAccessMode());
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request action access owner update in Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("Action access mode update failed"),tr("Action access mode update in Cloud has failed."));
            }
        }
    }
}

void CloudActionsTree::onRefreshButtonClicked()
{
    this->refreshCloudActions();
}

void CloudActionsTree::onRefreshTimeout()
{
    this->refreshCloudActions();
}

void CloudActionsTree::onRefreshTimeoutChanged(uint refreshTimeout)
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

void CloudActionsTree::onClientConfigurationChanged()
{
    this->refreshCloudActions();
}

void CloudActionsTree::onSelectionChanged()
{
    uint nSelected = this->actionsTree->selectedItems().count();
    this->editButton->setEnabled(nSelected);
}
