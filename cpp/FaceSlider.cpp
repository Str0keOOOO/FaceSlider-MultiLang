#include "FaceSlider.h"
#include "StyleLoader.h"
#include "ui_FaceSlider.h"

#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPropertyAnimation>
#include <QPushButton>
#include <QRandomGenerator>
#include <QTranslator>

static QTranslator translator;

FaceSlider::FaceSlider(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    setMouseMoveEnabled(this);
    initUI();
    initAnim();
    loadStyle();
}

FaceSlider::~FaceSlider()
{
    delete ui;
}

FaceSlider &FaceSlider::setMaxUnhappyCount(size_t cnt)
{
    max_unhappy_ = cnt;
    return *this;
}

size_t FaceSlider::maxUnhappyCount() const
{
    return max_unhappy_;
}

FaceSlider &FaceSlider::setFaceX(double x)
{
    ui->face->setFaceX(x);
    if (face_anim_starting_)
        ui->face->apply();
    return *this;
}

double FaceSlider::faceX() const
{
    return ui->face->faceX();
}

FaceSlider &FaceSlider::setFaceY(double y)
{
    ui->face->setFaceY(y);
    if (face_anim_starting_)
        ui->face->apply();
    return *this;
}

double FaceSlider::faceY() const
{
    return ui->face->faceY();
}

FaceSlider &FaceSlider::setHappiness(double happiness)
{
    ui->face->setHappiness(happiness);
    if (face_anim_starting_)
        ui->face->apply();
    return *this;
}

double FaceSlider::happiness() const
{
    return ui->face->happiness();
}

FaceSlider &FaceSlider::setDrep(double drep)
{
    ui->face->setDrep(drep);
    // Animations would call setter functions,
    // so we need call apply() to update the value and UI
    // But in other cases, we shouldn't call it,
    // to avoid duplicate calculations and improve peformance
    if (face_anim_starting_)
        ui->face->apply();
    return *this;
}

double FaceSlider::drep() const
{
    return ui->face->drep();
}

FaceSlider &FaceSlider::setTitle(const QString &title)
{
    ui->title->setText(title);
    return *this;
}

QString FaceSlider::title() const
{
    return ui->title->text();
}

FaceSlider &FaceSlider::setSubtitle(const QString &subtitle)
{
    ui->subtitle->setText(subtitle);
    return *this;
}

QString FaceSlider::subtitle() const
{
    return ui->subtitle->text();
}

FaceSlider &FaceSlider::setBtnHappyText(const QString &text)
{
    btnHappy_->setText(text);
    return *this;
}

QString FaceSlider::btnHappyText() const
{
    return btnHappy_->text();
}

FaceSlider &FaceSlider::setBtnUnhappyText(const QString &text)
{
    btnUnhappy_->setText(text);
    return *this;
}

QString FaceSlider::btnUnhappyText() const
{
    return btnUnhappy_->text();
}

void FaceSlider::apply()
{
    ui->face->apply();
}

bool FaceSlider::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::MouseMove) {
        if (controls_.contains(o)) {
            e->ignore();
        }
    }
    return QWidget::eventFilter(o, e);
}

void FaceSlider::contextMenuEvent(QContextMenuEvent *e)
{
    QMenu menu;
    auto actChinese = menu.addAction("简体中文", this, [this] {
        if (!translator.load("FaceSlider_zh_CN.qm")) {
            QMessageBox::critical(this,
                                  "错误",
                                  "翻译包加载失败！请确保翻译包(FaceSlider_zh_CN.qm)"
                                  "存在且与可执行文件位于同一目录");
            return;
        }
        QApplication::installTranslator(&translator);
        chinese_ = true;
    });
    actChinese->setCheckable(true);
    actChinese->setChecked(chinese_);
    auto actEnglish = menu.addAction("English", this, [this] {
        if (!translator.load("FaceSlider_en_US.qm")) {
            QMessageBox::critical(
                this,
                "Error",
                "Failed to load translation file! Please ensure the translation "
                "file(FaceSlider_en_US.qm)"
                "exists and is located in the same directory as the executable file");
            return;
        }
        QApplication::installTranslator(&translator);
        chinese_ = false;
    });
    actEnglish->setCheckable(true);
    actEnglish->setChecked(!chinese_);
    menu.exec(QCursor::pos());
    e->accept();
}

void FaceSlider::showEvent(QShowEvent *e)
{
    if (btn_clicked_) {
        return e->accept();
    }

    ui->face->setHappiness(0.9)
        .setFaceX(0.5)
        .setFaceY(0.5)
        .setDrep(1)
        .apply();

    e->accept();
}

void FaceSlider::resizeEvent(QResizeEvent *e)
{
    // adjustify the positions of the two buttons

    // If the position is being randomly moved
    // the "Uninstall" button does not need to change in size along with the size of the form.
    if (reject_cnt_ <= 0) {
        btnUnhappy_->move(width() - btnUnhappy_->width() - 50,
                          height() - btnUnhappy_->height() - 30);
    }
    btnHappy_->move(50, height() - btnHappy_->height() - 30);

    ui->face->setHappiness(0.9)
        .setFaceX(0.5)
        .setFaceY(0.5)
        .setDrep(1)
        .apply();

    QWidget::resizeEvent(e);
}

