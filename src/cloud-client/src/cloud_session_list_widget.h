#ifndef CLOUD_SESSION_LIST_WIDGET_H
#define CLOUD_SESSION_LIST_WIDGET_H

#include <QListWidget>
#include <QPushButton>

#include <rcl_cloud_session_manager.h>

class CloudSessionListWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Cloud session manager.
        RCloudSessionManager *cloudSession;

        //! List of session names.
        QListWidget *nameList;

        //! Set active button.
        QPushButton *setActiveButton;

        //! Add button.
        QPushButton *addButton;

        //! Remove button.
        QPushButton *removeButton;

    public:

        //! Constructor.
        explicit CloudSessionListWidget(RCloudSessionManager *cloudSession, QWidget *parent = nullptr);

        //! Select session with given name.
        void selectSession(const QString &sessionName);

    protected:

        //! Add item.
        void addItem(const QString &name);

    protected slots:

        //! Name item changed.
        void onNameItemChanged(QListWidgetItem *item);

        //! Name list selection changed.
        void onNameSelectionChanged();

        //! Set active button clicked.
        void onSetActiveButtonClicked();

        //! Add button clicked.
        void onAddButtonClicked();

        //! Remove button clicked.
        void onRemoveButtonClicked();

        //! Active session has changed.
        void onActiveSessionChanged(RCloudSessionInfo sessionInfo);

    signals:

        void sessionSelected(RCloudSessionInfo sessionInfo);

};

#endif // CLOUD_SESSION_LIST_WIDGET_H
