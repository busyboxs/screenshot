#include "APPMainDialog.h"
#include <QListWidgetItem>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QDebug>
#include <QFileDialog>
#include <QApplication>
#include <QClipboard>
#include <filesystem>
#include "functions/Passport.h"
#include "functions/Household.h"
#include "functions/BusinessLicense.h"
#include "functions/businessCard.h"
#include "functions/bankcard.h"
#include "functions/IDCard.h"
#include "SelectorWidget.h"

#include "xlsx/xlsxdocument.h"
#include "xlsx/xlsxformat.h"
#include "xlsx/xlsxchartsheet.h"
#include "xlsx/xlsxcellrange.h"
#include "xlsx/xlsxchart.h"
#include "xlsx/xlsxrichstring.h"
#include "xlsx/xlsxworkbook.h"
using namespace QXlsx;

namespace fs = std::filesystem;

APPMainDialog::APPMainDialog(QWidget *parent)
    : QWidget(parent)
{
    setWindowFlags(Qt::FramelessWindowHint);

    loadQss();
    setFixedSize(300, 600);

    QWidget* pCaptionWidget = createCaptionWidget();
    pCaptionWidget->setWhatsThis("captionWidget");
    QWidget* pTopWidget = createTopWidget();
    pTopWidget->setWhatsThis("topWidget");
    QWidget* pBtnWidget = createBtnWidget();
    pBtnWidget->setWhatsThis("btnWidget");
    QWidget* pBottomWidget = createBottomWidget();
    pBottomWidget->setWhatsThis("bottomWidget");
    QWidget* pExportWidget = createExportWidget();
    pExportWidget->setWhatsThis("exportWidget");

    QVBoxLayout* pvLayout = new QVBoxLayout();
    pvLayout->setSpacing(0);
    pvLayout->setContentsMargins(0, 0, 0, 0);
    pvLayout->addWidget(pCaptionWidget, 30);
    pvLayout->addWidget(pTopWidget, 50);
    pvLayout->addWidget(pBtnWidget, 80);
    pvLayout->addWidget(pBottomWidget, 360);
    pvLayout->addWidget(pExportWidget, 80);

    connect(this, SIGNAL(sigClearResult()), this, SLOT(onClearResult()));

    setLayout(pvLayout);
}

APPMainDialog::~APPMainDialog()
{
}

QWidget* APPMainDialog::createCaptionWidget()
{
    QWidget* pCaptionWidget = new QWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setAlignment(Qt::AlignVCenter);
    hLayout->setContentsMargins(3, 0, 0, 0);
    hLayout->setSpacing(0);

    QLabel* pIconLabel = new QLabel();
    pIconLabel->setFixedSize(20, 20);
    QPixmap pix("images/favicon.png");
    pIconLabel->setPixmap(pix.scaled(20, 20, Qt::KeepAspectRatio));

    QSpacerItem* spc = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Expanding);

    QPushButton* pMinBtn = new QPushButton();
    pMinBtn->setFixedSize(30, 30);
    pMinBtn->setWhatsThis("menuBtn");
    pMinBtn->setIcon(QIcon("images/min.png"));
    connect(pMinBtn, SIGNAL(clicked()), this, SLOT(showMinimized()));
    QPushButton* pCloseBtn = new QPushButton();
    pCloseBtn->setFixedSize(30, 30);
    pCloseBtn->setWhatsThis("menuBtn");
    pCloseBtn->setIcon(QIcon("images/close.png"));
    connect(pCloseBtn, SIGNAL(clicked()), this, SLOT(close()));

    hLayout->addWidget(pIconLabel);
    hLayout->addSpacerItem(spc);
    hLayout->addWidget(pMinBtn);
    hLayout->addWidget(pCloseBtn);

    pCaptionWidget->setLayout(hLayout);
    pCaptionWidget->setAcceptDrops(true);

    return pCaptionWidget;
}

QWidget* APPMainDialog::createTopWidget()
{
    QWidget* pTopWidget = new QWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setAlignment(Qt::AlignCenter);
    hLayout->setContentsMargins(0, 0, 0, 0);
    hLayout->setSpacing(0);
    QLabel* title = new QLabel(pTopWidget);
    title->setWhatsThis("title");
    title->setText(u8"OCR识别小工具");

    hLayout->addWidget(title);
    pTopWidget->setLayout(hLayout);

    m_pTitleLabel = title;
    return pTopWidget;
}

