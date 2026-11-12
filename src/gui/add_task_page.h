#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QSpinBox>

class AddTaskPage : public QWidget {
    Q_OBJECT

public:
    explicit AddTaskPage(QWidget* parent = nullptr);
    void reset();

signals:
    void saveRequested(const QString& name, int points);
    void cancelled();

private:
    QLineEdit* m_nameEdit = nullptr;
    QSpinBox* m_pointsSpin = nullptr;
};