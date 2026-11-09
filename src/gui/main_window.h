#pragma once
#include <QListWidget>
#include <QMainWindow>
#include <string>
#include <vector>
#include "task.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void loadTaskList();

    QListWidget* m_taskList;
    std::string m_connStr;
    int m_seasonId = 1;
    std::vector<Task> m_tasks;
};