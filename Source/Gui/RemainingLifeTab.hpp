#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtCharts>
#include "Model.hpp"

/* The remaining life tab. */
class RemainingLifeTab : public QWidget {
	Q_OBJECT

private:
    QGridLayout* _layout;
    QSplitter* _splitter;
    QWidget* _leftWidget;
    QGridLayout* _leftWidgetLayout;
    QWidget* _rightWidget;
    QGridLayout* _rightWidgetLayout;
    QGroupBox* _computeGroupBox;
    QGridLayout* _computeGroupBoxLayout;
    QPushButton* _computeButton;
    QTableWidget* _table;
    QPushButton* _editTableButton;
    QChart* _chart;
    QChartView* _chartView;
    LefmDetail* _detail;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

    /* Refreshes the plot. */
    void refreshPlot();

public:

    /* Remaining life tab constructor. */
    explicit RemainingLifeTab(QWidget* parent = nullptr);

    /* Sets the current detail. */
    void setDetail(LefmDetail* detail);

    /* On: compute button clicked. */
    void onComputeButtonClicked();

    /* On: edit table button clicked. */
    void onEditTableButtonClicked();

};
