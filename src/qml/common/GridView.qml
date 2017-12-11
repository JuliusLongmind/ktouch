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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.9
import QtQuick.Controls 2.2
import ktouch 1.0

GridView {
    id: control

    property alias colorScheme: colorScheme
    property alias background: backgroundItem

    KColorScheme {
        id: colorScheme
        colorGroup: control.enabled? KColorScheme.Active: KColorScheme.Disabled
        colorSet: KColorScheme.View
    }

    Rectangle {
        id: backgroundItem
        anchors.fill: parent
        color: colorScheme.normalBackground
        z: -1
    }

    ScrollBar.vertical: ScrollBar { }
}
