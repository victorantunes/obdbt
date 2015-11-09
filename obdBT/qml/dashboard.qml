/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.2
import QtQuick.Window 2.1
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Extras 1.4

Rectangle {
    id: root
    visible: true
    width: 500
    height: 200

    color: "#161616"
    transformOrigin: Item.Center

    ValueSource {
        id: valueSource
    }

    // Dashboards are typically in a landscape orientation, so we need to ensure
    // our height is never greater than our width.
    Item {
        id: container
        width: root.width
        //height: 200
        height: root.height
        anchors.centerIn: parent

        Row {
            id: gaugeRow
            spacing: container.width * 0.02
            anchors.centerIn: parent

            Item {
                x: 0
                y: 0
                width: 100
                //width: 100
                //height: 100
                height: 100
                transformOrigin: Item.Center

                CircularGauge {
                    id: fuelGauge
                    objectName: "combustivel"
                    y: 13
                    maximumValue: 1
                    width: parent.width
                    height: parent.height * 0.7
                    value: 0

                    style: IconGaugeStyle {
                        id: fuelGaugeStyle

                        icon: "qrc:/images/fuel-icon.png"
                        minWarningColor: Qt.rgba(0.5, 0, 0, 1)

                        tickmarkLabel: Text {
                            color: "white"
                            visible: styleData.value === 0 || styleData.value === 1
                            font.pixelSize: fuelGaugeStyle.toPixels(0.225)
                            text: styleData.value === 0 ? "E" : (styleData.value === 1 ? "F" : "")
                        }
                    }
                }

                CircularGauge {
                    objectName: "temperatura"
                    maximumValue: 1
                    width: parent.width
                    height: parent.height * 0.7
                    value: 1
                    y: parent.height / 2 + container.height * 0.01

                    style: IconGaugeStyle {
                        id: tempGaugeStyle

                        icon: "qrc:/images/temperature-icon.png"
                        maxWarningColor: Qt.rgba(0.5, 0, 0, 1)

                        tickmarkLabel: Text {
                            color: "white"
                            visible: styleData.value === 0 || styleData.value === 1
                            font.pixelSize: tempGaugeStyle.toPixels(0.225)
                            text: styleData.value === 0 ? "C" : (styleData.value === 1 ? "H" : "")
                        }
                    }
                }
            }

            CircularGauge {
                id: speedometer
                objectName: "velocidade"
                //width: 200
                //height: 200
                width: root.width*0.4
                height: root.height
                value: valueSource.kph
                anchors.verticalCenter: parent.verticalCenter
                maximumValue: 280
                // We set the width to the height, because the height will always be
                // the more limited factor. Also, all circular controls letterbox
                // their contents to ensure that they remain circular. However, we
                // don't want to extra space on the left and right of our gauges,
                // because they're laid out horizontally, and that would create
                // large horizontal gaps between gauges on wide screens.

                style: DashboardGaugeStyle {}
            }

            CircularGauge {
                id: tachometer
                objectName: "rpm"
                //width: 150
                //height: 150
                width: root.width*0.3
                height: root.height*0.75
                value: valueSource.rpm
                maximumValue: 8
                anchors.verticalCenter: parent.verticalCenter

                style: TachometerStyle {}
            }

        }


        FocusScope {
            id: focusScope1
            width: 100
            height: 100
        }

        Item {
            id: item1
            width: 200
            height: 200


            Label {
                id: label1
                x: 16
                y: 151
                color: "#ffffff"
                text: qsTr("Fluxo de Ar")
            }

            ProgressBar {
                id: progBarAr
                objectName: "progBarfluxo"
                x: 14
                y: 177
                width: 110
                height: 23
                maximumValue: 20
            }

            Label {
                id: label2
                x: 16
                y: 105
                color: "#ffffff"
                text: qsTr("Borboleta")
            }

            ProgressBar {
                id: progBarBorboleta
                objectName: "progBarBorboleta"
                x: 15
                y: 122
                width: 110
                height: 23
                value: 0
            }
        }
    }
}
