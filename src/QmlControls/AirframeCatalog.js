.pragma library

function image(name) {
    return "qrc:/qmlimages/Airframe/Uploaded/" + name + ".svg"
}

function categories() {
    return [
        {key: "multirotor", name: qsTranslate("AirframeCatalog", "多旋翼"), image: image("quadrotor-x")},
        {key: "fixedwing", name: qsTranslate("AirframeCatalog", "固定翼"), image: image("standard-plane")},
        {key: "vtol", name: qsTranslate("AirframeCatalog", "垂起固定翼"), image: image("standard-vtol")},
        {key: "rover", name: qsTranslate("AirframeCatalog", "无人车"), image: image("rover")},
        {key: "airship", name: qsTranslate("AirframeCatalog", "飞艇"), image: image("airship")},
        {key: "other", name: qsTranslate("AirframeCatalog", "直升机"), image: image("helicopter")}
    ]
}

function categoryName(key) {
    const item = categories().find(function(item) { return item.key === key })
    return item ? item.name : ""
}

function models(category) {
    const entries = [
        ["multirotor", "quadrotor-x", qsTranslate("AirframeCatalog", "四旋翼 X 型")],
        ["multirotor", "quadrotor-plus", qsTranslate("AirframeCatalog", "四旋翼 + 型")],
        ["multirotor", "quadrotor-h", qsTranslate("AirframeCatalog", "四旋翼 H 型")],
        ["multirotor", "tricopter-y-plus", qsTranslate("AirframeCatalog", "三旋翼 Y 型")],
        ["multirotor", "hexarotor-x", qsTranslate("AirframeCatalog", "六旋翼 X 型")],
        ["multirotor", "hexarotor-plus", qsTranslate("AirframeCatalog", "六旋翼 + 型")],
        ["multirotor", "hexarotor-coaxial", qsTranslate("AirframeCatalog", "六旋翼同轴")],
        ["multirotor", "octorotor-x", qsTranslate("AirframeCatalog", "八旋翼 X 型")],
        ["multirotor", "octorotor-plus", qsTranslate("AirframeCatalog", "八旋翼 + 型")],
        ["multirotor", "octorotor-coaxial", qsTranslate("AirframeCatalog", "八旋翼同轴")],
        ["multirotor", "dodecarotor-coaxial", qsTranslate("AirframeCatalog", "十二旋翼同轴")],
        ["fixedwing", "standard-plane", qsTranslate("AirframeCatalog", "标准固定翼")],
        ["fixedwing", "flying-wing", qsTranslate("AirframeCatalog", "飞翼")],
        ["fixedwing", "plane-a-tail", qsTranslate("AirframeCatalog", "A 尾固定翼")],
        ["vtol", "standard-vtol", qsTranslate("AirframeCatalog", "标准垂起固定翼")],
        ["vtol", "vtol-tailsitter", qsTranslate("AirframeCatalog", "尾座式垂起固定翼")],
        ["vtol", "vtol-tiltrotor", qsTranslate("AirframeCatalog", "倾转旋翼垂起固定翼")],
        ["rover", "rover", qsTranslate("AirframeCatalog", "地面无人车")],
        ["airship", "airship", qsTranslate("AirframeCatalog", "飞艇")],
        ["airship", "balloon", qsTranslate("AirframeCatalog", "气球")],
        ["other", "helicopter", qsTranslate("AirframeCatalog", "直升机")],
        ["other", "autogyro", qsTranslate("AirframeCatalog", "自转旋翼机")],
        ["other", "underwater-robot", qsTranslate("AirframeCatalog", "水下机器人")],
        ["other", "vectored-6dof-uuv", qsTranslate("AirframeCatalog", "六自由度潜航器")],
        ["other", "free-flyer", qsTranslate("AirframeCatalog", "自由飞行器")],
        ["other", "simulation", qsTranslate("AirframeCatalog", "仿真机架")],
        ["other", "boat", qsTranslate("AirframeCatalog", "水面无人船"), "qrc:/qmlimages/Airframe/Boat.svg"]
    ]
    return entries.filter(function(item) { return item[0] === category }).map(function(item) {
        return {key: item[1], name: item[2], image: item[3] || image(item[1])}
    })
}

function px4Image(groupName, fallbackUrl) {
    const names = {
        "airship": "airship", "autogyro": "autogyro", "balloon": "balloon",
        "dodecarotor cox": "dodecarotor-coaxial", "flying wing": "flying-wing",
        "free-flyer": "free-flyer", "helicopter": "helicopter",
        "hexarotor coaxial": "hexarotor-coaxial", "hexarotor +": "hexarotor-plus", "hexarotor x": "hexarotor-x",
        "octorotor coaxial": "octorotor-coaxial", "octorotor +": "octorotor-plus", "octorotor x": "octorotor-x",
        "plane a-tail": "plane-a-tail", "quadrotor h": "quadrotor-h", "quadrotor +": "quadrotor-plus",
        "quadrotor x": "quadrotor-x", "rover": "rover", "simulation": "simulation",
        "standard plane": "standard-plane", "standard vtol": "standard-vtol", "tricopter y+": "tricopter-y-plus",
        "underwater robot": "underwater-robot", "vectored 6 dof uuv": "vectored-6dof-uuv",
        "vtol tailsitter": "vtol-tailsitter", "vtol tiltrotor": "vtol-tiltrotor"
    }
    const name = names[groupName.toLowerCase()]
    return name ? image(name) : fallbackUrl
}
