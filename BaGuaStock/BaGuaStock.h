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

private:
    void LoadGua();
    int FindGua(bool isTop, Gua gua);
    bool LoadKeyFileData();
    bool LoadFileData(QString filePath);
    void UpdateStats();
    void Clear();
    bool ParseHeaders(QString firtLine);
    bool ParseLine(QString line);
    
    Ui::BaGuaStockClass ui;

    int m_total = 0;
    int m_show = 0;
    int m_filter = 0;
    int m_ST = 0;
    int m_noBuy = 0;
    int m_other = 0;

    int m_codeCol = -1;
    int m_nameCol = -1;
    int m_percentCol = -1;
    int m_todayCol = -1;
    int m_yesterdayCol = -1;
    int m_domainCol = -1;

    std::vector<std::vector<int>> m_keys;
    std::vector<Gua> m_topGua;
    std::vector<Gua> m_downGua;
};
