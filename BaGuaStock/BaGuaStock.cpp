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
    LoadGuaXiang();
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
    m_guaXiang.push_back(r1);

    std::vector<QString> r2;
    r2.push_back(QString::fromLocal8Bit("天泽履"));
    r2.push_back(QString::fromLocal8Bit("兑为泽"));
    r2.push_back(QString::fromLocal8Bit("火泽睽"));
    r2.push_back(QString::fromLocal8Bit("雷泽归妹"));
    r2.push_back(QString::fromLocal8Bit("风泽中孚"));
    r2.push_back(QString::fromLocal8Bit("水泽节"));
    r2.push_back(QString::fromLocal8Bit("山泽损"));
    r2.push_back(QString::fromLocal8Bit("地泽临"));
    m_guaXiang.push_back(r2);

    std::vector<QString> r3;
    r3.push_back(QString::fromLocal8Bit("天火同人"));
    r3.push_back(QString::fromLocal8Bit("泽火革"));
    r3.push_back(QString::fromLocal8Bit("离为火"));
    r3.push_back(QString::fromLocal8Bit("雷火丰"));
    r3.push_back(QString::fromLocal8Bit("风火家人"));
    r3.push_back(QString::fromLocal8Bit("水火既济"));
    r3.push_back(QString::fromLocal8Bit("山火贲"));
    r3.push_back(QString::fromLocal8Bit("地火明夷"));
    m_guaXiang.push_back(r3);

    std::vector<QString> r4;
    r4.push_back(QString::fromLocal8Bit("天雷无妄"));
    r4.push_back(QString::fromLocal8Bit("泽雷随"));
    r4.push_back(QString::fromLocal8Bit("火雷噬磕"));
    r4.push_back(QString::fromLocal8Bit("震为雷"));
    r4.push_back(QString::fromLocal8Bit("风雷益"));
    r4.push_back(QString::fromLocal8Bit("水雷屯"));
    r4.push_back(QString::fromLocal8Bit("山雷颐"));
    r4.push_back(QString::fromLocal8Bit("地雷复"));
    m_guaXiang.push_back(r4);

    std::vector<QString> r5;
    r5.push_back(QString::fromLocal8Bit("天风垢"));
    r5.push_back(QString::fromLocal8Bit("泽风大过"));
    r5.push_back(QString::fromLocal8Bit("火风鼎"));
    r5.push_back(QString::fromLocal8Bit("雷风恒"));
    r5.push_back(QString::fromLocal8Bit("巽为风"));
    r5.push_back(QString::fromLocal8Bit("水风井"));
    r5.push_back(QString::fromLocal8Bit("山风蛊"));
    r5.push_back(QString::fromLocal8Bit("地风升"));
    m_guaXiang.push_back(r5);

    std::vector<QString> r6;
    r6.push_back(QString::fromLocal8Bit("天水讼"));
    r6.push_back(QString::fromLocal8Bit("泽水困"));
    r6.push_back(QString::fromLocal8Bit("火水未济"));
    r6.push_back(QString::fromLocal8Bit("雷水解"));
    r6.push_back(QString::fromLocal8Bit("风水涣"));
    r6.push_back(QString::fromLocal8Bit("坎为水"));
    r6.push_back(QString::fromLocal8Bit("山水蒙"));
    r6.push_back(QString::fromLocal8Bit("地水师"));
    m_guaXiang.push_back(r6);

    std::vector<QString> r7;
    r7.push_back(QString::fromLocal8Bit("天山遁"));
    r7.push_back(QString::fromLocal8Bit("泽山咸"));
    r7.push_back(QString::fromLocal8Bit("火山旅"));
    r7.push_back(QString::fromLocal8Bit("雷山小过"));
    r7.push_back(QString::fromLocal8Bit("风山渐"));
    r7.push_back(QString::fromLocal8Bit("水山蹇"));
    r7.push_back(QString::fromLocal8Bit("艮为山"));
    r7.push_back(QString::fromLocal8Bit("地山谦"));
    m_guaXiang.push_back(r7);

    std::vector<QString> r8;
    r8.push_back(QString::fromLocal8Bit("天地否"));
    r8.push_back(QString::fromLocal8Bit("泽地萃"));
    r8.push_back(QString::fromLocal8Bit("火地晋"));
    r8.push_back(QString::fromLocal8Bit("雷地豫"));
    r8.push_back(QString::fromLocal8Bit("风地观"));
    r8.push_back(QString::fromLocal8Bit("水地比"));
    r8.push_back(QString::fromLocal8Bit("山地剥"));
    r8.push_back(QString::fromLocal8Bit("坤为地"));
    m_guaXiang.push_back(r8);
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
    ui.labelST->setText(QString::number(m_creation));
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
    m_creation;
    m_noBuy = 0;
    m_other = 0;

    UpdateStats();

    ui.tableWidget->setRowCount(0);
    ui.tableWidget->setColumnCount(0);

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
    ui.tableWidget->insertColumn(6);

    QStringList usefulheaders;
    usefulheaders << QString::fromLocal8Bit("卦象") 
        << QString::fromLocal8Bit("代码") << QString::fromLocal8Bit("名称")
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
        QString code = data.value(m_codeCol);
        if (code.startsWith("688", Qt::CaseInsensitive))
        {
            m_creation++;
            return true;
        }

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
                ui.tableWidget->insertRow(ui.tableWidget->rowCount());

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        0, new QTableWidgetItem(m_guaXiang[keyRow][keyCol]));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        1, new QTableWidgetItem(data.value(m_codeCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        2, new QTableWidgetItem(data.value(m_nameCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        3, new QTableWidgetItem(data.value(m_percentCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        4, new QTableWidgetItem(data.value(m_todayCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        5, new QTableWidgetItem(data.value(m_yesterdayCol)));

                ui.tableWidget->setItem(ui.tableWidget->rowCount() - 1,
                                        6, new QTableWidgetItem(data.value(m_domainCol)));

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