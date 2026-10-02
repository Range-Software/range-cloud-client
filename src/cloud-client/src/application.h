#ifndef APPLICATION_H
#define APPLICATION_H

#include <rgl_application.h>
#include <rgl_application_settings.h>
#include <rgl_cloud_connection_handler.h>

class Application : public RApplication
{

    Q_OBJECT

    public:

        //! Constructor.
        explicit Application(int &argc, char **argv);

        //! Return application instance.
        static Application *instance() noexcept;

        //! Return resource path of the application window icon.
        //! Windows gets a full bleed variant, the default one follows the macOS icon grid.
        static QString windowIconFile();

        //! Initialize user objects.
        void initialize();

        //! Finalize user objects.
        void finalize();

};

#endif // APPLICATION_H
