import QtQml 2.15
import QtQuick.Layouts 1.15


GridLayout {
    readonly property bool verticalActually: flow === GridLayout.TopToBottom
    property real criticalHeightToWidthRatio: 1.0
    property bool verticalTarget: height >= (width * criticalHeightToWidthRatio)
    property bool rotateGrid: true
    
    flow: verticalTarget ? GridLayout.TopToBottom : GridLayout.LeftToRight
    
    /// accessible outside
    layoutDirection: Qt.LeftToRight
    
    Component.onCompleted: {
        verticalActuallyChanged.connect(function() {
            if (rotateGrid) {
                [columns, rows] = [rows, columns];
            }
        });
    }
}
