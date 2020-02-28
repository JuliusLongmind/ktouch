/*
 *  Copyright 2017  Sebastian Gottfried <sebastian.gottfried@posteo.de>
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU General Public License as
 *  published by the Free Software Foundation; either version 2 of
 *  the License, or (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import QtQuick.Controls 2.2
import ktouch 1.0
import QtGraphicalEffects 1.0

Button {
    id: button

    property alias color: content.color
    property alias bgColor: bg.color
    property alias iconName: content.iconName
    property alias colorScheme: buttonColorScheme


    padding: 0

    hoverEnabled: true

    KColorScheme {
        id: buttonColorScheme
        colorGroup:  button.enabled? KColorScheme.Active: KColorScheme.Disabled
        colorSet: KColorScheme.Button
    }

    contentItem: IconLabel {
        color: buttonColorScheme.normalText
        id: content
        text: button.text
        elide: "ElideNone"

        Behavior on color {
            ColorAnimation { duration: Units.shortDuration }
        }

    }

    background: Item {
        Rectangle {
            anchors.fill: parent;
            id: bg
            color: buttonColorScheme.alternateBackground

            HueSaturation {
                anchors.fill: bg
                source: bg
                saturation: hovered? 0.3: 0
                lightness: hovered? -0.04: 0
                Behavior on saturation {
                    NumberAnimation {
                        duration: Units.shortDuration
                    }
                }
                Behavior on lightness {
                    NumberAnimation {
                        duration: Units.shortDuration
                    }
                }
            }
        }

        FocusBar {
            anchors {
                left: parent.left
                right: parent.right
                bottom: parent.bottom
            }
            control: button
        }
    }
}
