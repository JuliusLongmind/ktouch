/*
 *  Copyright 2012  Sebastian Gottfried <sebastiangottfried@web.de>
 *  Copyright 2015  Sebastian Gottfried <sebastiangottfried@web.de>
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
import QtQuick.Layouts 1.3
import ktouch 1.0

Item {
    property alias model: repeator.model
    id: root
    height: childrenRect.height

    KColorScheme {
        id: colorScheme
        colorGroup: KColorScheme.Active
        colorSet: KColorScheme.View
    }


    Column {
        id: column
        spacing: Units.largeSpacing
        width: parent.width
        height: childrenRect.height

        Repeater {
            id: repeator

            Row {
                spacing: Units.smallSpacing
                height: Math.max(titleLabel.height, valueLabel.height)
                width: root.width

                Label {
                    id: titleLabel
                    width: Math.round((parent.width - parent.spacing) / 2)
                    horizontalAlignment: Text.AlignRight
                    verticalAlignment: Text.AlignTop
                    color: colorScheme.normalText
                    opacity: 0.7
                    text: model.modelData.title
                    wrapMode: Text.Wrap
                    height: Math.max(paintedHeight, valueLabel.paintedHeight)
                }
                Label {
                    id: valueLabel
                    width: parent.width - titleLabel.width - parent.spacing
                    horizontalAlignment: Text.AlignLeft
                    verticalAlignment: Text.AlignBottom
                    text: model.modelData.text
                    wrapMode: Text.Wrap
                    height: Math.max(paintedHeight, titleLabel.paintedHeight)
                    color: colorScheme.normalText
                }
            }
        }
    }
}
