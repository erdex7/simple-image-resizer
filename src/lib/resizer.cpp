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

#include "resizer.h"
#include "resizerwork.h"

#include <QImage>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QMutex>

QThreadPool Resizer::threadPool;

Resizer::Resizer(QObject* parent) : QObject(parent)
{

}

Resizer::~Resizer()
{
    if (clearThreadsOnClose) {
        threadPool.clear();
        emit stopWorkers();
        threadPool.waitForDone(10000);
    }

}

QStringList Resizer::getStatusStrings() const
{
    return statusStrings;
}

QString Resizer::getOutPath() const
{
    return QDir::toNativeSeparators(outPath);
}

bool Resizer::errorsOccured() const
{
    return cntErr != 0;
}

int Resizer::countFiles() const
{
    return cntFiles;
}

int Resizer::countProcessed() const
{
    return cntProcFiles;
}

void Resizer::handleWork()
{
    --threadsLeft;
    if (threadsLeft == 0) {
        emit finished();
    }
}

void Resizer::fileProcessed(const QString& str)
{
    cntProcFiles++;
    int onePerc = cntFiles / 100;
    if (onePerc == 0) {
        onePerc = 1;
    }
    if (cntProcFiles % onePerc == 0) {
        emit percentCompleted((float(cntProcFiles) / float(cntFiles)) * 100.0);
    }
    emit fileComplete(str);
}

bool Resizer::checkPath(QStringList& paths)
{
    if (paths.isEmpty()) {
        return false;
    }

    for (int i = 0; i < paths.size(); ++i) {
        QString path = paths[i];
        QFileInfo fInfo(path);

        if (!fInfo.isDir() && !fInfo.isFile()) {
            addError(QStringLiteral("Error open from \"") + QDir::toNativeSeparators(path) +
                     QStringLiteral("\""));
            return false;
        }

        if (fInfo.isDir() && path.at(path.size() - 1) != '/') {
            paths[i].push_back('/');
        }
    }

    return true;
}

QStringList Resizer::getListFiles(const QStringList &paths)
{    
    QFileInfo fileInfo(paths.at(0));

    QStringList resList;

    if (!fileInfo.isDir()) {
        for (QString path : paths) {
            fileInfo.setFile(path);
            resList.push_back(fileInfo.fileName());
        }
    }
    else {
        QDir dirImages(paths.at(0));
        resList = dirImages.entryList(QDir::Files);
    }

    return resList;
}

void Resizer::startWork(const QStringList &paths, const ImageParams& params)
{
    ResizerWork* resWrk = new ResizerWork(paths, params);
    connect(resWrk, &ResizerWork::fileProcessed, this, &Resizer::fileProcessed, Qt::QueuedConnection);
    connect(resWrk, &ResizerWork::resizeReady, this, &Resizer::handleWork, Qt::QueuedConnection);
    connect(resWrk, &ResizerWork::error, this, &Resizer::addError);
    connect(this, &Resizer::stopWorkers, resWrk, &ResizerWork::needStop);
    threadPool.start(resWrk);    
    ++threadsLeft;
}

void Resizer::addError(const QString &errStr)
{
    statusStrings.push_back(errStr);
    emit errorMessage(errStr);
    ++cntErr;
}

void Resizer::startResize(QString path, int hght, bool smoothTransform, bool lowMode)
{
    if (path.isEmpty()) {
        return;
    }

    startResize(QStringList() << path, hght, smoothTransform, lowMode);
}

void Resizer::startResize(QStringList paths, int hght, bool smoothTransform, bool lowMode)
{
    if (hght == 0) {
        addError(QStringLiteral("Incorrect height"));
        emit finished();
        return;
    }

    cntErr = 0;
    cntProcFiles = 0;

    if (!checkPath(paths)) {
        addError(QStringLiteral("Incorrect path"));
        emit finished();
        return;
    }

    QFileInfo fileInfo(paths.at(0));
    QString _outPath = outPath;
    if (_outPath.isEmpty()) {
        addError(QStringLiteral("Destination directory is not set"));
        emit finished();
        return;
    }
    QDir outDir(_outPath);
    if (!outDir.exists()) {
        outDir.mkdir(_outPath);
    }

    QStringList listFiles = getListFiles(paths);
    if (listFiles.isEmpty()) {
        addError(QStringLiteral("Error open images"));
        emit finished();
        return;
    }
    cntFiles = listFiles.size();

    lowMode ? threadCount = 1 : threadCount = QThread::idealThreadCount();
    const int maxThreads = threadCount;    
    threadPool.setMaxThreadCount(maxThreads);

    if (listFiles.size() < threadCount) {
        threadCount = listFiles.size();
    }

    const int cntInThread = listFiles.size() / threadCount;

    ImageParams params;
    params.h = hght;
    params.smoothTransform = smoothTransform;
    params.outPath = _outPath;
    params.absolutePath = fileInfo.absolutePath();

    QVector<QStringList> vecPaths;
    for (int i = 0; i < threadCount; ++i) {
        QStringList list;
        for (int j = 0; j < cntInThread; ++j) {
            list.append(listFiles.at(i * cntInThread + j));
        }        
        vecPaths.push_back(list);
    }

    for (int i = threadCount * cntInThread; i < listFiles.size(); ++i) {
        vecPaths.last().append(listFiles.at(i));
    }

    while(!vecPaths.isEmpty()) {
        startWork(vecPaths.back(), params);
        vecPaths.pop_back();
    }
}

void Resizer::stopResize()
{
    emit stopWorkers();
}

void Resizer::setOutPath(const QString& str)
{
    outPath = str;
	
	if (!outPath.isEmpty()) {
		if (outPath.at(outPath.size() - 1) == QDir::separator()) {
			outPath.remove(outPath.size() - 1, 1);
		}
	}
}

void Resizer::seClearThreadsOnClose(bool b)
{
    clearThreadsOnClose = b;
}
