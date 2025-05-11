#include <tuple>
#include "Gui/DetailSelectionTab.hpp"

void DetailSelectionTab::buildGui() {

    // layout
    _layout = new QGridLayout(this);
    this->setLayout(_layout);

    // constructional detail label
    _constructionalDetailLabel = new QLabel(this);
    _constructionalDetailLabel->setText("Constructional Detail:");
    _layout->addWidget(_constructionalDetailLabel, 0, 0);

    // constructional detail box
    _constructionalDetailBox = new QComboBox(this);
    _constructionalDetailBox->addItem("<Select Constructional Detail>");
    _constructionalDetailBox->addItems(LefmDetail::availableDetails());
    _layout->addWidget(_constructionalDetailBox, 0, 1);

    // splitter
    _splitter = new QSplitter(this);
    _splitter->setHandleWidth(10);
    _splitter->setOrientation(Qt::Horizontal);
    _layout->addWidget(_splitter, 1, 0, 1, 2);

    // svg widget
    _svgWidget = new QSvgWidget(_splitter);
    _svgWidget->setStyleSheet("border: 1px solid #D8D8D8; background: #FFFFFF;");
    _splitter->addWidget(_svgWidget);

    // parameters group box
    _parametersGroupBox = new QGroupBox(_splitter);
    _parametersGroupBox->setTitle("Constructional Detail Parameters");
    _splitter->addWidget(_parametersGroupBox);

    // parameters group box layout
    _parametersGroupBoxLayout = new QGridLayout(_parametersGroupBox);
    _parametersGroupBox->setLayout(_parametersGroupBoxLayout);

    // parameters table
    _parametersTable = new QTableWidget(_parametersGroupBox);
    _parametersTable->setEditTriggers(QTableWidget::NoEditTriggers);
    _parametersTable->setColumnCount(1);
    _parametersTable->setHorizontalHeaderLabels({ "Parameter Value" });
    _parametersTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _parametersGroupBoxLayout->addWidget(_parametersTable, 0, 0);

    // edit parameters table button
    _editParametersTableButton = new QPushButton(_parametersGroupBox);
    _editParametersTableButton->setIcon(QIcon(":/Graphics/Edit.svg"));
    _editParametersTableButton->setText("Edit Table");
    _parametersGroupBoxLayout->addWidget(_editParametersTableButton, 1, 0);

    // stretches
    _layout->setColumnStretch(0, 0);
    _layout->setColumnStretch(1, 1);

    // connections
    connect(_constructionalDetailBox, &QComboBox::activated, this, &DetailSelectionTab::onDetailSelected);

}

void DetailSelectionTab::refreshGui() {
    if (_detail == nullptr) {
        _constructionalDetailBox->setEnabled(false);
        _parametersTable->setEnabled(false);
        _editParametersTableButton->setEnabled(false);
        _svgWidget->load(QByteArray());
        _parametersTable->clearContents();
        _parametersTable->setRowCount(0);
    } else {
        _constructionalDetailBox->setEnabled(true);
        _parametersTable->setEnabled(true);
        _editParametersTableButton->setEnabled(true);
        if (_detail->selectedDetail().isEmpty()) {
            _svgWidget->load(QByteArray());
            _parametersTable->clearContents();
            _parametersTable->setRowCount(0);
        } else {
            _svgWidget->load(QString(":/Graphics/LefmDetails/%1.svg").arg(_detail->selectedDetail()));
            _svgWidget->renderer()->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);
            const auto& parameters = LefmDetail::parameters(_detail->selectedDetail());
            QStringList verticalHeaderLabels {};
            _parametersTable->setRowCount(parameters.size());
            for (int i = 0; i < parameters.size(); ++i) {
                const auto& [name, description, units] = parameters[i];
                verticalHeaderLabels.push_back(QString("%1, %2 [%3]").arg(description).arg(name).arg(units));
            }
            _parametersTable->setVerticalHeaderLabels(verticalHeaderLabels);
        }
    }
}

DetailSelectionTab::DetailSelectionTab(QWidget* parent) : QWidget(parent) {
    buildGui();
    refreshGui();
}

void DetailSelectionTab::setDetail(LefmDetail* detail) {
    if (detail == _detail) return;
    if (_detail != nullptr) QObject::disconnect(_detail, &LefmDetail::modified, this, &DetailSelectionTab::refreshGui);
    _detail = detail;
    if (_detail != nullptr) QObject::connect(_detail, &LefmDetail::modified, this, &DetailSelectionTab::refreshGui);
    refreshGui();
}

void DetailSelectionTab::onDetailSelected() {
    if (_detail == nullptr) return;
    if (_constructionalDetailBox->currentText() == "<Select Constructional Detail>") _detail->setSelectedDetail("");
    else _detail->setSelectedDetail(_constructionalDetailBox->currentText());
}
