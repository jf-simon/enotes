#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT
#endif

#include "enotes.h"
int settings_on;
double step;

Evas_Object* settings_win = NULL;
static void
_close_settings(void* data,
                Evas_Object* obj EINA_UNUSED,
                void* event_info EINA_UNUSED)
{
  settings_on = 0;
  evas_object_del(settings_win);
  settings_win = NULL;
}


static double
step_size_calculate(double min, double max)
{
  double step = 0.0;
  int steps = 0;

  steps = max - min;
  if (steps)
    step = (10.0 / steps);
  return step;
}
/*
void
category_parse()
{
        char category_buf[PATH_MAX];
        char **arr;
        int i;
        printf("CAT %s\n", cat_settings);
        arr = eina_str_split(cat_settings, ",", 0);

        if(arr == NULL)
                return;
        eina_list_free(cat_list);

   for (i = 0; arr[i]; i++)
        {
// 		remove_space((char*)arr[i]); // TODO make it working
                snprintf(category_buf, sizeof(category_buf), "%s", arr[i]);
                if(strcmp(arr[i], ""))
                        cat_list = eina_list_append(cat_list, category_buf);
        }

// 	return eina_strbuf_string_get(tmp);
}*/

static void
_toggle_border(void* data, Evas_Object* obj, void* event_info EINA_UNUSED)
{
   
   Eina_Bool state = elm_check_state_get(obj);

     Eina_List* l;
  Eina_List* list_values;
  EINA_LIST_FOREACH(enotes_all_objects_list,
                    l,
                    list_values) // LISTE DER OBJEKTE DURCHGEHEN
  {
   
     Evas_Object* win = eina_list_nth(list_values, 0);
      if (state == EINA_TRUE) {         
         elm_win_borderless_set(win, EINA_FALSE);
      } else {
         elm_win_borderless_set(win, EINA_TRUE);
      }
  }
  ci_border_enabled = state;
  
}


static void
_toggle_sync(void* data, Evas_Object* obj, void* event_info EINA_UNUSED)
{
   Evas_Object *bx = data;
	Evas_Object *ic = evas_object_data_get(bx, "ic");
	Evas_Object *en_user = evas_object_data_get(bx, "en_user");
	Evas_Object *en_password = evas_object_data_get(bx, "en_password");
	Evas_Object *en_calendar = evas_object_data_get(bx, "en_calendar");
	Evas_Object *en_server = evas_object_data_get(bx, "en_server");
	Evas_Object *sl_refresh = evas_object_data_get(bx, "sl_refresh");
	Evas_Object *m_check = evas_object_data_get(bx, "m_check");   
	Evas_Object *lb = evas_object_data_get(bx, "lb");
	Evas_Object *check_sync_enabled = evas_object_data_get(bx, "check_sync_enabled");
   
   
   Eina_Bool state = elm_check_state_get(check_sync_enabled);

  if (state == EINA_TRUE) {
    elm_object_disabled_set(en_user, EINA_FALSE);
    elm_object_disabled_set(en_password, EINA_FALSE);
    elm_object_disabled_set(en_calendar, EINA_FALSE);
    elm_object_disabled_set(en_server, EINA_FALSE);
    elm_object_disabled_set(m_check, EINA_FALSE);
    elm_object_disabled_set(sl_refresh, EINA_FALSE);
//     elm_object_disabled_set(lb, EINA_FALSE);
    // TODO: disable sync timer
  } else {
    elm_object_disabled_set(en_user, EINA_TRUE);
    elm_object_disabled_set(en_password, EINA_TRUE);
    elm_object_disabled_set(en_calendar, EINA_TRUE);
    elm_object_disabled_set(en_server, EINA_TRUE);
    elm_object_disabled_set(m_check, EINA_TRUE);
    elm_object_disabled_set(sl_refresh, EINA_TRUE);
//     elm_object_disabled_set(lb, EINA_TRUE);
    // TODO: enable sync timer
  }
  ci_sync_enabled = state;
}

