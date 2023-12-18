#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/opencv.hpp>
#include <QVBoxLayout>
#include <QAction>
#include <QMenuBar>
#include <QFileDialog>
#include<QPainter>
#include <QContextMenuEvent>
#include <QSpinBox>
#include<QPushButton>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    kernelWidthForAveraging  = 10;
    kernelHeightForAveraging = 10;
    kernelWidthForGauss = 11;
    kernelHeightForGauss = 11;
    alpha = 1.0;
    beta = 10;

    QAction *actionOpen = new QAction("Выбрать файл", this);
    QAction *actionBeginningFiles = new QAction("Использовать файлы по умолчанию");
    QAction *actionAveragingFilter = new QAction("Однородный усредняющий фильтр", this);
    QAction *actionGaussFilter = new QAction("Фильтр Гаусса", this);
    QAction *actionLinearContrast = new QAction("Линейное контрастирование", this);
    actionMakeHistogram = new QAction("Построение гистограммы для серых изображений", this);
        QAction *actionMakeHistogramForColor = new QAction("Построение гистограммы для цветных изображений", this);
    QAction *actionEqualizeHistogram = new QAction("Выравнивание гистограммы для серых изображений", this);
    QAction* actionEqualizeHistogramWithRGB = new QAction("Выравнивание гистограммы для трех цветовых компонент в RGB");
    QAction* actionEqualizeHistogramWithHSV = new QAction("Выравнивание компоненты яркости в пространстве HSV");

    connect(actionAveragingFilter, &QAction::triggered, this, &MainWindow::averagingFilter);
    connect(actionGaussFilter, &QAction::triggered, this, &MainWindow::gaussFilter);
    connect(actionLinearContrast, &QAction::triggered, this, &MainWindow::linearContrast);
    connect(actionMakeHistogram, &QAction::triggered, this, &MainWindow::makeHistogram);
    connect(actionEqualizeHistogram, &QAction::triggered, this, &MainWindow::equalizeHistogram);
    connect(actionOpen, &QAction::triggered, this, &MainWindow::userFile);
    connect(actionBeginningFiles, &QAction::triggered, this, &MainWindow::defaultFiles);
    connect(actionMakeHistogramForColor, &QAction::triggered, this, &MainWindow::makeHistogramForColor);
    connect(actionEqualizeHistogramWithRGB, &QAction::triggered, this, &MainWindow::equalizeHistogramForRGB);
    connect(actionEqualizeHistogramWithHSV,&QAction::triggered,this,&MainWindow::equalizeHistogramForHSV);


    QMenuBar *menuBar = this->menuBar();

    QMenu *fileMenu = menuBar->addMenu("Файл");
    fileMenu->addAction(actionOpen);
    fileMenu->addAction(actionBeginningFiles);

    QMenu *lowPassFiltersMenu = menuBar->addMenu("Низкочастотные фильтры");
    lowPassFiltersMenu->addAction(actionAveragingFilter);
    lowPassFiltersMenu->addAction(actionGaussFilter);

    QMenu *linearContrastMenu = menuBar->addMenu("Линейное контрастирование");
    linearContrastMenu->addAction(actionLinearContrast);

    QMenu *histogramMenu = menuBar->addMenu("Гистограммы");
    histogramMenu->addAction(actionMakeHistogram);
    histogramMenu->addAction(actionMakeHistogramForColor);
    histogramMenu->addAction(actionEqualizeHistogram);
    histogramMenu->addAction(actionEqualizeHistogramWithRGB);
    histogramMenu->addAction(actionEqualizeHistogramWithHSV);
    QMenu *parametersMenu = menuBar->addMenu("Параметры");

    QAction *actionParameters = new QAction("Настройки фильтров", this);
    connect(actionParameters, &QAction::triggered, this, &MainWindow::showParametersDialog);

    parametersMenu->addAction(actionParameters);

    labelOriginal = new QLabel(this);
    labelOriginal->setScaledContents(true);

    labelFiltered = new QLabel(this);
    labelFiltered->setScaledContents(true);

    splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(labelOriginal);
    splitter->addWidget(labelFiltered);

    layoutOriginal = new QVBoxLayout(labelOriginal);
    layoutFiltered = new QVBoxLayout(labelFiltered);

    labelOriginal->setLayout(layoutOriginal);
    labelFiltered->setLayout(layoutFiltered);

    setCentralWidget(splitter);

    setFixedSize(1100, 700);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::averagingFilter()
{
    cv::Mat imageOriginal;
    if(imageFromUserPath == ""){
        imageOriginal = cv::imread(imageForHistogramPath);
    }
    else{
        imageOriginal = cv::imread(imageFromUserPath);
    }
    cv::Mat result;
    cv::boxFilter(imageOriginal, result, -1, cv::Size(kernelWidthForAveraging, kernelHeightForAveraging));

    cv::cvtColor(imageOriginal, imageOriginal, cv::COLOR_BGR2RGB);
    cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
    showImages(imageOriginal,result,QImage::Format_RGB888);
}

