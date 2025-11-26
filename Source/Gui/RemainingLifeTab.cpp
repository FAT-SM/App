#include <algorithm>
#include "Gui/EditTableDialog.hpp"
#include "Gui/RemainingLifeTab.hpp"

void RemainingLifeTab::buildGui() {

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

    // compute group box
    _computeGroupBox = new QGroupBox(_leftWidget);
    _computeGroupBox->setTitle("Remaining Fatigue Life");
    _leftWidgetLayout->addWidget(_computeGroupBox, 0, 0);

    // compute group box layout
    _computeGroupBoxLayout = new QGridLayout(_computeGroupBox);
    _computeGroupBox->setLayout(_computeGroupBoxLayout);

    // compute button
    _computeButton = new QPushButton(_computeGroupBox);
    _computeButton->setIcon(QIcon(":/Graphics/Calculator.svg"));
    _computeButton->setText("Calculate");
    _computeGroupBoxLayout->addWidget(_computeButton, 0, 0);

    // table
    _table = new QTableWidget(_computeGroupBox);
    _table->setEditTriggers(QTableWidget::NoEditTriggers);
    _table->setColumnCount(3);
    _table->setHorizontalHeaderLabels({
        "Initial Surface Crack Depth, a₀\n[mm]",
        "Design Fatigue Life, DFL\n[Years]",
        "Stop Criterion" });
    _table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    _table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    _table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    _computeGroupBoxLayout->addWidget(_table, 1, 0);

    // edit table button
    _editTableButton = new QPushButton(_computeGroupBox);
    _editTableButton->setIcon(QIcon(":/Graphics/Edit.svg"));
    _editTableButton->setText("Edit Table");
    _computeGroupBoxLayout->addWidget(_editTableButton, 2, 0);

    // chart
    _chart = new QChart();
    _chart->legend()->hide();
    _chart->layout()->setContentsMargins(0, 0, 0, 0);
    _chart->setBackgroundRoundness(0);
    _chart->setTitle("Remaining Fatigue Life Design Curve");

    // chart view
    _chartView = new QChartView(_chart, _rightWidget);
    _chartView->setRenderHint(QPainter::Antialiasing);
    _chartView->setStyleSheet("border: 1px solid #D8D8D8");
    _rightWidgetLayout->addWidget(_chartView, 0, 0);

    // connections
    QObject::connect(_computeButton, &QPushButton::clicked, this, &RemainingLifeTab::onComputeButtonClicked);
    QObject::connect(_editTableButton, &QPushButton::clicked, this, &RemainingLifeTab::onEditTableButtonClicked);

}

void RemainingLifeTab::refreshGui() {
    if (_detail == nullptr) {
        _computeButton->setEnabled(false);
        _editTableButton->setEnabled(false);
        _table->setEnabled(false);
        _table->clearContents();
        _table->setRowCount(0);
    }
    else {
        _computeButton->setEnabled(true);
        _editTableButton->setEnabled(true);
        _table->setEnabled(true);
        _table->setRowCount(_detail->initialCrackSample().size());
        for (int i = 0; i < _detail->initialCrackSample().size(); ++i) {
            _table->setItem(i, 0, new QTableWidgetItem(QString::number(_detail->initialCrackSample().at(i), 'g', 5)));
            if (!_detail->lifeSample().has_value()) {
                _table->setItem(i, 1, new QTableWidgetItem("..."));
                _table->setItem(i, 2, new QTableWidgetItem("..."));
            }
            else {
                _table->setItem(i, 1, new QTableWidgetItem(QString::number((*_detail->lifeSample()).at(i), 'g', 5)));
                _table->setItem(i, 2, new QTableWidgetItem((*_detail->stopCriteria()).at(i)));
            }
        }
    }
    refreshPlot();
}

