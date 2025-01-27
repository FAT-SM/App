#include <QtGlobal>
#include <stdexcept>
#include <utility>
#include <limits>
#include "Core.hpp"
#include "Gui/EditTableDialog.hpp"
#include "Gui/HistoryTab.hpp"

void HistoryTab::buildGui() {

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

    // time group box
    _timeGroupBox = new QGroupBox(_leftWidget);
    _timeGroupBox->setCheckable(true);
    _timeGroupBox->setTitle("Include Time Variable");
    _leftWidgetLayout->addWidget(_timeGroupBox, 0, 0);

    // time group box layout
    _timeGroupBoxLayout = new QGridLayout(_timeGroupBox);
    _timeGroupBox->setLayout(_timeGroupBoxLayout);

    // time units label
    _timeUnitsLabel = new QLabel(_timeGroupBox);
    _timeUnitsLabel->setText("Time Units:");
    _timeUnitsLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _timeGroupBoxLayout->addWidget(_timeUnitsLabel, 0, 1);

    // time units box
    _timeUnitsBox = new QLineEdit(_timeGroupBox);
    _timeUnitsBox->setFixedWidth(100);
    _timeGroupBoxLayout->addWidget(_timeUnitsBox, 0, 2);

    // time units info
    _timeUnitsInfo = new QLabel(_timeGroupBox);
    _timeUnitsInfo->setPixmap(QIcon(":/Graphics/Info.svg").pixmap(20, 20));
    _timeUnitsInfo->setToolTip("For plotting purposes only.");
    _timeGroupBoxLayout->addWidget(_timeUnitsInfo, 0, 3);

    // repetitions group box
    _repsGroupBox = new QGroupBox(_leftWidget);
    _repsGroupBox->setTitle("Sample Repetitions");
    _leftWidgetLayout->addWidget(_repsGroupBox, 1, 0);

    // repetitions group box layout
    _repsGroupBoxLayout = new QGridLayout(_repsGroupBox);
    _repsGroupBox->setLayout(_repsGroupBoxLayout);

    // repetitions label
    _repsLabel = new QLabel(_repsGroupBox);
    _repsLabel->setText("Number of Repetitions:");
    _repsLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _repsGroupBoxLayout->addWidget(_repsLabel, 0, 1);
    _timeUnitsLabel->setMinimumWidth(_repsLabel->sizeHint().width());

    // repetitions box
    _repsBox = new QLineEdit(_repsGroupBox);
    _repsBox->setFixedWidth(100);
    _repsGroupBoxLayout->addWidget(_repsBox, 0, 2);

    // repetitions info
    _repsInfo = new QLabel(_repsGroupBox);
    _repsInfo->setPixmap(QIcon(":/Graphics/Info.svg").pixmap(20, 20));
    _repsInfo->setToolTip("Number of times the sampled stress-time history repeats during the projected lifetime.");
    _repsGroupBoxLayout->addWidget(_repsInfo, 0, 3);

    // spacers
    _timeGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 0);
    _timeGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 4);
    _repsGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 0);
    _repsGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 4);

    // sample group box
    _sampleGroupBox = new QGroupBox(_leftWidget);
    _sampleGroupBox->setTitle("Stress-Time History Sample");
    _leftWidgetLayout->addWidget(_sampleGroupBox, 2, 0);

    // sample group box layout
    _sampleGroupBoxLayout = new QGridLayout(_sampleGroupBox);
    _sampleGroupBox->setLayout(_sampleGroupBoxLayout);

    // sample table
    _sampleTable = new QTableWidget(_sampleGroupBox);
    _sampleTable->setEditTriggers(QTableWidget::NoEditTriggers);
    _sampleGroupBoxLayout->addWidget(_sampleTable, 0, 0);

    // edit sample table button
    _editSampleTableButton = new QPushButton(_sampleGroupBox);
    _editSampleTableButton->setIcon(QIcon(":/Graphics/Edit.svg"));
    _editSampleTableButton->setText("Edit Table");
    _sampleGroupBoxLayout->addWidget(_editSampleTableButton, 1, 0);

    // chart A
    _chartA = new QChart();
    _chartA->legend()->hide();
    _chartA->layout()->setContentsMargins(0, 0, 0, 0);
    _chartA->setContentsMargins(-10, -10, 0, -10);
    _chartA->setBackgroundRoundness(0);
    _chartA->setTitle("Stress-Time History Sample");

    // chart B
    _chartB = new QChart();
    _chartB->legend()->hide();
    _chartB->layout()->setContentsMargins(0, 0, 0, 0);
    _chartB->setContentsMargins(-10, -10, 0, -10);
    _chartB->setBackgroundRoundness(0);
    _chartB->setTitle("Load Reversals");

    // chart view A
    _chartViewA = new QChartView(_chartA, _rightWidget);
    _chartViewA->setRenderHint(QPainter::Antialiasing);
    _chartViewA->setRubberBand(QChartView::RectangleRubberBand);
    _chartViewA->setStyleSheet("border: 1px solid #D8D8D8");
    _rightWidgetLayout->addWidget(_chartViewA, 0, 0);

    // chart view B
    _chartViewB = new QChartView(_chartB, _rightWidget);
    _chartViewB->setRenderHint(QPainter::Antialiasing);
    _chartViewB->setRubberBand(QChartView::RectangleRubberBand);
    _chartViewB->setStyleSheet("border: 1px solid #D8D8D8");
    _rightWidgetLayout->addWidget(_chartViewB, 1, 0);

    // connections
    QObject::connect(_timeGroupBox, &QGroupBox::toggled, this, &HistoryTab::onTimeGroupBoxToggled);
    QObject::connect(_timeUnitsBox, &QLineEdit::editingFinished, this, &HistoryTab::onTimeUnitsBoxEdited);
    QObject::connect(_repsBox, &QLineEdit::editingFinished, this, &HistoryTab::onRepsBoxEdited);
    QObject::connect(_editSampleTableButton, &QPushButton::clicked, this, &HistoryTab::onEditSampleTableButtonClicked);

}

