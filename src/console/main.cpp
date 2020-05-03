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

#include <QCoreApplication>
#include <QTextStream>

#include <csignal>

#include "resizerconsole.h"
#include "resizer.h"

void sigHandler(int);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    ResizerConsole console;

    Resizer resizer;

    QObject::connect(&resizer, &Resizer::finished, &a, &QCoreApplication::quit, Qt::QueuedConnection);
    QObject::connect(&resizer, &Resizer::percentCompleted, &console, &ResizerConsole::slotPercentComplete);
    QObject::connect(&resizer, &Resizer::errorMessage, &console, &ResizerConsole::slotErrorMessage);

    QString outPath = console.getDestPath();
    QString inPath = console.getSourcePath();
    bool lowCPU = console.getLowCPUMode();
    bool smooth = console.getSmooth();
    int h = console.getHeight();

    signal(SIGINT, sigHandler); // Signal by the application user

    QTextStream cout(stdout);
    cout << "Image resizer working\n";
    cout.flush();

    resizer.setOutPath(outPath);
    resizer.startResize(inPath, h, smooth, lowCPU);

    int result = a.exec();

    if (resizer.errorsOccured()) { // Show info before exit
        cout << "\n\nCompleted with errors\n";
    }
    else {
        cout << "\n\nSuccess completed: " << resizer.countProcessed()
                  << " images resized\n";
    }
    cout << "Output path: \"" << resizer.getOutPath() << "\"\n";

    return result;
}

void sigHandler(int signum)
{
    QTextStream cout(stdout);
    cout << "\n\n(" << signum  << ") Application stopped by user\n";
    cout.flush();

    QCoreApplication::quit();
}
