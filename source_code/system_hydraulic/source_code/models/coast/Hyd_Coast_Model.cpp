#include "Hyd_Headers_Precompiled.h"
//#include "Hyd_Coast_Model.h"

//constructor
Hyd_Coast_Model::Hyd_Coast_Model(void){
	this->params.filename_coastline=label::not_set;
	this->params.filename_instat_boundary=label::not_set;
	this->params.model_name="coast_model";
	this->params.filename_geometry_tecplot=label::not_set;

	this->global_sea_h=0.0;

	this->coastline_polysegment.set_line_type(_hyd_floodplain_polysegment_type::COASTLINE);
	this->coastline_polysegment.set_no_flow_flag(true);
	this->coastline_polysegment.set_element_type(_hyd_elem_type::COAST_ELEM);
	this->coastline_polysegment.set_is_closed(true);
	this->coastline_polysegment.set_index(0);
	this->coast_polygon.set_elementtype_inside(_hyd_elem_type::COAST_ELEM);
	
	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Hyd_Coast_Model), _sys_system_modules::HYD_SYS);
}
//destructor
Hyd_Coast_Model::~Hyd_Coast_Model(void){
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Hyd_Coast_Model), _sys_system_modules::HYD_SYS);
}
//____________
//public
//Input all members coastmodel members from files
void Hyd_Coast_Model::input_members(const string global_file, const string global_path){
	//set prefix for output
	ostringstream cout;
	ostringstream prefix;
	prefix << "CO> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	try{
		Hyd_Parse_CO parser;
		//read in the global fp params
		parser.set_input_filename(global_file);
		parser.open_input_file();
		parser.parse_coastmodel_params();

		//get the parameters
		this->params=parser.get_coastmodel_params();

		//complete the filenames with the global path
		this->complete_filenames_with_path(global_path);

		//read in the instationary boundary curves
		this->instat_boundary_curve.read_value(this->params.filename_instat_boundary,0);
		//set the type
		this->instat_boundary_curve.set_type(_hyd_bound_type::waterlevel_type);

		//read in the coastline
		this->coastline_polysegment.input_members(0,this->params.filename_coastline);

		//polyline to polygon
		this->polysegment2polygon();


	}catch(Error msg){
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}

	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();

}
//Transfer the coast model data to a database; the coast-line as well as the boundary condition
void Hyd_Coast_Model::transfer_input_members2database(QSqlDatabase *ptr_database){

	//set prefix for output
	ostringstream prefix;
	prefix << "CO> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout<<"Transfer the data of the coast model..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	try{
		//coastline
		this->coastline_polysegment.transfer_polysegment2database_table(ptr_database, 0, true);
		//instationary boundary curve
		this->instat_boundary_curve.transfer_instat_curve2database_table(ptr_database, _hyd_model_type::COAST_MODEL, 0);
	}
	catch(Error msg){
		ostringstream info;
		info<<"Try to transfer the data of the coast-model"<<endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}
	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Transfer a hydraulic boundary szenario from file to a database
void Hyd_Coast_Model::transfer_hydraulic_boundary_sz2database(QSqlDatabase *ptr_database){
	//set prefix for output
	ostringstream prefix;
	prefix << "CO> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	ostringstream cout;
	cout<<"Transfer hydraulic boundary scenario of the coast model..." <<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	try{
		//instationary boundary curve
		this->instat_boundary_curve.transfer_instat_curve2database_table(ptr_database, _hyd_model_type::COAST_MODEL, 0);
	}
	catch(Error msg){
		ostringstream info;
		info<<"Try to transfer the data of the coast-model"<<endl;
		msg.make_second_info(info.str());
		Sys_Common_Output::output_hyd->rewind_userprefix();
		throw msg;
	}


	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Input the coast model from a database selection of the relevant coastline; the coastline as well as the boundary condition
void Hyd_Coast_Model::input_members(const QSqlQueryModel *query_result, QSqlDatabase *ptr_database, const bool output_flag){

	if(output_flag==true){
		ostringstream cout;
		cout <<"Read the coast model from database..." <<endl;
		Sys_Common_Output::output_hyd->output_txt(&cout);
		//set prefix for output
		ostringstream prefix;
		prefix << "CO> ";
		Sys_Common_Output::output_hyd->set_userprefix(prefix.str());
	}

	try{
		//read in the coast line
		this->coastline_polysegment.input_members(query_result, ptr_database, 0);
		//polyline to polygon
		this->polysegment2polygon();


		//read in the boundary
		QSqlTableModel boundary_result(0, *ptr_database);
		int number=0;
		this->instat_boundary_curve.hyd_sz.set_members(&this->hyd_sz);
		number=this->instat_boundary_curve.select_relevant_curves_database(&boundary_result, 0,_hyd_model_type::COAST_MODEL, output_flag, &this->hyd_sz);
		if(number ==1){
			this->instat_boundary_curve.read_value(&boundary_result, 0);
		}
		else if(number >1){
			Error msg=this->set_error(2);
			throw msg;

		}
		else if(number <1){
			Error msg=this->set_error(4);
			throw msg;

		}

	}
	catch(Error msg){
		ostringstream info;
		info << "Coast model : " << 0 << endl;
		msg.make_second_info(info.str());
		if(output_flag==true){
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw msg;
	}
	if(output_flag==true){
		//rewind the prefix
		Sys_Common_Output::output_hyd->rewind_userprefix();
	}
}
//Get the parameters of the coast model
_hyd_coast_params Hyd_Coast_Model::get_coast_parameter(void){
	return this->params;
}
//Output the given members
void Hyd_Coast_Model::output_members(void){
	//set prefix for output
	ostringstream prefix;
	prefix << "CO> ";
	Sys_Common_Output::output_hyd->set_userprefix(prefix.str());

	ostringstream cout;
	cout<<"Output the general parameters..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout);

	// Put out specific information for the model
	cout << "GENERAL   " << endl;
	cout << " Name                         : " << W(7)  <<this->params.model_name << endl;
	cout << " Instationary boundary file   : " << W(7) << Hyd_Param_Global::get_print_filename(37,this->params.filename_instat_boundary.c_str())<< endl;
	cout << " Coastline file               : " << W(7) << Hyd_Param_Global::get_print_filename(37,this->params.filename_coastline.c_str())<< endl;
	cout << "GEOMETRY  " << endl;
	cout << " Geometry folder                : " << W(7) << Hyd_Param_Global::get_print_filename(37, this->params.filename_geometry_tecplot.c_str())<< endl;
	
	Sys_Common_Output::output_hyd->output_txt(&cout);
	//coastline
	cout<<"Output the coastline..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout,true);
	this->coastline_polysegment.output_members();
		
	cout<<"Output the instationary boundary..."<<endl;
	Sys_Common_Output::output_hyd->output_txt(&cout, true);	
	this->instat_boundary_curve.output_member();
	
	//rewind the prefix
	Sys_Common_Output::output_hyd->rewind_userprefix();
}
//Output the geometrie to tecplot
void Hyd_Coast_Model::output_geometrie2tecplot(void){

	if(this->params.filename_geometry_tecplot!=label::not_set){
		ofstream outfile;
		outfile.open(this->get_filename_geometrie2tecplot(hyd_label::tecplot).c_str());
		if(outfile.is_open()==false){
			Error msg=this->set_error(0);
			ostringstream info;
			info << "Filename : " << this->get_filename_geometrie2tecplot(hyd_label::tecplot) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}
		//fileheader
		//output the file header
		outfile << "TITLE = "<< "\" Coastmodel \"" << endl;
		outfile << "VARIABLES = " <<endl;
		outfile << "\" x " << label::m << "\" "<<endl;
		outfile << "\" y " << label::m << "\" "<< endl;
		//output zone header
		outfile << "ZONE T= " << " \" Geometrie \" " <<endl;
		outfile << "I="<<this->coastline_polysegment.get_number_segments()<<", J=1, K=1" << endl;
		outfile << "ZONETYPE=ORDERED" << endl;
		outfile << "DATAPACKING=BLOCK" << endl;
		outfile << "VARLOCATION=([1-2]=NODAL)" << endl;
		outfile << "DT=(SINGLE SINGLE)" << endl;
		outfile << endl;
		int counter_endl=0;
		//x-coordinate
		outfile <<"#x "<< label::m << " NODAL No. " << this->coastline_polysegment.get_number_segments() <<endl;
		for(int i=0; i< this->coastline_polysegment.get_number_segments(); i++){
			if(counter_endl==5){
				counter_endl=0;
				outfile << endl;
			}
			outfile<< P(2)<< FORMAT_FIXED_REAL << this->coastline_polysegment.my_segment[i].point1.get_xcoordinate() << "  ";
			counter_endl++;
		}
		counter_endl=0;
		outfile << endl << endl;
		//y-coordinate
		outfile <<"#y "<< label::m << " NODAL No. " << this->coastline_polysegment.get_number_segments() <<endl;
		for(int i=0; i< this->coastline_polysegment.get_number_segments(); i++){
			if(counter_endl==5){
				counter_endl=0;
				outfile << endl;
			}
			outfile<< P(2)<< FORMAT_FIXED_REAL << this->coastline_polysegment.my_segment[i].point1.get_ycoordinate() << "  ";
			counter_endl++;
		}
		counter_endl=0;
		outfile << endl << endl;
		//geometry
		outfile <<"#Geometry of the Coast model (just a geometry record) " << endl;
		outfile << endl;
		this->coastline_polysegment.output2tecplot(&outfile);
		outfile.close();
	}

}
//Output the geometrie to paraview
void Hyd_Coast_Model::output_geometrie2paraview(void) {
	if (this->params.filename_geometry_tecplot != label::not_set) {
		ofstream outfile;
		outfile.open(this->get_filename_geometrie2paraview(hyd_label::paraview).c_str());
		if (outfile.is_open() == false) {
			Error msg = this->set_error(5);
			ostringstream info;
			info << "Filename : " << this->get_filename_geometrie2tecplot(hyd_label::paraview) << endl;
			msg.make_second_info(info.str());
			throw msg;
		}

		//fileheader
		//output the file header
		outfile << "# vtk DataFile Version 3.0" << endl;
		outfile << "Geometry of coastline " << endl;
		outfile << "ASCII" << endl;
		outfile << "DATASET POLYDATA" << endl;
		outfile << "POINTS " << this->coastline_polysegment.get_number_segments() << " double" << endl;
		for (int i = 0; i < this->coastline_polysegment.get_number_segments(); i++) {
			outfile << this->coastline_polysegment.get_segment(i)->point1.get_xcoordinate() << " ";
			outfile << this->coastline_polysegment.get_segment(i)->point1.get_ycoordinate() << " ";
			outfile << "  0";
			outfile << endl;
		}

		outfile << "LINES " << " 1 " << this->coastline_polysegment.get_number_segments()+1 << endl;
		outfile << this->coastline_polysegment.get_number_segments()<< " ";
		for (int i = 0; i < this->coastline_polysegment.get_number_segments(); i++) {
			outfile << i << " ";
		}
		outfile << endl;


		outfile.close();
	}
}
//Make the syncronisation between the models and the boundaries; here set the sea waterlevel
void Hyd_Coast_Model::make_syncronisation(const double time_point){

	this->global_sea_h=this->instat_boundary_curve.calculate_actuel_boundary_value(time_point);
	

}
//Get the global sea waterlevel
double Hyd_Coast_Model::get_global_sea_h(void){
	return this->global_sea_h;
}
//Get the global sea waterlevel
double *Hyd_Coast_Model::get_ptr_global_sea_h(void){
	return &this->global_sea_h;
}
//Set the system-id (_sys_system_id) of the object
void Hyd_Coast_Model::set_systemid(const _sys_system_id actuel_id){
	_Sys_Common_System::set_systemid(actuel_id);
	this->coastline_polysegment.set_systemid(actuel_id);
	this->instat_boundary_curve.set_systemid(actuel_id);
}
//Set the system-id (_sys_system_id) of the object
void Hyd_Coast_Model::set_hydraulic_bound_sz(Hyd_Boundary_Szenario bound_sz){
	this->hyd_sz=bound_sz;
	this->instat_boundary_curve.hyd_sz.set_members(&bound_sz);
	this->hyd_sz.set_members(&bound_sz);
}
//Clear the boundary condition, which are set
void Hyd_Coast_Model::clear_boundary_condition(void){
	this->instat_boundary_curve.clear_boundary_curve();
}
//Set new boundary condition per database for an new hydraulic boundary scenario; the new boundary scenario has to be specified before
void Hyd_Coast_Model::set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database){
		try{
		//read in the boundary
		QSqlTableModel boundary_result(0, *ptr_database);
		int number=0;
		this->instat_boundary_curve.hyd_sz.set_members(&this->hyd_sz);
		number=this->instat_boundary_curve.select_relevant_curves_database(&boundary_result, 0,_hyd_model_type::COAST_MODEL, output_flag, &this->hyd_sz);
		if(number ==1){
			this->instat_boundary_curve.read_value(&boundary_result, 0);
		}
		else{
			Error msg=this->set_error(2);
			throw msg;

		}

	}
	catch(Error msg){
		ostringstream info;
		info << "Coast model : " << 0 << endl;
		msg.make_second_info(info.str());
		if(output_flag==true){
			//rewind the prefix
			Sys_Common_Output::output_hyd->rewind_userprefix();
		}
		throw msg;
	}

}
//Set the folder for the file-output in case of an database based calculation
void Hyd_Coast_Model::set_output_folder(const string folder){
	this->params.filename_geometry_tecplot=folder;
}
//Clone the members of the coast model
void Hyd_Coast_Model::clone_model(Hyd_Coast_Model *coast){
	this->params.model_name=coast->params.model_name;
	this->instat_boundary_curve.clone_curve(&coast->instat_boundary_curve);
	this->coastline_polysegment.clone_polysegment(&coast->coastline_polysegment);
	//polyline to polygon
	this->polysegment2polygon();
}
//Get the crude version of the filename for 2d output
string Hyd_Coast_Model::get_crude_filename_result_1d(void){
	return this->params.filename_geometry_tecplot;
}
//_______________
//private
//Make of the Hyd_Floodplain_Polysegment a Hyd_Floodplain_Polygon
void Hyd_Coast_Model::polysegment2polygon(void){

	//it is always closed=>number segments=number of points
	this->coast_polygon.set_number_points(this->coastline_polysegment.get_number_segments());

	Geo_Point *buffer=NULL;
	try{
		buffer=new Geo_Point[this->coastline_polysegment.get_number_segments()];
	}
	catch(bad_alloc &){
		Error msg=this->set_error(1);
		throw msg;
	}

	//polysegments points into buffer
	for(int i=0; i< this->coastline_polysegment.get_number_segments(); i++){
		buffer[i]=this->coastline_polysegment.my_segment[i].point1;
	}
	//buffer to polygon
	this->coast_polygon.set_points(buffer);
	
	if(buffer!=NULL){
		delete []buffer;
		buffer=NULL;
	}

}
//Get the filename for the floodplain geometrie file
string Hyd_Coast_Model::get_filename_geometrie2tecplot(const string type){
	string buffer;
	buffer=this->params.filename_geometry_tecplot;
	
	if(buffer!=label::not_set){
		stringstream suffix;
		suffix << "CO_GEO.dat";
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Get the filename for the floodplain geometrie file
string Hyd_Coast_Model::get_filename_geometrie2paraview(const string type) {
	string buffer;
	buffer = this->params.filename_geometry_tecplot;

	if (buffer != label::not_set) {
		stringstream suffix;
		suffix << "CO_GEO.vtk";
		buffer = functions::make_complete_output_path(buffer, type, suffix.str());
	}
	return buffer;
}
//Complete the filenames with the global path
void Hyd_Coast_Model::complete_filenames_with_path(string global_path){
	ostringstream buff;
	buff<<global_path<<this->params.filename_coastline;
	this->params.filename_coastline=_Hyd_Parse_IO::insert_linux_slash2string(buff.str());
	buff.str("");
	buff.clear();

	buff<<global_path<<this->params.filename_instat_boundary;
	this->params.filename_instat_boundary=_Hyd_Parse_IO::insert_linux_slash2string(buff.str());
	buff.str("");
	buff.clear();

	if(this->params.filename_geometry_tecplot!=label::not_set){
		buff<<global_path<<this->params.filename_geometry_tecplot;
		this->params.filename_geometry_tecplot=_Hyd_Parse_IO::insert_linux_slash2string(buff.str());
		buff.str("");
		buff.clear();

	}
}
//Set error(s)
Error Hyd_Coast_Model::set_error(const int err_type){
	string place="Hyd_Coast_Model::";
		string help;
		string reason;
		int type=0;
		bool fatal=false;
		stringstream info;
		Error msg;

	switch (err_type){
		case 0://can not open file
			place.append("output_geometrie2tecplot(void)");
			reason="Can not open file";
			help="Check the file";
			type=5;
			break;
		case 1://can not allocate the point buffer
			place.append("polysegment2polygon(void)");
			reason="Can not allocate the memory";
			help="Check the memory";
			type=10;
			break;
		case 2://there are multiple boundary curves are found 
			place.append("input_members(const QSqlTableModel *query_result, const bool output_flag)");
			reason="Multiple coast boundary curves are found; just one is possible";
			help="Check the coast boundaries in table";
			type=2;
			break;
		case 3://there are multiple boundary curves are found 
			place.append("set_new_boundary_condition(const bool output_flag, QSqlDatabase *ptr_database)");
			reason="There are multiple coast boundary curves are found; just one is possible";
			help="Check the coast boundaries in table";
			type=2;
			break;
		case 4://there are multiple boundary curves  found 
			place.append("input_members(const QSqlTableModel *query_result, const bool output_flag)");
			reason="No coast boundary curve is found";
			help="Check the coast boundaries in table";
			type=2;
			break;
		case 5://can not open file
			place.append("output_geometrie2paraview(void)");
			reason = "Can not open file";
			help = "Check the file";
			type = 5;
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