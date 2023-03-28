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
QMAKE_CXXFLAGS +=  -fpermissive -O2 

#make precompiled headers
PRECOMPILED_HEADER = source_code/Fpl_Headers_Precompiled.h

#include libs
#CONFIG(debug, debug|release) {
#     LIBS += C:/Users/Meiner/Documents/programmierung/promaides/debug/debug/libsystem_sys.a\
#        C:/Users/Meiner/Documents/programmierung/promaides/debug_hyd/debug/libhyd_system.a\
     
#} else {
#      LIBS +=  C:/Users/Meiner/Documents/programmierung/promaides/release/release/libsystem_sys.a\
#          C:/Users/Meiner/Documents/programmierung/promaides/release_hyd/release/libhyd_system.a\
# }
 
TEMPLATE = lib
TARGET = fpl_system

DEPENDPATH += . \
              source_code \
              source_code/calculation \
              source_code/gui \
              source_code/calculation/performance \
              source_code/calculation/random \
              source_code/calculation/section_fpl \
              source_code/calculation/section_fpl/geometrie_dike \
              source_code/calculation/section_fpl/mechanisms \
              source_code/calculation/section_fpl/mechanisms/dike \
              source_code/calculation/section_fpl/mechanisms/highground \
              source_code/calculation/section_fpl/mechanisms/wave_generation \
              source_code/calculation/section_fpl/mechanisms/slope_stability \
              source_code/calculation/section_fpl/mechanisms/landside_resistance \
              source_code/calculation/section_fpl/mechanisms/dune\
              source_code/calculation/section_fpl/mechanisms/gate\
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
              ../system_hydraulic/source_code/models/coast\
			  ../system_hydraulic/source_code/models/temperature\
			../../debug/\
                ../../debug_hyd/			  
              
INCLUDEPATH += . \
               source_code \
               source_code/calculation \
               source_code/gui \
               source_code/calculation/performance \
               source_code/calculation/random \
               source_code/calculation/section_fpl \
               source_code/calculation/section_fpl/geometrie_dike \
               source_code/calculation/section_fpl/mechanisms \
               source_code/calculation/section_fpl/mechanisms/dike \
               source_code/calculation/section_fpl/mechanisms/highground \
               source_code/calculation/section_fpl/mechanisms/wave_generation \
               source_code/calculation/section_fpl/mechanisms/slope_stability \
               source_code/calculation/section_fpl/mechanisms/landside_resistance \
               source_code/calculation/section_fpl/mechanisms/dune\
               source_code/calculation/section_fpl/mechanisms/gate\
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
               ../system_hydraulic/source_code/models/coast\
			   ../system_hydraulic/source_code/models/temperature\
			   ../../debug/\
                ../../debug_hyd/
			  
# Input


