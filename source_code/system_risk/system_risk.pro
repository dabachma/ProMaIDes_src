QT += sql svg xml #fuer mysql
QT += printsupport
QT -= opengl
CONFIG += console #ausgabe auch auf console
CONFIG += qt
#CONFIG += qwt #fuer nutzung von qwt
include (C:/Qwt/qwt-6.1.0/qwt.prf)
CONFIG += thread
CONFIG += staticlib
CONFIG  += c++11
QMAKE_CXXFLAGS += -fpermissive -O2 

#make precompiled headers
PRECOMPILED_HEADER = source_code/Risk_Headers_Precompiled.h

#include libs
#CONFIG(debug, debug|release) {
#     LIBS += ../system_hydraulic/debug/system_hydraulic.lib \
#     				 ../system_fpl/debug/system_fpl.lib \
#     				 ../system_dam/debug/system_dam.lib \
#           	 ../system_sys/debug/system_sys.lib
# } else {
#      LIBS += ../system_hydraulic/release/system_hydraulic.lib \
#           		../system_fpl/release/system_fpl.lib \
#     				  ../system_dam/release/system_dam.lib \
#      			  ../system_sys/release/system_sys.lib
# }

 #include libs
#CONFIG(debug, debug|release) {
#     LIBS += C:/Users/Meiner/Documents/programmierung/promaides/debug/debug/libsystem_sys.a\
#        C:/Users/Meiner/Documents/programmierung/promaides/debug_hyd/debug/libhyd_system.a\
#                C:/Users/Meiner/Documents/programmierung/promaides/debug_fpl/debug/libfpl_system.a\
#                C:/Users/Meiner/Documents/programmierung/promaides/debug_dam/debug/libdam_system.a\
     
#} else {
#      LIBS +=  C:/Users/Meiner/Documents/programmierung/promaides/release/release/libsystem_sys.a\
#          C:/Users/Meiner/Documents/programmierung/promaides/release_hyd/release/libhyd_system.a\
#          C:/Users/Meiner/Documents/programmierung/promaides/release_fpl/release/libfpl_system.a\
#                C:/Users/Meiner/Documents/programmierung/promaides/release_dam/release/libdam_system.a\
# }
 
 
TEMPLATE = lib
TARGET = risk_system

DEPENDPATH += . \
              source_code \
              source_code/gui \
              ../system_fpl/source_code \
              ../system_fpl/source_code/calculation \
              ../system_fpl/source_code/gui \
              ../system_fpl/source_code/calculation/random \
              ../system_fpl/source_code/calculation/performance \
              ../system_fpl/source_code/calculation/section_fpl \
              ../system_fpl/source_code/calculation/section_fpl/geometrie_dike \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/dike \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/highground \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/wave_generation \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/slope_stability \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/landside_resistance \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/dune \ 
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/gate \
              ../system_fpl/source_code/gui/data \
              ../system_fpl \
              ../system_sys/source_code \
              ../system_sys/source_code/exceptions \
              ../system_sys/source_code/geometrie_general \
              ../system_sys/source_code/database \
              ../system_sys/source_code/gui \
              ../system_sys \
              ../system_hydraulic \
              ../system_hydraulic/source_code \
              ../system_hydraulic/source_code/general \
              ../system_hydraulic/source_code/models \
              ../system_hydraulic/source_code/solver \
              ../system_hydraulic/source_code/system \
              ../system_hydraulic/source_code/szenario \
              ../system_hydraulic/source_code/gui \
              ../system_hydraulic/source_code/models/coupling \
              ../system_hydraulic/source_code/models/floodplain \
              ../system_hydraulic/source_code/models/floodplain\elements \
              ../system_hydraulic/source_code/models/river \
              ../system_hydraulic/source_code/models/river\profile \
              ../system_hydraulic/source_code/models/coast \
			  ../system_hydraulic/source_code/models/temperature\
              ../system_dam \
              ../system_dam/source_code \
              ../system_dam/source_code/economic \
			  ../system_dam/source_code/CI \
              ../system_dam/source_code/general \
              ../system_dam/source_code/ecologic \
              ../system_dam/source_code/psycho_social \
              ../system_dam/source_code/people \
              ../system_dam/source_code/simple_count \
              ../system_dam/source_code/gui\
			  ../../debug/\
              ../../debug_hyd/	\
			  ../../debug_fpl/ \
			  ../../debug_dam/
              
