#include "BaGuaStock.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include "FilterDialog.h"

BaGuaStock::BaGuaStock(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButtonOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui.pushButtonSave, SIGNAL(clicked()), this, SLOT(saveFile()));
    connect(ui.pushButtonFilter, SIGNAL(clicked()), this, SLOT(setFilter()));
    connect(ui.tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabSelected()));

    LoadAllData();
    ClearAll();
}

void BaGuaStock::resizeEvent(QResizeEvent *event)
{
    int columnCount = ui.tableWidgetOutput->columnCount();
    for(int c = 0; c < columnCount; c++)
    {
        ui.tableWidgetOutput->setColumnWidth(c, ui.tableWidgetOutput->width() / columnCount);
    }

    columnCount = ui.tableWidgetConcept->columnCount();
    for (int c = 0; c < columnCount; c++)
    {
        ui.tableWidgetConcept->setColumnWidth(c, ui.tableWidgetConcept->width() / columnCount);
    }

    columnCount = ui.tableWidgetDomain->columnCount();
    for (int c = 0; c < columnCount; c++)
    {
        ui.tableWidgetDomain->setColumnWidth(c, ui.tableWidgetDomain->width() / columnCount);
    }

    QMainWindow::resizeEvent(event);
}

void BaGuaStock::openFile()
{
    QString filePath = QFileDialog::getOpenFileName(this, 
        QString::fromLocal8Bit("打开"), ".", QString::fromLocal8Bit("通达信导出数据文件(*.txt)"));

    if (filePath.isEmpty())
    {
        return;
    }
    else
    {
        ClearAll();

        LoadStockFileData(filePath);
        
        ApplyFilter();
        
        UpdateStats();
        
        QMessageBox::information(this, QString::fromLocal8Bit("数据处理完成"),
                                 QString::fromLocal8Bit("数据处理完成！"));
    }
}

void BaGuaStock::saveFile()
{
    QString filePath = QFileDialog::getSaveFileName(this,
                                                    QString::fromLocal8Bit("保存"), ".", QString::fromLocal8Bit("文本文件(*.txt)"));

    if (filePath.isEmpty())
    {
        return;
    }
    else
    {
        QFile file(filePath);
        if (file.open(QIODevice::ReadWrite))
        {
            QTextStream stream(&file);

            stream << m_stock_data[0] << endl;

            for (int index : m_output_index_list)
            {
                if(index >= 0 && index < m_stock_data.size())
                stream << m_stock_data[index] << endl;
            }
        }
        else
        {
            QMessageBox::critical(this, QString::fromLocal8Bit("保存文件失败"),
                                  QString::fromLocal8Bit("保存") + filePath + QString::fromLocal8Bit("文件失败！"));
            return;
        }
    }
}

void BaGuaStock::setFilter()
{
    FilterDialog dialog(this);
    dialog.SetFilterStruct(m_filter);
    if (dialog.exec() == QDialog::Accepted)
    {
        if (m_filter != dialog.GetFilterStruct())
        {
            m_filter = dialog.GetFilterStruct();

            ClearStats(true);
            ApplyFilter();
            UpdateStats();
        }
    }
}

void BaGuaStock::tabSelected()
{
    int columnCount = ui.tableWidgetOutput->columnCount();
    columnCount = ui.tableWidgetConcept->columnCount();
    for (int c = 0; c < columnCount; c++)
    {
        ui.tableWidgetConcept->setColumnWidth(c, ui.tableWidgetConcept->width() / columnCount);
    }

    columnCount = ui.tableWidgetDomain->columnCount();
    for (int c = 0; c < columnCount; c++)
    {
        ui.tableWidgetDomain->setColumnWidth(c, ui.tableWidgetDomain->width() / columnCount);
    }
}

void BaGuaStock::LoadAllData()
{
    LoadKeyFilterFileData();
    LoadGua();
    LoadGuaXiang();
    LoadConceptClassFileData();
    LoadConceptFilterFileData();
    LoadDomainFilterFileData();
}

