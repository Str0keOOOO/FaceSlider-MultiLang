#include "Face.h"
#include "ui_Face.h"

#include <QPainter>
#include <QRadialGradient>

Face::Face(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Face)
{
    ui->setupUi(this);
    parent->installEventFilter(this);

    // 成员实例化的顺序会影响到绘制的层次，因此blushes应在eyes_前实例化
    // 从而到达eyes_覆盖blushed的效果
    blushes_[0] = new Blush(this);
    blushes_[1] = new Blush(this);
    eyes_[0] = new Eye(Eye::Left, this);
    eyes_[1] = new Eye(Eye::Right, this);
    mouth_ = new Mouth(this);
}

Face::~Face()
{
    delete ui;
}

Face &Face::setFaceX(double x)
{
    x_ = x;
    eyes_[0]->move(width() * (0.18 + x * 0.04), eyes_[0]->y());
    eyes_[1]->move(width() - eyes_[1]->width() - width() * (0.22 - x * 0.04), eyes_[1]->y());
    eyes_[0]->updateBallPos(x_, y_, drep_);
    eyes_[1]->updateBallPos(x_, y_, drep_);
    // blush
    auto percent = 0.07 + 0.02 * x; // left
    blushes_[0]->move(percent * width(), blushes_[0]->y());
    percent = 0.09 - 0.02 * x; // right
    blushes_[1]->move(width() - percent * width() - blushes_[1]->width(), blushes_[1]->y());
    // mouth
    auto leftPercent = 0.475 + x * 0.05;
    mouth_->move(leftPercent * width(), mouth_->y());
    return *this;
}

double Face::faceX() const
{
    return x_;
}

Face &Face::setFaceY(double y)
{
    y_ = y;
    eyes_[0]->move(eyes_[0]->x(), height() * (0.25 + y * 0.1));
    eyes_[1]->move(eyes_[1]->x(), height() * (0.25 + y * 0.1));
    eyes_[0]->updateBallPos(x_, y_, drep_);
    eyes_[1]->updateBallPos(x_, y_, drep_);
    // blush
    auto percent = 0.45 + 0.1 * y;
    blushes_[0]->move(blushes_[0]->x(), height() * percent);
    blushes_[1]->move(blushes_[1]->x(), height() * percent);
    // mouth
    auto topPercent = 0.7 + y * 0.05;
    mouth_->move(mouth_->x(), topPercent * height());
    return *this;
}

double Face::faceY() const
{
    return y_;
}

Face &Face::setDrep(double drep)
{
    drep_ = drep;
    eyes_[0]->updateBallPos(x_, y_, drep_);
    eyes_[1]->updateBallPos(x_, y_, drep_);
    return *this;
}

double Face::drep() const
{
    return drep_;
}

static const int BorderWidth = 3;

static QPainterPath setRadius(const QSize &sz, double tl, double tr, double br, double bl)
{
    auto min_radius = qMin(sz.width(), sz.height());
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
    path.moveTo(tl + BorderWidth, tl + BorderWidth);
    path.arcTo(BorderWidth, BorderWidth, 2 * tl, 2 * tl, 180, -90);
    // top right
    path.lineTo(sz.width() - tr - BorderWidth, BorderWidth);
    path.lineTo(sz.width() - tr - BorderWidth, tr + BorderWidth);
    path.arcTo(sz.width() - 2 * tr - BorderWidth, BorderWidth, 2 * tr, 2 * tr, 90, -90);
    // bottom right
    path.lineTo(sz.width() - BorderWidth, sz.height() - br - BorderWidth);
    path.lineTo(sz.width() - br - BorderWidth, sz.height() - br - BorderWidth);
    path.arcTo(sz.width() - 2 * br - BorderWidth,
               sz.height() - 2 * br - BorderWidth,
               2 * br,
               2 * br,
               0,
               -90);
    // bottom left
    path.lineTo(bl + BorderWidth, sz.height() - BorderWidth);
    path.lineTo(bl + BorderWidth, sz.height() - bl - BorderWidth);
    path.arcTo(BorderWidth, sz.height() - 2 * bl - BorderWidth, 2 * bl, 2 * bl, -90, -90);
    path.lineTo(BorderWidth, tl + BorderWidth);
    path.closeSubpath();

    return path;
}

