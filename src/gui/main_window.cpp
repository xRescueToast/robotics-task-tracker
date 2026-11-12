#include "main_window.h"
#include <QColor>
#include <QHeaderView>
#include <QMessageBox>
#include <exception>
#include "storage.h"

//color for action rows like + Add task
static const QColor kActionColor(140, 140, 140);

//builds the window and its widgets, then loads the data
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Robotics Task Tracker");
    resize(900, 600);


    //TEMPORARY
    //splitter holds the left and right panels side by side with a draggable divider
    m_splitter = new QSplitter(Qt::Horizontal, this);

    //left panel: mission and task tree
    m_taskTree = new QTreeWidget(m_splitter);
    m_taskTree->setHeaderHidden(true);
    //call onTreeItemClicked whenever a row in the tree is clicked
    connect(m_taskTree, &QTreeWidget::itemClicked, this, &MainWindow::onTreeItemClicked);

    //stack on the right side, shows one page at a time
    m_rightStack = new QStackedWidget(m_splitter);
    m_scoreTable = new QTableWidget(0, 4);
    m_scoreTable->setHorizontalHeaderLabels({"Date", "Successes", "Reliability", "Expected"});
    //read only, edits will go through forms so storage saves exactly what changed
    m_scoreTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    //last column fills the leftover width so there is no empty strip
    m_scoreTable->horizontalHeader()->setStretchLastSection(true);

    //add task form, page 1 of the stack
    m_addTaskPage = new AddTaskPage;
    m_rightStack->addWidget(m_scoreTable);
    m_rightStack->addWidget(m_addTaskPage);
    connect(m_addTaskPage, &AddTaskPage::saveRequested, this, &MainWindow::onAddTaskSaved);
    connect(m_addTaskPage, &AddTaskPage::cancelled, this, [this]() { m_rightStack->setCurrentIndex(0); });

    //right panel (index 1) gets the extra space when the window is resized
    m_splitter->setStretchFactor(1, 1);
    //splitter fills the main window
    setCentralWidget(m_splitter);
    loadTaskList();
}

//loads missions and tasks from the database and rebuilds the tree
void MainWindow::loadTaskList() {
    //storage throws on connection or query errors shows them in a popup
    try {
        m_connStr = loadConnString("tracker.env");
        m_missions = loadMissions(m_connStr, m_seasonId);
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Connection error", e.what());
        return;
    }

    //remove the old rows so reloading does not duplicate them
    m_taskTree->clear();
    for (const Mission& mission : m_missions) {
        //top level row for the mission created before the task loop so empty missions still show
        auto* missionItem = new QTreeWidgetItem(m_taskTree);
        missionItem->setText(0, QString::fromStdString(mission.name));
        //child rows for each task indented under the mission
        for (const Task& task : mission.tasks) {
            auto* taskItem = new QTreeWidgetItem(missionItem);
            taskItem->setText(0, QString::fromStdString(task.name));
        }
        //open the mission so its tasks are visible right away
        missionItem->setExpanded(true);
        //last child row is the gray + Add task action
        auto* addItem = new QTreeWidgetItem(missionItem);
        addItem->setText(0, "+ Add task");
        //hidden data: the mission's index in m_missions reads back when the row is clicked
        addItem->setData(0, Qt::UserRole, static_cast<int>(&mission - m_missions.data()));
        addItem->setForeground(0, kActionColor);
    }
}

//runs when any tree row is clicked
void MainWindow::onTreeItemClicked(QTreeWidgetItem* item, int column) {
    //only + Add task rows have hidden data, so mission and task rows return early
    QVariant data = item->data(0, Qt::UserRole);
    if (!data.isValid()) {
        return;
    }
    int missionIndex = data.toInt();
    //remember the mission and show the form
    m_addTaskMission = missionIndex;
    m_addTaskPage->reset();
    m_rightStack->setCurrentIndex(1);
}

void MainWindow::onAddTaskSaved(const QString& name, int points) {
    //guard against a bad mission index
    if (m_addTaskMission < 0 || m_addTaskMission >= static_cast<int>(m_missions.size())) {
        return;
    }
    const Mission& mission = m_missions[m_addTaskMission];

    //save to the database
    try {
        saveTask(m_connStr, mission.id, name.toStdString(), points);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Save error", e.what());
        return;
    }

    //refresh the tree and go back to the table
    loadTaskList();
    m_rightStack->setCurrentIndex(0);
}