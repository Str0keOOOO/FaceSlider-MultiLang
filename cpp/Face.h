#ifndef FACE_H
#define FACE_H

#include <QPainterPath>
#include <QWidget>

namespace Ui {
class Face;
}

class QPropertyAnimation;

class Face : public QWidget
{
    Q_OBJECT

public:
    explicit Face(QWidget *parent);
    ~Face();

    Face &setFaceX(double x);
    double faceX() const;
    Face &setFaceY(double y);
    double faceY() const;
    Face &setDrep(double drep);
    double drep() const;
    Face &setHappiness(double happiness);
    double happiness() const;

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
        void resizeEvent(QResizeEvent *e) override;
        void paintEvent(QPaintEvent *e) override;

    private:
        class EyeBall : public QWidget
        {
        public:
            explicit EyeBall(Eye *parent);

        protected:
            void paintEvent(QPaintEvent *e) override;
        } *ball_;

        Position pos_;
    } *eyes_[2]; // 0 is left eye and 1 is right eye

    class Blush : public QWidget
    {
    public:
        explicit Blush(QWidget *parent);
        double happiness;

    protected:
        void paintEvent(QPaintEvent *e) override;
    } *blushes_[2];

    class Mouth : public QWidget
    {
    public:
        explicit Mouth(QWidget *parent);

        QPainterPath path;

    protected:
        void paintEvent(QPaintEvent *e) override;
        void resizeEvent(QResizeEvent *e) override;

    private:
        class Teeth : public QWidget
        {
        public:
            explicit Teeth(QWidget *parent);

        protected:
            void paintEvent(QPaintEvent *e) override;
        } *teeth_;

        class Tongue : public QWidget
        {
        public:
            explicit Tongue(QWidget *parent);

        protected:
            void paintEvent(QPaintEvent *e) override;
        } *tongue_;
    } *mouth_;

    Ui::Face *ui;
    double x_;
    double y_;
    double drep_;
    double happiness_;

    static const unsigned BorderWidth = 2;
};

#endif // FACE_H
