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

void Application::initialize()
{
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