static void
_user_data_changed(void* data, Evas_Object* obj, void* event_info EINA_UNUSED)
{
   Evas_Object *bx = data;
	Evas_Object *ic = evas_object_data_get(bx, "ic");
	Evas_Object *en_user = evas_object_data_get(bx, "en_user");
	Evas_Object *en_password = evas_object_data_get(bx, "en_password");
	Evas_Object *en_calendar = evas_object_data_get(bx, "en_calendar");
	Evas_Object *en_server = evas_object_data_get(bx, "en_server");
   
   char buf[PATH_MAX];

//    if(online_check == EINA_TRUE)
//    {  
//       snprintf(buf, sizeof(buf), "%s/images/online_valid.png", PACKAGE_DATA_DIR);
//       elm_image_file_set(ic, buf, NULL);
//    }else
//    {
   
   snprintf(buf, sizeof(buf), "%s/images/online_notchecked.png", PACKAGE_DATA_DIR);
      if (strcmp(user_name, elm_object_text_get(en_user)))
      {
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_FALSE;
         printf("INHALT CHANGED\n");
      }else if (strcmp(password, elm_object_text_get(en_password)))
      {
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_FALSE;
         printf("INHALT CHANGED\n");
      }else if (strcmp(calendar_name, elm_object_text_get(en_calendar)))
      {
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_FALSE;
         printf("INHALT CHANGED\n");
      }else if (strcmp(server_url, elm_object_text_get(en_server)))
      {
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_FALSE;
         printf("INHALT CHANGED\n");
      }
      else
      {
         snprintf(buf, sizeof(buf), "%s/images/online_valid.png", PACKAGE_DATA_DIR);
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_TRUE;
      }
//    }
}


static void
_m_check_callback(void* data, Evas_Object* obj, void* event_info EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* ly = eina_list_nth(list_values, 0);
  Evas_Object* sl_refresh = eina_list_nth(list_values, 1);

  Evas_Object* edje_obj = elm_layout_edje_get(ly);

  Eina_Bool state = elm_check_state_get(obj);

  if (state != EINA_TRUE) {
    elm_object_disabled_set(sl_refresh, EINA_FALSE);
    // TODO: disable sync timer
    edje_object_signal_emit(edje_obj, "hide_status", "");
  } else {
    elm_object_disabled_set(sl_refresh, EINA_TRUE);
    // TODO: enable sync timer
    edje_object_signal_emit(edje_obj, "show_status", "");
  }
  ci_m_check = state;
  edje_object_signal_emit(edje_obj, "show_status", "");
}


static void
_systray_callback(void* data, Evas_Object* obj, void* event_info EINA_UNUSED)
{
  Eina_Bool state = elm_check_state_get(obj);

  if (state == EINA_TRUE) {
    elm_obj_systray_status_set(item, 0);
  } else {
    elm_obj_systray_status_set(item, 1);
  }
  ci_systray = state;
}


static void
_config_show_categories(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;
	
// 	Evas_Object *general_frame = evas_object_data_get(tb, "general_frame");
	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");

//    evas_object_hide(general_frame);
   evas_object_show(categories_frame);
   evas_object_hide(advanced_frame);
	evas_object_hide(help_frame);
	evas_object_hide(sync_frame);
	printf("SHOW CATEGORIES\n");
}


static void
_config_show_general(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;
   
// 	Evas_Object *general_frame = evas_object_data_get(tb, "general_frame");
	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");
	
//    evas_object_show(general_frame);
   evas_object_hide(categories_frame);
   evas_object_hide(advanced_frame);
	evas_object_hide(help_frame);
	evas_object_hide(sync_frame);
   
	printf("SHOW GENERAL\n");
}


static void
_config_show_advanced(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;
   
// 	Evas_Object *general_frame = evas_object_data_get(tb, "general_frame");
	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");
	
//    evas_object_hide(general_frame);
   evas_object_hide(categories_frame);
   evas_object_show(advanced_frame);
	evas_object_hide(help_frame);
	evas_object_hide(sync_frame);
	printf("SHOW ADVANCED\n");
}


static void
_config_show_help(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;

// 	Evas_Object *general_frame = evas_object_data_get(tb, "general_frame");
	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");
	
//    evas_object_hide(general_frame);
   evas_object_hide(categories_frame);
   evas_object_hide(advanced_frame);
	evas_object_show(help_frame);
	evas_object_hide(sync_frame);
	
	printf("SHOW HELP\n");
}


