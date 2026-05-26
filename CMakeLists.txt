cmake_minimum_required(VERSION 3.16)

project(company-manager-qt VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
    src/database/databasemanager.h
    src/database/databasemanager.cpp
    src/models/employee.h
    src/models/department.h
    src/models/product.h
    src/models/category.h
    src/models/location.h
        src/models/stockmovement.h
    src/services/employeeservice.h
    src/services/employeeservice.cpp
    src/services/departmentservice.h
    src/services/departmentservice.cpp
    src/services/productservice.h
    src/services/productservice.cpp
    src/services/categoryservice.h
    src/services/categoryservice.cpp
    src/services/locationservice.h
    src/services/locationservice.cpp
    src/services/stockmovementservice.h
    src/services/stockmovementservice.cpp
    src/ui/employeedialog.h
    src/ui/employeedialog.cpp
    src/ui/employeedialog.ui
    src/ui/departmentdialog.h
    src/ui/departmentdialog.cpp
    src/ui/departmentdialog.ui
    src/ui/categorydialog.h
    src/ui/categorydialog.cpp
    src/ui/categorydialog.ui
    src/ui/productdialog.h
    src/ui/productdialog.cpp
    src/ui/productdialog.ui
    src/ui/locationpickerdialog.h
    src/ui/locationpickerdialog.cpp
    src/ui/locationmanagerdialog.h
    src/ui/locationmanagerdialog.cpp
    src/ui/movementcarddelegate.h
    src/ui/movementcarddelegate.cpp
    src/ui/productcombodelegate.h
    src/ui/productcombodelegate.cpp
    src/models/table/employeetablemodel.h
    src/models/table/employeetablemodel.cpp
    src/models/table/departmenttablemodel.h
    src/models/table/departmenttablemodel.cpp
    src/models/table/producttablemodel.h
    src/models/table/producttablemodel.cpp
    src/models/table/categorytablemodel.h
    src/models/table/categorytablemodel.cpp
    src/models/table/locationtreemodel.h
    src/models/table/locationtreemodel.cpp
    src/models/table/currentstockreportmodel.h
    src/models/table/currentstockreportmodel.cpp
    src/models/table/stockmovementslistmodel.h
    src/models/table/stockmovementslistmodel.cpp
    src/models/table/stockmovementlinesmodel.h
    src/models/table/stockmovementlinesmodel.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(company-manager-qt
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET company-manager-qt APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(company-manager-qt SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(company-manager-qt
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(
    company-manager-qt PRIVATE Qt${QT_VERSION_MAJOR}::Widgets
    PRIVATE Qt${QT_VERSION_MAJOR}::Sql
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.company-manager-qt)
endif()
set_target_properties(company-manager-qt PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS company-manager-qt
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(company-manager-qt)
endif()
