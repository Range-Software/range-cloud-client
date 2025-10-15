#ifndef CLOUD_SESSION_COMBO_BOX_H
#define CLOUD_SESSION_COMBO_BOX_H

#include <QComboBox>

#include <rcl_cloud_session_manager.h>

class CloudSessionComboBox : public QWidget
{
    Q_OBJECT

    protected:

        //! Cloud session manager.
        RCloudSessionManager *cloudSessionManager;

        //! Combo box.
        QComboBox *comboBox;

    public:

        //! Constructor.
        explicit CloudSessionComboBox(RCloudSessionManager *cloudSessionManager, QWidget *parent = nullptr);

    protected slots:

        //! Active session changed.
        void onActiveSessionChanged(RCloudSessionInfo sessionInfo);

        //! Session inserted.
        void onSessionInserted(const QString &sessionName);

        //! Session renamed.
        void onSessionRenamed(const QString &oldSessionName, const QString &newSessionName);

        //! Session removed.
        void onSessionRemoved(const QString &sessionName);

        //! Combo box current text changed.
        void onComboBoxCurrentTextChanged(const QString &text);

};

#endif // CLOUD_SESSION_COMBO_BOX_H
