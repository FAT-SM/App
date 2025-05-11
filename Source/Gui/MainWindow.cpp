#include <stdexcept>
#include "Gui/MainWindow.hpp"

void MainWindow::buildGui() {

    // this
    setWindowTitle("Fatigue Assessment Tool for Structural Monitoring");
    resize(0, 720);

    // status bar
    _statusBar = new QStatusBar(this);
    this->setStatusBar(_statusBar);

    // status label
    _statusLabel = new QLabel(_statusBar);
    _statusBar->addWidget(_statusLabel);

    // menu bar
    _menuBar = new QMenuBar(this);
    this->setMenuBar(_menuBar);

    // file menu
    _fileMenu = new QMenu(_menuBar);
    _fileMenu->setTitle("File");
    _menuBar->addMenu(_fileMenu);

    // central widget
    _centralWidget = new QWidget(this);
    this->setCentralWidget(_centralWidget);

    // central widget layout
    _centralWidgetLayout = new QGridLayout(_centralWidget);
    _centralWidgetLayout->setContentsMargins(0, 0, 0, 0);
    _centralWidgetLayout->setSpacing(0);
    _centralWidget->setLayout(_centralWidgetLayout);

    // main nav bar
    _mainNavBar = new QListWidget(_centralWidget);
    _mainNavBar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _mainNavBar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _mainNavBar->setSelectionMode(QListWidget::SingleSelection);
    _mainNavBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    _mainNavBar->setSizeAdjustPolicy(QListWidget::AdjustToContents);
    _mainNavBar->setMinimumWidth(180);
    _mainNavBar->setStyleSheet([]() {
        QFile styleSheet(":/StyleSheets/NavBar.qss");
        styleSheet.open(QFile::ReadOnly);
        return styleSheet.readAll();
    }());
    _centralWidgetLayout->addWidget(_mainNavBar, 0, 0);

    // home item
    _homeItem = new QListWidgetItem(_mainNavBar);
    _homeItem->setIcon(QIcon(":/Graphics/Home.svg"));
    _homeItem->setText("Project Home");
    _mainNavBar->addItem(_homeItem);

    // s-n detail item
    _snDetailItem = new QListWidgetItem(_mainNavBar);
    _snDetailItem->setIcon(QIcon(":/Graphics/SnApproach.svg"));
    _snDetailItem->setText("S-N Approach");
    _mainNavBar->addItem(_snDetailItem);

    // lefm detail item
    _lefmDetailItem = new QListWidgetItem(_mainNavBar);
    _lefmDetailItem->setIcon(QIcon(":/Graphics/LefmApproach.svg"));
    _lefmDetailItem->setText("LEFM Approach");
    _mainNavBar->addItem(_lefmDetailItem);

    // side stack
    _sideStack = new QStackedWidget(_centralWidget);
    _sideStack->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    _centralWidgetLayout->addWidget(_sideStack, 1, 0);

    // s-n nav bar
    _snNavBar = new QListWidget(_sideStack);
    _snNavBar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _snNavBar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _snNavBar->setSelectionMode(QListWidget::SingleSelection);
    _snNavBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    _snNavBar->setSizeAdjustPolicy(QListWidget::AdjustToContents);
    _snNavBar->setMinimumWidth(_mainNavBar->minimumWidth());
    _snNavBar->setStyleSheet(_mainNavBar->styleSheet());
    _sideStack->addWidget(_snNavBar);

    // s-n curve item
    _snCurveItem = new QListWidgetItem(_snNavBar);
    _snCurveItem->setText("S-N Curve");
    _snNavBar->addItem(_snCurveItem);

    // s-n history item
    _snHistoryItem = new QListWidgetItem(_snNavBar);
    _snHistoryItem->setText("Stress-Time History");
    _snNavBar->addItem(_snHistoryItem);

    // s-n rainflow item
    _snRainflowItem = new QListWidgetItem(_snNavBar);
    _snRainflowItem->setText("Rainflow Counting");
    _snNavBar->addItem(_snRainflowItem);

    // damage item
    _damageItem = new QListWidgetItem(_snNavBar);
    _damageItem->setText("Fatigue Damage");
    _snNavBar->addItem(_damageItem);

    // lefm nav bar
    _lefmNavBar = new QListWidget(_sideStack);
    _lefmNavBar->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _lefmNavBar->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _lefmNavBar->setSelectionMode(QListWidget::SingleSelection);
    _lefmNavBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    _lefmNavBar->setSizeAdjustPolicy(QListWidget::AdjustToContents);
    _lefmNavBar->setMinimumWidth(_mainNavBar->minimumWidth());
    _lefmNavBar->setStyleSheet(_mainNavBar->styleSheet());
    _sideStack->addWidget(_lefmNavBar);

    // detail selection item
    _detailSelectionItem = new QListWidgetItem(_lefmNavBar);
    _detailSelectionItem->setText("Detail Selection");
    _lefmNavBar->addItem(_detailSelectionItem);

    // paris' law item
    _parisLawItem = new QListWidgetItem(_lefmNavBar);
    _parisLawItem->setText("Crack Growth Curve");
    _lefmNavBar->addItem(_parisLawItem);

    // lefm history item
    _lefmHistoryItem = new QListWidgetItem(_lefmNavBar);
    _lefmHistoryItem->setText("Stress-Time History");
    _lefmNavBar->addItem(_lefmHistoryItem);

    // lefm rainflow item
    _lefmRainflowItem = new QListWidgetItem(_lefmNavBar);
    _lefmRainflowItem->setText("Rainflow Counting");
    _lefmNavBar->addItem(_lefmRainflowItem);

    // main stack
    _mainStack = new QStackedWidget(_centralWidget);
    _mainStack->setProperty("parent", "true");
    _mainStack->setStyleSheet("QStackedWidget[parent='true'] { border: none; border-bottom: 1px solid #D8D8D8; }");
    _centralWidgetLayout->addWidget(_mainStack, 0, 1, 2, 1);

    // home tab
    _homeTab = new HomeTab(_mainStack);
    _mainStack->addWidget(_homeTab);

    // s-n curve tab
    _snCurveTab = new SnCurveTab(_mainStack);
    _mainStack->addWidget(_snCurveTab);

    // history tab
    _historyTab = new HistoryTab(_mainStack);
    _mainStack->addWidget(_historyTab);

    // rainflow tab
    _rainflowTab = new RainflowTab(_mainStack);
    _mainStack->addWidget(_rainflowTab);

    // damage tab
    _damageTab = new DamageTab(_mainStack);
    _mainStack->addWidget(_damageTab);

    // detail selection tab
    _detailSelectionTab = new DetailSelectionTab(_mainStack);
    _mainStack->addWidget(_detailSelectionTab);

    // paris' law tab
    _parisLawTab = new ParisLawTab(_mainStack);
    _mainStack->addWidget(_parisLawTab);

    // connections
    QObject::connect(_homeTab, &HomeTab::editDetailRequestReceived, this, &MainWindow::onEditDetailRequestReceived);
    QObject::connect(_snNavBar, &QListWidget::currentRowChanged, this, &MainWindow::onSnNavBarCurrentRowChanged);
    QObject::connect(_lefmNavBar, &QListWidget::currentRowChanged, this, &MainWindow::onLefmNavBarCurrentRowChanged);
    QObject::connect(_mainNavBar, &QListWidget::currentRowChanged, this, [this]() {
        if (_mainNavBar->currentItem() == _homeItem) setModule(Module::Home);
    });

}

