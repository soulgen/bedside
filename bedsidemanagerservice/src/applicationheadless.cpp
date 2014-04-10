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

const QString BedsideManagerService::m_author =  "Teleca"; // for creating settings
const QString BedsideManagerService::m_appName = "BedsideManagerApp"; // for creating settings

// keys for setting file
const QString BedsideManagerService::m_serviceStatus = "ServiceStatus";
const QString BedsideManagerService::m_daily = "Daily";
const QString BedsideManagerService::m_daily_settings = "DailySettings";

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
	BedsideSettings set = LoadFromQSettings(true);
	NotificationGlobalSettings globalsettings;

	if (!settings.value(m_serviceStatus).toBool()) {
		qDebug() << "Stopping Service";
		timer->stop();
		m_isBedsideModeActive = false;
	}
	else {
		qDebug() << "Running Service";
		if(!timer->isActive())
			timer->start(500000);
	}

	if(m_isBedsideModeActive &&
	   (set.to != restore_settings.to || set.from != restore_settings.from
		|| set.mode != globalsettings.mode() ))
	{
		m_isBedsideModeActive = false;
		checkcurtime();
	}
}

void BedsideManagerService::init() {
	// set the initial qsettings keys/values upon startup
	QSettings settings(m_author, m_appName);
	timer = new QTimer(this);

	settings.setValue(m_serviceStatus, QVariant::fromValue(true));
	// Force the creation of the settings file so that we can watch it for changes.
	settings.sync();

	// Watcher for changes in the settings file.
	m_settingsWatcher->addPath(settings.fileName());

	// Do all the necessary signal/slot connections to be invokable to receive led updates.
	bool ok = connect(m_invokeManager,
			SIGNAL(invoked(const bb::system::InvokeRequest&)), this,
			SLOT(onInvoked(const bb::system::InvokeRequest&)));
	Q_ASSERT(ok);

	ok = connect(timer, SIGNAL(timeout()), this, SLOT(checkcurtime()));
	Q_ASSERT(ok);
	if(!timer->isActive()){
		settings.setValue(m_serviceStatus, QVariant::fromValue(true));
		timer->start(500000);
	}

	ok = connect(m_settingsWatcher, SIGNAL(fileChanged(const QString&)), this,
			SLOT(settingsChanged(const QString&)));
	Q_ASSERT(ok);
	Q_UNUSED(ok);
}

void BedsideManagerService::checkcurtime() {
	qDebug() << "BedsideManagerService::checkcurtime()";
	QDateTime curDateTime;
	BedsideSettings set = LoadFromQSettings(true);

	//set flag isBedSideMode here and save/load current/previous settings

	qDebug() << "BedsideManagerService::checkcurtime() m_isBedsideModeActive ="
			<< m_isBedsideModeActive;

//	qDebug() << "BedsideManagerService::checkcurtime() curTime ="
//			<< curDateTime.currentDateTime();
//
//	qDebug() << "BedsideManagerService::checkcurtime() BedsideTime From ="
//			<< set.from;
//
//	qDebug() << "BedsideManagerService::checkcurtime() BedsideTime To ="
//			<< set.to;

	if (!m_isBedsideModeActive) {
		if (((set.from.toMSecsSinceEpoch()) >= 0)
			&& (curDateTime.currentDateTime() >= set.from)
			&& (curDateTime.currentDateTime() < set.to)) {
			saveCurrentPhoneSettings();
			m_isBedsideModeActive = true;
			setPhoneSettings();
		}
		else
			setPhoneSettings();
	} else {
		if (((set.to.toMSecsSinceEpoch()) >= 0)
			&& ((curDateTime.currentDateTime() >= set.to) ||
				(curDateTime.currentDateTime() < set.from))) {
			m_isBedsideModeActive = false;
			setPhoneSettings();
		}
	}
}

void BedsideManagerService::setPhoneSettings() {

	NotificationGlobalSettings globalsettings;
	BedsideSettings set = LoadFromQSettings(m_isBedsideModeActive);

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

BedsideSettings BedsideManagerService::LoadFromQSettings(bool isBedsideMode) {
	if (isBedsideMode) {
		BedsideSettings set = { 0, 0, QDateTime(), QDateTime() };
		QSettings settings(m_author, m_appName);
		if (settings.contains(m_daily_settings)) {
			QVariant var = settings.value(m_daily_settings);
			if (var.canConvert<BedsideSettings>()) {
				set = var.value<BedsideSettings>();
				qDebug() <<"Service: LoadFromQSettings(" << set.mode << ")";
			} else {
				qDebug()<<"Service: LoadFromQSettings: ERROR: can't load bedside qsettings";
			}
		}
		return set;
	} else
        return restore_settings;
}

void BedsideManagerService::saveCurrentPhoneSettings() {
	qDebug() << "BedsideManagerService::saveCurrentPhoneSettings()";
	NotificationGlobalSettings globalsettings;
	QSettings settings(m_author, m_appName);
	QVariant var = settings.value(m_daily_settings);
	BedsideSettings current_settings = var.value<BedsideSettings>();

	restore_settings = {0, globalsettings.mode(), current_settings.from, current_settings.to };

//	qDebug() << "saveCurrentPhoneSettings: w_connections = "
//			<< current_settings.w_connections;
	qDebug() << "saveCurrentPhoneSettings: mode = " << restore_settings.mode;
//	qDebug() << "saveCurrentPhoneSettings: From = "
//			<< current_settings.from.toString();
//	qDebug() << "saveCurrentPhoneSettings: To = "
//			<< current_settings.to.toString();
}
