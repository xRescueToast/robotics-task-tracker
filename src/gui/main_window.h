#pragma once
#include <QTreeWidget>
#include <QMainWindow>
#include <QSplitter>
#include <QTableWidget>
#include <QStackedWidget>
#include <string>
#include <vector>
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

    QTreeWidget* m_taskTree = nullptr;
    QSplitter* m_splitter = nullptr;
    QTableWidget* m_scoreTable = nullptr;
    std::string m_connStr;
    int m_seasonId = 1;
    std::vector<Mission> m_missions;
    QStackedWidget* m_rightStack = nullptr;
    AddTaskPage* m_addTaskPage = nullptr;
    int m_addTaskMission = -1;
};