INCLUDEPATH += . \
               source_code \
               source_code/gui \
               ../system_fpl/source_code \
               ../system_fpl/source_code/calculation \
               ../system_fpl/source_code/gui \
               ../system_fpl/source_code/calculation/random \
               ../system_fpl/source_code/calculation/performance \
               ../system_fpl/source_code/calculation/section_fpl \
               ../system_fpl/source_code/calculation/section_fpl/geometrie_dike \
               ../system_fpl/source_code/calculation/section_fpl/mechanisms \
               ../system_fpl/source_code/calculation/section_fpl/mechanisms/dike \
               ../system_fpl/source_code/calculation/section_fpl/mechanisms/highground \
               ../system_fpl/source_code/calculation/section_fpl/mechanisms/wave_generation \
               ../system_fpl/source_code/calculation/section_fpl/mechanisms/slope_stability \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/landside_resistance \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/dune \
              ../system_fpl/source_code/calculation/section_fpl/mechanisms/gate \
               ../system_fpl/source_code/gui/data \
               ../system_fpl \
               ../system_sys/source_code \
               ../system_sys/source_code/exceptions \
               ../system_sys/source_code/geometrie_general \
               ../system_sys/source_code/database \
               ../system_sys/source_code/gui \
               ../system_sys \
               ../system_hydraulic \
               ../system_hydraulic/source_code \
               ../system_hydraulic/source_code/general \
               ../system_hydraulic/source_code/models \
               ../system_hydraulic/source_code/solver \
               ../system_hydraulic/source_code/system \
               ../system_hydraulic/source_code/szenario \
               ../system_hydraulic/source_code/gui \
               ../system_hydraulic/source_code/models/coupling \
               ../system_hydraulic/source_code/models/floodplain \
               ../system_hydraulic/source_code/models/floodplain/elements \
               ../system_hydraulic/source_code/models/river \
               ../system_hydraulic/source_code/models/river/profile \
               ../system_hydraulic/source_code/models/coast \
			   ../system_hydraulic/source_code/models/temperature\
               ../system_dam \
               ../system_dam/source_code \
               ../system_dam/source_code/economic \
			   ../system_dam/source_code/CI \
               ../system_dam/source_code/general \
               ../system_dam/source_code/ecologic \
               ../system_dam/source_code/psycho_social \
               ../system_dam/source_code/people \
               ../system_dam/source_code/simple_count \
               ../system_dam/source_code/gui \
			   ../../debug/\
              ../../debug_hyd/	\
			  ../../debug_fpl/ \
			  ../../debug_dam/
              
# Input
HEADERS += source_code/Risk_System.h \
			source_code/Risk_Headers_Precompiled.h \
           source_code/Risk_Break_Scenario_Management.h \
           source_code/Risk_Break_Scenario.h \
           source_code/Risk_Hydraulic_Break_Calculation.h \
           source_code/Risk_Fpl_Stress.h \
           source_code/Risk_Dam_Results.h \
           source_code/Risk_Damage_Predictor.h \
           source_code/Risk_Catchment_Scenario_Tree.h \
           source_code/Risk_Catchment_Break_Scenario_List.h \
           source_code/gui/RiskGui_Result_Reliability_Dia.h \
           source_code/gui/RiskGui_Result_Cumulated_Wid.h \
           source_code/gui/RiskGui_Risk_Scenario_Dia.h \
           source_code/gui/RiskGui_Break_Height_Dia.h \
           source_code/gui/RiskGui_Catchment_Scenario_Dia.h \
           source_code/gui/RiskGui_Catchment_Scenario_Table_Wid.h \
           source_code/gui/RiskGui_Result_Wid.h \
           source_code/gui/RiskGui_Catchment_Scenario_Statistic_Wid.h \
           source_code/gui/RiskGui_Detailed_Result_Wid.h \
           source_code/gui/RiskGui_Detailed_Risk_Summary_Wid.h \
           source_code/gui/RiskGui_Result_Scenario_Wid.h \
           source_code/gui/RiskGui_Calculate_All_Dia.h \
           source_code/gui/RiskGui_Probabilty_Fpl_Section_Wid.h \
           source_code/gui/RiskGui_Optimisation_Prediction_Dia.h \
           source_code/gui/RiskGui_System_Member_Wid.h

