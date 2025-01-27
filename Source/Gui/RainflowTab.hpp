#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtCharts>
#include <QtDataVisualization>
#include "Model.hpp"

/* The rainflow counting tab. */
class RainflowTab : public QWidget {
    Q_OBJECT

private:
    QGridLayout* _layout;
    QSplitter* _splitter;
    QWidget* _leftWidget;
    QGridLayout* _leftWidgetLayout;
    QWidget* _rightWidget;
    QGridLayout* _rightWidgetLayout;
    QGroupBox* _rainflowGroupBox;
    QGridLayout* _rainflowGroupBoxLayout;
    QPushButton* _computeButton;
    QTableWidget* _rainflowTable;
    QChart* _chartA;
    QChart* _chartB;
    Q3DBars* _chartC;
    QChartView* _chartViewA;
    QChartView* _chartViewB;
    QWidget* _chartViewC;
    SnDetail* _detail;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

    /* Refreshes the plot. */
    void refreshPlot();

    /* Refreshes the plot A. */
    void refreshPlotA();

    /* Refreshes the plot B. */
    void refreshPlotB();

    /* Refreshes the plot C. */
    void refreshPlotC();

public:

    /* Rainflow counting tab constructor. */
    explicit RainflowTab(QWidget* parent = nullptr);

    /* The event filter. */
    bool eventFilter(QObject* watched, QEvent* event) override;

    /* Sets the current detail. */
    void setDetail(SnDetail* detail);

    /* On: compute button clicked. */
    void onComputeButtonClicked();

};
