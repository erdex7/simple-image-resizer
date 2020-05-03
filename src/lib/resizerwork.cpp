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

#include "resizerwork.h"

#include <QImage>
#include <QThread>
#include <QImageReader>
#include <QImageWriter>
#include <QDir>

ResizerWork::ResizerWork(const QStringList &_listFiles, const ImageParams &_params, QObject *parent)
    : QObject(parent)
{
    listFiles = _listFiles;
    params = _params;
    setAutoDelete(true);
}

void ResizerWork::run()
{
    QImage image;

    for (int i = 0; i < listFiles.size(); ++i) {
        if (stop) {
            break;
        }

        QImageReader reader(params.absolutePath + QStringLiteral("/") + listFiles[i]);
        image = reader.read();

        if (image.isNull()) {
            emit error(QStringLiteral("Error open file: \"") +
                       QDir::toNativeSeparators(params.absolutePath + QStringLiteral("/") +
                       listFiles[i]) + QStringLiteral("\": ") + reader.errorString());
            continue;
        }

        QImage image_out = image.scaledToHeight(params.h, params.smoothTransform ?
                                                    Qt::SmoothTransformation : Qt::FastTransformation);

        if (image_out.isNull()) {

            // Check out of memory
            int ht = params.h;
            int wt = float(image.width()) * (float(params.h) / float(image.height())) + 1;
            QImage image_tmp = QImage(ht, wt, image.format());
            if (image_tmp.isNull()) {
                emit error(QStringLiteral("Error resize image: \"") +
                           QDir::toNativeSeparators(params.absolutePath + QStringLiteral("/") +
                           listFiles[i]) + QStringLiteral("\": out of memory"));
            }
            else{
                emit error(QStringLiteral("Error resize image: \"") +
                           QDir::toNativeSeparators(params.absolutePath + QStringLiteral("/") +
                           listFiles[i]) + QStringLiteral("\""));
            }
            image_tmp = QImage();
            continue;
        }

        QImageWriter writer(params.outPath + QStringLiteral("/") + listFiles[i]);
        if (!writer.write(image_out)) {
            emit error(QStringLiteral("Error write file: \"") + QDir::toNativeSeparators(params.outPath
                       + QStringLiteral("/") + listFiles[i]) + QStringLiteral("\": ") + writer.errorString());
            continue;
        }

        emit fileProcessed(QDir::toNativeSeparators(params.absolutePath + QStringLiteral("/") +
                                                    listFiles[i]));

    }

    emit resizeReady();
}

void ResizerWork::needStop()
{
    stop = true;
}
