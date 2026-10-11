import QtQuick

MouseArea {
    acceptedButtons: Qt.LeftButton
    preventStealing: true
    cursorShape: pressed ? Qt.ClosedHandCursor : Qt.OpenHandCursor
    signal moved(point delta)
    property point _previousPosition
    onPressed: (mouse) => { _previousPosition = mapToItem(null, mouse.x, mouse.y) }
    onPositionChanged: (mouse) => {
        if (pressed) {
            const position = mapToItem(null, mouse.x, mouse.y)
            moved(Qt.point(position.x - _previousPosition.x, position.y - _previousPosition.y))
            _previousPosition = position
        }
    }
}
