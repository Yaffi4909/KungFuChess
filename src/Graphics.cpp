#include "../../It1_interfaces/Graphics.hpp"
// #include <filesystem>
// #include <vector>
// #include <string>
// #include <algorithm> // בשביל std::transform

// std::vector<Img> Graphics::frames;

// namespace fs = std::filesystem;

// // // Graphics Graphics::copy() const {
// // //     // TODO: החזרת עותק של האובייקט
// // //     return *this; // עותק רדוד
// // // }

Graphics::Graphics(std::string sprites_folder_, std::pair<int, int> cell_size_, bool loop, float fps)
    : sprites_folder(sprites_folder_), cell_size(cell_size_), loop(loop), fps(fps), action("idle"), current_frame(0), total_frames(5)
{
    std::cout << "sprites_folder" << sprites_folder << std::endl;
    // Img img1;
    // img1.read(sprites_folder_ +"/BB/states/idle/sprites/1.png", cell_size_, true);
    // frames[0] = img1;
    // if (frames.empty())
    // {
    //     // אתחול של frames אם הוא ריק
    //     reset(); // לדוגמה, להקצות מקום מראש ל-100 פריימים
    // }
}

Graphics::Graphics(const Graphics &other)
    : sprites_folder(other.sprites_folder),
      cell_size(other.cell_size),
      loop(other.loop),
      fps(other.fps),
      action(other.action),
      current_frame(other.current_frame),
      total_frames(other.total_frames),
      last_update_ms(other.last_update_ms)
{
}
// void Graphics::reset(const Command &cmd) {

void Graphics::reset()
{

    // מימוש...
    // בדיקה אם התיקייה קיימת והיא תקייה חוקית
    // if (!fs::exists(sprites_folder) || !fs::is_directory(sprites_folder))
    // {

    //     throw std::runtime_error("Invalid sprites folder: " + sprites_folder);
    // }

    // // יצירת רשימה של כל הקבצים בתיקייה
    // std::vector<fs::path> files;

    // מעבר על כל התמונות היחודיות (1 לכל חייל) בתיקייה והוספתם לרשימה

    // for (const auto &piece_dir : fs::directory_iterator(sprites_folder))
    // {
    //     if (!piece_dir.is_directory())
    //         continue;

    //     fs::path img_path = piece_dir.path() / "states" / "idle" / "sprites" / "1.png";

    //     if (fs::exists(img_path))
    //     {
    //         files.push_back(img_path);
    //     }
    // }

    // // מיון הרשימה לפי סדר אלפביתי – חשוב בשביל סדר הפריימים
    // std::sort(files.begin(), files.end());

    // // מעבר על כל קובץ בתיקייה
    // for (const auto &path : files)
    // {
    //     Img img;
    //     try
    //     {
    //         // קריאת התמונה מהקובץ, שינוי גודל ושמירה ברשימת הפריימים
    //         img.read(path.string(), cell_size, true); // שמירה על יחס גובה-רוחב

    //         img.set_id(path.string().substr(13, 2));

    //         frames.push_back(img);
    //     }
    //     catch (const std::exception &e)
    //     {
    //         // במידה ולא מצליח לטעון – מדווח אך ממשיך
    //         std::cerr << "Failed to load sprite: " << path << " (" << e.what() << ")\n";
    //     }
    // }

    // // אם לא נטענו תמונות כלל – זריקת שגיאה
    // if (frames.empty())
    // {
    //     throw std::runtime_error("No valid sprites loaded from folder: " + sprites_folder);
    // }
}

void Graphics::update(int now_ms) {}

// Img Graphics::get_current_sprite(int now_ms) const
// // {

// //     if (frames.empty())
// //     {
// //         throw std::runtime_error("No frames loaded in Graphics.");
// //     }

// //     // אפשרות 1: אם כבר יש current_frame שמנוהל לפי הזמן
// //     int frame_index = current_frame;

// //     // אפשרות 2: לחשב frame_index לפי הזמן ו־fps (אם תרצי)
// //     float seconds = now_ms / 1000.0f;
// //     int total_frames = frames.size();
// //     int frame_index = static_cast<int>(seconds * fps) % total_frames;

// //     if (frame_index < 0 || frame_index >= static_cast<int>(frames.size()))
// //     {
// //         frame_index = 0; // הגנה מפני חריגה
// //     }

// //     return frames[frame_index];
// // }

Img Graphics::get_img_by_id(const std::string &id)
{
    if (current_frame == total_frames)
    {
        current_frame = 0;
        total_frames = 5;
        if (action == "move")
        {
            action = "long_rest";
        }
        else if (action == "jump")
        {
            action = "short_rest";
        }
        else
        {
            action = "idle";
        }
    }

    std::string full_path = sprites_folder + "/" + id.substr(0, 2) + "/states/" + action + "/sprites/" + std::to_string(current_frame + 1) + ".png";
    
    Img img = load_img(full_path, true);
    current_frame++;

    return img;
    
}

Img Graphics::load_img(const std::string &path, bool flip)
{
    Img img;
    try
    {
        img.read(path, cell_size, flip);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Failed to load sprite: " << path << " (" << e.what() << ")\n";
    }
    return img;
}
