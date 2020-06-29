/** \class Sys_Elastic_GraphicsTreeObject
	\author Tobias Schruff, Daniel Bachmann et al.
	\author produced by the Institute of Hydraulic Engineering (IWW), RWTH Aachen University
	\version 0.0.1                                                              
	\date 2010 
*/
#ifndef SYS_ELASTIC_GRAPHICSTREEOBJECT_H
#define SYS_ELASTIC_GRAPHICSTREEOBJECT_H

//qt-libs
#include <QGraphicsScene>

//system sys
#include "Sys_Elastic_GraphicsObject.h"
#include "Sys_Elastic_GraphicsEdge.h"
#include "Common_Const.h"
#include "Sys_Memory_Count.h"

/// \ingroup sys
/**

*/
class Sys_Elastic_GraphicsTreeObject 
	: public Sys_Elastic_GraphicsObject
{
	//qt-macro
    Q_OBJECT

public:
    ///
    /**
     *
     */
    explicit Sys_Elastic_GraphicsTreeObject(Sys_Elastic_GraphicsTreeObject *topLevelObject, QGraphicsScene *scene, QObject *parent = 0);
    ///
    /**
     *
     */
    explicit Sys_Elastic_GraphicsTreeObject(QGraphicsScene *scene, QObject *parent = 0);
    ///
    /**
     *
     */
    ~Sys_Elastic_GraphicsTreeObject(void);
    ///
    /**
     *
     */
    bool add_bottomLevel_object(Sys_Elastic_GraphicsTreeObject *bottomLevelObject);
	///
	/**
	 *
	 */
	bool add_edge(Sys_Elastic_GraphicsEdge *edge);
	///
	/**
	 *
	 */
	virtual QPointF get_absolute_center(void) const = 0;
	///
	/**
	 *
	 */
	int get_bottomLevel_object_count(void) const;
    ///
    /**
     *
     */
    QList<Sys_Elastic_GraphicsTreeObject *> get_bottomLevel_objectList(void) const;
	///
	/**
	 *
	 */
	static qreal get_dockingPoint_offset(void);
	///
	/**
	 *
	 */
	virtual QPointF get_edge_docking_point(const qreal angle) = 0;
	///
	/**
	 *
	 */
	virtual qreal get_height(void) const = 0;
	///
	/**
	 *
	 */
	bool get_state(void) const;
	///
	/**
	 *
	 */
	virtual qreal get_width(void) const = 0;
    ///
    /**
     *
     */
    Sys_Elastic_GraphicsTreeObject *get_topLevel_object(void) const;
    ///
    /**
     *
     */
    bool remove_bottomLevel_object(Sys_Elastic_GraphicsTreeObject *bottomLevelObject);
    ///
    /**
     *
     */
    bool is_active(void) const;
    ///
    /**
     *
     */
	bool is_enabled(void) const;
    ///
    /**
     *
     */
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) = 0;
	///
	/**
	 *
	 */
	virtual QRectF boundingRect(void) const = 0;
	///
	/**
	 *
	 */
	QVariant itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value);
	///
	/**
	 *
	 */
	static void set_dockingPoint_offset(const qreal offset);
	///
	/**
	 *
	 */
	virtual void set_pos(const QPointF &pos) = 0;
	///
	/**
	 *
	 */
	virtual void set_pos(const qreal x, const qreal y) = 0;
	/////
    ///**
    // *
    // */
    //virtual void set_state(const bool state) = 0;
    ///
    /**
     *
     */
    void set_topLevel_object(Sys_Elastic_GraphicsTreeObject *topLevelObject);

public slots:
	///
	/**
	 *
	 */
	virtual void on_bottomLevel_state_changed(const bool state);
    ///
    /**
     *
     */
	virtual void set_disabled(void) = 0;
    ///
    /**
     *
     */
	virtual void set_enabled(const bool enabled = true) = 0;

signals:
    ///
    /**
     *
     */
    void state_changed(const bool newState);
	///
	/**
	 *
	 */
	void status_changed(const bool newStatus);

protected:
    Sys_Elastic_GraphicsTreeObject *topLevelObject;
    QList<Sys_Elastic_GraphicsTreeObject *> bottomLevelObjectList;
	QList<Sys_Elastic_GraphicsEdge *> edgeList;
    bool state;
	bool enabled;

	QPointF absoluteCenter;
	QGraphicsScene *scene;

	static qreal dockingPointOffset;
};

#endif // SYS_ELASTIC_GRAPHICSTREEOBJECT_H
