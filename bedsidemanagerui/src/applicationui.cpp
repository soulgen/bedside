#include "applicationui.hpp"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/LocaleHandler>
#include <QDebug>

const QString BedsideManagerUI::m_author = "Teleca"; // for creating settings
const QString BedsideManagerUI::m_appName = "BedsideManagerApp"; // for creating settings

// keys for setting file
const QString BedsideManagerUI::m_daily = "Daily";
const QString BedsideManagerUI::m_daily_settings = "DailySettings";
const QString BedsideManagerUI::m_current_settings = "CurrentSettings";
const QString BedsideManagerUI::m_serviceStatus = "ServiceStatus";

using namespace bb::cascades;

static bool sceneReady = false;

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


BedsideManagerUI::BedsideManagerUI(bb::cascades::Application *app)
    : QObject(app)
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

	qRegisterMetaTypeStreamOperators<BedsideSettings>("BedsideSettings");
  //  qmlRegisterUncreatableType<bb::platform::NotificationGlobalSettings>("bb.platform", 1, 0, "NotificationGlobalSettings", "");
    QSettings settings(m_author, m_appName);
    // Force the creation of the settings file so that we can watch it for changes.
    settings.sync();

    // Watcher for changes in the settings file.
    settingsWatcher = new QFileSystemWatcher(this);
    settingsWatcher->addPath(settings.fileName());
    connect(settingsWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(settingsChanged(const QString&)));

    // initial load
    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    // expose this class to the qml context so that we can query it for the necessary values
    // via properties, slots or invokable methods
    qml->setContextProperty("_app", this);
    // Create root object for the UI
    root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    app->setScene(root);
    sceneReady = true;

    readSettings();
    isServiceRunning();
}

void BedsideManagerUI::readSettings()
{
    QVariant var;
    saved_settings.w_connections = root->property("w_connections_checked").toBool();
    saved_settings.mode = root->property("mode_index").toInt();
    var = root->property("from_value");
    saved_settings.from = var.value<QDateTime>();
    var = root->property("to_value");
    saved_settings.to = var.value<QDateTime>();
}

void BedsideManagerUI::enableButtons()
{
	if(sceneReady) {
	    root->setProperty("save_enabled", true);
	    root->setProperty("cancel_enabled", true);
	}
}

void BedsideManagerUI::disableButtons()
{
	if(sceneReady) {
	    root->setProperty("save_enabled", false);
	    root->setProperty("cancel_enabled", false);
	}
}

bool BedsideManagerUI::isServiceRunning()
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

bool BedsideManagerUI::getServiceStatus()
{
    QSettings settings(m_author, m_appName);
    return settings.value(m_serviceStatus).toBool();
}

void BedsideManagerUI::setServiceStatus(bool status)
{
    qDebug() << "set service status to" << status;
    QSettings settings(m_author, m_appName);
    settings.setValue(m_serviceStatus, QVariant::fromValue(status));
}

bool BedsideManagerUI::getDaily()
{
    QSettings settings(m_author, m_appName);
    if (settings.contains(m_daily)) {
    	qDebug() << "UI: getDaily(" << settings.value(m_daily).toBool() << ")";
        return settings.value(m_daily).toBool();
    }
    return true;
}

void BedsideManagerUI::setDaily(bool checked)
{
    QSettings settings(m_author, m_appName);
    settings.setValue(m_daily, QVariant::fromValue(checked));
    qDebug() << "UI: setDaily(" << checked << ")";
}

bool BedsideManagerUI::getWConnections()
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

void BedsideManagerUI::setWConnections(bool checked)
{
	if(saved_settings.w_connections != checked)	{
	    unsaved_settings.w_connections = checked;
	    enableButtons();
	}
	else
		disableButtons();

    qDebug() << "UI: setWConnections(" << checked << ")";
}

int BedsideManagerUI::getNotificationMode()
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

void BedsideManagerUI::setNotificationMode(int mode)
{
	if(saved_settings.mode != mode)	{
	    unsaved_settings.mode = mode;
	    enableButtons();
	}
	else
		disableButtons();

    qDebug() << "UI: setNotificationMode(" << mode << ")";
}

QDateTime BedsideManagerUI::getTimeFrom()
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
    return QDateTime(QDate::currentDate(), QTime(22, 0, 0));
}

void BedsideManagerUI::setTimeFrom(QDateTime from)
{
	if(saved_settings.from != from)	{
	    unsaved_settings.from = from;
	    enableButtons();
	}
	else
		disableButtons();

    qDebug() << "UI: setTimeFrom(" << from.toString() << ")";
}

QDateTime BedsideManagerUI::getTimeTo()
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
    return QDateTime(QDate::currentDate(), QTime(6, 0, 0));
}

void BedsideManagerUI::setTimeTo(QDateTime to)
{
	if(saved_settings.to != to)	{
	    unsaved_settings.to = to;
	    enableButtons();
	}
	else
		disableButtons();

    qDebug() << "UI: setTimeTo(" << to.toString() << ")";
}

void BedsideManagerUI::save()
{
    qDebug() << "save()";
    QSettings settings(m_author, m_appName);
    settings.setValue(m_daily_settings, QVariant::fromValue(unsaved_settings));
    readSettings();
    disableButtons();
}

void BedsideManagerUI::cancel()
{
	qDebug() << "cancel()";
    root->setProperty("w_connections_checked", saved_settings.w_connections);
    root->setProperty("mode_index", saved_settings.mode);
    root->setProperty("from_value", saved_settings.from);
    root->setProperty("to_value", saved_settings.to);
	disableButtons();
}

void BedsideManagerUI::settingsChanged(const QString & path)
{
	Q_UNUSED(path);
    qDebug() << "UI: Settings has been changed";
    QSettings settings(m_author, m_appName);

    if (settings.contains(m_daily)) {
    	qDebug() << "UI: m_daily = " << settings.value(m_daily).toBool();
    }
    if (settings.contains(m_daily_settings)) {
     	QVariant var = settings.value(m_daily_settings);
   		BedsideSettings set = var.value<BedsideSettings>();
   		qDebug() << "UI: w_connections = " << set.w_connections;
   		qDebug() << "UI: mode = " << set.mode;
   		qDebug() << "UI: From = " << set.from.toString();
   		qDebug() << "UI: To = " << set.to.toString();
    }
}