static void
_config_show_sync(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;

// 	Evas_Object *general_frame = evas_object_data_get(tb, "general_frame");
	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");
	
//    evas_object_hide(general_frame);
   evas_object_hide(categories_frame);
   evas_object_hide(advanced_frame);
	evas_object_hide(help_frame);
	evas_object_show(sync_frame);

	printf("SHOW SYNC\n");
}


static void
_it_clicked_cb(void *data EINA_UNUSED, Evas_Object *li,
                 void *event_info EINA_UNUSED)
{
   Eina_List *l;
   cat_list = elm_list_selected_items_get(li);
   Elm_Object_Item *lit;
   
   EINA_LIST_FOREACH(cat_list, l, lit)
   {
   printf("Item clicked. %s is selected\n", elm_object_item_text_get(lit));
   //TODO: ABGLEICH MIT NOTE_LIST nach Übereinstimmung der CATEGORIES
   elm_list_item_selected_get(lit);
   }
}

static void
_it_clicked_select_all_cb(void *data, Evas_Object *li EINA_UNUSED,
                 void *event_info EINA_UNUSED)
{
   Eina_List *l;
   cat_list = elm_list_items_get(data);
   Elm_Object_Item *lit;
   
   EINA_LIST_FOREACH(cat_list, l, lit)
   {
      elm_list_item_selected_set(lit, EINA_TRUE);
   }
}


