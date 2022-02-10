#include "../lv_examples.h"
#include "test1.h"

lv_obj_t * obj[2];
lv_obj_t * cont;
//static void event_cb(lv_event_t * e)
//{
//	//static uint32_t cnt = 1;
//    /*The original target of the event. Can be the buttons or the container*/
//    lv_obj_t * target = lv_event_get_target(e); //获取事件最初的目标对象。即使事件是冒泡的，也是一样的。

////    /*The current target is always the container as the event is added to it*/
////    lv_obj_t * cont = lv_event_get_current_target(e);//获取事件的当前目标。它是事件处理程序被调用的对象。

////    /*If container was clicked do nothing*/
////    if(target == cont) return;

//    /*Make the clicked buttons red*/
//    lv_obj_set_style_bg_color(target, lv_palette_main(LV_PALETTE_RED), 0);
//	
//	lv_obj_align_to(obj[1], obj[0], LV_ALIGN_CENTER, 120, 0);
////	lv_obj_align(obj[0], LV_ALIGN_CENTER, -50, 0);
////	lv_obj_set_align(obj[1], LV_ALIGN_BOTTOM_MID);
////	lv_obj_remove_style_all(obj[0]);
//	
//	lv_obj_t * label1 = lv_label_create(cont);
//  lv_label_set_text_fmt(label1, "Obj is align");
//	lv_obj_set_pos(label1, 80, 150);
//	
////    lv_obj_t * label = lv_obj_get_child(target, 0);
////    lv_label_set_text_fmt(label, "%"LV_PRIu32, cnt);
////    cnt++;
//}

///**
// * Demonstrate event bubbling
// *lv_scr_act() Get the active screen of the default display 
// *LV_EVENT_CLICKED  Called on release if not scrolled (regardless to long press) 
// */
//void lv_example_test(void)
//{
//   lv_obj_t * label[2];
//    cont = lv_obj_create(lv_scr_act());//Create a base object (a rectangle)//
//    lv_obj_set_size(cont, 280, 200);
//    lv_obj_center(cont);
//   // lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

//    uint32_t i;
//	  uint32_t w1, h1;
//	  //i = 1;
//    for(i = 0; i < 2; i++) {
//        obj[i] = lv_obj_create(cont);
//       // lv_obj_set_size(obj[i], 80, 50);
//				lv_obj_set_width(obj[i], 90); //The actual width: padding left + 50 + padding right
//				lv_obj_set_height(obj[i], 60); //The actual width: padding top + 30 + padding bottom
//				lv_coord_t w = lv_obj_get_width(obj[i]);
//				lv_coord_t h = lv_obj_get_height(obj[i]);
//			w1 = w;
//			h1 = h;
//			  lv_obj_set_pos(obj[i], 90 * (i), 20 *i );

//        label[0] = lv_label_create(obj[i]);
//				lv_label_set_text_fmt(label[0], "width:%"LV_PRIu32, w1); //LV_PRIu32
//				lv_obj_set_pos(label[0], 0, 0 );
//			   
//			  label[1] = lv_label_create(obj[i]);
//				lv_label_set_text_fmt(label[1], "height:%"LV_PRIu32, h1);
//				lv_obj_set_pos(label[1], 0, 20 );
//			  
//			
//    }
//	
//				lv_obj_t * btn = lv_btn_create(cont);
//        lv_obj_set_size(btn, 80, 50);
//				//lv_obj_set_pos(btn, 100, 100);
//				lv_obj_center(btn);
//				
//				lv_obj_t * label1 = lv_label_create(btn);
//        lv_label_set_text_fmt(label1, "ALIGN");
//        lv_obj_center(label1);

//    lv_obj_add_event_cb(btn, event_cb, LV_EVENT_CLICKED, NULL);
//}

