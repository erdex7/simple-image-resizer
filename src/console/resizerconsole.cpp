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

#include "resizerconsole.h"

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QTextStream>
#include <QDir>

QTextStream cout(stdout);
QTextStream cin(stdin);

ResizerConsole::ResizerConsole(QObject *parent) : QObject(parent)
{
    readOptions();
    pathProcessing();
}

QString ResizerConsole::getDestPath() const
{
    return QDir::toNativeSeparators(destPath);
}

QString ResizerConsole::getSourcePath() const
{
    return QDir::toNativeSeparators(sourcePath);
}

int ResizerConsole::getHeight() const
{
    return h;
}

bool ResizerConsole::getSmooth() const
{
    return smooth;
}

bool ResizerConsole::getLowCPUMode() const
{
    return lowCPU;
}

void ResizerConsole::slotPercentComplete(int percComplete)
{
    // To show progress
    const int percentsInLabel = 4;
    int n = 100 / percentsInLabel;
    int needShow = percComplete / percentsInLabel;
    cout << "Processing: [#";
    for (int i = 0; i < n - 1; ++i) {
        if (i < needShow) {
            cout << '#';
        }
        else {
            cout << '-';
        }
    }
    cout << "] " << percComplete << "%\r";
    cout.flush();
}

void ResizerConsole::slotErrorMessage(const QString &str)
{
    cout << str << '\n';
    cout.flush();
}

void ResizerConsole::readOptions()
{
    // Init command line parser
    QCommandLineParser parser;
    parser.setApplicationDescription("\nSimple Image Resizer");

    QCommandLineOption pathFileOption({"f", "file"}, "Path to image. One image will be process", "path");
    QCommandLineOption pathDirOption({"d", "directory"}, "Path to directory with images (not recursive)", "path");
    QCommandLineOption outPathOption({"o", "out"}, "Path to output directory", "path");
    QCommandLineOption heightOption({"h", "height"}, "New height image in pixels", "value");
    QCommandLineOption noSmoothOption({"n", "nosmooth"}, "No smoothing. Quickly resize");
    QCommandLineOption lowCPUModeOption({"l", "low"}, "Lower CPU and memory usage");

    parser.addOption(pathFileOption);
    parser.addOption(pathDirOption);
    parser.addOption(outPathOption);
    parser.addOption(heightOption);
    parser.addOption(noSmoothOption);
    parser.addOption(lowCPUModeOption);
    parser.process(*qApp);

    // Check command line options
    bool dirSet = parser.isSet(pathDirOption);
    bool fileSet = parser.isSet(pathFileOption);
    bool heightSet = parser.isSet(heightOption);
    if ((!fileSet && !dirSet) ||
        (fileSet && dirSet) ||
        (!heightSet)) {
        parser.showHelp(1);
    }

    typeIsFile = fileSet;

    // Read options
    sourcePath = dirSet ? parser.value(pathDirOption) : parser.value(pathFileOption);
    destPath = parser.value(outPathOption);
    h = parser.value(heightOption).toInt();
    smooth = !parser.isSet(noSmoothOption);
    lowCPU = parser.isSet(lowCPUModeOption);
}

void ResizerConsole::pathProcessing()
{
    if (typeIsFile && !QFileInfo(sourcePath).isFile()) {
        cout << "Error: \"" << sourcePath << "\" file not found\n\n";
        exit(1);
    }
    if (!typeIsFile && !QFileInfo(sourcePath).isDir()) {
        cout << "Error: \"" << sourcePath << "\" directory not found\n\n";
        exit(1);
    }
    if (destPath.isEmpty()) {
        destPath = QFileInfo(sourcePath).absolutePath();
        destPath = QDir::toNativeSeparators(destPath);
        if (destPath.at(destPath.size() - 1) != QDir::separator()) {
            destPath.push_back(QDir::separator());
        }
		destPath += "out";
    }

    // Check out path
    QString fullOutPath = typeIsFile ? destPath + QDir::separator() + QFileInfo(sourcePath).fileName() : destPath;

    QFileInfo fileInfo(fullOutPath);
    QString type = fileInfo.isDir() ? "Directory" : "File";
    if (QFileInfo::exists(fullOutPath)) {
        cout << type << " \"" << fullOutPath <<"\" " << "already exists. "
                                                        "Do you want to overwrite (Y/N)?\n";
        cout.flush();
        char ch;
        cin >> ch;
        if (ch != 'y' && ch != 'Y') {
            exit(1);
        }
    }
}
