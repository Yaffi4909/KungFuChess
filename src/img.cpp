#include "img.hpp"
#include <iostream>
#include <stdexcept>

Img::Img()
{
    // Constructor - img is automatically initialized as empty
}

Img &Img::read(const std::string &path,
               const std::pair<int, int> &size,
               bool keep_aspect,
               int interpolation)
{
    img = cv::imread(path, cv::IMREAD_UNCHANGED);
    if (img.empty())
    {
        throw std::runtime_error("Cannot load image: " + path);
    }

    if (size.first != 0 && size.second != 0)
    { // Check if size is not empty
        int target_w = size.first;
        int target_h = size.second;
        int h = img.rows;
        int w = img.cols;

        if (keep_aspect)
        {
            double scale = std::min(static_cast<double>(target_w) / w,
                                    static_cast<double>(target_h) / h);
            int new_w = static_cast<int>(w * scale);
            int new_h = static_cast<int>(h * scale);
            cv::resize(img, img, cv::Size(new_w, new_h), 0, 0, interpolation);
        }
        else
        {
            cv::resize(img, img, cv::Size(target_w, target_h), 0, 0, interpolation);
        }
    }

    return *this;
}
/////////////
void Img::draw_on(Img &other_img, int x, int y)
{
    // std::cout << "draw_on" << std::endl;
    if (img.empty() || other_img.img.empty())
    {
        throw std::runtime_error("Both images must be loaded before drawing.");
    }

    // Match number of channels if needed
    if (img.channels() != other_img.img.channels())
    {

        if (img.channels() == 3 && other_img.img.channels() == 4)
        {

            cv::cvtColor(img, img, cv::COLOR_BGR2BGRA);
        }
        else if (img.channels() == 4 && other_img.img.channels() == 3)
        {

            cv::cvtColor(img, img, cv::COLOR_BGRA2BGR);
        }
    }

    int h = img.rows;
    int w = img.cols;
    int H = other_img.img.rows;
    int W = other_img.img.cols;
    // std::cout << y << "+" << h << ">" <<  H << "||" << x << "+" << w << ">" << W << std::endl;
    if (y + h > H || x + w > W)
    {
        throw std::runtime_error("Image does not fit at the specified position.");
    }

    cv::Mat roi = other_img.img(cv::Rect(x, y, w, h));

    // ...existing code...
    if (img.channels() == 4)
    {
        std::vector<cv::Mat> channels;
        cv::split(img, channels);
        cv::Mat alpha;
        channels[3].convertTo(alpha, CV_32F, 1.0 / 255.0);

        std::vector<cv::Mat> roi_channels;
        cv::split(roi, roi_channels);

        for (int c = 0; c < 3; ++c)
        {
            cv::Mat src_f, roi_f;
            channels[c].convertTo(src_f, CV_32F);
            roi_channels[c].convertTo(roi_f, CV_32F);

            cv::Mat blended = (1.0 - alpha).mul(roi_f) + alpha.mul(src_f);
            blended.convertTo(roi_channels[c], CV_8U);
        }
        cv::merge(roi_channels, roi);
    }
    else
    {
        img.copyTo(roi);
    }
    // std::cout << "drawn seccesfuly" << std::endl;
    // ...existing code...
}

void Img::put_text(const std::string &txt, int x, int y, double font_size,
                   const cv::Scalar &color, int thickness)
{
    if (img.empty())
    {
        throw std::runtime_error("Image not loaded.");
    }

    cv::putText(img, txt, cv::Point(x, y),
                cv::FONT_HERSHEY_SIMPLEX, font_size,
                color, thickness, cv::LINE_AA);
}

void Img::show()
{
    if (img.empty())
    {
        throw std::runtime_error("Image not loaded.");
    }

    cv::imshow("Image", img);
    cv::waitKey(0);
    cv::destroyAllWindows();
}