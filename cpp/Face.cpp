#include "Face.h"

#include <QPainter>
#include <QRadialGradient>
#include <QEvent>

Face::Face(QWidget *parent)
    : QWidget(parent)
{
    resize(457, 404);
    if (parent)
        parent->installEventFilter(this);

    // The order of instantiating members will affect the drawing hierarchy
    // Therefore, blushes_ should be instantiated before eyes_
    // Thus achieving the effect of eyes_ covering blushing
    blushes_[0] = new Blush(this);
    blushes_[1] = new Blush(this);
    eyes_[0] = new Eye(Eye::Left, this);
    eyes_[1] = new Eye(Eye::Right, this);
    mouth_ = new Mouth(this);
}

Face &Face::setFaceX(double x)
{
    x_ = x;
    return *this;
}

double Face::faceX() const
{
    return x_;
}

Face &Face::setFaceY(double y)
{
    y_ = y;
    return *this;
}

double Face::faceY() const
{
    return y_;
}

Face &Face::setDrep(double drep)
{
    drep_ = drep;
    return *this;
}

double Face::drep() const
{
    return drep_;
}

static QPainterPath setRadius(const QSizeF &sz, double tl, double tr, double br, double bl)
{
    const double min_radius = qMin(sz.width(), sz.height());
    if (tl > min_radius)
        tl = min_radius;
    if (tr > min_radius)
        tr = min_radius;
    if (br > min_radius)
        br = min_radius;
    if (bl > min_radius)
        bl = min_radius;

    QPainterPath path;
    // top left
    constexpr int borderWidth = 3;
    path.moveTo(tl + borderWidth, tl + borderWidth);
    path.arcTo(borderWidth, borderWidth, 2 * tl, 2 * tl, 180, -90);
    // top right
    path.lineTo(sz.width() - tr - borderWidth, borderWidth);
    path.lineTo(sz.width() - tr - borderWidth, tr + borderWidth);
    path.arcTo(sz.width() - 2 * tr - borderWidth, borderWidth, 2 * tr, 2 * tr, 90, -90);
    // bottom right
    path.lineTo(sz.width() - borderWidth, sz.height() - br - borderWidth);
    path.lineTo(sz.width() - br - borderWidth, sz.height() - br - borderWidth);
    path.arcTo(sz.width() - 2 * br - borderWidth,
               sz.height() - 2 * br - borderWidth,
               2 * br,
               2 * br,
               0,
               -90);
    // bottom left
    path.lineTo(bl + borderWidth, sz.height() - borderWidth);
    path.lineTo(bl + borderWidth, sz.height() - bl - borderWidth);
    path.arcTo(borderWidth, sz.height() - 2 * bl - borderWidth, 2 * bl, 2 * bl, -90, -90);
    path.lineTo(borderWidth, tl + borderWidth);
    path.closeSubpath();

    return path;
}

Face &Face::setHappiness(double happiness)
{
    happiness_ = happiness;
    return *this;
}

double Face::happiness() const
{
    return happiness_;
}

void Face::apply()
{
    const int w = width(), h = height();
    // eyes
    eyes_[0]->setHappiness(happiness_);// calc eye size and eye ball radius
    eyes_[1]->setHappiness(happiness_);

    const double eyeY = h * (0.25 + y_ * 0.1);
    eyes_[0]->move(w * (0.18 + x_ * 0.04), eyeY);
    // right eye depends on eye size, so setHappiness() MUST be called before right eye calls move()
    eyes_[1]->move(w - eyes_[1]->width() - w * (0.22 - x_ * 0.04), eyeY);

    eyes_[0]->updateBallPos(x_, y_, drep_);// calc eye ball position
    eyes_[1]->updateBallPos(x_, y_, drep_);

    // blush
    const double xLeftPercent = 0.07 + 0.02 * x_; // left
    const double blushY = (0.45 + 0.1 * y_) * h;
    blushes_[0]->move(xLeftPercent * w, blushY);
    const double xRightPercent = 0.09 - 0.02 * x_; // right
    blushes_[1]->move(w - xRightPercent * w - blushes_[1]->width(), blushY);

    blushes_[0]->happiness_ = happiness_;
    blushes_[1]->happiness_ = happiness_;

    // mouth
    const double wPercent = 0.51 - happiness_ * 0.02;
    const double hPercent = 0.26 - happiness_ * 0.02;
    mouth_->resize(wPercent * w, hPercent * h);

    const double leftPercent = 0.475 + x_ * 0.05;
    const double topPercent = 0.7 + y_ * 0.05;
    mouth_->move(leftPercent * w - mouth_->width() / 2,
                 topPercent * height() - mouth_->height() / 2);

    const double topRadius = (1 - happiness_) * mouth_->height();
    const double bottomRadius = happiness_ * mouth_->height();
    mouth_->path_ = setRadius(mouth_->size(), topRadius, topRadius, bottomRadius, bottomRadius);

    update();
}

bool Face::eventFilter(QObject *o, QEvent *e)
{
    if (o == parent()) {
        if (e->type() == QEvent::Resize) {
            QWidget *p = parentWidget();
            if (!p)
                return false;
            const int sz = qMin(p->width() / 2, p->height());
            resize(sz, sz);
            move((p->width() - width()) / 2, (p->height() - height()) / 2);
        }
    }
    return QWidget::eventFilter(o, e);
}

