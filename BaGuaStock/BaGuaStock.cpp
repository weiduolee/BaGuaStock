#include "BaGuaStock.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

#include "NumberSortTableWidgetItem.h"

BaGuaStock::BaGuaStock(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButtonOpen, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui.pushButtonOpen2, SIGNAL(clicked()), this, SLOT(openFile2()));
    connect(ui.pushButtonSave, SIGNAL(clicked()), this, SLOT(saveFile()));

    LoadAllData();
    ClearAll();
}

void BaGuaStock::resizeEvent(QResizeEvent *event)
{
    int columnWidth = ui.tableWidgetConcept->width() / 10;
    ui.tableWidgetConcept->setColumnWidth(0, columnWidth * 4);
    ui.tableWidgetConcept->setColumnWidth(1, columnWidth * 1.5);
    ui.tableWidgetConcept->setColumnWidth(2, columnWidth * 1.25);
    ui.tableWidgetConcept->setColumnWidth(3, columnWidth * 1.25);
    ui.tableWidgetConcept->setColumnWidth(4, columnWidth * 1.5);

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
        
        QMessageBox::information(this, QString::fromLocal8Bit("当日数据处理完成"),
                                 QString::fromLocal8Bit("当日数据处理完成！"));
    }
}

void BaGuaStock::openFile2()
{
    QString filePath = QFileDialog::getOpenFileName(this,
        QString::fromLocal8Bit("打开第一日数据"), ".", QString::fromLocal8Bit("通达信导出数据文件(*.txt)"));

    if (filePath.isEmpty())
    {
        return;
    }
    else
    {
        QString filePath2 = QFileDialog::getOpenFileName(this,
            QString::fromLocal8Bit("打开第二日数据"), ".", QString::fromLocal8Bit("通达信导出数据文件(*.txt)"));

        if (filePath2.isEmpty())
            return;

        if (filePath == filePath2)
        {
            QMessageBox::information(this, QString::fromLocal8Bit("两日数据处理失败"),
                QString::fromLocal8Bit("两日数据文件是同一个文件！"));
            return;
        }

        ClearAll();

        LoadStockFileData2(filePath, filePath2);

        ApplyFilter2();

        if (m_output_index_list.size() < 1)
        {
            ui.pushButtonSave->setDisabled(true);
        }
        else
        {
            ui.pushButtonSave->setDisabled(false);
        }

        QMessageBox::information(this, QString::fromLocal8Bit("两日数据处理完成"),
            QString::fromLocal8Bit("两日数据处理完成！"));
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


void BaGuaStock::LoadAllData()
{
    LoadKeyFilterFileData();
    LoadGua();
    LoadGuaXiang();
    LoadConceptClassFileData();
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
    filterFilPath.append("/concept_class.txt");
    QFile file(filterFilPath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("打开concept_class.txt文件失败"),
                              QString::fromLocal8Bit("打开concept_class.txt文件失败！"));
        return false;
    }

    QTextStream stream(&file);
    //stream.setCodec("UTF-8");
    
    QString line = stream.readLine();
    line = line.trimmed();
    while (!line.isEmpty())
    {
        QStringList data = line.split(" ", QString::SkipEmptyParts);

        if (data.count() > 0)
        {
            QString code = data.value(0);
            code = code.left(6);
            QRegExp re("^[0-9]{1,6}$"); 
            if (re.exactMatch(code))
            {
                if (code.isEmpty())
                    continue;
             
                if (data.count() > 2)
                {
                    QString concepts = data.value(2);
                    QStringList concept_list = concepts.split(",", QString::SkipEmptyParts);
                    for (int i = 0; i < concept_list.count(); i++)
                    {
                        m_concept_stock_list[concept_list.value(i)].push_back(code);
                        m_stock_concept_list[code].push_back(concept_list.value(i));
                    }
                }
                else
                {
                    m_stock_concept_list[code];
                }
            }
        }

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
    ui.labelExchangeFilter->setText(QString::number(m_stats_info.m_exchange_filter));
    ui.labelOther->setText(QString::number(m_stats_info.m_other));

    for (const auto& pair : m_stats_info.m_concept_stats)
    {
        ui.tableWidgetConcept->insertRow(ui.tableWidgetConcept->rowCount());

        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      0, new QTableWidgetItem(pair.first));

        int total = pair.second.m_output + pair.second.m_filtered;
        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      1, new NumberSortTableWidgetItem(QString::number(total)));

        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      2, new NumberSortTableWidgetItem(QString::number(pair.second.m_output)));

        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      3, new NumberSortTableWidgetItem(QString::number(pair.second.m_filtered)));

        float percent = total > 0 ? ((float)pair.second.m_output / (float)total) : 0;
        percent *= 100;
        ui.tableWidgetConcept->setItem(ui.tableWidgetConcept->rowCount() - 1,
                                      4, new NumberSortTableWidgetItem(QString().setNum(percent, 'f', 2)));
    }

    if (m_output_index_list.size() < 1)
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

    /*if (m_codeCol < 0 || m_nameCol < 0 || m_percentCol < 0 || m_todayCol < 0 
        || m_yesterdayCol < 0 || m_domainCol < 0)*/
    if (m_codeCol < 0 || m_nameCol < 0 || m_todayCol < 0
        || m_yesterdayCol < 0)
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

    ui.tableWidgetOutput->horizontalHeader()->setStretchLastSection(true);

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

    int columnWidth = ui.tableWidgetConcept->width() / 10;
    ui.tableWidgetConcept->setColumnWidth(0, columnWidth * 4);
    ui.tableWidgetConcept->setColumnWidth(1, columnWidth * 1.5);
    ui.tableWidgetConcept->setColumnWidth(2, columnWidth * 1.25);
    ui.tableWidgetConcept->setColumnWidth(3, columnWidth * 1.25);
    ui.tableWidgetConcept->setColumnWidth(4, columnWidth * 1.5);

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
            QString name = data.value(m_nameCol);
            QString today = data.value(m_todayCol);
            QString yestoday = data.value(m_yesterdayCol);
            QString domain = data.value(m_domainCol);

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

            std::vector<QString> concepts;
            if (m_stock_concept_list.count(code) > 0)
            {
                concepts = m_stock_concept_list[code];
            }
            else
            {
                m_stats_info.m_concept_filter++;
                continue;
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
                                            3, new NumberSortTableWidgetItem(data.value(m_percentCol)));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            4, new NumberSortTableWidgetItem(data.value(m_todayCol)));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            5, new NumberSortTableWidgetItem(data.value(m_yesterdayCol)));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            6, new QTableWidgetItem(data.value(m_domainCol)));

                    QStringList all_concepts(QVector<QString>::fromStdVector(concepts).toList());
                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                                            7, new QTableWidgetItem(all_concepts.join(',')));

                    m_stats_info.m_output++;
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

