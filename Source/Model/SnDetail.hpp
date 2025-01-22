#pragma once

#include <QtCore>
#include <optional>
#include "Core.hpp"
#include "Model/Detail.hpp"

/* Represents a constructional S-N detail. */
class SnDetail : public Detail {
    Q_OBJECT

private:
    std::optional<double> _category;
    Matrix<double> _slopes;

public:

    /* S-N detail constructor. */
    explicit SnDetail(QObject* parent = nullptr);

    /* The selected approach for the detail analysis. */
    Approach approach() const override;

    /* The detail category. */
    std::optional<double> category() const;

    /* Sets the detail category. */
    void setCategory(double category);

    /* The S-N curve slopes. */
    const Matrix<double>& slopes() const;

    /* Sets the S-N curve slopes. */
    void setSlopes(const Matrix<double>& slopes);

};
