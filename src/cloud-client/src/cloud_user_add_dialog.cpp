#include <QVBoxLayout>
#include <QFormLayout>
#include <QDialogButtonBox>

#include <rcl_user_info.h>

#include "cloud_user_add_dialog.h"

CloudUserAddDialog::CloudUserAddDialog(QWidget *parent)
    : QDialog(parent)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-add.svg");

    this->setWindowTitle(tr("Add user"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->userNameEdit = new QLineEdit;
    this->userNameEdit->setPlaceholderText(tr("User name"));
    mainLayout->addWidget(this->userNameEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->okButton = new QPushButton(okIcon, tr("OK"));
    this->okButton->setDisabled(true);
    buttonBox->addButton(this->okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);

    QObject::connect(this->userNameEdit,&QLineEdit::textChanged,this,&CloudUserAddDialog::onUserNameChanged);
}

void CloudUserAddDialog::onUserNameChanged(const QString &text)
{
    this->okButton->setEnabled(RUserInfo::isNameValid(text));
}
