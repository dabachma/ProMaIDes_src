QT += sql svg xml #fuer mysql
QT += printsupport
CONFIG += console #ausgabe auch auf console
CONFIG += qt
QT -= opengl
#CONFIG += qwt #fuer nutzung von qwt
include (C:/Qwt/qwt-6.1.0/qwt.prf)
CONFIG += thread
CONFIG += staticlib
CONFIG  += c++11
QMAKE_CXXFLAGS += -fpermissive -O2 -fopenmp
QMAKE_LFLAGS +=  -fopenmp
LIBS += -fopenmp
QMAKE_CFLAGS_RELEASE += -fopenmp

#make precompiled headers
PRECOMPILED_HEADER = source_code/Hydrol_Headers_Precompiled.h

#include libs
#CONFIG(debug, debug|release) {
#     LIBS += C:/Users/Meiner/Documents/programmierung/promaides/debug_solv/debug/libsolv.a\
#             C:/Users/Meiner/Documents/programmierung/promaides/debug/debug/libsystem_sys.a\

     
#} else {
 #     LIBS += C:/Users/Meiner/Documents/programmierung/promaides/release_solv/release/libsolv.a\
 #            C:/Users/Meiner/Documents/programmierung/promaides/release/release/libsystem_sys.a\
 #}

TEMPLATE = lib
TARGET = hydrol_system
DEPENDPATH += . \
			source_code \
              source_code/models \
              ../system_sys/source_code \
              ../system_sys/source_code/exceptions \
              ../system_sys/source_code/geometrie_general \
              ../system_sys/source_code/database \
              ../system_sys/source_code/gui \
              ../system_sys\
			 ../../debug\
             
INCLUDEPATH += . \
				source_code \
               source_code/models \
               ../system_sys/source_code \
               ../system_sys/source_code/exceptions \
               ../system_sys/source_code/geometrie_general \
               ../system_sys/source_code/database \
               ../system_sys/source_code/gui \
               ../system_sys\
			   ../../debug\

# Input
HEADERS +=  source_code/Hydrol_Multiple_Hydrologic_Systems.h\
            source_code/Hydrol_Headers_Precompiled.h\
			source_code/models/Hydrol_Hydrologic_Systems.h \
           
                     
#FORMS+=    source_code/gui/HydGui_System_Filechooser_Dia.ui \
				   
		
           
SOURCES += source_code/Hydrol_Multiple_Hydrologic_Systems.cpp\
			source_code/models/Hydrol_Hydrologic_Systems.cpp\
					

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../debug_solv/release/ -lsolver
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../debug_solv/debug/ -lsolver
#else:unix: LIBS += -L$$PWD/../../debug_solv/ -lsolver

#INCLUDEPATH += $$PWD/../../debug_solv/debug
#DEPENDPATH += $$PWD/../../debug_solv/debug

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_solv/release/libsolver.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_solv/debug/libsolver.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_solv/release/solver.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_solv/debug/solver.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../../debug_solv/libsolver.a



#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../debug/release/ -lsystem_sys
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../debug/debug/ -lsystem_sys
#else:unix: LIBS += -L$$PWD/../../debug/ -lsystem_sys

#INCLUDEPATH += $$PWD/../../debug/debug
#DEPENDPATH += $$PWD/../../debug/debug

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug/release/libsystem_sys.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug/debug/libsystem_sys.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug/release/system_sys.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug/debug/system_sys.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../../debug/libsystem_sys.a
