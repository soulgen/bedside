import bb.cascades 1.2

Page {
    ScrollView {
        Container {
            topPadding: 30.0
            Container {
                layout: DockLayout {
                }
                preferredWidth: 3000
                leftPadding: 10.0
                rightPadding: 10.0
                Label {
                    text: "Daily"
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.fontSize: FontSize.Medium
                }
                CheckBox {
                    verticalAlignment: VerticalAlignment.Center
                    horizontalAlignment: HorizontalAlignment.Right
                    checked: _app.getDaily()
                    onCheckedChanged: {
                        if(checked)
                            selectDay.visible = false;
                        else
                            selectDay.visible = true;
                        _app.setDaily(checked)
                    }
                }
            }
            Container {
                id: selectDay
                leftPadding: 10.0
                rightPadding: 10.0
                topPadding: 20.0
                visible: false
                DropDown {
                    title: "Select Day:"
                    Option { text: "Monday" }
                    Option { text: "Tuesday" }
                    Option { text: "Wednesday" }
                    Option { text: "Thursday" }
                    Option { text: "Friday" }
                    Option { text: "Saturday" }
                    Option { text: "Sunday" }
                }
            }
            Divider {
                preferredWidth: 3000
            }
            Container {
                layout: DockLayout {
                }
                preferredWidth: 3000
                leftPadding: 10.0
                rightPadding: 10.0
                Label {
                    text: "Wireless connections"
                    verticalAlignment: VerticalAlignment.Center
                    textStyle.fontSize: FontSize.Medium
                }
                ToggleButton {
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Center
                    checked: _app.getWConnections()
                    onCheckedChanged: {
                        _app.setWConnections(checked)
                    }
                }
            }
            Container {
                leftPadding: 10.0
                rightPadding: 10.0
                Label {
                    text: "Wi-Fi, Bluetooth, NFC"
                    textStyle.fontStyle: FontStyle.Italic
                    textStyle.fontSize: FontSize.XSmall
                    multiline: true
                }
            }
            Divider {
                preferredWidth: 3000
            }
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
                    preferredWidth: 380
                    horizontalAlignment: HorizontalAlignment.Right
                    verticalAlignment: VerticalAlignment.Center
                    title: "Mode:"
                    selectedIndex: 1
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
                    }
                    Option { text: "Normal " }
                    Option { text: "Silent" }
                    Option { text: "Vibrate" }
                    Option { text: "PhoneOnly" }
                    Option { text: "AlertsOff" }
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
                    rightMargin: 100.0
                    Label {
                        text: "From:"
                        verticalAlignment: VerticalAlignment.Center
                        textStyle.fontSize: FontSize.Medium
                    }
                    DateTimePicker {
                        verticalAlignment: VerticalAlignment.Center
                        mode: DateTimePickerMode.Time
                        value: _app.getTimeFrom()
                        onValueChanged: {
                            _app.setTimeFrom()
                        }
                    }
                }
                Container {
                    layoutProperties: StackLayoutProperties {
                        spaceQuota: 0.5
                    }
                    leftMargin: 100.0
                    Label {
                        text: "To:"
                        verticalAlignment: VerticalAlignment.Center
                        textStyle.fontSize: FontSize.Medium
                    }
                    DateTimePicker {
                        verticalAlignment: VerticalAlignment.Center
                        mode: DateTimePickerMode.Time
                        value: _app.getTimeTo()
                        onValueChanged: {
                            _app.setTimeTo()
                        }
                    }
                }
            }
            Divider {
                preferredWidth: 3000
            }
            Container {
                layout: DockLayout {
                }
                preferredWidth: 3000
                leftPadding: 10.0
                rightPadding: 10.0
                Button {
                    text: "Cancel"
                    horizontalAlignment: HorizontalAlignment.Left
                    //enabled: false
                    onClicked: {
                        _app.cancel()  
                    }
                }
                Button {
                    text: "Save"
                    horizontalAlignment: HorizontalAlignment.Right
                    //enabled: false
                    onClicked: {
                      _app.save()  
                    }
                }
            }
        }
    }
}
