#include <QVBoxLayout>
#include <QFormLayout>
#include <QSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>

#include "cloud_file_quota_dialog.h"

CloudFileQuotaDialog::CloudFileQuotaDialog(const RFileQuota &fileQuota, QWidget *parent)
    : QDialog{parent}
    , fileQuota{fileQuota}
{
    QIcon cancelIcon(":/icons/action/pixmaps/range-cancel.svg");
    QIcon okIcon(":/icons/action/pixmaps/range-ok.svg");

    this->setWindowTitle(tr("Edit file quotas"));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    this->setLayout(mainLayout);

    QFormLayout *formLayout = new QFormLayout;
    mainLayout->addLayout(formLayout);

    QSpinBox *storeSizeSpin = new QSpinBox;
    storeSizeSpin->setMinimum(-1);
    storeSizeSpin->setMaximum(INT_MAX);
    storeSizeSpin->setSuffix(" [MB]");
    storeSizeSpin->setValue(CloudFileQuotaDialog::b2mb(fileQuota.getStoreSize()));
    formLayout->addRow(tr("Store size"),storeSizeSpin);

    QSpinBox *fileSizeSpin = new QSpinBox;
    fileSizeSpin->setMinimum(-1);
    fileSizeSpin->setMaximum(INT_MAX);
    fileSizeSpin->setSuffix(" [MB]");
    fileSizeSpin->setValue(CloudFileQuotaDialog::b2mb(fileQuota.getFileSize()));
    formLayout->addRow(tr("File size"),fileSizeSpin);

    QSpinBox *fileCountSpin = new QSpinBox;
    fileCountSpin->setMinimum(-1);
    fileCountSpin->setMaximum(INT_MAX);
    fileCountSpin->setValue(fileQuota.getFileCount());
    formLayout->addRow(tr("File count"),fileCountSpin);

    QObject::connect(storeSizeSpin,&QSpinBox::valueChanged,this,&CloudFileQuotaDialog::onStoreSizeChanged);
    QObject::connect(fileSizeSpin,&QSpinBox::valueChanged,this,&CloudFileQuotaDialog::onFileSizeChanged);
    QObject::connect(fileCountSpin,&QSpinBox::valueChanged,this,&CloudFileQuotaDialog::onFileCountChanged);

    QDialogButtonBox *buttonBox = new QDialogButtonBox;
    mainLayout->addWidget(buttonBox);

    QPushButton *cancelButton = new QPushButton(cancelIcon, tr("Cancel"));
    buttonBox->addButton(cancelButton,QDialogButtonBox::RejectRole);

    QPushButton *okButton = new QPushButton(okIcon, tr("OK"));
    buttonBox->addButton(okButton,QDialogButtonBox::AcceptRole);

    QObject::connect(buttonBox,&QDialogButtonBox::rejected,this,&QDialog::reject);
    QObject::connect(buttonBox,&QDialogButtonBox::accepted,this,&QDialog::accept);
}

const RFileQuota &CloudFileQuotaDialog::getFileQuota() const
{
    return this->fileQuota;
}

int CloudFileQuotaDialog::b2mb(qint64 b)
{
    return int(std::ceil(double(b)/double(1048576)));
}

qint64 CloudFileQuotaDialog::mb2b(int mb)
{
    return qint64(mb)*1048576;
}

void CloudFileQuotaDialog::onStoreSizeChanged(int value)
{
    this->fileQuota.setStoreSize(CloudFileQuotaDialog::mb2b(value));
}

void CloudFileQuotaDialog::onFileSizeChanged(int value)
{
    this->fileQuota.setFileSize(CloudFileQuotaDialog::mb2b(value));
}

void CloudFileQuotaDialog::onFileCountChanged(int value)
{
    this->fileQuota.setFileCount(value);
}
