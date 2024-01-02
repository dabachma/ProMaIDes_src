




//#pragma once

//decide here the main program
#define all1
//#define hydraulic     // If uncommented then remove WIN32 from CMakeLists.txt in line add_executable(ProMaIDeS WIN32... | Otherwise the executable will open in the background without output
//#define geometry      // If uncommented then remove WIN32 from CMakeLists.txt in line add_executable(ProMaIDeS WIN32... | Otherwise the executable will open in the background without output
//#define damage        // If uncommented then remove WIN32 from CMakeLists.txt in line add_executable(ProMaIDeS WIN32... | Otherwise the executable will open in the background without output
//#define test_sc_break // If uncommented then remove WIN32 from CMakeLists.txt in line add_executable(ProMaIDeS WIN32... | Otherwise the executable will open in the background without output
//#define sizes1        // If uncommented then remove WIN32 from CMakeLists.txt in line add_executable(ProMaIDeS WIN32... | Otherwise the executable will open in the background without output
//#define solver        // If uncommented then remove WIN32 from CMakeLists.txt in line add_executable(ProMaIDeS WIN32... | Otherwise the executable will open in the background without output
//#define solvergpu     // If uncommented then remove WIN32 from CMakeLists.txt in line add_executable(ProMaIDeS WIN32... | Otherwise the executable will open in the background without output
    
//#define my

//

using namespace std;
//_______________________________________
//all
#ifdef my

#include <iostream>
using namespace std;

int main(int argc, char *argv[]) {
	Geo_Point test;
	test.set_point(3,4,"test");
	ostringstream cout1;
	test.output_members(&cout1);
	int n = 0;
	cout << "hallo world" << endl;
	cin >> n;

	return 0;


}






#endif
//_______________________________________
//all
#ifdef all1

//qtclass
#include <QApplication>
#include <QStyleFactory>
//class
//#include "System.h"
#include "Main_Wid.h"

#include "Sys_Reconnect_Db_Dia.h"
//#define _CRTDBG_MAP_ALLOC
//#include <stdlib.h>
//#include <crtdbg.h>
//
//#ifdef _DEBUG
//#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
//// Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
//// allocations to be of _CLIENT_BLOCK type
//#else
//#define DBG_NEW new
//#endif



//test

int main(int argc, char *argv[]){
    QApplication app(argc, argv);
    //qt-macro for inititalize the resources which are stored in a static library
        Q_INIT_RESOURCE(system_sys_resource);
        Q_INIT_RESOURCE(hydroxygen);
		//QLocale::setDefault(QLocale("en_US"));
		//set lib path
        QStringList buff;
        buff.append(QCoreApplication::applicationDirPath().append("/platforms"));
        buff.append(QCoreApplication::applicationDirPath().append("/"));
        buff.append(QCoreApplication::applicationDirPath().append("/sqldrivers"));
        buff.append(QCoreApplication::applicationDirPath().append("/imageformats"));
		
        QCoreApplication::setLibraryPaths(buff);
		//try to change style; no effect!
		//QApplication::setStyle(QStyleFactory::create("Fusion"));
       //app.setStyle(QStyleFactory::create("Windows"));
       //app.setStyle(QStyleFactory::create("windowsvista"));
       //app.setStyle(QStyleFactory::create("Fusion"));
       



	

    if(0==0){
        try{

            Main_Wid my_system(argc, argv);
            app.exec();
        }
        catch(Error msg){
            msg.output_msg(0);
        }
    }

    Sys_Memory_Count::self()->output_mem();
    Sys_Memory_Count::self()->make_warning_end_widget();

    /*int test;
     test=_CrtDumpMemoryLeaks();*/

	//_CrtDumpMemoryLeaks();

    return 0;
}
#endif
//end all
//__________________________________________
//hydraulic
#ifdef hydraulic
//general libs
#include <iostream>
//hyd libs
#include "Hyd_Multiple_Hydraulic_Systems.h"
//sys libs
#include "Sys_Common_Output.h"

#include <QApplication>

using namespace std;

