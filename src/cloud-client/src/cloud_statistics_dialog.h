#ifndef CLOUD_STATISTICS_DIALOG_H
#define CLOUD_STATISTICS_DIALOG_H

#include <QDialog>
#include <QTimer>

#include <rcl_cloud_client.h>

#include <rgl_text_browser.h>

class CloudStatisticsDialog : public QDialog
{
    Q_OBJECT

    protected:

        RCloudClient *cloudClient;
        RTextBrowser *textBrowser;

        QTimer *refreshTimer;

    public:

        //! Constructor.
        CloudStatisticsDialog(QWidget *parent = nullptr);

    private:

        //! Request statistics.
        void requestStatistics();

    protected slots:

        //! Refresh button was clicked.
        void onRefreshButtonClicked();

        //! Refresh timeout.
        void onRefreshTimeout();

        //! Refresh timeout has changed.
        void onRefreshTimeoutChanged(uint refreshTimeout);

        //! Cloud client configuratin has changed.
        void onClientConfigurationChanged();

        //! Statistics data are available.
        void onStatisticsAvailable(const QString &statistics);

};

#endif // CLOUD_STATISTICS_DIALOG_H
