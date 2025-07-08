#ifndef CLOUD_AUTH_TOKEN_MANAGER_DIALOG_H
#define CLOUD_AUTH_TOKEN_MANAGER_DIALOG_H

#include <QDialog>

class CloudAuthTokenManagerDialog : public QDialog
{
    Q_OBJECT

    protected:

        //! User name.
        QString userName;

    public:

        //! Constructor.
        explicit CloudAuthTokenManagerDialog(const QString &resourceName, QWidget *parent = nullptr);


};

#endif // CLOUD_AUTH_TOKEN_MANAGER_DIALOG_H
