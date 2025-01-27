#include <QtGlobal>
#include <cmath>
#include <vector>
#include <utility>
#include <stdexcept>
#include "Core.hpp"
#include "Gui/EditTableDialog.hpp"
#include "Gui/SnCurveTab.hpp"

void SnCurveTab::buildGui() {

    // layout
    _layout = new QGridLayout(this);
    this->setLayout(_layout);

    // splitter
    _splitter = new QSplitter(this);
    _splitter->setHandleWidth(10);
    _splitter->setOrientation(Qt::Horizontal);
    _layout->addWidget(_splitter, 0, 0);

    // left widget
    _leftWidget = new QWidget(_splitter);
    _leftWidget->resize(320, 0);
    _splitter->addWidget(_leftWidget);
    _splitter->setStretchFactor(0, 0);

    // left layout
    _leftWidgetLayout = new QGridLayout(_leftWidget);
    _leftWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _leftWidget->setLayout(_leftWidgetLayout);

    // right widget
    _rightWidget = new QWidget(_splitter);
    _rightWidget->setMinimumSize(400, 400);
    _splitter->addWidget(_rightWidget);
    _splitter->setStretchFactor(1, 1);

    // right widget layout
    _rightWidgetLayout = new QGridLayout(_rightWidget);
    _rightWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _rightWidget->setLayout(_rightWidgetLayout);

    // category group box
    _categoryGroupBox = new QGroupBox(_leftWidget);
    _categoryGroupBox->setTitle("Constructional Detail");
    _leftWidgetLayout->addWidget(_categoryGroupBox, 0, 0);

    // category group box layout
    _categoryGroupBoxLayout = new QGridLayout(_categoryGroupBox);
    _categoryGroupBox->setLayout(_categoryGroupBoxLayout);

    // category label
    _categoryLabel = new QLabel(_categoryGroupBox);
    _categoryLabel->setText("Detail Category [MPa]:");
    _categoryGroupBoxLayout->addWidget(_categoryLabel, 0, 1);

    // category box
    _categoryBox = new QLineEdit(_categoryGroupBox);
    _categoryBox->setFixedWidth(100);
    _categoryGroupBoxLayout->addWidget(_categoryBox, 0, 2);

    // category info label
    _categoryInfoLabel = new QLabel(_categoryGroupBox);
    _categoryInfoLabel->setPixmap(QIcon(":/Graphics/Info.svg").pixmap(20, 20));
    _categoryInfoLabel->setToolTip("Characteristic reference value of fatigue resistance at 2 million load cycles.");
    _categoryGroupBoxLayout->addWidget(_categoryInfoLabel, 0, 3);

    // spacers
    _categoryGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 0);
    _categoryGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 4);

    // slopes group box
    _slopesGroupBox = new QGroupBox(_leftWidget);
    _slopesGroupBox->setTitle("Curve Slopes");
    _leftWidgetLayout->addWidget(_slopesGroupBox, 1, 0);

    // slopes group box layout
    _slopesGroupBoxLayout = new QGridLayout(_slopesGroupBox);
    _slopesGroupBox->setLayout(_slopesGroupBoxLayout);

    // slopes table
    _slopesTable = new QTableWidget(_slopesGroupBox);
    _slopesTable->setEditTriggers(QTableWidget::NoEditTriggers);
    _slopesTable->setColumnCount(2);
    _slopesTable->setHorizontalHeaderLabels({ "Slope Parameter\n[-]", "Endurance Limit\n[Cycles]" });
    _slopesTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _slopesTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _slopesGroupBoxLayout->addWidget(_slopesTable, 0, 0);

    // edit slopes table button
    _editSlopesTableButton = new QPushButton(_slopesGroupBox);
    _editSlopesTableButton->setIcon(QIcon(":/Graphics/Edit.svg"));
    _editSlopesTableButton->setText("Edit Table");
    _slopesGroupBoxLayout->addWidget(_editSlopesTableButton, 1, 0);

    // chart
    _chart = new QChart();
    _chart->legend()->hide();
    _chart->layout()->setContentsMargins(0, 0, 0, 0);
    _chart->setBackgroundRoundness(0);
    _chart->setTitle("S-N Curve");

    // chart view
    _chartView = new QChartView(_chart, _rightWidget);
    _chartView->setRenderHint(QPainter::Antialiasing);
    _chartView->setStyleSheet("border: 1px solid #D8D8D8");
    _rightWidgetLayout->addWidget(_chartView, 0, 0);

    // connections
    QObject::connect(_categoryBox, &QLineEdit::editingFinished, this, &SnCurveTab::onCategoryBoxEdited);
    QObject::connect(_editSlopesTableButton, &QPushButton::clicked, this, &SnCurveTab::onEditSlopesTableButton);

}

void SnCurveTab::refreshGui() {
    if (_detail == nullptr) {
        _categoryBox->setEnabled(false);
        _slopesTable->setEnabled(false);
        _editSlopesTableButton->setEnabled(false);
        _categoryBox->clear();
        _slopesTable->clearContents();
        _slopesTable->setRowCount(0);
    } else {
        _categoryBox->setEnabled(true);
        _slopesTable->setEnabled(true);
        _editSlopesTableButton->setEnabled(true);
        _categoryBox->setText(_detail->category() ? QString::number(*_detail->category()) : "");
        _slopesTable->setRowCount(_detail->slopes().rowCount());
        for (int i = 0; i < _detail->slopes().rowCount(); ++i) {
            _slopesTable->setItem(i, 0, new QTableWidgetItem(QString::number(_detail->slopes().at(i, 0), 'g', 5)));
            _slopesTable->setItem(i, 1, new QTableWidgetItem(QString::number(_detail->slopes().at(i, 1), 'g', 5)));
        }
    }
    refreshPlot();
}