void MainWindow::refreshGui() {
    auto text = QString("Project: %1").arg(_project->name());
    if (_project->isUnsaved()) text += '*';
    if (_detail != nullptr) text += QString(" | Detail: %1").arg(_detail->name());
    _statusLabel->setText(text);
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    _project = new Project(this);
    _detail = nullptr;
    buildGui();
    refreshGui();
    setModule(Module::Home, true);
}

void MainWindow::setModule(Module module, bool force) {
    if (!force && module == _module) return;
    _module = module;

    // update gui
    switch (_module) {
        case Module::Home:
            _detail = nullptr;
            _snNavBar->clearSelection();
            _snNavBar->setCurrentRow(-1);
            _lefmNavBar->clearSelection();
            _lefmNavBar->setCurrentRow(-1);
            _sideStack->setVisible(false);
            _mainNavBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            _mainNavBar->setCurrentRow(0);
            _mainNavBar->takeItem(_mainNavBar->row(_snDetailItem));
            _mainNavBar->takeItem(_mainNavBar->row(_lefmDetailItem));
            _mainStack->setCurrentWidget(_homeTab);
            _homeTab->setProject(_project);
            break;
        case Module::SnDetail:
            if (_detail == nullptr) break;
            _homeTab->setProject(nullptr);
            _mainNavBar->addItem(_snDetailItem);
            _sideStack->setVisible(true);
            _sideStack->setCurrentWidget(_snNavBar);
            _mainNavBar->setCurrentItem(_snDetailItem);
            _mainNavBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
            _snNavBar->setCurrentRow(0);
            break;
        case Module::LefmDetail:
            if (_detail == nullptr) break;
            _homeTab->setProject(nullptr);
            _mainNavBar->addItem(_lefmDetailItem);
            _sideStack->setVisible(true);
            _sideStack->setCurrentWidget(_lefmNavBar);
            _mainNavBar->setCurrentItem(_lefmDetailItem);
            _mainNavBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
            _lefmNavBar->setCurrentRow(0);
            break;
        default:
            throw std::logic_error("Case not implemented.");
    }
    refreshGui();

}