int main(int argc , char *argv[]){
    //output settings
    Sys_Common_Output my_output;
    my_output.new_output_excep();
    my_output.new_output_hyd();
    my_output.new_output_system();
    my_output.output_hyd->set_detailflag(false);

    Hyd_Hydraulic_System *system_1;
    //Hyd_Hydraulic_System *system_2;

    QApplication app(argc, argv);

    system_1=new Hyd_Hydraulic_System;
    //system_2=new Hyd_Hydraulic_System;

    Sys_Memory_Count::self()->output_mem();
    try{
        //system_1->set_system_per_file("L:/daniel/work/promaides/test_pro/markgreifenheide/data/HYD/Hyd_mgh_10x10/HYD/mgh_10x10_Ref.ilm");
        //system_1->set_system_per_file("L:/daniel/work/promaides/test_pro/mark1/data/HYD/Hyd_mgh_25x25/mgh_25x25_Ref.ilm");
        //system_1->set_system_per_file("L:/daniel/work/promaides/test_pro/rur_test/data/HYD/Rur_Kompletto_nur_ein_Zeitschritt/HQ100NL_DGMNL_dikeline_all.ilm");

        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/TestCase_LargeFP_Montain/HYD/Simple_L_FP_Mountain.ilm");

        // FOR GPU
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/newcastle/newcastle.ilm");
        // FOR CPU
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/newcastleWithLessBound/newcastle.ilm");

        //Dike Testing
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/diketest/dike.ilm");

        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/TestCase_LargeFP_Montain/HYD/1000x1000_L_FP_Mountain.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/flat_plain_with_one_pointSource/flat_withPointSource.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/flat_plain_with_one_pointSource/flat_withPointSource2M.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/flat_plain_with_one_pointSource5x5/flat_withPointSource.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/flat_plain_with_one_pointSourceRec/flat_withPointSourceRec.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/flat_plain_with_one_pointSourceRec99/flat_withPointSourceRec99.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/1M_Perlin_30/project.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/TestCase_Aachen/ac_rain.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/TestCase_AachenCroppedInt/ac_rain.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/TestCase_Aachencropped/ac_rain.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/komplete_tests/HYD/test_case_HQ100.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/newcastleWithCoupling/newcastleCouple.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/simpleCoupling/simpleCoupling.ilm");

        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/herrenkrug/herrenkrug.ilm");
        system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/dikeBreak/project.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/aachen_hq/ac_rain.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/aachen_hq/ac_rain_gpu.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/herrenkrugNoCoupling/herrenkrugNoCap.ilm");

        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/roman_2fp_1rv/roman.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/rur21/HQ21.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/rur21Raster3(no.2)NoRv/HQ21Raster3noRv.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/flat_plain_with_one_pointSource_poleni/flat_with_poleni.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/flat_plain_with_one_pointSourceButRaster3/flat_withPointSourceRaster3.ilm");

        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/rur21OnlyRaster0/HQ21oneOnly.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/rur21Raster5/HQ21Raster5.ilm");

        //system_1->set_system_per_file("/home/ubunntu/Documents/khan/HR1000_l_12m.ilm");
        //system_1->set_system_per_file("/home/ubunntu/Documents/1M_Perlin_30/project.ilm");
        //system_1->set_system_per_file("/home/ubunntu/Documents/herrenkrug/herrenkrug.ilm");
        //system_1->set_system_per_file("/home/ubunntu/Documents/simpleCoupling/simpleCoupling.ilm");
        //system_1->set_system_per_file("/home/ubunntu/Documents/TestCase_Aachen/ac_rain.ilm");
        //system_1->set_system_per_file("/home/ubunntu/Documents/newcastleWithCoupling/newcastleCouple.ilm");

        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/SpeedTesting/1D-2D-Modell mit Deichbruechen 2002/HYD/Elbe_Deichbrueche_2002.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/SpeedTesting/1D-2D-Modell mit Deichbruechen 2002/HYD/Elbe_Deichbrueche_2002_gpu.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/SpeedTesting/1D-2D-Modell ohne Deichbrueche 2002/HYD/Elbe_ohne_Deichbrueche_2002.ilm");
        //system_1->set_system_per_file("C:/Users/abaghdad/Desktop/testcases/SpeedTesting/1D-2D-Modell ohne Deichbrueche 2002/HYD/Elbe_ohne_Deichbrueche_2002_gpu.ilm");


        system_1->init_models();
        system_1->init_solver();
        system_1->set_folder_name_file();

        system_1->output_setted_members();
        system_1->make_calculation();
        system_1->output_final_model_statistics();
    }
    catch(Error msg){
        msg.output_msg(2);
    }

    delete system_1;
    //Sys_Memory_Count::self()->output_mem();
    my_output.delete_output_excep();
    my_output.delete_output_hyd();
    my_output.delete_output_system();
    //Sys_Memory_Count::self()->output_mem();

return 0;
}
#endif
//end hydraulic
//__________________________________________
//geometry
#ifdef geometry
//general libs
#include "Common_Const.h"
#include "Geo_Simple_Polygon.h"
#include "Geo_Segment.h"
#include "Sys_Common_Output.h"

