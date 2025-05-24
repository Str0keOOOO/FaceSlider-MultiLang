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

QTranslator translator;

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
    return *this;
}

double FaceSlider::faceX() const
{
    return ui->face->faceX();
}

FaceSlider &FaceSlider::setFaceY(double y)
{
    ui->face->setFaceY(y);
    return *this;
}

double FaceSlider::faceY() const
{
    return ui->face->faceY();
}

FaceSlider &FaceSlider::setHappiness(double happiness)
{
    ui->face->setHappiness(happiness);
    return *this;
}

double FaceSlider::happiness() const
{
    return ui->face->happiness();
}

FaceSlider &FaceSlider::setDrep(double drep)
{
    ui->face->setDrep(drep);
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

bool FaceSlider::eventFilter(QObject *o, QEvent *e)
{
    if (e->type() == QEvent::MouseMove) {
        if (controls_.find(o) != controls_.end()) {
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
        e->ignore();
        return;
    }

    ui->face->setFaceX(0.5);
    ui->face->setFaceY(0.5);
    ui->face->setHappiness(0.9);
    ui->face->setDrep(1);
    e->accept();
}

void FaceSlider::resizeEvent(QResizeEvent *e)
{
    // 调整两个按钮的位置
    if (reject_cnt_ <= 0) { // 如果正在随机移动位置，则卸载按钮不需要跟随窗体大小变化而变化
        btnUnhappy_->move(width() - btnUnhappy_->width() - 50,
                          height() - btnUnhappy_->height() - 30);
    }
    btnHappy_->move(50, height() - btnHappy_->height() - 30);
    ui->face->setFaceX(0.5);
    ui->face->setFaceY(0.5);
    ui->face->setHappiness(0.9);
    ui->face->setDrep(1);
    e->accept();
}

void FaceSlider::enterEvent(QEnterEvent *e)
{
    if (btn_clicked_) {
        e->ignore();
        return;
    }

    startAnim(0);
    e->accept();
}

void FaceSlider::leaveEvent(QEvent *e)
{
    if (btn_clicked_) {
        e->ignore();
        return;
    }

    ui->face->setFaceX(0.5);
    ui->face->setFaceY(0.5);
    ui->face->setHappiness(0.9);
    startAnim(1);
    e->accept();
}

void FaceSlider::mouseMoveEvent(QMouseEvent *e)
{
    if (btn_clicked_) {
        e->ignore();
        return;
    }

    auto pos = e->position();
    ui->face->setFaceX(pos.x() / width());
    ui->face->setFaceY(pos.y() / height());
    // calc happiness
    auto happiness = calcHappiness(pos);
    qDebug() << "happiness:" << happiness;
    ui->face->setHappiness(happiness);
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

void FaceSlider::do_btnHappy_clicked()
{
    btn_clicked_ ^= 1;
    if (btn_clicked_) { // 点击取消
        btnHappy_->setText(tr("返回"));
        btnUnhappy_->hide();
        ui->title->setText(tr("已取消"));
        ui->subtitle->setText(tr("感谢您继续使用本插件"));
        ui->face->setFaceX(0.5);
        ui->face->setFaceY(0.5);
        ui->face->setHappiness(1);
        ui->face->setDrep(0);
    } else { // 点击返回
        btnHappy_->setText(tr("取消"));
        btnUnhappy_->show();
        ui->title->setText(tr("你确定要卸载吗"));
        ui->subtitle->setText(tr("希望不要卸载"));
    }
}

void FaceSlider::do_btnUnhappy_clicked()
{
    ++reject_cnt_;
    if (reject_cnt_ < max_unhappy_) {
        // 移动卸载按钮到随机位置
        auto generator = QRandomGenerator::global();
        btnUnhappy_->move(generator->bounded(0.8) * width(), generator->bounded(0.8) * height());
        // 加大取消按钮的大小
        if (anim_happy_size_->state() == QAbstractAnimation::Running)
            anim_happy_size_->stop();
        anim_happy_size_->setStartValue(btnHappy_->size());
        anim_happy_size_->setEndValue(btnUnhappy_->size() * (reject_cnt_ * 0.1 + 1));
        anim_happy_size_->start();
        return;
    }
    btn_clicked_ ^= 1;
    if (btn_clicked_) { // 点击卸载
        btnUnhappy_->setText(tr("返回"));
        btnHappy_->hide();
        ui->title->setText(tr("已删除"));
        ui->subtitle->setText(tr("感谢您使用本插件"));
        ui->face->setFaceX(0.5);
        ui->face->setFaceY(0.5);
        ui->face->setHappiness(0.2);
        ui->face->setDrep(0);
    } else { // 点击返回
        // 重置按钮状态
        reject_cnt_ = 0;
        btnUnhappy_->move(width() - btnUnhappy_->width() - 50,
                          height() - btnUnhappy_->height() - 30);
        btnHappy_->move(50, height() - btnUnhappy_->height() - 30);
        if (anim_happy_size_->state() == QAbstractAnimation::Running)
            anim_happy_size_->stop();
        anim_happy_size_->setStartValue(btnHappy_->size());
        anim_happy_size_->setEndValue(btnUnhappy_->size());
        anim_happy_size_->start();

        btnUnhappy_->setText(tr("卸载"));
        btnHappy_->show();
        ui->title->setText(tr("你确定要卸载吗"));
        ui->subtitle->setText(tr("希望不要卸载"));
    }
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

double FaceSlider::calcHappiness(const QPointF &pos)
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
    btnHappy_->setObjectName("btnHappy");
    btnHappy_->resize(100, 50);
    connect(btnHappy_, &QPushButton::clicked, this, &FaceSlider::do_btnHappy_clicked);
    btnUnhappy_ = new QPushButton(tr("卸载"), this);
    btnUnhappy_->setObjectName("btnUnhappy");
    btnUnhappy_->resize(100, 50);
    connect(btnUnhappy_, &QPushButton::clicked, this, &FaceSlider::do_btnUnhappy_clicked);
}

void FaceSlider::initAnim()
{
    anim_drep_ = new QPropertyAnimation(this, "drep", this);
    anim_drep_->setDuration(300);
    anim_drep_->setEasingCurve(QEasingCurve::OutQuad);

    anim_happy_size_ = new QPropertyAnimation(btnHappy_, "size", this);
    anim_happy_size_->setDuration(300);
    anim_happy_size_->setEasingCurve(QEasingCurve::OutQuad);
}

void FaceSlider::startAnim(double drep)
{
    if (anim_drep_->state() == QAbstractAnimation::Running)
        anim_drep_->stop();
    anim_drep_->setStartValue(ui->face->drep());
    anim_drep_->setEndValue(drep);
    anim_drep_->start();
}

void FaceSlider::loadStyle()
{
    StyleLoader().append(":/styles/button.qss").load(this);
}