bool BaGuaStock::LoadKeyFilterFileData()
{
    QString keyFilPath = QApplication::applicationDirPath();
    keyFilPath.append("/key.txt");
    QFile file(keyFilPath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("打开key.txt文件失败"),
                              QString::fromLocal8Bit("打开key.txt文件失败！"));
        return false;
    }

    int row = 0;
    QTextStream stream(&file);
    QString line = stream.readLine();
    while (!line.isEmpty() && row < 8)
    {
        std::vector<int> key;
        QStringList cols = line.split(" ", QString::SkipEmptyParts);
        for (int c = 0; c < 8; c++)
        {
            key.push_back(cols.value(c).toInt());
        }

        m_keys.push_back(key);
        row++;
        line = stream.readLine();
    }

    return true;
}

void BaGuaStock::LoadGuaXiang()
{
    std::vector<QString> r1;
    r1.push_back(QString::fromLocal8Bit("乾为天"));
    r1.push_back(QString::fromLocal8Bit("泽天夬"));
    r1.push_back(QString::fromLocal8Bit("火天大有"));
    r1.push_back(QString::fromLocal8Bit("雷天大壮"));
    r1.push_back(QString::fromLocal8Bit("风天小畜"));
    r1.push_back(QString::fromLocal8Bit("水天需"));
    r1.push_back(QString::fromLocal8Bit("山天大畜"));
    r1.push_back(QString::fromLocal8Bit("地天泰"));
    m_gua_xiang.push_back(r1);

    std::vector<QString> r2;
    r2.push_back(QString::fromLocal8Bit("天泽履"));
    r2.push_back(QString::fromLocal8Bit("兑为泽"));
    r2.push_back(QString::fromLocal8Bit("火泽睽"));
    r2.push_back(QString::fromLocal8Bit("雷泽归妹"));
    r2.push_back(QString::fromLocal8Bit("风泽中孚"));
    r2.push_back(QString::fromLocal8Bit("水泽节"));
    r2.push_back(QString::fromLocal8Bit("山泽损"));
    r2.push_back(QString::fromLocal8Bit("地泽临"));
    m_gua_xiang.push_back(r2);

    std::vector<QString> r3;
    r3.push_back(QString::fromLocal8Bit("天火同人"));
    r3.push_back(QString::fromLocal8Bit("泽火革"));
    r3.push_back(QString::fromLocal8Bit("离为火"));
    r3.push_back(QString::fromLocal8Bit("雷火丰"));
    r3.push_back(QString::fromLocal8Bit("风火家人"));
    r3.push_back(QString::fromLocal8Bit("水火既济"));
    r3.push_back(QString::fromLocal8Bit("山火贲"));
    r3.push_back(QString::fromLocal8Bit("地火明夷"));
    m_gua_xiang.push_back(r3);

    std::vector<QString> r4;
    r4.push_back(QString::fromLocal8Bit("天雷无妄"));
    r4.push_back(QString::fromLocal8Bit("泽雷随"));
    r4.push_back(QString::fromLocal8Bit("火雷噬磕"));
    r4.push_back(QString::fromLocal8Bit("震为雷"));
    r4.push_back(QString::fromLocal8Bit("风雷益"));
    r4.push_back(QString::fromLocal8Bit("水雷屯"));
    r4.push_back(QString::fromLocal8Bit("山雷颐"));
    r4.push_back(QString::fromLocal8Bit("地雷复"));
    m_gua_xiang.push_back(r4);

    std::vector<QString> r5;
    r5.push_back(QString::fromLocal8Bit("天风垢"));
    r5.push_back(QString::fromLocal8Bit("泽风大过"));
    r5.push_back(QString::fromLocal8Bit("火风鼎"));
    r5.push_back(QString::fromLocal8Bit("雷风恒"));
    r5.push_back(QString::fromLocal8Bit("巽为风"));
    r5.push_back(QString::fromLocal8Bit("水风井"));
    r5.push_back(QString::fromLocal8Bit("山风蛊"));
    r5.push_back(QString::fromLocal8Bit("地风升"));
    m_gua_xiang.push_back(r5);

    std::vector<QString> r6;
    r6.push_back(QString::fromLocal8Bit("天水讼"));
    r6.push_back(QString::fromLocal8Bit("泽水困"));
    r6.push_back(QString::fromLocal8Bit("火水未济"));
    r6.push_back(QString::fromLocal8Bit("雷水解"));
    r6.push_back(QString::fromLocal8Bit("风水涣"));
    r6.push_back(QString::fromLocal8Bit("坎为水"));
    r6.push_back(QString::fromLocal8Bit("山水蒙"));
    r6.push_back(QString::fromLocal8Bit("地水师"));
    m_gua_xiang.push_back(r6);

    std::vector<QString> r7;
    r7.push_back(QString::fromLocal8Bit("天山遁"));
    r7.push_back(QString::fromLocal8Bit("泽山咸"));
    r7.push_back(QString::fromLocal8Bit("火山旅"));
    r7.push_back(QString::fromLocal8Bit("雷山小过"));
    r7.push_back(QString::fromLocal8Bit("风山渐"));
    r7.push_back(QString::fromLocal8Bit("水山蹇"));
    r7.push_back(QString::fromLocal8Bit("艮为山"));
    r7.push_back(QString::fromLocal8Bit("地山谦"));
    m_gua_xiang.push_back(r7);

    std::vector<QString> r8;
    r8.push_back(QString::fromLocal8Bit("天地否"));
    r8.push_back(QString::fromLocal8Bit("泽地萃"));
    r8.push_back(QString::fromLocal8Bit("火地晋"));
    r8.push_back(QString::fromLocal8Bit("雷地豫"));
    r8.push_back(QString::fromLocal8Bit("风地观"));
    r8.push_back(QString::fromLocal8Bit("水地比"));
    r8.push_back(QString::fromLocal8Bit("山地剥"));
    r8.push_back(QString::fromLocal8Bit("坤为地"));
    m_gua_xiang.push_back(r8);
}

