#include <QVBoxLayout>
#include <QMenu>
#include <QTimer>

#include <rbl_logger.h>
#include <rgl_text_browser_dialog.h>

#include <rgl_access_owner_dialog.h>
#include <rgl_access_mode_dialog.h>
#include <rgl_message_box.h>

#include "application.h"
#include "cloud_process_execute_dialog.h"
#include "cloud_processes_tree.h"

CloudProcessesTree::CloudProcessesTree(QWidget *parent)
    : QWidget{parent}
{
    this->cloudClient = qobject_cast<Application*>(Application::instance())->getCloudConnectionHandler()->createPrivateClient(this);
    this->cloudClient->setBlocking(false);

    QIcon editIcon(":/icons/cloud/pixmaps/range-edit.svg");
    QIcon refreshIcon(":/icons/cloud/pixmaps/range-refresh.svg");
    QIcon executeIcon(":/icons/cloud/pixmaps/range-execute.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    QTreeWidgetItem *headerItem = new QTreeWidgetItem();
    for (int type=ColumnName;type<NunberOfColumns;type++)
    {
        headerItem->setText(type,CloudProcessesTree::getColumnName(ColumnType(type)));
    }

    this->processesTree = new QTreeWidget;
    this->processesTree->setColumnCount(NunberOfColumns);
    this->processesTree->setSortingEnabled(true);
    this->processesTree->setHeaderItem(headerItem);
    mainLayout->addWidget(this->processesTree);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);

    this->editButton = new QPushButton(editIcon,tr("Edit"));
    this->editButton->setDisabled(true);
    buttonsLayout->addWidget(this->editButton);

    QMenu *editMenu = new QMenu;
    this->editButton->setMenu(editMenu);

    QAction *cloudAccessModeProcess = new QAction(tr("Access mode"));
    editMenu->addAction(cloudAccessModeProcess);

    QAction *cloudAccessOwnerProcess = new QAction(tr("Owner"));
    editMenu->addAction(cloudAccessOwnerProcess);

    buttonsLayout->addWidget(new QWidget, 1);

    this->refreshButton = new QPushButton(refreshIcon,tr("Refresh"));
    buttonsLayout->addWidget(this->refreshButton);

    buttonsLayout->addWidget(new QWidget, 1);

    this->executeButton = new QPushButton(executeIcon,tr("Execute"));
    buttonsLayout->addWidget(this->executeButton);

    QObject::connect(this->processesTree,&QTreeWidget::itemSelectionChanged,this,&CloudProcessesTree::onSelectionChanged);

    QObject::connect(this->cloudClient,&RCloudClient::configurationChanged,this,&CloudProcessesTree::onClientConfigurationChanged);
    QObject::connect(this->cloudClient,&RCloudClient::processListAvailable,this,&CloudProcessesTree::onProcessListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::processAccessOwnerUpdated,this,&CloudProcessesTree::onProcessAccessOwnerUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::processAccessModeUpdated,this,&CloudProcessesTree::onProcessAccessModeUpdated);
    QObject::connect(this->cloudClient,&RCloudClient::processFinished,this,&CloudProcessesTree::onProcessFinished);

    QObject::connect(this->refreshButton,&QPushButton::clicked,this,&CloudProcessesTree::onRefreshButtonClicked);
    QObject::connect(this->executeButton,&QPushButton::clicked,this,&CloudProcessesTree::onExecuteButtonClicked);

    QObject::connect(cloudAccessModeProcess,&QAction::triggered,this,&CloudProcessesTree::onAccessModeProcessTriggered);
    QObject::connect(cloudAccessOwnerProcess,&QAction::triggered,this,&CloudProcessesTree::onAccessOwnerProcessTriggered);

    const RApplicationSettings *applicationSettings = qobject_cast<Application*>(Application::instance())->getApplicationSettings();

    this->refreshTimer = new QTimer(this);
    QObject::connect(this->refreshTimer,&QTimer::timeout,this,&CloudProcessesTree::onRefreshTimeout);
    QObject::connect(applicationSettings,&RApplicationSettings::cloudRefreshTimeoutChanged,this,&CloudProcessesTree::onRefreshTimeoutChanged);
    this->onRefreshTimeoutChanged(applicationSettings->getCloudRefreshTimeout());

    this->refreshCloudProcesses();
}

void CloudProcessesTree::populateTree(const QList<RCloudProcessInfo> &processInfoList, QTreeWidget *treeWidget)
{
    QList<RCloudProcessInfo> _processInfoList = processInfoList;

    for (int i=treeWidget->topLevelItemCount()-1; i>=0; i--)
    {
        const QString processName = treeWidget->topLevelItem(i)->text(ColumnName);
        bool processInList = false;
        for (auto iter = _processInfoList.cbegin(), end = _processInfoList.cend(); iter != end; ++iter)
        {
            if (processName == iter->getName())
            {
                CloudProcessesTree::updateTreeItem(treeWidget->topLevelItem(i),*iter);
                _processInfoList.erase(iter);
                processInList = true;
                break;
            }
        }
        if (!processInList)
        {
            treeWidget->takeTopLevelItem(i);
        }
    }
    for (const RCloudProcessInfo &processInfo : _processInfoList)
    {
        CloudProcessesTree::updateTreeItem(new QTreeWidgetItem(treeWidget),processInfo);
    }
    treeWidget->resizeColumnToContents(ColumnName);
}

