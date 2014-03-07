/*
 * Copyright (c) 2011-2013 BlackBerry Limited.
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

import bb.cascades 1.2

Page {
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
                checked: true
                onCheckedChanged: {
                    if(checked)
                        selectDay.visible = false;
                    else 
                        selectDay.visible = true;
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
                }
            }
        }        
    }
}