QWidget* APPMainDialog::createBtnWidget()
{
    QWidget* pToolWidget = new QWidget();
    QVBoxLayout* vLayout = new QVBoxLayout();
    vLayout->setSpacing(20);
    vLayout->setContentsMargins(10, 0, 10, 0);

    QWidget* pComboWidget = new QWidget();
    QHBoxLayout* comboLayout = new QHBoxLayout();
    comboLayout->setAlignment(Qt::AlignVCenter);
    comboLayout->setContentsMargins(70, 0, 70, 0);
    comboLayout->setSpacing(0);
    m_pCombobox = new QComboBox();
    m_pCombobox->setWhatsThis("comboType");
    m_pCombobox->setFixedHeight(24);
    connect(m_pCombobox, SIGNAL(activated(int)), this, SLOT(onComboBoxIdActive(int)));
    connect(m_pCombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(onComboBoxIdChange(int)));
    QStringList comboContents;

    for (auto i = 0; i < OCRTypes.size(); ++i)
    {
        comboContents << QString::fromStdString(OCRTypes[i]);
    }

    addComboBoxContents(comboContents);
    comboLayout->addWidget(m_pCombobox);
    pComboWidget->setLayout(comboLayout);

    QWidget* pBtnWidget = new QWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setAlignment(Qt::AlignVCenter);
    hLayout->setContentsMargins(40, 0, 40, 0);
    hLayout->setSpacing(20);

    m_pUploadBtn = new QPushButton();
    m_pUploadBtn->setText(u8"上传图片");
    m_pUploadBtn->setFixedHeight(24);
    m_pUploadBtn->setEnabled(false);
    connect(m_pUploadBtn, SIGNAL(clicked()), this, SLOT(onUpload()));

    m_pScreenShotBtn = new QPushButton();
    m_pScreenShotBtn->setText(u8"截图识别");
    m_pScreenShotBtn->setFixedHeight(24);
    m_pScreenShotBtn->setEnabled(false);
    connect(m_pScreenShotBtn, SIGNAL(clicked()), this, SLOT(onScreenShot()));
    
    hLayout->addWidget(m_pUploadBtn);
    hLayout->addWidget(m_pScreenShotBtn);
    pBtnWidget->setLayout(hLayout);

    vLayout->addWidget(pComboWidget);
    vLayout->addWidget(pBtnWidget);
    pToolWidget->setLayout(vLayout);

    return pToolWidget;
}

QWidget* APPMainDialog::createBottomWidget()
{
    QWidget* pBottomWidget = new QWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setAlignment(Qt::AlignVCenter);
    hLayout->setContentsMargins(20, 20, 20, 0);
    hLayout->setSpacing(0);
    m_listResults = new QListWidget();
    

    hLayout->addWidget(m_listResults);
    pBottomWidget->setLayout(hLayout);

    return pBottomWidget;
}

QWidget* APPMainDialog::createExportWidget()
{
    QWidget* pExportWidget = new QWidget();
    QHBoxLayout* hLayout = new QHBoxLayout();
    hLayout->setAlignment(Qt::AlignVCenter);
    hLayout->setContentsMargins(40, 0, 40, 0);
    hLayout->setSpacing(20);

    m_exportExcel = new QPushButton();
    m_exportExcel->setText(u8"导出Excel");
    m_exportExcel->setFixedHeight(24);
    m_exportExcel->setEnabled(false);
    connect(m_exportExcel, SIGNAL(clicked()), this, SLOT(onExport()));

    m_copy = new QPushButton();
    m_copy->setText((u8"一键复制"));
    m_copy->setFixedHeight(24);
    m_copy->setEnabled(false);
    connect(m_copy, SIGNAL(clicked()), this, SLOT(onCopy()));

    hLayout->addWidget(m_exportExcel);
    hLayout->addWidget(m_copy);

    pExportWidget->setLayout(hLayout);

    return pExportWidget;
}

void APPMainDialog::loadQss()
{
    QFile qss(":/screenshot/screenshot.qss");
    if (qss.open(QFile::ReadOnly))
    {
        QString style = qss.readAll();
        setStyleSheet(style);
        qss.close();
    }
}

void APPMainDialog::addListContents(QStringList contents)
{
    m_listResults->clear();
    m_listResults->addItems(contents);
    /*for (const auto& content : contents)
    {
        QListWidgetItem* pItem = new QListWidgetItem();
        pItem->setText(content);

    }*/
}

void APPMainDialog::addComboBoxContents(QStringList contents)
{

    m_pCombobox->addItems(contents);
}

QStringList APPMainDialog::OCRDetect(int index, const QString& imgPath)
{
    QStringList listContents;
    switch (index)
    {
    case 0:
        break;
    case 1:
        listContents = passportOCR(imgPath);
        break;
    case 2:
        listContents = businessLicenseOCR(imgPath);
        break;
    case 3:
        listContents = householdOCR(imgPath);
        break;
    case 4:
        listContents = idcardOCR(imgPath, true);
        break;
    case 5:
        listContents = idcardOCR(imgPath, false);
        break;
    case 6:
        listContents = bandcardOCR(imgPath);
        break;
    case 7:
        listContents = businesscardOCR(imgPath);
        break;
    default:
        break;
    }

    return listContents;
}

QStringList APPMainDialog::passportOCR(const QString& imgPath)
{
    PassportInfo result = PassPortDetect(imgPath.toStdString());

    return getStringList(result);
}

