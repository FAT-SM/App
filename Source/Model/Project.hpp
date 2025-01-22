#pragma once

#include <QtCore>
#include <memory>
#include <vector>
#include "Model/Detail.hpp"

/* Represents a project. */
class Project : public QObject {
    Q_OBJECT

private:
    bool _isUnsaved;
    QString _name;
    QString _description;
    std::vector<std::unique_ptr<Detail>> _details;

public:

    /* Project constructor. */
    explicit Project(QObject* parent = nullptr);

    /* Flags if the project has unsaved changes. */
    bool isUnsaved() const;

    /* The name of the project. */
    const QString& name() const;

    /* Sets the name of the project. */
    void setName(const QString& name);

    /* The description of the project. */
    const QString& description() const;

    /* Sets the description of the project. */
    void setDescription(const QString& description);

    /* The number of constructional details. */
    int detailCount() const;

    /* The i-th constructional detail. */
    Detail& detail(int i);
    const Detail& detail(int i) const;

    /* Creates a new detail. */
    Detail& createDetail(Detail::Approach approach);

    /* Destroys the i-th detail. */
    void destroyDetail(int index);

    /* On: detail modified. */
    void onDetailModified();

signals:

    /* Signal emitted when the project is modified. */
    void modified();

};
