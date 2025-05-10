#include <stdexcept>
#include "Model/SnDetail.hpp"
#include "Model/LefmDetail.hpp"
#include "Model/Project.hpp"

Project::Project(QObject* parent) : QObject(parent) {
    auto user = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).split('/').last();
    auto timeStamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    _isUnsaved = true;
    _name = "Unnamed-Project";
    _description = QString("Project created by %1 on %2.").arg(user).arg(timeStamp);
}

bool Project::isUnsaved() const { return _isUnsaved; }

const QString& Project::name() const { return _name; }

void Project::setName(const QString& name) {
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

const QString& Project::description() const { return _description; }

void Project::setDescription(const QString& description) {
    if (description == _description) return;
    _description = description.trimmed();
    emit modified();
}

int Project::detailCount() const { return static_cast<int>(_details.size()); }

Detail& Project::detail(int i) { return *_details.at(i); }

const Detail& Project::detail(int i) const { return *_details.at(i); }

Detail& Project::createDetail(Detail::Approach approach) {
    static int counter = 0;
    switch (approach) {
        case Detail::Approach::Sn: _details.emplace_back(new SnDetail(this)); break;
        case Detail::Approach::Lefm: _details.emplace_back(new LefmDetail(this)); break;
        default: throw std::logic_error("Case not implemented.");
    }
    _details.back()->setName(QString("%1-%2").arg(_details.back()->name()).arg(++counter));
    QObject::connect(_details.back().get(), &Detail::modified, this, &Project::onDetailModified);
    emit modified();
    return *_details.back();
}

void Project::destroyDetail(int index) {
    QObject::disconnect(_details.at(index).get(), &Detail::modified, this, &Project::onDetailModified);
    _details.erase(_details.begin() + index);
    emit modified();
}

void Project::onDetailModified() { emit modified(); }
