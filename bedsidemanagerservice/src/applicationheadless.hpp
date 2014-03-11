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
#include <QTime>
#include <QFileSystemWatcher>
#include <wifi/wifi_service.h>
#include <bb/platform/NotificationMode>

struct BedsideSettings{
	bool w_connections;
	bb::platform::NotificationMode mode;
	QTime from;
	QTime to;
};

namespace bb
{
    class Application;
    namespace system
    {
        class InvokeManager;
        class InvokeRequest;
    }
    namespace device
    {
        class Led;
    }
    namespace platform
    {
      class NotificationGlobalSettings;
    }
}

class ApplicationHeadless: public QObject
{
    Q_OBJECT
public:
    ApplicationHeadless(bb::Application *app);
    virtual ~ApplicationHeadless()
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
    void flashCountChanged(int x);

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
    void activeUpdate(bool active);

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
    static const QString m_author; // for creating settings
    static const QString m_appName; // for creating settings

    // keys used in setting
    static const QString m_flashNumber;
    static const QString m_remainingCount;
    static const QString m_serviceStatus;
    static const QString m_ledActive;
    static const QString m_reset;
    static const bool m_daily;
    static const BedsideSettings m_bssettings;
    static const BedsideSettings m_cursettings;

    // The Led instance
    bb::device::Led *m_led;
    // flash count holder
    int m_flashCount;
    // Watcher for qsettigns file changes
    QFileSystemWatcher* m_settingsWatcher;
};

#endif /* ApplicationHeadless_HPP_ */
