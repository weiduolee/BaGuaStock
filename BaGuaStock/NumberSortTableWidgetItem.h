#pragma once

#include <QTableWidgetItem>

class NumberSortTableWidgetItem : public QTableWidgetItem
{
public:
    explicit NumberSortTableWidgetItem(const QString &text, int type = Type)
        : QTableWidgetItem(text, type)
    {

    }

    bool operator <(const QTableWidgetItem &other) const
    {
        return text().toDouble() < other.text().toDouble();
    }
};