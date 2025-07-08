#ifndef CLOUD_GROUPS_TREE_H
#define CLOUD_GROUPS_TREE_H

#include <QTreeWidget>
#include <QPushButton>

#include <rcl_group_info.h>
#include <rcl_cloud_client.h>

class CloudGroupsTree : public QWidget
{
    Q_OBJECT

    protected:

        enum ColumnType
        {
            ColumnName = 0,
            NunberOfColumns
        };

        QTreeWidget *groupsTree;

        QPushButton *addButton;
        QPushButton *refreshButton;
        QPushButton *removeButton;

        RCloudClient *cloudClient;

        QTimer *refreshTimer;

    public:

        //! Constructor.
        explicit CloudGroupsTree(QWidget *parent = nullptr);

    protected:

        //! Populate group tree.
        static void populateTree(const QList<RGroupInfo> &groupInfoList, QTreeWidget *treeWidget);

        //! Populate group tree item.
        static void updateTreeItem(QTreeWidgetItem *listItem, const RGroupInfo &groupInfo);

        //! Return column name for given type.
        static QString getColumnName(ColumnType columnType);

        //! Refresh list of groups.
        void refreshCloudGroups();

    protected slots:

        //! Could group list is available.
        void onGroupListAvailable(QList<RGroupInfo> groupInfoList);

        //! Cloud group was added.
        void onGroupAdded(RGroupInfo groupInfo);

        //! Cloud group was removed.
        void onGroupRemoved(QString groupName);

        //! Add button clicked.
        void onAddButtonClicked();

        //! Remove button clicked.
        void onRemoveButtonClicked();

        //! Refresh button clicked.
        void onRefreshButtonClicked();

        //! Refresh timeout.
        void onRefreshTimeout();

        //! Refresh timeout has changed.
        void onRefreshTimeoutChanged(uint refreshTimeout);

        //! Cloud client configuratin has changed.
        void onClientConfigurationChanged();

        //! Cloud group selection has changed.
        void onSelectionChanged();

    signals:

        //! Group list changed.
        void groupListChanged(QList<RGroupInfo> groupInfoList);

};

#endif // CLOUD_GROUPS_TREE_H
