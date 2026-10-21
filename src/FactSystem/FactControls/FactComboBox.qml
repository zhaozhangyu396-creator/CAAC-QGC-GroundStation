import QtQuick
import QtQuick.Controls

import QGroundControl
import QGroundControl.Controls

QGCComboBox {
    property Fact fact: Fact { }
    property bool indexModel: fact ? fact.enumValues.length === 0 : true // true: Fact values are indices, false: Fact values are FactMetadata.enumValues
    property bool deferred: false
    readonly property bool modified: _pendingIndex >= 0
    property int _pendingIndex: -1

    model: fact ? OnlineMessageTranslator.parameterOptions(fact.enumStrings, OnlineMessageTranslator.revision) : null

    currentIndex: _pendingIndex >= 0 ? _pendingIndex : fact ? (indexModel ? fact.value : fact.enumIndex) : 0

    function applyPendingValue() {
        if (!fact || _pendingIndex < 0) { return }
        const index = _pendingIndex
        if (indexModel) {
            fact.value = index
        } else if (index < fact.enumValues.length) {
            fact.value = fact.enumValues[index]
        }
        _pendingIndex = -1
    }

    function _updateCurrentIndex() {
        Qt.callLater(function() {
            currentIndex = Qt.binding(function() {
                return _pendingIndex >= 0 ? _pendingIndex : fact ? (indexModel ? fact.value : fact.enumIndex) : 0
            })
        })
    }

    onModelChanged: {
        // When the model changes, the index gets reset to 0, so re-establish
        // the declarative currentIndex binding. callLater() avoids a binding
        // loop since enumIndex could trigger a model change.
        _updateCurrentIndex()
    }

    onFactChanged: {
        _pendingIndex = -1
        // When the fact changes to a different Fact object with the same
        // enumStrings, modelChanged does not fire, so the binding must be
        // re-established here to point at the new Fact.
        _updateCurrentIndex()
    }

    onActivated: (index) => {
        if (deferred) {
            _pendingIndex = index
        } else if (indexModel) {
            fact.value = index
        } else {
            fact.value = fact.enumValues[index]
        }
    }
}
