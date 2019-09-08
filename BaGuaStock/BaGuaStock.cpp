#include "BaGuaStock.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>

BaGuaStock::BaGuaStock(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButtonOpen, SIGNAL(clicked()), this, SLOT(openFile()));

    LoadKeyFileData();
    LoadGua();
}

void BaGuaStock::resizeEvent(QResizeEvent *event)
{
    int columnCount = ui.tableWidget->columnCount();
    for(int c = 0; c < columnCount; c++)
    {
        ui.tableWidget->setColumnWidth(c, ui.tableWidget->width() / columnCount);
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
        LoadFileData(filePath);
    }
}

void BaGuaStock::LoadGua()
{
    m_topGua.push_back(Gua(1, 1, 1));
    m_topGua.push_back(Gua(0, 1, 1));
    m_topGua.push_back(Gua(1, 0, 1));
    m_topGua.push_back(Gua(0, 0, 1));
    m_topGua.push_back(Gua(1, 1, 0));
    m_topGua.push_back(Gua(0, 1, 0));
    m_topGua.push_back(Gua(1, 0, 0));
    m_topGua.push_back(Gua(0, 0, 0));

    m_downGua.push_back(Gua(1, 1, 1));
    m_downGua.push_back(Gua(0, 1, 1));
    m_downGua.push_back(Gua(1, 0, 1));
    m_downGua.push_back(Gua(0, 0, 1));
    m_downGua.push_back(Gua(1, 1, 0));
    m_downGua.push_back(Gua(0, 1, 0));
    m_downGua.push_back(Gua(1, 0, 0));
    m_downGua.push_back(Gua(0, 0, 0));
}

int BaGuaStock::FindGua(bool isTop, Gua gua)
{
    if (isTop)
    {
        for (int i = 0; i < m_topGua.size(); i++)
        {
            if (m_topGua[i] == gua)
            {
                return i;
            }
        }
    }
    else
    {
        for (int i = 0; i < m_downGua.size(); i++)
        {
            if (m_downGua[i] == gua)
            {
                return i;
            }
        }
    }

    return -1;
}

bool BaGuaStock::LoadKeyFileData()
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
    while(!line.isEmpty() && row < 8)
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

bool BaGuaStock::LoadFileData(QString filePath)
{
    Clear();

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

    line = stream.readLine();
    while (!line.isEmpty())
    {
        m_total++;

        ParseLine(line);

        line = stream.readLine();
    }

    UpdateStats();

    QMessageBox::information(this, QString::fromLocal8Bit("数据处理完成"),
                          QString::fromLocal8Bit("数据处理完成！"));

    return true;
}

void BaGuaStock::UpdateStats()
{
    ui.labelTotal->setText(QString::number(m_total));
    ui.labelShow->setText(QString::number(m_show));
    ui.labelFilter->setText(QString::number(m_filter));
    ui.labelST->setText(QString::number(m_ST));
    ui.labelNoBuy->setText(QString::number(m_noBuy));
    ui.labelOther->setText(QString::number(m_other));
}

void BaGuaStock::Clear()
{
    m_codeCol = -1;
    m_nameCol = -1;
    m_percentCol = -1;
    m_todayCol = -1;
    m_yesterdayCol = -1;
    m_domainCol = -1;

    m_total = 0;
    m_show = 0;
    m_filter = 0;
    m_ST = 0;
    m_noBuy = 0;
    m_other = 0;

    UpdateStats();

    ui.tableWidget->clear();

    setWindowTitle(QApplication::applicationDisplayName());
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

    ui.tableWidget->insertColumn(0);
    ui.tableWidget->insertColumn(1);
    ui.tableWidget->insertColumn(2);
    ui.tableWidget->insertColumn(3);
    ui.tableWidget->insertColumn(4);
    ui.tableWidget->insertColumn(5);

    QStringList usefulheaders;
    usefulheaders << QString::fromLocal8Bit("代码") << QString::fromLocal8Bit("名称")
        << QString::fromLocal8Bit("涨幅%") << QString::fromLocal8Bit("现价")
        << QString::fromLocal8Bit("昨收") << QString::fromLocal8Bit("细分行业");
    ui.tableWidget->setHorizontalHeaderLabels(usefulheaders);

    int columnCount = ui.tableWidget->columnCount();
    for (int c = 0; c < columnCount; c++)
    {
        ui.tableWidget->setColumnWidth(c, ui.tableWidget->width() / columnCount);
    }

    return true;
}

bool BaGuaStock::ParseLine(QString line)
{
    QStringList data = line.split("\t", QString::SkipEmptyParts);
    
    if (data.count() > 6)
    {
        QString name = data.value(m_nameCol);
        if (name.contains("ST", Qt::CaseInsensitive))
        {
            m_ST++;
            return true;
        }

        QString today = data.value(m_todayCol);
        QString yestoday = data.value(m_yesterdayCol);

        bool ok = false;
        double todayValue = today.toDouble(&ok);
        if (!ok)
        {
            m_noBuy++;
            return true;
        }
        double yesterdayValue = yestoday.toDouble(&ok);
        if (!ok)
        {
            m_noBuy++;
            return true;
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

        Gua topGua = m_topGua[top - 1];
        Gua downGua = m_downGua[down - 1];

        int var = (sumYesterday + sumToday) % 6;
        if (var == 0)
            var = 6;

        if (var > 0 && var < 4 )
        {
            downGua.Rows()[3 - var] = abs(downGua.Rows()[3 - var] - 1);
        }
        else
        {
            topGua.Rows()[6 - var] = abs(topGua.Rows()[6 - var] - 1);
        }

        int keyRow = FindGua(false, downGua);
        int keyCol = FindGua(true, topGua);

        if (keyRow < m_keys.size())
        {
            int key = m_keys[keyRow][keyCol];
            if (key == 1)
            {
                ui.tableWidget->insertRow(ui.tableWidget->rowCount());

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        0, new QTableWidgetItem(data.value(m_codeCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        1, new QTableWidgetItem(data.value(m_nameCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        2, new QTableWidgetItem(data.value(m_percentCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        3, new QTableWidgetItem(data.value(m_todayCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        4, new QTableWidgetItem(data.value(m_yesterdayCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        5, new QTableWidgetItem(data.value(m_domainCol)));

                m_show++;
                return true;
            }
            else
            {
                m_filter++;
                return true;
            }
        }
        else
        {
            m_other++;
            return true;
        }
        
    }
    else
    {
        m_other++;
        return true;
    }

    return true;
}