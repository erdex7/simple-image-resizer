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

#include "widget.h"
#include "resizer.h"

#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QSpinBox>
#include <QBoxLayout>
#include <QFileDialog>
#include <QTextEdit>
#include <QProgressBar>
#include <QGroupBox>
#include <QFontMetrics>
#include <QResizeEvent>
#include <QMessageBox>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{    
    setWindowTitle(titleName);

    resizer = new Resizer(this);
    connect(resizer, &Resizer::finished, this, &Widget::slotFinished);
    connect(resizer, &Resizer::errorMessage, this, &Widget::slotAppendError);
    connect(resizer, &Resizer::fileComplete, this, &Widget::slotAppendInfo);
    connect(resizer, &Resizer::percentCompleted, this, &Widget::slotPercentChanged);

    QGroupBox* settingsGroup = getSettingsGroup();
    QGroupBox* infoGroup = getInfoGroup();
    QVBoxLayout* buttonsLayout = getButtonsLayout();

    auto mainLayout = new QVBoxLayout;
    mainLayout->addWidget(settingsGroup);
    mainLayout->addWidget(infoGroup);
    mainLayout->addLayout(buttonsLayout);

    resize(400, 500);

    setLayout(mainLayout);
 }

void Widget::resizeEvent(QResizeEvent* e)
{ 
    QFontMetrics metrics(pathLabel->font());
    QString new_text = resizer->getOutPath();

    int text_len = metrics.boundingRect(new_text).width();
    int lbl_len = e->size().width() / 2;

    if (text_len > lbl_len) {
        int posRemove = 0;
        while(text_len > lbl_len - 10) {
            posRemove = new_text.size() / 2;
            new_text.remove(posRemove, 1);
            text_len = metrics.size(Qt::TextSingleLine, new_text).width();
        }
        new_text.insert(posRemove, "...");
    }

    pathLabel->setText(new_text);

    QWidget::resizeEvent(e);
}

QGroupBox* Widget::getSettingsGroup()
{
    auto heightLabel = new QLabel("New height (pix): ");

    heightSpinBox = new QSpinBox;
    heightSpinBox->setMaximum(1000000);

    auto heightLayout = new QHBoxLayout;
    heightLayout->addWidget(heightLabel);
    heightLayout->addWidget(heightSpinBox);
    heightLayout->addStretch(1);

    smoothCheckBox = new QCheckBox("Smooth");
    smoothCheckBox->setChecked(true);
    lowCPUCheckBox = new QCheckBox("Low CPU and memory usage");
    showDetails = new QCheckBox("Show details");

    auto checkBoxLayout = new QHBoxLayout;
    checkBoxLayout->addWidget(smoothCheckBox);
    checkBoxLayout->addWidget(lowCPUCheckBox);
    checkBoxLayout->addWidget(showDetails);

    auto settingsLayout = new QVBoxLayout;
    settingsLayout->addLayout(heightLayout);
    settingsLayout->addLayout(checkBoxLayout);

    auto settingsGroup = new QGroupBox("Settings");
    settingsGroup->setLayout(settingsLayout);

    return settingsGroup;
}


QGroupBox* Widget::getInfoGroup()
{
    logs = new QTextEdit;
    logs->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    logs->setReadOnly(true);

    progressBar = new QProgressBar;
    progressBar->setAlignment(Qt::AlignCenter);
    progressBar->setRange(0, 100);

    auto infoLayout = new QVBoxLayout;
    infoLayout->addWidget(logs);
    infoLayout->addWidget(progressBar);

    auto groupBox = new QGroupBox("Process");
    groupBox->setLayout(infoLayout);

    return groupBox;
}

