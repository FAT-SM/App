#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Model.hpp"

/* The project home tab. */
class HomeTab : public QWidget {
    Q_OBJECT

private:
    QGridLayout* _layout;
    QGroupBox* _projectGroupBox;
    QGridLayout* _projectGroupBoxLayout;
    QLabel* _projectNameLabel;
    QLineEdit* _projectNameBox;
    QLabel* _projectDescriptionLabel;
    QPlainTextEdit* _projectDescriptionBox;
    QGroupBox* _detailsGroupBox;
    QGridLayout* _detailsGroupBoxLayout;
    QTableWidget* _detailsTable;
    QPushButton* _newSnDetailButton;
    QPushButton* _newLefmDetailButton;
    QPushButton* _editDetailButton;
    QPushButton* _deleteDetailButton;
    Project* _project;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

public:

    /* The home tab constructor. */
    explicit HomeTab(QWidget* parent = nullptr);

    /* The event filter. */
    bool eventFilter(QObject* watched, QEvent* event) override;

    /* Sets the current project. */
    void setProject(Project* project);

    /* On: current detail changed. */
    void onCurrentDetailChanged();

    /* On: project name box edited. */
    void onProjectNameBoxEdited();

    /* On: project description box edited. */
    void onProjectDescriptionBoxEdited();

    /* On: details table edited. */
    void onDetailsTableEdited(int row, int column);

    /* On: edit detail button clicked. */
    void onEditDetailButtonClicked();

    /* On: delete detail button clicked. */
    void onDeleteDetailButtonClicked();

    /* On: new S-N detail button clicked. */
    void onNewSnDetailButtonClicked();

signals:

    /* Emitted on: edit detail request received. */
    void editDetailRequestReceived(int index);

};
