#include <QApplication>
#include "Gui.hpp"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);
    app.setStyle("Fusion");
    app.styleHints()->setColorScheme(Qt::ColorScheme::Light);

    MainWindow mainWin;
    mainWin.show();

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();
    QRect windowGeometry = mainWin.frameGeometry();
    int x = (screenGeometry.width() - windowGeometry.width()) / 2;
    int y = (screenGeometry.height() - windowGeometry.height()) / 2;
    mainWin.move(x, y);

    return app.exec();

}