QVBoxLayout* Widget::getButtonsLayout()
{
    pathLabel = new QLabel;
    openFileButton = new QPushButton("&File(s)");
    openDirButton = new QPushButton("&Directory");
    startButton = new QPushButton("&Start");
    changeDirButton = new QPushButton("&Change");
    startButton->setEnabled(false);    

    openFileButton->setToolTip("Resize one or more images");
    openDirButton->setToolTip("Resize all images in directory");
    changeDirButton->setToolTip("Set output directory");

    connect(openDirButton, &QPushButton::clicked, this, &Widget::slotOpenDir);
    connect(openFileButton, &QPushButton::clicked, this, &Widget::slotOpenFile);
    connect(startButton, &QPushButton::clicked, this, &Widget::slotStartResize);
    connect(changeDirButton, &QPushButton::clicked, this, &Widget::slotChangeOutDir);

    auto openLayout = new QVBoxLayout;
    openLayout->addWidget(openFileButton);
    openLayout->addWidget(openDirButton);
    auto openGroup = new QGroupBox("Open");
    openGroup->setLayout(openLayout);

    auto saveLayout = new QVBoxLayout;
    saveLayout->addWidget(pathLabel);
    saveLayout->addWidget(changeDirButton);
    auto saveGroup = new QGroupBox("Save");
    saveGroup->setLayout(saveLayout);

    auto filesLayout = new QHBoxLayout;
    filesLayout->addWidget(openGroup);
    filesLayout->addWidget(saveGroup);

    auto buttonsLayout = new QVBoxLayout;
    buttonsLayout->addLayout(filesLayout);
    buttonsLayout->addWidget(startButton);

    return buttonsLayout;
}

void Widget::slotOpenDir()
{
    paths.clear();
    QString path = QFileDialog::getExistingDirectory(this, "Select directory");
    if (path.isEmpty()) {
        return;
    }
    paths << path;
    procPath();
}

void Widget::slotOpenFile()
{
    paths.clear();
    paths = QFileDialog::getOpenFileNames(this, "Open files");
    procPath();
}

void Widget::slotStartResize()
{
    if (currentState == STOPPED) {

        if (QFileInfo::exists(resizer->getOutPath())) {
            QDir outDir(resizer->getOutPath());
            outDir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
            if (outDir.count() != 0) {
                int btn = QMessageBox::warning(this, "Warning", "Path \"" + resizer->getOutPath() +
                                               "\" already exists and is not empty. Do you want to overwrite?",
                                               QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
                if (btn != QMessageBox::Yes) {
                    return;
                }
            }
        }

        logs->clear();
        timer.start();

        currentState = STARTED;
        updateButtons();

        resizer->startResize(paths, heightSpinBox->value(), smoothCheckBox->isChecked(), lowCPUCheckBox->isChecked());
    }
    else if (currentState == STARTED) {        
        resizer->stopResize();
    }
}

void Widget::slotFinished()
{
    logs->append("\nProcessed " + QString::number(resizer->countProcessed()) +
                 " images in " + QString::number(timer.elapsed() / 1000) + " sec");

    progressBar->reset();
    currentState = STOPPED;
    updateButtons();
    setWindowTitle(titleName);
}

void Widget::slotChangeOutDir()
{
    QString path = QFileDialog::getExistingDirectory(this, "Select Directory");
    if (path.isEmpty()) {
        return;
    }
    resizer->setOutPath(path);
    pathLabel->setText(resizer->getOutPath());
    pathLabel->setToolTip(resizer->getOutPath());

}

void Widget::slotPercentChanged(int res)
{
    progressBar->setValue(res);
    setWindowTitle(titleName + " - " + QString::number(res) + "% Completed");
}

void Widget::slotAppendInfo(const QString &str)
{
    if (showDetails->isChecked()) {
        logs->append(str);
    }
}

void Widget::slotAppendError(const QString &str)
{
    logs->append(str);
}

void Widget::procPath()
{    
    if (paths.isEmpty()) { // No input images
        startButton->setEnabled(false);
        pathLabel->clear();
        return;
    }
    startButton->setEnabled(true);

    if (resizer->getOutPath().isEmpty()) { // No output path
        QFileInfo fileInfo(paths.at(0));
        QString absPath(fileInfo.absolutePath());
        if (absPath.at(absPath.size() - 1) == '/') {
            absPath.remove(absPath.size() - 1, 1);
        }
        QString path(absPath + "/out");
        resizer->setOutPath(path);
    }
    pathLabel->setText(resizer->getOutPath());
}

void Widget::updateButtons()
{
    bool res = currentState == STOPPED;
    if (!res) {
        startButton->setText("&Stop");
    }
    else {
        startButton->setText("&Start");
        procPath();
    }
    openFileButton->setEnabled(res);
    openDirButton->setEnabled(res);
    changeDirButton->setEnabled(res);
    lowCPUCheckBox->setEnabled(res);
    heightSpinBox->setEnabled(res);
    smoothCheckBox->setEnabled(res);
}
