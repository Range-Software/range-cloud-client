#ifndef ACTION_H
#define ACTION_H

#include <QAction>

#include <rcl_cloud_session_manager.h>

#include <rgl_action.h>

#define ACTION_GROUP_ALL (             \
    Action::ACTION_GROUP_APPLICATION | \
    Action::ACTION_GROUP_SESSION     | \
    Action::ACTION_GROUP_VIEW        | \
    Action::ACTION_GROUP_EDIT        | \
    Action::ACTION_GROUP_DEBUG       | \
    Action::ACTION_GROUP_CLOUD         \
    )

class Action : public RAction
{

    Q_OBJECT

    public:

        typedef int GroupTypeMask;

        enum GroupType
        {
            ACTION_GROUP_NONE        = 0,
            ACTION_GROUP_APPLICATION = 1 << 1,
            ACTION_GROUP_SESSION     = 1 << 2,
            ACTION_GROUP_VIEW        = 1 << 3,
            ACTION_GROUP_EDIT        = 1 << 4,
            ACTION_GROUP_DEBUG       = 1 << 5,
            ACTION_GROUP_CLOUD       = 1 << 6
        };

        enum Type
        {
            ACTION_SEPARATOR = 0,
            ACTION_APPLICATION_SETTINGS,
            ACTION_APPLICATION_UPDATE,
            ACTION_APPLICATION_ABOUT,
            ACTION_APPLICATION_ABOUT_QT,
            ACTION_APPLICATION_LICENSE,
            ACTION_APPLICATION_RELEASE_NOTES,
            ACTION_APPLICATION_HELP,
            ACTION_APPLICATION_QUIT,
            ACTION_CLOUD_SESSION_MANAGER,
            ACTION_CLOUD_STATISTICS,
            ACTION_N_TYPES
        };

    private:

        //! Constructor.
        explicit Action(Definition definition, QObject *parent = nullptr);

    public:

        //! Return action group name.
        static QString getGroupName(GroupType groupType);

        //! Return action name.
        static QString getName(Type type);

        //! Return list of group types.
        static QList<GroupType> getGroupTypes(GroupTypeMask groupTypeMask = ACTION_GROUP_ALL);

        //! Generate list action descriptions.
        static QList<RAction::Definition> generateActionDefinitionList();

    private:

        static void regDef(QList<RAction::Definition> &actionDefinition,
                           GroupType groupType,
                           Type type,
                           const QString &text,
                           const QString &description,
                           const QString &shortCut,
                           const QString &icon,
                           PointerToMemberTrigger triggerSlot,
                           QAction::MenuRole menuRole = QAction::NoRole);

        static void regDef(QList<RAction::Definition> &actionDefinition,
                           GroupType groupType,
                           Type type,
                           const QString &text,
                           const QString &description,
                           const QString &shortCut,
                           const QString &icon,
                           PointerToMemberToggle toggleSlot,
                           QAction::MenuRole menuRole = QAction::NoRole);

    public slots:

        //! Help center.
        void onHelp();

        //! Quit program.
        void onQuit();

        //! Application settings.
        void onApplicationSettings();

        //! Check for update.
        void onUpdate();

        //! About.
        void onAbout();

        //! About Qt.
        void onAboutQt();

        //! License.
        void onLicense();

        //! Release notes.
        void onReleaseNotes();

        //! Open cloud session manager.
        void onCloudSessionManager();

        //! Open cloud statistics window.
        void onCloudStatistics();

};

#endif // ACTION_H
