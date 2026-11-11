#include "main_window.h"
#include <QColor>
#include <QHeaderView>
#include <QMessageBox>
#include <QInputDialog>
#include <exception>
#include "storage.h"

//color for action rows like + Add task
static const QColor kActionColor(140, 140, 140);

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Robotics Task Tracker");
    resize(900, 600);

    m_splitter = new QSplitter(Qt::Horizontal, this);

    m_taskTree = new QTreeWidget(m_splitter);
    m_taskTree->setHeaderHidden(true);
    connect(m_taskTree, &QTreeWidget::itemClicked, this, &MainWindow::onTreeItemClicked);
    m_scoreTable = new QTableWidget(0, 4, m_splitter);
    m_scoreTable->setHorizontalHeaderLabels({"Date", "Successes", "Reliability", "Expected"});
    m_scoreTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_scoreTable->horizontalHeader()->setStretchLastSection(true);

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

    m_taskTree->clear();
    for (const Mission& mission : m_missions) {
        auto* missionItem = new QTreeWidgetItem(m_taskTree);
        missionItem->setText(0, QString::fromStdString(mission.name));
        for (const Task& task : mission.tasks) {
            auto* taskItem = new QTreeWidgetItem(missionItem);
            taskItem->setText(0, QString::fromStdString(task.name));
        }
        missionItem->setExpanded(true);
        auto* addItem = new QTreeWidgetItem(missionItem);
        addItem->setText(0, "+ Add task");
        addItem->setData(0, Qt::UserRole, static_cast<int>(&mission - m_missions.data()));
        addItem->setForeground(0, kActionColor);
    }
}

void MainWindow::onTreeItemClicked(QTreeWidgetItem* item, int column) {
    QVariant data = item->data(0, Qt::UserRole);
    if (!data.isValid()) {
        return;
    }
    int missionIndex = data.toInt();
    const Mission& mission = m_missions[missionIndex];
    bool ok = false;
    QString name = QInputDialog::getText(this, "Add task", "Task name:", QLineEdit::Normal, "", &ok);
    if (!ok || name.trimmed().isEmpty()) {
        return;
    }
    int points = QInputDialog::getInt(this, "Add task", "Max points:", 0, 0, 1000, 1, &ok);
    if (!ok) {
        return;
    }
    try {
        saveTask(m_connStr, mission.id, name.trimmed().toStdString(), points);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Save error", e.what());
        return;
    }
    loadTaskList();
}