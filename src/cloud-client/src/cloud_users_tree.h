#ifndef CLOUD_USERS_TREE_H
#define CLOUD_USERS_TREE_H

#include <QTreeWidget>
#include <QPushButton>

#include <rcl_cloud_client.h>

class CloudUsersTree : public QWidget
{
    Q_OBJECT

    protected:

        enum ColumnType
        {
            ColumnName = 0,
            ColumnGroups,
            NunberOfColumns
        };

        QTreeWidget *usersTree;

        QPushButton *addButton;
        QPushButton *registerButton;
        QPushButton *editButton;
        QPushButton *refreshButton;
        QPushButton *removeButton;

        RCloudClient *cloudClient;

        QList<RGroupInfo> groupInfoList;

        QTimer *refreshTimer;

    public:

        //! Constructor.
        explicit CloudUsersTree(QWidget *parent = nullptr);

    protected:

        //! Populate user tree.
        static void populateTree(const QList<RUserInfo> &userInfoList, QTreeWidget *treeWidget);

        //! Populate user tree item.
        static void updateTreeItem(QTreeWidgetItem *listItem, const RUserInfo &userInfo);

        //! Return column name for given type.
        static QString getColumnName(ColumnType columnType);

        //! Refresh list of users.
        void refreshCloudUsers();

    public slots:

        //! Group list changed.
        void onGroupListChanged(QList<RGroupInfo> groupInfoList);

    protected slots:

        //! Could user list is available.
        void onUserListAvailable(QList<RUserInfo> userInfoList);

        //! Cloud user was added.
        void onUserAdded(RUserInfo userInfo);

        //! Cloud user was updated.
        void onUserUpdated(RUserInfo userInfo);

        //! Cloud user was removed.
        void onUserRemoved(QString userName);

        //! Cloud user was registered.
        void onUserRegistered(std::tuple<RUserInfo,QList<RAuthToken>> registrationInfo);

        //! Add button clicked.
        void onAddButtonClicked();

        //! Sign up button clicked.
        void onRegisterButtonClicked();

        //! Remove button clicked.
        void onRemoveButtonClicked();

        //! Refresh button clicked.
        void onRefreshButtonClicked();

        //! Edit groups triggered.
        void onEditGroupsButtonTriggered();

        //! Edit tokens triggered.
        void onEditTokensButtonTriggered();

        //! Refresh timeout.
        void onRefreshTimeout();

        //! Refresh timeout has changed.
        void onRefreshTimeoutChanged(uint refreshTimeout);

        //! Cloud client configuratin has changed.
        void onClientConfigurationChanged();

        //! Cloud user selection has changed.
        void onSelectionChanged();

    signals:

        //! User list changed.
        void userListChanged(QList<RUserInfo> userInfoList);

};

#endif // CLOUD_USERS_TREE_H
