#include "Sys_Headers_Precompiled.h"
//sys_libs
#include "Common_Const.h"
using namespace std;



//Signum function
double functions::signum_func(const double value){
	if(value>0.0){
		return 1.0;
	}
	else if(value <0.0){
		return -1.0;
	}
	else{
		return 0.0;
	}
}

//Convert seconds into a string day:hour:minute:second
string functions::convert_seconds2string(const double seconds){
	stringstream time_str;
	int second_buff=(int) seconds;
	int day = (int) (second_buff/constant::day_second);
	second_buff = second_buff % constant::day_second;
	int hour = (int) (second_buff / constant::hour_second);
	second_buff= second_buff % constant::hour_second;
	int minutes = (int) (second_buff / constant::minute_second);
	int sec =  second_buff % constant::minute_second;
	time_str << W(2) << FI('0') << day <<":"<< W(2) << FI('0')<<hour<<":"<< W(2) << FI('0')<<minutes<<":"<< W(2) << FI('0')<<sec;
	return time_str.str();
}
//Convert seconds into a string year/month/day hour:minute:second
string functions::convert_seconds2datestring(const double seconds){
	
	struct tm *timest;
	time_t t=seconds;
	timest=localtime(&t);
	string t_str;
	stringstream buffer;
	buffer<<"\"19"<<timest->tm_year<<"/";
	if(timest->tm_mon+1<10){
		buffer<<0<<timest->tm_mon+1<<"/";
	}
	else{
		buffer<<timest->tm_mon+1<<"/";
	}
	if(timest->tm_mday<10){
		buffer<<0<<timest->tm_mday<<"";
	}
	else{
		buffer<<timest->tm_mday<<"";
	}
	
		
	buffer<<" "<<timest->tm_hour<<":";
	if(timest->tm_min<10){
		buffer<<0<<timest->tm_min<<":";
	}
	else{
		buffer<<timest->tm_min<<":";
	}
	if(timest->tm_sec<10){
		buffer<<0<<timest->tm_sec<<"";
	}
	else{
		buffer<<timest->tm_sec<<"";
	}
	buffer<<"\"";
	t_str.append(buffer.str());
	return t_str;
}
//Convert seconds into a structure tm from ctime year/month/day hour:minute:second
void functions::convert_seconds2datestruct(const double seconds, tm *t_struct){


    time_t t=seconds;
    *t_struct=*localtime(&t);

}
//Convert time to time string
string functions::convert_time2time_str(const double seconds) {
	string time;
	stringstream buff_t;
	tm time_struct;

	functions::convert_seconds2datestruct(seconds, &time_struct);
	if (time_struct.tm_year >= 100) {
		buff_t << "'20";
		if (time_struct.tm_year - 100 < 10) {
			buff_t << "0"<< time_struct.tm_year - 100;
		}
		else {
			buff_t << time_struct.tm_year - 100;
		}
	}
	else{
		buff_t << "'19" << time_struct.tm_year;
	}
	buff_t  << "-" << setw(2) << setfill('0') << time_struct.tm_mon + 1;
	buff_t << "-" << setw(2) << setfill('0') << time_struct.tm_mday << " ";
	buff_t << setw(2) << setfill('0') << time_struct.tm_hour << ":";
	buff_t << setw(2) << setfill('0') << time_struct.tm_min << ":" << setw(2) << setfill('0') << time_struct.tm_sec << "'";
	time = buff_t.str();
	return time;

}
//Convert time to time string without '
string functions::convert_time2time_str_without(const double seconds) {
	string time;
	stringstream buff_t;
	tm time_struct;

	functions::convert_seconds2datestruct(seconds, &time_struct);
	if (time_struct.tm_year >= 100) {
		buff_t << "20";
		if (time_struct.tm_year - 100 < 10) {
			buff_t << "0" << time_struct.tm_year - 100;
		}
		else {
			buff_t << time_struct.tm_year - 100;
		}
	}
	else {
		buff_t << "19" << time_struct.tm_year;
	}
	buff_t << "-" << setw(2) << setfill('0') << time_struct.tm_mon + 1;
	buff_t << "-" << setw(2) << setfill('0') << time_struct.tm_mday << " ";
	buff_t << setw(2) << setfill('0') << time_struct.tm_hour << ":";
	buff_t << setw(2) << setfill('0') << time_struct.tm_min << ":" << setw(2) << setfill('0') << time_struct.tm_sec << "";
	time = buff_t.str();
	return time;

}
//Function to encrypt/decrypt the password with XOR
string functions::crypter(string value,string key){
    string retval(value);

    short unsigned int klen=key.length();
    short unsigned int vlen=value.length();
    short unsigned int k=0;
    short unsigned int v=0;
    
    for(v=0;v<vlen;v++)
    {
        retval[v]=value[v]^key[k];
        k=(++k<klen?k:0);
    }
    
    return retval;
}
//Convert a byte value (double) into a string with units
string functions::convert_byte2string(const long long int mem_bytes){
	ostringstream mem_str;

	mem_str<<FORMAT_FIXED_REAL<<P(0);

	if(mem_bytes <= constant::kbyte_size){
		mem_str<< mem_bytes << label::byte << endl;	
	}
	else if( mem_bytes > constant::kbyte_size && mem_bytes <= constant::mbyte_size){
        mem_str<<(double)(mem_bytes/constant::kbyte_size) << label::kbyte<< endl;
	}
	else if( mem_bytes >constant::mbyte_size &&  mem_bytes <= constant::gbyte_size){
        mem_str <<FORMAT_FIXED_REAL<<P(1)<<(double)(mem_bytes/constant::mbyte_size) << label::mbyte << endl;
	}
	else if( mem_bytes >constant::gbyte_size ){
        mem_str <<FORMAT_FIXED_REAL<<P(2)<<(double)(mem_bytes/constant::gbyte_size) << label::gbyte << endl;
	}
	return mem_str.str();

}
//Check a double-value for infinite
bool functions::check_infinite(const double value){
	//check for ind
	if(value!=value){
		return true;
	}
	//check for inf
	if(value==std::numeric_limits<double>::infinity()){
		return true;
	}


	return false;
}
//Delete a comment "#", leading and ending tabs/white spaces of a given string
void functions::clean_string(string *txt){

	//delete the comments with #
	int pos=txt->find("#",0);
	if(pos>=0){
		txt->erase(pos);
	}
	//erase leading whitespace and tabs
	char one_char[1];
	int length=txt->length();

	for (int i=0; i< length;i++){
		txt->copy(one_char, 1, 0);
		if(one_char[0]==' ' || one_char[0]=='\t'){
			txt->erase(0,1);
		}
		else{
			break;
		}
	}

	//erase ending whitespace, tabs, \r and \n
	if(txt->empty()==true){
		return;
	}
	length=txt->length();
	int new_length=length;
	
	do{ 
		//my_string->copy(one_char, 1, new_length-1);
		if(txt->at(new_length-1)==' ' || txt->at(new_length-1)=='\t' || txt->at(new_length-1)=='\r'|| txt->at(new_length-1)=='\n'){
			txt->erase(new_length-1, 1);
			new_length=txt->length();
		}
		else{
			break;
		}
	}
	while(txt->empty()==false);
}
//Clean all white space
string functions::clean_white_space(string *txt) {
	string buff= *txt;
	buff.erase(std::remove_if(buff.begin(), buff.end(), ::isspace), buff.end());

	return buff;
}
//Convert all char's in a string to the lower case
string functions::convert_string2lower_case(string *txt){

	int length = txt->length();
	for(int i=0; i<length; ++i){
		(*txt)[i] = tolower((*txt)[i]);
	}
	return *txt;
}
//Convert all char's in a string to the upper case
string functions::convert_string2lupper_case(string *txt){

	int length = txt->length();
	for(int i=0; i<length; ++i){
		(*txt)[i] = toupper((*txt)[i]);
	}

	return *txt;
}
//Convert all char's in a string to the lower case
string functions::convert_string2lower_case(string txt){

	int length = txt.length();
	for(int i=0; i<length; ++i){
		(txt)[i] = tolower((txt)[i]);
	}
	return txt;
}
//Convert all char's in a string to the upper case
string functions::convert_string2lupper_case(string txt){

	int length = txt.length();
	for(int i=0; i<length; ++i){
		(txt)[i] = toupper((txt)[i]);
	}

	return txt;
}
//Count the number of columns in a line; delimiters are tabs and whitepsaces
int functions::count_number_columns(string txt){
	int columns=1;
	unsigned int counter=0;
	bool txt_flag=true;
	if(txt.empty()==true){
		return 0;
	}

	do{
		
		if(txt.at(counter)=='\t' || txt.at(counter)==' '){
			if(txt_flag==true){
				txt_flag=false;
				columns++;
			}
		}
		else{
			txt_flag=true;

		}
		counter++;
	}
	while(counter<txt.length());

	return columns;
}
//Get the first column, erase it from the given string and return it
string functions::get_column(string *line){
	string buffer=*line;
	unsigned int counter=0;
	bool first_found=false;
	int pos_begin=-1;
	int pos_end=line->length();

	if(line->empty()==true){
		return buffer;
	}
	do{
		
		if(line->at(counter)=='\t' || line->at(counter)==' '){	
			if(first_found==true){
				pos_end=counter;
				break;
			}
		}
		else{
			if(first_found==false){
				first_found=true;
				pos_begin=counter;
			}
		}
		counter++;
	}
	while(counter<line->length());

	if(first_found==true){
		buffer=line->substr(pos_begin, pos_end-pos_begin);
		line->erase(pos_begin, pos_end-pos_begin);
	}


	return buffer;
}
//Convert a string to a boolean
bool functions::convert_string2boolean(const string my_string){
	string buff=my_string;
	functions::clean_string(&buff);
	functions::convert_string2lower_case(&buff);
	if(buff=="false"){
		return false;
	}
	else if (buff=="true"){
		return true;
	}
	else{
		Error msg;
		msg.set_msg("functions::convert_string2boolean(const string my_string)", "Can not convert this boolean type", "Check the given type", 1, false);
		ostringstream info;
		info << "Try to convert boolean type: " << my_string << endl;
		info << "Possible types: "<< endl;
		info << " true"  << endl;
		info << " false"  << endl;
		msg.make_second_info(info.str());
		throw msg;
	}

}
//Convert a boolean to a string
string functions::convert_boolean2string(const bool flag){
	string buff;
	if(flag==true){
		buff="true";
	}
	else{
		buff="false";
	}
	return buff;

}
//Get the file path without the file name
string functions::get_file_path(const string path_file){
	string buff=path_file;
	int pos1=0;
	int pos2=0;

	pos1=path_file.find_last_of("/");
	pos2=path_file.find_last_of("\\");
	if(pos1<0 && pos2<0){
		return buff;
	}
	if(pos1>pos2){
		buff=buff.substr(0, pos1);
	}
	else{
		buff=buff.substr(0, pos2);
	}
	return buff;
}
//Get the file name without the file path
string functions::get_file_name(const string path_file){
	string buff=path_file;
	int pos1=0;
	int pos2=0;

	pos1=path_file.find_last_of("/");
	pos2=path_file.find_last_of("\\");
	if(pos1<0 && pos2<0){
		return buff;
	}
	if(pos1>pos2){
		buff=buff.substr(pos1+1, buff.length());
	}
	else{
		buff=buff.substr(pos2+1, buff.length());
	}
	return buff;
}
//Remove the suffix of a file name (e.g. .dat)
string functions::remove_suffix_file_name(const string file_name){
	string buff=file_name;
	int pos1=0;

	pos1=file_name.find_last_of(".");
	if(pos1>0){
		buff=buff.substr(0, pos1);
	}
	return buff;
}
//Convert the system module enumerator (_sys_system_modules) to a string 
string functions::convert_system_module2txt(_sys_system_modules type){
	string buffer;
	switch (type){
		case _sys_system_modules::HYD_SYS:
			buffer=sys_label::str_hyd;
			break;
		case _sys_system_modules::DAM_SYS:
			buffer=sys_label::str_dam;
			break;
		case _sys_system_modules::FPL_SYS:
			buffer=sys_label::str_fpl;
			break;
		case _sys_system_modules::SYS_SYS:
			buffer=sys_label::str_sys;
			break;
		case _sys_system_modules::EXC_SYS:
			buffer=sys_label::str_excep;
			break;
		case _sys_system_modules::MADM_SYS:
			buffer=sys_label::str_madm;
			break;
		case _sys_system_modules::RISK_SYS:
			buffer=sys_label::str_risk;
			break;
		case _sys_system_modules::ALT_SYS:
			buffer=sys_label::str_alt;
			break;
		case _sys_system_modules::COST_SYS:
			buffer=sys_label::str_cost;
			break;
		case _sys_system_modules::GEOSYS_SYS:
			buffer=sys_label::str_geosys;
			break;
		default:
			buffer=label::not_defined;
	}

	return buffer;
}
//Convert a string  to the system module enumerator (_sys_system_modules) 
_sys_system_modules functions::convert_txt2system_module2(const string txt){
	_sys_system_modules type;
	if(txt==sys_label::str_hyd){
		type=_sys_system_modules::HYD_SYS;
	}
	else if(txt==sys_label::str_geosys){
		type=_sys_system_modules::GEOSYS_SYS;
	}
	else if(txt==sys_label::str_dam){
		type=_sys_system_modules::DAM_SYS;
	}
	else if(txt==sys_label::str_fpl){
		type=_sys_system_modules::FPL_SYS;
	}
	else if(txt==sys_label::str_madm){
		type=_sys_system_modules::MADM_SYS;
	}
	else if(txt==sys_label::str_sys){
		type=_sys_system_modules::SYS_SYS;
	}
	else if(txt==sys_label::str_risk){
		type=_sys_system_modules::RISK_SYS;
	}
	else if(txt==sys_label::str_alt){
		type=_sys_system_modules::ALT_SYS;
	}
	else if(txt==sys_label::str_cost){
		type=_sys_system_modules::COST_SYS;
	}
	else{
		type=_sys_system_modules::EXC_SYS;
	}

	return type;
}
//Decide if a line of file consists of a !$BEGIN (return true) or !$END-keyword (return false). No keyword found=> empty string
bool functions::decide_begin_end_keyword(QString *type){
	if(type->contains("!$BEGIN")==true){
		QStringList list;
		list=type->split("_");
		*type="";
		for(int i=1; i< list.count(); i++){
			type->append(list.at(i));
		}

		return true;
	}
	else if(type->contains("!$END")==true){
		QStringList list;
		list=type->split("_");
		*type="";
		for(int i=1; i< list.count(); i++){
			type->append(list.at(i));
		}
		return false;
	}
	else{
		*type="";
		return true;
	}	
}
//Set flags for table widgets (static)
void functions::set_table_widget_flags(QTableWidget * widget){

    Qt::ItemFlags my_flag;
    my_flag=my_flag|(Qt::ItemIsEnabled);
    my_flag=my_flag|(Qt::ItemIsSelectable);
    my_flag=my_flag|(Qt::ItemIsDragEnabled);
    for(int i=0; i<widget->rowCount(); i++){
        for(int j=0; j<widget->columnCount(); j++){
            QTableWidgetItem *buff=widget->item(i,j);
            if(buff!=NULL){
                buff->setFlags(my_flag);
            }
        }
    }
}
//Make output path complete
string functions::make_complete_output_path(const string path, const string folder, const string name) {
	string my_str;
	my_str = path;
	my_str += "/";
	my_str += folder;
	my_str += "/";
	my_str += name;

	return my_str;
}
//Add seperator for csv-output
void functions::add_seperator_csv(const string sep, ofstream *output, const int number) {
	for (int i = 0; i < number; i++) {
		*output << sep;
	}

}

