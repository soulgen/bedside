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

#define HOUR 3600000
#define H24_MSEC 86400000

static int day_index = 0; // 0-Daily, 1-Monday, 2-Tuesday, etc.

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
		QObject(app),
		m_invokeManager(new InvokeManager(this)),
		m_settingsWatcher(new QFileSystemWatcher(this)),
		m_isBedsideModeActive(false)
{
	QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);

	qRegisterMetaTypeStreamOperators<BedsideSettings>("BedsideSettings");

	// log the service PID
	qDebug() << "SERVICE: PID = " << QString::number(QCoreApplication::applicationPid());
}

void BedsideManagerService::onInvoked(const bb::system::InvokeRequest& request) {
	qDebug() << "SERVICE: service got invoked: " << request.action();

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

    qDebug() << "SERVICE: settingsChanged";
	if (!settings.value(m_monitoring_status).toBool()) {
		qDebug() << "SERVICE: Stopping Service";
		m_isBedsideModeActive = false;
		setPhoneSettings();
		timer->stop();
		hourly_timer->stop();
	}
	else {
		if(m_isBedsideModeActive && hourly_timer->isActive())
		{
			int current_time = getMsec(QDateTime::currentDateTime());

			if(current_time > getMsec(set.to) || current_time < getMsec(set.from))
				setBedsideMode();
		}

	    updateMonitoring();

	    if(!hourly_timer->isActive()) {
	    	hourly_timer->start(HOUR);
	    	qDebug() << "SERVICE: Running Service";
	    }
	}
}

void BedsideManagerService::updateMonitoring()
{
	BedsideSettings set = getActualSettings();

	qDebug() << "SERVICE: updateMonitoring()" << m_isBedsideModeActive;
	qDebug() << "SERVICE: day_index = " << day_index;

	if(!m_isBedsideModeActive) {
		int num_of_days = day_index - QDate::currentDate().dayOfWeek();
		if(num_of_days < 0 && day_index == 0)
			num_of_days = 0;
//		else if(num_of_days == -1 && getMsec(QDateTime::currentDateTime()) < H24_MSEC/2) {
//			num_of_days = 0;
//		}
		else if(num_of_days < 0)
			num_of_days = 7 - QDate::currentDate().dayOfWeek() + day_index;

		qDebug() << "SERVICE: num_of_days = " << num_of_days;
//		if(getMsec(QDateTime::currentDateTime()) < H24_MSEC/2 && num_of_days == 0 &&
//		   (getMsec(QDateTime::currentDateTime()) - getMsec(set.to)) < 0 ) {
//			setBedsideMode();
//		}
//		else {

		    int current_time = getMsec(QDateTime::currentDateTime());
		    int from_interval = 0;

    		if(current_time > getMsec(set.from) && current_time < getMsec(set.to))
     	    	setBedsideMode();
		    else if(current_time > getMsec(set.from) &&	current_time > getMsec(set.to) && num_of_days == 0) {
		    	from_interval = H24_MSEC - current_time + getMsec(set.from);
		    	timer->stop();
		    	timer->start(from_interval);
		    }
		    else
		    {
		    	from_interval = H24_MSEC*num_of_days + getMsec(set.from) - current_time;
			    timer->stop();
	   		    timer->start(from_interval);
		    }
		    qDebug() << "SERVICE: from_interval = " << from_interval << getMsec(set.from) << current_time;
		    qDebug() << "SERVICE: from = " << set.from.toString();
		    qDebug() << "SERVICE: current = " << QDateTime::currentDateTime().toString();
//		}
	}
	else {
		if((getMsec(QDateTime::currentDateTime()) - getMsec(set.to)) > 0){
			setBedsideMode();
		}
		else {
            int to_interval = 0;

			if(getMsec(set.to) < getMsec(set.from))
			    to_interval = H24_MSEC - getMsec(QDateTime::currentDateTime()) + getMsec(set.to);
			else
				to_interval = getMsec(set.to) - getMsec(QDateTime::currentDateTime());

			timer->stop();
			timer->start(to_interval);

		    qDebug() << "SERVICE: to_interval = " << to_interval << getMsec(set.to) << getMsec(QDateTime::currentDateTime());
		    qDebug() << "SERVICE: to = " << set.to.toString();
		    qDebug() << "SERVICE: current = " << QDateTime::currentDateTime().toString();
		}
	}
}

