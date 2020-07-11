#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BaGuaStock.h"
#include "Gua.h"

#include <vector>

class BaGuaStock : public QMainWindow
{
    Q_OBJECT

public:
    BaGuaStock(QWidget *parent = Q_NULLPTR);

    void resizeEvent(QResizeEvent *event) override;

private slots:
    void openFile();
    void openFile2();
    void saveFile();

private:
    struct DetailStats
    {
        QString m_name;
        int m_output = 0;
        int m_filtered = 0;
    };

    struct StatsInfo
    {
        int m_total = 0;
        int m_output = 0;
        int m_key_filter = 0;
        int m_concept_filter = 0;
        int m_exchange_filter = 0;
        int m_other = 0;

        std::map<QString, DetailStats> m_concept_stats;

        void Reset()
        {
            m_total = 0;
            m_output = 0;
            m_key_filter = 0;
            m_concept_filter = 0;
            m_exchange_filter = 0;
            m_other = 0;

            m_concept_stats.clear();
        }
    };

    void LoadAllData();
    void ApplyFilter();
    void UpdateStats();

    void LoadGuaXiang();
    void LoadGua();
    int FindGua(bool isTop, Gua gua);
    
    bool LoadKeyFilterFileData();
    bool LoadConceptClassFileData();

    bool LoadStockFileData(QString filePath);
    bool ParseHeaders(QString firtLine);

    bool LoadStockFileData2(QString filePath, QString filePath2);
    bool ParseHeaders2(QString firtLine);
    void ApplyFilter2();
    
    void ClearAll();
    void ClearStats(bool keep_headers);
    void ClearStockData();

    Ui::BaGuaStockClass ui;

    StatsInfo m_stats_info;

    int m_codeCol = -1;
    int m_nameCol = -1;
    int m_percentCol = -1;
    int m_todayCol = -1;
    int m_yesterdayCol = -1;
    int m_domainCol = -1;

    std::vector<QString> m_stock_data;
    std::map<QString, QString> m_stock_data2;
    std::vector<int> m_output_index_list;

    // No change after app launch
    std::map<QString, std::vector<QString>> m_concept_stock_list;
    std::map<QString, std::vector<QString>> m_stock_concept_list;
    std::vector<std::vector<int>> m_keys;
    std::vector<std::vector<QString>> m_gua_xiang;
    std::vector<Gua> m_top_gua;
    std::vector<Gua> m_down_gua;
};
