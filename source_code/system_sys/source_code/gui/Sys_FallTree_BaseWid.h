#pragma once
/**\class Sys_FallTree_BaseWid
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2009 
*/

#ifndef SYS_FALLTREE_BASEWID_H
#define SYS_FALLTREE_BASEWID_H

//qt libs
#include <QGraphicsPixmapItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QPen>

//system sys
#include "Common_Const.h"
#include "_Sys_Abstract_Base_Wid.h"
#include "Sys_Memory_Count.h"

///Widget to show or edit 'Fall Tree' data
/**

*/
class Sys_FallTree_BaseWid : public QGraphicsPixmapItem
{
public:
	///Default constructor
	Sys_FallTree_BaseWid(QPixmap pix, DataRole role = DisplayRole, QGraphicsItem *parent = 0);
	///Default destructor
	~Sys_FallTree_BaseWid(void);

	//methods

	///Return whether the item is active or not, like the QPushButton`s 'checked' member
	bool is_active(void) const { return this->active; }
	///The position of the center
	QPoint c_pos(void);
	///The position of the top boundary center
	QPoint tc_pos(void);
	///The position of the right boundary center
	QPoint rc_pos(void);
	///The position of the bottom boundary center
	QPoint bc_pos(void);
	///The position of the left boundary center
	QPoint lc_pos(void);
	///Add lines to the 'controlled lines' list of the item
	void add_dependent_lines(QList<QGraphicsLineItem*> lines);
	///Add a line to the 'controlled lines' list of the item
	void add_dependent_line(QPoint p1, QPoint p2);
	///Set the position of the item, with point as the center of the item
	void set_position(QPoint pos);
	///Overloaded method \see set_position(QPoint)
	void set_position(int x, int y);
	///Set the item active or not active
	void set_active(const bool state);

protected:
	///Reimplementation of QGraphicsItem::mousePressEvent
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	///Reimplementation of QGraphicsItem::hoverEnterEvent
	void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	///Reimplementation of QGraphicsItem::hoverLeaveEvent
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
	///The sate of the item, like a checkable button`s 'checked'
	bool active;
	///A list of all border lines, to provide some cuter look
	QList<QGraphicsLineItem*> border_lines;
	///A list of all dependent lines, lines the item 'controls'
	QList<QGraphicsLineItem*> dependent_lines;
	///The size of the item
	QSize size;
	///DataRole of the item
	DataRole role;
	
	//methods
};

#endif //SYS_FALLTREE_BASEWID_H
