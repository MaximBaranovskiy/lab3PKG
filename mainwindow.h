#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "qboxlayout.h"
#include "qsplitter.h"
#include <QMainWindow>
#include <string>
#include<QLabel>
#include<opencv2/opencv.hpp>
#include<QSpinBox>
#include<QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    QLabel* labelOriginal;
    QLabel* labelFiltered;
    QSplitter* splitter;
    QVBoxLayout* layoutOriginal;
    QVBoxLayout* layoutFiltered;
    const std::string imageForBoxFilterPath = "C:/Users/maxim/Documents/lab3PKG/boxFilter.png";
    const std::string imageForGaussFilterPath = "C:/Users/maxim/Documents/lab3PKG/boxFilter.png";
    const std::string imageForLinearContrastPath ="C:/Users/maxim/Documents/lab3PKG/linearContrast.png";
    const std::string imageForHistogramPath ="C:/Users/maxim/Documents/lab3PKG/imageForHistogram.png";
    int kernelWidthForAveraging;
    int kernelHeightForAveraging;
    int kernelWidthForGauss;
    int kernelHeightForGauss;
    double alpha;
    int beta;
    std::string imageFromUserPath ="";
    void plotHistogram(const cv::Mat& image);
    QAction *actionMakeHistogram;
    bool shouldbuildHistogram;
    void plotHistogramForColor(const std::vector<cv::Mat>& channel, cv::Mat& image);
    void showImages(cv::Mat& imageOriginal,cv::Mat& imageFiltered,QImage::Format);
    void showParametersDialog();
    void setStyledButton(QPushButton *button);
    void setStyledSpinBox(QSpinBox *spinBox,int min,int max,int value);
    void setStyledDoubleSpinBox(QDoubleSpinBox *spinBox, double min, double max, double value);
private slots:
    void averagingFilter();
    void gaussFilter();
    void linearContrast();
    void makeHistogram();
    void makeHistogramForColor();
    void equalizeHistogram();
    void equalizeHistogramForRGB();
    void equalizeHistogramForHSV();
    void userFile();
    void defaultFiles();

};
#endif
