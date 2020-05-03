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

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QElapsedTimer>

class QPushButton;
class QCheckBox;
class QTextEdit;
class QSpinBox;
class QLabel;
class Resizer;
class QProgressBar;
class QGroupBox;
class QVBoxLayout;

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent*);

private slots:
    void slotOpenFile();
    void slotOpenDir();
    void slotStartResize();
    void slotFinished();
    void slotChangeOutDir();
    void slotPercentChanged(int);
    void slotAppendInfo(const QString&);
    void slotAppendError(const QString&);

private:
    enum State{STARTED, STOPPED};

    QGroupBox* getSettingsGroup();
    QGroupBox* getInfoGroup();
    QVBoxLayout* getButtonsLayout();
    void procPath();
    void updateButtons();

    QProgressBar* progressBar;
    QPushButton* startButton;
    QPushButton* openFileButton;
    QPushButton* openDirButton;
    QPushButton* changeDirButton;
    QCheckBox* smoothCheckBox;
    QCheckBox* lowCPUCheckBox;
    QCheckBox* showDetails;
    QTextEdit* logs;
    QSpinBox* heightSpinBox;
    QLabel* pathLabel;
    QStringList paths; // input
    State currentState = STOPPED;
    Resizer* resizer;
    QElapsedTimer timer;
    const QString titleName = "Image Resizer";
};

#endif // WIDGET_H
