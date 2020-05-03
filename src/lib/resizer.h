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

#ifndef RESIZER_H
#define RESIZER_H

#if defined RESIZER_LIB
#define RESIZER_DLLSPEC Q_DECL_EXPORT
#else
#define RESIZER_DLLSPEC Q_DECL_IMPORT
#endif

#include <QObject>
#include <QVector>

#include "resizerwork.h"

class QThreadPool;

class RESIZER_DLLSPEC Resizer : public QObject
{
    Q_OBJECT

public:
    Resizer(QObject* parent = nullptr);
    ~Resizer();

    void startResize(QString path, int hght, bool smoothTransform = true,
                     bool lowMode = false);
    void startResize(QStringList paths, int hght, bool smoothTransform = true,
                     bool lowMode = false);
    void stopResize();
    void setOutPath(const QString&);
    void seClearThreadsOnClose(bool b);

    QStringList getStatusStrings() const;
    QString getOutPath() const;
    bool errorsOccured() const;
    int countFiles() const;
    int countProcessed() const;

signals:
    void finished();
    void errorMessage(const QString&);
    void stopWorkers();
    void percentCompleted(int);
    void fileComplete(const QString&);

private slots:
    void handleWork();
    void fileProcessed(const QString&);
    void addError(const QString&);

private:
    bool checkPath(QStringList &paths);
    QStringList getListFiles(const QStringList& paths);
    void startWork(const QStringList& paths, const ImageParams& params);

    QStringList statusStrings;
    QString outPath;
    QVector<QStringList> vecPaths;
    static QThreadPool threadPool;

    bool clearThreadsOnClose = true;
    int cntErr = 0;
    int cntFiles = 0;
    int cntProcFiles = 0;
    int threadCount = 0;
    int threadsLeft = 0;
};

#endif // RESIZER_H