void HistoryTab::refreshGui() {
    if (_detail == nullptr) {
        _timeGroupBox->setEnabled(false);
        _timeUnitsBox->setEnabled(false);
        _repsBox->setEnabled(false);
        _sampleTable->setEnabled(false);
        _editSampleTableButton->setEnabled(false);
        _timeGroupBox->setChecked(false);
        _timeUnitsBox->clear();
        _repsBox->clear();
        _sampleTable->clearContents();
        _sampleTable->setRowCount(0);
        _sampleTable->setColumnCount(0);
    } else {
        _timeGroupBox->setEnabled(true);
        _timeUnitsBox->setEnabled(!_detail->ignoreTime());
        _repsBox->setEnabled(true);
        _sampleTable->setEnabled(true);
        _editSampleTableButton->setEnabled(true);
        _timeGroupBox->setChecked(!_detail->ignoreTime());
        _timeUnitsBox->setText(_detail->ignoreTime() ? "" : _detail->timeUnits());
        _repsBox->setText(QString::number(_detail->repCount()));
        if (_detail->ignoreTime()) {
            _sampleTable->setColumnCount(1);
            _sampleTable->setHorizontalHeaderLabels({ "Stress\n[MPa]" });
            _sampleTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
            _sampleTable->setRowCount(_detail->historySample().rowCount());
            for (int i = 0; i < _detail->historySample().rowCount(); ++i) {
                _sampleTable->setItem(i, 0, new QTableWidgetItem(
                    QString::number(_detail->historySample().at(i, 1), 'g', 5)));
            }
        } else {
            _sampleTable->setColumnCount(2);
            _sampleTable->setHorizontalHeaderLabels(
                { QString("Time\n[%1]").arg(_detail->timeUnits()), "Stress\n[MPa]" });
            _sampleTable->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
            _sampleTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
            _sampleTable->setRowCount(_detail->historySample().rowCount());
            for (int i = 0; i < _detail->historySample().rowCount(); ++i) {
                _sampleTable->setItem(i, 0, new QTableWidgetItem(
                    QString::number(_detail->historySample().at(i, 0), 'g', 5)));
                _sampleTable->setItem(i, 1, new QTableWidgetItem(
                    QString::number(_detail->historySample().at(i, 1), 'g', 5)));
            }
        }
    }
    refreshPlot();
}

