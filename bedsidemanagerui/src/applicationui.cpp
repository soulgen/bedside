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
const QString BedsideManagerUI::m_monday_settings = "MondaySettings";
const QString BedsideManagerUI::m_tuesday_settings = "TuesdaySettings";
const QString BedsideManagerUI::m_wednesday_settings = "WednesdaySettings";
const QString BedsideManagerUI::m_thursday_settings = "ThursdaySettings";
const QString BedsideManagerUI::m_friday_settings = "FridaySettings";
const QString BedsideManagerUI::m_saturday_settings = "SaturdaySettings";
const QString BedsideManagerUI::m_sunday_settings = "SundaySettings";
const QString BedsideManagerUI::m_monitoring_status = "MonitoringStatus";
const QString BedsideManagerUI::m_selected_day = "SelectedDay";
const QString BedsideManagerUI::m_serviceStatus = "ServiceStatus";

using namespace bb::cascades;

static bool sceneReady = false;

QDataStream& operator<<(QDataStream& out, const BedsideSettings& v) {
    out << v.isActive << v.mode << v.from << v.to;
    return out;
}

QDataStream& operator>>(QDataStream& in, BedsideSettings& v) {
    in >> v.isActive;
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

    init();
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

    isServiceRunning();
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
        if (true == settings.value("ServiceStatus").toBool()) {
            settingsChanged("");
            return true;
        }
    }
    return false;
}

bool BedsideManagerUI::getMonitoringStatus()
{
    QSettings settings(m_author, m_appName);

    if (settings.contains(m_monitoring_status)) {
        return settings.value(m_monitoring_status).toBool();
    }
    else
    	return true;
}

void BedsideManagerUI::setMonitoringStatus(bool status)
{
    qDebug() << "set monitoring status to" << status;
    QSettings settings(m_author, m_appName);
    settings.setValue(m_monitoring_status, QVariant::fromValue(status));
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

int BedsideManagerUI::getSelectedDay()
{
    QSettings settings(m_author, m_appName);
    if (settings.contains(m_selected_day)) {
    	return settings.value(m_selected_day).toInt();
    }
    return 0;
}

void BedsideManagerUI::setSelectedDay(int day)
{
    QSettings settings(m_author, m_appName);
    settings.setValue(m_selected_day, QVariant::fromValue(day));
}

bool BedsideManagerUI::getServiceStatus()
{
    return getVisibleSettings().isActive;
}

void BedsideManagerUI::setServiceStatus(bool status)
{
    QSettings settings(m_author, m_appName);
    if(saved_settings.isActive != status) {
    	unsaved_settings.isActive = status;
    	enableButtons();
    }
    else
    	disableButtons();
}

int BedsideManagerUI::getNotificationMode()
{
	return getVisibleSettings().mode;
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
    return getVisibleSettings().from;
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
    return getVisibleSettings().to;
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
    QSettings settings(m_author, m_appName);

    if(settings.value(m_daily).toBool())
    	settings.setValue(m_daily_settings, QVariant::fromValue(unsaved_settings));
    else {
    	int index = settings.value(m_selected_day).toInt();
    	switch(index) {
    	   case 0: settings.setValue(m_monday_settings, QVariant::fromValue(unsaved_settings)); break;
    	   case 1: settings.setValue(m_tuesday_settings, QVariant::fromValue(unsaved_settings)); break;
    	   case 2: settings.setValue(m_wednesday_settings, QVariant::fromValue(unsaved_settings)); break;
    	   case 3: settings.setValue(m_thursday_settings, QVariant::fromValue(unsaved_settings)); break;
    	   case 4: settings.setValue(m_friday_settings, QVariant::fromValue(unsaved_settings)); break;
    	   case 5: settings.setValue(m_saturday_settings, QVariant::fromValue(unsaved_settings)); break;
    	   case 6: settings.setValue(m_sunday_settings, QVariant::fromValue(unsaved_settings)); break;
    	}
    }

    saved_settings = unsaved_settings;
    disableButtons();
}

void BedsideManagerUI::cancel()
{
	updateLayoutFromSettings();
	disableButtons();
}

void BedsideManagerUI::settingsChanged(const QString & path)
{
	Q_UNUSED(path);
    qDebug() << "UI: Settings has been changed";

    updateLayoutFromSettings();

    saved_settings = unsaved_settings = getVisibleSettings();

    disableButtons();
}

void BedsideManagerUI::updateLayoutFromSettings()
{
	BedsideSettings set = getVisibleSettings();

    root->setProperty("isActive_checked", set.isActive);
    root->setProperty("mode_index", set.mode);
    root->setProperty("from_value", set.from);
    root->setProperty("to_value", set.to);
}

BedsideSettings BedsideManagerUI::getVisibleSettings()
{
	QVariant var;
	QSettings settings(m_author, m_appName);

    if(settings.value(m_daily).toBool())
    	var = settings.value(m_daily_settings);
    else {
    	int index = settings.value(m_selected_day).toInt();
    	switch(index) {
    	   case 0: var = settings.value(m_monday_settings); break;
    	   case 1: var = settings.value(m_tuesday_settings); break;
    	   case 2: var = settings.value(m_wednesday_settings); break;
    	   case 3: var = settings.value(m_thursday_settings); break;
    	   case 4: var = settings.value(m_friday_settings); break;
    	   case 5: var = settings.value(m_saturday_settings); break;
    	   case 6: var = settings.value(m_sunday_settings); break;
    	}
    }
    return var.value<BedsideSettings>();
}

void BedsideManagerUI::init()
{
	QSettings settings(m_author, m_appName);

	if (!settings.contains(m_daily_settings))
		settings.setValue(m_daily_settings, QVariant::fromValue(getDefaultValues()));
	if (!settings.contains(m_monday_settings))
		settings.setValue(m_monday_settings, QVariant::fromValue(getDefaultValues()));
	if (!settings.contains(m_tuesday_settings))
		settings.setValue(m_tuesday_settings, QVariant::fromValue(getDefaultValues()));
	if (!settings.contains(m_wednesday_settings))
		settings.setValue(m_wednesday_settings, QVariant::fromValue(getDefaultValues()));
	if (!settings.contains(m_thursday_settings))
		settings.setValue(m_thursday_settings, QVariant::fromValue(getDefaultValues()));
	if (!settings.contains(m_friday_settings))
		settings.setValue(m_friday_settings, QVariant::fromValue(getDefaultValues()));
	if (!settings.contains(m_saturday_settings))
		settings.setValue(m_saturday_settings, QVariant::fromValue(getDefaultValues()));
	if (!settings.contains(m_sunday_settings))
		settings.setValue(m_sunday_settings, QVariant::fromValue(getDefaultValues()));
}

BedsideSettings BedsideManagerUI::getDefaultValues()
{
	BedsideSettings set;
    set.isActive = false;
	set.mode = 0;
	set.from = QDateTime(QDate::currentDate(), QTime(23, 0, 0));
	set.to = QDateTime(QDate::currentDate(), QTime(06, 0, 0));
	return set;
}
