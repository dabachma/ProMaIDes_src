#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Floodplainraster_Point.h"

//constructor
Hyd_Floodplainraster_Point::Hyd_Floodplainraster_Point(void){
		
	this->point_index=-1;
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplainraster_Point)-sizeof(Geo_Point), _sys_system_modules::HYD_SYS);

}
//Copy constructor
Hyd_Floodplainraster_Point::Hyd_Floodplainraster_Point(const Hyd_Floodplainraster_Point& object){
	Geo_Point::operator =(object);
	this->point_index=object.point_index;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Floodplainraster_Point)-sizeof(Geo_Point), _sys_system_modules::HYD_SYS);


}
//destructor
Hyd_Floodplainraster_Point::~Hyd_Floodplainraster_Point(void){

	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Floodplainraster_Point)-sizeof(Geo_Point), _sys_system_modules::HYD_SYS);

}
//________
//public
//Set the point index
void Hyd_Floodplainraster_Point::set_point_index(const int index){
	this->point_index=index;
}
//Get the point index
int Hyd_Floodplainraster_Point::get_point_index(void){
	return this->point_index;
}
//Output members
void Hyd_Floodplainraster_Point::output_members(ostringstream *cout){
	*cout << this->point_index << W(12) << P(2)<< FORMAT_FIXED_REAL<< this->get_xcoordinate();
	*cout << W(12) << P(2)<< FORMAT_FIXED_REAL << this->get_ycoordinate();
	*cout <<endl;

}
//Output members
void Hyd_Floodplainraster_Point::output_header(ostringstream *cout){
		
		*cout << "Index" << W(8) << "x" << label::m << W(8) << "y" << label::m  <<endl;
}
//Copy operator
Hyd_Floodplainraster_Point& Hyd_Floodplainraster_Point::operator=(const Hyd_Floodplainraster_Point& object){
	Geo_Point::operator =(object);
	this->point_index=object.point_index;
	return *this;
}