bool BaGuaStock::LoadStockFileData2(QString filePath, QString filePath2)
{
    QFile file(filePath);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("加载数据失败"),
            QString::fromLocal8Bit("打开第一日数据文件失败！"));
        return false;
    }

    QFile file2(filePath2);
    if (!file2.open(QFile::ReadOnly | QFile::Text))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("加载数据失败"),
            QString::fromLocal8Bit("打开第二日数据文件失败！"));
        return false;
    }

    setWindowTitle(QApplication::applicationDisplayName() + " - " + filePath);

    QTextStream stream(&file);
    QString line = stream.readLine();

    if (!ParseHeaders2(line))
    {
        QMessageBox::critical(this, QString::fromLocal8Bit("加载数据失败"),
            QString::fromLocal8Bit("第一日数据文件中没有符合的列名！"));
        return false;
    }

    m_stock_data.push_back(line);

    line = stream.readLine();
    while (!line.isEmpty())
    {
        m_stock_data.push_back(line);

        line = stream.readLine();
    }

    // Day2 data
    QTextStream stream2(&file2);
    line = stream2.readLine();
    while (!line.isEmpty())
    {
        QStringList data = line.split("\t", QString::SkipEmptyParts);
        if (data.count() > 6)
        {
            QString code = data.value(m_codeCol);
            if (code.isEmpty())
            {
                line = stream2.readLine();
                continue;
            }

            QString today = data.value(m_todayCol);

            bool ok = false;
            double todayValue = today.toDouble(&ok);
            if (!ok)
            {
                line = stream2.readLine();
                continue;
            }

            m_stock_data2[code] = today;
        }

        line = stream2.readLine();
    }

    return true;
}