Face &Face::setHappiness(double happiness)
{
    happiness_ = happiness;
    // eyes_
    eyes_[0]->setHappiness(happiness);
    eyes_[1]->setHappiness(happiness);
    // blush
    blushes_[0]->happiness = happiness;
    blushes_[1]->happiness = happiness;
    // mouth
    auto wPercent = 0.51 - happiness * 0.02;
    auto hPercent = 0.26 - happiness * 0.02;
    mouth_->resize(wPercent * width(), hPercent * height());
    auto topRadius = (1 - happiness) * mouth_->height();
    auto bottomRadius = happiness * mouth_->height();
    mouth_->path = setRadius(mouth_->size(), topRadius, topRadius, bottomRadius, bottomRadius);
    mouth_->move(mouth_->pos() - QPoint(mouth_->width() / 2, mouth_->height() / 2)); // 移到中心点
    update();
    return *this;
}

double Face::happiness() const
{
    return happiness_;
}

bool Face::eventFilter(QObject *o, QEvent *e)
{
    if (o == parent()) {
        if (e->type() == QEvent::Resize) {
            auto p = dynamic_cast<QWidget *>(parent());
            auto sz = qMin(p->width() / 2, p->height());
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
    auto drawingRect = rect().adjusted(BorderWidth, BorderWidth, -BorderWidth, -BorderWidth);
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
{
    ball_ = new EyeBall(this);
}

void Face::Eye::setHappiness(double happiness)
{
    // self
    auto percent = 0.26 - happiness * 0.02;
    auto p = dynamic_cast<QWidget *>(parent());
    resize(p->width() * percent, p->height() * percent);
    // eye ball
    auto ballPercentSz = 0.55 - happiness * 0.1;
    auto ballSz = width() * ballPercentSz;
    ball_->resize(ballSz, ballSz);
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
    auto ballX = ball_->width() * xPercent;
    auto ballY = ball_->height() * yPercent;
    ball_->move((width()) / 2 - ball_->width() + ballX, (height()) / 2 - ball_->height() + ballY);
}

void Face::Eye::resizeEvent(QResizeEvent *e)
{
    QRegion region(rect(), QRegion::Ellipse);
    setMask(region);
    QWidget::resizeEvent(e);
}

void Face::Eye::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(0xf6f6f6));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect());
    QWidget::paintEvent(e);
}

Face::Eye::EyeBall::EyeBall(Eye *parent)
    : QWidget{parent}
{}

void Face::Eye::EyeBall::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setBrush(QColor(0x442211));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(rect());
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
    painter.setOpacity(happiness * happiness * 0.9 + 0.1);
    painter.drawEllipse(rect());
    QWidget::paintEvent(e);
}

Face::Mouth::Mouth(QWidget *parent)
    : QWidget{parent}
{
    teeth_ = new Teeth(this);
    tongue_ = new Tongue(this);
}

void Face::Mouth::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    // 先画边框
    painter.setBrush(Qt::NoBrush);
    QPen pen(QColor(0x962d2d));
    pen.setWidth(BorderWidth);
    painter.setPen(pen);
    painter.setClipPath(path); // 裁剪掉那些超出path的部分
    painter.drawPath(path);
    // 再填充颜色
    painter.setBrush(QColor(0xaa3333));
    painter.setPen(Qt::NoPen);
    painter.drawPath(path);

    // set mask
    QBitmap bit(size());
    bit.fill(Qt::color0);
    QPainter bitPainter(&bit);
    bitPainter.setRenderHint(QPainter::Antialiasing);
    bitPainter.setPen(Qt::color0);
    bitPainter.setBrush(Qt::NoBrush);
    bitPainter.drawPath(path);
    bitPainter.setPen(Qt::NoPen);
    bitPainter.setBrush(Qt::color1);
    bitPainter.drawPath(path);
    setMask(bit);
    QWidget::paintEvent(e);
}

void Face::Mouth::resizeEvent(QResizeEvent *e)
{
    // teeth
    teeth_->resize(width() * 0.25, height() * 0.25);
    teeth_->move(width() * 0.5, BorderWidth);
    // tongue_
    tongue_->resize(width() * 0.6, height() * 0.5);
    tongue_->move(width() * 0.1, height() - tongue_->height());
    QWidget::resizeEvent(e);
}

Face::Mouth::Teeth::Teeth(QWidget *parent)
    : QWidget{parent}
{}

void Face::Mouth::Teeth::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::white);
    auto path = setRadius(size(), 0, 0, 0.4 * height(), 0.4 * height());
    painter.drawPath(path);
    QWidget::paintEvent(e);
}

Face::Mouth::Tongue::Tongue(QWidget *parent)
    : QWidget{parent}
{}

void Face::Mouth::Tongue::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(0, 0, 0, 51));
    auto path = setRadius(size(), height(), height(), 0, 0);
    painter.drawPath(path);
    QWidget::paintEvent(e);
}
