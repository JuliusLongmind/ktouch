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

Rectangle {
    property Item control

    height: Math.max(Math.floor(Units.gridUnit / 6), 1)
    color: control.colorScheme.focusDecoration
    opacity: control.activeFocus? 1: 0

    Behavior on opacity {
        NumberAnimation {
            duration: 150
        }
    }
}
