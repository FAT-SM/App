#include <stdexcept>
#include "Gui/Gnuplot.hpp"

Gnuplot::Gnuplot(QObject* parent) : QObject(parent), _plotFile(), _commandBuffer() {
    bool success = _plotFile.open();
    if (!success) throw std::runtime_error("Could not open temporary plot file.");
}

void Gnuplot::append(const QString& command) { _commandBuffer.append(command.trimmed() + '\n'); }

void Gnuplot::figure(const QSize& size) {
    append(QString("set terminal png size %1, %2 font 'sans'").arg(size.width()).arg(size.height()));
    append(QString("set output '%1'").arg(_plotFile.fileName()));
}

QPixmap Gnuplot::plot() {

    // do nothing if buffer is empty
    if (_commandBuffer.isEmpty()) return QPixmap();

    // create process
    auto* process = new QProcess(this);
    process->setProgram("gnuplot");
    QObject::connect(process, &QProcess::finished, process, &QProcess::deleteLater);

    // attempt to start process
    process->start();
    if (!process->waitForStarted()) {
        QMessageBox::critical(qApp->activeWindow(), "Error",
R"(
This application requires <a href='http://www.gnuplot.info/'>gnuplot</a> to be installed and included in the system's PATH.
This plotting tool is freely available to download <a href='https://sourceforge.net/projects/gnuplot/files/gnuplot/'>here</a>.
<br/><br/>
If gnuplot is already installed, ensure it is in the system's PATH.
<br/><br/>
This application will now close.
)"
        );
        qApp->exit(-1);
    }

    // call gnuplot
    process->write(_commandBuffer.toUtf8());
    process->closeWriteChannel();
    process->waitForFinished();

    // clear command buffer and return plot
    _commandBuffer.clear();
    return QPixmap(_plotFile.fileName());

}
