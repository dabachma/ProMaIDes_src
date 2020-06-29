#pragma once
/**\class Sys_Elastic_GraphicsJunction
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef SYS_ELASTIC_GRAPHICSJUNCTION
#define SYS_ELASTIC_GRAPHICSJUNCTION

//system sys
#include "Sys_Elastic_GraphicsTreeObject.h"
#include <QGraphicsScene>
#include <QStyleOptionGraphicsItem>

///
/**

*/
class Sys_Elastic_GraphicsJunction 
	: public Sys_Elastic_GraphicsTreeObject
{

public:
	///
	/**
	 *
	 */
	enum JunctionType { And, Or, Custom };
	///Default constructor
    /**
     *
     */
    explicit Sys_Elastic_GraphicsJunction(QGraphicsScene *scene, QObject *parent = 0);
	///Default constructor
	/**
	 *
	 */
	explicit Sys_Elastic_GraphicsJunction(Sys_Elastic_GraphicsTreeObject *topLevelObject, QGraphicsScene *scene, QObject *parent = 0);
	///Default destructor
	/**
	 *
	 */
	~Sys_Elastic_GraphicsJunction(void);
	///
	/**
	 *
	 */
	virtual QRectF boundingRect(void) const;
	///
	/**
	 *
	 */
	virtual QPointF get_absolute_center(void) const;
	///
	/**
	 *
	 */ 
	Qt::AspectRatioMode get_aspect_ratio_mode(void) const;
	///
	/**
	 *
	 */
	virtual QPointF get_edge_docking_point(const qreal angle);
	///
	/**
	 *
	 */
	QPixmap get_custom_pixmap(void) const;
	///
	/**
	 *
	 */
	virtual qreal get_height(void) const;
	///
	/**
	 *
	 */
	QSizeF get_size(void) const;
	///
	/**
	 *
	 */
	JunctionType get_type(void) const;
	///
	/**
	 *
	 */
	virtual qreal get_width(void) const;
	///
	/**
	 *
	 */
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
	///
	/**
	 *
	 */
	virtual void set_state(const bool state);
	///
	/**
	 *
	 */ 
	void set_aspect_ratio_mode(Qt::AspectRatioMode mode);
	///
	/**
	 *
	 */
	void set_custom_pixmap(const QPixmap &pixmap);
	///
	/**
	 *
	 */
	virtual void set_disabled(void);
	///
	/**
	 *
	 */
	virtual void set_enabled(const bool enabled = true);
	///
	/**
	 *
	 */
	virtual void set_pos(const QPointF &pos);
	///
	/**
	 *
	 */
	virtual void set_pos(const qreal x, const qreal y);
	///
	/**
	 *
	 */
	void set_size(const QSizeF &size);
	///
	/**
	 *
	 */
	void set_size(const qreal width, const qreal height); 
	///
	/**
	 *
	 */
	void set_type(JunctionType type);

protected:
	///
    /**
     *
     */
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    ///
    /**
     *
     */
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
	JunctionType type;
	QPixmap customPixmap;
	QPixmap andPixmap;
	QPixmap orPixmap;
	QPixmap pixmap;
	QPointF offset;
	QColor inColor;
	qreal inFrameWidth;
	///
	/**
	 * Default value is (width=100, height=100)
	 */
	QSizeF size;
	Qt::AspectRatioMode mode;
	bool inside;
};

#endif // SYS_ELASTIC_GRAPHICSJUNCTION
