/* Copyright (C) 2015  Moritz Nisblé <moritz.nisble@gmx.de>
**
** This file is part of QTouch.
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License along
** with this program; if not, write to the Free Software Foundation, Inc.,
** 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
**/
import QtQuick 2.3
import QtQuick.Controls 1.3

import "items" as Items

FocusScope {
    id: root

    property var courseModel
    property var profileModel
    property alias document: lessonSelector.document

    signal lessonStarted
    Component.onCompleted: {
        lessonSelector.onLessonStarted.connect(lessonStarted)
    }

    // Forward keys to the selectors as long as the ProfileScreen isn't enabled
    Keys.forwardTo: [courseSelector, lessonSelector]
    Keys.enabled: !profileScreen.open

    ToolBar {
        id: toolBar
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        ToolButton {
            id: btProfile
            anchors {
                left: parent.left
                verticalCenter: parent.verticalCenter
                leftMargin: 10
            }
            iconSource: "qrc:/icons/32x32/user-identity.png"
            iconName: "user-identity"
            checkable: true
        } // btProfile

        ToolButton {
            id: btConfig
            anchors {
                right: parent.right
                verticalCenter: parent.verticalCenter
                rightMargin: 10
            }
            iconSource: "qrc:/icons/32x32/configure.png"
            iconName: "configure"
        } // btConfig
    } // toolBar

    Item {
        id: container

        anchors {
            top: toolBar.bottom
            left: root.left
            right: root.right
            bottom: root.bottom
        }

        CourseSelector {
            id: courseSelector

            anchors {
                top: container.top
                left: container.left
                right: container.right

                topMargin: 5
            }

            focus: !profileScreen.open

            courseModel: root.courseModel
        } // courseSelector

        // Label that is inflated the button that is placed
        // on the PathView delegate of CourseSelector.
        Items.InflatedLabel {
            id: lblCourseDescription

            anchors {
                top: courseSelector.bottom
                topMargin: 5
                right: container.right
                rightMargin: 5
                left: container.left
                leftMargin: 5

                bottomMargin: 10
            }

            text: root.courseModel.course.description

            inflated: courseSelector.courseDescriptionBottonChecked
        } // lblCourseDescription

        LessonSelector {
            id: lessonSelector
            anchors {
                top: lblCourseDescription.bottom
                right: container.right
                left: container.left
                bottom: container.bottom

                topMargin: 5
            }
            // Fill the column
            height: container.height - courseSelector.height - lblCourseDescription.height

            lessonModel: root.courseModel.lessonModel
        } // lessonSelector
    } // container

    ProfileScreen {
        id: profileScreen

        anchors {
            top: toolBar.bottom
            right: root.right
            left: root.left
            bottom: root.bottom
        }

        focus: btProfile.checked
        open: btProfile.checked

        profileModel: root.profileModel

        // React to output signals
        onCreateProfile: {
            console.log("Creating profile: " + name + " (" + skilllevel + ")")
            root.profileModel.addProfile(name, skilllevel)
        }
    } // profileScreen

    Connections {
        target: root.profileModel
        onProfileChanged: {
            for (var i = 0; i < root.profileModel.profile.stats.length; i++) {
                var stats = root.profileModel.profile.stats[i]
                console.log("Status:", i, stats.course, stats.lesson,
                            stats.profile, stats.start, stats.time, stats.chars,
                            stats.errors)
            }
        }
    }
} // Item
