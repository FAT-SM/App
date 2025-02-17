#include <stdexcept>
#include "Gui/DamageTab.hpp"

void DamageTab::buildGui() {

    // layout
    _layout = new QGridLayout(this);
    this->setLayout(_layout);

    // resistance factor group box
    _resistanceFactorGroupBox = new QGroupBox(this);
    _resistanceFactorGroupBox->setTitle("Partial Factor for Fatigue Resistance");
    _layout->addWidget(_resistanceFactorGroupBox, 0, 0);

    // resistance factor group box layout
    _resistanceFactorGroupBoxLayout = new QGridLayout(_resistanceFactorGroupBox);
    _resistanceFactorGroupBox->setLayout(_resistanceFactorGroupBoxLayout);

    // design concept label
    _designConceptLabel = new QLabel(_resistanceFactorGroupBox);
    _designConceptLabel->setText("Design Concept:");
    _designConceptLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _resistanceFactorGroupBoxLayout->addWidget(_designConceptLabel, 0, 0);

    // design concept box
    _designConceptBox = new QComboBox(_resistanceFactorGroupBox);
    _resistanceFactorGroupBoxLayout->addWidget(_designConceptBox, 0, 1);

    // consequence of failure label
    _consequenceOfFailureLabel = new QLabel(_resistanceFactorGroupBox);
    _consequenceOfFailureLabel->setText("Consequence of Failure:");
    _consequenceOfFailureLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _resistanceFactorGroupBoxLayout->addWidget(_consequenceOfFailureLabel, 1, 0);

    // consequence of failure box
    _consequenceOfFailureBox = new QComboBox(_resistanceFactorGroupBox);
    _resistanceFactorGroupBoxLayout->addWidget(_consequenceOfFailureBox, 1, 1);

    // resistance factor value label
    _resistanceFactorValueLabel = new QLabel(_resistanceFactorGroupBox);
    _resistanceFactorValueLabel->setText("Partial Factor Value:");
    _resistanceFactorValueLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _resistanceFactorGroupBoxLayout->addWidget(_resistanceFactorValueLabel, 2, 0);

    // resistance factor value box
    _resistanceFactorValueBox = new QLineEdit(_resistanceFactorGroupBox);
    _resistanceFactorValueBox->setFixedWidth(150);
    _resistanceFactorGroupBoxLayout->addWidget(_resistanceFactorValueBox, 2, 1);

    // custom resistance factor box
    _customResistanceFactorBox = new QCheckBox(_resistanceFactorGroupBox);
    _customResistanceFactorBox->setText("Use Custom Value");
    _resistanceFactorGroupBoxLayout->addWidget(_customResistanceFactorBox, 2, 2);

    // stress factor group box
    _stressFactorGroupBox = new QGroupBox(this);
    _stressFactorGroupBox->setTitle("Partial Factor for Applied Stress Ranges");
    _layout->addWidget(_stressFactorGroupBox, 1, 0);

    // stress factor group box layout
    _stressFactorGroupBoxLayout = new QGridLayout(_stressFactorGroupBox);
    _stressFactorGroupBox->setLayout(_stressFactorGroupBoxLayout);

    // stress factor value label
    _stressFactorValueLabel = new QLabel(_stressFactorGroupBox);
    _stressFactorValueLabel->setText("Partial Factor Value:");
    _stressFactorValueLabel->setAlignment(Qt::AlignRight | Qt::AlignCenter);
    _stressFactorGroupBoxLayout->addWidget(_stressFactorValueLabel, 0, 0);

    // stress factor value box
    _stressFactorValueBox = new QLineEdit(_stressFactorGroupBox);
    _stressFactorValueBox->setFixedWidth(150);
    _stressFactorGroupBoxLayout->addWidget(_stressFactorValueBox, 0, 1);

    // damage group box
    _damageGroupBox = new QGroupBox(this);
    _damageGroupBox->setTitle("Fatigue Damage Accumulation");
    _layout->addWidget(_damageGroupBox, 0, 1, 3, 1);

    // damage group box layout
    _damageGroupBoxLayout = new QGridLayout(_damageGroupBox);
    _damageGroupBox->setLayout(_damageGroupBoxLayout);

    // compute button
    _computeButton = new QPushButton(_damageGroupBox);
    _computeButton->setIcon(QIcon(":/Graphics/Calculator.svg"));
    _computeButton->setText("Calculate");
    _damageGroupBoxLayout->addWidget(_computeButton, 0, 0);

    // damage table
    _damageTable = new QTableWidget(_damageGroupBox);
    _damageTable->setEditTriggers(QTableWidget::NoEditTriggers);
    _damageGroupBoxLayout->addWidget(_damageTable, 1, 0);

    // sizes
    _layout->setRowStretch(0, 0);
    _layout->setRowStretch(1, 0);
    _layout->setRowStretch(2, 1);
    _layout->setColumnStretch(0, 0);
    _layout->setColumnStretch(1, 1);
    _stressFactorGroupBoxLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Fixed), 0, 2);
    _stressFactorValueLabel->setFixedWidth(_consequenceOfFailureLabel->sizeHint().width());

    // connections
    connect(_designConceptBox, &QComboBox::activated, this, &DamageTab::onDesignConceptChanged);
    connect(_consequenceOfFailureBox, &QComboBox::activated, this, &DamageTab::onConsequenceOfFailureChanged);
    connect(_customResistanceFactorBox, &QCheckBox::checkStateChanged, this, &DamageTab::onUseCustomResistanceChanged);
    connect(_resistanceFactorValueBox, &QLineEdit::editingFinished, this, &DamageTab::onResistanceFactorChanged);
    connect(_stressFactorValueBox, &QLineEdit::editingFinished, this, &DamageTab::onStressFactorChanged);

}

