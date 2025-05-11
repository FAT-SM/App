#include "Gui/ParisLawTab.hpp"

void ParisLawTab::buildGui() {

    // layout
    _layout = new QGridLayout(this);
    this->setLayout(_layout);

    // paris' law group box
    _parisLawGroupBox = new QGroupBox(this);
    _parisLawGroupBox->setTitle("Paris' Law");
    _layout->addWidget(_parisLawGroupBox, 0, 0);

    // paris' law group box layout
    _parisLawGroupBoxLayout = new QGridLayout(_parisLawGroupBox);
    _parisLawGroupBox->setLayout(_parisLawGroupBoxLayout);

    // paris' law coefficient label
    _parisLawCoefficientLabel = new QLabel(_parisLawGroupBox);
    _parisLawCoefficientLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _parisLawCoefficientLabel->setText("Paris' Law Coefficient, C [√(mm)/(MPa·cycle)]:");
    _parisLawGroupBoxLayout->addWidget(_parisLawCoefficientLabel, 0, 1);

    // paris' law coefficient box
    _parisLawCoefficientBox = new QLineEdit(_parisLawGroupBox);
    _parisLawGroupBoxLayout->addWidget(_parisLawCoefficientBox, 0, 2);

    // paris' law exponent label
    _parisLawExponentLabel = new QLabel(_parisLawGroupBox);
    _parisLawExponentLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _parisLawExponentLabel->setText("Paris' Law Exponent, m [-]:");
    _parisLawGroupBoxLayout->addWidget(_parisLawExponentLabel, 1, 1);

    // paris' law exponent box
    _parisLawExponentBox = new QLineEdit(_parisLawGroupBox);
    _parisLawGroupBoxLayout->addWidget(_parisLawExponentBox, 1, 2);

    // extra parameters group box
    _extraParametersGroupBox = new QGroupBox(this);
    _extraParametersGroupBox->setTitle("Additional Fatigue Crack Growth Parameters");
    _layout->addWidget(_extraParametersGroupBox, 1, 0);

    // extra parameters group box layout
    _extraParametersGroupBoxLayout = new QGridLayout(_extraParametersGroupBox);
    _extraParametersGroupBox->setLayout(_extraParametersGroupBoxLayout);

    // sif threshold label
    _sifThresholdLabel = new QLabel(_extraParametersGroupBox);
    _sifThresholdLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _sifThresholdLabel->setText("Threshold Stress Intensity Factor Range, ΔK<sub>th</sub> [MPa·√(mm)]:");
    _extraParametersGroupBoxLayout->addWidget(_sifThresholdLabel, 0, 1);

    // sif threshold box
    _sifThresholdBox = new QLineEdit(_extraParametersGroupBox);
    _extraParametersGroupBoxLayout->addWidget(_sifThresholdBox, 0, 2);

    // sif critical label
    _sifCriticalLabel = new QLabel(_extraParametersGroupBox);
    _sifCriticalLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _sifCriticalLabel->setText("Critical Stress Intensity Factor Range, ΔK<sub>cr</sub> [MPa·√(mm)]:");
    _extraParametersGroupBoxLayout->addWidget(_sifCriticalLabel, 1, 1);

    // sif critical box
    _sifCriticalBox = new QLineEdit(_extraParametersGroupBox);
    _extraParametersGroupBoxLayout->addWidget(_sifCriticalBox, 1, 2);

    // svg widget
    _svgWidget = new QSvgWidget(this);
    _svgWidget->setStyleSheet("border: 1px solid #D8D8D8; background: #FFFFFF;");
    _svgWidget->load(QString(":/Graphics/ParisLaw.svg"));
    _svgWidget->renderer()->setAspectRatioMode(Qt::AspectRatioMode::KeepAspectRatio);
    _layout->addWidget(_svgWidget, 0, 1, 3, 1);

    // spacers
    _parisLawGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 0);
    _parisLawGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 3);
    _extraParametersGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 0);
    _extraParametersGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 3);

    // stretches
    _layout->setRowStretch(0, 0);
    _layout->setRowStretch(1, 0);
    _layout->setRowStretch(2, 1);
    _layout->setColumnStretch(0, 0);
    _layout->setColumnStretch(1, 1);

}

void ParisLawTab::refreshGui() {
    if (_detail == nullptr) {






    }
    else {





    }
}

ParisLawTab::ParisLawTab(QWidget* parent) : QWidget(parent) {
    buildGui();
    refreshGui();
}

void ParisLawTab::setDetail(LefmDetail* detail) {
    if (detail == _detail) return;
    if (_detail != nullptr) QObject::disconnect(_detail, &LefmDetail::modified, this, &ParisLawTab::refreshGui);
    _detail = detail;
    if (_detail != nullptr) QObject::connect(_detail, &LefmDetail::modified, this, &ParisLawTab::refreshGui);
    refreshGui();
}
