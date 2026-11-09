#include "main_window.h"
#include <QMessageBox>
#include <exception>
#include "storage.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Robotics Task Tracker");
    resize(900, 600);

    m_taskList = new QListWidget(this);
    setCentralWidget(m_taskList);
    loadTaskList();
}

void MainWindow::loadTaskList() {
    try {
        m_connStr = loadConnString("tracker.env");
        m_tasks = loadTasks(m_connStr, m_seasonId);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Connection error", e.what());
        return;
    }

    m_taskList->clear();
    for (const Task& task : m_tasks) {
        m_taskList->addItem(QString::fromStdString(task.name));
    }
}