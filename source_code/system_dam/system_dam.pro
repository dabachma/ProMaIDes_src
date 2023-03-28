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
CONFIG += x86_64

#make precompiled headers
PRECOMPILED_HEADER = source_code/Dam_Headers_Precompiled.h

#include libs
#CONFIG(debug, debug|release) {
#     LIBS += C:/Users/Meiner/Documents/programmierung/promaides/debug/debug/libsystem_sys.a\
#        C:/Users/Meiner/Documents/programmierung/promaides/debug_hyd/debug/libhyd_system.a\
     
#} else {
#      LIBS +=  C:/Users/Meiner/Documents/programmierung/promaides/release/release/libsystem_sys.a\
#          C:/Users/Meiner/Documents/programmierung/promaides/release_hyd/release/libhyd_system.a\
# }

TEMPLATE = lib
TARGET = dam_system


DEPENDPATH += . \
              source_code \                               
              source_code/economic \
				source_code/CI \ 			  
              source_code/general \                       
              source_code/ecologic \                      
              source_code/psycho_social \                 
              source_code/people \                        
              source_code/simple_count \                  
              source_code/gui \                           
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
               source_code/economic \
			   source_code/CI \ 
               source_code/general \
               source_code/ecologic \
               source_code/psycho_social \
               source_code/people \
               source_code/simple_count \
               source_code/gui \
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
                 ../../debug/ \
                ../../debug_hyd/

# Input      
HEADERS += source_code/Dam_Damage_System.h \
				source_code/Dam_Headers_Precompiled.h \
					 source_code/general/Dam_Hyd_Result_Raster.h\
					 source_code/general/Dam_Hydimport_Element.h\
					 source_code/economic/Dam_Ecn_Damage_Function.h \
					 source_code/economic/Dam_Ecn_System.h \
					 source_code/economic/Dam_Ecn_Element.h \
					 source_code/economic/Dam_Ecn_Raster.h \
					 source_code/general/Dam_Impact_Value_Floodplain.h \ 
					 source_code/general/Dam_Impact_Values.h \ 
					 source_code/general/Dam_Raster.h \
					 source_code/general/_Dam_Element.h \
					 source_code/general/_Dam_2value_Function.h \
					 source_code/general/_Dam_System.h \
					 source_code/general/Dam_Polygon.h \
					 source_code/ecologic/Dam_Eco_System.h \
					 source_code/ecologic/Dam_Eco_Btype_Damage_Function.h \
					 source_code/ecologic/Dam_Eco_Soil_Cost_Function.h \
					 source_code/ecologic/Dam_Eco_Soil_Erosion_Function.h \
					 source_code/ecologic/Dam_Eco_Btype_Element.h \
					 source_code/ecologic/Dam_Eco_Soil_Element.h \
					 source_code/ecologic/Dam_Eco_Btype_Raster.h \
					 source_code/ecologic/Dam_Eco_Soil_Raster.h \
					 source_code/ecologic/Dam_Eco_Btype.h \
					 source_code/people/Dam_People_System.h \
					 source_code/people/Dam_People_Raster.h \
					 source_code/people/Dam_People_Element.h \
					 source_code/people/Dam_People_Damage_Function.h \
					 source_code/simple_count/Dam_Sc_System.h \
					 source_code/simple_count/Dam_Sc_Point_Manager.h \
					 source_code/simple_count/Dam_Sc_Subcategory.h \
					 source_code/simple_count/Dam_Sc_Point.h \
					 source_code/CI/_Dam_CI_Element.h \
					 source_code/CI/Dam_CI_Point.h \
					 source_code/CI/Dam_CI_Polygon.h \
					 source_code/CI/Dam_CI_System.h\
					 source_code/CI/Dam_CI_Element_List.h\
					 source_code/psycho_social/Dam_Pys_System.h \
					 source_code/psycho_social/Dam_Pys_Raster.h \
					 source_code/psycho_social/Dam_Pys_Function.h \
					 source_code/psycho_social/Dam_Pys_Element.h \
					 source_code/psycho_social/Dam_Pys_Density_Function.h \
					 source_code/psycho_social/Dam_Pys_Category.h \
					 source_code/gui/DamGui_Result_Summary_Wid.h \
					 source_code/gui/DamGui_Result_Wid.h \
					 source_code/gui/DamGui_Number_Raster_Dia.h \
					 source_code/gui/DamGui_Ecn_Func_Wid.h \
					 source_code/gui/DamGui_Eco_Btype_Func_Wid.h \
					 source_code/gui/DamGui_Eco_Btype_Wid.h \
					 source_code/gui/DamGui_Eco_Soil_Cost_Wid.h \
					 source_code/gui/DamGui_Eco_Soil_Erosion_Wid.h \
					 source_code/gui/DamGui_Pop_Function_Wid.h \
					 source_code/gui/DamGui_Pys_Categories_Wid.h \
					 source_code/gui/DamGui_Raster_Wid.h\
					 source_code/gui/DamGui_Raster_Dia.h\
					 source_code/gui/DamGui_Sc_Point_Wid.h\
					 source_code/gui/DamGui_Sc_Subcategory_Wid.h\
					 source_code/gui/DamGui_Ci_Point_Wid.h\
					 source_code/gui/DamGui_Ci_Polygon_Wid.h\
					        
