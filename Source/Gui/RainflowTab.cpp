#include <vector>
#include <limits>
#include <utility>
#include <algorithm>
#include "Core.hpp"
#include "Gui/RainflowTab.hpp"

void RainflowTab::buildGui() {

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
    _leftWidget->setMinimumSize(440, 0);
    _splitter->addWidget(_leftWidget);
    _splitter->setStretchFactor(0, 0);

    // left layout
    _leftWidgetLayout = new QGridLayout(_leftWidget);
    _leftWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _leftWidget->setLayout(_leftWidgetLayout);

    // right widget
    _rightWidget = new QWidget(_splitter);
    _rightWidget->setMinimumSize(660, 0);
    _rightWidget->installEventFilter(this);
    _splitter->addWidget(_rightWidget);
    _splitter->setStretchFactor(1, 1);

    // right widget layout
    _rightWidgetLayout = new QGridLayout(_rightWidget);
    _rightWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _rightWidget->setLayout(_rightWidgetLayout);

    // rainflow group box
    _rainflowGroupBox = new QGroupBox(_leftWidget);
    _rainflowGroupBox->setTitle("Rainflow Counting");
    _leftWidgetLayout->addWidget(_rainflowGroupBox, 0, 0);

    // rainflow group box layout
    _rainflowGroupBoxLayout = new QGridLayout(_rainflowGroupBox);
    _rainflowGroupBox->setLayout(_rainflowGroupBoxLayout);

    // compute button
    _computeButton = new QPushButton(_rainflowGroupBox);
    _computeButton->setIcon(QIcon(":/Graphics/Calculator.svg"));
    _computeButton->setText("Calculate");
    _rainflowGroupBoxLayout->addWidget(_computeButton, 0, 0);

    // rainflow table
    _rainflowTable = new QTableWidget(_rainflowGroupBox);
    _rainflowTable->setEditTriggers(QTableWidget::NoEditTriggers);
    _rainflowGroupBoxLayout->addWidget(_rainflowTable, 1, 0);

    // rainflow table notes
    _rainflowGroupBoxLayout->addWidget(new QLabel(" \u00B9 Includes repeated occurrences.", _rainflowGroupBox), 2, 0);

    // chart A
    _chartA = new QChart();
    _chartA->legend()->hide();
    _chartA->layout()->setContentsMargins(0, 0, 0, 0);
    _chartA->setContentsMargins(-10, -10, 0, -10);
    _chartA->setBackgroundRoundness(0);
    _chartA->setTitle("Stress Range Histogram");

    // chart B
    _chartB = new QChart();
    _chartB->legend()->hide();
    _chartB->layout()->setContentsMargins(0, 0, 0, 0);
    _chartB->setContentsMargins(-10, -10, 0, -10);
    _chartB->setBackgroundRoundness(0);
    _chartB->setTitle("Mean Stress Histogram");

    // chart C
    _chartC = new Q3DBars();

    // chart view A
    _chartViewA = new QChartView(_chartA, _rightWidget);
    _chartViewA->setRenderHint(QPainter::Antialiasing);
    _chartViewA->setRubberBand(QChartView::RubberBand::VerticalRubberBand);
    _chartViewA->setStyleSheet("border: 1px solid #D8D8D8");
    _rightWidgetLayout->addWidget(_chartViewA, 0, 0);

    // chart view B
    _chartViewB = new QChartView(_chartB, _rightWidget);
    _chartViewB->setRenderHint(QPainter::Antialiasing);
    _chartViewB->setRubberBand(QChartView::RubberBand::VerticalRubberBand);
    _chartViewB->setStyleSheet("border: 1px solid #D8D8D8");
    _rightWidgetLayout->addWidget(_chartViewB, 0, 1);

    // chart view C
    _chartViewC = new QWidget(_rightWidget);
    auto* title = new QLabel("Combined Histogram", _chartViewC);
    title->setFont(_chartA->titleFont());
    title->setAlignment(Qt::AlignCenter);
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    title->setStyleSheet("border: none; background: #FFFFFF;");
    auto* innerWidget = QWidget::createWindowContainer(_chartC, _chartViewC);
    innerWidget->setStyleSheet("border: none; background: #FFFFFF;");
    _chartViewC->setLayout(new QVBoxLayout(_chartViewC));
    _chartViewC->layout()->setContentsMargins(2, 10, 2, 2);
    _chartViewC->layout()->setSpacing(5);
    _chartViewC->layout()->addWidget(title);
    _chartViewC->layout()->addWidget(innerWidget);
    _chartViewC->setStyleSheet("border: 1px solid #D8D8D8; background: #FFFFFF;");
    _rightWidgetLayout->addWidget(_chartViewC, 1, 0, 1, 2);
    _rightWidgetLayout->setRowStretch(0, 1);
    _rightWidgetLayout->setRowStretch(1, 2);

    // connections
    QObject::connect(_computeButton, &QPushButton::clicked, this, &RainflowTab::onComputeButtonClicked);

}

