#include "Sys_Headers_Precompiled.h"
//#include "Sys_FallTree_BaseWid.h"

//Default constructor
Sys_FallTree_BaseWid::Sys_FallTree_BaseWid(QPixmap pix, DataRole role, QGraphicsItem *parent) : QGraphicsPixmapItem(pix, parent) {
	
	//set the items DataRole to role
	this->role = role;
	//like 'bring layer with item to the front', to make it able to draw the 'dependent_lines' behind the item
	this->setZValue(1);
	//saving the items geometry
	this->size.setHeight(this->pixmap().height());
	this->size.setWidth(this->pixmap().width());
	//importent, to make the highlighting, when the mouse enters the item, possible
	this->setAcceptHoverEvents(true);
	//initializing the items 'active'-member
	this->active = true;
	//draw the border lines around the item
	this->border_lines.append(new QGraphicsLineItem(0, 0, this->size.width()-1, 0, this, this->scene()));
	this->border_lines.append(new QGraphicsLineItem(0, 0, 0, this->size.height()-1, this, this->scene()));
	this->border_lines.append(new QGraphicsLineItem(this->size.width()-1, 0, this->size.width()-1, this->size.height()-1, this, this->scene()));
	this->border_lines.append(new QGraphicsLineItem(0, this->size.height()-1, this->size.width()-1, this->size.height()-1, this, this->scene()));
	//and adjust the pen they are drawn with
	for (int i = 0; i < 4; i++) {
		this->border_lines[i]->setPen(QPen(QColor(Qt::black)));
		this->border_lines[i]->setZValue(2);
	}
	//count memory
	Sys_Memory_Count::self()->add_mem(sizeof(Sys_FallTree_BaseWid), _sys_system_modules::SYS_SYS);
}

//Default destructor
Sys_FallTree_BaseWid::~Sys_FallTree_BaseWid(void) {
	//count memory
	Sys_Memory_Count::self()->minus_mem(sizeof(Sys_FallTree_BaseWid), _sys_system_modules::SYS_SYS);
}

//______________
//public methods

//The position of the center
QPoint Sys_FallTree_BaseWid::c_pos(void) { 
	return QPoint(this->scenePos().x() + this->size.width()/2, this->scenePos().y() + this->size.height()/2); 
}

//The position of the top boundary center
QPoint Sys_FallTree_BaseWid::tc_pos(void) { 
	return QPoint(this->scenePos().x() + this->size.width()/2, this->scenePos().y()); 
}

//The position of the right boundary center
QPoint Sys_FallTree_BaseWid::rc_pos(void) { 
	return QPoint(this->scenePos().x() + this->size.width(), this->scenePos().y() + this->size.height()/2); 
}

//The position of the bottom boundary center
QPoint Sys_FallTree_BaseWid::bc_pos(void) { 
	return QPoint(this->scenePos().x() + this->size.width()/2, this->scenePos().y() + this->size.height()); 
}

//The position of the left boundary center
QPoint Sys_FallTree_BaseWid::lc_pos(void) { 
	return QPoint(this->scenePos().x(), this->scenePos().y() + this->size.height()/2); 
}

//Add lines to the 'controlled lines' list of the item
void Sys_FallTree_BaseWid::add_dependent_lines(QList<QGraphicsLineItem*> lines) {
	//adding lines to the dependent_lines list
	this->dependent_lines.append(lines);
	//and draw them behind the item (ZValue(0))
	for (int i = 0; i < lines.length(); i++) {
		lines[i]->setZValue(0);
		lines[i]->setPen(QPen(QBrush(Qt::black), 2));
	}
}

//Add a line to the 'controlled lines' list of the item
void Sys_FallTree_BaseWid::add_dependent_line(QPoint p1, QPoint p2) {
	QGraphicsLineItem *line = new QGraphicsLineItem(p1.x(), p1.y(), p2.x(), p2.y());
	line->setPen(QPen(QBrush(Qt::black), 2));
	line->setZValue(0);
	this->dependent_lines.append(line);
	this->scene()->addItem(line);
}

//Set the position of the item, point is the center
void Sys_FallTree_BaseWid::set_position(QPoint pos) {
	this->setPos(pos.x()-this->size.width()/2, pos.y()-this->size.height()/2);
}

//Overloaded method \see set_position(QPoint)
void Sys_FallTree_BaseWid::set_position(int x, int y) {
	this->setPos(x-this->size.width()/2, y-this->size.height()/2);
}

//____________
//public slots

//Set the item active or not active
void Sys_FallTree_BaseWid::set_active(const bool state) {

	this->active = state;
	//if state is 'false' -> draw the in a item more 'disabled' style and the dependent_lines too
	if (state == false) {
		this->setOpacity(0.5);
		for (int i = 0; i < this->dependent_lines.length(); i++) {
			if (!this->dependent_lines.empty()) {
				this->dependent_lines[i]->setOpacity(0.5);
			}
		}
		for (int i = 0; i < 4; i++) {
			this->border_lines[i]->setOpacity(0.5);
		}
	}
	//if state is 'true' -> do the opposite
	else if (state == true) {
		this->setOpacity(1.0);
		for (int i = 0; i < this->dependent_lines.length(); i++) {
			if (!this->dependent_lines.empty()) {
				this->dependent_lines[i]->setOpacity(1.0);
			}
		}
		for (int i = 0; i < 4; i++) {
			this->border_lines[i]->setOpacity(1.0);
		}
	}
}

//_________________
//protected methods

//Reimplementation of QGraphicsItem::mousePressEvent
void Sys_FallTree_BaseWid::mousePressEvent(QGraphicsSceneMouseEvent *event) {
	
	if (event->button() != Qt::LeftButton) {
		event->ignore();
		return;
	}
	else if (this->role == DisplayRole) {
		return;
	}
	else if (this->active == true) {
		this->set_active(false);
	}
	else if (this->active == false) {
		this->set_active(true);
	}
}

//Reimplementation of QGraphicsItem::hoverEnterEvent
void Sys_FallTree_BaseWid::hoverEnterEvent(QGraphicsSceneHoverEvent *event) {
	Q_UNUSED(event);

	this->setCursor(Qt::PointingHandCursor);
	for (int i = 0; i < 4; i++) {
		this->border_lines[i]->setPen(QPen(QColor(Qt::gray)));
	}
}

//Reimplementation of QGraphicsItem::hoverLeaveEvent
void Sys_FallTree_BaseWid::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) {
	Q_UNUSED(event);

	this->setCursor(Qt::ArrowCursor);
	for (int i = 0; i < 4; i++) {
		this->border_lines[i]->setPen(QPen(QColor(Qt::black)));
	}
}

//_______________
//private methods