void RemainingLifeTab::refreshPlot() {

    // get data
    auto* series = new QLineSeries(_chart);
    auto xRange = std::make_pair(0.0, 1.0);
    auto yRange = std::make_pair(0.0, 1.0);
    if (_detail != nullptr && _detail->lifeSample()) {

        // compute data points
        const auto& sample = _detail->initialCrackSample();
        std::vector<double> x;
        if (sample.size() < 2) {
            x.push_back(sample.at(0)/10.0);
            x.push_back(sample.at(0));
            x.push_back(sample.at(0)*10.0);
        } else {
            int n = 10;
            for (int i = 0; i < sample.size() - 1; ++i) {
                double a = sample.at(i);
                double b = sample.at(i + 1);
                double step = (b - a)/(n + 1);
                for (int k = 0; k <= n; ++k) x.push_back(a + k*step);
            }
            x.push_back(sample.max());
        }
        std::vector<double> y;
        y.resize(x.size());
        for (int i = 0; i < x.size(); ++i)
            y[i] = _detail->computeRemainingFatigueLife(x[i]).first;

        // add to series
        for (int i = 0; i < x.size(); ++i) series->append(y[i], x[i]);
        double xMax = *std::max_element(x.begin(), x.end());
        double xMin = *std::min_element(x.begin(), x.end());
        double yMax = *std::max_element(y.begin(), y.end());
        double yMin = *std::min_element(y.begin(), y.end());
        xRange = Utility::getHumanReadableRange({ xMin, xMax });
        yRange = Utility::getHumanReadableRange({ yMin, yMax });

    }

    // set series
    _chart->removeAllSeries();
    _chart->addSeries(series);

    // axes setup
    _chart->createDefaultAxes();
    auto* xAxis = qobject_cast<QValueAxis*>(_chart->axes(Qt::Horizontal).first());
    auto* yAxis = qobject_cast<QValueAxis*>(_chart->axes(Qt::Vertical).first());
    xAxis->setRange(yRange.first, yRange.second);
    yAxis->setRange(xRange.first, xRange.second);
    xAxis->setTitleText("Design Fatigue Life, DFL [Years]");
    yAxis->setTitleText("Initial Surface Crack Depth, a₀ [mm]");
    series->setColor("#006CD9");

    // font setup
    for (auto* axis : { xAxis, yAxis }) {
        auto font = axis->titleFont();
        font.setBold(false);
        axis->setTitleFont(font);
    }

}

RemainingLifeTab::RemainingLifeTab(QWidget* parent) : QWidget(parent) {
    buildGui();
    refreshGui();
}

void RemainingLifeTab::setDetail(LefmDetail* detail) {
    if (detail == _detail) return;
    if (_detail != nullptr) QObject::disconnect(_detail, &LefmDetail::modified, this, &RemainingLifeTab::refreshGui);
    _detail = detail;
    if (_detail != nullptr) QObject::connect(_detail, &LefmDetail::modified, this, &RemainingLifeTab::refreshGui);
    refreshGui();
}

void RemainingLifeTab::onComputeButtonClicked() {
    if (_detail == nullptr) return;
    try {
        _detail->computeRemainingFatigueLife();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", e.what());
    }
}

void RemainingLifeTab::onEditTableButtonClicked() {
    if (_detail == nullptr) return;
    EditTableDialog dialog(this, [this](const Matrix<QString>& rawData) {
        try {
            Matrix<double> parsedData(rawData.rowCount(), 1);
            for (int i = 0; i < rawData.rowCount(); ++i) {
                bool ok = false;
                double value = rawData.at(i).toDouble(&ok);
                if (!ok) throw std::runtime_error(
                    QString("Could not parse value at row %1.").arg(i + 1).toStdString());
                parsedData.at(i) = value;
            }
            _detail->setInitialCrackSample(parsedData);
            return true;
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Invalid initial surface crack depth(s):\n%1").arg(e.what()));
            return false;
        }
    });
    dialog.setRowCount(_detail->initialCrackSample().size());
    dialog.setColumnCount(1);
    dialog.setStretchColumns(true);
    dialog.setFixedColumnCount(true);
    dialog.setColumnLabels({ "Initial Surface Crack Depth, a₀\n[mm]" });
    dialog.setTableData(_detail->initialCrackSample());
    dialog.resize(350, 300);
    dialog.exec();
}
