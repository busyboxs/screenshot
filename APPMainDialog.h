#pragma once

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QComboBox>
#include <QMouseEvent>
#include <QPoint>
#include <vector>
#include <map>
#include <json/json.h>


class APPMainDialog : public QWidget
{
    Q_OBJECT

public:
    APPMainDialog(QWidget *parent = nullptr);
    ~APPMainDialog();

private:
    QWidget* createCaptionWidget();
    QWidget* createTopWidget();
    QWidget* createBtnWidget();
    QWidget* createBottomWidget();
    QWidget* createExportWidget();

    void loadQss();
    void addListContents(QStringList contents);
    void addComboBoxContents(QStringList contents);

private:
    template<typename T>
    QStringList getStringList(const T& result);
    QStringList OCRDetect(int index, const QString& imgPath);
    QStringList passportOCR(const QString& imgPath);
    QStringList householdOCR(const QString& imgPath);
    QStringList businessLicenseOCR(const QString& imgPath);
    QStringList idcardOCR(const QString& imgPath, bool front);
    QStringList bandcardOCR(const QString& imgPath);
    QStringList businesscardOCR(const QString& imgPath);
    void WriteExcel(const QString& filename, int index);

protected:
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

public slots:
    void onUpload();
    void onScreenShot();
    void onExport();
    void onComboBoxIdActive(int);
    void onComboBoxIdChange(int);
    void onCopy();
    void onClearResult();

signals:
    void sigClearResult();

private:
    QLabel*      m_pTitleLabel;
    QComboBox*   m_pCombobox;
    QPushButton* m_pUploadBtn;
    QPushButton* m_pScreenShotBtn;
    QPushButton* m_exportExcel;
    QPushButton* m_copy;
    QListWidget* m_listResults;
    bool         m_isDrag{ false };
    QPoint       m_position{};

    inline static std::map<int, std::string> OCRTypes = {
        {0, u8"请选择识别类型"},
        {1, u8"护照识别"},
        {2, u8"营业执照识别"},
        {3, u8"户口本识别"},
        {4, u8"身份证识别(正面)"},
        {5, u8"身份证识别(反面)"},
        {6, u8"银行卡识别"},
        {7, u8"名片识别"}
    };
};

template<typename T>
inline QStringList APPMainDialog::getStringList(const T& result)
{
    QStringList listContents;

    for (auto& [name, res] : result.wordsResult)
    {
        listContents << std::string(name + ": " + res.words).c_str();
    }

    return listContents;
}
