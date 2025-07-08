#ifndef CLOUD_USER_ADD_DIALOG_H
#define CLOUD_USER_ADD_DIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>

class CloudUserAddDialog : public QDialog
{

    Q_OBJECT

    protected:

        QLineEdit *userNameEdit;
        QPushButton *okButton;

    public:

        //! Constructor.
        explicit CloudUserAddDialog(QWidget *parent = nullptr);

        //! Return user name.
        inline QString getUserName() const { return this->userNameEdit->text(); }

    protected slots:

        //! User has changed.
        void onUserNameChanged(const QString &text);

};

#endif // CLOUD_USER_ADD_DIALOG_H