using namespace std;

void main(void){
Sys_Common_Output my_output;
my_output.new_output_excep();
my_output.new_output_hyd();

time_t start_time;
time_t actual_time;

const int number=4;
//const int number1=4;
Geo_Point my_points[number];
//
//
Geo_Simple_Polygon my_poly;
Geo_Simple_Polygon my_poly1;
//Geo_Segment seg;
//Geo_Segment seg2;

try{
    my_points[0].set_point_coordinate(0,0);
    my_points[1].set_point_coordinate(10,0);
    my_points[2].set_point_coordinate(10,10);
    my_points[3].set_point_coordinate(0,10);
	

    my_poly.set_number_points(number);
    my_poly.set_points(my_points);

    my_points[0].set_point_coordinate(5,0);
    my_points[1].set_point_coordinate(5,1);
    my_points[2].set_point_coordinate(6,1);
    my_points[3].set_point_coordinate(6,0);
	

    my_poly1.set_number_points(number);
    my_poly1.set_points(my_points);

    /*seg.set_points(&my_points[0],&my_points[1]);
    seg2.set_points(&my_points[1],&my_points[0]);*/

    double area=2.0;
    //time(&start_time);
    //for(int j=0; j<1000; j++){
    //	for(long int i=0; i< 100000000; i++){
    //		//area=my_poly.polygon_interception(&my_poly1);
    //		area=0.10449968880528*atan(159.877741951379*0.555*(i+1));
    //	}
    //	//cout <<j<<endl;
    //}
    //time(&actual_time);
    //cout << "Calculation completed in " <<actual_time-start_time << label::sec << endl;
    //time(&start_time);
    //for(int j=0; j<1000; j++){
    //	for(long int i=0; i< 100000000; i++){
    //		//area=my_poly.polygon_interception(&my_poly1);
    //		area=0.10449968880528*(159.877741951379*0.555*(i+1));
    //	}
    //	//cout <<j<<endl;
    //}
    //time(&actual_time);
    //cout << "Calculation completed in " <<actual_time-start_time << label::sec << endl;

    area=my_poly.polygon_interception(&my_poly1);
    cout << "1 area  " << area<<endl;
    area=my_poly1.polygon_interception(&my_poly);
    cout << "2 area  " << area<<endl;
}
catch(Error msg){
    msg.output_msg(2);
}

my_output.delete_output_excep();
my_output.delete_output_hyd();

return;
}
#endif
//end geometry
//__________________________________________
//damage
#ifdef damage
//general libs
#include <iostream>
//dam libs
#include "Dam_Ecn_System.h"
//sys libs
#include "Sys_Common_Output.h"

#include <QApplication>

using namespace std;

int main(int argc , char *argv[]){
    //output settings
    Sys_Common_Output my_output;
    my_output.new_output_excep();
    my_output.new_output_dam();
    my_output.output_dam->set_detailflag(true);

    QApplication app(argc, argv);

    Sys_Memory_Count::self()->output_mem();

    string immob[1];
    immob[0]="dam_raster_immob.txt";
    string immob_stock[1];
    immob_stock[0]="dam_raster_immob_stock.txt";;

    string mob[1];
    mob[0]="dam_raster_mob.txt";;
    string mob_stock[1];
    mob_stock[0]=label::not_set;

    Dam_Ecn_System my_system;
    try{
        my_system.read_damage_function_per_file("damage_function.dam");
        my_system.read_damage_raster_per_file(1,mob,immob,mob_stock, immob_stock);
        my_system.output_members();
    }
    catch(Error msg){
        msg.output_msg(4);
    }

    Sys_Memory_Count::self()->output_mem();
    my_output.delete_output_excep();
    my_output.delete_output_dam();
    Sys_Memory_Count::self()->output_mem();

return 0;
}
#endif
//end damage
//_______________________________________
//test break scenario
#ifdef test_sc_break

//qtclass
//class
#include "Risk_Break_Scenario.h"

