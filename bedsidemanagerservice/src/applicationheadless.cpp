/* Copyright (c) 2013 BlackBerry Limited.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include "applicationheadless.hpp"

#include <bb/Application>

#include <bb/platform/NotificationMode>
#include <bb/platform/NotificationGlobalSettings>

#include <bb/system/InvokeManager>
#include <bb/system/InvokeRequest>
#include <bb/system/InvokeTargetReply>
#include <QDebug>

#define HOUR 10000
#define H24_MSEC 86400000

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

const QString BedsideManagerService::m_author =  "Teleca"; // for creating settings
const QString BedsideManagerService::m_appName = "BedsideManagerApp"; // for creating settings

// keys for setting file
const QString BedsideManagerService::m_daily = "Daily";
const QString BedsideManagerService::m_daily_settings = "DailySettings";
const QString BedsideManagerService::m_monday_settings = "MondaySettings";
const QString BedsideManagerService::m_tuesday_settings = "TuesdaySettings";
const QString BedsideManagerService::m_wednesday_settings = "WednesdaySettings";
const QString BedsideManagerService::m_thursday_settings = "ThursdaySettings";
const QString BedsideManagerService::m_friday_settings = "FridaySettings";
const QString BedsideManagerService::m_saturday_settings = "SaturdaySettings";
const QString BedsideManagerService::m_sunday_settings = "SundaySettings";
const QString BedsideManagerService::m_monitoring_status = "MonitoringStatus";
const QString BedsideManagerService::m_selected_day = "SelectedDay";
const QString BedsideManagerService::m_serviceStatus = "ServiceStatus";

using namespace bb::system;
using namespace bb::platform;

BedsideManagerService::BedsideManagerService(bb::Application *app) :
		QObject(app), m_invokeManager(new InvokeManager(this)), m_settingsWatcher(new QFileSystemWatcher(this)), m_isBedsideModeActive(
				false) {
	QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);

	qRegisterMetaTypeStreamOperators<BedsideSettings>("BedsideSettings");

	// log the service PID
	qDebug() << "PID = " << QString::number(QCoreApplication::applicationPid());
}

void BedsideManagerService::onInvoked(const bb::system::InvokeRequest& request) {
	qDebug() << "#service got invoked: " << request.action();

//	if (request.action().compare("bb.action.system.STARTED") == 0) {
//		//m_led->flash(m_flashCount);
//	} else {
//		// write service running status to qsettings
//		QSettings settings(m_author, m_appName);
//		settings.setValue(m_serviceStatus, request.action());
//	}
}

void BedsideManagerService::settingsChanged(const QString & path) {
	Q_UNUSED(path);
	QSettings settings(m_author, m_appName);
	BedsideSettings set = getActualSettings();
    NotificationGlobalSettings globalsettings;

	if (!settings.value(m_monitoring_status).toBool()) {
		qDebug() << "Stopping Service";
		timer->stop();
		hourly_timer->stop();
		m_isBedsideModeActive = false;
	}
	else {
		if(m_isBedsideModeActive &&
		   (set.to != restore_settings.to || set.from != restore_settings.from
			|| set.mode != globalsettings.mode() ))
		{
			m_isBedsideModeActive = false;
		}

	    updateMonitoring();

	    if(!hourly_timer->isActive()) {
	    	hourly_timer->start(HOUR);
	    	qDebug() << "Running Service";
	    }
	}
}

void BedsideManagerService::updateMonitoring()
{
	BedsideSettings set = getActualSettings();

	qDebug() << "updateMonitoring()" << m_isBedsideModeActive;

	if(!m_isBedsideModeActive) {
		int from_interval = getMsec(set.from) - getMsec(QDateTime::currentDateTime());

		qDebug() << "from_interval = " << from_interval << getMsec(set.from) << getMsec(QDateTime::currentDateTime());

		qDebug() << "from = " << set.from.toString();
		qDebug() << "current = " << QDateTime::currentDateTime().toString();


		if(from_interval < 0)
		{
			qDebug() << "setBedsideMode()" << m_isBedsideModeActive;
			setBedsideMode();
		}
		else {
			timer->stop();
    		timer->start(from_interval);
		}
	}
	else {
		if(getMsec(QDateTime::currentDateTime()) > H24_MSEC/2 &&
		   getMsec(QDateTime::currentDateTime()) < H24_MSEC) {
			timer->stop();
			timer->start(H24_MSEC - getMsec(QDateTime::currentDateTime()) + getMsec(set.to));
		}
		else if((getMsec(QDateTime::currentDateTime()) - getMsec(set.to)) > 0){
			setBedsideMode();
		}
		else {
			timer->stop();
			timer->start(getMsec(set.to) - getMsec(QDateTime::currentDateTime()));
		}
	}
}

void BedsideManagerService::setBedsideMode()
{
	if(!m_isBedsideModeActive) {
		BedsideSettings set = getActualSettings();
		int to_interval = H24_MSEC - getMsec(QDateTime::currentDateTime()) + getMsec(set.to);
		qDebug() << "in Bedside to = " << to_interval;
		timer->stop();
		timer->start(to_interval);
		saveCurrentPhoneSettings();
		m_isBedsideModeActive = true;
		setPhoneSettings();
	}
	else {
		m_isBedsideModeActive = false;
		setPhoneSettings();
	}
}

BedsideSettings BedsideManagerService::getActualSettings()
{
	QVariant var;
	QSettings settings(m_author, m_appName);

    if(settings.value(m_daily).toBool())
    	var = settings.value(m_daily_settings);
    else {
    	int index = QDate::currentDate().dayOfWeek();
    	if(QTime::currentTime().hour() > 0 && QTime::currentTime().hour() < 12)
    		--index;
    	switch(index) {
    	   case 1: var = settings.value(m_monday_settings); break;
    	   case 2: var = settings.value(m_tuesday_settings); break;
    	   case 3: var = settings.value(m_wednesday_settings); break;
    	   case 4: var = settings.value(m_thursday_settings); break;
    	   case 5: var = settings.value(m_friday_settings); break;
    	   case 6: var = settings.value(m_saturday_settings); break;
    	   case 7: var = settings.value(m_sunday_settings); break;
    	}
    }
    return var.value<BedsideSettings>();
}

int BedsideManagerService::getMsec(QDateTime data)
{
    QTime time = data.time();
    return time.msec() + 1000*time.second() + 1000*60*time.minute() + 1000*60*60*time.hour();
}

void BedsideManagerService::init() {
	// set the initial qsettings keys/values upon startup
	QSettings settings(m_author, m_appName);
	timer = new QTimer(this);
	hourly_timer = new QTimer(this);

	// Force the creation of the settings file so that we can watch it for changes.
	settings.sync();

	// Watcher for changes in the settings file.
	m_settingsWatcher->addPath(settings.fileName());

	// Do all the necessary signal/slot connections to be invokable to receive led updates.
	bool ok = connect(m_invokeManager,
			SIGNAL(invoked(const bb::system::InvokeRequest&)), this,
			SLOT(onInvoked(const bb::system::InvokeRequest&)));
	Q_ASSERT(ok);

	ok = connect(hourly_timer, SIGNAL(timeout()), this, SLOT(updateMonitoring()));
	Q_ASSERT(ok);
	hourly_timer->start(HOUR);

	ok = connect(timer, SIGNAL(timeout()), this, SLOT(setBedsideMode()));
	Q_ASSERT(ok);
	settings.setValue(m_serviceStatus, QVariant::fromValue(true));

	ok = connect(m_settingsWatcher, SIGNAL(fileChanged(const QString&)), this,
			SLOT(settingsChanged(const QString&)));
	Q_ASSERT(ok);
	Q_UNUSED(ok);
}


void BedsideManagerService::setPhoneSettings() {

	NotificationGlobalSettings globalsettings;
	BedsideSettings set;

	if(m_isBedsideModeActive)
		set = getActualSettings();
	else
		set = restore_settings;

	qDebug() << "Service: setPhoneNotificationMode(" << set.mode << ")";

	switch (set.mode) {
	case 1:
		globalsettings.setMode(NotificationMode::Silent);
		break;
	case 2:
		globalsettings.setMode(NotificationMode::Vibrate);
		break;
	case 3:
		globalsettings.setMode(NotificationMode::Normal);
		break;
	case 4:
		globalsettings.setMode(NotificationMode::PhoneOnly);
		break;
	case 5:
		globalsettings.setMode(NotificationMode::AlertsOff);
		break;
	default:
		globalsettings.setMode(NotificationMode::Normal);
		break;
	}
}

void BedsideManagerService::saveCurrentPhoneSettings() {
	NotificationGlobalSettings globalsettings;
	QSettings settings(m_author, m_appName);
	QVariant var = settings.value(m_daily_settings);
	BedsideSettings current_settings = var.value<BedsideSettings>();

	restore_settings = {0, globalsettings.mode(), current_settings.from, current_settings.to };
	qDebug() << "BedsideManagerService::saveCurrentPhoneSettings()" << globalsettings.mode() ;
}
