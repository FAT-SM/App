#include <stdexcept>
#include "Gui/HomeTab.hpp"

void HomeTab::buildGui() {

    // layout
    _layout = new QGridLayout(this);
    _layout->setRowStretch(0, 0);
    _layout->setRowStretch(1, 1);
    this->setLayout(_layout);

    // project group box
    _projectGroupBox = new QGroupBox(this);
    _projectGroupBox->setTitle("Project Info");
    _layout->addWidget(_projectGroupBox, 0, 0);

    // project group box layout
    _projectGroupBoxLayout = new QGridLayout(_projectGroupBox);
    _projectGroupBox->setLayout(_projectGroupBoxLayout);

    // project name label
    _projectNameLabel = new QLabel(_projectGroupBox);
    _projectNameLabel->setText("Project Name:");
    _projectGroupBoxLayout->addWidget(_projectNameLabel, 0, 0);

    // project name box
    _projectNameBox = new QLineEdit(_projectGroupBox);
    _projectGroupBoxLayout->addWidget(_projectNameBox, 1, 0);

    // project description label
    _projectDescriptionLabel = new QLabel(_projectGroupBox);
    _projectDescriptionLabel->setText("Project Description:");
    _projectGroupBoxLayout->addWidget(_projectDescriptionLabel, 2, 0);

    // project description box
    _projectDescriptionBox = new QPlainTextEdit(_projectGroupBox);
    _projectDescriptionBox->setFixedHeight(120);
    _projectGroupBoxLayout->addWidget(_projectDescriptionBox, 3, 0);

    // details group box
    _detailsGroupBox = new QGroupBox(this);
    _detailsGroupBox->setTitle("Constructional Details");
    _layout->addWidget(_detailsGroupBox, 1, 0);

    // details group box layout
    _detailsGroupBoxLayout = new QGridLayout(_detailsGroupBox);
    _detailsGroupBox->setLayout(_detailsGroupBoxLayout);

    // details table
    _detailsTable = new QTableWidget(_detailsGroupBox);
    _detailsTable->setColumnCount(3);
    _detailsTable->setHorizontalHeaderLabels({ "Detail Name", "Selected Approach", "Detail Description" });
    _detailsTable->setColumnWidth(0, 150);
    _detailsTable->setColumnWidth(1, 150);
    _detailsTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    _detailsTable->setSelectionMode(QTableWidget::SingleSelection);
    _detailsTable->setSelectionBehavior(QTableWidget::SelectRows);
    _detailsGroupBoxLayout->addWidget(_detailsTable, 0, 0, 1, 5);

    // new s-n detail button
    _newSnDetailButton = new QPushButton(_detailsGroupBox);
    _newSnDetailButton->setIcon(QIcon(":/Graphics/Add.svg"));
    _newSnDetailButton->setText("New (S-N)");
    _newSnDetailButton->setFixedWidth(100);
    _detailsGroupBoxLayout->addWidget(_newSnDetailButton, 1, 1);

    // new lefm detail button
    _newLefmDetailButton = new QPushButton(_detailsGroupBox);
    _newLefmDetailButton->setIcon(QIcon(":/Graphics/Add.svg"));
    _newLefmDetailButton->setText("New (LEFM)");
    _newLefmDetailButton->setFixedWidth(100);
    _detailsGroupBoxLayout->addWidget(_newLefmDetailButton, 1, 2);

    // edit detail button
    _editDetailButton = new QPushButton(_detailsGroupBox);
    _editDetailButton->setIcon(QIcon(":/Graphics/Edit.svg"));
    _editDetailButton->setText("Edit");
    _editDetailButton->setFixedWidth(100);
    _detailsGroupBoxLayout->addWidget(_editDetailButton, 1, 3);

    // delete detail button
    _deleteDetailButton = new QPushButton(_detailsGroupBox);
    _deleteDetailButton->setIcon(QIcon(":/Graphics/Delete.svg"));
    _deleteDetailButton->setText("Delete");
    _deleteDetailButton->setFixedWidth(100);
    _detailsGroupBoxLayout->addWidget(_deleteDetailButton, 1, 4);

    // connections
    _projectDescriptionBox->installEventFilter(this);
    QObject::connect(_detailsTable, &QTableWidget::currentCellChanged, this, &HomeTab::onCurrentDetailChanged);
    QObject::connect(_projectNameBox, &QLineEdit::editingFinished, this, &HomeTab::onProjectNameBoxEdited);
    QObject::connect(_detailsTable, &QTableWidget::cellChanged, this, &HomeTab::onDetailsTableEdited);
    QObject::connect(_editDetailButton, &QPushButton::clicked, this, &HomeTab::onEditDetailButtonClicked);
    QObject::connect(_deleteDetailButton, &QPushButton::clicked, this, &HomeTab::onDeleteDetailButtonClicked);
    QObject::connect(_newSnDetailButton, &QPushButton::clicked, this, &HomeTab::onNewSnDetailButtonClicked);

}