void BaGuaStock::LoadGua()
{
    m_top_gua.push_back(Gua(1, 1, 1));
    m_top_gua.push_back(Gua(0, 1, 1));
    m_top_gua.push_back(Gua(1, 0, 1));
    m_top_gua.push_back(Gua(0, 0, 1));
    m_top_gua.push_back(Gua(1, 1, 0));
    m_top_gua.push_back(Gua(0, 1, 0));
    m_top_gua.push_back(Gua(1, 0, 0));
    m_top_gua.push_back(Gua(0, 0, 0));

    m_down_gua.push_back(Gua(1, 1, 1));
    m_down_gua.push_back(Gua(0, 1, 1));
    m_down_gua.push_back(Gua(1, 0, 1));
    m_down_gua.push_back(Gua(0, 0, 1));
    m_down_gua.push_back(Gua(1, 1, 0));
    m_down_gua.push_back(Gua(0, 1, 0));
    m_down_gua.push_back(Gua(1, 0, 0));
    m_down_gua.push_back(Gua(0, 0, 0));
}

int BaGuaStock::FindGua(bool isTop, Gua gua)
{
    if (isTop)
    {
        for (int i = 0; i < m_top_gua.size(); i++)
        {
            if (m_top_gua[i] == gua)
            {
                return i;
            }
        }
    }
    else
    {
        for (int i = 0; i < m_down_gua.size(); i++)
        {
            if (m_down_gua[i] == gua)
            {
                return i;
            }
        }
    }

    return -1;
}

