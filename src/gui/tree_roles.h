#pragma once

#include <Qt>

//custom data roles stored on tree rows, shared by MainWindow and TreeItemDelegate
enum TreeRole {
    kKindRole = Qt::UserRole, //what kind of row this is, stored as an int TreeRowKind
    kMissionIndexRole         //the row's mission index in m_missions
};

//kinds of rows in the mission and task tree
enum class TreeRowKind {
    Mission,
    Task,
    AddTask
};