void RainflowTab::refreshGui() {
    if (_detail == nullptr) {
        _computeButton->setEnabled(false);
        _rainflowTable->setEnabled(false);
        _rainflowTable->clearContents();
        _rainflowTable->setRowCount(0);
        _rainflowTable->setColumnCount(0);
    } else {
        _computeButton->setEnabled(true);
        _rainflowTable->setEnabled(true);
        if (_detail->ignoreTime()) {
            _rainflowTable->setColumnCount(3);
            _rainflowTable->setHorizontalHeaderLabels({
                "Count\u00B9\n[Cycles]",
                "Stress Range\n[MPa]",
                "Mean Stress\n[MPa]"
            });
            _rainflowTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
            _rainflowTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
            _rainflowTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
        } else {
            _rainflowTable->setColumnCount(5);
            _rainflowTable->setHorizontalHeaderLabels({
                "Count\u00B9\n[Cycles]",
                "Stress Range\n[MPa]",
                "Mean Stress\n[MPa]",
                QString("Start Time\n[%1]").arg(_detail->timeUnits()),
                QString("End Time\n[%1]").arg(_detail->timeUnits())
            });
            _rainflowTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
            _rainflowTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
            _rainflowTable->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
            _rainflowTable->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
            _rainflowTable->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
        }
        _rainflowTable->setRowCount(_detail->rainflowCounts().rowCount());
        for (int i = 0; i < _rainflowTable->rowCount(); ++i)
            for (int j = 0; j < _rainflowTable->columnCount(); ++j)
                _rainflowTable->setItem(i, j, new QTableWidgetItem(
                    QString::number(_detail->rainflowCounts().at(i, j), 'g', 5)));
    }
    refreshPlot();
}

void RainflowTab::refreshPlot() {
    refreshPlotA();
    refreshPlotB();
    refreshPlotC();
}

void RainflowTab::refreshPlotA() {

    // get data
    int nBinsX = 10;
    auto xAxisRange = std::make_pair(0.0, 1.0);
    auto yAxisRange = std::make_pair(0.0, 1.0);
    auto* set = new QBarSet("Counts"); set->setColor("#006CD9");
    if (_detail != nullptr && _detail->rainflowCounts().rowCount() > 0) {

        // find min and max
        double rangeMin = +std::numeric_limits<double>::infinity();
        double rangeMax = -std::numeric_limits<double>::infinity();
        for (int i = 0; i < _detail->rainflowCounts().rowCount(); ++i) {
            double range = _detail->rainflowCounts().at(i, 1);
            if (range < rangeMin) rangeMin = range;
            if (range > rangeMax) rangeMax = range;
        }

        // put stress ranges into bins
        auto [xMin, xMax] = Utility::getHumanReadableRange({ rangeMin, rangeMax });
        if (xMin < 0.0) xMin = 0.0; // clamp for stress range histogram
        double xDelta = (xMax - xMin)/nBinsX;
        std::vector<double> xBins(nBinsX + 1);
        for (int i = 0; i < xBins.size(); ++i) xBins[i] = xMin + i*xDelta;
        std::vector<double> counts(nBinsX);
        for (int k = 0; k < _detail->rainflowCounts().rowCount(); ++k) {
            double count = _detail->rainflowCounts().at(k, 0);
            double range = _detail->rainflowCounts().at(k, 1);
            for (int i = 0; i < nBinsX; ++i) {
                if (xBins[i] <= range && range < xBins[i + 1]) {
                    counts[i] += count;
                    break;
                }
            }
        }

        // save
        for (double count : counts) set->append(count);
        xAxisRange = std::make_pair(xMin, xMax);
        yAxisRange = Utility::getHumanReadableRange({ 0.0, *std::max_element(counts.begin(), counts.end()) });

    }

    // set new series
    auto* series = new QBarSeries(_chartA);
    series->append(set);
    series->setBarWidth(0.98);
    _chartA->removeAllSeries();
    _chartA->addSeries(series);

    // axes setup
    while (_chartA->axes().count() > 0) {
        auto* axis = _chartA->axes().back();
        _chartA->removeAxis(axis);
        axis->deleteLater();
    }
    auto* xAxis = new QValueAxis(_chartA);
    auto* yAxis = new QValueAxis(_chartA);
    _chartA->addAxis(xAxis, Qt::AlignBottom);
    _chartA->addAxis(yAxis, Qt::AlignLeft);
    xAxis->setRange(xAxisRange.first, xAxisRange.second);
    yAxis->setRange(yAxisRange.first, yAxisRange.second);
    xAxis->setTickCount(nBinsX + 1);
    yAxis->setTickCount(5);
    yAxis->setMinorTickCount(3);
    xAxis->setGridLineVisible(false);
    xAxis->setTitleText("Stress Range [MPa]");
    yAxis->setTitleText("Count [Cycles]");
    xAxis->setLabelFormat("%g");
    yAxis->setLabelFormat("%g");
    xAxis->setLabelsAngle(_rightWidget->width() > 900 ? 0 : 90);
    series->attachAxis(yAxis);

    // font setup
    for (auto* axis : { xAxis, yAxis }) {
        auto font = axis->titleFont();
        font.setBold(false);
        axis->setTitleFont(font);
    }

}

