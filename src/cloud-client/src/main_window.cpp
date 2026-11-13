#include <QtGui/QCloseEvent>
#include <QApplication>
#include <QMenuBar>
#include <QMenu>
#include <QGridLayout>
#include <QStatusBar>
#include <QSplitter>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QActionGroup>
#include <QSignalBlocker>
#include <QToolButton>
#include <QVBoxLayout>

#include <rbl_error.h>
#include <rbl_job_manager.h>
#include <rbl_logger.h>
#include <rbl_tool_task.h>
#include <rbl_utils.h>

#include <rgl_first_run_dialog.h>
#include <rgl_icon.h>
#include <rgl_message_box.h>
#include <rgl_progress_handler.h>
#include <rgl_progress_bar.h>
#include <rgl_ribbon_bar.h>

#include "main_window.h"
#include "action.h"
#include "application.h"
#include "central_widget.h"
#include "cloud_session_combo_box.h"

//! Version of tool bar arrangement. Increase it when tool bars change, to place them anew for existing users.
static constexpr int toolBarsVersion = 2;

MainWindow::MainWindow(RCloudSessionManager *cloudSession,
                       RApplicationSettings *applicationSettings,
                       RActionList *actionList,
                       QWidget *parent)
    : QMainWindow{parent}
    , cloudSession{cloudSession}
    , applicationSettings{applicationSettings}
    , actionList{actionList}
    , classicMenuBar{nullptr}
    , ribbonBar{nullptr}
    , ribbonSessionButton{nullptr}
    , isFirstRun{true}
{
    R_LOG_TRACE_IN;
    if (this->objectName().isEmpty())
    {
        this->setObjectName(QString::fromUtf8("MainWindow"));
    }

    this->readSettings();

    this->applyShortcutsToActions();

    this->setWindowTitle(RVendor::name() + " :: " + tr("Client tool"));
    this->setWindowIcon(RIcon::fromFile(Application::windowIconFile()));

    int toolbarIconSize = this->applicationSettings->getToolbarIconSize();
    this->setIconSize(QSize(toolbarIconSize,toolbarIconSize));
    this->setToolButtonStyle(Qt::ToolButtonIconOnly);

    this->setDockOptions(QMainWindow::ForceTabbedDocks);

    this->createMenus();
    this->createToolBars();
    this->createRibbon();
    this->createStatusBar();

    // Shortcut of an action works only while a widget holding the action is visible,
    // which buttons on other pages of the ribbon are not.
    for (uint i=0;i<this->actionList->getNActions();i++)
    {
        RAction *action = this->actionList->getAction(i);
        if (action && !action->isSeparator())
        {
            this->addAction(action);
        }
    }

    CentralWidget *centralWidget = new CentralWidget;
    this->setCentralWidget(centralWidget);

    this->restoreGeometry(this->applicationSettings->value("mainWindow/geometry").toByteArray());
    this->restoreState(this->applicationSettings->value("mainWindow/windowState").toByteArray());

    // Stored state knows only tool bars of the time it was saved.
    if (this->applicationSettings->value("mainWindow/toolBarsVersion",1).toInt() != toolBarsVersion)
    {
        this->placeToolBars();
        this->applicationSettings->setValue("mainWindow/toolBarsVersion",toolBarsVersion);
    }

    // Must follow restoring of the state, which brings back visibility of tool bars.
    const bool ribbonLayout = this->applicationSettings->value("mainWindow/ribbonLayout",false).toBool();
    {
        const QSignalBlocker ribbonLayoutActionBlocker(this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_LAYOUT));
        this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_LAYOUT)->setChecked(ribbonLayout);
    }
    this->setRibbonLayout(ribbonLayout);

    const bool ribbonCompact = this->applicationSettings->value("mainWindow/ribbonCompact",false).toBool();
    {
        const QSignalBlocker ribbonCompactActionBlocker(this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_COMPACT));
        this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_COMPACT)->setChecked(ribbonCompact);
    }
    this->ribbonBar->setCompact(ribbonCompact);

    this->ribbonBar->setCollapsed(this->applicationSettings->value("mainWindow/ribbonCollapsed",false).toBool());
    this->ribbonBar->setCurrentPage(this->applicationSettings->value("mainWindow/ribbonPage",0).toInt());

    // Toolbar icon size changed signal
    QObject::connect(this->applicationSettings,&RApplicationSettings::toolbarIconSizeChanged,this,&MainWindow::onToolbarIconSizeChanged);

    // Main progress signals
    QObject::connect(&RProgressHandler::getInstance(),&RProgressHandler::progress,this,&MainWindow::onMainProgress);
    QObject::connect(&RProgressHandler::getInstance(),&RProgressHandler::progressInitialize,this,&MainWindow::onMainProgressInitialize);
    QObject::connect(&RProgressHandler::getInstance(),&RProgressHandler::progressFinalize,this,&MainWindow::onMainProgressFinalize);

    // Job manager signals
    QObject::connect(&RJobManager::getInstance(),&RJobManager::jobBlocking,this,&MainWindow::setDisabled);

    if (this->isFirstRun)
    {
        RFirstRunDialog(this).exec();
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::progressAutoHideEnable()
{
    R_LOG_TRACE;
    this->mainProgressBar->setAutoHide(true);
}

void MainWindow::progressAutoHideDisable()
{
    R_LOG_TRACE;
    this->mainProgressBar->setAutoHide(false);
}

void MainWindow::setRibbonLayout(bool ribbonLayout)
{
    R_LOG_TRACE_IN;
    this->classicMenuBar->setVisible(!ribbonLayout);
    this->ribbonBar->setVisible(ribbonLayout);

    for (QToolBar *toolBar : std::as_const(this->toolBars))
    {
        toolBar->setVisible(!ribbonLayout);
        // Otherwise context menu could bring tool bars back to the ribbon layout.
        toolBar->toggleViewAction()->setVisible(!ribbonLayout);
    }

    this->applicationSettings->setValue("mainWindow/ribbonLayout",ribbonLayout);
    R_LOG_TRACE_OUT;
}

void MainWindow::setRibbonCompact(bool ribbonCompact)
{
    R_LOG_TRACE_IN;
    this->ribbonBar->setCompact(ribbonCompact);
    this->applicationSettings->setValue("mainWindow/ribbonCompact",ribbonCompact);

    // Asking for compact ribbon from the classic layout means asking for the ribbon as well.
    if (ribbonCompact)
    {
        this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_LAYOUT)->setChecked(true);
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::createMenus()
{
    R_LOG_TRACE_IN;
    // Menu bar and ribbon share the place above the tool bars, only one of them is visible at a time.
    QWidget *menuWidget = new QWidget(this);
    QVBoxLayout *menuWidgetLayout = new QVBoxLayout(menuWidget);
    menuWidgetLayout->setContentsMargins(0,0,0,0);
    menuWidgetLayout->setSpacing(0);
    this->setMenuWidget(menuWidget);

    QMenuBar *menuBar = new QMenuBar(menuWidget);
    menuBar->setNativeMenuBar(!this->applicationSettings->getDontUseNativeMenuBar());
    menuWidgetLayout->addWidget(menuBar);
    this->classicMenuBar = menuBar;

    menuBar->addAction(this->createApplicationMenu(menuBar)->menuAction());
    menuBar->addAction(this->createSessionMenu(menuBar)->menuAction());
    menuBar->addAction(this->createHelpMenu(menuBar)->menuAction());
    R_LOG_TRACE_OUT;
}

QMenu *MainWindow::createApplicationMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuApplication = new QMenu(parent);
    menuApplication->setTitle(tr("Application"));
    QMenu *menuApplicationFolders = new QMenu(menuApplication);
    menuApplicationFolders->setTitle(tr("Folders"));
    menuApplicationFolders->setIcon(RIcon::fromFile(":/icons/file/pixmaps/range-open.svg"));
    menuApplicationFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_CERT));
    menuApplicationFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_DATA));
    menuApplicationFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_LOG));
    menuApplicationFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_SESSION));
    menuApplicationFolders->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_FOLDER_TMP));
    menuApplication->addAction(menuApplicationFolders->menuAction());
    menuApplication->addSeparator();
    menuApplication->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_LAYOUT));
    menuApplication->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_RIBBON_COMPACT));
    menuApplication->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_SETTINGS));
    menuApplication->addSeparator();
    menuApplication->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_QUIT));
    R_LOG_TRACE_OUT;
    return menuApplication;
}

