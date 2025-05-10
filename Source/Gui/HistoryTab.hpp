#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtCharts>
#include "Model.hpp"

/* The stress-time history tab. */
class HistoryTab : public QWidget {
    Q_OBJECT

private:
    QGridLayout* _layout;
    QSplitter* _splitter;
    QWidget* _leftWidget;
    QGridLayout* _leftWidgetLayout;
    QWidget* _rightWidget;
    QGridLayout* _rightWidgetLayout;
    QGroupBox* _timeGroupBox;
    QGridLayout* _timeGroupBoxLayout;
    QLabel* _timeUnitsLabel;
    QLineEdit* _timeUnitsBox;
    QLabel* _timeUnitsInfo;
    QGroupBox* _repsGroupBox;
    QGridLayout* _repsGroupBoxLayout;
    QLabel* _repsLabel;
    QLineEdit* _repsBox;
    QLabel* _repsInfo;
    QGroupBox* _sampleGroupBox;
    QGridLayout* _sampleGroupBoxLayout;
    QTableWidget* _sampleTable;
    QPushButton* _editSampleTableButton;
    QChart* _chartA;
    QChart* _chartB;
    QChartView* _chartViewA;
    QChartView* _chartViewB;
    Detail* _detail;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

    /* Refreshes the plot. */
    void refreshPlot();

public:

    /* Stress-time history tab constructor. */
    explicit HistoryTab(QWidget* parent = nullptr);

    /* Sets the current detail. */
    void setDetail(Detail* detail);

    /* On: time group box toggled. */
    void onTimeGroupBoxToggled();

    /* On: time units box edited. */
    void onTimeUnitsBoxEdited();

    /* On: repetitions box edited. */
    void onRepsBoxEdited();

    /* On: edit sample table button clicked. */
    void onEditSampleTableButtonClicked();

};
