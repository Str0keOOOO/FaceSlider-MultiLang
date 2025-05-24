#ifndef STYLELOADER_H
#define STYLELOADER_H
#include <QByteArray>

class QWidget;

class StyleLoader
{
public:
    StyleLoader &append(const QString &filename);
    StyleLoader &operator<<(const QString &filename);
    void load(QWidget *w);

private:
    QByteArray d_;
};

#endif // STYLELOADER_H