bool BaGuaStock::LoadConceptClassFileData()
{
    QString filterFilPath = QApplication::applicationDirPath();
    filterFilPath.append("/concept_class.csv");
    QFile file(filterFilPath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("打开concept_class.csv文件失败"),
                              QString::fromLocal8Bit("打开concept_class.csv文件失败！"));
        return false;
    }

    QTextStream stream(&file);
    stream.setCodec("UTF-8");
    QString line = stream.readLine();
    line = line.trimmed();
    while (!line.isEmpty())
    {
        QStringList data = line.split(",", QString::SkipEmptyParts);

        if (data.count() == 8)
        {
            QString code = data.value(2);
            code = code.left(6);
            QRegExp re("^[0-9]{1,6}$"); 
            if (re.exactMatch(code))
            {
                QString concept = data.value(6);
                if (code.isEmpty() || concept.isEmpty())
                    continue;

                m_concept_stock_list[concept].push_back(code);
                m_stock_concept_list[code].push_back(concept);
            }
        }

        line = stream.readLine();
        line = line.trimmed();
    }

    return true;
}

bool BaGuaStock::LoadConceptFilterFileData()
{
    QString filterFilPath = QApplication::applicationDirPath();
    filterFilPath.append("/concept_filter.txt");
    QFile file(filterFilPath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("打开concept_filter.txt文件失败"),
                              QString::fromLocal8Bit("打开concept_filter.txt文件失败！"));
        return false;
    }

    QTextStream stream(&file);
    QString line = stream.readLine();
    line = line.trimmed();
    while (!line.isEmpty())
    {
        m_concept_filter_list.push_back(line);

        line = stream.readLine();
        line = line.trimmed();
    }

    return true;
}

bool BaGuaStock::LoadDomainFilterFileData()
{
    QString filterFilPath = QApplication::applicationDirPath();
    filterFilPath.append("/domain_filter.txt");
    QFile file(filterFilPath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("打开domain_filter.txt文件失败"),
                              QString::fromLocal8Bit("打开domain_filter.txt文件失败！"));
        return false;
    }

    QTextStream stream(&file);
    QString line = stream.readLine();
    line = line.trimmed();
    while (!line.isEmpty())
    {
        m_domain_filter_list.push_back(line);

        line = stream.readLine();
        line = line.trimmed();
    }

    return true;
}

bool BaGuaStock::LoadStockFileData(QString filePath)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("加载数据失败"),
                              QString::fromLocal8Bit("打开文件失败！"));
        return false;
    }

    setWindowTitle(QApplication::applicationDisplayName() + " - " + filePath);

    QTextStream stream(&file);
    QString line = stream.readLine();

    if (!ParseHeaders(line))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("加载数据失败"),
                              QString::fromLocal8Bit("文件中没有符合的列名！"));
        return false;
    }

    m_stock_data.push_back(line);

    line = stream.readLine();
    while (!line.isEmpty())
    {
        m_stats_info.m_total++;

        m_stock_data.push_back(line);

        line = stream.readLine();
    }

    return true;
}

