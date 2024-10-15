import QtQml 2.15
import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Dialogs 1.3

import WordsCounter 1.0


Window {
    width: 340
    height: 880
    visible: true
    
    title: wordsCounter.fileName ? wordsCounter.fileName : "Выберите файл..."
    
    WordsCounter {
        id: wordsCounter
    }
    
    OrientedGridLayout {
        id: adaptiveLayout
        anchors.fill: parent
        anchors.margins: columnSpacing
        rowSpacing: columnSpacing
        layoutDirection: Qt.RightToLeft
        
        ColumnLayout {
            Layout.fillHeight: true
            Layout.fillWidth: true
            
            BarChart {
                id: barChart
                Layout.fillHeight: true
                Layout.fillWidth: true
                verticalBars: height > width
                
                property int countBars: 15
                
                function* getBarColor() {
                    for (let color of ["orange", "purple","green", "red", "blue"])
                        yield color
                }
                
                labelTitle.text: `Топ ${countBars} самых частых слов`
                labelValueAxis.text: "Количество вхождений"
                labelNameAxis.text: "Слова"
                
                Timer {
                    interval: 33
                    repeat: true
                    triggeredOnStart: true
                    running: true
                    
                    onTriggered: {
                       barChart.model = wordsCounter.getFrequentWords(
                           barChart.countBars);
                    }
                }
            }
            
            Rectangle {
                id: progressBar
                
                Layout.fillWidth: true
                Layout.fillHeight: false
                Layout.preferredHeight: progressBarLabel.height
                
                property double current: wordsCounter.progressCurrent
                property double total: wordsCounter.progressTotal
                
                color: "transparent"
                border.width: 1
                border.color: "violet"
                
                Rectangle {
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                    width: parent.width * parent.current / 
                           (parent.total > 0 ? parent.total : Infinity);      
                    color: "lightgreen"
                    z: parent.z - 1
                }
                
                Label {
                    id: progressBarLabel
                    anchors.centerIn: parent
                    font.pointSize: 16
                    text: `Прогресс обработки файла: ${parent.current}/${parent.total}`
                }
            }
        }
        
        ColumnLayout {
            Layout.fillHeight: !adaptiveLayout.verticalActually
            Layout.fillWidth: adaptiveLayout.verticalActually
            
            component ButtonAutoWidth: Button {
                Layout.fillWidth: true
                Layout.fillHeight: true
            }
            
            ButtonAutoWidth {
                text: "Открыть"
                
                onClicked: {
                    fileDialogOpen.open();
                }
                
                FileDialog {
                    id: fileDialogOpen
                    
                    selectExisting: true
                    selectFolder: false
                    selectMultiple: false
                    folder: shortcuts.desktop
                    nameFilters: [
                        "Text files (*.txt)",
                        "All files (*)"
                    ]
                    
                    onAccepted: {
                        wordsCounter.fileName = fileUrl.toString();
                    }
                    
                }
            } 
            
            ButtonAutoWidth {
                text: "Старт"
                
                onClicked: {
                    wordsCounter.resumeFileProcessing() ||
                            wordsCounter.startFileProcessing();
                }
            }      
            
            ButtonAutoWidth {
                text: "Пауза"
                
                onClicked: {
                    wordsCounter.pauseFileProcessing();
                }
            }          
            
            ButtonAutoWidth {
                text: "Отмена"
                
                onClicked: {
                    wordsCounter.cancelFileProcessing();
                }
            }
            
        }
    }    
}
