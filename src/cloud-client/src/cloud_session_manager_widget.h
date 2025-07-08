#ifndef CLOUD_SESSION_MANAGER_WIDGET_H
#define CLOUD_SESSION_MANAGER_WIDGET_H

#include <QListWidget>

#include <rcl_cloud_session_manager.h>

#include <rgl_application_settings.h>
#include <rgl_cloud_connection_handler.h>

class CloudSessionManagerWidget : public QWidget
{

    Q_OBJECT

    protected:

        //! Cloud session manager.
        RCloudSessionManager *session;

    public:

        //! Constructor.
        explicit CloudSessionManagerWidget(RCloudSessionManager *session,
                                           RCloudConnectionHandler *connectionHandler,
                                           RApplicationSettings *applicationSettings,
                                           QWidget *parent = nullptr);

    signals:

};

#endif // CLOUD_SESSION_MANAGER_WIDGET_H
