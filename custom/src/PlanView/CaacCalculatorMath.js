.pragma library

function failure(error) { return {ok: false, error: error} }
function parsed(text) { return String(text).trim().length ? Number(text) : NaN }
function normalized(angle) { return (angle % 360 + 360) % 360 }

function windHeading(wind, relative) {
    if (!isFinite(wind) || wind < 0 || wind > 360 || !isFinite(relative)) { return failure("wind") }
    return {ok: true, value: normalized(wind - relative)}
}

function timeAngle(hours, minutes) {
    if (!Number.isInteger(hours) || hours < 0 || hours > 23
        || !Number.isInteger(minutes) || minutes < 0 || minutes > 59) { return failure("time") }
    return {ok: true, value: hours * 15 + minutes / 4}
}

function trigonometry(operation, value, radians) {
    if (!isFinite(value) || operation < 0 || operation > 5 || !Number.isInteger(operation)) { return failure("number") }
    if ((operation === 3 || operation === 4) && Math.abs(value) > 1) { return failure("domain") }
    const angle = radians ? value : value * Math.PI / 180
    if (operation === 2 && Math.abs(Math.cos(angle)) < 1e-14) { return failure("tangent") }
    const functions = [Math.sin, Math.cos, Math.tan, Math.asin, Math.acos, Math.atan]
    const result = functions[operation](operation < 3 ? angle : value)
    const converted = operation >= 3 && !radians ? result * 180 / Math.PI : result
    return isFinite(converted) ? {ok: true, value: converted} : failure("number")
}

function triangle(values) {
    const known = Object.keys(values).filter(key => values[key] !== null && values[key] !== undefined)
    if (known.length !== 2 || !known.some(key => key !== "theta")) { return failure("known") }
    for (const key of known) {
        if (!["a", "b", "c", "theta"].includes(key) || !isFinite(values[key]) || values[key] <= 0) { return failure("sides") }
    }
    let a, b, c
    if (known.includes("theta")) {
        const theta = values.theta
        if (theta >= 90) { return failure("angle") }
        const angle = theta * Math.PI / 180
        if (known.includes("c")) { c = values.c; a = c * Math.sin(angle); b = c * Math.cos(angle) }
        else if (known.includes("a")) { a = values.a; c = a / Math.sin(angle); b = a / Math.tan(angle) }
        else { b = values.b; c = b / Math.cos(angle); a = b * Math.tan(angle) }
    } else if (known.includes("a") && known.includes("b")) {
        a = values.a; b = values.b; c = Math.hypot(a, b)
    } else {
        c = values.c
        const side = known.includes("a") ? values.a : values.b
        if (side >= c) { return failure("hypotenuse") }
        const other = c * Math.sqrt((1 - side / c) * (1 + side / c))
        a = known.includes("a") ? side : other
        b = known.includes("b") ? side : other
    }
    if (![a, b, c].every(value => isFinite(value) && value > 0)) { return failure("sides") }
    const theta = Math.atan2(a, b) * 180 / Math.PI
    return {ok: true, a: a, b: b, c: c, theta: theta, phi: 90 - theta}
}
