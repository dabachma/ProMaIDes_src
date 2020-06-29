//#include "Sys_Elastic_GraphicsTreeObject.h"
#include "Sys_Headers_Precompiled.h"

qreal Sys_Elastic_GraphicsTreeObject::dockingPointOffset = 0.0;

Sys_Elastic_GraphicsTreeObject::Sys_Elastic_GraphicsTreeObject(Sys_Elastic_GraphicsTreeObject *topLevelObject, QGraphicsScene *scene, QObject *parent) 
	: Sys_Elastic_GraphicsObject(parent)
{
	this->topLevelObject = topLevelObject;
	this->scene = scene;
	this->setFlag(ItemIsMovable);

	this->state = true;
	this->enabled = true;

	ADD_MEM(Sys_Elastic_GraphicsTreeObject, _sys_system_modules::SYS_SYS);
}
//
Sys_Elastic_GraphicsTreeObject::Sys_Elastic_GraphicsTreeObject(QGraphicsScene *scene, QObject *parent) 
	: Sys_Elastic_GraphicsObject(parent)
{
	this->scene = scene;
	this->setFlag(ItemIsMovable);

	this->state = true;
	this->enabled = true;

	ADD_MEM(Sys_Elastic_GraphicsTreeObject, _sys_system_modules::SYS_SYS);
}
//
Sys_Elastic_GraphicsTreeObject::~Sys_Elastic_GraphicsTreeObject(void)
{
	MIN_MEM(Sys_Elastic_GraphicsTreeObject, _sys_system_modules::SYS_SYS);
}
//
bool Sys_Elastic_GraphicsTreeObject::add_bottomLevel_object(Sys_Elastic_GraphicsTreeObject *bottomLevelObject) 
{
    bool success = !(bottomLevelObjectList.contains(bottomLevelObject));
	if (success) 
	{
		bottomLevelObjectList.append(bottomLevelObject);
		connect(bottomLevelObject, SIGNAL(state_changed(const bool)), this, SLOT(on_bottomLevel_state_changed(const bool)));

		Sys_Elastic_GraphicsEdge *edge = new Sys_Elastic_GraphicsEdge(this, bottomLevelObject);
		connect(bottomLevelObject, SIGNAL(state_changed(const bool)), edge, SLOT(on_tree_object_state_changed(const bool)));
		edge->setZValue(1);
		edge->adjust();
		this->scene->addItem(edge);

		update();
	}
    return success;
}
//
bool Sys_Elastic_GraphicsTreeObject::add_edge(Sys_Elastic_GraphicsEdge *edge) {
	if (this->edgeList.contains(edge))
		return false;

	this->edgeList.append(edge);
	return true;
}
//
QVariant Sys_Elastic_GraphicsTreeObject::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
	switch(change) 
	{
	case ItemPositionHasChanged:
		foreach (Sys_Elastic_GraphicsEdge *edge, edgeList) 
		{
			edge->adjust();
		}
		break;
	}
	return QGraphicsItem::itemChange(change, value);
}
//
int Sys_Elastic_GraphicsTreeObject::get_bottomLevel_object_count(void) const {
	return this->bottomLevelObjectList.count();
}
//
QList<Sys_Elastic_GraphicsTreeObject *> Sys_Elastic_GraphicsTreeObject::get_bottomLevel_objectList(void) const {
    return this->bottomLevelObjectList;
}
//
qreal Sys_Elastic_GraphicsTreeObject::get_dockingPoint_offset(void) {
	return Sys_Elastic_GraphicsTreeObject::dockingPointOffset;
}
//
bool Sys_Elastic_GraphicsTreeObject::get_state(void) const {
	return state;
}
//
Sys_Elastic_GraphicsTreeObject *Sys_Elastic_GraphicsTreeObject::get_topLevel_object(void) const 
{
    return topLevelObject;
}
//
bool Sys_Elastic_GraphicsTreeObject::is_active(void) const 
{
    return state;
}
//
bool Sys_Elastic_GraphicsTreeObject::is_enabled(void) const 
{
	return enabled;
}

//
bool Sys_Elastic_GraphicsTreeObject::remove_bottomLevel_object(Sys_Elastic_GraphicsTreeObject *bottomLevelObject) 
{
    bool success = bottomLevelObjectList.contains(bottomLevelObject);
	if (success) 
	{
        bottomLevelObjectList.removeAll(bottomLevelObject);
		disconnect(bottomLevelObject, SIGNAL(state_changed(const bool)), this, SLOT(set_active_without_signals(const bool)));

		update();
	}
    return success;
}
//
void Sys_Elastic_GraphicsTreeObject::set_dockingPoint_offset(const qreal offset) {
	Sys_Elastic_GraphicsTreeObject::dockingPointOffset = offset;
}
//
void Sys_Elastic_GraphicsTreeObject::set_topLevel_object(Sys_Elastic_GraphicsTreeObject *topLevelObject)
{
    if (this->topLevelObject != 0)
        disconnect(this->topLevelObject, SLOT(update()));
    this->topLevelObject = topLevelObject;
    if (this->topLevelObject != 0)
        connect(this, SIGNAL(state_changed()), this->topLevelObject, SLOT(update()));

    update();
}
//public slots
//
void Sys_Elastic_GraphicsTreeObject::on_bottomLevel_state_changed(const bool state) 
{
	if (state == this->enabled)
		return;

	if (state)
	{
		this->set_enabled();
	}
	else
	{
		bool enabled = false;
		for (int i = 0; i < this->bottomLevelObjectList.count(); i++)
		{
			if (this->bottomLevelObjectList.at(i)->is_active() && this->bottomLevelObjectList.at(i)->is_enabled())
			{
				enabled = true;
				break;
			}
		}
		this->set_enabled(enabled);
	}
}
