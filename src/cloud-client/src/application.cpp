#include <rgl_application_settings.h>

#include "application.h"
#include "action.h"
#include "main_window.h"

Application::Application(int &argc, char **argv)
    : RApplication{argc,argv}
{
    // Set translator names
    this->translatorNames << "cloud-client";
    this->translatorNames << "range-gui-lib";
    this->translatorNames << "range-cloud-lib";
}

Application *Application::instance() noexcept
{
    return qobject_cast<Application*>(RApplication::instance());
}

QString Application::windowIconFile()
{
#ifdef Q_OS_WIN
    return QString(":/icons/logos/pixmaps/range-cloud-client-windows.svg");
#else
    return QString(":/icons/logos/pixmaps/range-cloud-client.svg");
#endif
}

void Application::initialize()
{
#ifdef STORE_BUILD
    // A store installs every version into a folder of its own (a versioned
    // directory under WindowsApps for MSIX), so a path stored by the previous
    // version points into a folder which no longer exists. The OpenSSL tool and
    // its configuration belong to the package itself, so they are reset to the
    // defaults without asking.
    this->applicationSettings->setOpensslToolPath(RApplicationSettings::getDefaultOpensslToolPath());
    this->applicationSettings->setOpensslConfPath(this->applicationSettings->findOpensslConfPath());
#endif

    // Initialize action list
    this->actionList = new RActionList(Action::generateActionDefinitionList(),this);

    // Prepare main window
    this->mainWindow = new MainWindow(this->cloudSessionManager,this->applicationSettings,this->actionList);
    this->cloudConnectionHandler->setMessageBoxParentWidget(this->mainWindow);
    this->mainWindow->show();
}

void Application::finalize()
{

}
