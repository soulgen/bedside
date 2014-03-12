#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QFileSystemWatcher>
#include <QDateTime>
#include <bb/platform/NotificationMode>

struct BedsideSettings{
	bool w_connections;
	int mode;
	QDateTime from;
	QDateTime to;
};

Q_DECLARE_METATYPE(BedsideSettings)

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
    }
}

class QTranslator;

class ApplicationHeadless: public QObject
{
    Q_OBJECT
    // property holding value for remaining flash count
    //Q_PROPERTY(int remainingFlashCount READ remainingFlashCount WRITE setRemainingFlashCount NOTIFY remainingFlashCountChanged)

public:
    ApplicationHeadless(bb::cascades::Application *app);
    virtual ~ApplicationHeadless() {}
    //Q_INVOKABLE void save();
    //Q_INVOKABLE void cancel();

Q_SIGNALS:
    // Emitted when the remaining flash count value has changed
    void remainingFlashCountChanged();

public Q_SLOTS:
    /**
     * Method that can be invoked from within qml
     * to read QSettings key/value for ServiceStatus,
     * in order to decipher if service is running (active).
     */
    bool isServiceRunning();

    /**
     * Method hooked into the signal/slots mechanism, which
     * gets invoked upon receiving fileChanged() signal
     * from the settingsWatcher instance.
     */
    void settingsChanged(const QString & path);

    bool getDaily();
    void setDaily(bool);

    bool getWConnections();
    void setWConnections(bool);

    int getNotificationMode();
    void setNotificationMode(int);

    QDateTime getTimeFrom();
    void setTimeFrom(QDateTime);

    QDateTime getTimeTo();
    void setTimeTo(QDateTime);

    void save();
    void cancel();

private:
    static const QString m_author; // for creating settings
    static const QString m_appName; // for creating settings
    static const QString m_daily; // for daily check box
    static const QString m_daily_settings; // for daily settings
    static const QString m_current_settings; // for storing current settings
    static const QString m_serviceStatus; // for checking service running

    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
    // QTimer used to periodically read QSettings to retrieve
    // new remaining flash count value
    int m_remainingFlashCount;
    // setter to store the new remaining flash count value
    void setRemainingFlashCount(int x);
    // Convenience method to retrieve remaining flash count.
    int remainingFlashCount();
    // Watcher for qsettigns file changes
    QFileSystemWatcher* settingsWatcher;

    BedsideSettings unsaved_settings;
};

#endif /* ApplicationUI_HPP_ */
