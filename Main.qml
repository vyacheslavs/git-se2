import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material
import QtQuick.Layouts

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Git Split-Explain")
    id: win

    //Material.theme: Material.Blue
    Material.accent: Material.Blue
    //Material.background: Material.White

    ColumnLayout {
        id: col
        width: parent.width
        height: parent.height

        TabBar {
            id: bar
            Layout.preferredHeight: 50
            Layout.preferredWidth: col.width

            Repeater {
                model: repeater_model

                TabButton {
                    text: modelData.title
                }

            }
        }

        ScrollView {
            id: scroll_view
            Layout.preferredWidth:  col.width
            Layout.preferredHeight: col.height - bar.height
            clip: true

            RowLayout {
                Repeater {
                    model: repeater_model

                    SplitView {
                        Layout.preferredWidth: scroll_view.width
                        Layout.preferredHeight: scroll_view.height

                        TreeView {
                            id: treeView
                            clip: true
                            SplitView.preferredHeight: scroll_view.height
                            SplitView.preferredWidth: 150
                            model: modelData.tree_model
                            alternatingRows: false
                            selectionModel: ItemSelectionModel {}

                            delegate: Item {
                                width: treeView.width
                                height: 30
                                implicitHeight: 30
                                implicitWidth: treeView.width

                                readonly property real indentation: 20
                                required property int depth
                                required property int hasChildren
                                required property bool current
                                required property bool expanded
                                required property bool isTreeNode

                                // Rotate indicator when expanded by the user
                                // (requires TreeView to have a selectionModel)
                                property Animation indicatorAnimation: NumberAnimation {
                                    target: indicator
                                    property: "rotation"
                                    from: expanded ? 0 : 90
                                    to: expanded ? 90 : 0
                                    duration: 100
                                    easing.type: Easing.OutQuart
                                }
                                TableView.onPooled: indicatorAnimation.complete()
                                TableView.onReused: if (current) indicatorAnimation.start()
                                onExpandedChanged: indicator.rotation = expanded ? 90 : 0

                                MouseArea {
                                    id: globalMouseArea
                                    anchors.fill: parent
                                    hoverEnabled: true
                                    cursorShape: containsMouse ? Qt.ClosedHandCursor : Qt.ArrowCursor
                                }

                                Rectangle {
                                    id: background
                                    anchors.fill: parent
                                    color: Material.background
                                    //color: row === treeView.currentRow ? palette.highlight : "black"
                                    // opacity: (treeView.alternatingRows && row % 2 !== 0) ? 0.3 : 0.1
                                }

                                Label {
                                    id: indicator
                                    x: padding + (depth * indentation)
                                    anchors.verticalCenter: parent.verticalCenter
                                    visible: isTreeNode && hasChildren
                                    text: "▶"

                                    TapHandler {
                                        onSingleTapped: {
                                            let index = treeView.index(row, column)
                                            // treeView.selectionModel.setCurrentIndex(index, ItemSelectionModel.NoUpdate)
                                            treeView.toggleExpanded(row)
                                        }
                                    }
                                }

                                Label {
                                    id: label
                                    x: padding + (isTreeNode ? (depth + 1) * indentation : 0)
                                    anchors.verticalCenter: parent.verticalCenter
                                    width: parent.width - padding - x
                                    clip: true
                                    text: model.display
                                    color: model.diff_type
                                    font.bold: current && !hasChildren

                                    TapHandler {
                                        onSingleTapped: {
                                            if (hasChildren) {
                                                treeView.toggleExpanded(row)
                                            } else {
                                                treeView.selectionModel.setCurrentIndex(treeView.index(row, column), ItemSelectionModel.NoUpdate)
                                            }
                                        }
                                    }
                                }
                            }
                        }

                        Item {
                            // here shoud be a patch view
                        }
                    }
                }
            }
        }
    }
}
