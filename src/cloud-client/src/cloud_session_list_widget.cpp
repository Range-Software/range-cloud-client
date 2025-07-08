#include <QVBoxLayout>
#include <QGridLayout>

#include <rgl_message_box.h>

#include "cloud_session_list_widget.h"

CloudSessionListWidget::CloudSessionListWidget(RCloudSessionManager *cloudSession, QWidget *parent)
    : QWidget{parent}
    , cloudSession{cloudSession}
{
    QIcon activateIcon(":/icons/action/pixmaps/range-ok.svg");
    QIcon addIcon(":/icons/action/pixmaps/range-add.svg");
    QIcon removeIcon(":/icons/action/pixmaps/range-remove.svg");

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->nameList = new QListWidget;
    this->nameList->setSelectionMode(QAbstractItemView::SingleSelection);
    QStringList sessionNames = this->cloudSession->getSessionNames();
    for (const QString &sessionName : std::as_const(sessionNames))
    {
        this->addItem(sessionName);
    }
    mainLayout->addWidget(this->nameList);

    QGridLayout *buttonsLayout = new QGridLayout;
    mainLayout->addLayout(buttonsLayout);

    this->setActiveButton = new QPushButton(activateIcon, tr("Set active"));
    this->setActiveButton->setDisabled(true);
    buttonsLayout->addWidget(this->setActiveButton,0,0,1,2);

    this->addButton = new QPushButton(addIcon, tr("Add"));
    buttonsLayout->addWidget(this->addButton,1,0,1,1);

    this->removeButton = new QPushButton(removeIcon, tr("Remove"));
    this->removeButton->setDisabled(true);
    buttonsLayout->addWidget(this->removeButton,1,1,1,1);

    QObject::connect(this->nameList,&QListWidget::itemChanged,this,&CloudSessionListWidget::onNameItemChanged);
    QObject::connect(this->nameList,&QListWidget::itemSelectionChanged,this,&CloudSessionListWidget::onNameSelectionChanged);
    QObject::connect(this->setActiveButton,&QPushButton::clicked,this,&CloudSessionListWidget::onSetActiveButtonClicked);
    QObject::connect(this->addButton,&QPushButton::clicked,this,&CloudSessionListWidget::onAddButtonClicked);
    QObject::connect(this->removeButton,&QPushButton::clicked,this,&CloudSessionListWidget::onRemoveButtonClicked);
    QObject::connect(this->cloudSession,&RCloudSessionManager::activeSessionChanged,this,&CloudSessionListWidget::onActiveSessionChanged);
}

void CloudSessionListWidget::selectSession(const QString &sessionName)
{
    // Select active session.
    for (int i=0;i<this->nameList->count();i++)
    {
        if (this->nameList->item(i)->text() == sessionName)
        {
            this->nameList->item(i)->setSelected(true);
            break;
        }
    }
}

void CloudSessionListWidget::addItem(const QString &name)
{
    QListWidgetItem *item = new QListWidgetItem(this->nameList);
    item->setText(name);
    item->setData(Qt::UserRole,name);
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    if (this->cloudSession->findActiveSessionName() == name)
    {
        QFont itemFont = item->font();
        itemFont.setBold(true);
        item->setFont(itemFont);
    }
}

void CloudSessionListWidget::onNameItemChanged(QListWidgetItem *item)
{
    this->cloudSession->renameSession(item->data(Qt::UserRole).toString(),item->text());
    item->setData(Qt::UserRole,item->text());
    this->onNameSelectionChanged();
}

void CloudSessionListWidget::onNameSelectionChanged()
{
    RCloudSessionInfo sessionInfo;
    bool disableButtons = true;
    QList<QListWidgetItem*> selectedItems = this->nameList->selectedItems();
    for (QListWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        sessionInfo = this->cloudSession->findSession(selectedItem->text());
        disableButtons = false;
    }
    this->setActiveButton->setDisabled(disableButtons);
    this->removeButton->setDisabled(disableButtons);
    emit this->sessionSelected(sessionInfo);
}

void CloudSessionListWidget::onSetActiveButtonClicked()
{
    QList<QListWidgetItem*> selectedItems = this->nameList->selectedItems();
    for (QListWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        this->cloudSession->setActiveSessionName(selectedItem->text());
    }
}

void CloudSessionListWidget::onAddButtonClicked()
{
    RCloudSessionInfo sessionInfo = RCloudSessionManager::getDefaultCloudSession();
    sessionInfo.setName(this->cloudSession->guessNewSessionName());

    this->cloudSession->insertSession(sessionInfo);

    this->addItem(sessionInfo.getName());
    this->selectSession(sessionInfo.getName());
}

void CloudSessionListWidget::onRemoveButtonClicked()
{
    QList<QListWidgetItem*> selectedItems = this->nameList->selectedItems();
    for (QListWidgetItem *selectedItem : std::as_const(selectedItems))
    {
        QString message = tr("Are you sure you want to permanently remove selected session?") + " <b>\"" + selectedItem->text() + "\"</b>";
        if (RMessageBox::question(this,tr("Remove selected session?"),message) == RMessageBox::Yes)
        {
            this->cloudSession->removeSession(selectedItem->text());
            delete selectedItem;
        }
    }
}

void CloudSessionListWidget::onActiveSessionChanged(RCloudSessionInfo sessionInfo)
{
    for (int i=0;i<this->nameList->count();i++)
    {
        QListWidgetItem *item = this->nameList->item(i);
        QFont itemFont = item->font();
        itemFont.setBold(item->text() == sessionInfo.getName());
        item->setFont(itemFont);
    }
}
