#include "../../lv_examples.h"
#if LV_USE_FLEX && LV_BUILD_EXAMPLES

/**
 * RTL base direction changes order of the items.
 * Also demonstrate how horizontal scrolling works with RTL.
 */
void lv_example_flex_6(void)
{
    lv_obj_t * cont = lv_obj_create(lv_scr_act());
   // lv_obj_set_style_base_dir(cont, LV_BASE_DIR_RTL, 0);
    lv_obj_set_size(cont, 300, 220);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);  //Set hot the item should flow

	    uint32_t i;
	
    for(i = 0; i < 20; i++) {
        lv_obj_t * obj = lv_obj_create(cont);
        lv_obj_set_size(obj, 70, LV_SIZE_CONTENT); //LV_SIZE_CONTENT
			

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text_fmt(label, "%"LV_PRIu32, i);
        lv_obj_center(label);
			
//				lv_obj_t * btn = lv_btn_create(obj);
//				lv_obj_set_size(btn, 10, 20);
//				lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
			
			
    }
}
#endif
