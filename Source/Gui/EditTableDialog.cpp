#include <string>
#include <type_traits>
#include <QtGlobal>
#include "Gui/EditTableDialog.hpp"

void EditTableDialog::buildGui() {

    // this
    setWindowTitle("Edit Table");

    // layout
    _layout = new QGridLayout(this);
    _layout->setContentsMargins(0, 0, 0, 0);
    this->setLayout(_layout);

    // menu bar
    _menuBar = new QMenuBar(this);
    _layout->addWidget(_menuBar, 0, 0);

    // tools menu
    _toolsMenu = new QMenu(_menuBar);
    _toolsMenu->setTitle("Tools");
    _menuBar->addMenu(_toolsMenu);

    // paste action
    _pasteAction = new QAction(_toolsMenu);
    _pasteAction->setIcon(QIcon(":/Graphics/PasteTable.svg"));
    _pasteAction->setText("Paste from Clipboard");
    _toolsMenu->addAction(_pasteAction);

    // clear action
    _clearAction = new QAction(_toolsMenu);
    _clearAction->setIcon(QIcon(":/Graphics/ClearTable.svg"));
    _clearAction->setText("Clear Table");
    _toolsMenu->addAction(_clearAction);

    // central widget
    _centralWidget = new QWidget(this);
    _layout->addWidget(_centralWidget, 1, 0);

    // central widget layout
    _centralWidgetLayout = new QGridLayout(_centralWidget);
    _centralWidget->setLayout(_centralWidgetLayout);

    // group box
    _groupBox = new QGroupBox(_centralWidget);
    _groupBox->setTitle("Tabular Data");
    _centralWidgetLayout->addWidget(_groupBox, 0, 0, 1, 5);

    // group box layout
    _groupBoxLayout = new QGridLayout(_groupBox);
    _groupBox->setLayout(_groupBoxLayout);

    // table
    _table = new QTableWidget(_groupBox);
    _groupBoxLayout->addWidget(_table, 0, 0, 1, 3);

    // row count label
    _rowCountLabel = new QLabel(_groupBox);
    _rowCountLabel->setText("Number of Rows:");
    _groupBoxLayout->addWidget(_rowCountLabel, 1, 1);

    // column count label
    _columnCountLabel = new QLabel(_groupBox);
    _columnCountLabel->setText("Number of Columns:");
    _groupBoxLayout->addWidget(_columnCountLabel, 2, 1);

    // row count box
    _rowCountBox = new QLineEdit(_groupBox);
    _rowCountBox->setFixedWidth(80);
    _groupBoxLayout->addWidget(_rowCountBox, 1, 2);

    // column count box
    _columnCountBox = new QLineEdit(_groupBox);
    _columnCountBox->setFixedWidth(80);
    _groupBoxLayout->addWidget(_columnCountBox, 2, 2);

    // row count spacer
    _rowCountSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    _groupBoxLayout->addItem(_rowCountSpacer, 1, 0);

    // column count spacer
    _columnCountSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed);
    _groupBoxLayout->addItem(_columnCountSpacer, 2, 0);

    // ok button
    _okButton = new QPushButton(_centralWidget);
    _okButton->setIcon(QIcon(":/Graphics/Ok.svg"));
    _okButton->setText("OK");
    _okButton->setFixedWidth(100);
    _okButton->setDefault(false);
    _okButton->setAutoDefault(false);
    _centralWidgetLayout->addWidget(_okButton, 1, 1);

    // cancel button
    _cancelButton = new QPushButton(_centralWidget);
    _cancelButton->setIcon(QIcon(":/Graphics/Cancel.svg"));
    _cancelButton->setText("Cancel");
    _cancelButton->setFixedWidth(100);
    _cancelButton->setDefault(false);
    _cancelButton->setAutoDefault(false);
    _centralWidgetLayout->addWidget(_cancelButton, 1, 3);

    // connections
    QObject::connect(_rowCountBox, &QLineEdit::editingFinished, this, &EditTableDialog::onRowCountBoxEdited);
    QObject::connect(_columnCountBox, &QLineEdit::editingFinished, this, &EditTableDialog::onColumnCountBoxEdited);
    QObject::connect(_okButton, &QPushButton::clicked, this, &EditTableDialog::onOkButtonClicked);
    QObject::connect(_cancelButton, &QPushButton::clicked, this, &EditTableDialog::close);
    QObject::connect(_pasteAction, &QAction::triggered, this, &EditTableDialog::onPasteActionTriggered);
    QObject::connect(_clearAction, &QAction::triggered, this, &EditTableDialog::onClearActionTriggered);

}

EditTableDialog::EditTableDialog(QWidget* parent, std::function<bool(const Matrix<QString>&)> parser) :
    QDialog(parent), _parser(parser) {

    // build the gui
    buildGui();

    // initial state
    setRowCount(1);
    setColumnCount(1);
    setStretchRows(false);
    setStretchColumns(false);
    setFixedRowCount(false);
    setFixedColumnCount(false);

}

void EditTableDialog::setRowCount(int rowCount) {

    // check input
    if (rowCount == _rowCount) return;
    rowCount = rowCount > 0 ? rowCount : 1;
    _rowCount = rowCount;

    // update gui
    _table->setRowCount(_rowCount);
    auto resizeMode = _stretchRows ? QHeaderView::Stretch : QHeaderView::Fixed;
    for (int i = 0; i < _rowCount; ++i) _table->verticalHeader()->setSectionResizeMode(i, resizeMode);
    _rowCountBox->setText(QString::number(_rowCount));

}