void
_open_settings(void* data,
               Evas_Object* obj EINA_UNUSED,
               const char* em EINA_UNUSED,
               const char* src EINA_UNUSED)
{
  Evas_Object* win;
  Evas_Object *bx, *lb, *en_server, *en_user, *en_password, *tb, *tb_settings, *sl_refresh, *en_calendar, *lbl;
  Evas_Object *general_frame, *categories_frame, *advanced_frame, *help_frame, *sync_frame, *bn, *o, *ic, *m_check, *en_help, *systray_check, *check_sync_enabled, *check_border_enabled, *bt_all;

	// List Objects
	Elm_Object_Item *list1;
	Evas_Object *it;
  Eina_List* list_values = data;
//   Evas_Object* ly = eina_list_nth(list_values, 5);

  Eina_List* list_values_check = NULL;

  if (settings_on != 1) {
    win = elm_win_util_standard_add("enotes-settings", "Enote SETTINGS");

    elm_win_title_set(win, gettext("eNotes Settings"));
    // 			elm_win_focus_highlight_enabled_set(win, EINA_TRUE);
    // 			elm_win_borderless_set(win, EINA_TRUE);
    // 			elm_win_alpha_set(win, EINA_TRUE);
    // 			elm_win_autodel_set(win, EINA_TRUE);


   evas_object_smart_callback_add(win, "delete,request", _close_settings, win);
    
	tb_settings = elm_table_add(win);
	evas_object_size_hint_weight_set(tb_settings, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(tb_settings);
/*	
	lbl = elm_label_add(tb_settings);
   evas_object_size_hint_weight_set(lbl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(lbl, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_style_set(lbl, "marker");
   char buf4[256];
	snprintf(buf4, sizeof(buf4), "<b>Enotes Configuration [%s]", NULL);
   elm_object_text_set(lbl, buf4);
   elm_table_pack(tb_settings, lbl, 0, 0, 2, 1);
   evas_object_show(lbl);*/
	
	list1 = elm_list_add(tb_settings);
	evas_object_size_hint_weight_set(list1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(list1, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb_settings, list1, 0, 1, 1, 1);
	
   elm_list_select_mode_set(list1, ELM_OBJECT_SELECT_MODE_ALWAYS);
	
   elm_scroller_content_min_limit(list1, 1, 1);
	
//    it = elm_list_item_append(list1, "General", NULL, NULL,
//                              _config_show_general, tb_settings);
	
   it = elm_list_item_append(list1, "Categories", NULL, NULL,
                             _config_show_categories, tb_settings);
	
   it = elm_list_item_append(list1, "Sync", NULL, NULL,
                             _config_show_sync, tb_settings);
   elm_list_item_selected_set(it, 1);

   it = elm_list_item_append(list1, "Advanced", NULL, NULL,
                             _config_show_advanced, tb_settings);
	
	it = elm_list_item_append(list1, "Help", NULL, NULL,
                             _config_show_help, tb_settings);
   elm_list_go(list1);
   evas_object_show(list1);
   
   
/// SYNC FRAME ///
	sync_frame = elm_frame_add(win);
   elm_object_text_set(sync_frame, "Sync");
	evas_object_size_hint_weight_set(sync_frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(sync_frame, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb_settings, sync_frame, 1, 1, 1, 1);

                  bx = elm_box_add(sync_frame);
                  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);

                  check_sync_enabled = elm_check_add(bx);
                  evas_object_size_hint_weight_set(
                     check_sync_enabled, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(check_sync_enabled, EVAS_HINT_FILL, 0);
                  elm_object_text_set(check_sync_enabled, "enable synchronization");
                  elm_check_state_set(check_sync_enabled, ci_sync_enabled);
                  evas_object_show(check_sync_enabled);
                  evas_object_data_set(bx, "check_sync_enabled", check_sync_enabled);
                  elm_box_pack_end(bx, check_sync_enabled);
                  
                  tb = elm_table_add(bx);
                  elm_table_padding_set(tb, 5, 0);
                  evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, 0);
                  evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, 0);
                  evas_object_show(tb);

                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "Server URL:");
                  evas_object_size_hint_align_set(lb, 1, 0);
                  evas_object_show(lb);
                  elm_table_pack(tb, lb, 0, 0, 1, 1);
                  evas_object_data_set(bx, "lb", lb);
                  evas_object_show(lb);

                  /// IMAGE
                  char buf[PATH_MAX];
                  ic = elm_icon_add(bx);
//                   snprintf(
//                      buf, sizeof(buf), "%s/images/online_valid.png", PACKAGE_DATA_DIR);
                  evas_object_size_hint_min_set(ic, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(71));
//                   elm_image_file_set(ic, buf, NULL);
                  evas_object_size_hint_weight_set(ic, 0, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
                  evas_object_data_set(bx, "ic", ic);
                  evas_object_show(ic);
                  elm_table_pack(tb, ic, 2, 0, 1, 5);

                  /// IMAGE

                  en_server = elm_entry_add(bx);
                  elm_config_context_menu_disabled_set(EINA_FALSE);
                  elm_object_text_set(en_server, server_url);
                  elm_object_part_text_set(
                     en_server, "elm.guide", "Enter caldav server url with calendar name");
                  elm_entry_editable_set(en_server, EINA_TRUE);
                  elm_entry_single_line_set(en_server, EINA_TRUE);
                  evas_object_size_hint_weight_set(
                     en_server, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(en_server, EVAS_HINT_FILL, 0);
                  evas_object_data_set(bx, "en_server", en_server);
                  elm_table_pack(tb, en_server, 1, 0, 1, 1);
                  evas_object_show(en_server);
                  
                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "Calendar Name:");
                  evas_object_size_hint_align_set(lb, 1, 0);
                  evas_object_show(lb);
                  elm_table_pack(tb, lb, 0, 1, 1, 1);
                  evas_object_show(lb);

                  en_calendar = elm_entry_add(bx);
                  elm_config_context_menu_disabled_set(EINA_FALSE);
                  elm_object_text_set(en_calendar, calendar_name);
                  elm_object_part_text_set(en_calendar, "elm.guide", "Enter Calendar Name");
                  elm_entry_editable_set(en_calendar, EINA_TRUE);
                  elm_entry_single_line_set(en_calendar, EINA_TRUE);
                  evas_object_size_hint_weight_set(
                     en_calendar, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(en_calendar, EVAS_HINT_FILL, 0);
                  evas_object_data_set(bx, "en_calendar", en_calendar);
                  elm_table_pack(tb, en_calendar, 1, 1, 1, 1);
                  evas_object_show(en_calendar);

                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "User:");
                  evas_object_size_hint_align_set(lb, 1, 0);
                  evas_object_show(lb);
                  elm_table_pack(tb, lb, 0, 2, 1, 1);
                  evas_object_show(lb);

                  en_user = elm_entry_add(bx);
                  elm_config_context_menu_disabled_set(EINA_FALSE);
                  elm_object_text_set(en_user, user_name);
                  elm_object_part_text_set(en_user, "elm.guide", "Enter username");
                  elm_entry_editable_set(en_user, EINA_TRUE);
                  elm_entry_single_line_set(en_user, EINA_TRUE);
                  evas_object_size_hint_weight_set(
                     en_user, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(en_user, EVAS_HINT_FILL, 0);
                  evas_object_data_set(bx, "en_user", en_user);
                  elm_table_pack(tb, en_user, 1, 2, 1, 1);
                  evas_object_show(en_user);

                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "Password:");
                  evas_object_size_hint_align_set(lb, 1, 0);
                  evas_object_show(lb);
                  elm_table_pack(tb, lb, 0, 3, 1, 1);

                  en_password = elm_entry_add(bx);
                  elm_entry_password_set(en_password, EINA_TRUE);
                  elm_config_context_menu_disabled_set(EINA_FALSE);
                  elm_object_text_set(en_password, password);
                  elm_object_part_text_set(en_password, "elm.guide", "Enter Password");
                  elm_entry_editable_set(en_password, EINA_TRUE);
                  elm_entry_single_line_set(en_password, EINA_TRUE);
                  evas_object_size_hint_weight_set(
                     en_password, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(en_password, EVAS_HINT_FILL, 0);
                  evas_object_data_set(bx, "en_password", en_password);
                  elm_table_pack(tb, en_password, 1, 3, 1, 1);
                  evas_object_show(en_password);

                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "Refresh intervall:");
                  evas_object_size_hint_align_set(lb, 1, 0);
                  evas_object_show(lb);
                  elm_table_pack(tb, lb, 0, 4, 1, 1);
                  evas_object_show(lb);

                  sl_refresh = elm_slider_add(bx);
                  elm_object_disabled_set(sl_refresh, EINA_TRUE);
                  evas_object_size_hint_weight_set(
                     sl_refresh, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(sl_refresh, EVAS_HINT_FILL, 0);
                  elm_slider_unit_format_set(sl_refresh, "%1.0f min");
                  elm_slider_indicator_format_set(sl_refresh, "%1.0f");
                  elm_slider_min_max_set(sl_refresh, 1, 60);
                  elm_slider_value_set(sl_refresh, 15);
                  step = step_size_calculate(1, 60);
                  elm_slider_step_set(sl_refresh, step);
                  evas_object_show(sl_refresh);
                  evas_object_data_set(bx, "sl_refresh", sl_refresh);
                  elm_table_pack(tb, sl_refresh, 1, 4, 1, 1);

                  list_values_check =
                     eina_list_append(list_values_check, eina_list_nth(list_values, 5));
                  list_values_check = eina_list_append(list_values_check, sl_refresh);

                  m_check = elm_check_add(bx);
                  evas_object_size_hint_weight_set(
                     m_check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(m_check, EVAS_HINT_FILL, 0);
                  elm_object_text_set(m_check, "manual sync [F11]");
                  elm_check_state_set(m_check, ci_m_check);
                  evas_object_show(m_check);
                  evas_object_data_set(bx, "m_check", m_check);
                  elm_table_pack(tb, m_check, 1, 5, 1, 1);

                  elm_box_pack_end(bx, tb);

                  bn = elm_button_add(bx);
                  evas_object_size_hint_weight_set(bn, EVAS_HINT_EXPAND, 0);
                  evas_object_size_hint_align_set(bn, EVAS_HINT_FILL, 0);
                  elm_object_text_set(bn, gettext("save and check user data"));
                  evas_object_data_set(bx, "bn", bn);
                  evas_object_show(bn);

                  elm_box_pack_end(bx, bn);
   
                  
                  evas_object_smart_callback_add(
                     en_server, "changed", _user_data_changed, bx);
                  evas_object_smart_callback_add(en_user, "changed", _user_data_changed, bx);
                  evas_object_smart_callback_add(en_password, "changed", _user_data_changed, bx);
                  evas_object_smart_callback_add(en_calendar, "changed", _user_data_changed, bx);
                  evas_object_smart_callback_add(check_sync_enabled, "changed", _toggle_sync, bx);
                  evas_object_smart_callback_add(m_check, "changed", _m_check_callback, list_values_check);
                  evas_object_smart_callback_add(bn, "clicked", _save_and_check_user_data, bx);
                  
                  _toggle_sync(bx, NULL, NULL);
                  _user_data_changed(bx, NULL, NULL);

   elm_object_content_set(sync_frame, bx);
	evas_object_data_set(tb_settings, "sync_frame", sync_frame);
/// SYNC FRAME END ///
   
/// CATEGORIES FRAME ///
	categories_frame = elm_frame_add(win);
   elm_object_text_set(categories_frame, "Categories");
	evas_object_size_hint_weight_set(categories_frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(categories_frame, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb_settings, categories_frame, 1, 1, 1, 1);

                  bx = elm_box_add(categories_frame);
                  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);

                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "Select Categories to show:");
                  evas_object_show(lb);
                  elm_box_pack_end(bx, lb);

                  Elm_Object_Item *li, *li1;
                  list = elm_list_add(bx);
                  elm_list_multi_select_set(list, EINA_TRUE);
                  elm_list_multi_select_mode_set(list, ELM_OBJECT_MULTI_SELECT_MODE_DEFAULT);
                  evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);

                  elm_list_mode_set(list, ELM_LIST_COMPRESS);
                  
                  elm_list_item_append(list, "Default", NULL, NULL, NULL, list);
                  li = elm_list_item_append(list, NULL, NULL, NULL, NULL, NULL);
                  elm_list_item_separator_set(li, EINA_TRUE);
                  elm_list_item_append(list, "Privat", NULL, NULL, NULL, list);
                  elm_list_item_append(list, "Work", NULL, NULL, NULL, list);
                  elm_list_item_append(list, "EJW FDS", NULL, NULL, NULL, list);
                  elm_list_item_append(list, "Pfalzgrafenweiler", NULL, NULL, NULL, list);
                  li1 = elm_list_item_append(list, "Enotes", NULL, NULL, NULL, list);
                  elm_list_go(list);
                  
                  evas_object_smart_callback_add(li1, "activated", _it_clicked_select_all_cb, list);
                  evas_object_show(list);
                  elm_box_pack_end(bx, list);
                  
                  bt_all = elm_button_add(bx);
                  elm_object_text_set(bt_all, "Select all");
                  evas_object_smart_callback_add(bt_all, "clicked", _it_clicked_select_all_cb, list);
                  evas_object_show(bt_all);
                  elm_box_pack_end(bx, bt_all);

                  evas_object_show(bx);

   elm_object_content_set(categories_frame, bx);
	evas_object_data_set(tb_settings, "categories_frame", categories_frame);
/// CATEGORIES FRAME END ///
   
/// HELP FRAME ///
	help_frame = elm_frame_add(win);
   elm_object_text_set(help_frame, "Help");
	evas_object_size_hint_weight_set(help_frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(help_frame, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb_settings, help_frame, 1, 1, 1, 1);

                  bx = elm_box_add(help_frame);
                  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);

                  en_help = elm_entry_add(bx);
                  elm_config_context_menu_disabled_set(EINA_FALSE);
                  elm_entry_scrollable_set(en_help, EINA_TRUE);
                  elm_object_text_set(en_help,  "Welcome to eNotes! <br><br>"
                                                "Type your text here, <br>"
                                                "i will save the note for you automaticly <br>"
                                                "when you leave the note with the mouse! <br><br>"
                                                "<item relsize=24x24 vsize=full href=done></item> click on "
                                                "&quot;tape&quot; to move the note<br>"
                                                "<item relsize=24x24 vsize=full href=done></item> click bottom right "
                                                "corner to resize<br>"
                                                "<item relsize=24x24 vsize=full href=done></item> On the left size you "
                                                "have a little menu, klick on the gray rect to open/close it<br>"
                                                "<item relsize=24x24 vsize=full href=done></item> Use Mouse Scolling + "
                                                "CTRL to decrease/increase the font. Or STRG+0 to use standard size<br>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;ESC&quot; "
                                                "Dismiss dialogs and colorselector<br>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F1&quot; "
                                                "Show/hide help<br>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F2&quot; Blur "
                                                "text<br>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F3&quot; adds "
                                                "<item relsize=24x24 vsize=full href=done></item><br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F4&quot; adds "
                                                "<item relsize=24x24 vsize=full href=important></item><br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F5&quot; "
                                                "creates a new note<br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F6&quot; "
                                                "delete note<br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F7&quot; set "
                                                "sticky<br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F8&quot; "
                                                "iconify note<br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F9&quot; "
                                                "toggle designs<br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;F10&quot; "
                                                "Write backup textfile<br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;ctrl + q&quot; "
                                                "quits enotes<br/>"
                                                "<item relsize=24x24 vsize=full href=done></item> &quot;crtl + m&quot; "
                                                "toggle menu<br/><br/>"
                                                "Have fun! <br><br>"
                                                "Download and updates:<br><a "
                                                "href=anc-02>https://github.com/jf-simon/enotes</a><br><br>"
                                                "Author: Simon Tischer [jf_simon on irc.freenode.net #e.de]");
                  elm_entry_editable_set(en_help, EINA_FALSE);
                  evas_object_size_hint_weight_set(
                     en_help, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(en_help, EVAS_HINT_FILL, EVAS_HINT_FILL);
                  

                  evas_object_show(en_help);
                  elm_box_pack_end(bx, en_help);

                  evas_object_show(bx);

   elm_object_content_set(help_frame, bx);
	evas_object_data_set(tb_settings, "help_frame", help_frame);
/// HELP FRAME END ///
   
/// ADVANCED FRAME ///
	advanced_frame = elm_frame_add(win);
   elm_object_text_set(advanced_frame, "Advanced");
	evas_object_size_hint_weight_set(advanced_frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(advanced_frame, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb_settings, advanced_frame, 1, 1, 1, 1);

                  bx = elm_box_add(advanced_frame);
                  evas_object_size_hint_weight_set(bx, 0, 0);
                  evas_object_size_hint_align_set(bx, 0, 0);

                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "Systray Icon");
                  evas_object_size_hint_weight_set(lb, 0, 0);
                  evas_object_size_hint_align_set(lb, 0, 0);
                  evas_object_show(lb);
                  elm_box_pack_end(bx, lb);
                  
                  systray_check = elm_check_add(bx);
                  evas_object_size_hint_weight_set(
                     m_check, 0, 0);
                  evas_object_size_hint_align_set(systray_check, 0, 0);
                  elm_object_text_set(systray_check, "Disable Systray Icon");
                  elm_check_state_set(systray_check, ci_systray);
                  evas_object_show(systray_check);
                  evas_object_data_set(tb, "systray_check", systray_check);
                  evas_object_smart_callback_add(
                     systray_check, "changed", _systray_callback, NULL);

                  evas_object_show(systray_check);
                  elm_box_pack_end(bx, systray_check);
                  
                  
                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "Note Look");
                  evas_object_size_hint_weight_set(lb, 0, 0);
                  evas_object_size_hint_align_set(lb, 0, 0);
                  evas_object_show(lb);
                  elm_box_pack_end(bx, lb);
                  
                  check_border_enabled = elm_check_add(bx);
                  evas_object_size_hint_weight_set(
                     m_check, 0, 0);
                  evas_object_size_hint_align_set(check_border_enabled, 0, 0);
                  elm_object_text_set(check_border_enabled, "Show Window Border");
                  elm_check_state_set(check_border_enabled, ci_systray);
                  evas_object_show(check_border_enabled);
                  evas_object_data_set(tb, "check_border_enabled", check_border_enabled);
                  evas_object_smart_callback_add(check_border_enabled, "changed", _toggle_border, NULL);

                  evas_object_show(check_border_enabled);
                  elm_box_pack_end(bx, check_border_enabled);                  

                  evas_object_show(bx);

   elm_object_content_set(advanced_frame, bx);
	evas_object_data_set(tb_settings, "advanced_frame", advanced_frame);
/// ADVANCED FRAME END ///

   
   
   
   
   
    elm_win_resize_object_add(win, tb_settings);
//     evas_object_resize(win, 390, 390);
    
	_config_show_sync(tb_settings, NULL, NULL);
   
    elm_win_center(win, EINA_TRUE, EINA_TRUE);
    evas_object_show(win);
    elm_object_focus_set(win, EINA_TRUE);
    settings_win = win;
    settings_on = 1;
  } else {
    settings_on = 0;
    evas_object_del(settings_win);
    settings_win = NULL;
  }
}
