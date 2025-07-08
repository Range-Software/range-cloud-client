#ifndef CENTRAL_WIDGET_H
#define CENTRAL_WIDGET_H

#include <QUuid>
#include <QTabWidget>

#include <rbl_message.h>

#include <rgl_text_browser.h>

class CentralWidget : public QWidget
{
    Q_OBJECT

    protected:

        //! Tab widget.
        QTabWidget *tabWidget;
        //! Log output browser.
        RTextBrowser *applicationOutputBrowser;
        //! File manager tab position.
        int fileManasgerTabPosition;
        //! User manager tab position.
        int userManagerTabPosition;
        //! Output tab position.
        int outputTabPosition;
    public:

        enum TabType
        {
            FileManager = 0,
            UserManager,
            Output
        };

    public:

        //! Constructor.
        explicit CentralWidget(QWidget *parent = nullptr);

    private:

        //! Set text and icon for given tab.
        void setTabTitle(CentralWidget::TabType tabType, RMessage::Type messageType = RMessage::Type::None, const QString &additionalText = QString());

    private slots:

        //! Print application info message.
        void onInfoPrinted(const QString &message);

        //! Print application notice message.
        void onNoticePrinted(const QString &message);

        //! Print application warning message.
        void onWarningPrinted(const QString &message);

        //! Print application error message.
        void onErrorPrinted(const QString &message);

        //! Central tab widget current changed.
        void onCurrentChanged(int tabPosition);

        //! Get tab name.
        static QString getTabName(TabType tabType);

};

#endif // CENTRAL_WIDGET_H