int main(int argc, char *argv[]){
    Risk_Break_Scenario my_scenario;
    Hyd_Boundary_Szenario bound_sc;
    bound_sc.set_id(3);

    const int number=3;
    int sc[number];
    for(int i=0;i<number; i++){
        sc[i]=i;
    }
    my_scenario.set_risk_type(_risk_type::catchment_risk);
    my_scenario.set_break_scenario(number, sc, bound_sc);
    my_scenario.output_members();

    cout << endl;
    bound_sc.set_id(13);
    my_scenario.set_risk_type(_risk_type::catchment_risk);
    my_scenario.set_break_scenario(0, NULL, bound_sc);
    my_scenario.output_members();

    cout << endl;
    bound_sc.set_id(34);
    string test;
    test="sc_1_4_6_";
    my_scenario.set_break_scenario_str(bound_sc, test);
    my_scenario.output_members();

    cout << endl;
    bound_sc.set_id(80);
    test="sc_";
    my_scenario.set_break_scenario_str(bound_sc, test);
    my_scenario.output_members();

    return 0;
}
#endif
//end test break scenario
//_______________________________________
//test size
#ifdef sizes1

//qtclass
#include <QtWidgets>
#include "qsplitter.h"
//class
#include "Geo_Point.h"
#include "Geo_Polysegment.h"
#include "Geo_Straight_Line.h"
#include "Geo_Simple_Polygon.h"
#include "Geo_Raster.h"
#include "Geo_Ray.h"
#include "Common_Const.h"

#include "Hyd_Hydraulic_System.h"
#include "Dam_Damage_System.h"
#include "Alt_System.h"
#include "Fpl_Calculation.h"
#include "Madm_System.h"
#include "Cost_System.h"

int main(int argc, char *argv[]){

 Hyd_Hydraulic_System test;
  Fpl_Calculation test2;
Dam_Damage_System test3;
Madm_System test4;
Cost_System test5;
Alt_System test6;

    Hyd_Hydraulic_System::close_hyd_database_tables();
    cout << "sizeof(Geo_Point) "<<sizeof(Geo_Point) << " " << (double)256/(double)sizeof(Geo_Point)<<endl;
    cout << "sizeof(Geo_Straight_Line) "<<sizeof(Geo_Straight_Line) << " " << (double)256/(double)sizeof(Geo_Straight_Line)<<endl;
    cout << "sizeof(Geo_Polysegment) "<<sizeof(Geo_Polysegment) << " " << (double)256/(double)sizeof(Geo_Polysegment)<<endl;
    cout << "sizeof(Geo_Simple_Polygon) "<<sizeof(Geo_Simple_Polygon) << " " << (double)256/(double)sizeof(Geo_Simple_Polygon)<<endl;
    cout << "sizeof(Geo_Raster_Polygon) "<<sizeof(Geo_Raster_Polygon) << " " << (double)256/(double)sizeof(Geo_Raster_Polygon)<<endl;
    cout << "sizeof(Geo_Raster) "<<sizeof(Geo_Raster) << " " << (double)256/(double)sizeof(Geo_Raster)<<endl;
    cout << "sizeof(Geo_Ray) "<<sizeof(Geo_Ray) << " " << (double)256/(double)sizeof(Geo_Ray)<<endl;
//	cout << "sizeof(Hyd_Element_Floodplain_Type_Standard) "<<sizeof(Hyd_Element_Floodplain_Type_Standard) << " " << (double)256/(double)sizeof(Hyd_Element_Floodplain_Type_Standard)<<endl;
//	cout << "sizeof(Hyd_Element_Floodplain_Type_Dam) "<<sizeof(Hyd_Element_Floodplain_Type_Dam) << " " << (double)256/(double)sizeof(Hyd_Element_Floodplain_Type_Dam)<<endl;
//	cout << "sizeof(Hyd_Element_Floodplain) "<<sizeof(Hyd_Element_Floodplain) << " " << (double)256/(double)sizeof(Hyd_Element_Floodplain)<<endl;
//	cout << "sizeof(double) "<<sizeof(double) << " " << (double)256/(double)sizeof(double)<<endl;
    cout << "sizeof(int) "<<sizeof(int) << " " << (double)256/(double)sizeof(int)<<endl;
    cout << "sizeof(bool) "<<sizeof(bool) << " " << (double)256/(double)sizeof(bool)<<endl;
//	cout << "sizeof(Hyd_Boundary_Szenario) "<<sizeof(Hyd_Boundary_Szenario) << " " << (double)256/(double)sizeof(Hyd_Boundary_Szenario)<<endl;
//	cout << "sizeof(_hyd_hydrological_balance) "<<sizeof(_hyd_hydrological_balance) << " " << (double)256/(double)sizeof(_hyd_hydrological_balance)<<endl;
//	cout << "sizeof(Hyd_Floodplainraster_Segment) "<<sizeof(Hyd_Floodplainraster_Segment) << " " << (double)256/(double)sizeof(Hyd_Floodplainraster_Segment)<<endl;
//	cout << "sizeof(Hyd_Floodplainraster_Point) "<<sizeof(Hyd_Floodplainraster_Point) << " " << (double)256/(double)sizeof(Hyd_Floodplainraster_Point)<<endl;
//	cout << "sizeof(Hyd_Floodplainraster_Polygon) "<<sizeof(Hyd_Floodplainraster_Polygon) << " " << (double)256/(double)sizeof(Hyd_Floodplainraster_Polygon)<<endl;
//	cout << "sizeof(Geo_Raster_Segment) "<<sizeof(Geo_Raster_Segment) << " " << (double)256/(double)sizeof(Geo_Raster_Segment)<<endl;
//	cout << "sizeof(Geo_Segment) "<<sizeof(Geo_Segment) << " " << (double)256/(double)sizeof(Geo_Segment)<<endl;
//	cout << "sizeof(Dam_Ecn_Element) "<<sizeof(Dam_Ecn_Element) << " " << (double)256/(double)sizeof(Dam_Ecn_Element)<<endl;

 return 0;
}
#endif
//end size

