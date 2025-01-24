#include <QtGlobal>
#include <stdexcept>
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
    _editSampleTableButton->setText("Edit Table");
    _sampleGroupBoxLayout->addWidget(_editSampleTableButton, 1, 0);

    // plot area
    _plotArea = new QLabel(_rightWidget);
    _plotArea->setMinimumSize(400, 400);
    _plotArea->setStyleSheet("border: 1px solid #D8D8D8; background: #FFFFFF;");
    _plotArea->installEventFilter(this);
    _rightWidgetLayout->addWidget(_plotArea, 0, 0);

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
                    QString::asprintf("%.5g", _detail->historySample().at(i, 1))));
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
                    QString::asprintf("%.5g", _detail->historySample().at(i, 0))));
                _sampleTable->setItem(i, 1, new QTableWidgetItem(
                    QString::asprintf("%.5g", _detail->historySample().at(i, 1))));
            }
        }
    }
    refreshPlot();
}

void HistoryTab::refreshPlot() {

    // figure setup
    _plotter->figure(_plotArea->size());
    _plotter->append("set multiplot layout 2, 1");
    if (_detail == nullptr || _detail->ignoreTime()) {
        _plotter->append(R"(
# axes setup
set style line 10 lt 1 lw 1 lc rgb 'gray'
set grid ytics ls 10
unset xtics
set mytics

# labels
set ytics font ', 8'
set ylabel 'Stress [MPa]' font ', 9'
unset key
        )");
    } else {
        _plotter->append(QString(R"(
# axes setup
set style line 10 lt 1 lw 1 lc rgb 'gray'
set grid xtics ytics ls 10
set mxtics
set mytics

# labels
set xtics font ', 8'
set ytics font ', 8'
set xlabel 'Time [%1]' font ', 9'
set ylabel 'Stress [MPa]' font ', 9'
unset key
        )").arg(_detail->timeUnits()));
    }

    // write plot data
    if (_detail == nullptr || _detail->historySample().rowCount() == 0 || _detail->historyExtrema().size() == 0) {
        _plotter->append("$history << end\n0.0 0.0\nend");
        _plotter->append("$extrema << end\n0.0 0.0\nend");
    } else {

        // history
        _plotter->append("$history << end");
        for (int i = 0; i < _detail->historySample().rowCount(); ++i) _plotter->append(
            QString("%1 %2").arg(_detail->historySample().at(i, 0)).arg(_detail->historySample().at(i, 1)));
        _plotter->append("end");

        // extrema
        _plotter->append("$extrema << end");
        for (int i = 0; i < _detail->historyExtrema().size(); ++i) {
            double x = _detail->historySample().at(_detail->historyExtrema().at(i), 0);
            double y = _detail->historySample().at(_detail->historyExtrema().at(i), 1);
            _plotter->append(QString("%1 %2").arg(x).arg(y));
        }
        _plotter->append("end");

    }

    // plot
    _plotter->append(R"(
# line style
set style line 100 lt 1 lw 2 lc rgb '#006CD9'

# plot A
set title 'Stress-Time History Sample' font ', 10'
plot $history with lines ls 100

# plot B
set title 'Load Reversals' font ', 10'
plot $extrema with lines ls 100
    )");
    _plotArea->setPixmap(_plotter->plot());
    _outdatedPlot = false;

}

HistoryTab::HistoryTab(QWidget* parent) : QWidget(parent) {

    // create plotter
    _plotter = new Gnuplot(this);

    // create auto refresh timer
    _plotTimer = new QTimer(this);
    _plotTimer->setInterval(100);
    QObject::connect(_plotTimer, &QTimer::timeout, this, [this]() { if (_outdatedPlot) refreshPlot(); });
    _plotTimer->start();

    // initialize gui
    buildGui();
    refreshGui();

}

bool HistoryTab::eventFilter(QObject* watched, QEvent* event) {
    if (watched == _plotArea && event->type() == QEvent::Resize) _outdatedPlot = true;
    return QWidget::eventFilter(watched, event);
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
    } else {
        dialog.setColumnCount(2);
        dialog.setColumnLabels({ QString("Time\n[%1]").arg(_detail->timeUnits()), "Stress\n[MPa]" });
    }
    dialog.setStretchColumns(true);
    dialog.setFixedColumnCount(true);
    dialog.setTableData(_detail->historySample());
    dialog.resize(350, 300);
    dialog.exec();
}
