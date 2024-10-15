import QtQuick 2.15
import QtQuick.Layouts 1.15
import QtQuick.Controls 2.15

Item {
    property alias model: barRepeater.model
    
    property bool verticalBars: true
    
    property alias labelTitle: labelTitle
    property alias labelNameAxis: labelNameAxis
    property alias labelValueAxis: labelValueAxis
    property alias colors: colors
    
    // TODO: remake into theme
    QtObject {
        id: colors
        
        property var barsColors: ["blue"]
        property color background: "transparent"
        property color barBackground: background
    }
    
    function* getBarColor() {
        for (let color of colors.barsColors) yield color
    }
    
    // readonly property double valueMin: 
    //     model.reduce((acc, item) => Math.min(acc, item.value), Infinity)
    readonly property double valueMax: 
        model.reduce((acc, item) => Math.max(acc, item.value), -Infinity)
    
    function roundWithPrecision(x, floor=true, digits=2, subprecision=0.5) {
        let precision = Math.pow(10, --digits) / subprecision;
        let order = Math.pow(0.1, Math.floor(Math.log10(Math.abs(x))));
        order *= precision;
        x *= order;
        return (floor ? Math.floor(x) : Math.ceil(x )) / order;
    }
    
    // TODO axis with ticks
    // property int valueAxisTicksCount: 0
    
    /// min/max values rounded up to 2+1/2 significant digits
    /// example: 2537.28 -> 2550 , 0.0801 -> 0.085
    // property double valueAxisMin: roundWithPrecision(valueMin, true)
    property double valueAxisMax: roundWithPrecision(valueMax, false)
    
    
    ColumnLayout {
        anchors.fill: parent
        spacing: 0
        
        Label {
            id: labelTitle
            
            visible: text
            font.pointSize: 20
            Layout.alignment: Qt.AlignHCenter
            bottomPadding: 10
        }
        
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: colors.background
            
            OrientedGridLayout {
                anchors.fill: parent
                columns: 2
                rows: 2
                columnSpacing: 0
                rowSpacing: 0
                verticalTarget: !verticalBars
                layoutDirection: Qt.RightToLeft
                
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: colors.barBackground
                    
                    OrientedGridLayout {
                        id: barLayout
                        
                        anchors.fill: parent
                        columns: barRepeater.count
                        columnSpacing: 0
                        rowSpacing: 0
                        verticalTarget: !verticalBars
                        layoutDirection: verticalBars ? Qt.LeftToRight : Qt.RightToLeft
                        
                        Repeater {
                            id: barRepeater
                            
                            function* getColorInfiniteWrapper() {
                                while (true) for (let color of getBarColor()) yield color
                            }
                            property var colorGenerator: getColorInfiniteWrapper()
                            
                            onModelChanged: {
                                colorGenerator = getColorInfiniteWrapper();
                            }
                            
                            delegate: Item {
                                Layout.fillWidth: true
                                Layout.fillHeight: true
                                
                                property color barColor: 
                                    'color' in modelData ?
                                        modelData.color : barRepeater.colorGenerator.next().value
                                
                                // TODO make customizable for user
                                OrientedGridLayout {
                                    anchors.fill: parent
                                    readonly property real padding: 4
                                    property real horizontlaPadding: verticalBars ? padding : 0
                                    property real verticalPadding: !verticalBars ? padding : 0
                                    anchors.rightMargin: horizontlaPadding
                                    anchors.leftMargin: horizontlaPadding
                                    anchors.topMargin: verticalPadding
                                    anchors.bottomMargin: verticalPadding
                                    columnSpacing: 0
                                    rowSpacing: 0
                                    verticalTarget: verticalBars     
                                    layoutDirection: Qt.RightToLeft                               
                                    
                                    Item {
                                        Layout.fillWidth: true
                                        Layout.fillHeight: true
                                        Layout.minimumHeight: 0
                                        Layout.minimumWidth: 0
                                    }
                                    
                                    Label {
                                        id: labelValue
                                        
                                        color: barColor
                                        text: modelData.value
                                        verticalAlignment: Text.AlignBottom
                                        Layout.fillWidth: false
                                        Layout.fillHeight: false
                                        Layout.alignment: verticalBars ? 
                                                              Qt.AlignHCenter | Qt.AlignBottom :
                                                              Qt.AlignVCenter | Qt.AlignLeft
                                        readonly property real padding: 4
                                        leftPadding: !verticalBars ? padding : 0
                                        bottomPadding: verticalBars ? padding : 0
                                    }
                                    
                                    Rectangle {
                                        Layout.fillWidth: verticalBars
                                        Layout.fillHeight: !verticalBars
                                        property double sizeRatio: modelData.value / valueMax
                                        Layout.minimumHeight: 
                                            !verticalBars ? 0 : (parent.height - labelValue.height) * sizeRatio
                                        Layout.minimumWidth: 
                                            verticalBars ? 0 : (parent.width - labelValue.width) * sizeRatio
                                        color: barColor
                                    }            
                                }                        
                            }
                        }
                        
                        Repeater {
                            model: barRepeater.model
                            // TODO make customizable for user
                            delegate: RotationFixer {
                                Layout.fillWidth: false
                                Layout.fillHeight: false
                                Layout.alignment: verticalBars ? 
                                                      Qt.AlignTop |Qt.AlignHCenter :
                                                      Qt.AlignRight | Qt.AlignVCenter
                                
                                Label {
                                    horizontalAlignment: Text.AlignRight
                                    text: modelData.name
                                    rightPadding: 4
                                    rotation: verticalBars ? 270 : 0
                                }  
                            }
                            
                        }
                    }
                }
                
                RotationFixer {
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillHeight: false
                    Layout.fillWidth: false
                    
                    Label {
                        id: labelValueAxis
                        
                        font.bold: true
                        rotation: verticalBars ? 270 : 0
                    }  
                }
                
                RotationFixer {
                    Layout.alignment: Qt.AlignCenter
                    Layout.fillHeight: false
                    Layout.fillWidth: false
                    
                    Label {
                        id: labelNameAxis
                        
                        font.bold: true
                        rotation: !verticalBars ? 270 : 0
                    }  
                }
            }
        }
    }
}