void HistoryTab::refreshPlot() {

    // get data
    auto* seriesA = new QLineSeries(_chartA);
    auto* seriesB = new QLineSeries(_chartB);
    auto xRange = std::make_pair(0.0, 1.0);
    auto yRange = std::make_pair(0.0, 1.0);
    if (_detail != nullptr && _detail->historySample().rowCount() > 0 && _detail->historyExtrema().size() > 0) {

        // history
        xRange = std::make_pair(+std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
        yRange = std::make_pair(+std::numeric_limits<double>::infinity(), -std::numeric_limits<double>::infinity());
        for (int i = 0; i < _detail->historySample().rowCount(); ++i) {
            double x = _detail->historySample().at(i, 0);
            double y = _detail->historySample().at(i, 1);
            seriesA->append(x, y);
            if (x < xRange.first) xRange.first = x;
            if (y < yRange.first) yRange.first = y;
            if (x > xRange.second) xRange.second = x;
            if (y > yRange.second) yRange.second = y;
        }
        xRange = Utility::getHumanReadableRange(xRange);
        yRange = Utility::getHumanReadableRange(yRange);

        // extrema
        for (int i = 0; i < _detail->historyExtrema().size(); ++i) {
            double x = _detail->historySample().at(_detail->historyExtrema().at(i), 0);
            double y = _detail->historySample().at(_detail->historyExtrema().at(i), 1);
            seriesB->append(x, y);
        }

    }

    // set series and use OpenGL on large series
    _chartA->removeAllSeries();
    _chartB->removeAllSeries();
    _chartA->addSeries(seriesA);
    _chartB->addSeries(seriesB);
    if (seriesA->count() > 10'000) seriesA->setUseOpenGL(true);
    if (seriesB->count() > 10'000) seriesB->setUseOpenGL(true);

    // axes setup
    _chartA->createDefaultAxes();
    _chartB->createDefaultAxes();
    auto* xAxisA = qobject_cast<QValueAxis*>(_chartA->axes(Qt::Horizontal).first());
    auto* xAxisB = qobject_cast<QValueAxis*>(_chartB->axes(Qt::Horizontal).first());
    auto* yAxisA = qobject_cast<QValueAxis*>(_chartA->axes(Qt::Vertical).first());
    auto* yAxisB = qobject_cast<QValueAxis*>(_chartB->axes(Qt::Vertical).first());
    xAxisA->setRange(xRange.first, xRange.second);
    xAxisB->setRange(xRange.first, xRange.second);
    yAxisA->setRange(yRange.first, yRange.second);
    yAxisB->setRange(yRange.first, yRange.second);
    xAxisA->setMinorTickCount(4);
    xAxisB->setMinorTickCount(4);
    yAxisA->setMinorTickCount(4);
    yAxisB->setMinorTickCount(4);
    yAxisA->setTitleText("Stress [MPa]");
    yAxisB->setTitleText("Stress [MPa]");
    if (_detail == nullptr || _detail->ignoreTime()) {
        xAxisA->setVisible(false);
        xAxisB->setVisible(false);
    } else {
        xAxisA->setTitleText(QString("Time [%1]").arg(_detail->timeUnits()));
        xAxisB->setTitleText(QString("Time [%1]").arg(_detail->timeUnits()));
    }
    seriesA->setColor("#006CD9");
    seriesB->setColor("#006CD9");

    // font setup
    for (auto* axis : { xAxisA, xAxisB, yAxisA, yAxisB }) {
        auto font = axis->titleFont();
        font.setBold(false);
        axis->setTitleFont(font);
    }

}

HistoryTab::HistoryTab(QWidget* parent) : QWidget(parent) {
    buildGui();
    refreshGui();
}

void HistoryTab::setDetail(SnDetail* detail) {
    if (detail == _detail) return;
    if (_detail != nullptr) QObject::disconnect(_detail, &SnDetail::modified, this, &HistoryTab::refreshGui);
    _detail = detail;
    if (_detail != nullptr) QObject::connect(_detail, &SnDetail::modified, this, &HistoryTab::refreshGui);
    refreshGui();
}

void HistoryTab::onTimeGroupBoxToggled() {
    if (_detail == nullptr) return;
    _detail->setIgnoreTime(!_timeGroupBox->isChecked());
}

void HistoryTab::onTimeUnitsBoxEdited() {
    if (_detail == nullptr) return;
    _detail->setTimeUnits(_timeUnitsBox->text());
}

void HistoryTab::onRepsBoxEdited() {
    if (_detail == nullptr) return;
    try {
        bool ok = false;
        double reps = _repsBox->text().toDouble(&ok);
        if (!ok) throw std::invalid_argument("The input must be a valid real number.");
        else _detail->setRepCount(reps);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Invalid number of repetitions:\n%1").arg(e.what()));
        _repsBox->setText(QString::number(_detail->repCount()));
    }
}

void HistoryTab::onEditSampleTableButtonClicked() {
    if (_detail == nullptr) return;
    EditTableDialog dialog(this, [this](const Matrix<QString>& rawData) {
        try {
            Matrix<double> parsedData(rawData.rowCount(), 2);
            if (_detail->ignoreTime()) {
                double delta = 1.0/(rawData.rowCount() - 1);
                for (int i = 0; i < rawData.rowCount(); ++i) {
                    bool ok = false;
                    double value = rawData.at(i, 0).toDouble(&ok);
                    if (!ok) throw std::runtime_error(
                        QString("Could not parse value at cell (%1, 1).").arg(i + 1).toStdString());
                    parsedData.at(i, 0) = delta*i;
                    parsedData.at(i, 1) = value;
                }
            } else {
                for (int i = 0; i < rawData.rowCount(); ++i) {
                    for (int j = 0; j < rawData.columnCount(); ++j) {
                        bool ok = false;
                        double value = rawData.at(i, j).toDouble(&ok);
                        if (!ok) throw std::runtime_error(
                            QString("Could not parse value at cell (%1, %2).").arg(i + 1).arg(j + 1).toStdString());
                        parsedData.at(i, j) = value;
                    }
                }
            }
            _detail->setHistorySample(parsedData);
            return true;
        }
        catch (const std::exception& e) {
            QMessageBox::critical(this, "Error", QString("Invalid stress-time history:\n%1").arg(e.what()));
            return false;
        }
    });
    dialog.setRowCount(qMax(1, _detail->historySample().rowCount()));
    if (_detail->ignoreTime()) {
        dialog.setColumnCount(1);
        dialog.setColumnLabels({ "Stress\n[MPa]" });
        if (_detail->historySample().rowCount() > 0) dialog.setTableData(_detail->historySample().getColumn(1));
    } else {
        dialog.setColumnCount(2);
        dialog.setColumnLabels({ QString("Time\n[%1]").arg(_detail->timeUnits()), "Stress\n[MPa]" });
        if (_detail->historySample().rowCount() > 0) dialog.setTableData(_detail->historySample());
    }
    dialog.setStretchColumns(true);
    dialog.setFixedColumnCount(true);
    dialog.resize(350, 300);
    dialog.exec();
}
