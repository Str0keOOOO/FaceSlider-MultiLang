#include "StyleLoader.h"

#include <QFile>
#include <QWidget>

StyleLoader &StyleLoader::append(const QString &filename)
{
    return *this << filename;
}

StyleLoader &StyleLoader::operator<<(const QString &filename)
{
    QFile file(filename);
    file.open(QIODevice::ReadOnly);
    auto data = file.readAll();
    file.close();
    d_.append(data.append('\n'));
    return *this;
}

void StyleLoader::load(QWidget *w)
{
    w->setStyleSheet(d_);
}