FORMS += source_code/gui/RiskGui_Result_Cumulated_Wid.ui \
         source_code/gui/RiskGui_Risk_Scenario_Dia.ui \
         source_code/gui/RiskGui_Break_Height_Dia.ui \
         source_code/gui/RiskGui_Catchment_Scenario_Dia.ui \
         source_code/gui/RiskGui_Catchment_Scenario_Table_Wid.ui \
         source_code/gui/RiskGui_Result_Wid.ui \
         source_code/gui/RiskGui_Detailed_Result_Wid.ui \
         source_code/gui/RiskGui_Calculate_All_Dia.ui \
         source_code/gui/RiskGui_Optimisation_Prediction_Dia.ui \
         source_code/gui/RiskGui_System_Member_Wid.ui
 				
SOURCES += source_code/Risk_System.cpp \
           source_code/Risk_Break_Scenario_Management.cpp \
           source_code/Risk_Break_Scenario.cpp \
           source_code/Risk_Hydraulic_Break_Calculation.cpp \
           source_code/Risk_Damage_Predictor.cpp \             
           source_code/Risk_Catchment_Scenario_Tree.cpp \     
           source_code/Risk_Catchment_Break_Scenario_List.cpp \ 
           source_code/Risk_Fpl_Stress.cpp \
           source_code/Risk_Dam_Results.cpp \
           source_code/gui/RiskGui_Result_Reliability_Dia.cpp \
           source_code/gui/RiskGui_Result_Reliability_Dia.cpp \
           source_code/gui/RiskGui_Result_Cumulated_Wid.cpp \
           source_code/gui/RiskGui_Risk_Scenario_Dia.cpp \
           source_code/gui/RiskGui_Break_Height_Dia.cpp \
           source_code/gui/RiskGui_Catchment_Scenario_Dia.cpp \
           source_code/gui/RiskGui_Catchment_Scenario_Table_Wid.cpp \ 
           source_code/gui/RiskGui_Result_Wid.cpp \
           source_code/gui/RiskGui_Catchment_Scenario_Statistic_Wid.cpp \
           source_code/gui/RiskGui_Detailed_Result_Wid.cpp \
           source_code/gui/RiskGui_Detailed_Risk_Summary_Wid.cpp \
           source_code/gui/RiskGui_Result_Scenario_Wid.cpp \
           source_code/gui/RiskGui_Calculate_All_Dia.cpp \
           source_code/gui/RiskGui_Probabilty_Fpl_Section_Wid.cpp \
           source_code/gui/RiskGui_Optimisation_Prediction_Dia.cpp \
           source_code/gui/RiskGui_System_Member_Wid.cpp

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

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../debug_fpl/release/ -lfpl_system
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../debug_fpl/debug/ -lfpl_system
#else:unix: LIBS += -L$$PWD/../../debug_fpl/ -lfpl_system

#INCLUDEPATH += $$PWD/../../debug_fpl/debug
#DEPENDPATH += $$PWD/../../debug_fpl/debug

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_fpl/release/libfpl_system.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_fpl/debug/libfpl_system.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_fpl/release/fpl_system.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_fpl/debug/fpl_system.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../../debug_fpl/libfpl_system.a

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../debug_hyd/release/ -lhyd_system
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../debug_hyd/debug/ -lhyd_system
#else:unix: LIBS += -L$$PWD/../../debug_hyd/ -lhyd_system

#INCLUDEPATH += $$PWD/../../debug_hyd/debug
#DEPENDPATH += $$PWD/../../debug_hyd/debug

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_hyd/release/libhyd_system.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_hyd/debug/libhyd_system.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_hyd/release/hyd_system.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_hyd/debug/hyd_system.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../../debug_hyd/libhyd_system.a

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../debug_dam/release/ -ldam_system
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../debug_dam/debug/ -ldam_system
#else:unix: LIBS += -L$$PWD/../../debug_dam/ -ldam_system

#INCLUDEPATH += $$PWD/../../debug_dam/debug
#DEPENDPATH += $$PWD/../../debug_dam/debug

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_dam/release/libdam_system.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_dam/debug/libdam_system.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_dam/release/dam_system.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../debug_dam/debug/dam_system.lib
#else:unix: PRE_TARGETDEPS += $$PWD/../../debug_dam/libdam_system.a
