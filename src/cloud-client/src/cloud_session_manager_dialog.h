#ifndef CLOUD_SESSION_MANAGER_DIALOG_H
#define CLOUD_SESSION_MANAGER_DIALOG_H

#include <QDialog>

#include <rcl_cloud_session_manager.h>

#include <rgl_application_settings.h>
#include <rgl_cloud_connection_handler.h>

class CloudSessionManagerDialog : public QDialog
{

    Q_OBJECT

    protected:

    public:

        //! Constructor.
        explicit CloudSessionManagerDialog(RCloudSessionManager *session,
                                           RCloudConnectionHandler *connectionHandler,
                                           RApplicationSettings *applicationSettings,
                                           QWidget *parent = nullptr);

};

#endif // CLOUD_SESSION_MANAGER_DIALOG_H
