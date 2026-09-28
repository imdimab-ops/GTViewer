cmake_minimum_required(VERSION 3.16)

project(GTViewer VERSION 0.1 LANGUAGES CXX)

#---------------Добавляем версию проекта и номер сборки#---------------
set(SOURCE_NUMBER_FILE "${CMAKE_SOURCE_DIR}/version/build_number.txt")
if(EXISTS ${SOURCE_NUMBER_FILE})
    file(READ ${SOURCE_NUMBER_FILE} BUILD_NUMBER)
    math(EXPR BUILD_NUMBER "${BUILD_NUMBER} + 1")
else()
    set(BUILD_NUMBER 1)
endif()
file(WRITE ${SOURCE_NUMBER_FILE} "${BUILD_NUMBER}")

# Установка версии проекта
set(PROJECT_VERSION "${PROJECT_VERSION}")
set(PROJECT_VERSION_BUILD "${PROJECT_VERSION}.${BUILD_NUMBER}")
set(PROJECT_VERSION_FULL "${PROJECT_NAME}.${PROJECT_VERSION}.${BUILD_NUMBER}")
# Создаем конфигурационный файл версии
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version/version.in.h
    ${CMAKE_CURRENT_SOURCE_DIR}/version/version.h
)
#----------------------------------------------------------------------

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# Просто создаем исполняемый файл
add_executable(GTViewer ${PROJECT_SOURCES}
    systemlog/uisystemlog.h systemlog/uisystemlog.cpp systemlog/uisystemlog.ui
    systemlog/modelsystemlog.h systemlog/modelsystemlog.cpp

    tcpclient/tcpthread.h tcpclient/tcpthread.cpp
    tcpclient/tcpclient.h tcpclient/tcpclient.cpp
    tcpclient/processdata.h tcpclient/processdata.cpp
    tcpclient/parsingdata.h tcpclient/parsingdata.cpp
    tcpclient/uitcpclient.h tcpclient/uitcpclient.cpp tcpclient/uitcpclient.ui

    events/message.h events/message.cpp
    events/modelevent.h events/modelevent.cpp
    events/proxyeventmodel.h events/proxyeventmodel.cpp
    customplot/millisecondticker.h
    customplot/customplotwidget.h customplot/customplotwidget.cpp
    customplot/fieldselectordialog.h customplot/fieldselectordialog.cpp
    customplot/qcustomplot.h customplot/qcustomplot.cpp
    uistatistics.h uistatistics.cpp uistatistics.ui
    # engine.h engine.cpp
    # dialog.h dialog.cpp
    applib.h applib.cpp
    fileio.h fileio.cpp
)

# Добавляем ресурсы (если используете .qrc)
qt_add_resources(GTViewer Resources
    PREFIX "/"
    FILES
    "qml/Main.qml"
    "qml/GTU.qml"

    "qml/components/AI.qml"
    "qml/components/AIpbv.qml"
    "qml/components/ContextMenu.qml"
    "qml/components/PassportParam.qml"
    "qml/components/img/chartOn.svg"
    "qml/components/img/chartOff.svg"
    "qml/components/img/rotate_left.svg"
    "qml/components/img/rotate_right.svg"
    "qml/components/img/passport.svg"

    "qml/img/gtu/base.svg"
    "qml/img/gtu/cam1.svg"
    "qml/img/gtu/cam2.svg"
    "qml/img/gtu/cam3.svg"
    "qml/img/gtu/cam4.svg"
    "qml/img/gtu/fire.gif"
    "qml/img/gtu/firecam.gif"
    "qml/img/gtu/rotate.gif"
    "qml/img/gtu/turboFireEx.svg"

    "qml/js/connectionsLogic.js"
    "qml/components/paramLogic.js"

    "img/logo_slow.gif"
    "img/logo.ico"
    "img/icon/mainmenu/close.svg"
    "img/icon/mainmenu/openfolder.svg"
    "img/icon/mainmenu/systemlog.svg"
    "img/icon/mainmenu/statistics.svg"
    "img/icon/mainmenu/server.svg"
    "img/icon/mainmenu/fullscreen.svg"
    "img/icon/mainmenu/about.svg"
    "img/icon/mainmenu/aboutqt.svg"
    "img/icon/plugconnected.svg"
    "img/icon/plugdisconnected.svg"
    "img/icon/clean.svg"
    "img/icon/success.svg"
    "img/icon/successErr.svg"
    "img/icon/check_single.svg"
    "img/icon/check_double.svg"
)

# Иконка приложения
if(WIN32)
    # Создаем файл ресурсов с иконкой
    set(RC_FILE "${CMAKE_CURRENT_BINARY_DIR}/app.rc")
    file(WRITE ${RC_FILE} "IDI_ICON1 ICON \"${CMAKE_CURRENT_SOURCE_DIR}/img/logo.ico\"\n")
    # Добавляем RC файл к исполняемому файлу
    target_sources(GTViewer PRIVATE ${RC_FILE})
    # Указываем CMake, что это файл ресурсов
    set_source_files_properties(${RC_FILE} PROPERTIES LANGUAGE RC)
endif()

find_package(Qt6 REQUIRED COMPONENTS
    Core
    Widgets
    QuickWidgets
    OpenGL
    OpenGLWidgets
    PrintSupport
    Network
)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
# Найти системный OpenGL
find_package(OpenGL REQUIRED)

target_link_libraries(GTViewer PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::QuickWidgets
    Qt${QT_VERSION_MAJOR}::OpenGL          # ВАЖНО: добавляем OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets   # И OpenGLWidgets
    Qt${QT_VERSION_MAJOR}::PrintSupport    # Для работы QCustomPlot
    Qt${QT_VERSION_MAJOR}::Network
    ${OPENGL_gl_LIBRARY}        # Системная OpenGL
    opengl32                    # Явно для Windows
    glu32                       # Опционально, для GLU функций
)
target_compile_definitions(GTViewer PRIVATE QCUSTOMPLOT_USE_OPENGL)

# Установка HighDPI атрибутов
target_compile_definitions(GTViewer PRIVATE QT_ENABLE_HIGHDPI_SCALING=1)

# Устанавливаем специальный флаг компиляции ТОЛЬКО для файла qcustomplot.cpp (иначе при сборке файл слишком большой и не собирается)
set_source_files_properties(customplot/qcustomplot.cpp PROPERTIES COMPILE_FLAGS "-Wa,-mbig-obj")

# 1.Найти OpenSSL
set(OPENSSL_ROOT_DIR "C:/Qt/Tools/OpenSSLv4/usr/local")
find_package(OpenSSL REQUIRED)
# копируем libcrypto-4-x64.dll, libssl-4-x64.dll в дирректорию с исполняемым файлом
add_custom_command(TARGET GTViewer POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${OPENSSL_ROOT_DIR}/bin"
    "$<TARGET_FILE_DIR:GTViewer>"
    COMMENT "Copying OpenSSL DLLs to build directory"
)
# 2.Добавляем OpenSSL
target_link_libraries(GTViewer PRIVATE
    OpenSSL::SSL
    OpenSSL::Crypto
)
target_link_libraries(GTViewer PRIVATE Qt6::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
    set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.GTViewer)
endif()
set_target_properties(GTViewer PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS GTViewer
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(GTViewer)
endif()
