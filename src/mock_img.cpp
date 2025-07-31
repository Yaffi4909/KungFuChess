#include "../../It1_interfaces/MockImg.hpp"

// אתחול סטטי של הווקטורים
std::vector<std::pair<int, int>> MockImg::traj;
std::vector<std::pair<std::pair<int, int>, std::string>> MockImg::txt_traj;

MockImg::MockImg() : Img()
{
    // כאן לא צריך cv2 אמיתי, אז שום דבר לא נעשה
}

void MockImg::read(const std::string &path)
{
    // מדמה קריאת תמונה – לא עושה כלום בפועל
    return;
}

void MockImg::draw_on(Img &other, int x, int y)
{

    // מתעד מיקום ציור
    traj.push_back({x, y});
}

void MockImg::put_text(const std::string &txt, int x, int y, int font_size)
{
    // מתעד טקסט ומיקומו
    txt_traj.push_back({{x, y}, txt});
}

void MockImg::show()
{
    // לא עושה כלום
}

void MockImg::reset()
{
    // איפוס היסטוריות
    traj.clear();
    txt_traj.clear();
}

const std::vector<std::pair<int, int>> &MockImg::get_traj()
{
    return traj;
}
const std::vector<std::pair<std::pair<int, int>, std::string>> &MockImg::get_txt_traj()
{
    return txt_traj;
}
