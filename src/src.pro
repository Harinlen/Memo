# Copyright (C) Kreogist Dev Team
#
# You can redistribute this software and/or modify it under the
# terms of the HARERU Software License; either version 1 of the
# License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# license file for more details.

# Basic informations about the main mu project.
TEMPLATE = app
TARGET = memo

# Add Qt modules, the Qt major version should be greater than 6.
QT += \
    core \
    gui \
    widgets \
    xml \
    network \
    printsupport \
    core5compat

# Enabled C++ 17 configures.
CONFIG += c++17

# Add translation paths for Qt Linguist to generate translation files.
TRANSLATIONS += \
    i18n/Simplified_Chinese.ts \
    i18n/Traditional_Chinese.ts \
    i18n/Japanese.ts \
    i18n/French.ts \
    i18n/German.ts \
    i18n/Italian.ts \
    i18n/Korean.ts \
    i18n/Russian.ts \
    i18n/Spanish.ts \
    i18n/Turkish.ts \
    i18n/Dutch.ts \
    i18n/Urdu.ts

# Platform Specific Configuration.
win32: {
    # Application icon.
    RC_FILE += resource/resource_win32.rc \
               resource/memo.ico
    ICON = resource/memo.ico
    # Enable the backend and analysiser.
    # Add windows specific libs.
    LIBS += -luser32
    # Add Windows platform special extras.
    SOURCES += \
        sdk/knlockedfile_win.cpp
}

unix: {
    # Add UNIX series specific platform sources.
    SOURCES += sdk/knlockedfile_unix.cpp
}

# International Configurations.
i18n: {
    # MAKE_QM_FILES is a extra execution to called the lrelease to update the
    # qm file according to the original ts file real-time.
    MAKE_QM_FILES.input = TRANSLATIONS
    MAKE_QM_FILES.output = ${QMAKE_FILE_BASE}.qm
    MAKE_QM_FILES.commands = lrelease ${QMAKE_FILE_NAME} \
                                  -qm ../bin/${QMAKE_FILE_BASE}.qm
    MAKE_QM_FILES.CONFIG += no_link target_predeps
    # Add MAKE_QM_FILES to the extra compilers.
    QMAKE_EXTRA_COMPILERS += MAKE_QM_FILES
}

# Add sdk directory to include path.
INCLUDEPATH += \
    sdk \

RESOURCES += \
    resource/resource.qrc

HEADERS += \
    sdk/kncodecmenu.h \
    sdk/knconfigure.h \
    sdk/knconfiguremanager.h \
    sdk/kndocumentlayout.h \
    sdk/kneditmenu.h \
    sdk/knfilemanager.h \
    sdk/knfindwindow.h \
    sdk/knglobal.h \
    sdk/kngotowindow.h \
    sdk/knlocalpeer.h \
    sdk/knlockedfile.h \
    sdk/knmainwindow.h \
    sdk/knrecentfilerecorder.h \
    sdk/knsearchbar.h \
    sdk/knsearchmenu.h \
    sdk/knsingletonapplication.h \
    sdk/knstatusbar.h \
    sdk/knstatuslabel.h \
    sdk/knsyntaxhighlighter.h \
    sdk/kntabbar.h \
    sdk/kntextblockdata.h \
    sdk/kntexteditor.h \
    sdk/kntexteditorpanel.h \
    sdk/kntoolhash.h \
    sdk/kntoolhashfile.h \
    sdk/kntoolhashinput.h \
    sdk/kntoolmenu.h \
    sdk/knuimanager.h \
    sdk/knutil.h \
    sdk/knversion.h \
    sdk/knviewmenu.h

SOURCES += \
    main.cpp \
    sdk/kncodecmenu.cpp \
    sdk/knconfigure.cpp \
    sdk/knconfiguremanager.cpp \
    sdk/kndocumentlayout.cpp \
    sdk/kneditmenu.cpp \
    sdk/knfilemanager.cpp \
    sdk/knfindwindow.cpp \
    sdk/knglobal.cpp \
    sdk/kngotowindow.cpp \
    sdk/knlocalpeer.cpp \
    sdk/knlockedfile.cpp \
    sdk/knmainwindow.cpp \
    sdk/knrecentfilerecorder.cpp \
    sdk/knsearchbar.cpp \
    sdk/knsearchmenu.cpp \
    sdk/knsingletonapplication.cpp \
    sdk/knstatusbar.cpp \
    sdk/knstatuslabel.cpp \
    sdk/knsyntaxhighlighter.cpp \
    sdk/kntabbar.cpp \
    sdk/kntexteditor.cpp \
    sdk/kntexteditorpanel.cpp \
    sdk/kntoolhash.cpp \
    sdk/kntoolhashfile.cpp \
    sdk/kntoolhashinput.cpp \
    sdk/kntoolmenu.cpp \
    sdk/knuimanager.cpp \
    sdk/knutil.cpp \
    sdk/knviewmenu.cpp