void FaceSlider::enterEvent(QEnterEvent *e)
{
    leaving_ = false;
    if (btn_clicked_) {
        return e->accept();
    }

    startFaceAnim(0);
    e->accept();
}

void FaceSlider::leaveEvent(QEvent *e)
{
    leaving_ = true;
    if (btn_clicked_) {
        return e->accept();
    }

    startFaceAnim(1, 0.5, 0.5, 0.9);
    e->accept();
}

void FaceSlider::mouseMoveEvent(QMouseEvent *e)
{
    if (btn_clicked_ || leaving_) {
        return e->accept();
    }

    mousePos_ = e->position();
    if (timerId_ >= 0) {
        return e->accept();
    }
    timerId_ = startTimer(26);

    e->accept();
}

void FaceSlider::changeEvent(QEvent *e)
{
    if (e->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        if (chinese_) {
            btnHappy_->setText("取消");
            btnUnhappy_->setText("卸载");
        } else {
            btnHappy_->setText("cancel");
            btnUnhappy_->setText("uninstall");
        }
    }
    QWidget::changeEvent(e);
}

void FaceSlider::timerEvent(QTimerEvent *e)
{
    if (mousePos_.isNull())
        return e->accept();

    if (!leaving_) {
        // The face update triggered by mouseMove needs to stop all animations except for drep,
        // to avoid bounce due to conflicts.
        stopFaceAnim();

        const double happiness = calcHappiness(mousePos_);
        // qDebug() << "happiness:" << happiness;
        ui->face->setFaceX(mousePos_.x() / width())
            .setFaceY(mousePos_.y() / height())
            .setHappiness(happiness)
            .apply();
    }

    killTimer(timerId_);
    timerId_ = -1;

    e->accept();
}

void FaceSlider::do_btnHappy_clicked()
{
    btn_clicked_ ^= 1;
    if (btn_clicked_) { // clicked cancel
        btnHappy_->setText(tr("返回"));
        btnUnhappy_->hide();
        ui->title->setText(tr("已取消"));
        ui->subtitle->setText(tr("感谢您继续使用本插件"));
    } else { // clicked back
        btnHappy_->setText(tr("取消"));
        btnUnhappy_->show();
        ui->title->setText(tr("你确定要卸载吗"));
        ui->subtitle->setText(tr("希望不要卸载"));

        reject_cnt_ = 0;
        btnUnhappy_->move(width() - btnUnhappy_->width() - 50,
                          height() - btnUnhappy_->height() - 30);
        startButtonAnim(QRect(QPoint(50, height() - btnUnhappy_->height() - 30), btnUnhappy_->size()));
    }
    // update face
    startFaceAnim(0, 0.5, 0.5, 1);
}

void FaceSlider::do_btnUnhappy_clicked()
{
    ++reject_cnt_;
    if (reject_cnt_ < max_unhappy_) {
        btnUnhappy_->setAttribute(Qt::WA_UnderMouse, false);
        // move uninstall button to a random position
        auto generator = QRandomGenerator::global();
        btnUnhappy_->move(generator->bounded(0.8) * width(), generator->bounded(0.8) * height());

        // increase the size of the uninstall button
        const QSize expandedSize = static_cast<QSize>(btnUnhappy_->size() * 0.1);
        const QRect endGeo = btnHappy_->geometry().marginsAdded(QMargins(expandedSize.width() / 2, expandedSize.height() / 2,
                                                    expandedSize.width() / 2, expandedSize.height() / 2));
        startButtonAnim(endGeo);
    } else {
        btn_clicked_ ^= 1;
        if (btn_clicked_) { // click uninstall
            btnUnhappy_->setText(tr("返回"));
            btnHappy_->hide();
            ui->title->setText(tr("已删除"));
            ui->subtitle->setText(tr("感谢您使用本插件"));
            startFaceAnim(0, 0.5, 0.5, 0.2);
        } else { // click back
            // reset buttons
            reject_cnt_ = 0;
            btnUnhappy_->move(width() - btnUnhappy_->width() - 50,
                              height() - btnUnhappy_->height() - 30);
            startButtonAnim(QRect(QPoint(50, height() - btnUnhappy_->height() - 30), btnUnhappy_->size()));

            btnUnhappy_->setText(tr("卸载"));
            btnHappy_->show();
            ui->title->setText(tr("你确定要卸载吗"));
            ui->subtitle->setText(tr("希望不要卸载"));
        }
    }
    // update face
    const double happiness = calcHappiness(mousePos_);
    startFaceAnim(0, mousePos_.x() / width(), mousePos_.y() / height(), happiness);
}

