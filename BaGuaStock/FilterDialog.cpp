#include "FilterDialog.h"

FilterDialog::FilterDialog(QWidget *parent)
    : QDialog(parent)
{
    ui.setupUi(this);

    connect(ui.buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(ui.buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

FilterDialog::~FilterDialog()
{
}

void FilterDialog::SetFilterStruct(FilterStruct filter)
{
    ui.checkBoxConceptFilter->setChecked(filter.m_concept_filter);
    ui.checkBoxDomainFilter->setChecked(filter.m_domain_filter);
    ui.checkBoxCreativeFilter->setChecked(filter.m_creative_filter);
    ui.checkBoxStFilter->setChecked(filter.m_st_filter);
}

FilterStruct FilterDialog::GetFilterStruct()
{
    FilterStruct filter;
    filter.m_concept_filter = ui.checkBoxConceptFilter->isChecked();
    filter.m_domain_filter = ui.checkBoxDomainFilter->isChecked();
    filter.m_creative_filter = ui.checkBoxCreativeFilter->isChecked();
    filter.m_st_filter = ui.checkBoxStFilter->isChecked();
    return filter;
}