void DamageTab::refreshGui() {
    if (_detail == nullptr) {
        _designConceptBox->setEnabled(false);
        _consequenceOfFailureBox->setEnabled(false);
        _customResistanceFactorBox->setEnabled(false);
        _resistanceFactorValueBox->setEnabled(false);
        _stressFactorValueBox->setEnabled(false);
        _designConceptBox->clear();
        _consequenceOfFailureBox->clear();
        _customResistanceFactorBox->setChecked(false);
        _resistanceFactorValueBox->clear();
        _stressFactorValueBox->clear();
    } else {
        if (_detail->useCustomResistanceFactor()) {
            _designConceptBox->setEnabled(false);
            _consequenceOfFailureBox->setEnabled(false);
            _designConceptBox->clear();
            _consequenceOfFailureBox->clear();
        } else {
            _designConceptBox->setEnabled(true);
            _consequenceOfFailureBox->setEnabled(true);
            if (_designConceptBox->count() == 0)
                _designConceptBox->addItems({ "Safe Life", "Damage Tolerant" });
            if (_consequenceOfFailureBox->count() == 0)
                _consequenceOfFailureBox->addItems({ "Low Consequence", "Medium Consequence", "High Consequence" });
            switch (_detail->designConcept()) {
                case SnDetail::DesignConcept::SafeLife:
                    _designConceptBox->setCurrentText("Safe Life");
                    break;
                case SnDetail::DesignConcept::DamageTolerant:
                    _designConceptBox->setCurrentText("Damage Tolerant");
                    break;
                default:
                    throw std::logic_error("Case not implemented.");
            }
            switch (_detail->consequenceOfFailure()) {
                case SnDetail::ConsequenceOfFailure::LowConsequence:
                    _consequenceOfFailureBox->setCurrentText("Low Consequence");
                    break;
                case SnDetail::ConsequenceOfFailure::MediumConsequence:
                    _consequenceOfFailureBox->setCurrentText("Medium Consequence");
                    break;
                case SnDetail::ConsequenceOfFailure::HighConsequence:
                    _consequenceOfFailureBox->setCurrentText("High Consequence");
                    break;
                default:
                    throw std::logic_error("Case not implemented.");
            }
        }
        _customResistanceFactorBox->setEnabled(true);
        _resistanceFactorValueBox->setEnabled(_detail->useCustomResistanceFactor());
        _stressFactorValueBox->setEnabled(true);
        _customResistanceFactorBox->setChecked(_detail->useCustomResistanceFactor());
        _resistanceFactorValueBox->setText(QString::number(_detail->resistanceFactor()));
        _stressFactorValueBox->setText(QString::number(_detail->stressFactor()));
    }
}

DamageTab::DamageTab(QWidget* parent) : QWidget(parent) {
    buildGui();
    refreshGui();
}

void DamageTab::setDetail(SnDetail* detail) {
    if (detail == _detail) return;
    if (_detail != nullptr) QObject::disconnect(_detail, &SnDetail::modified, this, &DamageTab::refreshGui);
    _detail = detail;
    if (_detail != nullptr) QObject::connect(_detail, &SnDetail::modified, this, &DamageTab::refreshGui);
    refreshGui();
}

void DamageTab::onDesignConceptChanged() {
    if (_detail == nullptr || _designConceptBox->currentIndex() == -1) return;
    if (_designConceptBox->currentText() == "Safe Life")
        _detail->setDesignConcept(SnDetail::DesignConcept::SafeLife);
    else if (_designConceptBox->currentText() == "Damage Tolerant")
        _detail->setDesignConcept(SnDetail::DesignConcept::DamageTolerant);
    else
        throw std::logic_error("Case not implemented.");
}

void DamageTab::onConsequenceOfFailureChanged() {
    if (_detail == nullptr || _consequenceOfFailureBox->currentIndex() == -1) return;
    if (_consequenceOfFailureBox->currentText() == "Low Consequence")
        _detail->setConsequenceOfFailure(SnDetail::ConsequenceOfFailure::LowConsequence);
    else if (_consequenceOfFailureBox->currentText() == "Medium Consequence")
        _detail->setConsequenceOfFailure(SnDetail::ConsequenceOfFailure::MediumConsequence);
    else if (_consequenceOfFailureBox->currentText() == "High Consequence")
        _detail->setConsequenceOfFailure(SnDetail::ConsequenceOfFailure::HighConsequence);
    else
        throw std::logic_error("Case not implemented.");
}

void DamageTab::onUseCustomResistanceChanged() {
    if (_detail == nullptr) return;
    _detail->setUseCustomResistanceFactor(_customResistanceFactorBox->isChecked());
}

void DamageTab::onResistanceFactorChanged() {
    if (_detail == nullptr) return;
    try {
        bool ok = false;
        double resistanceFactor = _resistanceFactorValueBox->text().toDouble(&ok);
        if (!ok) throw std::invalid_argument("The input must be a valid real number.");
        else _detail->setCustomResistanceFactor(resistanceFactor);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
            QString("Invalid partial factor for fatigue resistance:\n%1").arg(e.what()));
        _resistanceFactorValueBox->setText(QString::number(_detail->resistanceFactor()));
    }
}

void DamageTab::onStressFactorChanged() {
    if (_detail == nullptr) return;
    try {
        bool ok = false;
        double stressFactor = _stressFactorValueBox->text().toDouble(&ok);
        if (!ok) throw std::invalid_argument("The input must be a valid real number.");
        else _detail->setStressFactor(stressFactor);
    }
    catch (const std::exception& e) {
        QMessageBox::critical(this, "Error",
            QString("Invalid partial factor for applied stress ranges:\n%1").arg(e.what()));
        _stressFactorValueBox->setText(QString::number(_detail->stressFactor()));
    }
}
