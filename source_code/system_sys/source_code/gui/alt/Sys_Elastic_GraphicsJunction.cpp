//#include "Sys_Elastic_GraphicsJunction.h"
#include "Sys_Headers_Precompiled.h"

//
Sys_Elastic_GraphicsJunction::Sys_Elastic_GraphicsJunction(Sys_Elastic_GraphicsTreeObject *topLevelObject, QGraphicsScene *scene, QObject *parent)
	: Sys_Elastic_GraphicsTreeObject(topLevelObject, scene, parent)
{
	this->andPixmap = QPixmap(":/and_sign");
	this->orPixmap = QPixmap(":/or_sign");
	this->customPixmap = QPixmap(":/null");
	this->pixmap = this->customPixmap;
	this->offset = QPointF();
	this->inColor = QColor(Qt::lightGray);
	this->inFrameWidth = 2.0;
	this->scene->addItem(this);
	this->setZValue(1.0);

	this->inside = false;
	this->mode = Qt::IgnoreAspectRatio;
	this->setAcceptHoverEvents(true);
	this->size = QSizeF(32.0, 32.0);

	ADD_MEM(Sys_Elastic_GraphicsJunction, _sys_system_modules::SYS_SYS);
}
//Default constructor
Sys_Elastic_GraphicsJunction::Sys_Elastic_GraphicsJunction(QGraphicsScene *scene, QObject *parent) 
	: Sys_Elastic_GraphicsTreeObject(scene, parent)
{
	this->andPixmap = QPixmap(":/and_sign");
	this->orPixmap = QPixmap(":/or_sign");
	this->customPixmap = QPixmap(":/null");
	this->pixmap = this->customPixmap;
	this->offset = QPointF();
	this->inColor = QColor(Qt::lightGray);
	this->inFrameWidth = 2.0;
	this->scene->addItem(this);
	this->setZValue(1.0);

	this->inside = false;
	this->mode = Qt::IgnoreAspectRatio;
	this->setAcceptHoverEvents(true);
	this->size = QSizeF(32.0, 32.0);

	ADD_MEM(Sys_Elastic_GraphicsJunction, _sys_system_modules::SYS_SYS);
}
//Default destructor
Sys_Elastic_GraphicsJunction::~Sys_Elastic_GraphicsJunction(void) 
{
	MIN_MEM(Sys_Elastic_GraphicsJunction, _sys_system_modules::SYS_SYS);
}
//
QRectF Sys_Elastic_GraphicsJunction::boundingRect(void) const {
	qreal pw = 1.0;
    if (this->pixmap.isNull())
        return QRectF();
    return QRectF(this->offset, this->size).adjusted(-pw/2, -pw/2, pw/2, pw/2);
}
//
QPointF Sys_Elastic_GraphicsJunction::get_absolute_center(void) const {
	return QPointF(this->pos().x() + this->size.width() / 2., this->pos().y() + this->size.height() / 2.);
}
//
Qt::AspectRatioMode Sys_Elastic_GraphicsJunction::get_aspect_ratio_mode(void) const {
	return this->mode;
}
//
QPointF Sys_Elastic_GraphicsJunction::get_edge_docking_point(const qreal angle) {
	qDebug("QPointF Sys_Elastic_GraphicsJunction::get_edge_docking_point(const qreal angle) has not been implemented yet!");

	Q_UNUSED(angle);
	return QPointF();
}
//
QPixmap Sys_Elastic_GraphicsJunction::get_custom_pixmap(void) const {
	return this->customPixmap;
}
//
qreal Sys_Elastic_GraphicsJunction::get_height(void) const {
	return this->size.height();
}
//
QSizeF Sys_Elastic_GraphicsJunction::get_size(void) const {
	return this->size;
}
//
Sys_Elastic_GraphicsJunction::JunctionType Sys_Elastic_GraphicsJunction::get_type(void) const {
	return this->type;
}
//
qreal Sys_Elastic_GraphicsJunction::get_width(void) const {
	return this->size.width();
}
//
void Sys_Elastic_GraphicsJunction::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(widget);

    painter->setRenderHint(QPainter::SmoothPixmapTransform);

    QRectF exposed = option->exposedRect.adjusted(-1, -1, 1, 1);
	QPixmap pixmap = this->pixmap.scaled(this->size.toSize(), this->mode, Qt::SmoothTransformation);
    exposed &= QRectF(this->offset.x(), this->offset.y(), pixmap.width(), pixmap.height());
    painter->drawPixmap(exposed, pixmap, exposed.translated(-this->offset));

	if (this->inside) 
	{
		QPen pen;
		pen.setColor(this->inColor);
		pen.setWidthF(this->inFrameWidth);
		painter->setPen(pen);
		painter->drawRect(exposed);
	}
}
//
void Sys_Elastic_GraphicsJunction::set_state(const bool state) {
	this->state = state;
	emit state_changed(state);
}
//
void Sys_Elastic_GraphicsJunction::set_aspect_ratio_mode(Qt::AspectRatioMode mode) {
	this->mode = mode;
	this->update();
}
//
void Sys_Elastic_GraphicsJunction::set_custom_pixmap(const QPixmap &pixmap) {
	this->customPixmap = pixmap;
	this->pixmap = this->customPixmap;
	this->update();
}
//
void Sys_Elastic_GraphicsJunction::set_disabled(void) {
	this->enabled = false;
	this->setOpacity(0.6);
	this->update();

	emit state_changed(false);
}
//
void Sys_Elastic_GraphicsJunction::set_enabled(const bool enabled) {
	if (enabled == this->enabled)
		return;

	if (enabled)
	{
		this->enabled = true;
		this->setOpacity(1.0);
		this->update();

		emit state_changed(true);
		return;
	}
	this->set_disabled();
}
//
void Sys_Elastic_GraphicsJunction::set_pos(const QPointF &pos) {
	setPos((pos.x() - size.width() / 2.0), (pos.y() - size.height() / 2.0));
}
//
void Sys_Elastic_GraphicsJunction::set_pos(const qreal x, const qreal y) {
	setPos((x - size.width() / 2.0), (y - size.height() / 2.0));
}
//
void Sys_Elastic_GraphicsJunction::set_size(const QSizeF &size) {
	this->size = size;
	update();
}
//
void Sys_Elastic_GraphicsJunction::set_size(const qreal width, const qreal height) {
	this->size.setWidth(width);
	this->size.setHeight(height);
	update();
}
//
void Sys_Elastic_GraphicsJunction::set_type(JunctionType type) {
	this->type = type;

	switch(type) 
	{
	case And:
		this->pixmap = this->andPixmap;
		break;
	case Or:
		this->pixmap = this->orPixmap;
		break;
	case Custom:
		this->pixmap = this->customPixmap;
		break;
	}

	this->update();
}
//protected methods
//
void Sys_Elastic_GraphicsJunction::hoverEnterEvent(QGraphicsSceneHoverEvent *event) 
{
	this->inside = true;

	QGraphicsItem::hoverEnterEvent(event);
}
//
void Sys_Elastic_GraphicsJunction::hoverLeaveEvent(QGraphicsSceneHoverEvent *event) 
{
	this->inside = false;

	QGraphicsItem::hoverLeaveEvent(event);
}
