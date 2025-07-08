#ifndef CLOUD_ACTIONS_TREE_H
#define CLOUD_ACTIONS_TREE_H

#include <QTreeWidget>
#include <QPushButton>

#include <rcl_cloud_action_info.h>
#include <rcl_cloud_client.h>

class CloudActionsTree : public QWidget
{
    Q_OBJECT

    protected:

        enum ColumnType
        {
            ColumnName = 0,
            ColumnAccessMode,
            ColumnAccessOwner,
            NunberOfColumns
        };

        QTreeWidget *actionsTree;

        QPushButton *editButton;
        QPushButton *refreshButton;

        RCloudClient *cloudClient;

        QList<RUserInfo> userInfoList;
        QList<RGroupInfo> groupInfoList;

        QTimer *refreshTimer;

    public:

        //! Constructor.
        explicit CloudActionsTree(QWidget *parent = nullptr);

    protected:

        //! Populate actions tree.
        static void populateTree(const QList<RCloudActionInfo> &actionInfoList, QTreeWidget *treeWidget);

        //! Populate actions tree item.
        static void updateTreeItem(QTreeWidgetItem *listItem, const RCloudActionInfo &actionInfo);

        //! Return column name for given type.
        static QString getColumnName(ColumnType columnType);

        //! Refresh list of actions.
        void refreshCloudActions();

    public slots:

        //! User list changed.
        void onUserListChanged(QList<RUserInfo> userInfoList);

        //! Group list changed.
        void onGroupListChanged(QList<RGroupInfo> groupInfoList);

    protected slots:

        //! Could action list is available.
        void onActionListAvailable(QList<RCloudActionInfo> actionInfoList);

        //! Action access owner was updated.
        void onActionAccessOwnerUpdated(RCloudActionInfo);

        //! Action access mode was updated.
        void onActionAccessModeUpdated(RCloudActionInfo);

        //! Cloud access owner triggered.
        void onAccessOwnerActionTriggered();

        //! Cloud access mode triggered.
        void onAccessModeActionTriggered();

        //! Refresh button clicked.
        void onRefreshButtonClicked();

        //! Refresh timeout.
        void onRefreshTimeout();

        //! Refresh timeout has changed.
        void onRefreshTimeoutChanged(uint refreshTimeout);

        //! Cloud client configuratin has changed.
        void onClientConfigurationChanged();

        //! Cloud user selection has changed.
        void onSelectionChanged();

    signals:

};

#endif // CLOUD_ACTIONS_TREE_H