void Face::resizeEvent(QResizeEvent *e)
{
    blushes_[0]->resize(0.2 * width(), 0.1 * height());
    blushes_[1]->resize(0.2 * width(), 0.1 * height());
    QWidget::resizeEvent(e);
}

void Face::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // background
    QRadialGradient radGrad(width() / 2.0, height() / 2.0, width() / 2.0);
    radGrad.setColorAt(0, QColor(0xf7e0b2));
    radGrad.setColorAt(1, QColor(0xeebb55));
    QBrush brush(radGrad);
    painter.setBrush(brush);
    // border
    QPen pen(QColor(0xecb23e));
    pen.setWidth(BorderWidth);
    painter.setPen(pen);
    // shape
    const QRect drawingRect = rect().adjusted(BorderWidth, BorderWidth, -BorderWidth, -BorderWidth);
    painter.drawEllipse(drawingRect);
    // unhappy mask
    QLinearGradient linearGrad(width() / 2, 0, width() / 2, height());
    linearGrad.setColorAt(0, QColor(0x55aa88));
    linearGrad.setColorAt(1, Qt::transparent);
    QBrush brush2(linearGrad);
    painter.setOpacity(1 - happiness_);
    painter.setBrush(brush2);
    painter.drawEllipse(drawingRect);

    QWidget::paintEvent(e);
}

Face::Eye::Eye(Position pos, QWidget *parent)
    : QWidget{parent}
    , pos_(pos)
{}

void Face::Eye::setHappiness(double happiness)
{
    // self
    const double percent = 0.26 - happiness * 0.02;
    const QWidget *p = dynamic_cast<const QWidget *>(parent());
    resize(p->width() * percent, p->height() * percent);
    // eye ball
    const double ballPercentSz = 0.55 - happiness * 0.1;
    ball_radius_ = width() * ballPercentSz / 2.0;
}

void Face::Eye::updateBallPos(double x, double y, double drep)
{
    double xPercent, yPercent;
    if (isLeft()) {
        xPercent = x + drep * 0.45;
        yPercent = y + drep * 0.45;
    } else {
        xPercent = x - drep * 0.40;
        yPercent = y - drep * 0.40;
    }
    const double ballSize = 2 * ball_radius_;
    const double ballX = ballSize * xPercent;
    const double ballY = ballSize * yPercent;
    ball_pos_ = QPointF(width() / 2.0 - ball_radius_ + ballX, height() / 2.0 - ball_radius_ + ballY);
}

void Face::Eye::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addEllipse(rect());
    painter.setClipPath(path);

    // draw eye
    painter.setBrush(QColor(0xf6f6f6));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect());

    // draw eye ball
    painter.setBrush(QColor(0x442211));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(ball_pos_, ball_radius_, ball_radius_);

    QWidget::paintEvent(e);
}

Face::Blush::Blush(QWidget *parent)
    : QWidget{parent}
{}

void Face::Blush::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(255, 100, 100, 77));
    QPen pen(QColor(255, 100, 100, 77));
    pen.setWidth(3);
    painter.setPen(pen);
    painter.setOpacity(happiness_ * happiness_ * 0.9 + 0.1);
    painter.drawEllipse(rect());
    QWidget::paintEvent(e);
}

Face::Mouth::Mouth(QWidget *parent)
    : QWidget{parent}
{}

void Face::Mouth::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // draw border first
    painter.setBrush(Qt::NoBrush);
    QPen pen(QColor(0x962d2d));
    pen.setWidth(BorderWidth);
    painter.setPen(pen);
    painter.setClipPath(path_); // clip off the parts that exceed the path
    painter.drawPath(path_);
    // then fill the background
    painter.setBrush(QColor(0xaa3333));
    painter.setPen(Qt::NoPen);
    painter.drawPath(path_);
    // draw teeth
    painter.setBrush(Qt::white);
    const QPainterPath teethPath = setRadius(teeth_sz_, 0, 0, 0.4 * teeth_sz_.height(), 0.4 * teeth_sz_.height());
    painter.translate(teeth_pos_);
    painter.drawPath(teethPath);
    // draw tongue
    painter.setBrush(QColor(0, 0, 0, 51));
    const QPainterPath tonguePath = setRadius(tongue_sz_, tongue_sz_.height(), tongue_sz_.height(), 0, 0);
    painter.translate(tongue_pos_ - teeth_pos_);
    painter.drawPath(tonguePath);

    QWidget::paintEvent(e);
}

void Face::Mouth::resizeEvent(QResizeEvent *e)
{
    // teeth
    teeth_sz_ = QSizeF(width() * 0.25, height() * 0.25);
    teeth_pos_ = QPointF(width() * 0.5, BorderWidth);
    // tongue
    tongue_sz_ = QSizeF(width() * 0.6, height() * 0.5);
    tongue_pos_ = QPointF(width() * 0.1, height() - tongue_sz_.height());
    update();
    QWidget::resizeEvent(e);
}