void CloudProcessesTree::updateTreeItem(QTreeWidgetItem *listItem, const RCloudProcessInfo &processInfo)
{
    listItem->setData(ColumnName,Qt::UserRole,QVariant(processInfo.getArguments()));
    listItem->setText(ColumnName,processInfo.getName());
    listItem->setText(ColumnAccessMode,processInfo.getAccessRights().getMode().toHuman());
    listItem->setText(ColumnAccessOwner,processInfo.getAccessRights().getOwner().toString(':'));
}

QString CloudProcessesTree::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnName:             return tr("Name");
        case ColumnAccessMode:       return tr("Access rights");
        case ColumnAccessOwner:      return tr("Owner");
        default:                     return QString();
    }
}

void CloudProcessesTree::refreshCloudProcesses()
{
    try
    {
        this->cloudClient->requestListProcesses();
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of processes from Cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("Cloud transfer failed"),tr("List of processes from Cloud has failed."));
    }
}

void CloudProcessesTree::onUserListChanged(QList<RUserInfo> userInfoList)
{
    this->userInfoList = userInfoList;
}

void CloudProcessesTree::onGroupListChanged(QList<RGroupInfo> groupInfoList)
{
    this->groupInfoList = groupInfoList;
}

void CloudProcessesTree::onProcessListAvailable(QList<RCloudProcessInfo> processInfoList)
{
    CloudProcessesTree::populateTree(processInfoList,this->processesTree);
}

void CloudProcessesTree::onProcessAccessOwnerUpdated(RCloudProcessInfo)
{
    this->refreshCloudProcesses();
}

void CloudProcessesTree::onProcessAccessModeUpdated(RCloudProcessInfo)
{
    this->refreshCloudProcesses();
}

void CloudProcessesTree::onProcessFinished(QString message)
{
    RTextBrowserDialog textBrowserDialog(tr("Process output"),tr("Process has finished"),message,this);
    textBrowserDialog.exec();
}

void CloudProcessesTree::onAccessOwnerProcessTriggered()
{
    QList<QTreeWidgetItem*> selectedItems = this->processesTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RCloudProcessInfo processInfo;
        processInfo.setName(selectedItem->text(ColumnName));

        RAccessRights accessRights;
        accessRights.setOwner(RAccessOwner::fromString(selectedItem->text(ColumnAccessOwner),':'));

        processInfo.setAccessRights(accessRights);

        RAccessOwnerDialog cloudAccessOwnerDialog(QUuid(),processInfo.getName(),processInfo.getAccessRights().getOwner(),this);
        if (cloudAccessOwnerDialog.exec() == QDialog::Accepted)
        {
            try
            {
                this->cloudClient->requestProcessUpdateAccessOwner(processInfo.getName(),cloudAccessOwnerDialog.getAccessOwner());
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request process access owner update in Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("Process access owner update failed"),tr("Process access owner update in Cloud has failed."));
            }
        }
    }
}

void CloudProcessesTree::onAccessModeProcessTriggered()
{
    QList<QTreeWidgetItem*> selectedItems = this->processesTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RCloudProcessInfo processInfo;
        processInfo.setName(selectedItem->text(ColumnName));

        RAccessRights accessRights;
        accessRights.setMode(RAccessMode::fromHuman(selectedItem->text(ColumnAccessMode)));

        processInfo.setAccessRights(accessRights);

        RAccessModeDialog cloudAccessModeDialog(QUuid(),processInfo.getName(),processInfo.getAccessRights().getMode(),RAccessMode::X,this);
        if (cloudAccessModeDialog.exec() == QDialog::Accepted)
        {
            try
            {
                this->cloudClient->requestProcessUpdateAccessMode(processInfo.getName(),cloudAccessModeDialog.getAccessMode());
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request process access owner update in Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("Process access mode update failed"),tr("Process access mode update in Cloud has failed."));
            }
        }
    }
}

void CloudProcessesTree::onRefreshButtonClicked()
{
    this->refreshCloudProcesses();
}

void CloudProcessesTree::onRefreshTimeout()
{
    this->refreshCloudProcesses();
}

void CloudProcessesTree::onRefreshTimeoutChanged(uint refreshTimeout)
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

void CloudProcessesTree::onClientConfigurationChanged()
{
    this->refreshCloudProcesses();
}

void CloudProcessesTree::onExecuteButtonClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->processesTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        RCloudProcessInfo processInfo;
        processInfo.setName(selectedItem->text(ColumnName));
        processInfo.setArguments(selectedItem->data(ColumnName,Qt::UserRole).toStringList());
        CloudProcessExecuteDialog processExecuteDialog(processInfo,this);
        processExecuteDialog.exec();

        RCloudProcessRequest processRequest;
        processRequest.setName(processInfo.getName());
        processRequest.setArgumentValues(processExecuteDialog.getArguments());

        try
        {
            this->cloudClient->requestProcess(processRequest);
        }
        catch (const RError &rError)
        {
            RLogger::error("Failed to request process in Cloud. %s\n",rError.getMessage().toUtf8().constData());
            RMessageBox::critical(this,tr("Process has failed"),tr("Process in Cloud has failed."));
        }
    }
}

void CloudProcessesTree::onSelectionChanged()
{
    uint nSelected = this->processesTree->selectedItems().count();
    this->editButton->setEnabled(nSelected);
    this->executeButton->setEnabled(nSelected);
}
