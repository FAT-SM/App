#include <tuple>
#include "Gui/EditTableDialog.hpp"
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
    connect(_editParametersTableButton, &QPushButton::clicked, this, &DetailSelectionTab::onEditTableButtonClicked);

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
        if (_detail->selectedDetail().isEmpty()) {
            _constructionalDetailBox->setCurrentIndex(0);
            _svgWidget->load(QByteArray());
            _parametersTable->clearContents();
            _parametersTable->setRowCount(0);
            _editParametersTableButton->setEnabled(false);
        } else {
            _constructionalDetailBox->setCurrentText(_detail->selectedDetail());
            _svgWidget->load(QString(":/Graphics/LefmDetails/%1.svg").arg(_detail->selectedDetail()));
            _svgWidget->renderer()->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);
            const auto& parameters = LefmDetail::parameters(_detail->selectedDetail());
            QStringList verticalHeaderLabels {};
            _parametersTable->setRowCount(parameters.size());
            for (int i = 0; i < parameters.size(); ++i) {
                const auto& [symbol, description, units] = parameters[i];
                verticalHeaderLabels.push_back(QString("%1, %2 [%3]").arg(description).arg(symbol).arg(units));
                if (_detail->hasParameters())
                    _parametersTable->setItem(i, 0, new QTableWidgetItem(
                        QString::number(_detail->parameterValue(symbol), 'g', 5)));
            }
            _parametersTable->setVerticalHeaderLabels(verticalHeaderLabels);
            _editParametersTableButton->setEnabled(true);
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

void DetailSelectionTab::onEditTableButtonClicked() {
    if (_detail == nullptr) return;
    const auto& pSpec = LefmDetail::parameters(_detail->selectedDetail());
    EditTableDialog dialog(this, [this, &pSpec](const Matrix<QString>& rawData) {
        try {
            std::unordered_map<QString, double> paramValues;
            for (int i = 0; i < rawData.rowCount(); ++i) {
                const auto& symbol = std::get<0>(pSpec[i]);
                bool ok = false;
                double value = rawData.at(i, 0).toDouble(&ok);
                if (!ok) throw std::runtime_error(QString("Could not parse value at row %1.").arg(i + 1).toStdString());
                paramValues[symbol] = value;
            }
            _detail->setParameterValues(paramValues);
            return true;
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Error",
                QString("Invalid constructional detail parameters:\n%1").arg(e.what()));
            return false;
        }
        return false;
    });
    dialog.setRowCount(pSpec.size());
    dialog.setColumnCount(1);
    dialog.setFixedRowCount(true);
    dialog.setFixedColumnCount(true);
    QStringList rowLabels{};
    for (const auto& [symbol, description, units] : pSpec)
        rowLabels.push_back(QString("%1, %2 [%3]").arg(description).arg(symbol).arg(units));
    dialog.setRowLabels(rowLabels);
    dialog.setColumnLabels({ "Parameter Value" });
    dialog.setStretchColumns(true);
    if (_detail->hasParameters()) {
        Matrix<double> values(pSpec.size(), 1);
        for (int i = 0; i < pSpec.size(); ++i) values.at(i) = _detail->parameterValue(std::get<0>(pSpec[i]));
        dialog.setTableData(values);
    }
    dialog.resize(400, 500);
    dialog.exec();
}