QMenu *MainWindow::createSessionMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuSession = new QMenu(parent);
    menuSession->setTitle(tr("Session"));
    menuSession->addAction(this->actionList->getAction(Action::ACTION_CLOUD_SESSION_MANAGER));
    menuSession->addAction(this->actionList->getAction(Action::ACTION_CLOUD_STATISTICS));
    R_LOG_TRACE_OUT;
    return menuSession;
}

QMenu *MainWindow::createHelpMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menuHelp = new QMenu(parent);
    menuHelp->setTitle(tr("Help"));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_HELP));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_UPDATE));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_ABOUT));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_ABOUT_QT));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_LICENSE));
    menuHelp->addAction(this->actionList->getAction(Action::ACTION_APPLICATION_RELEASE_NOTES));
    R_LOG_TRACE_OUT;
    return menuHelp;
}

QMenu *MainWindow::createSessionSelectMenu(QWidget *parent)
{
    R_LOG_TRACE_IN;
    QMenu *menu = new QMenu(parent);
    // Sessions are listed anew each time, so the menu never shows a stale list.
    QObject::connect(menu,&QMenu::aboutToShow,this,[this,menu]()
    {
        menu->clear();
        QActionGroup *actionGroup = new QActionGroup(menu);
        const QString activeSessionName = this->cloudSession->findActiveSessionName();
        const QStringList sessionNames = this->cloudSession->getSessionNames();
        for (const QString &sessionName : sessionNames)
        {
            QAction *action = menu->addAction(sessionName);
            action->setCheckable(true);
            action->setChecked(sessionName == activeSessionName);
            actionGroup->addAction(action);
            QObject::connect(action,&QAction::triggered,this,[this,sessionName]()
            {
                this->cloudSession->setActiveSessionName(sessionName);
            });
        }
        if (!sessionNames.isEmpty())
        {
            menu->addSeparator();
        }
        menu->addAction(this->actionList->getAction(Action::ACTION_CLOUD_SESSION_MANAGER));
    });
    R_LOG_TRACE_OUT;
    return menu;
}

