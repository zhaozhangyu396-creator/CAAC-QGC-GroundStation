.pragma library

function createTracker() {
    return { progress: [0, 0, 0, 0, 0, 0], side: -1, angle: 0,
        turn: 0, minimum: 0, maximum: 0, timestamp: 0 }
}

function interrupt(tracker) {
    tracker.side = -1
}

// This measures the guided rotation, not firmware acceptance of magnetic samples.
function sample(tracker, rollDegrees, pitchDegrees, yawDegrees, timestamp) {
    if (![rollDegrees, pitchDegrees, yawDegrees, timestamp].every(Number.isFinite)) {
        interrupt(tracker)
        return
    }
    const radians = Math.PI / 180
    const roll = rollDegrees * radians
    const pitch = pitchDegrees * radians
    const yaw = yawDegrees * radians
    const gravity = [-Math.sin(pitch), Math.cos(pitch) * Math.sin(roll), Math.cos(pitch) * Math.cos(roll)]
    const axis = gravity.map(Math.abs).indexOf(Math.max(...gravity.map(Math.abs)))
    if (Math.abs(gravity[axis]) < Math.cos(30 * radians)) {
        interrupt(tracker)
        return
    }
    const side = axis === 0 ? (gravity[0] > 0 ? 3 : 4)
        : axis === 1 ? (gravity[1] > 0 ? 2 : 1) : (gravity[2] > 0 ? 0 : 5)
    // Project a body axis perpendicular to gravity onto the world horizontal plane.
    const x = axis === 0
        ? Math.cos(yaw) * Math.sin(pitch) * Math.sin(roll) - Math.sin(yaw) * Math.cos(roll)
        : Math.cos(yaw) * Math.cos(pitch)
    const y = axis === 0
        ? Math.sin(yaw) * Math.sin(pitch) * Math.sin(roll) + Math.cos(yaw) * Math.cos(roll)
        : Math.sin(yaw) * Math.cos(pitch)
    const angle = Math.atan2(y, x) / radians
    const delta = ((angle - tracker.angle + 540) % 360) - 180
    if (tracker.side !== side || timestamp <= tracker.timestamp || timestamp - tracker.timestamp > 1000
            || Math.abs(delta) > 45) {
        tracker.turn = 0
        tracker.minimum = 0
        tracker.maximum = 0
    } else {
        tracker.turn += delta
        tracker.minimum = Math.min(tracker.minimum, tracker.turn)
        tracker.maximum = Math.max(tracker.maximum, tracker.turn)
        const sweptAngle = tracker.maximum - tracker.minimum
        tracker.progress[side] = Math.max(tracker.progress[side],
            sweptAngle >= 360 - 1e-7 ? 1 : sweptAngle / 360)
    }
    tracker.side = side
    tracker.angle = angle
    tracker.timestamp = timestamp
}