void HomeTab::refreshGui() {

    // general
    if (_project == nullptr) {
        _projectNameBox->setEnabled(false);
        _projectDescriptionBox->setEnabled(false);
        _newSnDetailButton->setEnabled(false);
        _newLefmDetailButton->setEnabled(false);
        _editDetailButton->setEnabled(false);
        _deleteDetailButton->setEnabled(false);
        _projectNameBox->clear();
        _projectDescriptionBox->clear();
    } else {
        _projectNameBox->setEnabled(true);
        _projectDescriptionBox->setEnabled(true);
        _newSnDetailButton->setEnabled(true);
        _newLefmDetailButton->setEnabled(true);
        _editDetailButton->setEnabled(_detailsTable->currentRow() >= 0);
        _deleteDetailButton->setEnabled(_detailsTable->currentRow() >= 0);
        _projectNameBox->setText(_project->name());
        _projectDescriptionBox->setPlainText(_project->description());
    }

    // details table
    if (_project == nullptr) {
        _detailsTable->setEnabled(false);
        _detailsTable->clearContents();
        _detailsTable->setRowCount(0);
    } else {
        _detailsTable->setEnabled(true);
        _detailsTable->clearContents();
        _detailsTable->setRowCount(_project->detailCount());
        for (int i = 0; i < _project->detailCount(); ++i) {
            auto& detail = _project->detail(i);
            _detailsTable->setItem(i, 0, new QTableWidgetItem(detail.name()));
            _detailsTable->setItem(i, 2, new QTableWidgetItem(detail.description()));
            auto* approachItem = new QTableWidgetItem();
            approachItem->setFlags(approachItem->flags() & ~Qt::ItemIsEditable);
            switch (detail.approach()) {
                case Detail::Approach::Sn: approachItem->setText("S-N"); break;
                default: throw std::logic_error("Case not implemented.");
            }
            _detailsTable->setItem(i, 1, approachItem);
        }
    }

}

HomeTab::HomeTab(QWidget* parent) : QWidget(parent) {
    buildGui();
    refreshGui();
}

bool HomeTab::eventFilter(QObject* watched, QEvent* event) {
    if (watched == _projectDescriptionBox && event->type() == QEvent::FocusOut) onProjectDescriptionBoxEdited();
    return QWidget::eventFilter(watched, event);
}

void HomeTab::setProject(Project* project) {
    if (project == _project) return;
    if (_project != nullptr) QObject::disconnect(_project, &Project::modified, this, &HomeTab::refreshGui);
    _project = project;
    if (_project != nullptr) QObject::connect(_project, &Project::modified, this, &HomeTab::refreshGui);
    refreshGui();
}

void HomeTab::onCurrentDetailChanged() {
    _editDetailButton->setEnabled(_detailsTable->currentRow() >= 0);
    _deleteDetailButton->setEnabled(_detailsTable->currentRow() >= 0);
}

void HomeTab::onProjectNameBoxEdited() {
    if (_project == nullptr) return;
    try { _project->setName(_projectNameBox->text()); }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Invalid project name:\n%1").arg(e.what()));
        _projectNameBox->setText(_project->name());
    }
}

void HomeTab::onProjectDescriptionBoxEdited() {
    if (_project == nullptr) return;
    try { _project->setDescription(_projectDescriptionBox->toPlainText()); }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Invalid project description:\n%1").arg(e.what()));
        _projectDescriptionBox->setPlainText(_project->description());
    }
}

void HomeTab::onDetailsTableEdited(int row, int column) {
    if (_project == nullptr) return;
    switch (column) {
        case 0: // name
            try { _project->detail(row).setName(_detailsTable->item(row, column)->text()); }
            catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", QString("Invalid detail name:\n%1").arg(e.what()));
                _detailsTable->item(row, column)->setText(_project->detail(row).name());
            }
            break;
        case 2: // description
            try { _project->detail(row).setDescription(_detailsTable->item(row, column)->text()); }
            catch (const std::exception& e) {
                QMessageBox::critical(this, "Error", QString("Invalid detail description:\n%1").arg(e.what()));
                _detailsTable->item(row, column)->setText(_project->detail(row).description());
            }
            break;
    }
    _detailsTable->setCurrentCell(row, column);
}

void HomeTab::onEditDetailButtonClicked() {
    if (_project == nullptr) return;
    if (int row = _detailsTable->currentRow(); row >= 0)
        emit editDetailRequestReceived(row);
}

void HomeTab::onDeleteDetailButtonClicked() {
    if (_project == nullptr) return;
    if (int row = _detailsTable->currentRow(); row >= 0)
        _project->destroyDetail(row);
}

void HomeTab::onNewSnDetailButtonClicked() {
    if (_project == nullptr) return;
    _project->createDetail(Detail::Approach::Sn);
    _detailsTable->setCurrentCell(_project->detailCount() - 1, 0);
    _detailsTable->scrollToBottom();
}
