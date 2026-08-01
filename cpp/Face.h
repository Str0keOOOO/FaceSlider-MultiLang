#ifndef FACE_H
#define FACE_H

#include <QPainterPath>
#include <QWidget>

class QPropertyAnimation;

class Face : public QWidget
{
    Q_OBJECT

public:
    explicit Face(QWidget *parent);

    Face &setFaceX(double x);
    double faceX() const;
    Face &setFaceY(double y);
    double faceY() const;
    Face &setDrep(double drep);
    double drep() const;
    Face &setHappiness(double happiness);
    double happiness() const;
    void apply();

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void resizeEvent(QResizeEvent *e) override;
    void paintEvent(QPaintEvent *e) override;

private:
    class Eye : public QWidget
    {
    public:
        enum Position { Left, Right };
        Eye(Position pos, QWidget *parent);

        void setHappiness(double happiness);
        void updateBallPos(double x, double y, double drep);
        inline bool isLeft() const { return pos_ == Left; }

    protected:
        void paintEvent(QPaintEvent *e) override;

    private:
        QPointF ball_pos_;// the center point of the eye ball
        double ball_radius_{};
        Position pos_;
    } *eyes_[2]; // 0 is left eye and 1 is right eye

    class Blush : public QWidget
    {
    public:
        explicit Blush(QWidget *parent);
        double happiness_;

    protected:
        void paintEvent(QPaintEvent *e) override;
    } *blushes_[2];

    class Mouth : public QWidget
    {
    public:
        explicit Mouth(QWidget *parent);

        QPainterPath path_;

    protected:
        void paintEvent(QPaintEvent *e) override;
        void resizeEvent(QResizeEvent *e) override;

    private:
        QPointF teeth_pos_;
        QSizeF teeth_sz_;
        QPointF tongue_pos_;
        QSizeF tongue_sz_;
    } *mouth_;

    double x_;
    double y_;
    double drep_;
    double happiness_;

    static constexpr int BorderWidth = 2;
};

#endif // FACE_H