#ifdef solver

//solverclass
//#include "sundails_band.c"

#define ROW(i,j,smu) (i-j+smu)

int main(int argc, char *argv[]){

    int mat_n=4;

    realtype **a;
    int n=mat_n;
    int mu=mat_n;
    int ml=mat_n;
    int smu=mat_n*2;
    int *p;

    p=new int[mat_n];
    for(int i=0; i<mat_n;i++){
        p[i]=0;
    }

        int  colSize;

          if (n <= 0) return(NULL);

          a = NULL;
          a = (realtype **) malloc(n * sizeof(realtype *));
          if (a == NULL) return(NULL);

          colSize = smu + ml + 1;
          a[0] = NULL;
          a[0] = (realtype *) malloc(n * colSize * sizeof(realtype));
          if (a[0] == NULL) {
            free(a); a = NULL;
            return(NULL);
          }

          for (int j=1; j < n; j++) a[j] = a[0] + j * colSize;



    a[0][0]=6;
    a[0][1]=5;
    a[0][2]=3;
    a[0][3]=-10;
    a[1][0]=3;
    a[1][1]=7;
    a[1][2]=-3;
    a[1][3]=5;
    a[2][0]=12;
    a[2][1]=4;
    a[2][2]=4;
    a[2][3]=4;
    a[3][0]=0;
    a[3][1]=12;
    a[3][2]=0;
    a[3][3]=-8;

    cout <<"p"<<endl;
    for(int i=0; i<mat_n;i++){
        cout <<  p[i]<<endl;
    }
    cout <<"a"<<endl;
    for(int i=0; i<mat_n;i++){
        cout << i << "   ";
        for(int j=0; j<mat_n;j++){
        cout << a[i][j]<< " " ;
        }
        cout << endl;
    }

  int c, r, num_rows, n_minus;
  int i, j, k, l, storage_l, storage_k, last_col_k, last_row_k;
  realtype *a_c, *col_k, *diag_k, *sub_diag_k, *col_j, *kptr, *jptr;
  realtype max, temp, mult, a_kj;
  booleantype swap;
  realtype abs_buff;
  /* zero out the first smu - mu rows of the rectangular array a */

  n_minus=n-1;
  num_rows = smu - mu;
  if (num_rows > 0) {
    for (c=0; c < n; c++) {
      a_c = a[c];
      for (r=0; r < num_rows; r++) {
        a_c[r] = 0;
      }
    }
  }


    cout <<"p"<<endl;
    for(int i=0; i<mat_n;i++){
        cout <<  p[i]<<endl;
    }
    cout <<"a"<<endl;
    for(int i=0; i<mat_n;i++){
        cout << i << "   ";
        for(int j=0; j<mat_n;j++){
        cout << a[i][j]<< " " ;
        }
        cout << endl;
    }


  /* k = elimination step number */

  for (k=0; k < n_minus; k++, p++) {
    
    col_k     = a[k];
    diag_k    = col_k + smu;
    sub_diag_k = diag_k + 1;
    last_row_k = MIN(n_minus,k+ml);

    /* find l = pivot row number */

    l=k;
    max = ABS(*diag_k);
    for (i=k+1, kptr=sub_diag_k; i <= last_row_k; i++, kptr++) {
        abs_buff=ABS(*kptr);
      /*if (ABS(*kptr) > max) {
        l=i;
        max = ABS(*kptr);
      }*/
      if (abs_buff > max) {
        l=i;
        max = abs_buff;
      }
    }
    storage_l = ROW(l, k, smu);
    *p = l;
    
    /* check for zero pivot element */

    if (col_k[storage_l] == 0) return(k+1);
    
    /* swap a(l,k) and a(k,k) if necessary */
    
    if ( (swap = (l != k) )) {
      temp = col_k[storage_l];
      col_k[storage_l] = *diag_k;
      *diag_k = temp;
    }

    /* Scale the elements below the diagonal in         */
    /* column k by -1.0 / a(k,k). After the above swap, */
    /* a(k,k) holds the pivot element. This scaling     */
    /* stores the pivot row multipliers -a(i,k)/a(k,k)  */
    /* in a(i,k), i=k+1, ..., MIN(n-1,k+ml).            */
    
    mult = -1 / (*diag_k);
    for (i=k+1, kptr = sub_diag_k; i <= last_row_k; i++, kptr++){
        if((*kptr)!=0.0){
            (*kptr) *= mult;
        }

    }


    /* row_i = row_i - [a(i,k)/a(k,k)] row_k, i=k+1, ..., MIN(n-1,k+ml) */
    /* row k is the pivot row after swapping with row l.                */
    /* The computation is done one column at a time,                    */
    /* column j=k+1, ..., MIN(k+smu,n-1).                               */
    
    last_col_k = MIN(k+smu,n_minus);
    for (j=k+1; j <= last_col_k; j++) {
      
      col_j = a[j];
      storage_l = ROW(l,j,smu);
      storage_k = ROW(k,j,smu);
      a_kj = col_j[storage_l];

      /* Swap the elements a(k,j) and a(k,l) if l!=k. */
      if (swap) {
        col_j[storage_l] = col_j[storage_k];
        col_j[storage_k] = a_kj;
      }

      /* a(i,j) = a(i,j) - [a(i,k)/a(k,k)]*a(k,j) */
      /* a_kj = a(k,j), *kptr = - a(i,k)/a(k,k), *jptr = a(i,j) */

      if (a_kj != 0) {
        for (i=k+1, kptr=sub_diag_k, jptr=col_j+ROW(k+1,j,smu);
             i <= last_row_k;
             i++, kptr++, jptr++)
                (*jptr) += a_kj * (*kptr);

      }
    }
    cout <<"p"<<endl;
    for(int i=0; i<mat_n;i++){
        cout <<  p[i]<<endl;
    }
    cout <<"a"<<endl;
    for(int i=0; i<mat_n;i++){
        cout << i << "   ";
        for(int j=0; j<mat_n;j++){
        cout << a[i][j]<< " " ;
        }
        cout << endl;
    }
  }
  
  /* set the last pivot row to be n-1 and check for a zero pivot */

  *p = n_minus;
  if (a[n_minus][smu] == 0) return(n);

    cout <<"p"<<endl;
    for(int i=0; i<mat_n;i++){
        cout <<  p[i]<<endl;
    }
    cout <<"a"<<endl;
    for(int i=0; i<mat_n;i++){
        cout << i << "   ";
        for(int j=0; j<mat_n;j++){
        cout << a[i][j] <<endl;
        }
    }



    return 0;
}
#endif
//end solver

