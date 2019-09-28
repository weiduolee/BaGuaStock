#pragma once

struct FilterStruct
{
    bool m_concept_filter = true;
    bool m_domain_filter = true;
    bool m_creative_filter = true;
    bool m_st_filter = true;

    bool operator==(const FilterStruct& other)
    {
        return m_concept_filter == other.m_concept_filter
            && m_domain_filter == other.m_domain_filter
            && m_creative_filter == other.m_creative_filter
            && m_st_filter == other.m_st_filter;
    }
};