bool BaGuaStock::ParseHeaders2(QString firtLine)
{
    // 代码 名称 现价
    QStringList headers = firtLine.split("\t", QString::SkipEmptyParts);
    for (int c = 0; c < headers.count(); c++)
    {
        if (headers.value(c) == QString::fromLocal8Bit("代码"))
            m_codeCol = c;
        else if (headers.value(c) == QString::fromLocal8Bit("名称"))
            m_nameCol = c;
        else if (headers.value(c) == QString::fromLocal8Bit("现价"))
            m_todayCol = c;
    }

    if (m_codeCol < 0 || m_nameCol < 0 || m_todayCol < 0)
    {
        return false;
    }

    ui.tableWidgetOutput->insertColumn(0);
    ui.tableWidgetOutput->insertColumn(1);
    ui.tableWidgetOutput->insertColumn(2);
    ui.tableWidgetOutput->insertColumn(3);
    ui.tableWidgetOutput->insertColumn(4);

    QStringList usefulheaders;
    usefulheaders << QString::fromLocal8Bit("卦象")
        << QString::fromLocal8Bit("代码") << QString::fromLocal8Bit("名称")
        << QString::fromLocal8Bit("第一日现价") << QString::fromLocal8Bit("第二日现价");
    ui.tableWidgetOutput->setHorizontalHeaderLabels(usefulheaders);

    ui.tableWidgetOutput->horizontalHeader()->setStretchLastSection(true);

    return true;
}

void BaGuaStock::ApplyFilter2()
{
    int line_index = 0;
    for (const QString& line : m_stock_data)
    {
        line_index++;

        QStringList data = line.split("\t", QString::SkipEmptyParts);

        if (data.count() > 6)
        {
            QString code = data.value(m_codeCol);
            QString name = data.value(m_nameCol);
            QString day1 = data.value(m_todayCol);

            bool ok = false;
            double day1Value = day1.toDouble(&ok);
            if (!ok)
                continue;

            std::vector<QString> concepts;
            if (m_stock_concept_list.count(code) > 0)
            {
                concepts = m_stock_concept_list[code];
            }
            else
            {
                continue;
            }

            if (m_stock_data2.count(code) == 0)
                continue;

            QString day2 = m_stock_data2[code];

            int sumToday = 0;
            for (int i = 0; i < day2.length(); i++)
            {
                if (day2[i] == '.')
                    continue;

                int value = QString(day2[i]).toInt();
                sumToday += value;
            }

            int sumYesterday = 0;
            for (int i = 0; i < day1.length(); i++)
            {
                if (day1[i] == '.')
                    continue;

                int value = QString(day1[i]).toInt();
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
                        3, new NumberSortTableWidgetItem(day1));

                    ui.tableWidgetOutput->setItem(ui.tableWidgetOutput->rowCount() - 1,
                        4, new NumberSortTableWidgetItem(day2));

                    m_output_index_list.push_back(line_index - 1);
                    continue;
                }
                else
                {
                    continue;
                }
            }
            else
            {
                continue;
            }
        }
        else
        {
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
    m_stock_data2.clear();
    m_output_index_list.clear();
}