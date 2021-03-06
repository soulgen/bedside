import bb.cascades 1.2

Page {
    property alias daily_checked: daily.checked
    property alias selectDay_index: selectDayDropDown.selectedIndex
    property alias save_enabled: save.enabled
    property alias cancel_enabled: cancel.enabled
    property alias isActive_checked: isActive.checked
    property alias monitoring_checked: monitoringStatus.checked
    property alias mode_index: mode.selectedIndex
    property alias from_value: from.value
    property alias from_min: from.minimum
    property alias to_value: to.value

    Container {
        id: main
        visible: false
        topPadding: 20.0
        background: back.imagePaint
        attachedObjects: [
            ImagePaintDefinition {
                id: back
                repeatPattern: RepeatPattern.XY
                imageSource: "asset:///images/bg.jpg"
            }
        ]
        verticalAlignment: VerticalAlignment.Fill
        horizontalAlignment: HorizontalAlignment.Fill
        Container {
            layout: DockLayout {
            }
            preferredWidth: 3000
            leftPadding: 10.0
            rightPadding: 10.0
            Label {
                text: "Bedside Manager"
                verticalAlignment: VerticalAlignment.Center
                textStyle.fontSize: FontSize.Medium
            }
            ToggleButton {
                id: monitoringStatus
                verticalAlignment: VerticalAlignment.Center
                horizontalAlignment: HorizontalAlignment.Right
                checked: _app.getMonitoringStatus()
                onCheckedChanged: {
                    globalSettings.enabled = checked;
                    _app.setMonitoringStatus(checked)
                }
            }
        }
        Divider {
            preferredWidth: 3000
        }
        ScrollView {
            Container {
                id: globalSettings
                enabled: monitoringStatus.checked
                Container {
                    layout: DockLayout {
                    }
                    preferredWidth: 3000
                    leftPadding: 10.0
                    rightPadding: 10.0
                    topPadding: 5.0
                    Label {
                        text: "Daily"
                        verticalAlignment: VerticalAlignment.Center
                        textStyle.fontSize: FontSize.Medium
                    }
                    CheckBox {
                        id: daily
                        verticalAlignment: VerticalAlignment.Center
                        horizontalAlignment: HorizontalAlignment.Right
                        checked: _app.getDaily()
                        onCheckedChanged: {
                            selectDay.visible = !checked
                            bedsideMode.visible = !checked
                            bedsideDivider.visible = checked
                            _app.setDaily(checked)
                        }
                    }
                }
                Container {
                    id: selectDay
                    leftPadding: 10.0
                    rightPadding: 10.0
                    topPadding: 30.0
                    visible: !daily.checked
                    DropDown {
                        id: selectDayDropDown
                        selectedIndex: _app.getSelectedDay()
                        title: "Select Day:"
                        Option { text: "Monday" }
                        Option { text: "Tuesday" }
                        Option { text: "Wednesday" }
                        Option { text: "Thursday" }
                        Option { text: "Friday" }
                        Option { text: "Saturday" }
                        Option { text: "Sunday" }
                        onSelectedIndexChanged: {
                            _app.setSelectedDay(selectedIndex)
                        }
                    }
                }
                Divider {
                    id: bedsideDivider
                    visible: daily.checked
                    preferredWidth: 3000
                }
                Container {
                    id: bedsideMode
                    Container {
                        layout: DockLayout {
                        }
                        preferredWidth: 3000
                        leftPadding: 10.0
                        rightPadding: 10.0
                        topPadding: 20.0
                        Label {
                            text: "Bedside Mode"
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontSize: FontSize.Medium
                        }
                        ToggleButton {
                            id: isActive
                            verticalAlignment: VerticalAlignment.Center
                            horizontalAlignment: HorizontalAlignment.Right
                            checked: _app.getServiceStatus()
                            onCheckedChanged: {
                                settings.enabled = checked;
                                _app.setServiceStatus(checked)
                            }
                        }
                    }
                    Divider {
                        preferredWidth: 3000
                    }
                }

                Container {
                    id: settings
                    enabled: isActive.checked
                    Container {
                        layout: DockLayout {
                        }
                        preferredWidth: 3000
                        leftPadding: 10.0
                        rightPadding: 10.0
                        Label {
                            text: "Notifications"
                            verticalAlignment: VerticalAlignment.Center
                            textStyle.fontSize: FontSize.Medium
                        }
                        DropDown {
                            id: mode
                            preferredWidth: 380
                            horizontalAlignment: HorizontalAlignment.Right
                            verticalAlignment: VerticalAlignment.Center
                            title: "Mode:"
                            selectedIndex: _app.getNotificationMode()
                            onSelectedIndexChanged: {
                                if(selectedIndex == 0)
                                    modeDescription.text = "The normal mode of operations for notifications. Custom settings are saved when switching out of this mode, and are restored when switching into this mode"
                                if(selectedIndex == 1)
                                    modeDescription.text = "Disable sound and vibrate"
                                if(selectedIndex == 2)
                                    modeDescription.text = "Play all sound and vibration notifications as vibrations"
                                if(selectedIndex == 3)
                                    modeDescription.text = "Deliver only phone related notifications" 
                                if(selectedIndex == 4)
                                    modeDescription.text = "Disable all notifications"
                                _app.setNotificationMode(selectedIndex)
                            }
                            Option { text: "Normal"; }
                            Option { text: "Silent"; }
                            Option { text: "Vibrate"; }
                            Option { text: "PhoneOnly"; }
                            Option { text: "AlertsOff"; }
                        }
                    }
                    Container {
                        leftPadding: 10.0
                        rightPadding: 10.0
                        topPadding: 10.0
                        Label {
                            id: modeDescription
                            text: "Disable sound and vibrate"
                            textStyle.fontStyle: FontStyle.Italic
                            textStyle.fontSize: FontSize.XSmall
                            multiline: true
                        }
                    }
                    Divider {
                        preferredWidth: 3000
                    }
                    Container {
                        preferredWidth: 3000
                        leftPadding: 10.0
                        rightPadding: 10.0
                        layout: StackLayout {
                            orientation: LayoutOrientation.LeftToRight
                        }
                        Container {
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 0.5
                            }
                            rightMargin: 40.0
                            DateTimePicker {
                                id: from
                                verticalAlignment: VerticalAlignment.Center
                                mode: DateTimePickerMode.Time
                                minuteInterval: 1
                                title: "Begin:"
                                value: _app.getTimeFrom()
                                onValueChanged: {
                                    _app.setTimeFrom(value)
                                }
                            }
                        }
                        Container {
                            layoutProperties: StackLayoutProperties {
                                spaceQuota: 0.5
                            }
                            leftMargin: 40.0
                            DateTimePicker {
                                id: to
                                verticalAlignment: VerticalAlignment.Center
                                mode: DateTimePickerMode.Time
                                minuteInterval: 1
                                title: "End:"
                                value: _app.getTimeTo()
                                onValueChanged: {
                                    _app.setTimeTo(value)
                                }
                            }
                        }
                    }
                    Divider {
                        preferredWidth: 3000
                    }
                    }
                    Container {
                        layout: DockLayout {
                        }
                        preferredWidth: 3000
                        leftPadding: 10.0
                        rightPadding: 10.0
                        Button {
                            id: cancel
                            text: "Cancel"
                            horizontalAlignment: HorizontalAlignment.Left
                            enabled: false
                            onClicked: {
                                _app.cancel()
                            }
                        }
                        Button {
                            id: save
                            text: "Save"
                            horizontalAlignment: HorizontalAlignment.Right
                            enabled: false
                            onClicked: {
                                _app.save()
                            }
                        }
                    }
                }
            }
        }
    onCreationCompleted: {
        main.visible = true
    }
}

