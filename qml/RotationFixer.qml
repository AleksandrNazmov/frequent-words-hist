import QtQuick 2.15
import QtQuick.Layouts 1.15

/// fix rotated object bounds (especially Label)
Item {
    id: rotationFixer
    
    readonly property var item: children[0]
    
    Layout.preferredHeight: height
    Layout.preferredWidth: width
    height: diagonal(item.rotation,
                     item.implicitWidth,
                     item.implicitHeight)                
    width: diagonal(item.rotation,
                    item.implicitHeight,
                    item.implicitWidth)
    
    onItemChanged: {
        item.anchors.centerIn = rotationFixer;
    }
    
    
    function degToRad(degrees) {
        return degrees * (Math.PI / 180);
    }
    
    function radToDeg(rad) {
        return rad / (Math.PI / 180);
    }
    
    function diagonal(rot, w, h) {
        let angle1 = degToRad(rot);
        let angle2 = angle1 + (Math.PI / 2.0);
        return Math.abs(Math.sin(angle1)) * w + Math.abs(Math.sin(angle2)) * h;
    }
}
