#pragma once

#include <Qt>

//custom data roles stored on tree rows, shared by MainWindow and TreeItemDelegate
enum TreeRole {
    kKindRole = Qt::UserRole, //what kind of row this is, stored as an int TreeRowKind
    kMissionIdRole,           //the row's mission database id
    kTaskIdRole               //the row's task database id, task rows only
};

//kinds of rows in the mission and task tree
enum class TreeRowKind { Mission, Task, AddTask };
