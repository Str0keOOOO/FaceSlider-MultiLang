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

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void contextMenuEvent(QContextMenuEvent *e) override;
    void showEvent(QShowEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void enterEvent(QEnterEvent *e) override;
    void leaveEvent(QEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void changeEvent(QEvent *e) override;

private slots:
    void do_btnHappy_clicked();
    void do_btnUnhappy_clicked();

private:
    Ui::Widget *ui;

    // 设置所有组件mouseTracking为true且需要传递给父组件
    void setMouseMoveEnabled(QWidget *parent);
    double calcHappiness(const QPointF &pos);
    void initUI();
    void initAnim();
    void startAnim(double drep);
    void loadStyle();

    QSet<QObject *> controls_;
    bool btn_clicked_ = false;
    bool chinese_ = true;
    size_t max_unhappy_ = 3;
    size_t reject_cnt_ = 0;
    QPushButton *btnHappy_;
    QPushButton *btnUnhappy_;

    Q_PROPERTY(double drep READ drep WRITE setDrep)
    QPropertyAnimation *anim_drep_;
    QPropertyAnimation *anim_happy_size_;
};
#endif // FACESLIDER_H
