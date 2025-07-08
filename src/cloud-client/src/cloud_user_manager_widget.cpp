#include <QVBoxLayout>
#include <QSplitter>
#include <QGroupBox>

#include "cloud_user_manager_widget.h"
#include "cloud_actions_tree.h"
#include "cloud_processes_tree.h"
#include "cloud_users_tree.h"
#include "cloud_groups_tree.h"

CloudUserManagerWidget::CloudUserManagerWidget(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QSplitter *verticalSplitter = new QSplitter(Qt::Horizontal);
    verticalSplitter->setChildrenCollapsible(false);
    mainLayout->addWidget(verticalSplitter);

    // Actions

    QGroupBox *actionsWidget = new QGroupBox(tr("Actions"));
    verticalSplitter->addWidget(actionsWidget);

    QVBoxLayout *actionsLayout = new QVBoxLayout;
    actionsWidget->setLayout(actionsLayout);

    CloudActionsTree *actionsTree = new CloudActionsTree;
    actionsLayout->addWidget(actionsTree);

    // Processes

    QGroupBox *processesWidget = new QGroupBox(tr("Processes"));
    verticalSplitter->addWidget(processesWidget);

    QVBoxLayout *processesLayout = new QVBoxLayout;
    processesWidget->setLayout(processesLayout);

    CloudProcessesTree *processesTree = new CloudProcessesTree;
    processesLayout->addWidget(processesTree);

    // Users

    QGroupBox *usersWidget = new QGroupBox(tr("Users"));
    verticalSplitter->addWidget(usersWidget);

    QVBoxLayout *usersLayout = new QVBoxLayout;
    usersWidget->setLayout(usersLayout);

    CloudUsersTree *usersTree = new CloudUsersTree;
    usersLayout->addWidget(usersTree);

    // Groups

    QGroupBox *groupsWidget = new QGroupBox(tr("User groups"));
    verticalSplitter->addWidget(groupsWidget);

    QVBoxLayout *groupsLayout = new QVBoxLayout;
    groupsWidget->setLayout(groupsLayout);

    CloudGroupsTree *groupsTree = new CloudGroupsTree;
    groupsLayout->addWidget(groupsTree);

    QObject::connect(usersTree,&CloudUsersTree::userListChanged,actionsTree,&CloudActionsTree::onUserListChanged);
    QObject::connect(groupsTree,&CloudGroupsTree::groupListChanged,actionsTree,&CloudActionsTree::onGroupListChanged);
    QObject::connect(groupsTree,&CloudGroupsTree::groupListChanged,usersTree,&CloudUsersTree::onGroupListChanged);
}
