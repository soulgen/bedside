#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <QDebug>
#include <QTimer>

const QString ApplicationHeadless::m_author = "Teleca"; // for creating settings
const QString ApplicationHeadless::m_appName = "BedsideManagerApp"; // for creating settings

// keys for setting file
const QString ApplicationHeadless::m_daily = "Daily";
const QString ApplicationHeadless::m_daily_settings = "DailySettings";
const QString ApplicationHeadless::m_current_settings = "CurrentSettings";
const QString ApplicationHeadless::m_serviceStatus = "ServiceStatus";

using namespace bb::cascades;

ApplicationHeadless::ApplicationHeadless(bb::cascades::Application *app)
    : QObject(app)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);
  //  qmlRegisterUncreatableType<bb::platform::NotificationGlobalSettings>("bb.platform", 1, 0, "NotificationGlobalSettings", "");
    QSettings settings(m_author, m_appName);
    // Force the creation of the settings file so that we can watch it for changes.
    settings.sync();

    // Watcher for changes in the settings file.
    settingsWatcher = new QFileSystemWatcher(this);
    settingsWatcher->addPath(settings.fileName());
    connect(settingsWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(settingsChanged(const QString&)));

    save();
    // initial load
    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    // expose this class to the qml context so that we can query it for the necessary values
    // via properties, slots or invokable methods
    qml->setContextProperty("_app", this);
    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    app->setScene(root);
    isServiceRunning();
}

bool ApplicationHeadless::isServiceRunning()
{
    qDebug() << "check for service running via qsettings...";
    QSettings settings(m_author, m_appName);
    if (settings.value("ServiceStatus").isNull()) {
        qDebug() << "found null value for ServiceStatus key...";
    } else {
        QString status = settings.value("ServiceStatus").toString();
        if (status == "running") {
            settingsChanged("");
            return true;
        }
    }
    return false;
}

bool ApplicationHeadless::getDaily()
{
    QSettings settings(m_author, m_appName);
    if (settings.contains(m_daily)) {
    	qDebug() << "UI: getDaily(" << settings.value(m_daily).toBool() << ")";
        return settings.value(m_daily).toBool();
    }
    return true;
}

void ApplicationHeadless::setDaily(bool checked)
{
    QSettings settings(m_author, m_appName);
    settings.setValue(m_daily, QVariant::fromValue(checked));
    qDebug() << "UI: setDaily(" << checked << ")";
}

bool ApplicationHeadless::getWConnections()
{
    QSettings settings(m_author, m_appName);
    if (settings.contains(m_daily_settings)) {
    	QVariant var = settings.value(m_daily_settings);
    	if(var.canConvert<BedsideSettings>()){
    		BedsideSettings set = var.value<BedsideSettings>();
    		qDebug() << "UI: getWConnections(" << set.w_connections << ")";
    	    return set.w_connections;
    	}
    }
    return false;
}

void ApplicationHeadless::setNotificationMode(int mode)
{
	unsaved_settings.mode = mode;
    qDebug() << "UI: setNotificationMode(" << mode << ")";
}

int ApplicationHeadless::getNotificationMode()
{
    QSettings settings(m_author, m_appName);
    if (settings.contains(m_daily_settings)) {
    	QVariant var = settings.value(m_daily_settings);
    	if(var.canConvert<BedsideSettings>()){
    		BedsideSettings set = var.value<BedsideSettings>();
    		qDebug() << "UI: getNotificationMode(" << set.mode << ")";
    	    return set.mode;
    	}
    }
    return 0;
}

void ApplicationHeadless::setTimeFrom(QDateTime from)
{
	unsaved_settings.from = from;
    qDebug() << "UI: setTimeFrom(" << from.toString() << ")";
}

QDateTime ApplicationHeadless::getTimeFrom()
{
    QSettings settings(m_author, m_appName);
    if (settings.contains(m_daily_settings)) {
    	QVariant var = settings.value(m_daily_settings);
    	if(var.canConvert<BedsideSettings>()){
    		BedsideSettings set = var.value<BedsideSettings>();
    		qDebug() << "UI: getTimeFrom(" << set.from.toString() << ")";
    	    return set.from;
    	}
    }
    return QDateTime();
}

void ApplicationHeadless::setTimeTo(QDateTime to)
{
	unsaved_settings.to = to;
    qDebug() << "UI: setTimeTo(" << to.toString() << ")";
}

QDateTime ApplicationHeadless::getTimeTo()
{
    QSettings settings(m_author, m_appName);
    if (settings.contains(m_daily_settings)) {
    	QVariant var = settings.value(m_daily_settings);
    	if(var.canConvert<BedsideSettings>()){
    		BedsideSettings set = var.value<BedsideSettings>();
    		qDebug() << "UI: getTimeTo(" << set.to.toString() << ")";
    	    return set.to;
    	}
    }
    return QDateTime();
}

void ApplicationHeadless::setWConnections(bool checked)
{
	unsaved_settings.w_connections = checked;
    qDebug() << "UI: setWConnections(" << checked << ")";
}

void ApplicationHeadless::save()
{
    qDebug() << "save()";
    QSettings settings(m_author, m_appName);
    settings.setValue(m_daily_settings, QVariant::fromValue(unsaved_settings));
}

void ApplicationHeadless::cancel()
{
	qDebug() << "cancel()";
}

//void ApplicationHeadless::setRemainingFlashCount(int x)
//{
//    m_remainingFlashCount = x;
//    qDebug() << "emitting update signal flc";
//    Q_EMIT remainingFlashCountChanged();
//}

//int ApplicationHeadless::remainingFlashCount()
//{
//    return m_remainingFlashCount;
//}

void ApplicationHeadless::settingsChanged(const QString & path)
{
	Q_UNUSED(path);
    qDebug() << "UI: Settings has been changed";
    QSettings settings(m_author, m_appName);

    if (settings.contains(m_daily)) {
    	qDebug() << "m_daily = " << settings.value(m_daily).toBool();
    }
}