void EditTableDialog::setColumnCount(int columnCount) {

    // check input
    if (columnCount == _columnCount) return;
    columnCount = columnCount > 0 ? columnCount : 1;
    _columnCount = columnCount;

    // update gui
    _table->setColumnCount(_columnCount);
    auto resizeMode = _stretchColumns ? QHeaderView::Stretch : QHeaderView::Fixed;
    for (int i = 0; i < _columnCount; ++i) _table->horizontalHeader()->setSectionResizeMode(i, resizeMode);
    _columnCountBox->setText(QString::number(_columnCount));

}

void EditTableDialog::setStretchRows(bool stretchRows) {

    // check input
    if (stretchRows == _stretchRows) return;
    _stretchRows = stretchRows;

    // update gui
    auto resizeMode = _stretchRows ? QHeaderView::Stretch : QHeaderView::Fixed;
    for (int i = 0; i < _rowCount; ++i) _table->verticalHeader()->setSectionResizeMode(i, resizeMode);

}

void EditTableDialog::setStretchColumns(bool stretchColumns) {

    // check input
    if (stretchColumns == _stretchColumns) return;
    _stretchColumns = stretchColumns;

    // update gui
    auto resizeMode = _stretchColumns ? QHeaderView::Stretch : QHeaderView::Fixed;
    for (int i = 0; i < _columnCount; ++i) _table->horizontalHeader()->setSectionResizeMode(i, resizeMode);

}

void EditTableDialog::setFixedRowCount(bool fixedRowCount) {
    _rowCountBox->setEnabled(!fixedRowCount);
}

void EditTableDialog::setFixedColumnCount(bool fixedColumnCount) {
    _columnCountBox->setEnabled(!fixedColumnCount);
}

void EditTableDialog::setRowLabels(const QStringList& labels) {
    _table->setVerticalHeaderLabels(labels);
}

void EditTableDialog::setColumnLabels(const QStringList& labels) {
    _table->setHorizontalHeaderLabels(labels);
}

template<typename T>
void EditTableDialog::setTableData(const Matrix<T>& tableData) {
    for (int i = 0; i < qMin(tableData.rowCount(), _rowCount); ++i) {
        for (int j = 0; j < qMin(tableData.columnCount(), _columnCount); ++j) {
            auto* item = _table->item(i, j);

            if (item == nullptr) {
                item = new QTableWidgetItem();
                _table->setItem(i, j, item);
            }

            if constexpr (std::is_same_v<T, int>)
                item->setText(QString::number(tableData.at(i, j)));
            else if constexpr (std::is_same_v<T, double>)
                item->setText(QString::number(tableData.at(i, j), 'g', 17));
            else if constexpr (std::is_same_v<T, std::string>)
                item->setText(QString::fromStdString(tableData.at(i, j)));
            else if constexpr (std::is_same_v<T, QString>)
                item->setText(tableData.at(i, j));
            else static_assert(false, "Invalid type.");
        }
    }
}

template void EditTableDialog::setTableData(const Matrix<int>& tableData);
template void EditTableDialog::setTableData(const Matrix<double>& tableData);
template void EditTableDialog::setTableData(const Matrix<QString>& tableData);
template void EditTableDialog::setTableData(const Matrix<std::string>& tableData);

void EditTableDialog::onRowCountBoxEdited() {
    bool ok = false;
    int rowCount = _rowCountBox->text().toInt(&ok);
    if (!ok || rowCount < 1) {
        QMessageBox::critical(this, "Error", "Invalid number of rows.");
        _rowCountBox->setText(QString::number(_rowCount));
        return;
    }
    setRowCount(rowCount);
}

void EditTableDialog::onColumnCountBoxEdited() {
    bool ok = false;
    int columnCount = _columnCountBox->text().toInt(&ok);
    if (!ok || columnCount < 1) {
        QMessageBox::critical(this, "Error", "Invalid number of columns.");
        _columnCountBox->setText(QString::number(_columnCount));
        return;
    }
    setColumnCount(columnCount);
}

void EditTableDialog::onOkButtonClicked() {

    // get data
    Matrix<QString> tableData(_rowCount, _columnCount);
    for (int i = 0; i < _rowCount; ++i) {
        for (int j = 0; j < _columnCount; ++j) {
            auto* item = _table->item(i, j);
            tableData.at(i, j) = item != nullptr ? item->text() : "";
        }
    }

    // try to accept
    if (_parser != nullptr && _parser(tableData) == false) return;
    else close();

}

void EditTableDialog::onPasteActionTriggered() {

    // get clipboard text
    QString clipboardText = QApplication::clipboard()->text();
    if (clipboardText.isEmpty()) return;
    if (clipboardText.back() != '\n') clipboardText.append('\n');

    // allocate storage
    int rowCount = clipboardText.count('\n');
    int columnCount = clipboardText.count('\t')/rowCount + 1;
    if (rowCount*columnCount < 1) return;
    Matrix<QString> rawData(rowCount, columnCount);

    // get data
    int i = 0;
    for (const auto& line : clipboardText.split('\n')) {
        if (i < 0 || i >= rowCount) break;
        int j = 0;
        for (const auto& token : line.split('\t')) {
            if (j < 0 || j >= columnCount) break;
            rawData.at(i, j) = token;
            ++j;
        }
        ++i;
    }

    // update gui
    if (_rowCountBox->isEnabled()) setRowCount(rowCount);
    if (_columnCountBox->isEnabled()) setColumnCount(columnCount);
    setTableData(rawData);

}

void EditTableDialog::onClearActionTriggered() {
    for (int i = 0; i < _rowCount; ++i)
        for (int j = 0; j < _columnCount; ++j)
            if (auto* item = _table->item(i, j); item != nullptr)
                item->setText("");
    if (_rowCountBox->isEnabled()) setRowCount(1);
    if (_columnCountBox->isEnabled()) setColumnCount(1);
}