void MainWindow::onEditDetailRequestReceived(int index) {

    // get detail
    Detail* detail = &_project->detail(index);
    if (detail == _detail) return;
    _detail = detail;

    // update gui
    switch (_detail->approach()) {
        case Detail::Approach::Sn: setModule(Module::SnDetail); break;
        case Detail::Approach::Lefm: setModule(Module::LefmDetail); break;
        default: throw std::logic_error("Case not implemented.");
    }

}

void MainWindow::onSnNavBarCurrentRowChanged() {

    // clear previous
    _snCurveTab->setDetail(nullptr);
    _historyTab->setDetail(nullptr);
    _rainflowTab->setDetail(nullptr);
    _damageTab->setDetail(nullptr);
    if (_detail == nullptr) return;

    // show new
    if (_snNavBar->currentItem() == _snCurveItem) {
        _mainStack->setCurrentWidget(_snCurveTab);
        _snCurveTab->setDetail(dynamic_cast<SnDetail*>(_detail));
    }
    else if (_snNavBar->currentItem() == _snHistoryItem) {
        _mainStack->setCurrentWidget(_historyTab);
        _historyTab->setDetail(_detail);
    }
    else if (_snNavBar->currentItem() == _snRainflowItem) {
        _mainStack->setCurrentWidget(_rainflowTab);
        _rainflowTab->setDetail(_detail);
    }
    else if (_snNavBar->currentItem() == _damageItem) {
        _mainStack->setCurrentWidget(_damageTab);
        _damageTab->setDetail(dynamic_cast<SnDetail*>(_detail));
    }

}

void MainWindow::onLefmNavBarCurrentRowChanged() {

    // clear previous
    _historyTab->setDetail(nullptr);
    _rainflowTab->setDetail(nullptr);
    if (_detail == nullptr) return;

    // show new
    if (_lefmNavBar->currentItem() == _detailSelectionItem) {
        _mainStack->setCurrentWidget(_detailSelectionTab);
        _detailSelectionTab->setDetail(dynamic_cast<LefmDetail*>(_detail));
    }
    else if (_lefmNavBar->currentItem() == _parisLawItem) {
        _mainStack->setCurrentWidget(_parisLawTab);
        _parisLawTab->setDetail(dynamic_cast<LefmDetail*>(_detail));
    }
    else if (_lefmNavBar->currentItem() == _lefmHistoryItem) {
        _mainStack->setCurrentWidget(_historyTab);
        _historyTab->setDetail(_detail);
    }
    else if (_lefmNavBar->currentItem() == _lefmRainflowItem) {
        _mainStack->setCurrentWidget(_rainflowTab);
        _rainflowTab->setDetail(_detail);
    }

}
