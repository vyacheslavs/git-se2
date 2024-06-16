import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Hello World")
    id: win

    Material.theme: Material.Dark
    Material.accent: Material.Blue

    ColumnLayout {
        id: col
        width: parent.width
        height: parent.height

        TabBar {
            id: bar
            Layout.preferredHeight: 50
            Layout.preferredWidth: col.width
            TabButton {
                text: qsTr("Tab1")
            }
            TabButton {
                text: qsTr("Tab2")
            }
        }

        ScrollView {
            id: scroll_view
            Layout.preferredWidth:  col.width
            Layout.preferredHeight: col.height - bar.height
            clip: true

            RowLayout {
                SplitView {
                    Layout.preferredWidth: scroll_view.width
                    Layout.preferredHeight: scroll_view.height

                    TreeView {
                        id: treeView
                        clip: true
                        SplitView.preferredHeight: scroll_view.height
                        SplitView.preferredWidth: 150
                    }

                    Item {
                        // here shoud be a patch view
                    }
                }
                SplitView {
                    Layout.preferredWidth: scroll_view.width
                    Layout.preferredHeight: scroll_view.height

                    Item {
                        SplitView.preferredWidth: 150
                    }

                    Item {

                    }
                }
            }
        }
    }
}
