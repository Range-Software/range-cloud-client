#include <QVBoxLayout>
#include <QDialogButtonBox>

#include <rcl_group_info.h>

#include "cloud_group_add_dialog.h"

CloudGroupAddDialog::CloudGroupAddDialog(QWidget *parent)
    : QDialog(parent)
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-add.svg");

    this->setWindowTitle(tr("Add user group"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    this->groupNameEdit = new QLineEdit;
    this->groupNameEdit->setPlaceholderText(tr("Group name"));
    mainLayout->addWidget(this->groupNameEdit);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    this->okButton = new QPushButton(okIcon, tr("OK"));
    this->okButton->setDisabled(true);
    buttonBox->addButton(cancelButton,QDialogButtonBox::AcceptRole);

    QObject::connect(this->groupNameEdit,&QLineEdit::textChanged,this,&CloudGroupAddDialog::onGroupNameChanged);
    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

void CloudGroupAddDialog::onGroupNameChanged(const QString &text)
{
    this->okButton->setEnabled(RGroupInfo::isNameValid(text));
}
