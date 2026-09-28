// paramLogic.js - скрипт для работы с логикой и обновления UI аналоговых компонентов AI
.pragma library
.import QtQml as QQml //импорт модуля QtQml, для доступа к Component при создании контексного меню и паспорта, т.к. JS его нет

//хранилище
var _passportStore = {} //паспорта компонентов
var _contextMenuInstance = null //экземпляр контекстного меню

//конфигурация свойств для обновления
var _propertyConfig = {
    pv: Number,
    ah: Number,
    ah2: Number,
    al: Number,
    al2: Number,
    hyst: Number,
    wh: Number,
    wl: Number,
    lrv: Number,
    urv: Number,
    bcw: Number, //borders check word (Byte)
    stw: Number,
    sim: Boolean,
    timestamp: v => v,
    unit: v => v,
    description: v => v,
    tagname: v => v
}

//Паспорт
function _showPassport(obj, path) {
    if (!obj) return null

    var passport = _passportStore[obj]
    if (passport && passport.visible) {
        passport.raise()
        passport.requestActivate()
        return passport
    }

    //создаем новый паспорт
    var component = Qt.createComponent(path || "PassportParam.qml")
    if (component.status === QQml.Component.Ready) {
        passport = component.createObject(obj)
        if (passport) {
            passport.updateData({
                                    name: obj.name,
                                    symbol: obj.symbol,
                                    pv: obj.pv,
                                    ah: obj.ah,
                                    ah2: obj.ah2,
                                    al: obj.al,
                                    al2: obj.al2,
                                    hyst: obj.hyst,
                                    wh: obj.wh,
                                    wl: obj.wl,
                                    lrv: obj.lrv,
                                    urv: obj.urv,
                                    bcw: obj.bcw,
                                    stw: obj.stw,
                                    sim: obj.sim,
                                    timestamp: obj.timestamp,
                                    unit: obj.unit,
                                    description: obj.description,
                                    tagname: obj.tagname,
                                    location: "ГТЭС"
                                })

            passport.closing.connect(function() {
                _passportStore[obj] = null
                passport.destroy()
            })

            _passportStore[obj] = passport
            passport.show()
            return passport
        }
    }
    return null
}

function updateData(obj, data) {
    if (!obj || !data) return

    // obj.pv = data.pv !== undefined ? Number(data.pv) : obj.pv;

    //обновляем свойства
    Object.keys(_propertyConfig).forEach(function(key) { //forEach - обновляем каждый элемент массива propertyConfig
        if (data[key] !== undefined) {
            var newVal = _propertyConfig[key](data[key])
            if (obj[key] !== newVal) {
                obj[key] = newVal
            }
        }
    })

    //oбновляем паспорт если открыт
    var passport = _passportStore[obj]
    if (passport && passport.visible) {
        passport.updateData({
                                pv: obj.pv,
                                ah: obj.ah,
                                ah2: obj.ah2,
                                al: obj.al,
                                al2: obj.al2,
                                hyst: obj.hyst,
                                wh: obj.wh,
                                wl: obj.wl,
                                lrv: obj.lrv,
                                urv: obj.urv,
                                bcw: obj.bcw,
                                stw: obj.stw,
                                sim: obj.sim,
                                timestamp: obj.timestamp,
                            })
    }
}

//Контекстное меню
function showContextMenu(obj, path) {
    if (!obj) return null

    //закрываем предыдущее меню если открыто
    if (_contextMenuInstance) {
        _contextMenuInstance.close()
        _contextMenuInstance.destroy()
        _contextMenuInstance = null
    }

    var component = Qt.createComponent(path || "ContextMenu.qml")
    if (component.status === QQml.Component.Ready) {
        var contextMenu = component.createObject(obj)
        if (contextMenu) {
            //Обновляем данные
            contextMenu.name = obj.name;
            contextMenu.symbol = obj.symbol;
            contextMenu.addToChart = obj.addTag;

            //Подключаем сигналы
            contextMenu.emitAddToChart.connect(function(symbolFromMenu) {
                obj.addTag = !obj.addTag
                var data = {
                    addTag: obj.addTag,
                    name: obj.name,
                    symbol: obj.symbol,
                    unit: obj.unit,
                    description: obj.description,
                    tagname: obj.tagname
                }
                obj.emitAddToChart(data)
            });
            contextMenu.emitShowPassport.connect(function(symbolFromMenu) {
                _showPassport(obj, "PassportParam.qml")
            });

            contextMenu.closed.connect(function() {
                _contextMenuInstance = null
                contextMenu.destroy();
            })
            _contextMenuInstance = contextMenu
            contextMenu.open()
            return contextMenu
        }
    }
    return null
}

function getBit(value, bitPosition) {
    return (value >> bitPosition) & 1; //сдвигаем биты числа вправо на bitPosition позиций & 1 - побитовое И с числом 1
}

function getStatusColor(obj) {
    if (!obj) return "#4f4f4f"
    if (obj.sim) return "#BF88BF" //Simulation
    if (obj.stw & 0x02) return "#5b6262" //bit1 - Bad
    if (obj.stw & 0x08) return "#ff0000" //bit2 - Alarm
    if (obj.stw & 0x04) return "#ffA000" //bit3 - Warning
    return "#4f4f4f"
}

function getProgressColor(obj) {
    if (!obj) return "#2ecc71"
    if (obj.stw & 0x02) return "#5b6262" //bit1 - Bad
    if (obj.stw & 0x08) return "#ff0000" //bit2 - Alarm
    if (obj.stw & 0x04) return "#ffA000" //bit3 - Warning
    return "#2ecc71"
}

function getAlarm(obj) {
    return (obj.stw & 0x02) || (obj.stw & 0x04) || (obj.stw & 0x08)
}

//Функция снятия визуализации на элементе (при удалении графиков)(визуализация в контекстном меню)
function updateAddToChartIcon(obj, data) {
    if (data.symbol === obj.symbol) {
        obj.addTag = data.addTag
    }
}