void FaceSlider::setMouseMoveEnabled(QWidget *parent)
{
    parent->setMouseTracking(true);
    parent->installEventFilter(this);
    for (int i = 0; i < parent->children().count(); ++i) {
        auto w = dynamic_cast<QWidget *>(parent->children().at(i));
        if (!w)
            continue;
        controls_.insert(w);
        setMouseMoveEnabled(w);
    }
}

double FaceSlider::calcHappiness(const QPointF &pos) const
{
    auto happyCenter = btnHappy_->geometry().toRectF().center();
    auto unhappyCenter = btnUnhappy_->geometry().toRectF().center();
    auto d1 = pos - happyCenter;
    auto d2 = pos - unhappyCenter;
    auto distHappy = qSqrt(d1.x() * d1.x() + d1.y() * d1.y());
    auto distUnhappy = qSqrt(d2.x() * d2.x() + d2.y() * d2.y());
    return qPow(distUnhappy / (distHappy + distUnhappy), 0.75);
}

void FaceSlider::initUI()
{
    setAutoFillBackground(true);
    auto pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);

    btnHappy_ = new QPushButton(tr("取消"), this);
    btnHappy_->setCursor(Qt::PointingHandCursor);
    btnHappy_->setObjectName("btnHappy");
    btnHappy_->resize(100, 50);
    connect(btnHappy_, &QPushButton::clicked, this, &FaceSlider::do_btnHappy_clicked);

    btnUnhappy_ = new QPushButton(tr("卸载"), this);
    btnUnhappy_->setCursor(Qt::PointingHandCursor);
    btnUnhappy_->setObjectName("btnUnhappy");
    btnUnhappy_->resize(100, 50);
    connect(btnUnhappy_, &QPushButton::clicked, this, &FaceSlider::do_btnUnhappy_clicked);
}

void FaceSlider::initAnim()
{
    anim_faceX_ = new QPropertyAnimation(this, "faceX", this);
    anim_faceX_->setDuration(300);
    anim_faceX_->setEasingCurve(QEasingCurve::OutQuad);

    anim_faceY_ = new QPropertyAnimation(this, "faceY", this);
    anim_faceY_->setDuration(300);
    anim_faceY_->setEasingCurve(QEasingCurve::OutQuad);

    anim_happiness_ = new QPropertyAnimation(this, "happiness", this);
    anim_happiness_->setDuration(300);
    anim_happiness_->setEasingCurve(QEasingCurve::OutQuad);

    anim_drep_ = new QPropertyAnimation(this, "drep", this);
    anim_drep_->setDuration(300);
    anim_drep_->setEasingCurve(QEasingCurve::OutQuad);
    connect(anim_drep_, &QPropertyAnimation::finished, this, [this] {
        face_anim_starting_ = false;
    });

    anim_happy_size_ = new QPropertyAnimation(btnHappy_, "geometry", this);
    anim_happy_size_->setDuration(300);
    anim_happy_size_->setEasingCurve(QEasingCurve::Linear);
}

void FaceSlider::startFaceAnim(double drep, double faceX, double faceY, double happiness)
{
    face_anim_starting_ = true;
    if (faceX >= 0) {
        if (anim_faceX_->state() == QAbstractAnimation::Running)
            anim_faceX_->stop();
        anim_faceX_->setStartValue(ui->face->faceX());
        anim_faceX_->setEndValue(faceX);
        anim_faceX_->start();
    }

    if (faceY >= 0) {
        if (anim_faceY_->state() == QAbstractAnimation::Running)
            anim_faceY_->stop();
        anim_faceY_->setStartValue(ui->face->faceY());
        anim_faceY_->setEndValue(faceY);
        anim_faceY_->start();
    }

    if (happiness >= 0) {
        if (anim_happiness_->state() == QAbstractAnimation::Running)
            anim_happiness_->stop();
        anim_happiness_->setStartValue(ui->face->happiness());
        anim_happiness_->setEndValue(happiness);
        anim_happiness_->start();
    }

    if (anim_drep_->state() == QAbstractAnimation::Running)
        anim_drep_->stop();
    anim_drep_->setStartValue(ui->face->drep());
    anim_drep_->setEndValue(drep);
    anim_drep_->start();
}

void FaceSlider::stopFaceAnim()
{
    if (anim_faceX_->state() == QAbstractAnimation::Running)
        anim_faceX_->stop();
    if (anim_faceY_->state() == QAbstractAnimation::Running)
        anim_faceY_->stop();
    if (anim_happiness_->state() == QAbstractAnimation::Running)
        anim_happiness_->stop();
}

void FaceSlider::startButtonAnim(const QRect &endGeometry)
{
    if (anim_happy_size_->state() == QAbstractAnimation::Running)
        anim_happy_size_->stop();
    anim_happy_size_->setStartValue(btnHappy_->geometry());
    anim_happy_size_->setEndValue(endGeometry);
    anim_happy_size_->start();
}

void FaceSlider::loadStyle()
{
    StyleLoader().append(":/styles/common.qss").load(this);
}
