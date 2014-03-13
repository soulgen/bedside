#include "applicationheadless.hpp"

#include <bb/Application>
#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <QDebug>
#include <bb/platform/NotificationGlobalSettings>
#include <bb/device/WiFiDirect>
#include <wifi/wifi_service.h>

const QString BedsideManagerService::m_author = "Teleca"; // for creating settings
const QString BedsideManagerService::m_appName = "BedsideManagerApp"; // for creating settings

// keys for setting file
const QString BedsideManagerService::m_daily = "Daily";
const QString BedsideManagerService::m_daily_settings = "DailySettings";
const QString BedsideManagerService::m_current_settings = "CurrentSettings";
const QString BedsideManagerService::m_serviceStatus = "ServiceStatus";

using namespace bb::device;
using namespace bb::system;
using namespace bb::platform;

QDataStream& operator<<(QDataStream& out, const BedsideSettings& v) {
    out << v.w_connections << v.mode << v.from << v.to;
    return out;
}

QDataStream& operator>>(QDataStream& in, BedsideSettings& v) {
    in >> v.w_connections;
    in >> v.mode;
    in >> v.from;
    in >> v.to;
    return in;
}

BedsideManagerService::BedsideManagerService(bb::Application *app)
    : QObject(app)
    , m_invokeManager(new InvokeManager(this))
    , m_glsettings(new NotificationGlobalSettings(this))
    , m_settingsWatcher(new QFileSystemWatcher(this))
{
	qRegisterMetaTypeStreamOperators<BedsideSettings>("BedsideSettings");
    QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
    // log the service PID
    qDebug() << "PID------------" << QString::number(QCoreApplication::applicationPid());
}

void BedsideManagerService::onInvoked(const bb::system::InvokeRequest& request)
{
    qDebug() << "##service got invoked: " << request.action();

    // start led flashing once the start request is received
    if (request.action().compare("bb.action.system.STARTED") == 0) {
        //m_led->flash(m_flashCount);
    } else {
        // write service running status to qsettings
        QSettings settings(m_author, m_appName);
        settings.setValue(m_serviceStatus, request.action());
    }
}
/*
void BedsideManagerService::flashCountChanged(int x)
{
   // WiFiDirect wifisettings;
	wifi_result_t res;
	    res = wifi_service_initialize(&m_wifis);
	    qDebug()<<"GLOBAL SETTINGS res="<<res ;
	//wifi_status_t *st;
//	result1=wifi_get_status(st);
	//qDebug() << "The result1 is " <<  ;

    wifi_set_sta_power(false);



    m_glsettings->setMode(NotificationMode::Silent);
    qDebug()<<"GLOBAL SETTINGS 3 mode="<<m_glsettings->mode();
  //  qDebug()<<"GLOBAL SETTINGS 4 wifisettings.isenabled="<<wifisettings.isEnabled();
   // qDebug()<<"GLOBAL SETTINGS 5 wifisettings.connectionState="<<wifisettings.connectionState();


  //  NotificationGlobalSettings globalsettings;

   //    globalsettings.setMode(NotificationMode::Silent);


  //  qDebug()<<"GLOBAL SETTINGS 4 mode="<<globalsettings.mode();

    qDebug() << "---------" + QString::number(x);
    QSettings settings(m_author, m_appName);
    settings.setValue(m_remainingCount, x);
}

void BedsideManagerService::activeUpdate(bool active)
{
    qDebug() << "---active: " << active;
    QSettings settings(m_author, m_appName);
    settings.value(m_ledActive, active);
    if(!active) {
        QSettings settings(m_author, m_appName);
        settings.setValue(m_remainingCount, m_led->remainingFlashCount());
    }
}
*/
void BedsideManagerService::settingsChanged(const QString & path)
{
	Q_UNUSED(path);
	/*
    QSettings settings(m_author, m_appName);
    if (settings.value(m_reset).toBool()) {
        settings.setValue(m_reset, false);
        settings.setValue(m_flashNumber, m_flashCount);
        settings.setValue(m_remainingCount, m_flashCount);
        settings.value(m_ledActive, false);
        m_led->cancel();
        disconnect(m_led, SIGNAL(remainingFlashCountChanged(int)), this, SLOT(flashCountChanged(int)));
        disconnect(m_led, SIGNAL(activeChanged(bool)), this, SLOT(activeUpdate(bool)));
        delete m_led;
        m_led = new Led(LedColor::Blue, this);
        bool ok = connect(m_led, SIGNAL(remainingFlashCountChanged(int)), this, SLOT(flashCountChanged(int)));
        Q_ASSERT(ok);
        ok = connect(m_led, SIGNAL(activeChanged(bool)), this, SLOT(activeUpdate(bool)));
        Q_ASSERT(ok);
        Q_UNUSED(ok);
        m_led->flash(m_flashCount);

        //m_globalsettings->setMode( NotificationMode::Vibrate);

    }
    */
	qDebug() << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
}

void BedsideManagerService::init()
{
	qDebug() << "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB";
    // set the initial qsettings keys/values upon startup
    QSettings settings(m_author, m_appName);
 //   NotificationGlobalSettings globalsettings;
  //  globalsettings.setMode(NotificationMode::Silent);

    settings.setValue(m_serviceStatus, "running");
//    settings.setValue(m_flashNumber, m_flashCount);
//    settings.setValue(m_remainingCount, m_flashCount);
    // Force the creation of the settings file so that we can watch it for changes.
    settings.sync();

    // Watcher for changes in the settings file.
    m_settingsWatcher->addPath(settings.fileName());

    // Do all the necessary signal/slot connections to be invokable to receive led updates.
    bool ok = connect(m_invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), this, SLOT(onInvoked(const bb::system::InvokeRequest&)));
    Q_ASSERT (ok);

    //ok = connect(m_led, SIGNAL(remainingFlashCountChanged(int)), this, SLOT(flashCountChanged(int)));
    //Q_ASSERT(ok);

    //ok = connect(m_led, SIGNAL(activeChanged(bool)), this, SLOT(activeUpdate(bool)));
    //Q_ASSERT(ok);

    ok = connect(m_settingsWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(settingsChanged(const QString&)));
    Q_ASSERT(ok);
    Q_UNUSED(ok);
}