///**
// * Demonstrate event bubbling
// *lv_scr_act() Get the active screen of the default display 
// *LV_EVENT_CLICKED  Called on release if not scrolled (regardless to long press) 
// */
//void lv_example_test(void)
//{
//	lv_obj_t * label[2];
//	lv_style_value_t v;
//		static lv_style_t style_btn;
//			
//		lv_style_init(&style_btn);
//		lv_style_set_bg_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
//		lv_style_set_bg_opa(&style_btn, LV_OPA_30);	
//		lv_style_set_border_width(&style_btn, 2);
//		lv_style_set_border_color(&style_btn, lv_palette_main(LV_PALETTE_BLUE));
//	
//	
//		static lv_style_t style_btn_red;
//		lv_style_init(&style_btn_red);
//		lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
//		lv_style_set_bg_opa(&style_btn_red, LV_OPA_COVER);
//	
//	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
//	lv_obj_add_style(btn1, &style_btn, LV_STATE_DEFAULT);
//	lv_obj_add_style(btn1, &style_btn_red, LV_STATE_PRESSED);
//	lv_obj_set_pos(btn1, 5, 5);
//	lv_obj_set_size(btn1, 80, 50);
//	
//	lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
//		lv_obj_add_style(btn2, &style_btn_red, LV_STATE_PRESSED);
//	lv_obj_set_pos(btn2, 100, 5);
//	lv_obj_set_size(btn2, 80, 50);
//	
//	
//		 obj[0] = lv_obj_create(lv_scr_act());
//		lv_obj_set_size(obj[0], 80, 50);
//		lv_obj_set_pos(obj[0], 90, 100);
//     
//		lv_obj_add_style(obj[0], &style_btn_red, LV_PART_SCROLLBAR | LV_STATE_SCROLLED ); //LV_PART_SCROLLBAR
//		label[0] = lv_label_create(obj[0]);
//		lv_label_set_text_fmt(label[0], "Scroll test"); //LV_PRIu32
//		lv_obj_set_pos(label[0], 0, 0 );
//		//lv_obj_set_scroll_dir(obj[0], LV_DIR_LEFT);
//		lv_obj_set_scrollbar_mode(obj[0], LV_SCROLLBAR_MODE_AUTO);

//}




static void event_cb(lv_event_t * e)
{
    /*The original target of the event. Can be the buttons or the container*/
    lv_obj_t * target = lv_event_get_target(e); //获取事件最初的目标对象。即使事件是冒泡的，也是一样的。


    lv_obj_move_foreground(target);
	
	
}
 
 
 
 
 
// 
//void lv_example_test(void)
//{
//	lv_obj_t * label[2];
//	lv_style_value_t v;
//		static lv_style_t style_btn;
////	lv_disp_t *disp;
//	
////	lv_obj_t * scr = lv_obj_create(NULL);
////	//		lv_disp_set_default(disp);
////	
//	
//		lv_style_init(&style_btn);
//		lv_style_set_bg_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
//		lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);	
//		lv_style_set_border_width(&style_btn, 2);
//		lv_style_set_border_color(&style_btn, lv_palette_main(LV_PALETTE_BLUE));
//	
//	
//		static lv_style_t style_btn_red;
//		lv_style_init(&style_btn_red);
//		lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
//		lv_style_set_bg_opa(&style_btn_red, LV_OPA_COVER);
//	
//	lv_obj_t * btn1 = lv_btn_create(lv_scr_act());
//	lv_obj_add_style(btn1, &style_btn, LV_STATE_DEFAULT);
//	lv_obj_add_style(btn1, &style_btn_red, LV_STATE_PRESSED);
////	lv_btn_set_fit(btn1, true, true); /*Enable to automatically set the size according to the content*/
//	lv_obj_set_pos(btn1, 5, 5);
//	lv_obj_set_size(btn1, 80, 50);
//	label[0] = lv_label_create(btn1);
//		lv_label_set_text_fmt(label[0], "Button1"); //LV_PRIu32
//		lv_obj_set_pos(label[0], 0, 0 );
//		//lv_obj_set_scroll_dir(obj[0], LV_DIR_LEFT);
//	lv_label_set_text(label[0], "Button1" LV_SYMBOL_OK);