HEADERS += source_code/Fpl_Calculation.h \
           source_code/Fpl_Headers_Precompiled.h \
           source_code/calculation/performance/Fpl_Frc_Sim.h \
           source_code/calculation/performance/Fpl_Mc_Sim.h \
           source_code/calculation/random/_Fpl_Distribution_Class.h \
           source_code/calculation/random/Fpl_Distribution_Class_Discret.h \
           source_code/calculation/random/Fpl_Distribution_Class_Discret_Mean.h \
           source_code/calculation/random/Fpl_Distribution_Class_Mean.h \
           source_code/calculation/random/Fpl_Distribution_Class_Mean_Mean.h \
           source_code/calculation/random/Fpl_Distribution_Class_Triangle.h \
           source_code/calculation/random/Fpl_Random_Variables.h \
           source_code/calculation/random/_Fpl_Random_Generator.h \
           source_code/calculation/random/Fpl_Random_Generator_Ranmar.h \
           source_code/calculation/random/Fpl_Random_Generator_Zikurat.h \
           source_code/calculation/random/Fpl_Random_Generator_MofAll.h \
           source_code/calculation/random/Fpl_Random_Generator_Sfmt.h \
           source_code/calculation/random/Fpl_Check_Distribution.h \
           source_code/calculation/section_fpl/_Fpl_Section_Type.h \
           source_code/calculation/section_fpl/Fpl_Sec_Type_Dike.h \
           source_code/calculation/section_fpl/Fpl_Sec_Type_Highground.h \
           source_code/calculation/section_fpl/Fpl_Sec_Type_Dune.h \
           source_code/calculation/section_fpl/Fpl_Sec_Type_Gate.h \
           source_code/calculation/section_fpl/Fpl_Section.h \
           source_code/calculation/section_fpl/Fpl_Section_Points.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Segment_Waterside.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Waterside.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Crest.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Landside.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Segment_Landside.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Materialzone.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Materialzone_Connector.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Wet_Materialzone_Table_Point.h \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Wet_Materialzone_Table.h\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Var_Materialzone.h\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Hinterland.h\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Foreland.h\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Segment.h\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Waterside_Materialzone.h\
           source_code/calculation/section_fpl/mechanisms/_Fpl_Failure_Mechanism.h \
           source_code/calculation/section_fpl/mechanisms/_Fpl_Mechanism.h \
           source_code/calculation/section_fpl/mechanisms/Fpl_Seepage_Line_Point_List.h\
           source_code/calculation/section_fpl/mechanisms/Fpl_Seepage_Line_Point.h\
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Overflow_Dike.h \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waverunup_Dike.h \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Bursting_Din_Dike.h \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Lane_Dike.h \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Sell_Dike.h \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Schmertmann_Dike.h \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Bursting_Slice.h\
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Seepage_Calculator_Dike.h \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Wave_Impact_Waterside_Dike.h\
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waterside_Erosion.h\
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waterside_Erosion_Start.h\
           source_code/calculation/section_fpl/mechanisms/highground/Fpl_Fail_Overflow_HiGround.h \
           source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Local_Wind_Fetch.h \
           source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Mech_Wave_Generation.h \
           source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Waveparam_Calculation.h \
           source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Wind_Fetch.h \
           source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Mech_Slope_Stability_Dike.h \
           source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slip_Circle.h \
           source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slope_Slices.h \
           source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slope_External_Force.h\
           source_code/calculation/section_fpl/mechanisms/dune/Fpl_Mech_Erosion_Dune_van_Gent.h\
           source_code/calculation/section_fpl/mechanisms/dune/Fpl_Mech_Waverunup_Dune.h\
           source_code/calculation/section_fpl/mechanisms/landside_resistance/Fpl_Mech_Landside_Erosion.h\
		       source_code/calculation/section_fpl/Fpl_Frc_Curve.h \
           source_code/gui/FplGui_Section_Dia.h \
           source_code/gui/FplGui_Random_Check_Dia.h \
           source_code/gui/FplGui_Check_Disttype_Triangle_Dia.h \
           source_code/gui/FplGui_Check_Disttype_Mean_Dia.h \
           source_code/gui/FplGui_Prob_Result_Wid.h \
		   source_code/gui/FplGui_Detailed_Result_Wid.h \
           source_code/gui/FplGui_Frc_Values_Dia.h \
           source_code/gui/FplGui_System_Member_Wid.h \
		       source_code/gui/FplGui_Faulttree_Dike_Wid.h\
		       source_code/gui/FplGui_Faulttree_Dune_Wid.h\
		       source_code/gui/FplGui_Faulttree_Gate_Wid.h\

FORMS +=   source_code/gui/FplGui_Section_Dia.ui \
           source_code/gui/FplGui_Random_Check_Dia.ui \
           source_code/gui/FplGui_Check_Disttype_Triangle_Dia.ui \
           source_code/gui/FplGui_Check_Disttype_Mean_Dia.ui \
           source_code/gui/FplGui_Prob_Result_Wid.ui \
		   source_code/gui/FplGui_Detailed_Result_Wid.ui \
           source_code/gui/FplGui_Frc_Values_Dia.ui \
           source_code/gui/FplGui_System_Member_Wid.ui \
           source_code/gui/FplGui_Faulttree_Dike_Wid.ui\
		       source_code/gui/FplGui_Faulttree_Dune_Wid.ui\
		       source_code/gui/FplGui_Faulttree_Gate_Wid.ui\
 				