FORMS+=    source_code/gui/DamGui_Result_Wid.ui \
					 source_code/gui/DamGui_Number_Raster_Dia.ui \
					 source_code/gui/DamGui_Ecn_Func_Wid.ui \
					 source_code/gui/DamGui_Eco_Btype_Func_Wid.ui \
					 source_code/gui/DamGui_Eco_Btype_Wid.ui \
					 source_code/gui/DamGui_Eco_Soil_Cost_Wid.ui \
					 source_code/gui/DamGui_Eco_Soil_Erosion_Wid.ui \
					 source_code/gui/DamGui_Pop_Function_Wid.ui \
					 source_code/gui/DamGui_Pys_Categories_Wid.ui \
					 source_code/gui/DamGui_Raster_Wid.ui\
					 source_code/gui/DamGui_Raster_Dia.ui\
					 source_code/gui/DamGui_Sc_Point_Wid.ui\
					 source_code/gui/DamGui_Ci_Point_Wid.ui\
					 source_code/gui/DamGui_Ci_Polygon_Wid.ui\
             
SOURCES += source_code/Dam_Damage_System.cpp \
					 source_code/general/Dam_Hyd_Result_Raster.cpp\
					 source_code/general/Dam_Hydimport_Element.cpp\
					 source_code/economic/Dam_Ecn_Damage_Function.cpp \
					 source_code/economic/Dam_Ecn_System.cpp \
					 source_code/economic/Dam_Ecn_Element.cpp \
					 source_code/economic/Dam_Ecn_Raster.cpp \
					 source_code/general/Dam_Impact_Value_Floodplain.cpp \ 
           source_code/general/Dam_Impact_Values.cpp \ 
					 source_code/general/Dam_Raster.cpp \
					 source_code/general/_Dam_Element.cpp \
					 source_code/general/_Dam_2value_Function.cpp \
					 source_code/general/_Dam_System.cpp \
					 source_code/general/Dam_Polygon.cpp \
					 source_code/ecologic/Dam_Eco_System.cpp \
					 source_code/ecologic/Dam_Eco_Btype_Damage_Function.cpp \
					 source_code/ecologic/Dam_Eco_Soil_Cost_Function.cpp \
					 source_code/ecologic/Dam_Eco_Soil_Erosion_Function.cpp \
					 source_code/ecologic/Dam_Eco_Btype_Element.cpp \
					 source_code/ecologic/Dam_Eco_Soil_Element.cpp \
					 source_code/ecologic/Dam_Eco_Btype_Raster.cpp \
					 source_code/ecologic/Dam_Eco_Soil_Raster.cpp \
					 source_code/ecologic/Dam_Eco_Btype.cpp \
					 source_code/people/Dam_People_System.cpp \
					 source_code/people/Dam_People_Raster.cpp \
					 source_code/people/Dam_People_Element.cpp \
					 source_code/people/Dam_People_Damage_Function.cpp \
					 source_code/simple_count/Dam_Sc_System.cpp \
					 source_code/simple_count/Dam_Sc_Point_Manager.cpp \
					 source_code/simple_count/Dam_Sc_Subcategory.cpp \
					 source_code/simple_count/Dam_Sc_Point.cpp \
					 source_code/CI/_Dam_CI_Element.cpp \
					 source_code/CI/Dam_CI_Point.cpp \
					 source_code/CI/Dam_CI_Polygon.cpp \
					 source_code/CI/Dam_CI_System.cpp\
					 source_code/CI/Dam_CI_Element_List.cpp\
					 source_code/psycho_social/Dam_Pys_System.cpp \
					 source_code/psycho_social/Dam_Pys_Raster.cpp \
					 source_code/psycho_social/Dam_Pys_Function.cpp \
					 source_code/psycho_social/Dam_Pys_Element.cpp \
					 source_code/psycho_social/Dam_Pys_Density_Function.cpp \
					 source_code/psycho_social/Dam_Pys_Category.cpp \
					 source_code/gui/DamGui_Result_Summary_Wid.cpp \ 
					 source_code/gui/DamGui_Result_Wid.cpp \
					 source_code/gui/DamGui_Number_Raster_Dia.cpp \
					 source_code/gui/DamGui_Ecn_Func_Wid.cpp \
					 source_code/gui/DamGui_Eco_Btype_Func_Wid.cpp \
					 source_code/gui/DamGui_Eco_Btype_Wid.cpp \
					 source_code/gui/DamGui_Eco_Soil_Cost_Wid.cpp \
					 source_code/gui/DamGui_Eco_Soil_Erosion_Wid.cpp \
					 source_code/gui/DamGui_Pop_Function_Wid.cpp \
					 source_code/gui/DamGui_Pys_Categories_Wid.cpp \
					 source_code/gui/DamGui_Raster_Wid.cpp\
					 source_code/gui/DamGui_Raster_Dia.cpp\
					 source_code/gui/DamGui_Sc_Point_Wid.cpp\
					 source_code/gui/DamGui_Ci_Point_Wid.cpp\
					 source_code/gui/DamGui_Ci_Polygon_Wid.cpp\
					 source_code/gui/DamGui_Sc_Subcategory_Wid.cpp

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
