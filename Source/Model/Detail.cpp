#include <stdexcept>
#include "Model/Detail.hpp"

Detail::Detail(QObject* parent) : QObject(parent) {
    auto user = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).split('/').last();
    auto timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    _name = "Unnamed-Detail";
    _description = QString("Detail created by %1 on %2.").arg(user).arg(timeStamp);
}

const QString& Detail::name() const { return _name; }

void Detail::setName(const QString& name) {
    if (name == _name) return;

    // simplify string
    QString result = name.simplified();

    // check for empty string
    if (result.isEmpty()) throw std::invalid_argument("Name is empty.");

    // check for invalid characters
    const static QString allowed = "-_ ";
    for (auto ch : result) if (!ch.isLetterOrNumber() && !allowed.contains(ch)) throw std::invalid_argument(
        QString("Name contains invalid character: '%1'.").arg(ch).toStdString());

    // success
    _name = result;
    emit modified();

}

const QString& Detail::description() const { return _description; }

void Detail::setDescription(const QString& description) {
    if (description == _description) return;
    _description = description.simplified();
    emit modified();
}
