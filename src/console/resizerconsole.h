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

#ifndef RESIZERCONSOLE_H
#define RESIZERCONSOLE_H

#include <QObject>

class ResizerConsole : public QObject
{
    Q_OBJECT

public:
    ResizerConsole(QObject *parent = nullptr);

    QString getDestPath() const;
    QString getSourcePath() const;
    int getHeight() const;
    bool getSmooth() const;
    bool getLowCPUMode() const;

public slots:
    void slotPercentComplete(int);
    void slotErrorMessage(const QString&);

private:
    void readOptions();
    void pathProcessing();

    QString destPath;
    QString sourcePath;
    int h;
    bool typeIsFile;
    bool smooth;
    bool lowCPU;

};

#endif // RESIZERCONSOLE_H
