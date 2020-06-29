#pragma once
/**\class Sys_Elastic_GraphicsEdge
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef SYS_ELASTIC_GRAPHICSEDGE_H
#define SYS_ELASTIC_GRAPHICSEDGE_H

//qt-libs
#include <QPainter>

//system sys
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

#include "Sys_Elastic_GraphicsObject.h"

class Sys_Elastic_GraphicsTreeObject;

/// \ingroup sys
/**
 *
 */
class Sys_Elastic_GraphicsEdge : public Sys_Elastic_GraphicsObject
{
	Q_OBJECT

public:

    ///Default Constructor
    /**
     *
     */
    explicit Sys_Elastic_GraphicsEdge(Sys_Elastic_GraphicsTreeObject *root, Sys_Elastic_GraphicsTreeObject *destination);

	///Default Destructor
	/**
	 *
	 */
	~Sys_Elastic_GraphicsEdge(void);

	///
	/**
	 *
	 */
	void adjust(void);

	///
    /**
     *
     */
	bool are_arrows_visible(void) const;

	///
    /**
     *
     */
	QRectF boundingRect(void) const;

	///
	/**
	 *
	 */
	qreal get_angle(void) const;

	///
	/**
	 *
	 */
	static qreal get_line_width(void);
	///
    /**
     *
     */
    bool is_visible(void) const;

	///
    /**
     *
     */
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

	///
    /**
     *
     */
	void set_arrows_visible(const bool visible);

	///
	/**
	 *
	 */
	static void set_line_width(const qreal width);

    ///
    /**
     *
     */
    void set_visible(const bool visible);

public slots:

	///
	/**
	 *
	 */
	void on_tree_object_state_changed(const bool state);

private:
	Sys_Elastic_GraphicsTreeObject *root;
	Sys_Elastic_GraphicsTreeObject *dest;

	static qreal lineWidth;

    QPointF rootPoint;
    QPointF destPoint;

	bool visibleArrows;
	qreal arrowSize;

	bool state;

	///
	/**
	 *
	 */
	QLineF get_line_between(Sys_Elastic_GraphicsTreeObject *root, Sys_Elastic_GraphicsTreeObject *dest);
};

#endif // SYS_ELASTIC_GRAPHICSEDGE_H