void RainflowTab::refreshPlotB() {

    // get data
    int nBinsX = 10;
    auto xAxisRange = std::make_pair(0.0, 1.0);
    auto yAxisRange = std::make_pair(0.0, 1.0);
    auto* set = new QBarSet("Counts"); set->setColor("#006CD9");
    if (_detail != nullptr && _detail->rainflowCounts().rowCount() > 0) {

        // find min and max
        double meanMin = +std::numeric_limits<double>::infinity();
        double meanMax = -std::numeric_limits<double>::infinity();
        for (int i = 0; i < _detail->rainflowCounts().rowCount(); ++i) {
            double mean = _detail->rainflowCounts().at(i, 2);
            if (mean < meanMin) meanMin = mean;
            if (mean > meanMax) meanMax = mean;
        }

        // put mean stresses into bins
        auto [xMin, xMax] = Utility::getHumanReadableRange({ meanMin, meanMax });
        double xDelta = (xMax - xMin)/nBinsX;
        std::vector<double> xBins(nBinsX + 1);
        for (int i = 0; i < xBins.size(); ++i) xBins[i] = xMin + i*xDelta;
        std::vector<double> counts(nBinsX);
        for (int k = 0; k < _detail->rainflowCounts().rowCount(); ++k) {
            double count = _detail->rainflowCounts().at(k, 0);
            double mean = _detail->rainflowCounts().at(k, 2);
            for (int i = 0; i < nBinsX; ++i) {
                if (xBins[i] <= mean && mean < xBins[i + 1]) {
                    counts[i] += count;
                    break;
                }
            }
        }

        // save
        for (double count : counts) set->append(count);
        xAxisRange = std::make_pair(xMin, xMax);
        yAxisRange = Utility::getHumanReadableRange({ 0.0, *std::max_element(counts.begin(), counts.end()) });

    }

    // set new series
    auto* series = new QBarSeries(_chartB);
    series->append(set);
    series->setBarWidth(0.98);
    _chartB->removeAllSeries();
    _chartB->addSeries(series);

    // axes setup
    while (_chartB->axes().count() > 0) {
        auto* axis = _chartB->axes().back();
        _chartB->removeAxis(axis);
        axis->deleteLater();
    }
    auto* xAxis = new QValueAxis(_chartB);
    auto* yAxis = new QValueAxis(_chartB);
    _chartB->addAxis(xAxis, Qt::AlignBottom);
    _chartB->addAxis(yAxis, Qt::AlignLeft);
    xAxis->setRange(xAxisRange.first, xAxisRange.second);
    yAxis->setRange(yAxisRange.first, yAxisRange.second);
    xAxis->setTickCount(nBinsX + 1);
    yAxis->setTickCount(5);
    yAxis->setMinorTickCount(3);
    xAxis->setGridLineVisible(false);
    xAxis->setTitleText("Mean Stress [MPa]");
    yAxis->setTitleText("Count [Cycles]");
    xAxis->setLabelFormat("%g");
    yAxis->setLabelFormat("%g");
    xAxis->setLabelsAngle(_rightWidget->width() > 900 ? 0 : 90);
    series->attachAxis(yAxis);

    // font setup
    for (auto* axis : { xAxis, yAxis }) {
        auto font = axis->titleFont();
        font.setBold(false);
        axis->setTitleFont(font);
    }

}

