#include "Dam_Headers_Precompiled.h"
//#include "_Dam_Element.h"


//Default constructor
_Dam_Element::_Dam_Element(void){
	this->elem_index=-1;
	this->index_fp=-1;
	this->index_fp_elem=-1;
	this->area=0.0;
	this->mid_point.set_point_name(label::elem_mid);
	this->polygon_string=label::not_set;
	this->global_index=-1;
}
//Default destructor
_Dam_Element::~_Dam_Element(void){
}
//_______________
//public
//Set the index of the connected floodplain
void _Dam_Element::set_index_floodplain(const int index){
	this->index_fp=index;
}
//Get the index of the connected floodplain
int _Dam_Element::get_index_floodplain(void){
	return this->index_fp;
}
//Get the midpoint of the element
Geo_Point* _Dam_Element::get_mid_point(void){
	return &this->mid_point;
}
//Set the index of the connected floodplain element
void _Dam_Element::set_index_floodplain_element(const int index){
	this->index_fp_elem=index;
}
//Get the index of the connected floodplain element
int _Dam_Element::get_index_floodplain_element(void){
	return this->index_fp_elem;
}
//Set the area of the element
void _Dam_Element::set_area_element(const double area){
	this->area=area;
}
//Get the area of the element
double _Dam_Element::get_area_element(void){
	return this->area;
}
//Set the index of the element
void _Dam_Element::set_index_element(const int index){
	this->elem_index=index;
}
//Get the index of the element
int _Dam_Element::get_element_index(void){
	return this->elem_index;
}
//_________
//protected

//Get the sql string for a raster element polygon
string _Dam_Element::get_elem_ploy_string(const double mid_x, const double mid_y, const double half_width_x, const double half_width_y){

    ostringstream buff;
    buff << FORMAT_FIXED_REAL << P(3);
    buff << " ST_GeomFromText('POLYGON((";

    buff<<mid_x-half_width_x << "  ";
    buff<<mid_y-half_width_y << ",  ";
    buff<<mid_x+half_width_x << "  ";
    buff<<mid_y-half_width_y << ",  ";
    buff<<mid_x+half_width_x << "  ";
    buff<<mid_y+half_width_y << ",  ";
    buff<<mid_x-half_width_x << "  ";
    buff<<mid_y+half_width_y << ",  ";
    buff<<mid_x-half_width_x << "  ";
    buff<<mid_y-half_width_y << "  ";


    buff << "))') ";
    return buff.str();
}