void SnCurveTab::refreshPlot() {

    // get data
    auto* series = new QLineSeries(_chart);
    auto xRange = std::make_pair(1.0, 10.0);
    auto yRange = std::make_pair(1.0, 10.0);
    if (_detail != nullptr && _detail->category() && _detail->slopes().rowCount() > 0) {

        // compute data points
        double xMin = 1e4;
        double yMin = 1e1;
        double xRef = 2e6;
        double yRef = *_detail->category();
        std::vector<double> xVec, yVec;
        xVec.push_back(xRef);
        yVec.push_back(yRef);
        for (int i = 0; i < _detail->slopes().rowCount(); ++i) {
            double m = _detail->slopes().at(i, 0);
            double x2 = _detail->slopes().at(i, 1);
            double x1 = xVec.back();
            double y1 = yVec.back();
            double y2 = y1*std::pow(x1/x2, 1.0/m);
            xVec.push_back(x2);
            yVec.push_back(y2);
        }
        {
            double m = _detail->slopes().at(0, 0);
            double x2 = xVec.front();
            double y2 = yVec.front();
            double x1 = xMin;
            double y1 = y2/std::pow(x1/x2, 1.0/m);
            xVec.insert(xVec.begin(), x1);
            yVec.insert(yVec.begin(), y1);
        }
        double xMax = std::pow(10, std::ceil(std::log10(xVec.back())));
        double yMax = std::pow(10, std::ceil(std::log10(yVec.front())));
        if (xMax == xVec.back()) xMax *= 10.0;
        xVec.push_back(xMax);
        yVec.push_back(yVec.back());

        // add to series
        for (int i = 0; i < xVec.size(); ++i) series->append(xVec.at(i), yVec.at(i));
        xRange = std::make_pair(xMin, xMax);
        yRange = std::make_pair(yMin, yMax);

    }

    // set series
    _chart->removeAllSeries();
    _chart->addSeries(series);

    // axes setup
    while (_chart->axes().count() > 0) {
        auto* axis = _chart->axes().back();
        _chart->removeAxis(axis);
        axis->deleteLater();
    }
    auto* xAxis = new QLogValueAxis(_chart);
    auto* yAxis = new QLogValueAxis(_chart);
    _chart->addAxis(xAxis, Qt::AlignBottom);
    _chart->addAxis(yAxis, Qt::AlignLeft);
    xAxis->setRange(xRange.first, xRange.second);
    yAxis->setRange(yRange.first, yRange.second);
    xAxis->setMinorTickCount(8);
    yAxis->setMinorTickCount(8);
    xAxis->setLabelFormat("%.0e");
    yAxis->setLabelFormat("%.0e");
    xAxis->setTitleText("Endurance [Cycles]");
    yAxis->setTitleText("Stress Range [MPa]");
    series->setColor("#006CD9");
    series->attachAxis(xAxis);
    series->attachAxis(yAxis);

    // font setup
    for (auto* axis : { xAxis, yAxis }) {
        auto font = axis->titleFont();
        font.setBold(false);
        axis->setTitleFont(font);
    }

}

SnCurveTab::SnCurveTab(QWidget* parent) : QWidget(parent) {
    buildGui();
    refreshGui();
}

void SnCurveTab::setDetail(SnDetail* detail) {
    if (detail == _detail) return;
    if (_detail != nullptr) QObject::disconnect(_detail, &SnDetail::modified, this, &SnCurveTab::refreshGui);
    _detail = detail;
    if (_detail != nullptr) QObject::connect(_detail, &SnDetail::modified, this, &SnCurveTab::refreshGui);
    refreshGui();
}

void SnCurveTab::onCategoryBoxEdited() {
    if (_detail == nullptr) return;
    if (!_detail->category() && _categoryBox->text().isEmpty()) return;
    try {
        bool ok = false;
        double category = _categoryBox->text().toDouble(&ok);
        if (!ok) throw std::invalid_argument("The input must be a valid real number.");
        else _detail->setCategory(category);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Invalid detail category:\n%1").arg(e.what()));
        _categoryBox->setText(_detail->category() ? QString::number(*_detail->category()) : "");
    }
}

void SnCurveTab::onEditSlopesTableButton() {
    if (_detail == nullptr) return;
    EditTableDialog dialog(this, [this](const Matrix<QString>& rawData) {
        try {
            Matrix<double> parsedData(rawData.rowCount(), rawData.columnCount());
            for (int i = 0; i < rawData.rowCount(); ++i) {
                for (int j = 0; j < rawData.columnCount(); ++j) {
                    bool ok = false;
                    double value = rawData.at(i, j).toDouble(&ok);
                    if (!ok) throw std::runtime_error(
                        QString("Could not parse value at cell (%1, %2).").arg(i + 1).arg(j + 1).toStdString());
                    parsedData.at(i, j) = value;
                }
            }
            _detail->setSlopes(parsedData);
            return true;
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Invalid S-N curve:\n%1").arg(e.what()));
            return false;
        }
    });
    dialog.setRowCount(qMax(1, _detail->slopes().rowCount()));
    dialog.setColumnCount(2);
    dialog.setStretchColumns(true);
    dialog.setFixedColumnCount(true);
    dialog.setColumnLabels({ "Slope Parameter\n[-]", "Endurance Limit\n[Cycles]" });
    dialog.setTableData(_detail->slopes());
    dialog.resize(350, 300);
    dialog.exec();
}
