#pragma once

#include <QtCore>
#include <QtGui>
#include <QtWidgets>
#include "Model.hpp"
#include "Gui/HomeTab.hpp"
#include "Gui/SnCurveTab.hpp"
#include "Gui/HistoryTab.hpp"
#include "Gui/RainflowTab.hpp"
#include "Gui/DamageTab.hpp"
#include "Gui/DetailSelectionTab.hpp"
#include "Gui/ParisLawTab.hpp"
#include "Gui/RemainingLifeTab.hpp"

/* The main window of the application. */
class MainWindow : public QMainWindow {
    Q_OBJECT

public:

    /* Available modules. */
    enum class Module { Home, SnDetail, LefmDetail };

private:
    QStatusBar* _statusBar;
    QLabel* _statusLabel;
    QMenuBar* _menuBar;
    QMenu* _fileMenu;
    QWidget* _centralWidget;
    QGridLayout* _centralWidgetLayout;
    QListWidget* _mainNavBar;
    QListWidgetItem* _homeItem;
    QListWidgetItem* _snDetailItem;
    QListWidgetItem* _lefmDetailItem;
    QStackedWidget* _sideStack;
    QListWidget* _snNavBar;
    QListWidget* _lefmNavBar;
    QListWidgetItem* _snCurveItem;
    QListWidgetItem* _snHistoryItem;
    QListWidgetItem* _snRainflowItem;
    QListWidgetItem* _damageItem;
    QListWidgetItem* _detailSelectionItem;
    QListWidgetItem* _parisLawItem;
    QListWidgetItem* _lefmHistoryItem;
    QListWidgetItem* _lefmRainflowItem;
    QListWidgetItem* _remainingLifeItem;
    QStackedWidget* _mainStack;
    HomeTab* _homeTab;
    SnCurveTab* _snCurveTab;
    HistoryTab* _historyTab;
    RainflowTab* _rainflowTab;
    DamageTab* _damageTab;
    DetailSelectionTab* _detailSelectionTab;
    ParisLawTab* _parisLawTab;
    RemainingLifeTab* _remainingLifeTab;
    Module _module;
    Project* _project;
    Detail* _detail;

    /* Builds the GUI. */
    void buildGui();

    /* Refreshes the GUI. */
    void refreshGui();

public:

    /* Main window constructor. */
    explicit MainWindow(QWidget* parent = nullptr);

    /* Sets the current GUI module. */
    void setModule(Module module, bool force = false);

    /* On: edit detail request received. */
    void onEditDetailRequestReceived(int index);

    /* On: S-N nav bar current row changed. */
    void onSnNavBarCurrentRowChanged();

    /* On: LEFM nav bar current row changed. */
    void onLefmNavBarCurrentRowChanged();

};
