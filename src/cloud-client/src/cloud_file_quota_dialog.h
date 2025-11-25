#ifndef CLOUD_FILE_QUOTA_DIALOG_H
#define CLOUD_FILE_QUOTA_DIALOG_H

#include <QDialog>

#include <rcl_file_quota.h>

class CloudFileQuotaDialog : public QDialog
{
    Q_OBJECT

    //! File quota.
    RFileQuota fileQuota;

    public:

        //! Constructor.
        explicit CloudFileQuotaDialog(const RFileQuota &fileQuota, QWidget *parent = nullptr);

        //! Return const reference to file quota.
        const RFileQuota &getFileQuota() const;;

    private:

        static int b2mb(qint64 b);
        static qint64 mb2b(int mb);

    protected slots:

        void onStoreSizeChanged(int value);
        void onFileSizeChanged(int value);
        void onFileCountChanged(int value);

};

#endif // CLOUD_FILE_QUOTA_DIALOG_H
