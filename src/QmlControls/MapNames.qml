pragma Singleton

import QtQuick

QtObject {
    function displayName(rawName) {
        const types = {
            "Road": qsTr("Road"), "Street": qsTr("Street"), "Street Map": qsTr("Street map"),
            "Streets": qsTr("Streets"), "StreetsBasic": qsTr("Basic streets"),
            "World Street": qsTr("World street map"), "World Satellite": qsTr("World satellite map"),
            "Satellite": qsTr("Satellite"), "Satellite Map": qsTr("Satellite map"), "Sat": qsTr("Satellite"),
            "Hybrid": qsTr("Hybrid"), "Terrain": qsTr("Terrain"), "Labels": qsTr("Labels"),
            "Light": qsTr("Light"), "Dark": qsTr("Dark"), "Bright": qsTr("Bright"),
            "Outdoors": qsTr("Outdoors"), "Custom": qsTr("Custom"), "Map": qsTr("Map"),
            "Basemap": qsTr("Basemap"), "Standard": qsTr("Standard"), "Seamless": qsTr("Seamless"),
            "Std": qsTr("Standard"), "Seamlessphoto": qsTr("Seamless"),
            "Contour": qsTr("Contour"),
            "Topo": qsTr("Topographic map"),
            "Anaglyph": qsTr("Anaglyph"), "Slope": qsTr("Slope"), "Relief": qsTr("Relief"),
            "Elevation": qsTr("Elevation"), "OpenStreetMap": qsTr("OpenStreetMap"),
            "OpenAIP": qsTr("OpenAIP"), "CustomURL": qsTr("Custom URL"),
            "Copernicus": qsTr("Copernicus elevation"), "Terrarium": qsTr("Terrarium elevation")
        }
        if (types[rawName]) {
            return types[rawName]
        }
        const separator = rawName.indexOf(" ")
        if (separator > 0) {
            const provider = rawName.substring(0, separator)
            const type = rawName.substring(separator + 1)
            if (types[type]) {
                return provider + " · " + types[type]
            }
        }
        return rawName
    }
}
