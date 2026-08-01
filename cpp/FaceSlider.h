#ifndef FACESLIDER_H
#define FACESLIDER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class QPropertyAnimation;
class QPushButton;

class FaceSlider : public QWidget
{
    Q_OBJECT

public:
    FaceSlider(QWidget *parent = nullptr);
    ~FaceSlider();

    // public inteface
    FaceSlider &setMaxUnhappyCount(size_t cnt);
    size_t maxUnhappyCount() const;

    FaceSlider &setFaceX(double x);
    double faceX() const;

    FaceSlider &setFaceY(double y);
    double faceY() const;

    FaceSlider &setHappiness(double happiness);
    double happiness() const;

    FaceSlider &setDrep(double drep);
    double drep() const;

    FaceSlider &setTitle(const QString &title);
    QString title() const;

    FaceSlider &setSubtitle(const QString &subtitle);
    QString subtitle() const;

    FaceSlider &setBtnHappyText(const QString &text);
    QString btnHappyText() const;

    FaceSlider &setBtnUnhappyText(const QString &text);
    QString btnUnhappyText() const;

    void apply();// MUST call it AFTER setting all properties that you need

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void changeEvent(QEvent *e) override;
    void timerEvent(QTimerEvent *e) override;

private slots:
    void do_btnHappy_clicked();
    void do_btnUnhappy_clicked();

private:
    Ui::Widget *ui;

    // set mouseTracking of all components to true
    void setMouseMoveEnabled(QWidget *parent);
    double calcHappiness(const QPointF &pos) const;
    void initUI();
    void initAnim();
    void startFaceAnim(double drep, double faceX = -1, double faceY = -1, double happiness = -1);
    void stopFaceAnim();
    void startButtonAnim(const QRect &endGeometry);
    void loadStyle();

    Q_PROPERTY(double faceX READ faceX WRITE setFaceX)
    Q_PROPERTY(double faceY READ faceY WRITE setFaceY)
    Q_PROPERTY(double happiness READ happiness WRITE setHappiness)
    Q_PROPERTY(double drep READ drep WRITE setDrep)
    QPropertyAnimation *anim_faceX_{};
    QPropertyAnimation *anim_faceY_{};
    QPropertyAnimation *anim_happiness_{};
    QPropertyAnimation *anim_drep_{};
    QPropertyAnimation *anim_happy_size_{};

    QSet<QObject *> controls_;
    size_t max_unhappy_ = 3;
    size_t reject_cnt_ = 0;
    QPushButton *btnHappy_{};
    QPushButton *btnUnhappy_{};
    QPointF mousePos_;
    int timerId_ = -1;
    bool leaving_ = false;
    bool btn_clicked_ = false;
    bool chinese_ = true;
    bool face_anim_starting_ = false;
};
#endif // FACESLIDER_H
