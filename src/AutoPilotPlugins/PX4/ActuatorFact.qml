import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import QGroundControl
import QGroundControl.Controls
import QGroundControl.FactControls

Loader {
	property var fact
	property bool constrainedWidth: QGroundControl.corePlugin.directMissionItemEditing
	Layout.minimumWidth: 0
	Layout.preferredWidth: constrainedWidth ? ScreenTools.defaultFontPixelWidth * 9 : -1
	Layout.fillWidth: constrainedWidth
	id:                     loader

	Component {
		id: factComboBox
		FactComboBox {
			fact:           loader.fact
			indexModel:     false
			sizeToContents: !loader.constrainedWidth
			wrapCurrentText: loader.constrainedWidth
		}
	}
	Component {
		id: factCheckbox
		FactCheckBox {
			fact:           loader.fact
		}
	}

	Component {
		id: factTextField
		FactTextField {
			fact:           loader.fact
			width:          ScreenTools.defaultFontPixelWidth * 8
			showUnits:      false
		}
	}
	Component {
		id: factReadOnly
		QGCLabel {
			text:           loader.fact.valueString
		}
	}
	Component {
		id: notAvailable
		QGCLabel {
			text:           qsTr("(Param not available)")
		}
	}
	sourceComponent: fact ?
		(fact.enumStrings.length > 0 ? factComboBox :
			(fact.readOnly ? factReadOnly : (fact.typeIsBool ? factCheckbox : factTextField))
		) : notAvailable
}