void MainWindow::gaussFilter()
{
    cv::Mat imageOriginal;
    if(imageFromUserPath == ""){
        imageOriginal = cv::imread(imageForHistogramPath);
    }
    else{
        imageOriginal = cv::imread(imageFromUserPath);
    }

        if (imageOriginal.empty()) {
            qDebug()<<"Error: Unable to load image";
            return;
        }

        if (imageOriginal.channels() != 3) {
            qDebug() << "Error: Image must have three channels (RGB)";
            return;
        }

        cv::Mat result;
        cv::GaussianBlur(imageOriginal, result, cv::Size(kernelWidthForGauss,kernelHeightForGauss), 0);
        cv::cvtColor(imageOriginal, imageOriginal, cv::COLOR_BGR2RGB);
        cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
         showImages(imageOriginal,result,QImage::Format_RGB888);
}

void MainWindow::linearContrast()
{
        cv::Mat imageOriginal;
        if(imageFromUserPath == ""){
            imageOriginal = cv::imread(imageForHistogramPath);
        }
        else{
            imageOriginal = cv::imread(imageFromUserPath);
        }

        if (imageOriginal.empty()) {
            qDebug()<<"Error: Unable to load image";
            return;
        }

        cv::Mat result;
        cv::convertScaleAbs(imageOriginal, result, alpha, beta);

        cv::cvtColor(imageOriginal, imageOriginal, cv::COLOR_BGR2RGB);
        cv::cvtColor(result, result, cv::COLOR_BGR2RGB);
        showImages(imageOriginal,result,QImage::Format_RGB888);
}

void MainWindow::makeHistogram()
{
        cv::Mat imageOriginal;
        if(imageFromUserPath == ""){
            imageOriginal = cv::imread(imageForHistogramPath);
        }
        else{
            imageOriginal = cv::imread(imageFromUserPath);
        }

        if (imageOriginal.empty()) {
            qDebug() << "Error: Unable to load image";
            return;
        }

        cv::cvtColor(imageOriginal, imageOriginal, cv::COLOR_BGR2GRAY);

        plotHistogram(imageOriginal);

        labelOriginal->clear();

        QImage imgOriginal(imageOriginal.data, imageOriginal.cols, imageOriginal.rows, imageOriginal.step, QImage::Format_Grayscale8);
        QPixmap pixOriginal = QPixmap::fromImage(imgOriginal);
        labelOriginal->setPixmap(pixOriginal);
}

void MainWindow::showImages(cv::Mat &imageOriginal, cv::Mat &imageFiltered,QImage::Format format)
{
        labelFiltered->clear();
        QImage imgEqualized(imageFiltered.data, imageFiltered.cols, imageFiltered.rows, imageFiltered.step, format);
        QPixmap pixEqualized = QPixmap::fromImage(imgEqualized);
        labelFiltered->setPixmap(pixEqualized);

        labelOriginal->clear();
        QImage imgOriginal(imageOriginal.data, imageOriginal.cols, imageOriginal.rows, imageOriginal.step,format);
        QPixmap pixOriginal = QPixmap::fromImage(imgOriginal);
        labelOriginal->setPixmap(pixOriginal);

}

void MainWindow::makeHistogramForColor()
{
        cv::Mat originalImage;
        if (imageFromUserPath == "") {
            originalImage = cv::imread(imageForHistogramPath);
        } else {
            originalImage = cv::imread(imageFromUserPath);
        }

        if (originalImage.empty()) {
            qDebug() << "Error: Unable to load image";
            return;
        }

        std::vector<cv::Mat> bgrChannels;
        cv::split(originalImage, bgrChannels);

        int histHeight = 400;
        int histWidth = 256;
        cv::Mat histImage(histHeight, histWidth, CV_8UC3, cv::Scalar(255, 255, 255));

        plotHistogramForColor(bgrChannels, histImage);

        cv::cvtColor(originalImage, originalImage, cv::COLOR_BGR2RGB);
        QImage imgOriginal(originalImage.data, originalImage.cols, originalImage.rows, originalImage.step, QImage::Format_RGB888);
        QPixmap pixOriginal = QPixmap::fromImage(imgOriginal);
        labelOriginal->setPixmap(pixOriginal);
}


