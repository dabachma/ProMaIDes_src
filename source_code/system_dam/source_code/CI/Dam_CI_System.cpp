#include "source_code\Dam_Headers_Precompiled.h"
#include "Dam_CI_System.h"

//Default constructor
Dam_CI_System::Dam_CI_System(void){

	this->no_ci_point = 0;
	this->no_ci_polygon = 0;
	this->file_ci_point = label::not_set;
	this->file_ci_polygon = label::not_set;
	this->file_ci_connection = label::not_set;
	this->dam_ci_polygon = NULL; 
	this->dam_ci_point = NULL;

	this->del_data_flag = false;
	this->del_res_flag = false;

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(Dam_CI_System), _sys_system_modules::DAM_SYS);
	
}
//Default destructor
Dam_CI_System::~Dam_CI_System(void){
	this->delete_CI_point();
	this->delete_CI_polygon();
	//count the memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Dam_CI_System), _sys_system_modules::DAM_SYS);
}
//__________
//public
//Import CI data from a file to the database
void Dam_CI_System::ci_data_file2database(QSqlDatabase *ptr_database) {



}
//Delete all CI data in the database: points, polygons, connections, results etc.
void Dam_CI_System::del_ci_data_database(QSqlDatabase *ptr_database) {

}
//Ask for the file of the CI data per dialog
bool Dam_CI_System::ask_file_CI_data(QWidget *parent) {
	/** If the return-value is true, the dialog is accepted. "False" as return
	value, means it is canceled. The result is set to the member file_category_func */
	Sys_Multi_Filechooser_Dia my_dia(parent);

	//set up dialog
	QIcon icon;
	icon.addFile(":ci_icon");
	my_dia.set_number_file_browser(3, icon);
	stringstream buffer;
	buffer << "Choose the files, where the CI data are stored." << endl;
	buffer << "This files contain the definition of the CI-points, CI-polygons and their connections." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose CI-data files");
	my_dia.get_ptr_file_browser(0)->set_text_label("CI-point file");
	my_dia.get_ptr_file_browser(1)->set_text_label("CI-polygon file");
	my_dia.get_ptr_file_browser(2)->set_text_label("CI-connection file");

	//start dialog
	if (my_dia.start_dialog() == false) {
		return false;
	}
	else {
		//points
		this->file_ci_point = my_dia.get_ptr_file_browser(0)->get_file_name();

		if (this->file_ci_point == label::not_set) {
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt << "No file is set for the CI-points!" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		//polygons
		this->file_ci_polygon = my_dia.get_ptr_file_browser(1)->get_file_name();

		if (this->file_ci_polygon == label::not_set) {
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt << "No file is set for the CI-polygons!" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		//connections
		this->file_ci_connection = my_dia.get_ptr_file_browser(2)->get_file_name();

		if (file_ci_connection == label::not_set) {
			Sys_Diverse_Text_Dia dialog2(true);
			QIcon icon;
			icon.addFile(":dam_icon");
			dialog2.set_window_icon(icon);
			ostringstream txt;
			txt << "No file is set for the CI-connections!" << endl;
			dialog2.set_dialog_question(txt.str());
			dialog2.start_dialog();
			return false;
		}

		return true;
	}
}
//Ask for the flag, what should be deleted (points, subcategories)
bool Dam_CI_System::ask_deleting_flag(QWidget *parent) {
	Sys_Multi_CheckBox_Dia my_dia(parent);
	//set up dialog
	QIcon icon;
	icon.addFile(":ci_icon");
	my_dia.set_number_check_boxes(2, icon);
	stringstream buffer;
	buffer << "Choose what should be deleted in the CI-system in the database..." << endl;
	my_dia.set_main_text_label(buffer.str());
	buffer.str("");
	my_dia.set_window_title("Choose deleting CI-system");
	my_dia.get_ptr_check_box(0)->setText("Delete CI-data (points, polygons and connections)");
	my_dia.get_ptr_check_box(1)->setText("Delete CI-results");

	//start dialog
	if (my_dia.start_dialog() == false) {
		return false;
	}
	else {
		this->del_data_flag = my_dia.get_bool_check_box(0);
		this->del_res_flag = my_dia.get_bool_check_box(1);
		return true;
	}
}
//Get a text for deleting CI information in database
string Dam_CI_System::get_deleting_text(void) {
	ostringstream buffer;
	if (this->del_data_flag == true) {
		buffer << " The CI data will be deleted in database, also the result data!" << endl;
	}
	if (this->del_res_flag == true) {
		buffer << " All CI result data will be deleted in database!" << endl;
	}


	return buffer.str();
}
//Output the members
void Dam_CI_System::output_members(void) {
	ostringstream prefix;
	prefix << "CI> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());

	ostringstream cout;

	//if (this->number_subcategories > 0) {
	//	cout << "Output " << this->number_subcategories << " subcategory(s) of the simple counting damage..." << endl;
	//	cout << " Id " << "     " << "Name " << "   " << endl;
	//	Sys_Common_Output::output_dam->output_txt(&cout);
	//	for (int i = 0; i < this->number_subcategories; i++) {
	//		this->subcategories[i].output_members(&cout);
	//		Sys_Common_Output::output_dam->output_txt(&cout);
	//	}
	//}

	cout << "Output the points of the simple counting damage..." << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);
	//this->point_manager.output_member();

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//Output statistic of CI system
void Dam_CI_System::output_statistic(void) {
	ostringstream prefix;
	prefix << "CI> ";
	Sys_Common_Output::output_dam->set_userprefix(prefix.str());
	ostringstream cout;
	cout << "CI POINTS" << endl;
	//cout << " Number             " << this->point_manager.get_number_points() << endl;
	cout << "CI POLYGONS" << endl;
	//cout << " Number             " << this->number_subcategories << endl;
	cout << "CI CONNECTIONS" << endl;
	//cout << " Number             " << this->number_subcategories << endl;
	Sys_Common_Output::output_dam->output_txt(&cout);

	Sys_Common_Output::output_dam->rewind_userprefix();
}
//____________
//private
//Allocate the CI points
void Dam_CI_System::allocate_CI_point(void) {
	try {
		this->dam_ci_point = new Dam_CI_Point[this->no_ci_point];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(0);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}
}
//Delete the CI points
void Dam_CI_System::delete_CI_point(void) {
	if (this->dam_ci_point != NULL) {
		delete[]this->dam_ci_point;
		this->dam_ci_point = NULL;
	}
}
//Allocate the CI polygon
void Dam_CI_System::allocate_CI_polygon(void) {
	try {
		this->dam_ci_polygon = new Dam_CI_Polygon[this->no_ci_polygon];
	}
	catch (bad_alloc &t) {
		Error msg = this->set_error(1);
		ostringstream info;
		info << "Info bad alloc: " << t.what() << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Delete the CI polygon
void Dam_CI_System::delete_CI_polygon(void) {
	if (this->dam_ci_polygon != NULL) {
		delete[]this->dam_ci_polygon;
		this->dam_ci_polygon = NULL;
	}
}
//Set error(s)
Error Dam_CI_System::set_error(const int err_type) {
	string place = "Dam_CI_System::";
	string help;
	string reason;
	int type = 0;
	bool fatal = false;
	stringstream info;
	Error msg;

	switch (err_type) {
	case 0://bad alloc
		place.append("allocate_CI_point(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	case 1://bad alloc
		place.append("allocate_CI_polygon(void)");
		reason = "Can not allocate the memory";
		help = "Check the memory";
		type = 10;
		break;
	//case 1://not all files are set
	//	place.append("add_damage_points_file2database(QSqlDatabase *ptr_database)");
	//	reason = "The required simpe counting point file is not set. The points are not transferd to database";
	//	help = "Set the file correctly";
	//	type = 1;
	//	break;
	//case 2://can not open file
	//	place.append("read_sc_subcategory_per_file(const string file)");
	//	reason = "Can not open file";
	//	help = "Check the file";
	//	type = 5;
	//	break;
	//case 3://not all data are found
	//	place.append("read_sc_subcategory_per_file(const string file)");
	//	reason = "Not all data are found";
	//	help = "Check the file";
	//	type = 5;
	//	break;
	//case 4://wrong input sequenze
	//	place.append("read_sc_subcategory_per_file(const string file)");
	//	reason = "Wrong input by reading the number of subcategories";
	//	help = "Check the file";
	//	type = 5;
	//case 5://not all info there
	//	place.append("read_sc_subcategory_per_file(const string file)");
	//	reason = "Not all relevant subcategory data are found in file";
	//	help = "Check the point data in file";
	//	type = 5;
	//	break;
	//case 6://can not read in the x-ycoordinates properly
	//	place.append("read_sc_subcategory_per_file(const string file)");
	//	reason = "Can not read in the id and name of the subcategory properly";
	//	help = "Check the coordinates and other data in file";
	//	type = 5;
	//	break;
	//case 7://not all points are found
	//	place.append("read_sc_subcategory_per_file(const string file)");
	//	reason = "Not all subcategories are found";
	//	help = "Check the number of subcategories and the given subcategories";
	//	info << "Number of searched subcategories : " << this->number_subcategories << endl;
	//	type = 5;
	//	break;
	default:
		place.append("set_error(const int err_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 6;
	}
	msg.set_msg(place, reason, help, type, fatal);
	msg.make_second_info(info.str());
	return msg;
}
//Set warning(s)
Warning Dam_CI_System::set_warning(const int warn_type) {
	string place = "Dam_CI_System::";
	string help;
	string reason;
	string reaction;
	int type = 0;
	Warning msg;
	stringstream info;

	switch (warn_type) {
	//case 0://no !END-flag is found in file
	//	place.append("read_sc_subcategory_per_file(const string file)");
	//	reason = "The subcategory data block is not closed with !END in file";
	//	help = "Check the file and the number of subcategory specification in file";
	//	type = 1;
	//	break;
	//case 1://not all points are connected
	//	place.append("check_system(void)");
	//	reason = "Not all points are connected to the hydraulic system";
	//	help = "Reconnect the simple counting system";
	//	type = 1;
	//	break;
	default:
		place.append("set_warning(const int warn_type)");
		reason = "Unknown flag!";
		help = "Check the flags";
		type = 5;
	}
	msg.set_msg(place, reason, help, reaction, type);
	msg.make_second_info(info.str());
	return msg;
}