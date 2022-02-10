#include "../../lv_examples.h"
#if LV_USE_BTNMATRIX && LV_BUILD_EXAMPLES

static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * obj = lv_event_get_target(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        uint32_t id = lv_btnmatrix_get_selected_btn(obj);
        const char * txt = lv_btnmatrix_get_btn_text(obj, id);

        LV_LOG_USER("%s was pressed\n", txt);
    }
}


static const char * btnm_map[] = {"%", "CE", "C", "BACK", "\n",
																	"7", "8", "9", "*", "\n",
																	"4", "5", "6", "-", "\n",
																	"1", "2", "3", "+", "\n", 
																	"&", "0", ".", "/", "\n",
                                  "F1", "F2", "F3", "="};

void lv_example_btnmatrix_1(void)
{
    lv_obj_t * btnm1 = lv_btnmatrix_create(lv_scr_act());
    lv_btnmatrix_set_map(btnm1, btnm_map);
//    lv_btnmatrix_set_btn_width(btnm1, 10, 6);        /*Make "Action1" twice as wide as "Action2"*/
   // lv_btnmatrix_set_btn_ctrl(btnm1, 10, LV_BTNMATRIX_CTRL_POPOVER );
   // lv_btnmatrix_set_btn_ctrl(btnm1, 11, LV_BTNMATRIX_CTRL_CHECKED);
	  lv_btnmatrix_set_btn_ctrl_all(btnm1, LV_BTNMATRIX_CTRL_POPOVER);
    lv_obj_align(btnm1, LV_ALIGN_CENTER, 0, 20);
    lv_obj_add_event_cb(btnm1, event_handler, LV_EVENT_ALL, NULL);
	lv_obj_set_size(btnm1, 250, 200);
}

#endif
