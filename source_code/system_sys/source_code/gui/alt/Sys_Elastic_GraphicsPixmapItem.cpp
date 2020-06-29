//#include "Sys_Elastic_GraphicsPixmapItem.h"
#include "Sys_Headers_Precompiled.h"
#include <math.h>

static const double Pi = 3.14159265358979323846264338327950288419717;

//Default constructor
Sys_Elastic_GraphicsPixmapItem::Sys_Elastic_GraphicsPixmapItem(Sys_Elastic_GraphicsTreeObject *topLevelObject, QGraphicsScene *scene, QObject *parent) 
	: Sys_Elastic_GraphicsTreeObject(topLevelObject, scene, parent)
{
	this->setZValue(2.0);
	this->in = false;
	this->inColor = QColor(Qt::lightGray);
	this->inFrameWidth = 2.0;
	this->transformationMode = Qt::SmoothTransformation;
	this->nullPixmap = QPixmap();

	this->setAcceptHoverEvents(true);
	this->setCacheMode(DeviceCoordinateCache);

	this->size = QSizeF(100.0, 100.0);
	this->disabledMenuRise = false;
	this->aspectMode = Qt::IgnoreAspectRatio;

	//this->nullPixmap = QPixmap(":/null");
	this->buttonMenu = new Sys_Elastic_PixmapButtonMenu(parent = this);
	this->buttonMenu->setZValue(3.0);
	this->buttonMenu->set_scene(scene);
	connect(buttonMenu, SIGNAL(button_clicked(Sys_Elastic_PixmapButton *, const bool)), this, SLOT(on_menu_clicked(Sys_Elastic_PixmapButton *, const bool)));
	this->scene->addItem(this);
	this->scene->removeItem(this->buttonMenu);
	this->buttonMenu->setPos(this->pos().x() + this->size.width() + 3.0, this->pos().y() + /* this->size.height() + */ 0.0);

	ADD_MEM(Sys_Elastic_GraphicsPixmapItem, _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Elastic_GraphicsPixmapItem::Sys_Elastic_GraphicsPixmapItem(QGraphicsScene *scene, QObject *parent) 
	: Sys_Elastic_GraphicsTreeObject(scene, parent) 
{
	this->setZValue(2.0);
	this->in = false;
	this->inColor = QColor(Qt::lightGray);
	this->inFrameWidth = 2.0;
	this->transformationMode = Qt::SmoothTransformation;
	this->nullPixmap = QPixmap();

	this->setAcceptHoverEvents(true);
	this->setCacheMode(DeviceCoordinateCache);

	this->size = QSizeF(100.0, 100.0);
	this->disabledMenuRise = false;
	this->aspectMode = Qt::IgnoreAspectRatio;

	//this->nullPixmap = QPixmap(":/null");
	this->buttonMenu = new Sys_Elastic_PixmapButtonMenu(parent = this);
	this->buttonMenu->setZValue(3.0);
	this->buttonMenu->set_scene(scene);
	connect(buttonMenu, SIGNAL(button_clicked(Sys_Elastic_PixmapButton *, const bool)), this, SLOT(on_menu_clicked(Sys_Elastic_PixmapButton *, const bool)));
	this->scene->addItem(this);
	this->scene->removeItem(this->buttonMenu);
	this->buttonMenu->setPos(this->pos().x() + this->size.width() + 3.0, this->pos().y() + /* this->size.height() + */ 0.0);

	ADD_MEM(Sys_Elastic_GraphicsPixmapItem, _sys_system_modules::SYS_SYS);
}
//
Sys_Elastic_GraphicsPixmapItem::~Sys_Elastic_GraphicsPixmapItem(void) 
{
	MIN_MEM(Sys_Elastic_GraphicsPixmapItem, _sys_system_modules::SYS_SYS);
}
//
bool Sys_Elastic_GraphicsPixmapItem::add_pixmap(const QString &labelText, const QPixmap &pixmap) 
{
	//Set currently visible pixmap
	this->pixmap = pixmap;
	this->buttonMenu->append_button(labelText, pixmap);
	this->update();
	return true;
}
//
QRectF Sys_Elastic_GraphicsPixmapItem::boundingRect(void) const 
{
    qreal pw = 1.0;
    if (this->pixmap.isNull())
        return QRectF();
    return QRectF(this->offset, this->size).adjusted(-pw/2, -pw/2, pw/2, pw/2);
}
//
QPointF Sys_Elastic_GraphicsPixmapItem::get_absolute_center(void) const 
{
	return QPointF(this->pos().x() + this->size.width() / 2., this->pos().y() + this->size.height() / 2.);
}
//
QString Sys_Elastic_GraphicsPixmapItem::get_active_button_text(void) const 
{
	return buttonMenu->get_active_button_text();
}
//
int Sys_Elastic_GraphicsPixmapItem::get_active_button_index(void) const 
{
	return buttonMenu->get_active_button_index();
}
//
Qt::AspectRatioMode Sys_Elastic_GraphicsPixmapItem::get_aspect_ratio_mode(void) const 
{
	return this->aspectMode;
}
//
int Sys_Elastic_GraphicsPixmapItem::get_button_index(const QString &labelText) const 
{
	return buttonMenu->get_button_index(labelText);
}
//
QPointF Sys_Elastic_GraphicsPixmapItem::get_edge_docking_point(const qreal angle) 
{
	qreal m = tan(angle);
	qreal a1 = atan(this->size.height() / this->size.width());
	qreal a2 = atan(this->size.width() / this->size.height());

	QPointF dockingPoint;

	if (angle >= 2 * Pi - a1 || angle < a1) 
	{
		qreal x = this->size.width() + Sys_Elastic_GraphicsTreeObject::get_dockingPoint_offset();
		dockingPoint.setX(x);
		dockingPoint.setY(x * m + this->size.width() / 2.);
	}

	else if (angle >= a1 && angle < a1 + 2 * a2) 
	{
		qreal y = this->size.height() + Sys_Elastic_GraphicsTreeObject::get_dockingPoint_offset();
		dockingPoint.setY(y);
		dockingPoint.setX(y / m + this->size.height() / 2.);
	}

	else if (angle >= a1 + 2 * a2 && angle < 3 * a1 + 2 * a2) 
	{
		qreal x = - Sys_Elastic_GraphicsTreeObject::get_dockingPoint_offset();
		dockingPoint.setX(x);
		dockingPoint.setY(x * m + this->size.width() / 2.);
	}

	else if (angle >= 3 * a1 + 2 * a2 && angle < 3 * a1 + 4 * a2) 
	{
		qreal y = - Sys_Elastic_GraphicsTreeObject::get_dockingPoint_offset();
		dockingPoint.setY(y);
		dockingPoint.setX(y / m + this->size.height() / 2.);
	}

	QString debugStr = QString("angle: %1, m: %2").arg(angle * 180 / Pi).arg(m);
	qDebug(debugStr.toStdString().c_str());

	return this->pos() + dockingPoint;
}
//
qreal Sys_Elastic_GraphicsPixmapItem::get_height(void) const 
{
	return this->size.height();;
}
//
QSizeF Sys_Elastic_GraphicsPixmapItem::get_size(void) const 
{
	return this->size;
}
//
qreal Sys_Elastic_GraphicsPixmapItem::get_width(void) const 
{
	return this->size.width();
}
//
void Sys_Elastic_GraphicsPixmapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) 
{
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::SmoothPixmapTransform,
                           (this->transformationMode == Qt::SmoothTransformation));

    QRectF exposed = option->exposedRect.adjusted(-1, -1, 1, 1);
	QPixmap pixmap = this->pixmap.scaled(this->size.toSize(), this->aspectMode, this->transformationMode);
    exposed &= QRectF(this->offset.x(), this->offset.y(), pixmap.width(), pixmap.height());
    painter->drawPixmap(exposed, pixmap, exposed.translated(-this->offset));
	
	if (!enabled) 
	{
		painter->setBrush(QBrush(QColor(Qt::red)));
		painter->setOpacity(0.1);
		painter->drawRect(exposed);
		painter->setOpacity(1.0);
	}

	if (in) 
	{
		QPen pen;
		pen.setColor(this->inColor);
		pen.setWidthF(this->inFrameWidth);
		painter->setPen(pen);
		painter->setBrush(QBrush());
		painter->drawRect(exposed);
	}
}
//
bool Sys_Elastic_GraphicsPixmapItem::remove_pixmap(const QString &)
{
	qDebug("Sys_Elastic_GraphicsPixmapItem::remove_pixmap(const QString &labelText): Method not implemented yet");
	this->update();
	return true;
}
//
bool Sys_Elastic_GraphicsPixmapItem::remove_pixmap(const QPixmap &) 
{
	qDebug("Sys_Elastic_GraphicsPixmapItem::remove_pixmap(const QPixmap &pixmap): Method not implemented yet");
	this->update();
	return true;
}
//
void Sys_Elastic_GraphicsPixmapItem::set_aspect_ratio_mode(Qt::AspectRatioMode mode) 
{
	this->aspectMode = mode;
	this->update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_height(const qreal height) 
{
	this->size.setHeight(height);
	this->update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_null_pixmap(const QPixmap &pixmap) 
{
	this->nullPixmap = pixmap;
	update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_pos(const QPointF &pos) 
{
	setPos((pos.x() - size.width() / 2.0), (pos.y() - size.height() / 2.0));
	buttonMenu->setPos(this->size.width() + 3.0, /* this->size.height() + */ 0.0);

	update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_pos(const qreal x, const qreal y) 
{
	setPos((x - size.width() / 2.0), (y - size.height() / 2.0));
	buttonMenu->setPos(this->size.width() + 3.0, /* this->size.height() + */ 0.0);
	update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_size(const QSizeF &size) 
{
	this->size = size;
	this->buttonMenu->setPos(size.width() + 3.0, 0.0);
	this->update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_size(const qreal width, const qreal height) 
{
	this->size.setWidth(width);
	this->size.setHeight(height);
	this->buttonMenu->setPos(width + 3.0, 0.0);
	this->update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_width(const qreal width) 
{
	this->size.setWidth(width);
	this->buttonMenu->setPos(width + 3.0, 0.0);
	this->update();
}
//____________
//public slots
//
void Sys_Elastic_GraphicsPixmapItem::on_menu_clicked(Sys_Elastic_PixmapButton *button, const bool checked)
{
	// if state is false and new state also, here is nothing to do
	// if new state would be true, another button than the currently
	// active could have been hitten
	if (checked == state && checked == false) return;

	if (checked == false)
	{
		// update pixmap...
		if (!nullPixmap.isNull()) pixmap = nullPixmap;
		// ...and opacity...
		setOpacity(0.5);
		// ...and the item`s state
		state = false;
		update();
		// tell all items above about it
		emit state_changed(false);
	}
	else
	{
		QPixmap pixmap = button->get_pixmap();
		// update pixmap...
		this->pixmap = pixmap;
		// ...and opacity...
		setOpacity(1.0);
		// ...and the item`s state
		state = true;
		// tell all items above about it
		emit state_changed(true);
	}
	// update item to show new opacity and pixmap
	update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_active_button(const int index) 
{
	// Display the proper pixmap and change the state if necessary
	if (index >= 0 && index < buttonMenu->get_button_count())
	{
		//this->pixmap = pixmapList[index];
		this->buttonMenu->set_active_button(index);
		this->pixmap = buttonMenu->get_active_button()->get_pixmap();

		if (!state) 
		{
			this->state = true;
			this->setOpacity(1.0);
			
			emit state_changed(true);
		}
	}
	// If index is -1, which is the value for inactive state of the item
	// reduce item`s opacity and change the displayed pixmap if necessary.
	// Emit state change of course.
	else if (index == -1 && state == true)
	{
		if (!nullPixmap.isNull())
		{
			this->pixmap = nullPixmap;
		}
		this->buttonMenu->set_active_button(-1);

		state = false;
		setOpacity(0.5);
		
		emit state_changed(false);
	}

	update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_active_button(const QString &labelText) 
{
	int button = get_button_index(labelText);
	set_active_button(button);
}
//
void Sys_Elastic_GraphicsPixmapItem::set_disabled(void) 
{
	this->enabled = false;
	//this->setOpacity(0.6);
	this->buttonMenu->disable();
	this->buttonMenu->set_zValue(1.0);

	emit status_changed(false);
	this->update();
}
//
void Sys_Elastic_GraphicsPixmapItem::set_enabled(const bool enabled) 
{
	if (enabled == this->enabled)
		return;

	//if (this->get_bottomLevel_object_count() == 0)
	//	return;

	if (enabled)
	{
		this->enabled = true;
		this->buttonMenu->enable();
		this->buttonMenu->set_zValue(3.0);

		emit status_changed(true);
		this->update();
	}
	else this->set_disabled();
}
//_________
//protected
//event handling
//
void Sys_Elastic_GraphicsPixmapItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event) 
{
	this->in = true;
	QPointF pos = this->pos() + QPointF(size.width() + 3.0, 0.0);

	if (enabled && (!buttonMenu->is_rising() || !buttonMenu->is_visible())) 
		this->buttonMenu->rise(pos);

	this->update();
	QGraphicsItem::hoverEnterEvent(event);
}

//
void Sys_Elastic_GraphicsPixmapItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) 
{
	this->in = false;
	
	if (buttonMenu->is_visible() || buttonMenu->is_rising())
		this->buttonMenu->fall();

	this->update();
	QGraphicsItem::hoverLeaveEvent(event);
}

//
void Sys_Elastic_GraphicsPixmapItem::mousePressEvent(QGraphicsSceneMouseEvent *event) 
{
	QPointF pos = this->pos() + QPointF(size.width() + 3.0, 0.0);
	if (enabled) this->buttonMenu->rise(pos);
	QGraphicsItem::mousePressEvent(event);
}

//
void Sys_Elastic_GraphicsPixmapItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) 
{
	QGraphicsItem::mouseReleaseEvent(event);
}

//
void Sys_Elastic_GraphicsPixmapItem::wheelEvent(QGraphicsSceneWheelEvent *event) 
{
	if (!enabled)
		return;

	int btnCount = this->buttonMenu->get_button_count();
	int index = this->buttonMenu->get_active_button_index();
	int newIndex;
	if (event->delta() < 0) 
	{
		if (index < btnCount - 1)
		{
			newIndex = index + 1;
			//this->buttonMenu->set_active_button(newIndex);
		}
		else if (index == btnCount -1)
		{
			newIndex = -1;
		}
		else
		{
			newIndex = -1;
			//this->buttonMenu->set_active_button(newIndex);
		}
	}
	else 
	{
		if (index > 0)
		{
			newIndex = index - 1;
			//this->buttonMenu->set_active_button(newIndex);
		}
		else if (index == 0)
		{
			newIndex = -1;
		}
		else
		{
			newIndex = btnCount - 1;
			//this->buttonMenu->set_active_button(newIndex);
		}
	}
	set_active_button(newIndex);
	//this->on_menuIndex_changed(newIndex);

	this->update();
	QGraphicsItem::wheelEvent(event);
}
//
QVariant Sys_Elastic_GraphicsPixmapItem::itemChange(QGraphicsItem::GraphicsItemChange change, const QVariant &value) {
	if (change == ItemPositionChange)
	{
		buttonMenu->setPos(this->pos().x() + this->size.width() + 3.0, this->pos().y() + /* this->size.height() + */ 0.0);
		this->buttonMenu->fall();
	}

	return Sys_Elastic_GraphicsTreeObject::itemChange(change, value);
}
