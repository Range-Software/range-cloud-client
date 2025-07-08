#ifndef CLOUD_AUTH_TOKEN_MANAGER_WIDGET_H
#define CLOUD_AUTH_TOKEN_MANAGER_WIDGET_H

#include <QTreeWidget>
#include <QLineEdit>
#include <QPushButton>

#include <rcl_cloud_client.h>
#include <rcl_auth_token.h>

class CloudAuthTokenManagerWidget : public QWidget
{
    Q_OBJECT

    protected:

        enum ColumnType
        {
            ColumnId = 0,
            ColumnContent,
            ColumnValidity,
            NunberOfColumns
        };

        QString resourceName;

        QTreeWidget *tokensTree;
        QLineEdit *tokenContent;
        QPushButton *generateButton;
        QPushButton *removeButton;

        //! Cloud client.
        RCloudClient *cloudClient;

    public:

        //! Constructor.
        explicit CloudAuthTokenManagerWidget(const QString &resourceName, QWidget *parent = nullptr);

    protected:

        //! Refresh list of tokens.
        void refreshTokens();

        //! Populate user tree item.
        static void updateTreeItem(QTreeWidgetItem *listItem, const RAuthToken &authToken);

        //! Return column name for given type.
        static QString getColumnName(ColumnType columnType);

    protected slots:

        //! Cloud client configuratin has changed.
        void onClientConfigurationChanged();

        //! Tree selection has changed.
        void onSelectionChanged();

        //! Cloud client configuratin has changed.
        void onUserTokenListAvailable(QList<RAuthToken> authTokenList);

        //! Cloud user authentication token was generated.
        void onUserTokenGenerated(RAuthToken authToken);

        //! Cloud user authentication token was removed.
        void onUserTokenRemoved(QUuid id);

        //! Generate token button was clicked.
        void onGenerateClicked();

        //! Remove token button was clicked.
        void onRemoveClicked();

    signals:

};

#endif // CLOUD_AUTH_TOKEN_MANAGER_WIDGET_H
