#include "../lv_examples.h"
#if LV_BUILD_EXAMPLES && LV_USE_FLEX

static void event_cb(lv_event_t * e)
{
    /*The original target of the event. Can be the buttons or the container*/
    lv_obj_t * target = lv_event_get_target(e); //获取事件最初的目标对象。即使事件是冒泡的，也是一样的。

    /*The current target is always the container as the event is added to it*/
    lv_obj_t * cont = lv_event_get_current_target(e);//获取事件的当前目标。它是事件处理程序被调用的对象。

    /*If container was clicked do nothing*/
    if(target == cont) 
		{
			return;
		}

    /*Make the clicked buttons red*/
    lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_RED), 0);
}

/**
 * Demonstrate event bubbling
 *lv_scr_act() Get the active screen of the default display 
 *LV_EVENT_CLICKED  Called on release if not scrolled (regardless to long press) 
 */
void lv_example_event_3(void)
{

    lv_obj_t * cont = lv_obj_create(lv_scr_act());//Create a base object (a rectangle)//
    lv_obj_set_size(cont, 290, 200);
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    uint32_t i;
    for(i = 0; i < 40; i++) {
        lv_obj_t * btn = lv_btn_create(cont);
        lv_obj_set_size(btn, 80, 50);
        lv_obj_add_flag(btn, LV_OBJ_FLAG_EVENT_BUBBLE);
				//lv_obj_center(btn);

        lv_obj_t * label = lv_label_create(btn);
        lv_label_set_text_fmt(label, "%"LV_PRIu32, i);
        lv_obj_center(label);
    }

    lv_obj_add_event_cb(cont, event_cb, LV_EVENT_CLICKED, NULL);
}

#endif