//	
//	 lv_obj_t * btn2 = lv_btn_create(lv_scr_act());
//		lv_obj_add_style(btn2, &style_btn_red, LV_STATE_PRESSED);
//	lv_obj_set_pos(btn2, 50, 20);
//	lv_obj_set_size(btn2, 80, 50);
//	label[0] = lv_label_create(btn2);
//		lv_label_set_text_fmt(label[0], "Button2"); //LV_PRIu32
//		lv_obj_set_pos(label[0], 0, 0 );
//	
//		obj[0] = lv_obj_create(lv_scr_act());
//		lv_obj_set_size(obj[0], 80, 50);
//		lv_obj_set_pos(obj[0], 100, 150);
//     
//		lv_obj_add_style(obj[0], &style_btn_red, LV_PART_SCROLLBAR | LV_STATE_SCROLLED ); //LV_PART_SCROLLBAR
//		label[0] = lv_label_create(obj[0]);
//		lv_label_set_text_fmt(label[0], "Scroll test"); //LV_PRIu32
//		lv_obj_set_pos(label[0], 0, 0 );
//		//lv_obj_set_scroll_dir(obj[0], LV_DIR_LEFT);
//	//lv_obj_move_foreground(btn);
//	lv_obj_add_event_cb(btn1, event_cb, LV_EVENT_CLICKED, NULL);
//	lv_obj_add_event_cb(btn2, event_cb, LV_EVENT_CLICKED, NULL);
//	lv_obj_add_flag(btn2, LV_OBJ_FLAG_EVENT_BUBBLE);
//	
//	

//}




//static void event_handler(lv_event_t * e)
//{
//    lv_obj_t * obj = lv_event_get_target(e);
//    LV_LOG_USER("Button %d clicked", (int)lv_obj_get_index(obj));
//}

//void lv_example_test(void)
//{
//		static lv_style_t style_btn_red;
//		lv_style_init(&style_btn_red);
//		lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
//		lv_style_set_bg_opa(&style_btn_red, LV_OPA_COVER);
//	
//	
//    lv_obj_t * win = lv_win_create(lv_scr_act(), 40);
//    lv_obj_t * btn;
//    btn = lv_win_add_btn(win, LV_SYMBOL_LEFT, 40);
//    lv_obj_add_style(btn, &style_btn_red, LV_STATE_PRESSED); 
//		lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);
//	
//	
//    lv_win_add_title(win, "A title");

//    btn = lv_win_add_btn(win, LV_SYMBOL_RIGHT, 40);
//		lv_obj_add_style(btn, &style_btn_red, LV_STATE_PRESSED); 
//    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

//    btn = lv_win_add_btn(win, LV_SYMBOL_CLOSE, 60);
//		lv_obj_add_style(btn, &style_btn_red, LV_STATE_PRESSED); 
//    lv_obj_add_event_cb(btn, event_handler, LV_EVENT_CLICKED, NULL);

//    lv_obj_t * cont = lv_win_get_content(win);  /*Content can be aded here*/
//    lv_obj_t * label =  lv_label_create(cont);
//    lv_label_set_text(label, "This is\n"
//                             "a pretty\n"
//                             "long text\n"
//                             "to see how\n"
//                             "the window\n"
//                             "becomes\n"
//                             "scrollable.\n"
//                             "\n"
//                             "\n"
//                             "Some more\n"
//                             "text to be\n"
//                             "sure it\n"
//                             "overflows. :)");
//														 
//		lv_obj_add_style(cont, &style_btn_red, LV_PART_SCROLLBAR | LV_STATE_SCROLLED );


//}


//static void event_handler(lv_event_t * e)
//{
//    lv_event_code_t code = lv_event_get_code(e);
//    lv_obj_t * obj = lv_event_get_current_target(e);

//    if(code == LV_EVENT_VALUE_CHANGED) {
//        lv_calendar_date_t date;
//        if(lv_calendar_get_pressed_date(obj, &date)) {
//            LV_LOG_USER("Clicked date: %02d.%02d.%d", date.day, date.month, date.year);
//        }
//    }
//}

//void lv_example_test(void)
//{
//    lv_obj_t  * calendar = lv_calendar_create(lv_scr_act());
//    lv_obj_set_size(calendar, 240, 240);
//    lv_obj_align(calendar, LV_ALIGN_CENTER, 0, 0);
//    lv_obj_add_event_cb(calendar, event_handler, LV_EVENT_ALL, NULL);

//    lv_calendar_set_today_date(calendar, 2022, 01, 21);
//    lv_calendar_set_showed_date(calendar, 2022, 01);

