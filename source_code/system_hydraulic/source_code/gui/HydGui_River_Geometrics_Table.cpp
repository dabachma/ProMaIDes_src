//#include "HydGui_River_Geometrics_Table.h"
#include "Hyd_Headers_Precompiled.h"

//default constructor
HydGui_River_Geometrics_Table::HydGui_River_Geometrics_Table(QWidget *parent) : QTableView(parent), columnCount(6) {

	this->model = new QStandardItemModel(0, this->columnCount);
	this->setModel(model);
    this->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	this->rowCount = 0;

	QStringList hhLabels;
	hhLabels << tr("global x [m]") << tr("global y [m]") << tr("global geodetic height [m]") 
			 << tr("material id [-]") << tr("distance between points [m]") 
			 << tr("channel type [-]");

	this->model->setHorizontalHeaderLabels(hhLabels);

	this->setColumnWidth(0, 83);
	this->setColumnWidth(1, 83);
	this->setColumnWidth(2, 150);
	this->setColumnWidth(3, 95);
	this->setColumnWidth(4, 150);
	this->setColumnWidth(5, 110);

	this->cmbDelegate = new Sys_ComboBox_Delegate(this);
	
	QStringList items;
	items << hyd_label::channel_left.c_str() << hyd_label::channel_main.c_str() << hyd_label::channel_right.c_str();
	this->cmbDelegate->set_string_list(items);

	this->intDelegate = new Sys_SpinBox_Delegate(this);
	this->dblDelegate = new Sys_DoubleSpinBox_Delegate(this);

	this->setItemDelegateForColumn(0, dblDelegate);
	this->setItemDelegateForColumn(1, dblDelegate);
	this->setItemDelegateForColumn(2, dblDelegate);
	this->setItemDelegateForColumn(3, intDelegate);
	this->setItemDelegateForColumn(4, dblDelegate);
	this->setItemDelegateForColumn(5, cmbDelegate);

	this->setShowGrid(true);

	QObject::connect(this, SIGNAL(clicked(const QModelIndex&)), this, SLOT(selection_changed(const QModelIndex&)));

	//count the memory
	Sys_Memory_Count::self()->add_mem(sizeof(HydGui_River_Geometrics_Table), _sys_system_modules::HYD_SYS);
}

//default destructor
HydGui_River_Geometrics_Table::~HydGui_River_Geometrics_Table(void) {
	Sys_Memory_Count::self()->minus_mem(sizeof(HydGui_River_Geometrics_Table), _sys_system_modules::HYD_SYS);
}

//______________
//public methods
//Set the number of rows, should do that at the beginning, before rows are filled
void HydGui_River_Geometrics_Table::set_rows(int rows) {
	this->model->setRowCount(rows);
	this->rowCount = rows;
}
//Remove a certain row, characterized by the given index
void HydGui_River_Geometrics_Table::remove_row(int row_index) {
	if (this->rowCount > 0) {
		this->model->removeRow(row_index);
		this->rowCount = this->rowCount - 1;
	}
}
//Add a point (fill a row) to the table, if row doesent exist, a new row is beeing added (attention: index could differ then)
void HydGui_River_Geometrics_Table::set_point_data(int row, double global_x, double global_y, double global_geo_height, int material_id, double dist_between_points, int channel_type) {
	
	if (row <= this->rowCount && row >= 0) {
		this->model->setData(this->model->index(row, 0, QModelIndex()), global_x);
		this->model->setData(this->model->index(row, 1, QModelIndex()), global_y);
		this->model->setData(this->model->index(row, 2, QModelIndex()), global_geo_height);
		this->model->setData(this->model->index(row, 3, QModelIndex()), material_id);
		this->model->setData(this->model->index(row, 4, QModelIndex()), dist_between_points);
		
		this->model->setData(this->model->index(row, 5, QModelIndex()), _Hyd_River_Profile_Type::channel_type2text(channel_type).c_str());
	}
	else {
		this->add_row();
		
		this->model->setData(this->model->index(this->rowCount, 0, QModelIndex()), global_x);
		this->model->setData(this->model->index(this->rowCount, 1, QModelIndex()), global_y);
		this->model->setData(this->model->index(this->rowCount, 2, QModelIndex()), global_geo_height);
		this->model->setData(this->model->index(this->rowCount, 3, QModelIndex()), material_id);
		this->model->setData(this->model->index(this->rowCount, 4, QModelIndex()), dist_between_points);
		this->model->setData(this->model->index(row, 5, QModelIndex()), _Hyd_River_Profile_Type::channel_type2text(channel_type).c_str());

		//this->model->setData(this->model->index(this->rowCount, 5, QModelIndex()), channel_type);
	}
}
//Get the values of the specified row
void HydGui_River_Geometrics_Table::get_point_data(int ) {
	//if (row >= 0 && row <= this->rowCount) {
	//	double global_x = this->model->data(this->model->index(row, 0, QModelIndex())).toDouble();
	//	double global_y = this->model->data(this->model->index(row, 1, QModelIndex())).toDouble();
	//	double global_z = this->model->data(this->model->index(row, 2, QModelIndex())).toDouble();
	//	int material_id = this->model->data(this->model->index(row, 3, QModelIndex())).toInt();
	//	double dist_btwn_points = this->model->data(this->model->index(row, 4, QModelIndex())).toDouble();
	//	int channel_type = this->model->data(this->model->index(row, 5, QModelIndex())).toInt();
	//}
}
//____________
//public slots
//Append a row at the end of the table, connected to add_row button
void HydGui_River_Geometrics_Table::add_row(void) {
	this->model->insertRow(this->rowCount);
	this->rowCount++;
}
//Remove the selected line from the table, connected to remove_row button
void HydGui_River_Geometrics_Table::remove_row(void) {
	if (this->rowCount > 0) {
		this->model->removeRow(this->current_index.row());
		this->rowCount = this->rowCount - 1;
	}
}
//Set the current selected line
void HydGui_River_Geometrics_Table::selection_changed(const QModelIndex &index) {
	this->current_index = index;
}
