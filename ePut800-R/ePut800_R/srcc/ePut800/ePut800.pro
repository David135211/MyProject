######################################################################
# Automatically generated by qmake (2.01a) Wed Jun 28 01:16:01 2017
######################################################################

TEMPLATE = app
TARGET =
DEPENDPATH += . ComPort Dbase Element Keyboard Pages Proto
INCLUDEPATH += . Keyboard Proto ComPort Dbase Element Pages
LIBS += -ldl libsqlite3.so.0

# Input
HEADERS += BasePort.h \
           calarmdlg.h \
           cglobal.h \
           clogindlg.h \
           cmsg.h \
           cresetpwddlg.h \
           cykdlg.h \
           eput800lineedit.h \
           loadlibrary.h \
           profile.h \
           structure.h \
           Typedef.h \
           widget.h \
           ComPort/ccomport.h \
           ComPort/CTcpPortServer.h \
           ComPort/SerialPort.h \
           ComPort/TcpClient.h \
           ComPort/TcpListen.h \
           ComPort/TcpPort.h \
           Dbase/database.h \
           Dbase/dbasedatatype.h \
           Dbase/dbasemanager.h \
           Dbase/dbasemanager_soe.h \
           Dbase/sqlite3.h \
           Dbase/sqlite3dbase.h \
           Element/calarm_indicator_yx.h \
           Element/canalog_data_sign.h \
           Element/cbreaker.h \
           Element/ccircle_indicator_sign.h \
           Element/ccontactor.h \
           Element/cct_sign.h \
           Element/ceelment.h \
           Element/cgeneratrix.h \
           Element/cimage_yc.h \
           Element/clightning_protector.h \
           Element/clinker.h \
           Element/cload_breaker_2pos.h \
           Element/cload_breaker_3pos.h \
           Element/cpower.h \
           Element/cpt_sign.h \
           Element/crect_indicator_sign.h \
           Element/cswitch_2pos.h \
           Element/cswitch_3pos.h \
           Element/cswitch_connect_earth.h \
           Element/ctext_sign.h \
           Element/ctransformer.h \
           Keyboard/keyboard.h \
           Pages/cpage.h \
           Pages/cpage_dataquery.h \
           Pages/cpage_diagnostic.h \
           Pages/cpage_home.h \
           Pages/cpage_sysparameter.h \
           Pages/cpage_syspic.h \
           Proto/cmethod.h \
           Proto/cprotocol.h \
           Proto/publicmethod.h
FORMS += widget.ui
SOURCES += BasePort.cpp \
           calarmdlg.cpp \
           cglobal.cpp \
           clogindlg.cpp \
           cmsg.cpp \
           cresetpwddlg.cpp \
           cykdlg.cpp \
           eput800lineedit.cpp \
           loadlibrary.cpp \
           main.cpp \
           profile.cpp \
           widget.cpp \
           ComPort/ccomport.cpp \
           ComPort/CTcpPortServer.cpp \
           ComPort/SerialPort.cpp \
           ComPort/TcpClient.cpp \
           ComPort/TcpListen.cpp \
           ComPort/TcpPort.cpp \
           Dbase/database.cpp \
           Dbase/dbasemanager.cpp \
           Dbase/dbasemanager_soe.cpp \
           Dbase/sqlite3dbase.cpp \
           Element/calarm_indicator_yx.cpp \
           Element/canalog_data_sign.cpp \
           Element/cbreaker.cpp \
           Element/ccircle_indicator_sign.cpp \
           Element/ccontactor.cpp \
           Element/cct_sign.cpp \
           Element/ceelment.cpp \
           Element/cgeneratrix.cpp \
           Element/cimage_yc.cpp \
           Element/clightning_protector.cpp \
           Element/clinker.cpp \
           Element/cload_breaker_2pos.cpp \
           Element/cload_breaker_3pos.cpp \
           Element/cpower.cpp \
           Element/cpt_sign.cpp \
           Element/crect_indicator_sign.cpp \
           Element/cswitch_2pos.cpp \
           Element/cswitch_3pos.cpp \
           Element/cswitch_connect_earth.cpp \
           Element/ctext_sign.cpp \
           Element/ctransformer.cpp \
           Keyboard/keyboard.cpp \
           Pages/cpage.cpp \
           Pages/cpage_dataquery.cpp \
           Pages/cpage_diagnostic.cpp \
           Pages/cpage_home.cpp \
           Pages/cpage_sysparameter.cpp \
           Pages/cpage_syspic.cpp \
           Proto/cmethod.cpp \
           Proto/cprotocol.cpp \
           Proto/publicmethod.cpp
