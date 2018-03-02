
find_package(Qt5Widgets)
find_package(Qt5Gui)

set (essynthgui_src 
    ESDesignerScene.cpp
    ESMainWindowUI.cpp
    ESModuleUI.cpp
    ESConnectionUI.cpp
)

set (essynthgui_hdr
    ESDesignerScene.h
    ESMainWindowUI.h
    ESModuleUI.h
    ESConnectionUI.h
)

set (essynthgui_ui
    ESMainWindowUI.ui
)

qt5_wrap_cpp(essynthgui_hdr_moc ${essynthgui_hdr})
qt5_wrap_ui(essynthgui_ui_moc ${essynthgui_ui})

add_library(essynthgui ${essynthgui_src} ${essynthgui_hdr_moc} ${essynthgui_ui_moc})
target_link_libraries(essynthgui pthread Qt5::Widgets Qt5::Gui)

