// connectionsLogic.js - скрипт для работы с логикой и обновления UI
.pragma library
.import "../components/paramLogicAI.js" as ParamLogicAI //импортировал для обновления сразу через JS
.import "../components/paramLogicMTR.js" as ParamLogicMTR

//хранилище компонентов
var _componentStore = {}  //AI, Motors, Pump, Valves, etc
var _pglobalPlot = null  //указатель на globalPlot
var _pappEng = null      //указатель на appEng

//преобразуем слово в биты и представляем в качестве строки для отладки
function _wordsToBitsToString(value, bitsCount = 16) {
    let result = '';
    for (let i = bitsCount - 1; i >= 0; i--) {
        result += (value >> i) & 1;
        if (i % 4 === 0 && i !== 0) result += ' '; //группировка по 4 бита
    }
    return result;
}

//Функция снятия визуализации на элементе (при удалении графиков)(визуализация в контекстном меню)
function _updateComponentChartIcon(obj, data) {
    if (!obj || !data) return
    if (data.symbol === obj.symbol) {
        obj.addTag = data.addTag
    }
}


//функция добавления графиков (Берем указатель на график и добавляем графики прямо тут)
function _addCharForPlot (data) {
    if (_pglobalPlot) {
        _pglobalPlot.fsetAddGraph(data)
    } else {
        console.error("CustomPlotSingleton is null!")
        return
    }
}

//функция инициализации с передачей контекста appEng
function initializeContext(appEng) {
    _pappEng = appEng
    _pglobalPlot = _pappEng.fpgetCustomPlot()
    console.log("connectionsLogic.js", "initializeContext _pappEng", _pappEng)
    if (!_pglobalPlot) return

    //сигнал добавления графика со стороны Widget (окно выбора графиков)
    _pglobalPlot.onFemitAddGraphW.connect((data) => {
                                              //поиск компонентов по symbol
                                              var components = _componentStore[data.symbol];
                                              if (!components || components.length === 0) {
                                                  console.warn("connectionsLogic.js", "No components found for symbol:", data.symbol);
                                                  return;
                                              }
                                              //берем tagname из самого элемента т.к. виджет с тегами не хранит данную информацию
                                              _addCharForPlot({
                                                                  addTag: data.addTag,
                                                                  symbol: components[0].symbol,
                                                                  name: components[0].name,
                                                                  unit: components[0].unit,
                                                                  description: components[0].description,
                                                                  tagname: components[0].tagname
                                                              })
                                              // console.log("connectionsLogic.js", "Found", components.length, "components for symbol:", data.symbol, components[0].tagname);
                                          });
    //сигнал состояния графика добавлен/удален для обновления компонентов *.qml
    _pglobalPlot.onFemitStateGgaph.connect((data) => {
                                               //поиск компонентов по symbol
                                               var components = _componentStore[data.symbol];
                                               if (!components || components.length === 0) {
                                                   console.warn("connectionsLogic.js", "No components found for symbol:", data.symbol);
                                                   return;
                                               }
                                               //обновляем ВСЕ компоненты с этим ID
                                               for (var i = 0; i < components.length; i++) {
                                                   _updateComponentChartIcon(components[i], data) //обновляем компоненты на прямую через JS а не через QML
                                               }
                                           });
}

//Функция регистрации компонента
function registerComponent(component) {
    if (!component && !component.symbol) return
    var componentId = component.symbol

    //если такого ID еще нет, создаем массив
    if (!_componentStore[componentId]) {
        _componentStore[componentId] = []
    }
    _componentStore[componentId].push(component) //добавляем компонент в массив
    // console.log("connectionsLogic.js", "Registered component:", component.symbol, component.name, "total:", _componentStore[componentId].length)

    //динамически подписываемся на onEmitAddToChart, если он существует
    if (component.onEmitAddToChart && typeof component.onEmitAddToChart.connect === 'function') {
        component.onEmitAddToChart.connect((data) => {
                                               _addCharForPlot(data)
                                               // console.log("connectionsLogic.js", "componentId", componentId, data.name, data.tagname, data.addTag);
                                           });
        //заполняем диалоговое окно выбора параметров на графиках
        if (_pglobalPlot) {
            _pglobalPlot.fsetFillParameters({
                                                //берем первый элемент из массива (их может быть несколько повторяющихся на UI)
                                                symbol: component.symbol,
                                                name: component.name,
                                                tagname: component.tagname
                                            })
        }
        // console.log("Subscribed to onEmitAddToChart for:", componentId);
    }
}

//функция обновления данных AI, TAG и тп
//перебираем весь массив _componentStore, берем все ключи массива, и по совпадающим ключам из структуры arg обновляем компоненты. Дольше!
function handleDataUpdateAI(arg) {
    if (!arg) return;
    //метеостанция
    // if (_componentStore.PT908) _componentStore.PT908[0].updateData(arg.PT908)
    // if (_componentStore.AT901) _componentStore.AT901[0].updateData(arg.AT901)
    // if (_componentStore.TT903) _componentStore.TT903[0].updateData(arg.TT903)

    /* Обращение в JavaScript к элементам:
     * - arg.PT908 - точечная нотация (dot notation)
     * - arg["PT908"] - скобочная нотация (bracket notation) - позволяет использовать переменные */

    var store = _componentStore;
    var keys = Object.keys(store);
    for (var i = 0; i < keys.length; i++) {
        var componentId = keys[i];
        var data = arg[componentId];
        if (data === null || data === undefined) continue;

        var components = store[componentId];
        for (var j = 0; j < components.length; j++) {
            ParamLogicAI.updateData(components[j], data) //обновляем компоненты на прямую через JS а не через QML
        }
    }
    //возвращаем состояния
    return {
        rotateOn: arg.ST50 && Number(arg.ST50.pv) >= 300,
        fireOn: arg.TE71 && Number(arg.TE71.pv) >= 170,
        aiKC1:  arg.TE521 && Number(arg.TE521.pv),
        aiKC2:  arg.TE522 && Number(arg.TE522.pv),
        aiKC3:  arg.TE523 && Number(arg.TE523.pv),
        aiKC4:  arg.TE524 && Number(arg.TE524.pv),
        aiKC5:  arg.TE525 && Number(arg.TE525.pv)
    }
}

//функция обновления Motor, Pump, Vent и тп
//ищем по полям в переданной структере arg, и обновляем соответствующие компоненты из массива _componentStore по этим Id. Быстрее!
function handleDataUpdateMTR(arg) {
    if (!arg) return;
    var store = _componentStore;
    for (var id in arg) {
        var data = arg[id];
        if (data === null || data === undefined) continue;

        var components = store[id];
        if (!components) continue;

        for (var j = 0; j < components.length; j++) {
            ParamLogicMTR.updateData(components[j], data);
        }
    }
}
