#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include <QtSvgWidgets>
#include "Model.hpp"

/* The Paris' law tab. */
class ParisLawTab : public QWidget {
    Q_OBJECT

private:
    QGridLayout* _layout;
    QGroupBox* _parisLawGroupBox;
    QGridLayout* _parisLawGroupBoxLayout;
    QLabel* _parisLawCoefficientLabel;
    QLineEdit* _parisLawCoefficientBox;
    QLabel* _parisLawExponentLabel;
    QLineEdit* _parisLawExponentBox;
    QGroupBox* _extraParametersGroupBox;
    QGridLayout* _extraParametersGroupBoxLayout;
    QLabel* _sifThresholdLabel;
    QLineEdit* _sifThresholdBox;
    QLabel* _sifCriticalLabel;
    QLineEdit* _sifCriticalBox;
    QSvgWidget* _svgWidget;
    LefmDetail* _detail;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

public:

    /* Paris' law tab constructor. */
    explicit ParisLawTab(QWidget* parent = nullptr);

    /* Sets the current detail. */
    void setDetail(LefmDetail* detail);

    /* On: Paris's law coefficient box edited. */
    void onParisLawCoefficientBoxEdited();

    /* On: Paris's law exponent box edited. */
    void onParisLawExponentBoxEdited();

    /* On: stress intensity factor threshold box edited. */
    void onSifThresholdBoxEdited();

    /* On: critical stress intensity factor box edited. */
    void onSifCriticalBoxEdited();

};
