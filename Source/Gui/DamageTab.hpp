#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Model.hpp"

/* The damage tab. */
class DamageTab : public QWidget {
    Q_OBJECT

private:
    QGridLayout* _layout;
    QGroupBox* _resistanceFactorGroupBox;
    QGridLayout* _resistanceFactorGroupBoxLayout;
    QLabel* _designConceptLabel;
    QComboBox* _designConceptBox;
    QLabel* _consequenceOfFailureLabel;
    QComboBox* _consequenceOfFailureBox;
    QLabel* _resistanceFactorValueLabel;
    QLineEdit* _resistanceFactorValueBox;
    QCheckBox* _customResistanceFactorBox;
    QGroupBox* _stressFactorGroupBox;
    QGridLayout* _stressFactorGroupBoxLayout;
    QLabel* _stressFactorValueLabel;
    QLineEdit* _stressFactorValueBox;
    QGroupBox* _damageGroupBox;
    QGridLayout* _damageGroupBoxLayout;
    QPushButton* _computeButton;
    QTableWidget* _damageTable;
    SnDetail* _detail;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

public:

    /* Damage tab constructor. */
    explicit DamageTab(QWidget* parent = nullptr);

    /* Sets the current detail. */
    void setDetail(SnDetail* detail);

    /* On: design concept changed. */
    void onDesignConceptChanged();

    /* On: consequence of failure changed. */
    void onConsequenceOfFailureChanged();

    /* On: use custom resistance changed. */
    void onUseCustomResistanceChanged();

    /* On: resistance factor changed. */
    void onResistanceFactorChanged();

    /* On: stress factor changed. */
    void onStressFactorChanged();

};