void MainWindow::createToolBars()
{
    R_LOG_TRACE_IN;

    auto action = [this](Action::Type type) -> QAction* { return this->actionList->getAction(type); };

    QToolBar *sessionToolBar = this->createToolBar("mainToolBar",tr("Session toolbar"));
    CloudSessionComboBox *sessionComboBox = new CloudSessionComboBox(this->cloudSession,sessionToolBar);
    sessionToolBar->addWidget(sessionComboBox);
    sessionToolBar->addAction(action(Action::ACTION_CLOUD_SESSION_MANAGER));
    sessionToolBar->addAction(action(Action::ACTION_CLOUD_STATISTICS));

    QToolBar *applicationToolBar = this->createToolBar("applicationToolBar",tr("Application toolbar"));

    // Last tool bar of a row takes the remaining width, this pushes its buttons to the right edge of the window.
    QWidget *spacer = new QWidget(applicationToolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    applicationToolBar->addWidget(spacer);

    applicationToolBar->addAction(action(Action::ACTION_APPLICATION_RIBBON_LAYOUT));
    applicationToolBar->addAction(action(Action::ACTION_APPLICATION_SETTINGS));
    applicationToolBar->addSeparator();
    applicationToolBar->addAction(action(Action::ACTION_APPLICATION_HELP));

    // Tool bars are placed in this order.
    this->toolBars = { sessionToolBar, applicationToolBar };
    this->placeToolBars();

    R_LOG_TRACE_OUT;
}

QToolBar *MainWindow::createToolBar(const QString &name, const QString &title)
{
    R_LOG_TRACE;
    QToolBar *toolBar = new QToolBar(this);
    toolBar->setObjectName(name);
    toolBar->setWindowTitle(title);
    // An icon size the tool bar only inherits from the main window is replaced
    // by the default one of the style whenever the style or color scheme
    // changes. Set explicitly, it is kept.
    toolBar->setIconSize(this->iconSize());
    return toolBar;
}

void MainWindow::placeToolBars()
{
    R_LOG_TRACE_IN;
    for (QToolBar *toolBar : std::as_const(this->toolBars))
    {
        this->removeToolBar(toolBar);
    }
    for (QToolBar *toolBar : std::as_const(this->toolBars))
    {
        this->addToolBar(Qt::TopToolBarArea,toolBar);
        // Removing a tool bar hides it.
        toolBar->show();
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::createRibbon()
{
    R_LOG_TRACE_IN;
    this->ribbonBar = new RRibbonBar(this->menuWidget());
    this->menuWidget()->layout()->addWidget(this->ribbonBar);
    this->ribbonBar->setIconSize(this->applicationSettings->getToolbarIconSize());

    auto action = [this](Action::Type type) -> QAction* { return this->actionList->getAction(type); };

    // Line with tabs
    this->ribbonBar->addLeadingMenu(tr("Application"),this->createApplicationMenu(this->ribbonBar));

    this->ribbonBar->addTrailingAction(action(Action::ACTION_APPLICATION_RIBBON_LAYOUT));
    this->ribbonBar->addTrailingAction(action(Action::ACTION_APPLICATION_RIBBON_COMPACT));
    this->ribbonBar->addTrailingAction(action(Action::ACTION_APPLICATION_SETTINGS));
    this->ribbonBar->addTrailingMenu(tr("Help"),action(Action::ACTION_APPLICATION_HELP)->icon(),this->createHelpMenu(this->ribbonBar));

    RRibbonGroup *group = nullptr;

    // Session
    const int sessionPage = this->ribbonBar->addPage(tr("Session"));

    group = this->ribbonBar->addGroup(sessionPage,tr("Session"));
    group->addLargeAction(action(Action::ACTION_CLOUD_SESSION_MANAGER),tr("Manager"));
    group->addLargeAction(action(Action::ACTION_CLOUD_STATISTICS));
    // Session combo box is on the tool bar, which the ribbon layout hides.
    this->ribbonSessionButton = group->addSmallMenu(QString(),
                                                    action(Action::ACTION_CLOUD_SESSION_MANAGER)->icon(),
                                                    this->createSessionSelectMenu(this->ribbonBar));
    this->ribbonSessionButton->setToolTip(tr("Select active session"));
    this->updateRibbonSessionButton();
    QObject::connect(this->cloudSession,&RCloudSessionManager::activeSessionChanged,this,&MainWindow::updateRibbonSessionButton);
    QObject::connect(this->cloudSession,&RCloudSessionManager::sessionRenamed,this,&MainWindow::updateRibbonSessionButton);
    QObject::connect(this->cloudSession,&RCloudSessionManager::sessionRemoved,this,&MainWindow::updateRibbonSessionButton);

    // Application
    const int applicationPage = this->ribbonBar->addPage(tr("Application"));

    group = this->ribbonBar->addGroup(applicationPage,tr("Folders"));
    group->addSmallAction(action(Action::ACTION_APPLICATION_FOLDER_CERT));
    group->addSmallAction(action(Action::ACTION_APPLICATION_FOLDER_DATA));
    group->addSmallAction(action(Action::ACTION_APPLICATION_FOLDER_LOG));
    group->addSmallAction(action(Action::ACTION_APPLICATION_FOLDER_SESSION));
    group->addSmallAction(action(Action::ACTION_APPLICATION_FOLDER_TMP));

    group = this->ribbonBar->addGroup(applicationPage,tr("Software"));
    group->addLargeAction(action(Action::ACTION_APPLICATION_UPDATE),tr("Update"));
    group->addSmallAction(action(Action::ACTION_APPLICATION_RELEASE_NOTES));
    group->addSmallAction(action(Action::ACTION_APPLICATION_LICENSE));
    group->addSmallAction(action(Action::ACTION_APPLICATION_ABOUT));

    R_LOG_TRACE_OUT;
}

void MainWindow::updateRibbonSessionButton()
{
    R_LOG_TRACE;
    const QString activeSessionName = this->cloudSession->findActiveSessionName();
    this->ribbonSessionButton->setText(activeSessionName.isEmpty() ? tr("No session") : activeSessionName);
}

void MainWindow::createStatusBar()
{
    R_LOG_TRACE_IN;
    QStatusBar *statusbar;

    statusbar = new QStatusBar(this);
    statusbar->setObjectName(QString::fromUtf8("statusbar"));
    this->setStatusBar(statusbar);

    QWidget *progressBarWidget = new QWidget;
    statusbar->addWidget(progressBarWidget,1);

    QVBoxLayout *progressBarLayout = new QVBoxLayout;
    progressBarLayout->setSpacing(0);
    progressBarLayout->setContentsMargins(0,0,0,0);
    progressBarWidget->setLayout(progressBarLayout);

    this->mainProgressBar = new RProgressBar(this);
    this->mainProgressBar->hide();
    progressBarLayout->addWidget(this->mainProgressBar);
    R_LOG_TRACE_OUT;
}

void MainWindow::applyShortcutsToActions()
{
    R_LOG_TRACE_IN;
    for (uint i=0;i<this->actionList->getNActions();i++)
    {
        RAction *action = this->actionList->getAction(i);
        if (!action || action->isSeparator())
        {
            continue;
        }
        QString key = "shortcuts/shortcut_" + action->getName();
        if (this->applicationSettings->contains(key))
        {
            action->setShortcut(this->applicationSettings->value(key).toString());
        }
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::readSettings()
{
    R_LOG_TRACE_IN;
    this->isFirstRun = (this->applicationSettings->getStoredVersion() != RVendor::version());
    this->applicationSettings->sync();
    R_LOG_TRACE_OUT;
}

void MainWindow::writeSettings() const
{
    R_LOG_TRACE_IN;
    // Main window
    this->applicationSettings->setValue("mainWindow/geometry", this->saveGeometry());
    this->applicationSettings->setValue("mainWindow/windowState", this->saveState());
    this->applicationSettings->setValue("mainWindow/ribbonCollapsed", this->ribbonBar->isCollapsed());
    this->applicationSettings->setValue("mainWindow/ribbonPage", this->ribbonBar->getCurrentPage());

    // Shortcuts
    for (uint i=0;i<this->actionList->getNActions();i++)
    {
        const RAction *action = this->actionList->getAction(i);
        if (!action || action->isSeparator())
        {
            continue;
        }
        QString key = "shortcuts/shortcut_" + action->getName();
        this->applicationSettings->setValue(key, action->shortcut().toString());
    }

    this->applicationSettings->sync();
    R_LOG_TRACE_OUT;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    R_LOG_TRACE_IN;
    if (RMessageBox::quit(this) == RMessageBox::Yes)
    {
        this->writeSettings();
        // Accept event -> close window.
        event->accept();
    }
    else
    {
        // Ignore event -> leave window open.
        event->ignore();
    }
    R_LOG_TRACE_OUT;
}

void MainWindow::progressBar(RProgressBar *progressBar, double fraction)
{
    R_LOG_TRACE;
    progressBar->setValue(qRound(fraction*100));
//    progressBar->show();
}

void MainWindow::progressBarInitialize(RProgressBar *progressBar, const QString &message, bool pulseType)
{
    R_LOG_TRACE_IN;
    if (pulseType)
    {
        progressBar->startPulse();
    }
    else
    {
        progressBar->setRange(0,100);
    }
    progressBar->setMessage(message);
    progressBar->setValue(0);
    progressBar->show();
    this->statusBar()->clearMask();
    R_LOG_TRACE_OUT;
}

void MainWindow::progressBarFinalize(RProgressBar *progressBar, const QString &message)
{
    R_LOG_TRACE_IN;
    progressBar->stopPulse();
    progressBar->setRange(0,100);
    progressBar->setValue(100);
    progressBar->setMessage("");
    progressBar->hide();
    this->statusBar()->showMessage(message,5000);
    R_LOG_TRACE_OUT;
}

void MainWindow::setDisabled(bool disabled)
{
    R_LOG_TRACE_IN;
    this->QWidget::setDisabled(disabled);
    this->actionList->setDisabled(disabled);
    R_LOG_TRACE_OUT;
}

void MainWindow::onMainProgress(double fraction)
{
    R_LOG_TRACE;
    this->progressBar(this->mainProgressBar,fraction);
}

void MainWindow::onMainProgressInitialize(const QString &message, bool pulseType)
{
    R_LOG_TRACE;
    this->progressBarInitialize(this->mainProgressBar,message,pulseType);
}

void MainWindow::onMainProgressFinalize(const QString &message)
{
    R_LOG_TRACE;
    this->progressBarFinalize(this->mainProgressBar,message);
}

void MainWindow::onToolbarIconSizeChanged(int toolbarIconSize)
{
    R_LOG_TRACE;
    this->setIconSize(QSize(toolbarIconSize,toolbarIconSize));
    // Tool bars have their icon size set explicitly, so they do not follow the
    // main window.
    for (QToolBar *toolBar : std::as_const(this->toolBars))
    {
        toolBar->setIconSize(QSize(toolbarIconSize,toolbarIconSize));
    }
    this->ribbonBar->setIconSize(toolbarIconSize);
}
