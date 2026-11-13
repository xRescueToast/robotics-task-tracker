#pragma once
#include <QTreeWidget>
#include <QMainWindow>
#include <QSplitter>
#include <QTableWidget>
#include <QStackedWidget>
#include <QPushButton>
#include <string>
#include <vector>
#include "add_mission_page.h"
#include "task.h"
#include "add_task_page.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void loadTaskList();
    void onTreeItemClicked(QTreeWidgetItem* item, int column);
    void onAddTaskSaved(const QString& name, int points);
    void onAddMissionSaved(const QString& name);

    QTreeWidget* m_taskTree = nullptr;
    QSplitter* m_splitter = nullptr;
    QTableWidget* m_scoreTable = nullptr;
    std::string m_connStr;
    int m_seasonId = 1;
    std::vector<Mission> m_missions;
    QStackedWidget* m_rightStack = nullptr;
    AddTaskPage* m_addTaskPage = nullptr;
    int m_addTaskMission = -1;
    AddMissionPage* m_addMissionPage = nullptr;
    QPushButton* m_addMissionButton = nullptr;
};
