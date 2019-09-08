#pragma once

#include <vector>

// --- => 1
// - - => 0

class Gua
{
public:
    Gua(int r1, int r2, int r3)
    {
        m_rows.push_back(r1);
        m_rows.push_back(r2);
        m_rows.push_back(r3);
    }

    std::vector<int> Rows()
    {
        return m_rows;
    }

    bool operator==(Gua gua)
    {
        if (m_rows.size() == gua.m_rows.size())
        {
            for (int i = 0; i < m_rows.size(); i++)
            {
                if (m_rows[i] != gua.m_rows[i])
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

private:
    std::vector<int> m_rows;
};