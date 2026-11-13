#include "add_mission_page.h"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QPushButton>

AddMissionPage::AddMissionPage(QWidget* parent) : QWidget(parent) {
    //form layout with some breathing room
    auto* form = new QFormLayout(this);
    form->setContentsMargins(24, 24, 24, 24);
    form->setFieldGrowthPolicy(QFormLayout::FieldsStayAtSizeHint);

    //input widget
    m_nameEdit = new QLineEdit;
    m_nameEdit->setPlaceholderText("e.g. Mission 02: Crater Crossing");

    //buttons side by side on the right
    auto* saveButton = new QPushButton("Save mission");
    auto* cancelButton = new QPushButton("Cancel");
    auto* buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(cancelButton);
    buttons->addWidget(saveButton);

    //add everything to the form
    form->addRow("Mission name", m_nameEdit);
    form->addRow(buttons);

    //save: validate the name, then announce it
    connect(saveButton, &QPushButton::clicked, this, [this]() {
        QString name = m_nameEdit->text().trimmed();
        if (name.isEmpty()) {
            m_nameEdit->setFocus();
            return;
        }
        emit saveRequested(name);
    });

    //cancel: just announce it
    connect(cancelButton, &QPushButton::clicked, this, &AddMissionPage::cancelled);
}

void AddMissionPage::reset() {
    m_nameEdit->clear();
    m_nameEdit->setFocus();
}