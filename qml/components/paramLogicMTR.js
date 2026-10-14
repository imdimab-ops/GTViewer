// paramLogicMTR.js - скрипт для работы с логикой и обновления UI электродвигателей Motor MTR см.PsTechEE
.pragma library
.import QtQml as QQml //импорт модуля QtQml, для доступа к Component при создании контексного меню и паспорта, т.к. JS его нет

//хранилище
var _passportStore = {} //паспорта компонентов
var _contextMenuInstance = null //экземпляр контекстного меню


//конфигурация свойств для обновления (_propertyConfig — это объект-«схема», где для каждого свойства задана функция-преобразователь)
var _propertyConfig = {
    mode: Number,
    state: Number,
    ctlw: Number,
    diagnw: Number,
    blockw: Number,
    diagnd: v => v,
    blockd: v => v,
    load: Number,
    worktime: Number,
    timestamp: v => v,
    unit: v => v,
    description: v => v,
    tagname: v => v
}

/* Показываем паспорт параметра */
function _showPassport(obj, path) {
    if (!obj) return null

    var passport = _passportStore[obj]
    if (passport && passport.visible) {
        passport.raise()
        passport.requestActivate()
        return passport
    }

    //создаем новый паспорт
    var component = Qt.createComponent(path || "PassportParamMTR.qml")
    if (component.status === QQml.Component.Ready) {
        passport = component.createObject(obj)
        if (passport) {
            passport.updateData({
                                    name: obj.name,
                                    symbol: obj.symbol,
                                    running: obj.running,
                                    block: obj.block,
                                    fault: obj.fault,
                                    mode: obj.mode,
                                    state: obj.state,
                                    ctlw: obj.ctlw,
                                    diagnw: obj.diagnw,
                                    blockw: obj.blockw,
                                    diagnd: obj.diagnd,
                                    blockd: obj.blockd,
                                    load: obj.load,
                                    nominalLoad: obj.nominalLoad,
                                    worktime: obj.worktime,
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

/* функция обновления состояния двигателя (работа, отказ, ошибка) */
function _updateStatus(obj, key) {
    if (!obj || !key) return
    switch (key) {
    case 'state':
        obj.running = (obj.state === 2) //см.PsTechEE MTR ENUM_MTR_STATE
        break
    case 'diagnw':
        obj.fault = (obj.diagnw & 0x01) === 0 //маска bit0 - ОК (Все в норме или выбран режим "Ремонтный") см.PsTechEE MTR DIAGN
        break
    case 'blockw':
        /* g 'blockw' отображаем в паспорте и не взводим "block",
         * тк у Regul это "Признаки действующих технологических команд и запретов" */
        break
    default:
        break
    }
}

/* !!!свойства каждого элемента obj обновляем из функции handleDataUpdate(arg) connectionsLogic.js */
function updateData(obj, data) {
    if (!obj || obj.type !== "MTR" || !data) return

    //обновляем свойства
    Object.keys(_propertyConfig).forEach(function(key) { //forEach - обновляем каждый элемент массива propertyConfig
        if (data[key] !== undefined) {
            var newVal = _propertyConfig[key](data[key])    //_propertyConfig["mode"] -> Number(data["mode"]) -> var newVal = 42
            if (obj[key] !== newVal) {
                obj[key] = newVal
                _updateStatus(obj, key)  //обновляем по ключам состояние двигателя
            }
        }
    })

    //oбновляем паспорт если открыт
    var passport = _passportStore[obj]
    if (passport && passport.visible) {
        passport.updateData({
                                running: obj.running,
                                block: obj.block,
                                fault: obj.fault,
                                mode: obj.mode,
                                state: obj.state,
                                ctlw: obj.ctlw,
                                diagnw: obj.diagnw,
                                blockw: obj.blockw,
                                diagnd: obj.diagnd,
                                blockd: obj.blockd,
                                load: obj.load,
                                worktime: obj.worktime,
                                timestamp: obj.timestamp,
                            })
    }
}

/* отображаем контествное меню */
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
            contextMenu.targetObject = obj
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
                _showPassport(obj, "PassportParamMTR.qml")
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

//преобразуем слово в биты и представляем в качестве строки для отладки
function wordsToBitsToString(value, bitsCount = 16) {
    let result = '';
    for (let i = bitsCount - 1; i >= 0; i--) {
        result += (value >> i) & 1;
        if (i % 4 === 0 && i !== 0) result += ' '; //группировка по 4 бита
    }
    return result;
}

function getBit(value, bitPosition) {
    return (value >> bitPosition) & 1; //сдвигаем биты числа вправо на bitPosition позиций & 1 - побитовое И с числом 1
}

/* расшифровку см.PsTechEE MTR - выходные пераметры */
function getMode(obj) {
    if (!obj) return "undefined"
    switch (obj.mode) {
    case 0: return "R" //Remote
    case 1: return "A" //Auto
    case 2: return "T" //Test
    case 3: return "R" //Repair
    case 4: return "L" //Local
    default: return "!"; // неизвестный режим - белый
    }
}

function getModeColor(obj) {
    if (!obj) return "transparent";
    switch (obj.mode) {
    case 0: return "#4A90D9"; // Remote - синий
    case 1: return "#3ab842"; // Auto - зелёный
    case 2: return "#ffA000"; // Test - оранжевый
    case 3: return "#ff0000"; // Repair - красный
    case 4: return "#9B59B6"; // Local - фиолетовый
    default: return "transparent"; // неизвестный режим - белый
    }
}

function getModeText(obj) {
    if (!obj) return "undefined"
    switch (obj.mode) {
    case 0: return "Remote - дистанционный"   //Remote
    case 1: return "Auto - автоматический"    //Auto
    case 2: return "Test - опробование"       //Test
    case 3: return "Repair - ремонтный"       //Repair
    case 4: return "Local - местный"          //Local
    default: return "mode: " + obj.mode; // неизвестный режим
    }
}

function getCtlwText(obj) {
    if (!obj) return "undefined"
    /*bit0*/    if (obj.ctlw & 0x01) return "Команда «Включить»"
    /*bit1*/    if (obj.ctlw & 0x02) return "Команда «Отключить»"
    return "unknown"
}

function getStateText(obj) {
    if (!obj) return "undefined"
    switch (obj.state) {
    case 0: return "unknown - неопределенное"
    case 1: return "Off - отключен"
    case 2: return "On - включен"
    case 3: return "Nowork - нерабочее"
    default: return "state: " + obj.state; // неизвестный режим
    }
}