void BaGuaStock::UpdateStats()
{
    ui.labelTotal->setText(QString::number(m_stats_info.m_total));
    ui.labelOutput->setText(QString::number(m_stats_info.m_output));
    ui.labelKeyFilter->setText(QString::number(m_stats_info.m_key_filter));
    ui.labelConceptFilter->setText(QString::number(m_stats_info.m_concept_filter));
    ui.labelDomainFilter->setText(QString::number(m_stats_info.m_domain_filter));
    ui.labelCreativeFilter->setText(QString::number(m_stats_info.m_creative_filter));
    ui.labelStFilter->setText(QString::number(m_stats_info.m_st_filter));
    ui.labelExchangeFilter->setText(QString::number(m_stats_info.m_exchange_filter));
    ui.labelOther->setText(QString::number(m_stats_info.m_other));

    for (const auto& pair : m_stats_info.m_concept_stats)
    {
        ui.tableWidgetConcept->insertRow(ui.tableWidgetConcept->rowCount());

        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      0, new QTableWidgetItem(pair.first));

        int total = pair.second.m_output + pair.second.m_filtered;
        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      1, new QTableWidgetItem(QString::number(total)));

        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      2, new QTableWidgetItem(QString::number(pair.second.m_output)));

        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      3, new QTableWidgetItem(QString::number(pair.second.m_filtered)));

        float percent = total > 0 ? ((float)pair.second.m_output / (float)total) : 0;
        percent *= 100;
        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      4, new QTableWidgetItem(QString().setNum(percent, 'f', 2)));
    }

    for (const auto& pair : m_stats_info.m_domain_stats)
    {
        ui.tableWidgetDomain->insertRow(ui.tableWidgetDomain->rowCount());

        ui.tableWidgetDomain->setItem(ui.tableWidgetDomain->rowCount() - 1,
                                0, new QTableWidgetItem(pair.first));

        int total = pair.second.m_output + pair.second.m_filtered;
        ui.tableWidgetDomain->setItem(ui.tableWidgetDomain->rowCount() - 1,
                                1, new QTableWidgetItem(QString::number(total)));

        ui.tableWidgetDomain->setItem(ui.tableWidgetDomain->rowCount() - 1,
                                2, new QTableWidgetItem(QString::number(pair.second.m_output)));

        ui.tableWidgetDomain->setItem(ui.tableWidgetDomain->rowCount() - 1,
                                3, new QTableWidgetItem(QString::number(pair.second.m_filtered)));

        float percent = total > 0 ? ((float)pair.second.m_output/ (float)total) : 0;
        percent *= 100;
        ui.tableWidgetDomain->setItem(ui.tableWidgetDomain->rowCount() - 1,
                                4, new QTableWidgetItem(QString().setNum(percent, 'f', 2)));
    }

    if (m_output_index_list.size() <= 1)
    {
        ui.pushButtonSave->setDisabled(true);
    }
    else
    {
        ui.pushButtonSave->setDisabled(false);
    }
}

