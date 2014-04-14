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

#ifndef ApplicationHeadless_HPP_
#define ApplicationHeadless_HPP_

#include <QObject>
#include <QFileSystemWatcher>
#include <QMetaType>
#include <QDateTime>
#include <QTimer>

struct BedsideSettings{
	bool isActive;
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
     * gets invoked upon receiving fileChanged() signal
     * from the settingsWatcher instance.
     */
    void settingsChanged(const QString & path);

    /**
     * Method invoked when a activeChanged() signal is
     * received from the Led instance, indicating
     * a led state change.
     */
    void init();

    void updateMonitoring();
    int getMsec(QDateTime);
    void saveCurrentPhoneSettings();
    void setPhoneSettings();
    void setBedsideMode();

    BedsideSettings getActualSettings();

private:
    // Invocation Manager instance for receiving system events
    bb::system::InvokeManager *m_invokeManager;
    QTimer *timer;
    QTimer *hourly_timer;

    static const QString m_author; // for creating settings
    static const QString m_appName; // for creating settings

    static const QString m_daily; // for daily check box
    static const QString m_daily_settings; // for daily settings
    static const QString m_monday_settings; // for Monday settings
    static const QString m_tuesday_settings; // for Tuesday settings
    static const QString m_wednesday_settings; // for Wednesday settings
    static const QString m_thursday_settings; // for Thursday settings
    static const QString m_friday_settings; // for Friday settings
    static const QString m_saturday_settings; // for Saturday settings
    static const QString m_sunday_settings; // for Sunday settings
    static const QString m_monitoring_status; // for checking monitoring status
    static const QString m_selected_day; // for daily settings
    static const QString m_serviceStatus; // for checking service running

    // Watcher for qsettigns file changes
    QFileSystemWatcher* m_settingsWatcher;
    BedsideSettings restore_settings;

    bool m_isBedsideModeActive;
};

#endif /* ApplicationHeadless_HPP_ */
