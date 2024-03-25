#include "Fpl_Headers_Precompiled.h"
//#include "Fpl_Seepage_Line_Point_List.h"


//Default constructor
Fpl_Seepage_Line_Point_List::Fpl_Seepage_Line_Point_List(void){

	this->number_points=0;
	this->points=NULL;

	Sys_Memory_Count::self()->add_mem(sizeof(Fpl_Seepage_Line_Point_List), _sys_system_modules::FPL_SYS);//count the memory

}
//Default destructor
Fpl_Seepage_Line_Point_List::~Fpl_Seepage_Line_Point_List(void){
	this->delete_points();
	Sys_Memory_Count::self()->minus_mem(sizeof(Fpl_Seepage_Line_Point_List), _sys_system_modules::FPL_SYS);//count the memory
}
//___________
//public
///Add a new point to the list
int Fpl_Seepage_Line_Point_List::add_new_point(const double x, const double h, const bool inside_body, Geo_Polysegment *cubature){
	Fpl_Seepage_Line_Point *buffer=NULL;
	try{
		buffer=new Fpl_Seepage_Line_Point[this->number_points+1];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(0);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

	//copy the points
	for(int i=0; i< this->number_points; i++){
		buffer[i]=this->points[i];
	}
	//set the last point
	buffer[this->number_points].set_waterlevel(h);
	buffer[this->number_points].set_x_coordinate(x);
	buffer[this->number_points].set_inside_dike_body_flag(inside_body);
	buffer[this->number_points].set_y_coordinate_cubature(cubature);

	this->number_points++;

	//delete the old table points
	this->delete_points();
	//set the new pointer to the table points
	this->points=buffer;


	return (this->number_points-1);
}
///Get a pointer to a point specified by a given index
Fpl_Seepage_Line_Point *Fpl_Seepage_Line_Point_List::get_list_point(const int index){
	if(index<0 || index>=this->number_points){
		return NULL;
	}
	else{
		return &this->points[index];
	}
}
///Get the number of points
int Fpl_Seepage_Line_Point_List::get_number_points(void){
	return this->number_points;
}
//Output member to display/console
void Fpl_Seepage_Line_Point_List::output_members(Geo_Polysegment *cubature){

	//output an sorted buffer; do never sort the connected seepage points
	Fpl_Seepage_Line_Point_List buffer;
	//copy them
	for(int i=0;i<this->number_points; i++){
		buffer.add_new_point(this->points[i].get_x_coordinate(), this->points[i].get_waterlevel(), this->points[i].get_inside_dike_body_flag(), cubature);
	}
	//sort them
	this->sort_list(&buffer);

	//output them	
	ostringstream cout;
	cout<<"SEEPAGE PONTS "<< endl;
	cout<<" Number points : "<<this->number_points<<endl;
	cout<<" no."<<W(12)<<" x "<<label::m<<W(12)<<" h " << label::m<<endl;
	Sys_Common_Output::output_fpl->output_txt(&cout);
	for(int i=0;i<this->number_points; i++){
		cout<<"  " <<i <<" " <<W(12)<< buffer.get_list_point(i)->get_x_coordinate()<<" "<<W(12)<<buffer.get_list_point(i)->get_waterlevel()<<endl;
		Sys_Common_Output::output_fpl->output_txt(&cout);
	}

}
//Output members to tecplot
void Fpl_Seepage_Line_Point_List::output_members2tecplot(ofstream *file_name, Geo_Polysegment *cubature){
//output an sorted buffer; do never sort the connected seepage points
	Fpl_Seepage_Line_Point_List buffer;
	//copy them
	for(int i=0;i<this->number_points; i++){
		buffer.add_new_point(this->points[i].get_x_coordinate(), this->points[i].get_waterlevel(), this->points[i].get_inside_dike_body_flag(), cubature);
	}
	//sort them
	this->sort_list(&buffer);
	for(int i=0;i<this->number_points; i++){
		if(buffer.get_list_point(i)->get_inside_dike_body_flag()==true){
			*file_name<<" " <<W(12)<< buffer.get_list_point(i)->get_x_coordinate()<<" "<<W(12)<<buffer.get_list_point(i)->get_waterlevel()<<endl;
		}

	}

}
//Output members to paraview
void Fpl_Seepage_Line_Point_List::output_members2paraview(ofstream *file_name, Geo_Polysegment *cubature, const int before, const int after) {
	Fpl_Seepage_Line_Point_List buffer;
	//copy them
	for (int i = 0; i < this->number_points; i++) {
		buffer.add_new_point(this->points[i].get_x_coordinate(), this->points[i].get_waterlevel(), this->points[i].get_inside_dike_body_flag(), cubature);
	}
	//sort them
	this->sort_list(&buffer);
	for (int i = 0; i < this->number_points; i++) {
		if (buffer.get_list_point(i)->get_inside_dike_body_flag() == true) {
			*file_name << buffer.get_list_point(i)->get_x_coordinate() << ",";
			functions::add_seperator_csv("NAN,", file_name, before);
			*file_name << buffer.get_list_point(i)->get_waterlevel();
			functions::add_seperator_csv(",NAN", file_name, after);

			*file_name<< endl;
		}

	}


}
//Output members to excel
void Fpl_Seepage_Line_Point_List::output_members2excel(ofstream *file_name, Geo_Polysegment *cubature, const int before, const int after) {
	Fpl_Seepage_Line_Point_List buffer;
	//copy them
	for (int i = 0; i < this->number_points; i++) {
		buffer.add_new_point(this->points[i].get_x_coordinate(), this->points[i].get_waterlevel(), this->points[i].get_inside_dike_body_flag(), cubature);
	}
	//sort them
	this->sort_list(&buffer);
	for (int i = 0; i < this->number_points; i++) {
		if (buffer.get_list_point(i)->get_inside_dike_body_flag() == true) {
			*file_name << buffer.get_list_point(i)->get_x_coordinate() << ";";
			functions::add_seperator_csv(";", file_name, before);
			*file_name << buffer.get_list_point(i)->get_waterlevel();
			functions::add_seperator_csv(";", file_name, after);

			*file_name << endl;
		}

	}
	
}
//Count the number of points inside the section body#
int Fpl_Seepage_Line_Point_List::count_number_inside_points(void){
	int counter=0;
	for(int i=0;i<this->number_points; i++){
		if(this->get_list_point(i)->get_inside_dike_body_flag()==true){
			counter++;		
		}
	}
	return counter;
}
//__________
//private
//Delete the points of the list
void Fpl_Seepage_Line_Point_List::delete_points(void){
	if(this->points!=NULL){
		delete []this->points;
		this->points=NULL;
	}
}
//Allocate the points of the list
void Fpl_Seepage_Line_Point_List::allocate_points(void){
	this->delete_points();
	try{
		this->points=new Fpl_Seepage_Line_Point[this->number_points];
	}
	catch(bad_alloc &t){
		Error msg=this->set_error(1);
		ostringstream info;
		info<< "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Sort the list after the x-values (just needed for an output)
void Fpl_Seepage_Line_Point_List::sort_list(Fpl_Seepage_Line_Point_List *list){
	//make the buffer for sorting
	Fpl_Seepage_Line_Point swap;
	int j=0;

	for (int i = 1; i < list->get_number_points(); i++) {
		swap=list->points[i];
		//the higher value goes first => ">" ??
		//the smaller value goes first => "<" 
		j = i-1;
		while ( j >= 0 && swap.get_x_coordinate() < list->points[j].get_x_coordinate()){
			list->points[j+1]=list->points[j];
			j--;
		}
		list->points[j+1]=swap;
	}
}
//Set the error(s)
Error Fpl_Seepage_Line_Point_List::set_error(const int err_type){
	string place="Fpl_Seepage_Line_Point_List::";
	string help;
	string reason;
	int type=0;
	bool fatal=false;
	stringstream info;
	Error msg;

	switch (err_type){
		case 0://bad_alloc
			place.append("add_new_point(const double x, const double h)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 1://bad_alloc
			place.append("allocate_points(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		default:
			place.append("set_error(const int err_type)");
			reason ="Unknown flag!";
			help="Check the flags";
			type=6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}