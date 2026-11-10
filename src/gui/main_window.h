#pragma once
#include <QListWidget>
#include <QMainWindow>
#include <QSplitter>
#include <QTableWidget>
#include <string>
#include <vector>
#include "task.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    void loadTaskList();

    QListWidget* m_taskList = nullptr;
    QSplitter* m_splitter = nullptr;
    QTableWidget* m_scoreTable = nullptr;
    std::string m_connStr;
    int m_seasonId = 1;
    std::vector<Mission> m_missions;
};