//    /*Highlight a few days*/
//    static lv_calendar_date_t highlighted_days[3];       /*Only its pointer will be saved so should be static*/
//    highlighted_days[0].year = 2021;
//    highlighted_days[0].month = 02;
//    highlighted_days[0].day = 6;

//    highlighted_days[1].year = 2021;
//    highlighted_days[1].month = 02;
//    highlighted_days[1].day = 11;

//    highlighted_days[2].year = 2022;
//    highlighted_days[2].month = 02;
//    highlighted_days[2].day = 22;

//    lv_calendar_set_highlighted_dates(calendar, highlighted_days, 3);

//#if LV_USE_CALENDAR_HEADER_DROPDOWN
//    lv_calendar_header_dropdown_create(calendar);
//#elif LV_USE_CALENDAR_HEADER_ARROW
//    lv_calendar_header_arrow_create(calendar);
//#endif
//    lv_calendar_set_showed_date(calendar, 2022, 01);
//}

//static void ta_event_cb(lv_event_t * e)
//{
//    lv_event_code_t code = lv_event_get_code(e);
//    lv_obj_t * ta = lv_event_get_target(e);
//    lv_obj_t * kb = lv_event_get_user_data(e);
//    if(code == LV_EVENT_FOCUSED) {
//        lv_keyboard_set_textarea(kb, ta);
//        lv_obj_clear_flag(kb, LV_OBJ_FLAG_HIDDEN);
//    }

//    if(code == LV_EVENT_DEFOCUSED) {
//        lv_keyboard_set_textarea(kb, NULL);
//        lv_obj_add_flag(kb, LV_OBJ_FLAG_HIDDEN);
//    }
//}

//void lv_example_test(void)
//{
//		static lv_style_t style_btn_red;
//		lv_style_init(&style_btn_red);
//		lv_style_set_bg_color(&style_btn_red, lv_palette_main(LV_PALETTE_RED));
//		lv_style_set_bg_opa(&style_btn_red, LV_OPA_COVER);
//    /*Create a keyboard to use it with an of the text areas*/
//    lv_obj_t *kb = lv_keyboard_create(lv_scr_act());
//		//lv_obj_add_style(kb, &style_btn_red, LV_STATE_PRESSED); 
//    /*Create a text area. The keyboard will write here*/
//    lv_obj_t * ta;
//    ta = lv_textarea_create(lv_scr_act());
//    lv_obj_align(ta, LV_ALIGN_TOP_LEFT, 10, 10);
//    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
//    lv_textarea_set_placeholder_text(ta, "text1");
//	lv_obj_add_style(ta, &style_btn_red, LV_PART_SCROLLBAR | LV_STATE_SCROLLED ); 
//    lv_obj_set_size(ta, 140, 80);

//    ta = lv_textarea_create(lv_scr_act());
//    lv_obj_align(ta, LV_ALIGN_TOP_RIGHT, -10, 10);
//    lv_obj_add_event_cb(ta, ta_event_cb, LV_EVENT_ALL, kb);
//		lv_textarea_set_placeholder_text(ta, "text2");
//	lv_obj_add_style(ta, &style_btn_red, LV_PART_SCROLLBAR | LV_STATE_SCROLLED );
//    lv_obj_set_size(ta, 140, 80);

//    lv_keyboard_set_textarea(kb, ta);
//}


void lv_example_test(void)
{
    /*Create a LED and switch it OFF*/
    lv_obj_t * led1  = lv_led_create(lv_scr_act());
    lv_obj_align(led1, LV_ALIGN_CENTER, -80, 0);
    lv_led_off(led1);

    /*Copy the previous LED and set a brightness*/
    lv_obj_t * led2  = lv_led_create(lv_scr_act());
    lv_obj_align(led2, LV_ALIGN_CENTER, 0, 0);
    lv_led_set_brightness(led2, 250);
    lv_led_set_color(led2, lv_palette_main(LV_PALETTE_RED));
	  lv_led_on(led2);

   /*Copy the previous LED and switch it ON*/
    lv_obj_t * led3  = lv_led_create(lv_scr_act());
    lv_obj_align(led3, LV_ALIGN_CENTER, 80, 0);
   lv_led_off(led3);
}



