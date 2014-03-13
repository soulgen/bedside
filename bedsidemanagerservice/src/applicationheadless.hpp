#ifndef ApplicationHeadless_HPP_
#define ApplicationHeadless_HPP_

#include <QObject>
#include <QTime>
#include <QFileSystemWatcher>
#include <wifi/wifi_service.h>
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
    class Application;
    namespace system
    {
        class InvokeManager;
        class InvokeRequest;
    }
    namespace platform
    {
      class NotificationGlobalSettings;
    }
}

class BedsideManagerService: public QObject
{
    Q_OBJECT
public:
    BedsideManagerService(bb::Application *app);
    virtual ~BedsideManagerService()
    {
    }

public Q_SLOTS:
    /**
     * Method invoked by the invocation framework
     * upon bb.action.system.STARTED. Meaning when this
     * service is install for the first time and upon startup
     * of the system.
     */
    void onInvoked(const bb::system::InvokeRequest& request);

    /**
     * Method hooked into the signal/slots mechanism, which
     * gets invoked upon receiving remainingFlashCountChanged() signal
     * from the Led instance.
     */
    //void flashCountChanged(int x);

    /**
     * Method hooked into the signal/slots mechanism, which
     * gets invoked upon receiving fileChanged() signal
     * from the settingsWatcher instance.
     */
    void settingsChanged(const QString & path);

    /**
     * Method invoked when a activeChanged() signal is
     * received from the Led instance, indicating
     * a led state change.
     */
    //void activeUpdate(bool active);

    /**
     * Initialization method to create QSettings entries
     * and prepare Led instance.
     */
    void init();

private:
    // hasdf
    // Invocation Manager instance for receiving system events
    bb::system::InvokeManager *m_invokeManager;
    bb::platform::NotificationGlobalSettings *m_glsettings;
    wifi_service_t *m_wifis;

    // keys used in setting
    static const QString m_author; // for creating settings
    static const QString m_appName; // for creating settings
    static const QString m_daily; // for daily check box
    static const QString m_daily_settings; // for daily settings
    static const QString m_current_settings; // for storing current settings
    static const QString m_serviceStatus; // for checking service running

    // Watcher for qsettigns file changes
    QFileSystemWatcher* m_settingsWatcher;
};

#endif /* ApplicationHeadless_HPP_ */
