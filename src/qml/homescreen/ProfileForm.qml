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
import QtQuick.Layouts 1.3
import ktouch 1.0

import "../common"

ColumnLayout {
    id: root

    property alias name: nameTextField.text
    property int skillLevel: 0

    property bool skillLevelSelectionEnabled: true
    property alias showWelcomeLabel: welcomeLabel.visible
    property alias doneButtonIconSource: doneBtn.iconName
    property alias doneButtonText: doneBtn.text
    signal done()

    onSkillLevelChanged: {
        beginnerRadioButton.checked = skillLevel == Profile.Beginner
        advancedRadioButton.checked = skillLevel == Profile.Advanced
    }

    spacing: Units.gridUnit

    Label {
        id: welcomeLabel
        Layout.fillWidth: true
        Layout.preferredWidth: 24 * Units.gridUnit
        text: i18n("Before you start training, please introduce yourself:")
    }

    TextField {
        id: nameTextField
        Layout.fillWidth: true
        placeholderText: i18n("Name")
    }

    RadioButton {
        id: beginnerRadioButton
        Layout.maximumWidth: parent.width
        enabled: root.skillLevelSelectionEnabled
        text: i18n("I have no or only very little experience in machine typing")
        label.wrapMode: Text.Wrap
        onCheckedChanged: {
            if (checked) {
                root.skillLevel = Profile.Beginner
            }
        }
    }

    Label {
        text: i18n("Lessons are unlocked as your typing skills improve over time.")
        wrapMode: Text.Wrap
        Layout.maximumWidth: parent.width
        leftPadding: font.pixelSize + Units.largeSpacing
        font.italic: true
        enabled: root.skillLevelSelectionEnabled
    }

    RadioButton {
        id: advancedRadioButton
        Layout.maximumWidth: parent.width
        enabled: root.skillLevelSelectionEnabled
        text: i18n("I am an experienced machine typist and want to improve my skills")
        label.wrapMode: Text.Wrap
        onCheckedChanged: {
            if (checked) {
                root.skillLevel = Profile.Advanced
            }
        }
    }

    Label {
        text: i18n("All lessons are unlocked immediately.")
        wrapMode: Text.Wrap
        Layout.maximumWidth: parent.width
        leftPadding: font.pixelSize * 2
        font.italic: true
        enabled: root.skillLevelSelectionEnabled
    }

    Item {
        Layout.fillWidth: true
        Layout.preferredHeight: doneBtn.implicitHeight

        IconButton {
            id: doneBtn
            anchors.horizontalCenter: parent.horizontalCenter
            bgColor: colorScheme.positiveBackground
            text: i18n("Done")
            enabled: nameTextField.text !== "" && (beginnerRadioButton.checked || advancedRadioButton.checked)
            iconName: "dialog-ok"
            onClicked: done()
        }
    }
}
