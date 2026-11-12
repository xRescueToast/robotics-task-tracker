#include "add_task_page.h"

#include <QFormLayout>
#include <QPushButton>

AddTaskPage::AddTaskPage(QWidget* parent) : QWidget(parent) {
    //form layout puts labels on the left and inputs on the right
    auto* form = new QFormLayout(this);

    //layout
    form->setContentsMargins(24, 24, 24, 24);
    form->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    //input widgets
    m_nameEdit = new QLineEdit;
    m_pointsSpin = new QSpinBox;
    m_pointsSpin->setRange(0, 1000);

    //buttons
    auto* saveButton = new QPushButton("Save task");
    auto* cancelButton = new QPushButton("Cancel");

    //add everything to the form
    form->addRow("Task name", m_nameEdit);
    form->addRow("Max points", m_pointsSpin);
    auto* buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(cancelButton);
    buttons->addWidget(saveButton);
    form->addRow(buttons);

    //save: validate the name, then announce it
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        QString name = m_nameEdit->text().trimmed();
        if (name.isEmpty()) {
            m_nameEdit->setFocus();
            return;
        }
        emit saveRequested(name, m_pointsSpin->value());
    });

    //cancel: just announce it
    connect(cancelButton, &QPushButton::clicked, this, &AddTaskPage::cancelled);
}

void AddTaskPage::reset() {
    m_nameEdit->clear();
    m_pointsSpin->setValue(0);
    m_nameEdit->setFocus();
}