QStringList APPMainDialog::householdOCR(const QString& imgPath)
{
    HouseholdInfo result = HouseholdDetect(imgPath.toStdString());

    return getStringList(result);
}

QStringList APPMainDialog::businessLicenseOCR(const QString& imgPath)
{
    BusinessLicenseInfo result = BusinessLicenseDetect(imgPath.toStdString());

    return getStringList(result);
}

QStringList APPMainDialog::idcardOCR(const QString& imgPath, bool front)
{
    IDCardInfo result = IDCardDetect(imgPath.toStdString(), front);

    return getStringList(result);
}

QStringList APPMainDialog::bandcardOCR(const QString& imgPath)
{
    BankCardInfo result = BandCardDetect(imgPath.toStdString());

    QStringList listContents;

    for (auto& [name, res] : result.resultMap)
    {
        listContents << std::string(name + ": " + res).c_str();
    }

    return listContents;
}

QStringList APPMainDialog::businesscardOCR(const QString& imgPath)
{
    BusinessCardInfo result = BusinessCardDetect(imgPath.toStdString());
    
    QStringList listContents;

    for (auto& [name, res] : result.wordsResult)
    {
        listContents << std::string(name + ": " + res).c_str();
    }

    return listContents;
}

void APPMainDialog::WriteExcel(const QString& filename, int index)
{
    Document xlsx;
    Format format;
    format.setHorizontalAlignment(Format::AlignHCenter);
    format.setVerticalAlignment(Format::AlignVCenter);

    xlsx.addSheet(QString::fromStdString(OCRTypes[index]));
    xlsx.setColumnWidth(1, 20);
    xlsx.setColumnWidth(2, 70);

    int currentRow = 1;
    std::string text;
    for (int i = 0; i < m_listResults->count(); ++i)
    {
        text = m_listResults->item(i)->text().toStdString();
        auto pos = text.find(u8":");
        xlsx.write(i + 1, 1, text.substr(0, pos).c_str());
        xlsx.write(i + 1, 2, text.substr(pos + 1).c_str());
    }

    xlsx.saveAs(filename);
}

void APPMainDialog::mouseMoveEvent(QMouseEvent* event)
{
    if (m_isDrag && (event->buttons() == Qt::LeftButton))
    {
        move(event->globalPos() - m_position);
        event->accept();
    }
}

void APPMainDialog::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        m_isDrag = true;
        m_position = event->globalPos() - this->pos();
        event->accept();
    }
}

void APPMainDialog::mouseReleaseEvent(QMouseEvent* event)
{
    m_isDrag = false;
}

void APPMainDialog::onUpload()
{
    m_listResults->clear();
    QString path = QFileDialog::getOpenFileName(this, tr(u8"选择图片文件"), ".", tr("Image Files(*.jpg *.png *.jpeg)"));
    if (!path.isEmpty())
    {
        addListContents(OCRDetect(m_pCombobox->currentIndex(), path));
        m_exportExcel->setEnabled(true);
        m_copy->setEnabled(true);
    }
}

void APPMainDialog::onScreenShot()
{
    this->hide();
    Sleep(200);
    m_listResults->clear();
    SelectorWidget* selectWidget = new SelectorWidget(this);
    selectWidget->exec();
    if (fs::exists("shot.jpg"))
    {
        addListContents(OCRDetect(m_pCombobox->currentIndex(), "shot.jpg"));
        m_exportExcel->setEnabled(true);
        m_copy->setEnabled(true);
        this->show();
        fs::remove("shot.jpg");
    }
}

void APPMainDialog::onExport()
{
    std::string saveName = "./" + OCRTypes[m_pCombobox->currentIndex()] + ".xlsx";
    QString fileName = QFileDialog::getSaveFileName(this, tr("导出Excel文件"), saveName.c_str(), "*.xlsx");
    if (!fileName.isEmpty())
    {
        WriteExcel(fileName, m_pCombobox->currentIndex());
    }
}

void APPMainDialog::onComboBoxIdActive(int id)
{
    m_pUploadBtn->setEnabled(id);
    m_pScreenShotBtn->setEnabled(id);
    if (id == 0)
    {
        m_exportExcel->setEnabled(false);
        m_copy->setEnabled(false);
    }
}

void APPMainDialog::onComboBoxIdChange(int id)
{
    emit sigClearResult();
}

void APPMainDialog::onCopy()
{
    QStringList texts;
    for (int i = 0; i < m_listResults->count(); ++i)
    {
        texts.append(m_listResults->item(i)->text());
    }

    QApplication::clipboard()->setText(texts.join("\n"));
    m_copy->setText(u8"复制成功");
}

void APPMainDialog::onClearResult()
{
    m_exportExcel->setEnabled(false);
    m_copy->setEnabled(false);
    m_copy->setText(u8"一键复制");
    m_listResults->clear();
}


