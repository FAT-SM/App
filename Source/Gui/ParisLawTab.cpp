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

    // connections
    connect(_parisLawCoefficientBox, &QLineEdit::editingFinished, this, &ParisLawTab::onParisLawCoefficientBoxEdited);
    connect(_parisLawExponentBox, &QLineEdit::editingFinished, this, &ParisLawTab::onParisLawExponentBoxEdited);
    connect(_sifThresholdBox, &QLineEdit::editingFinished, this, &ParisLawTab::onSifThresholdBoxEdited);
    connect(_sifCriticalBox, &QLineEdit::editingFinished, this, &ParisLawTab::onSifCriticalBoxEdited);

}

void ParisLawTab::refreshGui() {
    if (_detail == nullptr) {
        _parisLawCoefficientBox->setEnabled(false);
        _parisLawExponentBox->setEnabled(false);
        _sifThresholdBox->setEnabled(false);
        _sifCriticalBox->setEnabled(false);
        _parisLawCoefficientBox->clear();
        _parisLawExponentBox->clear();
        _sifThresholdBox->clear();
        _sifCriticalBox->clear();
    } else {
        _parisLawCoefficientBox->setEnabled(true);
        _parisLawExponentBox->setEnabled(true);
        _sifThresholdBox->setEnabled(true);
        _sifCriticalBox->setEnabled(true);
        _parisLawCoefficientBox->setText(
            _detail->parisCoefficient() ? QString::number(*_detail->parisCoefficient()) : "");
        _parisLawExponentBox->setText(
            _detail->parisExponent() ? QString::number(*_detail->parisExponent()) : "");
        _sifThresholdBox->setText(
            _detail->sifThreshold() ? QString::number(*_detail->sifThreshold()) : "");
        _sifCriticalBox->setText(
            _detail->sifCritical() ? QString::number(*_detail->sifCritical()) : "");
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

void ParisLawTab::onParisLawCoefficientBoxEdited() {
    if (_detail == nullptr) return;
    if (!_detail->parisCoefficient() && _parisLawCoefficientBox->text().isEmpty()) return;
    try {
        bool ok = false;
        double value = _parisLawCoefficientBox->text().toDouble(&ok);
        if (!ok) throw std::invalid_argument("The input must be a valid real number.");
        else _detail->setParisCoefficient(value);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Invalid Paris' law coefficient:\n%1").arg(e.what()));
        _parisLawCoefficientBox->setText(
            _detail->parisCoefficient() ? QString::number(*_detail->parisCoefficient()) : "");
    }
}

void ParisLawTab::onParisLawExponentBoxEdited() {
    if (_detail == nullptr) return;
    if (!_detail->parisExponent() && _parisLawExponentBox->text().isEmpty()) return;
    try {
        bool ok = false;
        double value = _parisLawExponentBox->text().toDouble(&ok);
        if (!ok) throw std::invalid_argument("The input must be a valid real number.");
        else _detail->setParisExponent(value);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error", QString("Invalid Paris' law exponent:\n%1").arg(e.what()));
        _parisLawExponentBox->setText(_detail->parisExponent() ? QString::number(*_detail->parisExponent()) : "");
    }
}

void ParisLawTab::onSifThresholdBoxEdited() {
    if (_detail == nullptr) return;
    if (!_detail->sifThreshold() && _sifThresholdBox->text().isEmpty()) return;
    try {
        bool ok = false;
        double value = _sifThresholdBox->text().toDouble(&ok);
        if (!ok) throw std::invalid_argument("The input must be a valid real number.");
        else _detail->setSifThreshold(value);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
            QString("Invalid threshold stress intensity factor range:\n%1").arg(e.what()));
        _sifThresholdBox->setText(_detail->sifThreshold() ? QString::number(*_detail->sifThreshold()) : "");
    }
}

void ParisLawTab::onSifCriticalBoxEdited() {
    if (_detail == nullptr) return;
    if (!_detail->sifCritical() && _sifCriticalBox->text().isEmpty()) return;
    try {
        bool ok = false;
        double value = _sifCriticalBox->text().toDouble(&ok);
        if (!ok) throw std::invalid_argument("The input must be a valid real number.");
        else _detail->setSifCritical(value);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
            QString("Invalid critical stress intensity factor range:\n%1").arg(e.what()));
        _sifCriticalBox->setText(_detail->sifCritical() ? QString::number(*_detail->sifCritical()) : "");
    }
}
