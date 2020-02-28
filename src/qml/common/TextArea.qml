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
import QtQuick.Controls 2.2 as Controls
import ktouch 1.0

Controls.TextArea {
    id: control

    property KColorScheme colorScheme: KColorScheme {
        colorGroup: control.enabled? KColorScheme.Active: KColorScheme.Disabled
        colorSet:KColorScheme.View
    }

    property KColorScheme selectionColorScheme: KColorScheme {
        colorGroup: control.enabled? KColorScheme.Active: KColorScheme.Disabled
        colorSet:KColorScheme.Selection
    }

    selectByMouse: true
    selectByKeyboard: true
    color: control.colorScheme.normalText
    selectionColor: selectionColorScheme.normalBackground
    selectedTextColor: selectionColorScheme.normalText

    background: Rectangle {
        color: control.colorScheme.normalBackground
        anchors.fill: parent
        border.width: 1
        border.color: control.activeFocus?
                          control.colorScheme.focusDecoration:
                          Qt.hsva(control.colorScheme.focusDecoration.hslHue,
                                  0,
                                  control.colorScheme.focusDecoration.hslValue,
                                  control.enabled? 0.5: 0.2)
        Behavior on border.color {
            ColorAnimation { duration: Units.shortDuration }
        }
    }


}
