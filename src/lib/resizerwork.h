/*
 * Copyright 2020, Ewgeny Repin <erpndev@gmail.com>
 *
 * This file is part of Simple image resizer.
 *
 * Simple image resizer is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Simple image resizer is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Simple image resizer.  If not, see <https://www.gnu.org/licenses/>.
 *
 */

#ifndef RESIZERWORK_H
#define RESIZERWORK_H

#include <QObject>
#include <QRunnable>

struct ImageParams
{
    QString absolutePath;
    QString outPath;
    bool smoothTransform;
    int h;
};

class ResizerWork : public QObject, public QRunnable
{
    Q_OBJECT

public:
    ResizerWork(const QStringList& _listFiles, const ImageParams& _params, QObject* parent = nullptr);    
    void run();

signals:
    void resizeReady();
    void fileProcessed(const QString&);
    void error(const QString&);

public slots:    
    void needStop();

private:
    QStringList listFiles;
    ImageParams params;
    bool stop = false;
    int cntProcessed = 0;
};

#endif // RESIZERWORK_H
