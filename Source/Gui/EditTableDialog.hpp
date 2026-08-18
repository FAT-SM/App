#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <functional>
#include "Core.hpp"

/* The edit table dialog. */
class EditTableDialog : public QDialog {
    Q_OBJECT

private:
    QGridLayout* _layout;
    QMenuBar* _menuBar;
    QMenu* _toolsMenu;
    QAction* _pasteAction;
    QAction* _loadAction;
    QAction* _clearAction;
    QWidget* _centralWidget;
    QGridLayout* _centralWidgetLayout;
    QGroupBox* _groupBox;
    QGridLayout* _groupBoxLayout;
    QTableWidget* _table;
    QLabel* _rowCountLabel;
    QLabel* _columnCountLabel;
    QLineEdit* _rowCountBox;
    QLineEdit* _columnCountBox;
    QSpacerItem* _rowCountSpacer;
    QSpacerItem* _columnCountSpacer;
    QPushButton* _okButton;
    QPushButton* _cancelButton;
    int _rowCount;
    int _columnCount;
    bool _stretchRows;
    bool _stretchColumns;
    std::function<bool(const Matrix<QString>&)> _parser;

    /* Builds the GUI. */
    void buildGui();

public:

    /* Edit table dialog constructor. */
    explicit EditTableDialog(QWidget* parent = nullptr, std::function<bool(const Matrix<QString>&)> parser = nullptr);

    /* Sets the number of rows. */
    void setRowCount(int rowCount);

    /* Sets the number of columns. */
    void setColumnCount(int columnCount);

    /* Sets if rows should be stretched. */
    void setStretchRows(bool stretchRows);

    /* Sets if columns should be stretched. */
    void setStretchColumns(bool stretchColumns);

    /* Sets if the user can change the number of rows. */
    void setFixedRowCount(bool fixedRowCount);

    /* Sets if the user can change the number of columns. */
    void setFixedColumnCount(bool fixedColumnCount);

    /* Sets the row labels. */
    void setRowLabels(const QStringList& labels);

    /* Sets the column labels. */
    void setColumnLabels(const QStringList& labels);

    /* Sets the table's data. */
    template<typename T>
    void setTableData(const Matrix<T>& tableData);

    /* On: row count box editing finished. */
    void onRowCountBoxEdited();

    /* On: column count box editing finished. */
    void onColumnCountBoxEdited();

    /* On: OK button clicked. */
    void onOkButtonClicked();

    /* On: paste action triggered. */
    void onPasteActionTriggered();

    /* On: load action triggered. */
    void onLoadActionTriggered();

    /* On: clear action triggered. */
    void onClearActionTriggered();

};