bool BaGuaStock::ParseHeaders(QString firtLine)
{
    // 代码 名称 涨幅 现价 昨收 细分行业
    QStringList headers = firtLine.split("\t", QString::SkipEmptyParts);
    for (int c = 0; c < headers.count(); c++)
    {
        if (headers.value(c) == QString::fromLocal8Bit("代码"))
            m_codeCol = c;
        else if (headers.value(c) == QString::fromLocal8Bit("名称"))
            m_nameCol = c;
        else if (headers.value(c).startsWith(QString::fromLocal8Bit("涨幅")))
            m_percentCol = c;
        else if (headers.value(c) == QString::fromLocal8Bit("现价"))
            m_todayCol = c;
        else if (headers.value(c) == QString::fromLocal8Bit("昨收"))
            m_yesterdayCol = c;
        else if (headers.value(c) == QString::fromLocal8Bit("细分行业"))
            m_domainCol = c;
    }

    if (m_codeCol < 0 || m_nameCol < 0 || m_percentCol < 0 || m_todayCol < 0 
        || m_yesterdayCol < 0 || m_domainCol < 0)
    {
        return false;
    }

    ui.tableWidgetOutput->insertColumn(0);
    ui.tableWidgetOutput->insertColumn(1);
    ui.tableWidgetOutput->insertColumn(2);
    ui.tableWidgetOutput->insertColumn(3);
    ui.tableWidgetOutput->insertColumn(4);
    ui.tableWidgetOutput->insertColumn(5);
    ui.tableWidgetOutput->insertColumn(6);
    ui.tableWidgetOutput->insertColumn(7);

    QStringList usefulheaders;
    usefulheaders << QString::fromLocal8Bit("卦象") 
        << QString::fromLocal8Bit("代码") << QString::fromLocal8Bit("名称")
        << QString::fromLocal8Bit("涨幅%") << QString::fromLocal8Bit("现价")
        << QString::fromLocal8Bit("昨收") << QString::fromLocal8Bit("细分行业")
        << QString::fromLocal8Bit("概念");
    ui.tableWidgetOutput->setHorizontalHeaderLabels(usefulheaders);

    int columnCount = ui.tableWidgetOutput->columnCount();
    for (int c = 0; c < columnCount; c++)
    {
        ui.tableWidgetOutput->setColumnWidth(c, ui.tableWidgetOutput->width() / columnCount);
    }

    ////////////////////////

    ui.tableWidgetConcept->insertColumn(0);
    ui.tableWidgetConcept->insertColumn(1);
    ui.tableWidgetConcept->insertColumn(2);
    ui.tableWidgetConcept->insertColumn(3);
    ui.tableWidgetConcept->insertColumn(4);

    QStringList concept_headers;
    concept_headers << QString::fromLocal8Bit("概念")
        << QString::fromLocal8Bit("总共") << QString::fromLocal8Bit("输出")
        << QString::fromLocal8Bit("不输出") << QString::fromLocal8Bit("输出占比%");
    ui.tableWidgetConcept->setHorizontalHeaderLabels(concept_headers);

    columnCount = ui.tableWidgetConcept->columnCount();
    for (int c = 0; c < columnCount; c++)
    {
        ui.tableWidgetConcept->setColumnWidth(c, ui.tableWidgetConcept->width() / columnCount);
    }

    ////////////////////////

    ui.tableWidgetDomain->insertColumn(0);
    ui.tableWidgetDomain->insertColumn(1);
    ui.tableWidgetDomain->insertColumn(2);
    ui.tableWidgetDomain->insertColumn(3);
    ui.tableWidgetDomain->insertColumn(4);

    QStringList domainheaders;
    domainheaders << QString::fromLocal8Bit("细分行业")
        << QString::fromLocal8Bit("总共") << QString::fromLocal8Bit("输出")
        << QString::fromLocal8Bit("不输出") << QString::fromLocal8Bit("输出占比%");
    ui.tableWidgetDomain->setHorizontalHeaderLabels(domainheaders);

    columnCount = ui.tableWidgetDomain->columnCount();
    for (int c = 0; c < columnCount; c++)
    {
        ui.tableWidgetDomain->setColumnWidth(c, ui.tableWidgetDomain->width() / columnCount);
    }


    return true;
}

