#pragma once

#include <QtCore>

/* Represents a constructional detail. */
class Detail : public QObject {
    Q_OBJECT

public:

    /* Available detail approaches. */
    enum class Approach { Sn, Lefm };

private:
    QString _name;
    QString _description;

public:

    /* Detail constructor. */
    explicit Detail(QObject* parent = nullptr);

    /* The name of the detail. */
    const QString& name() const;

    /* Sets the name of the detail. */
    void setName(const QString& name);

    /* The description of the detail. */
    const QString& description() const;

    /* Sets the description of the detail. */
    void setDescription(const QString& description);

    /* The selected approach for the detail analysis. */
    virtual Approach approach() const = 0;

signals:

    /* Signal emitted when the detail is modified. */
    void modified();

};
