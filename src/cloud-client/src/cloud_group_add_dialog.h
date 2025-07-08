#ifndef CLOUD_GROUP_ADD_DIALOG_H
#define CLOUD_GROUP_ADD_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class CloudGroupAddDialog : public QDialog
{

    Q_OBJECT

    protected:

        QLineEdit *groupNameEdit;
        QPushButton *okButton;

    public:

        //! Constructor.
        explicit CloudGroupAddDialog(QWidget *parent = nullptr);

        //! Return group name.
        inline QString getGroupName() const { return this->groupNameEdit->text(); }

    protected slots:

        //! Group has changed.
        void onGroupNameChanged(const QString &text);

};

#endif // CLOUD_GROUP_ADD_DIALOG_H
