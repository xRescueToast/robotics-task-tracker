#pragma once

#include <QWidget>
#include <QLineEdit>

class AddMissionPage : public QWidget {
    Q_OBJECT

public:
    explicit AddMissionPage(QWidget* parent = nullptr);
    void reset();

signals:
    void saveRequested(const QString& name);
    void cancelled();

private:
    QLineEdit* m_nameEdit = nullptr;
};