void BaGuaStock::ApplyFilter()
{
    int line_index = 0;
    for (const QString& line : m_stock_data)
    {
        line_index++;

        QStringList data = line.split("\t", QString::SkipEmptyParts);

        if (data.count() > 6)
        {
            QString code = data.value(m_codeCol);
            if (m_filter.m_creative_filter && code.startsWith("688", Qt::CaseInsensitive))
            {
                m_stats_info.m_creative_filter++;
                continue;
            }

            QString name = data.value(m_nameCol);
            if (m_filter.m_st_filter && name.contains("ST", Qt::CaseInsensitive))
            {
                m_stats_info.m_st_filter++;
                continue;
            }

            QString today = data.value(m_todayCol);
            QString yestoday = data.value(m_yesterdayCol);

            bool ok = false;
            double todayValue = today.toDouble(&ok);
            if (!ok)
            {
                m_stats_info.m_exchange_filter++;
                continue;
            }
            double yesterdayValue = yestoday.toDouble(&ok);
            if (!ok)
            {
                m_stats_info.m_exchange_filter++;
                continue;
            }

            QString domain = data.value(m_domainCol);
            if (std::find(m_domain_filter_list.begin(), m_domain_filter_list.end(), domain)
                != m_domain_filter_list.end())
            {
                m_stats_info.m_domain_filter++;
                continue;
            }

            std::vector<QString> concepts;
            if (m_stock_concept_list.count(code) > 0)
            {
                concepts = m_stock_concept_list[code];
                bool all_hits = true;
                for (const QString concept : concepts)
                {
                    if (std::find(m_concept_filter_list.begin(), m_concept_filter_list.end(), concept)
                        == m_concept_filter_list.end())
                    {
                        all_hits = false;
                        break;
                    }
                }

                if (all_hits)
                {
                    m_stats_info.m_concept_filter++;
                    continue;
                }
            }

            int sumToday = 0;
            for (int i = 0; i < today.length(); i++)
            {
                if (today[i] == '.')
                    continue;

                int value = QString(today[i]).toInt();
                sumToday += value;
            }

            int sumYesterday = 0;
            for (int i = 0; i < yestoday.length(); i++)
            {
                if (yestoday[i] == '.')
                    continue;

                int value = QString(yestoday[i]).toInt();
                sumYesterday += value;
            }

            int top = sumYesterday % 8;
            if (top == 0)
                top = 8;

            int down = sumToday % 8;
            if (down == 0)
                down = 8;

            Gua topGua = m_top_gua[top - 1];
            Gua downGua = m_down_gua[down - 1];

            int var = (sumYesterday + sumToday) % 6;
            if (var == 0)
                var = 6;

            if (var > 0 && var < 4)
            {
                downGua.SetRow((3 - var), abs(downGua.Rows()[3 - var] - 1));
            }
            else
            {
                topGua.SetRow((6 - var), abs(topGua.Rows()[6 - var] - 1));
            }

            int keyRow = FindGua(false, downGua);
            int keyCol = FindGua(true, topGua);

            if (keyRow < m_keys.size())
            {
                int key = m_keys[keyRow][keyCol];
                if (key == 1)
                {
                    ui.tableWidgetOutput->insertRow(ui.tableWidgetOutput->rowCount());

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            0, new QTableWidgetItem(m_gua_xiang[keyRow][keyCol]));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            1, new QTableWidgetItem(data.value(m_codeCol)));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            2, new QTableWidgetItem(data.value(m_nameCol)));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            3, new QTableWidgetItem(data.value(m_percentCol)));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            4, new QTableWidgetItem(data.value(m_todayCol)));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            5, new QTableWidgetItem(data.value(m_yesterdayCol)));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            6, new QTableWidgetItem(data.value(m_domainCol)));

                    QStringList all_concepts(QVector<QString>::fromStdVector(concepts).toList());
                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            7, new QTableWidgetItem(all_concepts.join(',')));

                    m_stats_info.m_output++;
                    m_stats_info.m_domain_stats[domain].m_output++;
                    for (const QString concept : concepts)
                    {
                        m_stats_info.m_concept_stats[concept].m_output++;
                    }

                    m_output_index_list.push_back(line_index - 1);
                    continue;
                }
                else
                {
                    m_stats_info.m_key_filter++;
                    m_stats_info.m_domain_stats[domain].m_filtered++;
                    for (const QString concept : concepts)
                    {
                        m_stats_info.m_concept_stats[concept].m_filtered++;
                    }

                    continue;
                }
            }
            else
            {
                m_stats_info.m_other++;
                continue;
            }
        }
        else
        {
            m_stats_info.m_other++;
            continue;
        }
    }
}

void BaGuaStock::ClearAll()
{
    ClearStats(false);
    ClearStockData();
}

void BaGuaStock::ClearStats(bool keep_headers)
{
    m_stats_info.Reset();
    UpdateStats();

    ui.tableWidgetConcept->setRowCount(0);
    if(!keep_headers)
        ui.tableWidgetConcept->setColumnCount(0);

    ui.tableWidgetDomain->setRowCount(0);
    if (!keep_headers)
        ui.tableWidgetDomain->setColumnCount(0);

    ui.tableWidgetOutput->setRowCount(0);
    if (!keep_headers)
        ui.tableWidgetOutput->setColumnCount(0);

    setWindowTitle(QApplication::applicationDisplayName());
}

void BaGuaStock::ClearStockData()
{
    m_codeCol = -1;
    m_nameCol = -1;
    m_percentCol = -1;
    m_todayCol = -1;
    m_yesterdayCol = -1;
    m_domainCol = -1;

    m_stock_data.clear();
    m_output_index_list.clear();
}