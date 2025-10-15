#include <QHBoxLayout>
#include <QLabel>

#include "cloud_session_combo_box.h"

CloudSessionComboBox::CloudSessionComboBox(RCloudSessionManager *cloudSessionManager, QWidget *parent)
    : QWidget{parent}
    , cloudSessionManager{cloudSessionManager}
{
    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->setContentsMargins(0,0,0,0);
    this->setLayout(mainLayout);

    QLabel *label = new QLabel(tr("Session") + ":");
    mainLayout->addWidget(label);

    this->comboBox = new QComboBox;
    QStringList sessionNames = this->cloudSessionManager->getSessionNames();
    for (const QString &name : std::as_const(sessionNames))
    {
        this->comboBox->addItem(name);
    }
    this->comboBox->setCurrentText(this->cloudSessionManager->findActiveSessionName());
    mainLayout->addWidget(this->comboBox);

    QObject::connect(this->cloudSessionManager,&RCloudSessionManager::activeSessionChanged,this,&CloudSessionComboBox::onActiveSessionChanged);
    QObject::connect(this->cloudSessionManager,&RCloudSessionManager::sessionInserted,this,&CloudSessionComboBox::onSessionInserted);
    QObject::connect(this->cloudSessionManager,&RCloudSessionManager::sessionRenamed,this,&CloudSessionComboBox::onSessionRenamed);
    QObject::connect(this->cloudSessionManager,&RCloudSessionManager::sessionRemoved,this,&CloudSessionComboBox::onSessionRemoved);
    QObject::connect(this->comboBox,&QComboBox::currentTextChanged,this,&CloudSessionComboBox::onComboBoxCurrentTextChanged);

}

void CloudSessionComboBox::onActiveSessionChanged(RCloudSessionInfo sessionInfo)
{
    bool signalsBlockedSaved = this->comboBox->signalsBlocked();
    this->comboBox->blockSignals(true);
    this->comboBox->setCurrentText(sessionInfo.getName());
    this->comboBox->blockSignals(signalsBlockedSaved);
}

void CloudSessionComboBox::onSessionInserted(const QString &sessionName)
{
    if (this->comboBox->findText(sessionName) == -1)
    {
        bool signalsBlockedSaved = this->comboBox->signalsBlocked();
        this->comboBox->blockSignals(true);
        this->comboBox->addItem(sessionName);
        this->comboBox->setCurrentText(this->cloudSessionManager->findActiveSessionName());
        this->comboBox->blockSignals(signalsBlockedSaved);
    }
}

void CloudSessionComboBox::onSessionRenamed(const QString &oldSessionName,
                                            const QString &newSessionName)
{
    for (int i=0;i<this->comboBox->count();i++)
    {
        if (this->comboBox->itemText(i) == oldSessionName)
        {
            this->comboBox->setItemText(i,newSessionName);
        }
    }
}

void CloudSessionComboBox::onSessionRemoved(const QString &sessionName)
{
    for (int i=this->comboBox->count()-1;i>=0;i--)
    {
        if (this->comboBox->itemText(i) == sessionName)
        {
            this->comboBox->removeItem(i);
        }
    }
}

void CloudSessionComboBox::onComboBoxCurrentTextChanged(const QString &text)
{
    this->cloudSessionManager->setActiveSessionName(text);
}