void RainflowTab::refreshPlotC() {

    // get data
    int nBinsX = 10;
    int nBinsY = 10;
    auto zAxisRange = std::make_pair(0.0, 1.0);
    auto* series = new QBar3DSeries();
    QStringList xLabels, yLabels;
    if (_detail != nullptr && _detail->rainflowCounts().rowCount() > 0) {

        // create binned data
        double xMin = qobject_cast<QValueAxis*>(_chartA->axes(Qt::Horizontal).front())->min();
        double xMax = qobject_cast<QValueAxis*>(_chartA->axes(Qt::Horizontal).front())->max();
        double yMin = qobject_cast<QValueAxis*>(_chartB->axes(Qt::Horizontal).front())->min();
        double yMax = qobject_cast<QValueAxis*>(_chartB->axes(Qt::Horizontal).front())->max();
        double xDelta = (xMax - xMin)/nBinsX;
        double yDelta = (yMax - yMin)/nBinsY;
        std::vector<double> xBins(nBinsX + 1); for (int i = 0; i < xBins.size(); ++i) xBins[i] = xMin + i*xDelta;
        std::vector<double> yBins(nBinsY + 1); for (int j = 0; j < yBins.size(); ++j) yBins[j] = yMin + j*yDelta;
        Matrix<double> counts(nBinsX, nBinsY);
        for (int k = 0; k < _detail->rainflowCounts().rowCount(); ++k) {
            double count = _detail->rainflowCounts().at(k, 0);
            double range = _detail->rainflowCounts().at(k, 1);
            double mean  = _detail->rainflowCounts().at(k, 2);
            for (int i = 0; i < nBinsX; ++i) {
                if (xBins[i] <= range && range < xBins[i + 1]) {
                    for (int j = 0; j < nBinsY; ++j) {
                        if (yBins[j] <= mean && mean < yBins[j + 1]) {
                            counts.at(i, j) += count;
                            break;
                        }
                    }
                    break;
                }
            }
        }

        // save
        double maxCount = 0.0;
        auto* dataArray = new QBarDataArray();
        for (int i = 0; i < nBinsX; ++i) {
            auto* dataRow = new QBarDataRow();
            for (int j = nBinsY - 1; j >= 0; --j) {
                double count = counts.at(i, j);
                dataRow->append(QBarDataItem(count));
                if (count > maxCount) maxCount = count;
                if (i == 0) yLabels.append(QString("%1 to %2").arg(yBins[j]).arg(yBins[j + 1]));
            }
            dataArray->append(dataRow);
            xLabels.append(QString("%1 to %2").arg(xBins[i]).arg(xBins[i + 1]));
        }
        series->dataProxy()->resetArray(dataArray);
        zAxisRange = Utility::getHumanReadableRange({ 0.0, maxCount });

    }

    // set new series
    for (auto* s : _chartC->seriesList()) { _chartC->removeSeries(s); s->deleteLater(); }
    _chartC->addSeries(series);
    auto* xAxis = new QCategory3DAxis();
    auto* yAxis = new QCategory3DAxis();
    auto* zAxis = new QValue3DAxis();
    xAxis->setTitle("Stress Range [MPa]");
    yAxis->setTitle("Mean Stress [MPa]");
    zAxis->setTitle("Count [Cycles]");
    xAxis->setTitleVisible(true);
    yAxis->setTitleVisible(true);
    zAxis->setTitleVisible(true);
    xAxis->setLabels(xLabels);
    yAxis->setLabels(yLabels);
    zAxis->setRange(zAxisRange.first, zAxisRange.second);
    xAxis->setLabelAutoRotation(45);
    yAxis->setLabelAutoRotation(45);
    zAxis->setLabelAutoRotation(45);
    _chartC->setRowAxis(xAxis);
    _chartC->setColumnAxis(yAxis);
    _chartC->setValueAxis(zAxis);
    _chartC->setBarSpacing({ 0.0, 0.0 });
    _chartC->scene()->activeCamera()->setCameraPosition(-45, 30, 90);
    _chartC->setShadowQuality(Q3DBars::ShadowQualityNone);
    series->setBaseColor("#006CD9");
    series->setSingleHighlightColor("#00D96C");

}

RainflowTab::RainflowTab(QWidget* parent) : QWidget(parent) {
    buildGui();
    refreshGui();
}

bool RainflowTab::eventFilter(QObject* watched, QEvent* event) {
    if (watched == _rightWidget && event->type() == QEvent::Resize)
        for (auto* chart : { _chartA, _chartB })
            for (auto* axis : chart->axes(Qt::Horizontal))
                axis->setLabelsAngle(_rightWidget->width() > 900 ? 0 : 90);
    return QWidget::eventFilter(watched, event);
}

void RainflowTab::setDetail(SnDetail* detail) {
    if (detail == _detail) return;
    if (_detail != nullptr) QObject::disconnect(_detail, &SnDetail::modified, this, &RainflowTab::refreshGui);
    _detail = detail;
    if (_detail != nullptr) QObject::connect(_detail, &SnDetail::modified, this, &RainflowTab::refreshGui);
    refreshGui();
}

void RainflowTab::onComputeButtonClicked() {
    if (_detail == nullptr) return;
    try {
        _detail->executeRainflowCounting();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Missing input data:\n%1").arg(e.what()));
    }
}
