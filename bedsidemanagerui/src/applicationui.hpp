#ifndef ApplicationUI_HPP_
#define ApplicationUI_HPP_

#include <QObject>
#include <QFileSystemWatcher>
#include <QDateTime>
#include <QMetaType>
#include <bb/cascades/AbstractPane>
#include <QtCore/QDataStream>
#include <QtCore/QSettings>

struct BedsideSettings{
	bool isActive;
	int mode;
	QDateTime from;
	QDateTime to;
};

Q_DECLARE_METATYPE(BedsideSettings)

namespace bb
{
    namespace cascades
    {
        class Application;
        class LocaleHandler;
        class AbstractPane;
    }
}

class QTranslator;

class BedsideManagerUI: public QObject
{
    Q_OBJECT

public:
    BedsideManagerUI(bb::cascades::Application *app);
    virtual ~BedsideManagerUI() {}

public Q_SLOTS:
    /**
     * Method that can be invoked from within qml
     * to read QSettings key/value for ServiceStatus,
     * in order to decipher if service is running (active).
     */
    bool isServiceRunning();

    /**
     * Method hooked into the signal/slots mechanism, which
     * gets invoked upon receiving fileChanged() signal
     * from the settingsWatcher instance.
     */
    void settingsChanged(const QString & path);

    bool getDaily();
    void setDaily(bool);

    int getNotificationMode();
    void setNotificationMode(int);

    int getSelectedDay();
    void setSelectedDay(int);

    QDateTime getTimeFrom();
    void setTimeFrom(QDateTime);

    QDateTime getTimeTo();
    void setTimeTo(QDateTime);

    bool getMonitoringStatus();
    void setMonitoringStatus(bool);

    bool getServiceStatus();
    void setServiceStatus(bool);

    void save();
    void cancel();

private:

    void init();
    BedsideSettings getDefaultValues();
    BedsideSettings getVisibleSettings();
    void updateLayoutFromSettings();
    void enableButtons();
    void disableButtons();

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

    bb::cascades::AbstractPane *root;
    QTranslator* m_pTranslator;
    bb::cascades::LocaleHandler* m_pLocaleHandler;

    // Watcher for qsettigns file changes
    QFileSystemWatcher* settingsWatcher;

    BedsideSettings unsaved_settings;
};

#endif /* ApplicationUI_HPP_ */