void MainWindow::equalizeHistogram()
{
        cv::Mat imageOriginal;
        if(imageFromUserPath == ""){
            imageOriginal = cv::imread(imageForHistogramPath);
        }
        else{
            imageOriginal = cv::imread(imageFromUserPath);
        }

        if (imageOriginal.empty()) {
            qDebug() << "Error: Unable to load image";
            return;
        }

        cv::cvtColor(imageOriginal, imageOriginal, cv::COLOR_BGR2GRAY);

        cv::Mat equalizedImage = imageOriginal.clone();

        cv::equalizeHist(equalizedImage, equalizedImage);

        showImages(imageOriginal,equalizedImage,QImage::Format_Grayscale8);
}

void MainWindow::equalizeHistogramForRGB()
{
        cv::Mat imageOriginal;
        if (imageFromUserPath == "") {
            imageOriginal = cv::imread(imageForHistogramPath);
        } else {
            imageOriginal = cv::imread(imageFromUserPath);
        }

        if (imageOriginal.empty()) {
            qDebug() << "Error: Unable to load image";
            return;
        }

        std::vector<cv::Mat> rgbChannels;
            cv::cvtColor(imageOriginal, imageOriginal, cv::COLOR_BGR2RGB);
        cv::split(imageOriginal, rgbChannels);

        for (int i = 0; i < 3; ++i) {
            cv::equalizeHist(rgbChannels[i], rgbChannels[i]);
        }

        cv::Mat equalizedImage;
        cv::merge(rgbChannels, equalizedImage);

        showImages(imageOriginal,equalizedImage,QImage::Format_RGB888);
}

void MainWindow::equalizeHistogramForHSV()
{
        cv::Mat imageOriginal;
        if (imageFromUserPath == "") {
            imageOriginal = cv::imread(imageForHistogramPath);
        } else {
            imageOriginal = cv::imread(imageFromUserPath);
        }

        if (imageOriginal.empty()) {
            qDebug() << "Error: Unable to load image";
            return;
        }

        cv::Mat hsvImage;
        cv::cvtColor(imageOriginal, hsvImage, cv::COLOR_BGR2HSV);

        std::vector<cv::Mat> hsvChannels;
        cv::split(hsvImage, hsvChannels);

        cv::equalizeHist(hsvChannels[2], hsvChannels[2]);

        cv::Mat equalizedImage;
        cv::merge(hsvChannels, equalizedImage);

        cv::cvtColor(imageOriginal, imageOriginal, cv::COLOR_BGR2RGB);
        cv::cvtColor(equalizedImage, equalizedImage, cv::COLOR_HSV2RGB);


        showImages(imageOriginal,equalizedImage,QImage::Format_RGB888);
}

