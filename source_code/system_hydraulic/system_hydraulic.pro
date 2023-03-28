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
PRECOMPILED_HEADER = source_code/Hyd_Headers_Precompiled.h

#include libs
#CONFIG(debug, debug|release) {
#     LIBS += C:/Users/Meiner/Documents/programmierung/promaides/debug_solv/debug/libsolv.a\
#             C:/Users/Meiner/Documents/programmierung/promaides/debug/debug/libsystem_sys.a\

     
#} else {
 #     LIBS += C:/Users/Meiner/Documents/programmierung/promaides/release_solv/release/libsolv.a\
 #            C:/Users/Meiner/Documents/programmierung/promaides/release/release/libsystem_sys.a\
 #}

TEMPLATE = lib
TARGET = hyd_system
DEPENDPATH += . \
							source_code \
              source_code/general \
              source_code/models \
              source_code/solver \
              source_code/system \
              source_code/szenario \
              source_code/gui \
              source_code/models/coupling \
              source_code/models/floodplain \
			  source_code/models/groundwater \
              source_code/models/floodplain/elements \
              source_code/models/river \
              source_code/models/river/profile \
              source_code/models/coast \
			  source_code/models/temperature \
              ../system_sys/source_code \
              ../system_sys/source_code/exceptions \
              ../system_sys/source_code/geometrie_general \
              ../system_sys/source_code/database \
              ../system_sys/source_code/gui \
              ../system_sys\
			 ../../debug/
             
INCLUDEPATH += . \
							 source_code \
               source_code/general \
               source_code/models \
               source_code/solver \
               source_code/system \
               source_code/szenario \
               source_code/gui \
               source_code/models/coupling \
               source_code/models/floodplain \
			   source_code/models/groundwater \
               source_code/models/floodplain/elements \
               source_code/models/river \
               source_code/models/river/profile \
               source_code/models/coast \
			   source_code/models/temperature \
               ../system_sys/source_code \
               ../system_sys/source_code/exceptions \
               ../system_sys/source_code/geometrie_general \
               ../system_sys/source_code/database \
               ../system_sys/source_code/gui \
               ../system_sys\
			   ../../debug/

