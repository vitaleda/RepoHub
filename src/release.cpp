#include "release.h"
#include "draw.h"

#include "curl-tools.h"

void draw_release_details(Release release, std::string header_start) {
    init_keys();

    int list_size = static_cast<int> (release.assets.size());
    int cursor_pos = 0;

    std::string final_body_string = release.body;

    strip_carriage_return(final_body_string);

    std::string header_string = header_start;
    header_string += "->";
    header_string += release.name;

    int status = 0;

    while(1) {
        update_keys();
        if(cross_released && list_size > 0) {

            if(release.assets[cursor_pos].size < get_space_avail()) {
                std::string message_string = "Download ";
                message_string += release.assets[cursor_pos].name;
                message_string += "\nto 'ux0:data/Repo-Explorer?'";
                if(draw_yes_no_message(message_string)) {
                    std::string filePath = "ux0:data/repo-browser/Downloads/";
                    filePath += release.assets[cursor_pos].name;
                    curl_download_file(release.assets[cursor_pos].url, filePath);
                }
            }
            else {
                std::string message_string = "Not enough Space!";
                draw_alert_message(message_string);
            }
        }

        if(circle_released)
            break;

        if(left_released) status -= 1;
        if(right_released) status += 1;
        if(status < 0) status = 1;
        if(status > 1) status = 0;

        if(up_released) cursor_pos -= 1;
        if(down_released) cursor_pos += 1;
        if(cursor_pos < 0) cursor_pos = list_size -1;
        if(cursor_pos > list_size -1) cursor_pos = 0;

        vita2d_start_drawing();
        vita2d_clear_screen();

        draw_header(header_string);

        if(status == 0) {
            //file list
            for(int i = 0; i < list_size; i++) {
                draw_list_item_small(release.assets[i].name, 44+81+i*100, cursor_pos == i);
            }

            //sub-header bg.
            vita2d_draw_rectangle(0, 45, 960, 80, RGBA8(255,255,255,255));
            vita2d_draw_line(0, 45+80, 960, 45+80, RGBA8(150,150,150,150));
            vita2d_font_draw_text(font40, 40, (45+40)+10, RGBA8(0,0,0,255), 40.0f, "Files");
            vita2d_font_draw_text(font40, 960-560, (45+40)+10, RGBA8(150,150,150,150), 40.0f, "Notes");
        }
        else {
            //body and body bg
            vita2d_font_draw_text(font15, 40, 45+100, RGBA8(0,0,0,255), 15.0f, final_body_string.c_str());

            //sub-header bg.
            vita2d_draw_rectangle(0, 45, 960, 80, RGBA8(255,255,255,255));
            vita2d_draw_line(0, 45+80, 960, 45+80, RGBA8(150,150,150,150));
            vita2d_font_draw_text(font40, 40, (45+40)+10, RGBA8(150,150,150,150), 40.0f, "Files");
            vita2d_font_draw_text(font40, 960-560, (45+40)+10, RGBA8(0,0,0,255), 40.0f, "Notes");
        }

        vita2d_end_drawing();
        vita2d_common_dialog_update();
        vita2d_swap_buffers();
    }
}