void MainWindow::plotHistogram(const cv::Mat& image)
{
            int histSize = 256;
            float range[] = {0, 256};
            const float* histRange = {range};

            cv::Mat hist;
            cv::calcHist(&image, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

            cv::normalize(hist, hist, 0, labelFiltered->height(), cv::NORM_MINMAX);

            QImage histImage(labelFiltered->width(), labelFiltered->height(), QImage::Format_RGB32);
            histImage.fill(Qt::white);

            QPainter painter(&histImage);
            painter.setPen(Qt::black);

            float binWidth = labelFiltered->width() / histSize;
            for (int i = 0; i < histSize; ++i) {
                int x = i * binWidth;
                double width = binWidth;
                double height = hist.at<float>(i);

                painter.drawRect(x, labelFiltered->height() - height, width, height);
            }

            painter.drawLine(0, labelFiltered->height(), labelFiltered->width(), labelFiltered->height());

            painter.drawText(0, labelFiltered->height(), "0");
            painter.drawText(labelFiltered->width() - 20, labelFiltered->height(), "255");

            labelFiltered->clear();
            labelFiltered->setPixmap(QPixmap::fromImage(histImage));
}

void MainWindow::plotHistogramForColor(const std::vector<cv::Mat>& channel, cv::Mat& image) {
            int histSize = 256;
            float range[] = {0, 256};
            const float* histRange = {range};

            cv::Mat hist;
            cv::calcHist(&channel[0], 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

            cv::normalize(hist, hist, 0, labelFiltered->height()/3, cv::NORM_MINMAX);

            QImage histImage(labelFiltered->width(), labelFiltered->height(), QImage::Format_RGB32);
            histImage.fill(Qt::white);

            QPainter painter(&histImage);
            painter.setPen(QColorConstants::Svg::blue);

            float binWidth = labelFiltered->width() / histSize;
            for (int i = 0; i < histSize; ++i) {
                int x = i * binWidth;
                double width = binWidth;
                double height = hist.at<float>(i);

                painter.drawRect(x, labelFiltered->height()/3 - height, width, height);
            }

            painter.drawLine(0, labelFiltered->height()/3, labelFiltered->width(), labelFiltered->height()/3);

            painter.drawText(0, labelFiltered->height()/3, "0");
            painter.drawText(labelFiltered->width() - 20, labelFiltered->height()/3, "255");

            cv::calcHist(&channel[1], 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

            cv::normalize(hist, hist, 0, labelFiltered->height()/3, cv::NORM_MINMAX);

            painter.setPen(QColorConstants::Svg::green);

            for (int i = 0; i < histSize; ++i) {
                int x = i * binWidth;
                double width = binWidth;
                double height = hist.at<float>(i);

                painter.drawRect(x, 2*labelFiltered->height()/3 - height, width, height);
            }

            painter.drawLine(0, 2*labelFiltered->height()/3, labelFiltered->width(), 2*labelFiltered->height()/3);

            painter.drawText(0, 2*labelFiltered->height()/3 + 10, "0");
            painter.drawText(labelFiltered->width() - 20, 2*labelFiltered->height()/3 + 10, "255");

            cv::calcHist(&channel[2], 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

            cv::normalize(hist, hist, 0, labelFiltered->height()/3, cv::NORM_MINMAX);

            painter.setPen(QColorConstants::Svg::red);

            for (int i = 0; i < histSize; ++i) {
                int x = i * binWidth;
                double width = binWidth;
                double height = hist.at<float>(i);

                painter.drawRect(x, labelFiltered->height() - height, width, height);
            }

            painter.drawLine(0, labelFiltered->height(), labelFiltered->width(), labelFiltered->height());

            painter.drawText(0, labelFiltered->height() -10, "0");
            painter.drawText(labelFiltered->width() - 20, labelFiltered->height() - 10, "255");


            labelFiltered->clear();
            labelFiltered->setPixmap(QPixmap::fromImage(histImage));
}

void MainWindow::defaultFiles()
{
            imageFromUserPath = "";
}

void MainWindow::userFile()
{
            QString filePath = QFileDialog::getOpenFileName(this, "Выберите изображение", QDir::homePath(), "Images (*.png *.jpg *.bmp *.jpeg)");

            if (!filePath.isEmpty()) {
                imageFromUserPath = filePath.toStdString();
                qDebug() << "Выбранный путь к изображению: " << imageFromUserPath;
                cv::Mat image = cv::imread(imageFromUserPath);
            } else {
                qDebug() << "Выбор файла отменен";
            }

}

void MainWindow::showParametersDialog()
{
            QDialog parametersDialog(this);
            parametersDialog.setWindowTitle("Настройки фильтров");

            QSpinBox *averagingFilterWidthSpinBox = new QSpinBox(&parametersDialog);
            setStyledSpinBox(averagingFilterWidthSpinBox,1,99,kernelWidthForAveraging);

            QSpinBox *averagingFilterHeightSpinBox = new QSpinBox(&parametersDialog);
            setStyledSpinBox(averagingFilterHeightSpinBox,1,99,kernelHeightForAveraging);

            QSpinBox *gaussFilterWidthSpinBox = new QSpinBox(&parametersDialog);
            setStyledSpinBox(gaussFilterWidthSpinBox,1,99,kernelWidthForGauss);

            QSpinBox *gaussFilterHeightSpinBox = new QSpinBox(&parametersDialog);
            setStyledSpinBox(gaussFilterHeightSpinBox,1,99,kernelHeightForGauss);

            QDoubleSpinBox *linearContrastAlphaSpinBox = new QDoubleSpinBox(&parametersDialog);
            setStyledDoubleSpinBox(linearContrastAlphaSpinBox,1,99,alpha);

            QSpinBox *linearContrastBetaSpinBox = new QSpinBox(&parametersDialog);
            setStyledSpinBox(linearContrastBetaSpinBox,1,99,beta);

            QPushButton *okButton = new QPushButton("OK", &parametersDialog);
            QPushButton *cancelButton = new QPushButton("Cancel", &parametersDialog);
            setStyledButton(okButton);
            setStyledButton(cancelButton);

            QVBoxLayout *layout = new QVBoxLayout(&parametersDialog);

            layout->addWidget(new QLabel("Размер ядра для однородного усредняющего фильтра:"));
            layout->addWidget(new QLabel("Ширина:"));
            layout->addWidget(averagingFilterWidthSpinBox);
            layout->addWidget(new QLabel("Высота:"));
            layout->addWidget(averagingFilterHeightSpinBox);
            layout->addWidget(new QFrame);

            layout->addWidget(new QLabel("Размер ядра для фильтра Гаусса:"));
            layout->addWidget(new QLabel("Ширина:"));
            layout->addWidget(gaussFilterWidthSpinBox);
            layout->addWidget(new QLabel("Высота:"));
            layout->addWidget(gaussFilterHeightSpinBox);
            layout->addWidget(new QFrame);

            layout->addWidget(new QLabel("Параметры для линейного контрастирования:"));
            layout->addWidget(new QLabel("Alpha:"));
            layout->addWidget(linearContrastAlphaSpinBox);
            layout->addWidget(new QLabel("Beta:"));
            layout->addWidget(linearContrastBetaSpinBox);

            layout->addWidget(okButton);
            layout->addWidget(cancelButton);

            connect(okButton, &QPushButton::clicked, &parametersDialog, &QDialog::accept);
            connect(cancelButton, &QPushButton::clicked, &parametersDialog, &QDialog::reject);

            if (parametersDialog.exec() == QDialog::Accepted) {
                kernelWidthForAveraging = averagingFilterWidthSpinBox->value();
                kernelHeightForAveraging = averagingFilterHeightSpinBox->value();

                int kernelWFG = gaussFilterWidthSpinBox->value();
                int kernelHFG = gaussFilterHeightSpinBox->value();
                if(kernelWFG%2!=0){
                    kernelWidthForGauss = kernelWFG;
                }
                if(kernelHFG%2!=0){
                    kernelHeightForGauss = kernelHFG;
                }

                alpha = linearContrastAlphaSpinBox->value();
                beta = linearContrastBetaSpinBox->value();
            }
}

void MainWindow::setStyledSpinBox(QSpinBox *spinBox,int min,int max,int value)
{
            spinBox->setMinimum(min);
            spinBox->setMaximum(max);
            spinBox->setValue(value);
            spinBox->setAlignment(Qt::AlignCenter);

            spinBox->setStyleSheet("QSpinBox {"
                                   "   background-color: #f0f0f0;"
                                   "   color: #333333;"
                                   "   border: 1px solid #aaaaaa;"
                                   "   border-radius: 2px;"
                                   "   padding: 2px;"
                                   "}"
                                   "QSpinBox::up-button, QSpinBox::down-button {"
                                   "   width: 16px;"
                                   "   height: 16px;"
                                   "}");
            QFont font = spinBox->font();
            font.setPointSize(12);
            spinBox->setFont(font);
}

void MainWindow::setStyledDoubleSpinBox(QDoubleSpinBox *spinBox,double min,double max,double value)
{
            spinBox->setMinimum(min);
            spinBox->setMaximum(max);
            spinBox->setValue(value);
            spinBox->setAlignment(Qt::AlignCenter);

            spinBox->setStyleSheet("QSpinBox {"
                                   "   background-color: #f0f0f0;"
                                   "   color: #333333;"
                                   "   border: 1px solid #aaaaaa;"
                                   "   border-radius: 2px;"
                                   "   padding: 2px;"
                                   "}"
                                   "QSpinBox::up-button, QSpinBox::down-button {"
                                   "   width: 16px;"
                                   "   height: 16px;"
                                   "}");
            QFont font = spinBox->font();
            font.setPointSize(12);
            spinBox->setFont(font);
}


void MainWindow::setStyledButton(QPushButton *button)
{
            button->setStyleSheet("QPushButton {"
                                  "   background-color: #4CAF50;"
                                  "   color: white;"
                                  "   border: none;"
                                  "   border-radius: 3px;"
                                  "   padding: 8px 16px;"
                                  "   text-align: center;"
                                  "   text-decoration: none;"
                                  "   display: inline-block;"
                                  "   font-size: 14px;"
                                  "   transition-duration: 0.4s;"
                                  "   cursor: pointer;"
                                  "}"
                                  "QPushButton:hover {"
                                  "   background-color: white;"
                                  "   color: black;"
                                  "   border: 1px solid #4CAF50;"
                                  "}");
            QFont font = button->font();
            font.setPointSize(14);
            button->setFont(font);
}
