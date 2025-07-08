#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QTreeWidget>

#include <rbl_logger.h>

#include <rgl_message_box.h>

#include "cloud_auth_token_manager_widget.h"
#include "application.h"

CloudAuthTokenManagerWidget::CloudAuthTokenManagerWidget(const QString &resourceName, QWidget *parent)
    : QWidget{parent}
    , resourceName{resourceName}
{
    this->cloudClient = qobject_cast<Application*>(Application::instance())->getCloudConnectionHandler()->createPrivateClient(this);
    this->cloudClient->setBlocking(false);

    QIcon generateIcon(":/icons/action/pixmaps/range-add.svg");
    QIcon removeIcon(":/icons/action/pixmaps/range-remove.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    QTreeWidgetItem *headerItem = new QTreeWidgetItem();
    for (int type=ColumnId;type<NunberOfColumns;type++)
    {
        headerItem->setText(type,CloudAuthTokenManagerWidget::getColumnName(ColumnType(type)));
    }

    this->tokensTree = new QTreeWidget;
    this->tokensTree->setColumnCount(NunberOfColumns);
    this->tokensTree->setSortingEnabled(true);
    this->tokensTree->setHeaderItem(headerItem);
    this->tokensTree->hideColumn(ColumnContent);
    mainLayout->addWidget(this->tokensTree);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    this->tokenContent = new QLineEdit;
    this->tokenContent->setReadOnly(true);
    formLayout->addRow(tr("Token") + ":",this->tokenContent);

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    mainLayout->addLayout(buttonsLayout);

    this->generateButton = new QPushButton(generateIcon,tr("New"));
    buttonsLayout->addWidget(this->generateButton);

    this->removeButton = new QPushButton(removeIcon,tr("Remove"));
    this->removeButton->setDisabled(true);
    buttonsLayout->addWidget(this->removeButton);

    QObject::connect(this->tokensTree,&QTreeWidget::itemSelectionChanged,this,&CloudAuthTokenManagerWidget::onSelectionChanged);

    QObject::connect(this->generateButton,&QPushButton::clicked,this,&CloudAuthTokenManagerWidget::onGenerateClicked);
    QObject::connect(this->removeButton,&QPushButton::clicked,this,&CloudAuthTokenManagerWidget::onRemoveClicked);

    QObject::connect(this->cloudClient,&RCloudClient::configurationChanged,this,&CloudAuthTokenManagerWidget::onClientConfigurationChanged);
    QObject::connect(this->cloudClient,&RCloudClient::userTokenListAvailable,this,&CloudAuthTokenManagerWidget::onUserTokenListAvailable);
    QObject::connect(this->cloudClient,&RCloudClient::userTokenGenerated,this,&CloudAuthTokenManagerWidget::onUserTokenGenerated);
    QObject::connect(this->cloudClient,&RCloudClient::userTokenRemoved,this,&CloudAuthTokenManagerWidget::onUserTokenRemoved);

    this->refreshTokens();
}

void CloudAuthTokenManagerWidget::refreshTokens()
{
    try
    {
        this->cloudClient->requestListUserTokens(this->resourceName);
    }
    catch (const RError &rError)
    {
        RLogger::error("Failed to request list of authentication tokens from Cloud. %s\n",rError.getMessage().toUtf8().constData());
        RMessageBox::critical(this,tr("Cloud transfer failed"),tr("List of authentication tokens from Cloud has failed."));
    }
}

void CloudAuthTokenManagerWidget::updateTreeItem(QTreeWidgetItem *listItem, const RAuthToken &authToken)
{
    listItem->setText(ColumnId,authToken.getId().toString(QUuid::WithoutBraces));
    listItem->setData(ColumnId,Qt::UserRole,authToken.getId());
    listItem->setText(ColumnContent,authToken.getContent());
    listItem->setText(ColumnValidity,QDateTime::fromSecsSinceEpoch(authToken.getValidityDate()).toString());
}

QString CloudAuthTokenManagerWidget::getColumnName(ColumnType columnType)
{
    switch (columnType)
    {
        case ColumnId:       return tr("ID");
        case ColumnContent:  return tr("Content");
        case ColumnValidity: return tr("Validity");
        default:             return QString();
    }
}

void CloudAuthTokenManagerWidget::onClientConfigurationChanged()
{
    this->refreshTokens();
}

void CloudAuthTokenManagerWidget::onSelectionChanged()
{
    this->removeButton->setEnabled(false);
    this->tokenContent->clear();
    QList<QTreeWidgetItem*> selectedItems = this->tokensTree->selectedItems();
    for (QTreeWidgetItem *item : std::as_const(selectedItems))
    {
        this->tokenContent->setText(item->text(ColumnContent));
        this->removeButton->setEnabled(true);
    }
}

void CloudAuthTokenManagerWidget::onUserTokenListAvailable(QList<RAuthToken> authTokenList)
{
    QList<RAuthToken> _authTokenList = authTokenList;

    for (int i=this->tokensTree->topLevelItemCount()-1; i>=0; i--)
    {
        const QUuid id = this->tokensTree->topLevelItem(i)->data(ColumnId,Qt::UserRole).toUuid();
        bool userInList = false;
        for (auto iter = _authTokenList.cbegin(), end = _authTokenList.cend(); iter != end; ++iter)
        {
            if (id == iter->getId())
            {
                CloudAuthTokenManagerWidget::updateTreeItem(this->tokensTree->topLevelItem(i),*iter);
                _authTokenList.erase(iter);
                userInList = true;
                break;
            }
        }
        if (!userInList)
        {
            this->tokensTree->takeTopLevelItem(i);
        }
    }
    for (const RAuthToken &authToken : _authTokenList)
    {
        CloudAuthTokenManagerWidget::updateTreeItem(new QTreeWidgetItem(this->tokensTree),authToken);
    }
    // this->tokensTree->resizeColumnToContents(ColumnId);
    // this->tokensTree->resizeColumnToContents(ColumnContent);
    this->tokensTree->resizeColumnToContents(ColumnValidity);
}

void CloudAuthTokenManagerWidget::onUserTokenGenerated(RAuthToken authToken)
{
    RLogger::info("Cloud user authentication token was generated \'%s\' : \'%s\'\n",
                  authToken.getResourceName().toUtf8().constData(),
                  authToken.getContent().toUtf8().constData());
    QString message = tr("User authentication token was generated successfully.")
                      + "<ul>"
                      + "<li>" + tr("Resource") + ": " + authToken.getResourceName() + "</li>"
                      + "<li>" + tr("Token") + ": <tt>" + authToken.getContent() + "</tt></li>"
                      + "<li>" + tr("Validity") + ": " + QDateTime::fromSecsSinceEpoch(authToken.getValidityDate()).toString() + "</li>"
                      + "</ul>";
    RMessageBox::information(this,tr("User authentication token was generated"),message);
    this->refreshTokens();
}

void CloudAuthTokenManagerWidget::onUserTokenRemoved(QUuid id)
{
    RLogger::info("Cloud user authentication token was removed id = \'%s\'.\n",
                  id.toString(QUuid::WithoutBraces).toUtf8().constData());
    QString message = tr("User authentication token was removed successfully.")
                      + ": <pre>" + id.toString(QUuid::WithoutBraces) + "</pre>";
    RMessageBox::information(this,tr("User authentication token was removed"),message);
    this->refreshTokens();
}

void CloudAuthTokenManagerWidget::onGenerateClicked()
{
    QString question = tr("Would you like to generate a temporary authentication token for selected user?") + "<pre>" + this->resourceName + "</pre>";

    if (RMessageBox::question(this,tr("Generate user token?"),question) == RMessageBox::Yes)
    {
        try
        {
            this->cloudClient->requestUserTokenGenerate(this->resourceName);
        }
        catch (const RError &rError)
        {
            RLogger::error("Failed to request to generate user token on Cloud. %s\n",rError.getMessage().toUtf8().constData());
            RMessageBox::critical(this,tr("Generation of user token failed"),tr("Generation of temporary user authentication token on Cloud has failed."));
        }
    }
}

void CloudAuthTokenManagerWidget::onRemoveClicked()
{
    QList<QTreeWidgetItem*> selectedItems = this->tokensTree->selectedItems();
    for (QTreeWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        QUuid id = selectedItem->data(ColumnId,Qt::UserRole).toUuid();

        QString question = tr("Are you sure you want to remove selected user authentication token?")
                         + "<pre>" + id.toString(QUuid::WithoutBraces) + "</pre>";

        if (RMessageBox::question(this,tr("Remove user token?"),question) == RMessageBox::Yes)
        {
            try
            {
                this->cloudClient->requestUserTokenRemove(id);
            }
            catch (const RError &rError)
            {
                RLogger::error("Failed to request to remove user token from Cloud. %s\n",rError.getMessage().toUtf8().constData());
                RMessageBox::critical(this,tr("Removal of user authentication token failed"),tr("Removal of temporary user authentication token from Cloud has failed."));
            }
        }
    }
}
