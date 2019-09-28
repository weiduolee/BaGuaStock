#pragma once

#include <QDialog>
#include "ui_FilterDialog.h"

#include "FilterStruct.h"

class FilterDialog : public QDialog
{
    Q_OBJECT

public:
    FilterDialog(QWidget *parent = Q_NULLPTR);
    ~FilterDialog();

    void SetFilterStruct(FilterStruct filter);
    FilterStruct GetFilterStruct();

private:
    Ui::FilterDialog ui;
};