SOURCES += source_code/Fpl_Calculation.cpp \
           source_code/calculation/performance/Fpl_Frc_Sim.cpp \
           source_code/calculation/performance/Fpl_Mc_Sim.cpp \
           source_code/calculation/random/_Fpl_Distribution_Class.cpp \
           source_code/calculation/random/Fpl_Distribution_Class_Discret.cpp \
           source_code/calculation/random/Fpl_Distribution_Class_Discret_Mean.cpp \
           source_code/calculation/random/Fpl_Distribution_Class_Mean.cpp \
           source_code/calculation/random/Fpl_Distribution_Class_Mean_Mean.cpp \
           source_code/calculation/random/Fpl_Distribution_Class_Triangle.cpp \
           source_code/calculation/random/Fpl_Random_Variables.cpp \
           source_code/calculation/random/_Fpl_Random_Generator.cpp \
           source_code/calculation/random/Fpl_Random_Generator_Ranmar.cpp \
           source_code/calculation/random/Fpl_Random_Generator_Zikurat.cpp \
           source_code/calculation/random/Fpl_Random_Generator_MofAll.cpp \
           source_code/calculation/random/Fpl_Random_Generator_Sfmt.cpp \
           source_code/calculation/random/Fpl_Check_Distribution.cpp \
           source_code/calculation/section_fpl/_Fpl_Section_Type.cpp \
           source_code/calculation/section_fpl/Fpl_Sec_Type_Dike.cpp \
           source_code/calculation/section_fpl/Fpl_Sec_Type_Highground.cpp \
           source_code/calculation/section_fpl/Fpl_Sec_Type_Dune.cpp \
           source_code/calculation/section_fpl/Fpl_Sec_Type_Gate.cpp \
           source_code/calculation/section_fpl/Fpl_Section.cpp \
           source_code/calculation/section_fpl/Fpl_Section_Points.cpp \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Segment_Waterside.cpp \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Waterside.cpp \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Crest.cpp \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Landside.cpp \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Dike_Segment_Landside.cpp \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Materialzone.cpp \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Materialzone_Connector.cpp \
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Wet_Materialzone_Table_Point.cpp\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Wet_Materialzone_Table.cpp\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Var_Materialzone.cpp\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Hinterland.cpp\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Geo_Foreland.cpp\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Dike_Segment.cpp\
           source_code/calculation/section_fpl/geometrie_dike/Fpl_Cub_Waterside_Materialzone.cpp\
           source_code/calculation/section_fpl/mechanisms/_Fpl_Failure_Mechanism.cpp \
           source_code/calculation/section_fpl/mechanisms/_Fpl_Mechanism.cpp \
           source_code/calculation/section_fpl/mechanisms/Fpl_Seepage_Line_Point_List.cpp\
           source_code/calculation/section_fpl/mechanisms/Fpl_Seepage_Line_Point.cpp\
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Overflow_Dike.cpp \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waverunup_Dike.cpp \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Bursting_Din_Dike.cpp \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Lane_Dike.cpp \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Sell_Dike.cpp \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Piping_Schmertmann_Dike.cpp \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Bursting_Slice.cpp\
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Seepage_Calculator_Dike.cpp \
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Wave_Impact_Waterside_Dike.cpp\
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waterside_Erosion.cpp\
           source_code/calculation/section_fpl/mechanisms/dike/Fpl_Mech_Waterside_Erosion_Start.cpp\
           source_code/calculation/section_fpl/mechanisms/highground/Fpl_Fail_Overflow_HiGround.cpp \
           source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Local_Wind_Fetch.cpp \
           source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Mech_Wave_Generation.cpp \
           source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Waveparam_Calculation.cpp \
           source_code/calculation/section_fpl/mechanisms/wave_generation/Fpl_Wind_Fetch.cpp \
           source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Mech_Slope_Stability_Dike.cpp \
           source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slip_Circle.cpp \
           source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slope_Slices.cpp \
           source_code/calculation/section_fpl/mechanisms/slope_stability/Fpl_Slope_External_Force.cpp\
		       source_code/calculation/section_fpl/mechanisms/dune/Fpl_Mech_Erosion_Dune_van_Gent.cpp\
		       source_code/calculation/section_fpl/mechanisms/dune/Fpl_Mech_Waverunup_Dune.cpp\
		       source_code/calculation/section_fpl/mechanisms/landside_resistance/Fpl_Mech_Landside_Erosion.cpp\
		       source_code/calculation/section_fpl/Fpl_Frc_Curve.cpp \
           source_code/gui/FplGui_Section_Dia.cpp \
           source_code/gui/FplGui_Random_Check_Dia.cpp \
           source_code/gui/FplGui_Check_Disttype_Triangle_Dia.cpp \
           source_code/gui/FplGui_Check_Disttype_Mean_Dia.cpp \
		   source_code/gui/FplGui_Detailed_Result_Wid.cpp\
           source_code/gui/FplGui_Prob_Result_Wid.cpp \
           source_code/gui/FplGui_Frc_Values_Dia.cpp \
           source_code/gui/FplGui_System_Member_Wid.cpp \
		       source_code/gui/FplGui_Faulttree_Dike_Wid.cpp\
		       source_code/gui/FplGui_Faulttree_Dune_Wid.cpp\
		       source_code/gui/FplGui_Faulttree_Gate_Wid.cpp\

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

##win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../debug_hyd/release/ -lhyd_system
##else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../debug_hyd/debug/ -lhyd_system
##else:unix: LIBS += -L$$PWD/../../debug_hyd/ -lhyd_system

##INCLUDEPATH += $$PWD/../../debug_hyd/debug
##DEPENDPATH += $$PWD/../../debug_hyd/debug

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