#ifdef solvergpu

#include "common.h"
#include "CDomainCartesian.h"


int main(int argc, char* argv[]) {

    double outputFrequency = 10.0;
    double simulationLength = 100.0;

    CModel* pManager = new CModel(NULL, false);

    //Set up the Manager Settings
    pManager->setSelectedDevice(1);							                // Set GPU device to Use. 
    pManager->setSimulationLength(simulationLength);						// Set Simulation Length
    pManager->setOutputFrequency(outputFrequency);							// Set Output Frequency
    pManager->setFloatPrecision(model::floatPrecision::kDouble);			// Set Precision

    //255 works
    //257 no

    //Create the domain
    CDomainCartesian* ourCartesianDomain = pManager->getDomain();
    ourCartesianDomain->setCellResolution(1.0, 1.0);
    ourCartesianDomain->setCols(65537);
    ourCartesianDomain->setRows(1);
    ourCartesianDomain->setUseOptimizedCoupling(false);
    ourCartesianDomain->setOptimizedCouplingSize(0);
    ourCartesianDomain->setName("main.cpp Domain");

    //Create the Scheme,
    model::SchemeSettings schemeSettings;
    schemeSettings.scheme_type = model::schemeTypes::kGodunovGPU;
    schemeSettings.CourantNumber = 0.5;
    schemeSettings.DryThreshold = 1E-10;
    schemeSettings.Timestep = 0.1;
    schemeSettings.ReductionWavefronts = 200;
    schemeSettings.FrictionStatus = false;
    schemeSettings.NonCachedWorkgroupSize[0] = 27;
    schemeSettings.NonCachedWorkgroupSize[1] = 27;
    schemeSettings.debuggerOn = false;
    CScheme::createScheme(pManager, schemeSettings);


    pManager->log->logInfo("Setting Data...");
    unsigned long ulCellID;
    unsigned char	ucRounding = 6;
    for (unsigned long iRow = 0; iRow < ourCartesianDomain->getRows(); iRow++) {
        for (unsigned long iCol = 0; iCol < ourCartesianDomain->getCols(); iCol++) {
            ulCellID = ourCartesianDomain->getCellID(iCol, ourCartesianDomain->getRows() - iRow - 1);
            //Elevations
            ourCartesianDomain->setBedElevation(ulCellID, 0.0);
            ourCartesianDomain->setFSL(ulCellID, 0.0);
            //Manning Coefficient
            ourCartesianDomain->setManningCoefficient(ulCellID, 0.028);
            //Depth
            ourCartesianDomain->setFSL(ulCellID, 0.0);
            //MaxDepth
            ourCartesianDomain->setMaxFSL(ulCellID, 0.0);
            //VelocityX
            ourCartesianDomain->setDischargeX(ulCellID, 0.0);
            //VelocityY
            ourCartesianDomain->setDischargeY(ulCellID, 0.0);
            //Boundary Condition
            ourCartesianDomain->setBoundaryCondition(ulCellID, 0.0);
            //Poleni Conditions
            ourCartesianDomain->setPoleniConditionX(ulCellID, false);
            //Poleni Conditions
            ourCartesianDomain->setPoleniConditionY(ulCellID, false);
            //Zxmax
            ourCartesianDomain->setZxmax(ulCellID, 0.0);
            //cx
            ourCartesianDomain->setcx(ulCellID, 0.0);
            //Zymax
            ourCartesianDomain->setZymax(ulCellID, 0.0);
            //cy
            ourCartesianDomain->setcy(ulCellID, 0.0);
            //Coupling Condition
            //ourCartesianDomain->setCouplingCondition(ulCellID, 0.0);

        }
    }

    pManager->log->logInfo("The computational engine is now ready.");
    pManager->ValidateAndPrepareModel();



    /// Running
    pManager->log->logInfo("Setting boundary conditions.");
    ourCartesianDomain->resetBoundaryCondition();
    for (unsigned long cellId = 0; cellId < ourCartesianDomain->getCellCount(); cellId++){
        ourCartesianDomain->setBoundaryCondition(cellId, 0.0);

    }
    ourCartesianDomain->getScheme()->importBoundaries();
    pManager->runNext(10);

    //Result Import
    pManager->log->logInfo("Importing results.");
    double* opt_h_gpu = new double[ourCartesianDomain->getCellCount()];
    double* opt_v_x_gpu = new double[ourCartesianDomain->getCellCount()];
    double* opt_v_y_gpu = new double[ourCartesianDomain->getCellCount()];
    pManager->getDomain()->readBuffers_h_vx_vy(opt_h_gpu, opt_v_x_gpu, opt_v_y_gpu);

    delete[] opt_h_gpu;
    delete[] opt_v_x_gpu;
    delete[] opt_v_y_gpu;


    pManager->log->logInfo("Deleting model...");
    //Deletion
    delete pManager;
}
#endif