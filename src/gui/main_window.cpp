#include <QColor>
#include <QFrame>
#include <QHeaderView>
#include <QMessageBox>
#include <QVBoxLayout>
#include <exception>
#include "storage.h"
#include "tree_item_delegate.h"
#include "tree_roles.h"
#include "main_window.h"

//color for action rows like + Add task
static const QColor kActionColor(140, 140, 140);

//builds the window and its widgets, then loads the data
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("Robotics Task Tracker");
    resize(900, 600);

    //splitter holds the left and right panels side by side with a draggable divider
    m_splitter = new QSplitter(Qt::Horizontal, this);

    //sidebar panel: tree and button share one frame and background
    auto* sidebar = new QFrame(m_splitter);
    sidebar->setFrameShape(QFrame::StyledPanel);
    //paint the sidebar the same color as the tree so it reads as one box
    sidebar->setBackgroundRole(QPalette::Base);
    sidebar->setAutoFillBackground(true);
    auto* sidebarLayout = new QVBoxLayout(sidebar);
    //small gap so the button sits inside the panel
    sidebarLayout->setContentsMargins(4, 4, 4, 4);
    m_taskTree = new QTreeWidget;
    //no frame on the tree, the sidebar draws the border
    m_taskTree->setFrameShape(QFrame::NoFrame);
    m_addMissionButton = new QPushButton("+ Add mission");
    sidebarLayout->addWidget(m_taskTree);
    sidebarLayout->addWidget(m_addMissionButton);
    m_taskTree->setHeaderHidden(true);

    //hover icons for edit and delete
    m_taskTree->setMouseTracking(true);
    m_taskTree->setItemDelegate(new TreeItemDelegate(m_taskTree));
    
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

    //add mission form page 2 of the stack
    m_addMissionPage = new AddMissionPage;
    m_rightStack->addWidget(m_addMissionPage);
    connect(m_addMissionPage, &AddMissionPage::saveRequested, this, &MainWindow::onAddMissionSaved);
    connect(m_addMissionPage, &AddMissionPage::cancelled, this, [this]() { m_rightStack->setCurrentIndex(0); });

    //add mission button opens the form
    connect(m_addMissionButton, &QPushButton::clicked, this, [this]() {
        m_addMissionPage->reset();
        m_rightStack->setCurrentIndex(2);
    });

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
        missionItem->setData(0, kKindRole, static_cast<int>(TreeRowKind::Mission));
        //child rows for each task indented under the mission
        for (const Task& task : mission.tasks) {
            auto* taskItem = new QTreeWidgetItem(missionItem);
            taskItem->setText(0, QString::fromStdString(task.name));
            taskItem->setData(0, kKindRole, static_cast<int>(TreeRowKind::Task));
        }
        //open the mission so its tasks are visible right away
        missionItem->setExpanded(true);
        //last child row is the gray + Add task action
        auto* addItem = new QTreeWidgetItem(missionItem);
        addItem->setText(0, "+ Add task");
        //hidden data: the row kind and the mission's index in m_missions, read back when the row is clicked
        addItem->setData(0, kKindRole, static_cast<int>(TreeRowKind::AddTask));
        addItem->setData(0, kMissionIndexRole, static_cast<int>(&mission - m_missions.data()));
        addItem->setForeground(0, kActionColor);
    }
}

//runs when any tree row is clicked
void MainWindow::onTreeItemClicked(QTreeWidgetItem* item, int column) {
    //only + Add task rows open the form, mission and task rows return early for now
    auto kind = static_cast<TreeRowKind>(item->data(0, kKindRole).toInt());
    if (kind != TreeRowKind::AddTask) {
        return;
    }
    int missionIndex = item->data(0, kMissionIndexRole).toInt();
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


void MainWindow::onAddMissionSaved(const QString& name) {
    //build the mission and save it
    Mission mission;
    mission.name = name.toStdString();
    try {
        saveMission(m_connStr, m_seasonId, mission);
    } catch (const std::exception& e) {
        QMessageBox::warning(this, "Save error", e.what());
        return;
    }

    //refresh the tree and go back to the table
    loadTaskList();
    m_rightStack->setCurrentIndex(0);
}