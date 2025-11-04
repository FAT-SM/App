#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtSvgWidgets>
#include "Model.hpp"

/* The LEFM detail selection tab. */
class DetailSelectionTab : public QWidget {
    Q_OBJECT

private:
    QGridLayout* _layout;
    QLabel* _constructionalDetailLabel;
    QComboBox* _constructionalDetailBox;
    QSplitter* _splitter;
    QSvgWidget* _svgWidget;
    QGroupBox* _parametersGroupBox;
    QGridLayout* _parametersGroupBoxLayout;
    QTableWidget* _parametersTable;
    QPushButton* _editParametersTableButton;
    LefmDetail* _detail;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

public:

    /* LEFM detail selection tab constructor. */
    explicit DetailSelectionTab(QWidget* parent = nullptr);

    /* Sets the current detail. */
    void setDetail(LefmDetail* detail);

    /* On: detail selected. */
    void onDetailSelected();

    /* On: edit table button clicked. */
    void onEditTableButtonClicked();

};
