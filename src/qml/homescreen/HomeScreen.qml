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
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.4
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import ktouch 1.0

import "../common"

FocusScope {
    id: screen

    property CategorizedResourceSortFilterProxyModel courseModel
    property KeyboardLayout keyboardLayout
    property string keyboardLayoutName
    signal lessonSelected(variant course, variant lesson, variant profile)

    Connections {
        target: profileDataAccess
        onProfileCountChanged: findCurrentProfile()

    }

    function start() {}
    function reset() {
        profileDataAccess.loadProfiles();
    }

    function findCurrentProfile() {
        profileComboBox.profile = null

        var lastProfileId = preferences.lastUsedProfileId

        for (var i = 0; i < profileDataAccess.profileCount; i++) {
            var profile = profileDataAccess.profile(i)
            if (profile.id === lastProfileId) {
                profileComboBox.profile = profile
                return;
            }
        }

        if (profileDataAccess.profileCount > 0) {
            profileComboBox.profile = profileDataAccess.profile(0)
            preferences.lastUsedProfileId = profileComboBox.profile.id
            preferences.writeConfig()
        }
    }

    function safeLastUsedProfile(profile) {
        preferences.lastUsedProfileId = profile.id
        preferences.writeConfig()
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Item {
            id: navigationArea
            z: 2
            Layout.preferredWidth: 300
            Layout.fillHeight: true

            DropShadow {
                anchors.fill: navigationAreaLayout
                source: navigationAreaLayout
                samples: 16
                horizontalOffset: 0
                verticalOffset: 0
            }

            ColumnLayout {
                id: navigationAreaLayout
                anchors.fill: parent
                spacing: 0

                ToolBar {
                    id: header
                    Layout.fillWidth: true
                    visible: courseSelector.opacity > 0

                    background: Rectangle {
                        color: toolbarColorScheme.toolbarBackground
                    }

                    KColorScheme {
                        id: toolbarColorScheme
                        colorGroup: KColorScheme.Active
                        colorSet: KColorScheme.Complementary
                        property color toolbarBackground: Qt.darker(toolbarColorScheme.shade(toolbarColorScheme.hoverDecoration, KColorScheme.MidShade, toolbarColorScheme.contrast, -0.2), 1.3)
                    }


                    RowLayout {
                        anchors.fill: parent
                        spacing: 5


                        ProfileComboBox {
                            id: profileComboBox
                            colorScheme: toolbarColorScheme
                            manageProfileButtonBgColor: toolbarColorScheme.toolbarBackground
                            Layout.fillHeight: true
                            Layout.preferredWidth: 300
                            Layout.fillWidth: true
                            onActivated: {
                                safeLastUsedProfile(profile)
                            }
                        }

                    }
                }

                CourseSelector {
                    id: courseSelector
                    Layout.fillHeight: true
                    Layout.fillWidth: true
                    opacity: 1 - initialProfileForm.opacity
                    profile: profileComboBox.profile
                    keyboardLayout: screen.keyboardLayout
                    currentKeyboardLayoutName: screen.keyboardLayoutName
                }

            }
        }



        LessonSelector {
            Layout.fillHeight: true
            Layout.fillWidth: true
            profile: profileComboBox.profile
            dataIndexCourse: courseSelector.selectedCourse
            onLessonSelected: screen.lessonSelected(course, lesson, profileComboBox.profile)
            z: 1
        }
    }


    InitialProfileForm {
        id: initialProfileForm
        opacity: profileDataAccess.profileCount == 0? 1: 0
        visible: opacity > 0
        anchors.fill: parent
        anchors.margins: 5

        Behavior on opacity {
            NumberAnimation {
                duration: screen.visible? 500: 0
                easing.type: Easing.InOutCubic
            }
        }
    }

    SheetDialog {
        id: profileSelectorSheet
        anchors.fill: parent
        onOpened: {
            if (profileComboBox.profile) {
                var index = profileDataAccess.indexOfProfile(profileComboBox.profile)
                profileSelector.selectProfile(index)
            }
        }
        onClosed: {
            profileButton.checked = false;
        }
        content: ProfileSelector {
            id: profileSelector
            anchors.fill: parent
            onProfileChosen: {
                screen.switchToProfile(profile)
                profileSelectorSheet.close()
            }
        }
    }
}
