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

import bb.cascades 1.2

/**
 * Custom component representing a rectangle
 * with properties for chaning it's color and title.
 */
Container {
    id: root

    property string title
    property variant color: Color.Black

    preferredWidth: 300
    preferredHeight: 302

    layout: DockLayout {}

    Container {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Top
        preferredHeight: 300

        background: root.color
        opacity: 0.6
    }

    Label {
        horizontalAlignment: HorizontalAlignment.Center
        verticalAlignment: VerticalAlignment.Center

        text: root.title
    }

    ImageView {
        horizontalAlignment: HorizontalAlignment.Fill
        verticalAlignment: VerticalAlignment.Fill

        imageSource: "asset:///images/rect_overlay.png"
    }
}