void BedsideManagerService::setBedsideMode()
{
	qDebug() << "SERVICE: setBedsideMode()" << m_isBedsideModeActive;
	if(!m_isBedsideModeActive) {
		BedsideSettings set = getActualSettings();
		int to_interval = 0;
		if(getMsec(set.to) < getMsec(set.from))
		    to_interval = H24_MSEC - getMsec(QDateTime::currentDateTime()) + getMsec(set.to);
		else
			to_interval = getMsec(set.to) - getMsec(QDateTime::currentDateTime());
		qDebug() << "SERVICE: in Bedside to = " << to_interval;
		timer->stop();
		timer->start(to_interval);
		saveCurrentPhoneSettings();
		m_isBedsideModeActive = true;
		setPhoneSettings();
	}
	else {
		m_isBedsideModeActive = false;
		setPhoneSettings();
		updateMonitoring();
	}
}

BedsideSettings BedsideManagerService::getActualSettings()
{
	QVariant var;
	QSettings settings(m_author, m_appName);
	NotificationGlobalSettings globalsettings;

	BedsideSettings set = {false, globalsettings.mode(), QDateTime::currentDateTime(), QDateTime::currentDateTime() };

    if(settings.value(m_daily).toBool()) {
    	day_index = 0;
    	var = settings.value(m_daily_settings);
    	return var.value<BedsideSettings>();
    }
    else {
    	int index = day_index;
    	if(!m_isBedsideModeActive)
            index = QDate::currentDate().dayOfWeek();

    	for(int i = 1; i <= 7; ++i){
    	    switch(index) {
    	       case 1: var = settings.value(m_monday_settings); break;
    	       case 2: var = settings.value(m_tuesday_settings); break;
    	       case 3: var = settings.value(m_wednesday_settings); break;
    	       case 4: var = settings.value(m_thursday_settings); break;
    	       case 5: var = settings.value(m_friday_settings); break;
    	       case 6: var = settings.value(m_saturday_settings); break;
    	       case 7: var = settings.value(m_sunday_settings); break;
    	    }
    	    if(var.value<BedsideSettings>().isActive) {
    	    	day_index = index;
    	    	return var.value<BedsideSettings>();
    	    }
    	    else
    		    index++;

    	    if(index > 7)
    	    	index = 1;
        }
    }

    day_index = 0;
    settings.setValue(m_monitoring_status, QVariant::fromValue(false));
	return set;
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

	settings.setValue(m_serviceStatus, "running");
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

	qDebug() << "SERVICE: setPhoneNotificationMode"<< set.mode;

	switch (set.mode) {
	case 0:
		globalsettings.setMode(NotificationMode::Normal);
		break;
	case 1:
		globalsettings.setMode(NotificationMode::Silent);
		break;
	case 2:
		globalsettings.setMode(NotificationMode::Vibrate);
		break;
	case 3:
		globalsettings.setMode(NotificationMode::PhoneOnly);
		break;
	case 4:
		globalsettings.setMode(NotificationMode::AlertsOff);
		break;
	default:
		globalsettings.setMode(NotificationMode::Normal);
		break;
	}
}

void BedsideManagerService::saveCurrentPhoneSettings() {
	NotificationGlobalSettings globalsettings;
	BedsideSettings actual_settings = getActualSettings();

	restore_settings.isActive = false;
	restore_settings.from = actual_settings.from;
	restore_settings.to = actual_settings.to;

	switch (globalsettings.mode()) {
        case NotificationMode::Normal: restore_settings.mode = 0; break;
	    case NotificationMode::Silent: restore_settings.mode = 1; break;
	    case NotificationMode::Vibrate: restore_settings.mode = 2; break;
	    case NotificationMode::PhoneOnly: restore_settings.mode = 3; break;
	    case NotificationMode::AlertsOff: restore_settings.mode = 4; break;
	    default: restore_settings.mode = 0; break;
	}
	qDebug() << "SERVICE: saveCurrentPhoneSettings()" << globalsettings.mode() ;
}
