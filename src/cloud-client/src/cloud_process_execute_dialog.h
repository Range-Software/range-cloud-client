#ifndef CLOUD_PROCESS_EXECUTE_DIALOG_H
#define CLOUD_PROCESS_EXECUTE_DIALOG_H

#include <QDialog>
#include <QWidget>
#include <QLineEdit>
#include <QMap>

#include <rcl_cloud_process_info.h>

class CloudProcessArgumentLineEdit : public QLineEdit
{

    Q_OBJECT

    protected:

        QString placeholder;

    public:
        explicit CloudProcessArgumentLineEdit(const QString &placeholder, QWidget *parent = nullptr);

    protected slots:

        void onTextChanged(const QString &text);

    signals:

        void valueChanged(const QString &placeholder, const QString &value);

};

class CloudProcessExecuteDialog : public QDialog
{

    Q_OBJECT

    protected:

        RCloudProcessInfo processInfo;

        QMap<QString,QString> arguments;

    public:

        //! Constructor.
        explicit CloudProcessExecuteDialog(const RCloudProcessInfo &processInfo, QWidget *parent = nullptr);

        //! Return const reference to arguments.
        const QMap<QString,QString> &getArguments() const;

    protected slots:

        void onArgumentEditValueChanged(const QString &placeholder, const QString &value);

};

#endif // CLOUD_PROCESS_EXECUTE_DIALOG_H
