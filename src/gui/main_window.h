#pragma once

#include <QMainWindow>
#include <QPushButton>
#include <QSplitter>
#include <QStackedWidget>
#include <QTableWidget>
#include <QTreeWidget>
#include <string>
#include <vector>
#include "add_mission_page.h"
#include "add_task_page.h"
#include "task.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void loadTaskList();
    void onTreeItemClicked(QTreeWidgetItem* item, int column);
    void onAddTaskSaved(const QString& name, int points);
    void onAddMissionSaved(const QString& name);

    std::string m_connStr;
    int m_seasonId = 1;
    std::vector<Mission> m_missions;
    int m_addTaskMission = -1;

    QSplitter* m_splitter = nullptr;
    QTreeWidget* m_taskTree = nullptr;
    QPushButton* m_addMissionButton = nullptr;

    QStackedWidget* m_rightStack = nullptr;
    QTableWidget* m_scoreTable = nullptr;
    AddTaskPage* m_addTaskPage = nullptr;
    AddMissionPage* m_addMissionPage = nullptr;
};