# Input
HEADERS += source_code/Hyd_Multiple_Hydraulic_Systems.h \
            source_code/Hyd_Headers_Precompiled.h\
           source_code/general/_Hyd_Parse_IO.h \
           source_code/models/_Hyd_Model.h \
           source_code/models/Hyd_Instationary_Boundary.h \
           source_code/system/Hyd_Hydraulic_System.h \
           source_code/system/Hyd_Param_Global.h \
           source_code/system/Hyd_Param_Material.h \
           source_code/system/Hyd_Parse_Glob.h \
           source_code/system/Hyd_Observation_Point.h\
           source_code/system/Hyd_Observation_Point_Manager.h\
           source_code/models/floodplain/Hyd_Model_Floodplain.h \
           source_code/models/floodplain/Hyd_Param_FP.h \
           source_code/models/floodplain/Hyd_Parse_FP.h \
           source_code/models/floodplain/Hyd_Floodplain_Polygon.h \
           source_code/models/floodplain/Hyd_Floodplain_Polysegment.h \
           source_code/models/floodplain/Hyd_Floodplain_Raster.h \
           source_code/models/floodplain/Hyd_Gis2Promraster_Converter.h\
           source_code/models/floodplain/Hyd_Profil2Dgm_Converter.h\
           source_code/models/floodplain/Hyd_Floodplain_Dikeline_Point_List.h\
           source_code/models/floodplain/Hyd_Floodplain_Dikeline_Point.h\
           source_code/models/floodplain/Hyd_Element_Coup_Members.h\
           source_code/models/floodplain/Hyd_Element_Bound_Members.h\
           source_code/models/floodplain/elements/Hyd_Element_Floodplain.h \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_Coast.h \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_River.h \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_Standard.h \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_Noflow.h \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_Dam.h\
           source_code/models/floodplain/elements/_Hyd_Element_Floodplain_Type.h \
           source_code/models/floodplain/elements/Hyd_Floodplainraster_Polygon.h \
           source_code/models/floodplain/elements/Hyd_Floodplainraster_Segment.h \
           source_code/models/floodplain/elements/Hyd_Floodplainraster_Point.h \
           source_code/models/floodplain/elements/Hyd_Floodplainraster_Point_List.h \
           source_code/models/river/Hyd_Model_River.h \
           source_code/models/river/Hyd_Param_RV.h \
           source_code/models/river/Hyd_Parse_RV.h \
           source_code/models/river/profile/_Hyd_River_Profile_Type.h \
           source_code/models/river/profile/Hyd_River_Profile_Type_Standard.h \ 
           source_code/models/river/profile/Hyd_River_Profile_Type_Weir.h \
           source_code/models/river/profile/Hyd_River_Profile_Type_Bridge.h \
           source_code/models/river/profile/Hyd_River_Profile_Connection_Inflow.h \
           source_code/models/river/profile/Hyd_River_Profile_Connection_Outflow.h \
           source_code/models/river/profile/Hyd_River_Profile_Connection_Standard.h \
           source_code/models/river/profile/_Hyd_River_Profile.h \
           source_code/models/river/profile/Hyd_Tables.h \
           source_code/models/river/profile/Hyd_Profile_Segment.h \
           source_code/models/river/profile/Hyd_River_Profile_Point.h \
           source_code/models/coast\Hyd_Coast_Model.h \
           source_code/models/coast\Hyd_Parse_CO.h \
		   source_code/models/temperature\HydTemp_Parse.h\
		   source_code/models/temperature\HydTemp_Param.h\
		   source_code/models/temperature\HydTemp_Model.h\  
		   source_code/models/temperature\HydTemp_Profile.h\ 
           source_code/models/coupling/Hyd_Coupling_Model_List.h \
           source_code/models/coupling/Hyd_Coupling_Management.h \
           source_code/models/coupling/Hyd_Coupling_RV2CO.h \
           source_code/models/coupling/Hyd_Coupling_FP2FP.h \
           source_code/models/coupling/Hyd_Coupling_RV2FP.h \
           source_code/models/coupling/Hyd_Coupling_RV2FP_Merged.h \
           source_code/models/coupling/Hyd_Coupling_FP2CO.h \
           source_code/models/coupling/Hyd_Coupling_FP2CO_Merged.h \
           source_code/models/coupling/Hyd_Coupling_RV2RV.h \
           source_code/models/coupling/Hyd_Coupling_RV2RV_Diversion.h \
           source_code/models/coupling/Hyd_Coupling_RV2FP_Structure.h \
           source_code/models/coupling/Hyd_Coupling_RV2FP_Dikebreak.h \
           source_code/models/coupling/_Hyd_Coupling_Point.h \
           source_code/models/coupling/Hyd_Coupling_Point_FP2CO.h \
           source_code/models/coupling/Hyd_Coupling_Point_FP2FP.h \
           source_code/models/coupling/Hyd_Coupling_Point_RV2FP.h \
           source_code/models/coupling/_Hyd_Coupling_Point_List.h \
           source_code/models/coupling/Hyd_Coupling_Point_FP2CO_List.h \
           source_code/models/coupling/Hyd_Coupling_Point_FP2FP_List.h \
           source_code/models/coupling/Hyd_Coupling_Point_RV2FP_List.h \
           source_code/models/coupling/_Hyd_Coupling_Structure.h \
           source_code/models/coupling/Hyd_Coupling_Structure_Weir.h \
           source_code/models/coupling/Hyd_Coupling_Structure_Gate.h \
           source_code/models/coupling/Hyd_Parse_Coupling_Diversion.h \
           source_code/models/coupling/Hyd_Parse_Coupling_RV2FP_Structure.h \
           source_code/models/coupling/Hyd_Parse_Coupling_RV2FP_Dikebreak.h \
           source_code/szenario/Hyd_Boundary_Szenario.h \
           source_code/szenario/Hyd_Boundary_Szenario_Management.h \
           source_code/szenario/Hyd_Boundary_Scenario_List.h \
           source_code/gui/HydGui_Profile_Plot.h \
           source_code/gui/HydGui_System_Filechooser_Dia.h \
           source_code/gui/HydGui_Boundary_Scenario_Dia.h \
           source_code/gui/HydGui_Floodplain_Member_Wid.h \
           source_code/gui/HydGui_River_Member_Wid.h \
           source_code/gui/HydGui_Rv_Profile_General_Wid.h \
           source_code/gui/HydGui_System_Member_Wid.h \
           source_code/gui/HydGui_Bound_Scenario_Table_Wid.h \
           source_code/gui/HydGui_Bound_Scenario_Data_Dia.h \
           source_code/gui/HydGui_System_State_Dia.h \
           source_code/gui/HydGui_Coupling_Structure_Wid.h \
           source_code/gui/HydGui_Diversion_Channel_Wid.h \
           source_code/gui/HydGui_River_Geometrics_Table.h \
           source_code/gui/HydGui_Rv_Profile_Boundary_Lateral_Params_Wid.h \
           source_code/gui/HydGui_Rv_Profile_Boundary_Point_Params_Wid.h \
           source_code/gui/HydGui_Rv_Profile_Break_Params_Wid.h \
           source_code/gui/HydGui_Rv_Profile_Overflow_Params_Wid.h\
           source_code/gui/HydGui_Profil2Dgm_Converter_Dia.h\
           source_code/models/coupling/Hyd_Coupling_FP2CO_Dikebreak.h\
           source_code/models/coupling/_Hyd_Coupling_Dikebreak.h\
           source_code/models/coupling/Hyd_Parse_Coupling_FP2CO_Dikebreak.h\
		   source_code/models/groundwater\Hyd_Model_Groundwater.h
                     
