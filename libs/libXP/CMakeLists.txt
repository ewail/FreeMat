INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} ${QT_QTCORE_INCLUDE_DIR} ${QT_QTGUI_INCLUDE_DIR} )


ADD_LIBRARY( XP 
DynLib.cpp 
KeyManager.cpp 
PathSearch.cpp 
QTTerm.cpp 
System.cpp 
Editor.cpp 
FMFontDialog.cpp
PathTool.cpp 
highlighter.cpp 
FileTool.cpp 
Common.cpp 
ToolDock.cpp 
VariablesTool.cpp 
HistoryWidget.cpp 
DebugWin.cpp
DataTable.cpp
StackTool.cpp
${CMAKE_CURRENT_BINARY_DIR}/fontform.ui.h
${CMAKE_CURRENT_BINARY_DIR}/findform.ui.h
${CMAKE_CURRENT_BINARY_DIR}/replaceform.ui.h 
${CMAKE_CURRENT_BINARY_DIR}/synlightconf.ui.h 
${CMAKE_CURRENT_BINARY_DIR}/indentconf.ui.h 
${CMAKE_CURRENT_BINARY_DIR}/KeyManager.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/QTTerm.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/Editor.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/FMFontDialog.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/PathTool.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/highlighter.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/VariablesTool.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/FileTool.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/HistoryWidget.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/ToolDock.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/DebugWin.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/DataTable.moc.cpp 
${CMAKE_CURRENT_BINARY_DIR}/StackTool.moc.cpp
)

QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/KeyManager.hpp ${CMAKE_CURRENT_BINARY_DIR}/KeyManager.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/QTTerm.hpp ${CMAKE_CURRENT_BINARY_DIR}/QTTerm.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/Editor.hpp ${CMAKE_CURRENT_BINARY_DIR}/Editor.moc.cpp ) 
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/FMFontDialog.hpp ${CMAKE_CURRENT_BINARY_DIR}/FMFontDialog.moc.cpp ) 
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/PathTool.hpp ${CMAKE_CURRENT_BINARY_DIR}/PathTool.moc.cpp )  
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/highlighter.hpp ${CMAKE_CURRENT_BINARY_DIR}/highlighter.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/VariablesTool.hpp ${CMAKE_CURRENT_BINARY_DIR}/VariablesTool.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/FileTool.hpp ${CMAKE_CURRENT_BINARY_DIR}/FileTool.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/HistoryWidget.hpp ${CMAKE_CURRENT_BINARY_DIR}/HistoryWidget.moc.cpp )  
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/ToolDock.hpp ${CMAKE_CURRENT_BINARY_DIR}/ToolDock.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/VariablesTool.hpp ${CMAKE_CURRENT_BINARY_DIR}/VariablesTool.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/DebugWin.hpp ${CMAKE_CURRENT_BINARY_DIR}/DebugWin.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/DataTable.hpp ${CMAKE_CURRENT_BINARY_DIR}/DataTable.moc.cpp )
QT4_GENERATE_MOC( ${CMAKE_CURRENT_SOURCE_DIR}/StackTool.hpp ${CMAKE_CURRENT_BINARY_DIR}/StackTool.moc.cpp )

QT4_GENERATE_UI( ${CMAKE_CURRENT_SOURCE_DIR}/findform.ui ${CMAKE_CURRENT_BINARY_DIR}/findform.ui.h )
QT4_GENERATE_UI( ${CMAKE_CURRENT_SOURCE_DIR}/replaceform.ui ${CMAKE_CURRENT_BINARY_DIR}/replaceform.ui.h )
QT4_GENERATE_UI( ${CMAKE_CURRENT_SOURCE_DIR}/synlightconf.ui ${CMAKE_CURRENT_BINARY_DIR}/synlightconf.ui.h )  
QT4_GENERATE_UI( ${CMAKE_CURRENT_SOURCE_DIR}/indentconf.ui ${CMAKE_CURRENT_BINARY_DIR}/indentconf.ui.h )
QT4_GENERATE_UI( ${CMAKE_CURRENT_SOURCE_DIR}/fontform.ui ${CMAKE_CURRENT_BINARY_DIR}/fontform.ui.h )
