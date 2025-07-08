#ifndef CLOUD_USER_GROUPS_DIALOG_H
#define CLOUD_USER_GROUPS_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QListWidgetItem>

#include <rcl_user_info.h>
#include <rcl_group_info.h>

class CloudUserGroupsDialog : public QDialog
{

    Q_OBJECT

    protected:

        RUserInfo userInfo;

        QPushButton *okButton;

    public:

        //! Constructor.
        explicit CloudUserGroupsDialog(const RUserInfo &userInfo, const QList<RGroupInfo> &groupInfoList, QWidget *parent = nullptr);

        //! Return user info.
        inline RUserInfo getUserInfo() const { return this->userInfo; }

    protected slots:

        //! List item has changed.
        void onListItemChanged(QListWidgetItem *item);



};

#endif // CLOUD_USER_GROUPS_DIALOG_H