FORMS+=    source_code/gui/HydGui_System_Filechooser_Dia.ui \
				   source_code/gui/HydGui_Boundary_Scenario_Dia.ui \
				   source_code/gui/HydGui_Floodplain_Member_Wid.ui \
           source_code/gui/HydGui_River_Member_Wid.ui \
           source_code/gui/HydGui_Rv_Profile_General_Wid.ui \
           source_code/gui/HydGui_System_Member_Wid.ui \
           source_code/gui/HydGui_Bound_Scenario_Data_Dia.ui \
           source_code/gui/HydGui_System_State_Dia.ui \
           source_code/gui/HydGui_Coupling_Structure_Wid.ui \
           source_code/gui/HydGui_Diversion_Channel_Wid.ui \
           source_code/gui/HydGui_Rv_Profile_Boundary_Lateral_Params_Wid.ui \
           source_code/gui/HydGui_Rv_Profile_Boundary_Point_Params_Wid.ui \
           source_code/gui/HydGui_Rv_Profile_Break_Params_Wid.ui \
           source_code/gui/HydGui_Rv_Profile_Overflow_Params_Wid.ui\
           source_code/gui/HydGui_Profil2Dgm_Converter_Dia.ui\
           
SOURCES += source_code/Hyd_Multiple_Hydraulic_Systems.cpp \
					 source_code/general/_Hyd_Parse_IO.cpp \
           source_code/models/_Hyd_Model.cpp \
           source_code/models/Hyd_Instationary_Boundary.cpp \
           source_code/system/Hyd_Hydraulic_System.cpp \
           source_code/system/Hyd_Param_Global.cpp \
           source_code/system/Hyd_Param_Material.cpp \
           source_code/system/Hyd_Parse_Glob.cpp \
           source_code/system/Hyd_Observation_Point.cpp\
           source_code/system/Hyd_Observation_Point_Manager.cpp\
           source_code/models/floodplain/Hyd_Model_Floodplain.cpp \
           source_code/models/floodplain/Hyd_Param_FP.cpp \
           source_code/models/floodplain/Hyd_Parse_FP.cpp \
           source_code/models/floodplain/Hyd_Floodplain_Polygon.cpp \
           source_code/models/floodplain/Hyd_Floodplain_Polysegment.cpp \
           source_code/models/floodplain/Hyd_Floodplain_Raster.cpp \
           source_code/models/floodplain/Hyd_Gis2Promraster_Converter.cpp\
           source_code/models/floodplain/Hyd_Profil2Dgm_Converter.cpp\
           source_code/models/floodplain/Hyd_Floodplain_Dikeline_Point_List.cpp\
           source_code/models/floodplain/Hyd_Floodplain_Dikeline_Point.cpp\
           source_code/models/floodplain/Hyd_Element_Coup_Members.cpp\
           source_code/models/floodplain/Hyd_Element_Bound_Members.cpp\
           source_code/models/floodplain/elements/Hyd_Element_Floodplain.cpp \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_Coast.cpp \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_River.cpp \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_Standard.cpp \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_Noflow.cpp \
           source_code/models/floodplain/elements/Hyd_Element_Floodplain_Type_Dam.cpp\
           source_code/models/floodplain/elements/_Hyd_Element_Floodplain_Type.cpp \
           source_code/models/floodplain/elements/Hyd_Floodplainraster_Polygon.cpp \
           source_code/models/floodplain/elements/Hyd_Floodplainraster_Segment.cpp \
           source_code/models/floodplain/elements/Hyd_Floodplainraster_Point.cpp \
           source_code/models/floodplain/elements/Hyd_Floodplainraster_Point_List.cpp \
           source_code/models/river/Hyd_Model_River.cpp \
           source_code/models/river/Hyd_Param_RV.cpp \
           source_code/models/river/Hyd_Parse_RV.cpp \
           source_code/models/river/profile/_Hyd_River_Profile_Type.cpp \
           source_code/models/river/profile/Hyd_River_Profile_Type_Standard.cpp \
           source_code/models/river/profile/Hyd_River_Profile_Type_Weir.cpp \
           source_code/models/river/profile/Hyd_River_Profile_Type_Bridge.cpp \
           source_code/models/river/profile/Hyd_River_Profile_Connection_Inflow.cpp \
           source_code/models/river/profile/Hyd_River_Profile_Connection_Outflow.cpp \
           source_code/models/river/profile/Hyd_River_Profile_Connection_Standard.cpp \
           source_code/models/river/profile/_Hyd_River_Profile.cpp \
           source_code/models/river/profile/Hyd_Tables.cpp \
           source_code/models/river/profile/Hyd_Profile_Segment.cpp \
           source_code/models/river/profile/Hyd_River_Profile_Point.cpp \
           source_code/models/coast/Hyd_Coast_Model.cpp \
           source_code/models/coast/Hyd_Parse_CO.cpp \
		   source_code/models/temperature\HydTemp_Parse.cpp\
		   source_code/models/temperature\HydTemp_Param.cpp\
		   source_code/models/temperature\HydTemp_Model.cpp\
		   source_code/models/temperature\HydTemp_Profile.cpp\
           source_code/models/coupling/Hyd_Coupling_Model_List.cpp \
           source_code/models/coupling/Hyd_Coupling_Management.cpp \
           source_code/models/coupling/Hyd_Coupling_RV2CO.cpp \
           source_code/models/coupling/Hyd_Coupling_FP2FP.cpp \
           source_code/models/coupling/Hyd_Coupling_RV2FP.cpp \
           source_code/models/coupling/Hyd_Coupling_RV2FP_Merged.cpp \
           source_code/models/coupling/Hyd_Coupling_FP2CO.cpp \
           source_code/models/coupling/Hyd_Coupling_FP2CO_Merged.cpp \
           source_code/models/coupling/Hyd_Coupling_RV2RV.cpp \ 
           source_code/models/coupling/Hyd_Coupling_RV2RV_Diversion.cpp \
           source_code/models/coupling/Hyd_Coupling_RV2FP_Structure.cpp \
           source_code/models/coupling/Hyd_Coupling_RV2FP_Dikebreak.cpp \
           source_code/models/coupling/_Hyd_Coupling_Point.cpp \
           source_code/models/coupling/Hyd_Coupling_Point_FP2CO.cpp \
           source_code/models/coupling/Hyd_Coupling_Point_FP2FP.cpp \
           source_code/models/coupling/Hyd_Coupling_Point_RV2FP.cpp \
           source_code/models/coupling/_Hyd_Coupling_Point_List.cpp \
           source_code/models/coupling/Hyd_Coupling_Point_FP2CO_List.cpp \
           source_code/models/coupling/Hyd_Coupling_Point_FP2FP_List.cpp \
           source_code/models/coupling/Hyd_Coupling_Point_RV2FP_List.cpp \
           source_code/models/coupling/_Hyd_Coupling_Structure.cpp \
           source_code/models/coupling/Hyd_Coupling_Structure_Weir.cpp \
           source_code/models/coupling/Hyd_Coupling_Structure_Gate.cpp \
           source_code/models/coupling/Hyd_Parse_Coupling_Diversion.cpp \
           source_code/models/coupling/Hyd_Parse_Coupling_RV2FP_Structure.cpp \
           source_code/models/coupling/Hyd_Parse_Coupling_RV2FP_Dikebreak.cpp \
           source_code/szenario/Hyd_Boundary_Szenario.cpp \
           source_code/szenario/Hyd_Boundary_Szenario_Management.cpp \
           source_code/szenario/Hyd_Boundary_Scenario_List.cpp \
           source_code/gui/HydGui_Profile_Plot.cpp \
           source_code/gui/HydGui_System_Filechooser_Dia.cpp \
           source_code/gui/HydGui_Boundary_Scenario_Dia.cpp \
           source_code/gui/HydGui_Floodplain_Member_Wid.cpp \
           source_code/gui/HydGui_River_Member_Wid.cpp \
           source_code/gui/HydGui_Rv_Profile_General_Wid.cpp \
           source_code/gui/HydGui_System_Member_Wid.cpp \
           source_code/gui/HydGui_Bound_Scenario_Table_Wid.cpp \
           source_code/gui/HydGui_Bound_Scenario_Data_Dia.cpp \
           source_code/gui/HydGui_System_State_Dia.cpp \
           source_code/gui/HydGui_Coupling_Structure_Wid.cpp \
           source_code/gui/HydGui_Diversion_Channel_Wid.cpp \
           source_code/gui/HydGui_River_Geometrics_Table.cpp \
           source_code/gui/HydGui_Rv_Profile_Boundary_Lateral_Params_Wid.cpp \
        	 source_code/gui/HydGui_Rv_Profile_Boundary_Point_Params_Wid.cpp \
        	 source_code/gui/HydGui_Rv_Profile_Break_Params_Wid.cpp \
        	 source_code/gui/HydGui_Rv_Profile_Overflow_Params_Wid.cpp\
        	 source_code/gui/HydGui_Profil2Dgm_Converter_Dia.cpp\
        	 source_code/models/coupling/Hyd_Coupling_FP2CO_Dikebreak.cpp\
        	 source_code/models/coupling/_Hyd_Coupling_Dikebreak.cpp\
        	 source_code/models/coupling/Hyd_Parse_Coupling_FP2CO_Dikebreak.cpp\
			 source_code/models/groundwater\Hyd_Model_Groundwater.cpp

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
