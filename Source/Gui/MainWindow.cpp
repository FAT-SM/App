#include <stdexcept>
#include "Gui/MainWindow.hpp"

void MainWindow::buildGui() {

    // this
    setWindowTitle("Fatigue Assessment Tool for Structural Monitoring");
    resize(1200, 600);

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
    _mainNavBar->setStyleSheet([]() {
        QFile styleSheet(":/res/qss/MainNavBar.qss");
        styleSheet.open(QFile::ReadOnly);
        return styleSheet.readAll();
    }());
    _centralWidgetLayout->addWidget(_mainNavBar, 0, 0);

    // home item
    _homeItem = new QListWidgetItem(_mainNavBar);
    _homeItem->setIcon(QIcon(":/res/svg/Home.svg"));
    _homeItem->setText("Project Home");
    _mainNavBar->addItem(_homeItem);

    // s-n detail item
    _snDetailItem = new QListWidgetItem(_mainNavBar);
    _snDetailItem->setIcon(QIcon(":/res/svg/StressLife.svg"));
    _snDetailItem->setText("S-N Approach");
    _mainNavBar->addItem(_snDetailItem);

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
    _snNavBar->setStyleSheet(_mainNavBar->styleSheet());
    _sideStack->addWidget(_snNavBar);

    // s-n curve item
    _snCurveItem = new QListWidgetItem(_snNavBar);
    _snCurveItem->setText("S-N Curve");
    _snNavBar->addItem(_snCurveItem);

    // history item
    _historyItem = new QListWidgetItem(_snNavBar);
    _historyItem->setText("Stress-Time History");
    _snNavBar->addItem(_historyItem);

    // rainflow item
    _rainflowItem = new QListWidgetItem(_snNavBar);
    _rainflowItem->setText("Rainflow Counting");
    _snNavBar->addItem(_rainflowItem);

    // main stack
    _mainStack = new QStackedWidget(_centralWidget);
    _mainStack->setProperty("parent", "true");
    _mainStack->setStyleSheet([]() {
        QFile styleSheet(":/res/qss/MainStack.qss");
        styleSheet.open(QFile::ReadOnly);
        return styleSheet.readAll();
    }());
    _centralWidgetLayout->addWidget(_mainStack, 0, 1, 2, 1);

    // home tab
    _homeTab = new HomeTab(_mainStack);
    _mainStack->addWidget(_homeTab);

    // s-n curve tab
    _snCurveTab = new SnCurveTab(_mainStack);
    _mainStack->addWidget(_snCurveTab);

    // connections
    QObject::connect(_homeTab, &HomeTab::editDetailRequestReceived, this, &MainWindow::onEditDetailRequestReceived);
    QObject::connect(_snNavBar, &QListWidget::currentRowChanged, this, &MainWindow::onSnNavBarCurrentRowChanged);
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
            _sideStack->setVisible(false);
            _mainNavBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
            _mainNavBar->setCurrentRow(0);
            _snDetailItem->setHidden(true);
            _mainStack->setCurrentWidget(_homeTab);
            _homeTab->setProject(_project);
            break;
        case Module::SnDetail:
            if (_detail == nullptr) break;
            _homeTab->setProject(nullptr);
            _snDetailItem->setHidden(false);
            _sideStack->setVisible(true);
            _mainNavBar->setCurrentItem(_snDetailItem);
            _mainNavBar->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
            _snNavBar->setCurrentRow(0);
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
        default: throw std::logic_error("Case not implemented.");
    }

}

void MainWindow::onSnNavBarCurrentRowChanged() {

    // clear previous
    _snCurveTab->setDetail(nullptr);
    if (_detail == nullptr) return;

    // show new
    if (_snNavBar->currentItem() == _snCurveItem) {
        _mainStack->setCurrentWidget(_snCurveTab);
        _snCurveTab->setDetail(dynamic_cast<SnDetail*>(_detail));
    }

}
