#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtCharts>
#include "Model.hpp"

/* The S-N curve tab. */
class SnCurveTab : public QWidget {
    Q_OBJECT

private:
    QGridLayout* _layout;
    QSplitter* _splitter;
    QWidget* _leftWidget;
    QGridLayout* _leftWidgetLayout;
    QWidget* _rightWidget;
    QGridLayout* _rightWidgetLayout;
    QGroupBox* _categoryGroupBox;
    QGridLayout* _categoryGroupBoxLayout;
    QLabel* _categoryLabel;
    QLineEdit* _categoryBox;
    QLabel* _categoryInfoLabel;
    QGroupBox* _slopesGroupBox;
    QGridLayout* _slopesGroupBoxLayout;
    QTableWidget* _slopesTable;
    QPushButton* _editSlopesTableButton;
    QChart* _chart;
    QChartView* _chartView;
    SnDetail* _detail;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

    /* Refreshes the plot. */
    void refreshPlot();

public:

    /* S-N curve tab constructor. */
    explicit SnCurveTab(QWidget* parent = nullptr);

    /* Sets the current detail. */
    void setDetail(SnDetail* detail);

    /* On: category box edited. */
    void onCategoryBoxEdited();

    /* On: edit slopes table button. */
    void onEditSlopesTableButton();

};
