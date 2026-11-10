#include "main_window.h"
#include <QMessageBox>
#include <exception>
#include "storage.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Robotics Task Tracker");
    resize(900, 600);

    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_taskList = new QListWidget(m_splitter);
    m_scoreTable = new QTableWidget(m_splitter);

    m_scoreTable->setColumnCount(3);
    m_scoreTable->setHorizontalHeaderLabels({"Date", "Expected", "Actual"});

    m_splitter->setStretchFactor(1, 1);
    setCentralWidget(m_splitter);
    loadTaskList();
}

void MainWindow::loadTaskList() {
    try {
        m_connStr = loadConnString("tracker.env");
        m_missions = loadMissions(m_connStr, m_seasonId);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Connection error", e.what());
        return;
    }

    m_taskList->clear();
    for (const Mission& mission : m_missions) {
        for (const Task& task : mission.tasks) {
            m_taskList->addItem(QString::fromStdString(mission.name + " - " + task.name));
        }
    }
}