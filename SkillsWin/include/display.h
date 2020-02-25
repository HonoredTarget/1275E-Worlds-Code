#include "../include/display/lvgl.h"
#include "display/lv_themes/lv_theme_templ_code.h"
#include "display/lv_themes/lv_theme_default_code.h"
//#include "display/lv_themes/lv_theme_alien_code.h"
#include "display/lv_themes/lv_theme_night.h"
#include "display/lv_themes/lv_theme_mono_code.h"
#include "display/lv_conf.h"
#include "display/lv_draw/lv_draw.h"
#include "../src/globals.hpp"

int autonomousPreSet;

LV_IMG_DECLARE(DogLogo);

lv_res_t setAuton(lv_obj_t *, const char *txt) {
    if (txt == "Blue P") autonomousPreSet = 1;
    else if (txt == "Blue UP") autonomousPreSet = 2;
    else if (txt == "Red P") autonomousPreSet = 3;
    else if (txt == "Red UP") autonomousPreSet = 4;
    else if (txt == "EXP") autonomousPreSet = 5;
    else if (txt == "Skills") autonomousPreSet = 6;
};
// 1 : Blue Protected
// 2 : Blue Unprotected
// 3 : Red Protected
// 4 : Red Unprotected
// 5 : Experimental
// 6 : Skills


// Auton Selector Written by Owen Oertell
int AutonSelector() {
    // Set the current theme
    lv_theme_t * th = lv_theme_alien_init(65, NULL);
    lv_theme_set_current(th);

    // Create New Page scr
    lv_obj_t * scr = lv_page_create(NULL, NULL);
    lv_scr_load(scr);

    // Add a title label
    lv_obj_t * label = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_text(label, ""); 
    // ---------------------------------------------------------------------------------------------- 
    
    // Add the button matrix
    static const char * btnm_str[] = {"Blue P", "Skills", "Red P", "\n", "Blue UP", "EXP", "Red UP", ""};
    lv_obj_t * btnm = lv_btnm_create(lv_scr_act(), NULL);
    lv_obj_set_size(btnm, 280, 180);
    lv_btnm_set_map(btnm, btnm_str);
    lv_obj_align(btnm, lv_scr_act(), LV_ALIGN_CENTER, -70, 0);
    lv_btnm_set_toggle(btnm, true, 1);
    lv_btnm_set_action(btnm, setAuton);
    // Add the bulldog logo?
    lv_obj_t * img1 = lv_img_create(lv_scr_act(), NULL);
    lv_img_set_src(img1,&DogLogo);
    lv_img_set_auto_size(img1, true);
    lv_obj_align(img1, lv_scr_act(), LV_ALIGN_CENTER, 160,0);
};  