#include "utils.h"

QPixmap cvMat2QPixmap(cv::Mat image)
{
    if (image.channels() == 4)
    {
        cv::Mat bgra;
        cv::cvtColor(image, bgra, cv::COLOR_BGRA2RGBA);
        return QPixmap::fromImage(QImage(bgra.data, bgra.cols, bgra.rows, bgra.step, QImage::Format_RGBA8888));
    }
    else
    {
        cv::Mat rgb;
        cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
        return QPixmap::fromImage(QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888));
    }
}

QImage cvMat2QImage(cv::Mat image)
{
    if (image.channels() == 4)
    {
        cv::Mat bgra;
        cv::cvtColor(image, bgra, cv::COLOR_BGRA2RGBA);
        return QImage(bgra.data, bgra.cols, bgra.rows, bgra.step, QImage::Format_RGBA8888).copy();
    }
    else
    {
        cv::Mat rgb;
        cv::cvtColor(image, rgb, cv::COLOR_BGR2RGB);
        return QImage(rgb.data, rgb.cols, rgb.rows, rgb.step, QImage::Format_RGB888).copy();
    }
}

cv::Mat channelUpgrade(cv::Mat image)
{
    if (image.channels() == 4) {
        return image;
    }
    else {
        cv::Mat upgradedImage;
        cv::cvtColor(image, upgradedImage, cv::COLOR_BGR2BGRA);
        return upgradedImage;
    }
}

cv::Mat QImage2cvMat(const QImage& image)
{
    cv::Mat mat;
    switch (image.format()) {
    case QImage::Format_ARGB32:
    case QImage::Format_ARGB32_Premultiplied: {
        mat = cv::Mat(image.height(), image.width(), CV_8UC4, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        cv::cvtColor(mat, mat, cv::COLOR_BGRA2BGR);  // 移除 alpha 通道
        break;
    }
    case QImage::Format_RGB32:
    case QImage::Format_RGB888: {
        QImage swapped = image;
        if (image.format() == QImage::Format_RGB888) {
            swapped = image.rgbSwapped();
        }
        mat = cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine());
        break;
    }
    case QImage::Format_Grayscale8: {
        mat = cv::Mat(image.height(), image.width(), CV_8UC1, const_cast<uchar*>(image.bits()), image.bytesPerLine());
        break;
    }
    default: {
        QImage converted = image.convertToFormat(QImage::Format_RGB888);
        QImage swapped = converted.rgbSwapped();
        mat = cv::Mat(swapped.height(), swapped.width(), CV_8UC3, const_cast<uchar*>(swapped.bits()), swapped.bytesPerLine());
        break;
    }
    }
    return mat.clone(); // 返回 mat 的深拷贝，以确保数据的独立性
}
