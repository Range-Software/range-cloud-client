#include <QVBoxLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QDialogButtonBox>

#include "cloud_process_execute_dialog.h"

CloudProcessArgumentLineEdit::CloudProcessArgumentLineEdit(const QString &placeholder, QWidget *parent)
    : QLineEdit(parent)
    , placeholder(placeholder)
{
    this->setPlaceholderText(placeholder);
    QObject::connect(this,&QLineEdit::textChanged,this,&CloudProcessArgumentLineEdit::onTextChanged);
}

void CloudProcessArgumentLineEdit::onTextChanged(const QString &text)
{
    emit this->valueChanged(this->placeholder,text);
}

CloudProcessExecuteDialog::CloudProcessExecuteDialog(const RCloudProcessInfo &processInfo, QWidget *parent)
    : QDialog{parent}
    , processInfo{processInfo}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon executeIcon(":/icons/cloud/pixmaps/range-execute.svg");

    this->setWindowTitle(tr("Execute process"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    formLayout->addRow(tr("Name") + ":",new QLabel(processInfo.getName()));

    for (const QString &argument : processInfo.getArguments())
    {
        QStringList argumentList = argument.split("=");

        if (argumentList.size() > 1)
        {
            CloudProcessArgumentLineEdit *argumentEdit = new CloudProcessArgumentLineEdit(argumentList.at(1));
            QObject::connect(argumentEdit,&CloudProcessArgumentLineEdit::valueChanged,this,&CloudProcessExecuteDialog::onArgumentEditValueChanged);

            formLayout->addRow(argumentList.at(1) + ":",argumentEdit);
        }
    }

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    QPushButton *executeButton = new QPushButton(executeIcon, tr("Execute"));
    buttonBox->addButton(executeButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

const QMap<QString, QString> &CloudProcessExecuteDialog::getArguments() const
{
    return this->arguments;
}

void CloudProcessExecuteDialog::onArgumentEditValueChanged(const QString &placeholder, const QString &value)
{
    QString key = placeholder;
    key.erase(key.cbegin());
    key.erase(key.cend()-1);
    this->arguments[key] = value;
}
