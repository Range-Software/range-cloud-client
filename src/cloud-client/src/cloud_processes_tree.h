#ifndef CLOUD_PROCESSES_TREE_H
#define CLOUD_PROCESSES_TREE_H

#include <QTreeWidget>
#include <QPushButton>

#include <rcl_cloud_process_info.h>
#include <rcl_cloud_client.h>

class CloudProcessesTree : public QWidget
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

        QTreeWidget *processesTree;

        QPushButton *editButton;
        QPushButton *refreshButton;
        QPushButton *executeButton;

        RCloudClient *cloudClient;

        QList<RUserInfo> userInfoList;
        QList<RGroupInfo> groupInfoList;

        QTimer *refreshTimer;

    public:

        //! Constructor.
        explicit CloudProcessesTree(QWidget *parent = nullptr);

    protected:

        //! Populate processes tree.
        static void populateTree(const QList<RCloudProcessInfo> &processInfoList, QTreeWidget *treeWidget);

        //! Populate processes tree item.
        static void updateTreeItem(QTreeWidgetItem *listItem, const RCloudProcessInfo &processInfo);

        //! Return column name for given type.
        static QString getColumnName(ColumnType columnType);

        //! Refresh list of processes.
        void refreshCloudProcesses();

    public slots:

        //! User list changed.
        void onUserListChanged(QList<RUserInfo> userInfoList);

        //! Group list changed.
        void onGroupListChanged(QList<RGroupInfo> groupInfoList);

    protected slots:

        //! Could process list is available.
        void onProcessListAvailable(QList<RCloudProcessInfo> processInfoList);

        //! Process access owner was updated.
        void onProcessAccessOwnerUpdated(RCloudProcessInfo);

        //! Process access mode was updated.
        void onProcessAccessModeUpdated(RCloudProcessInfo);

        //! Process has finished.
        void onProcessFinished(QString message);

        //! Cloud access owner triggered.
        void onAccessOwnerProcessTriggered();

        //! Cloud access mode triggered.
        void onAccessModeProcessTriggered();

        //! Refresh button clicked.
        void onRefreshButtonClicked();

        //! Refresh timeout.
        void onRefreshTimeout();

        //! Refresh timeout has changed.
        void onRefreshTimeoutChanged(uint refreshTimeout);

        //! Cloud client configuratin has changed.
        void onClientConfigurationChanged();

        //! Execute button clicked.
        void onExecuteButtonClicked();

        //! Cloud user selection has changed.
        void onSelectionChanged();

    signals:

};

#endif // CLOUD_PROCESSES_TREE_H
