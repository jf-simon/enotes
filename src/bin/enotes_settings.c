#ifdef HAVE_CONFIG_H
# include "config.h"
#else
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT
#endif


#include "enotes.h"
int settings_on;
double step;


Evas_Object *settings_win = NULL;
static void
_close_settings(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   settings_on = 0;
	evas_object_del(settings_win);
	settings_win = NULL;
}
/*
static void
_save_and_check_user_data(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   Evas_Object *en_server = evas_object_data_get(data, "en_server");
   Evas_Object *en_password = evas_object_data_get(data, "en_password");
   Evas_Object *en_user = evas_object_data_get(data, "en_user");
   
   server_url = elm_object_text_get(en_server);
   password = elm_object_text_get(en_password);
   user = elm_object_text_get(en_user);
   
   printf("Server URL: %s\n", server_url);
}*/


static double
step_size_calculate(double min, double max)
{
   double step = 0.0;
   int steps = 0;

   steps = max - min;
   if (steps) step = (10.0 / steps);
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
_user_data_changed(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   char buf[PATH_MAX];
   
	snprintf(buf, sizeof(buf), "%s/images/online_notchecked.png", PACKAGE_DATA_DIR);
	elm_image_file_set(data, buf, NULL);
}

static void
_m_check_callback(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{	
   Eina_Bool state = elm_check_state_get(obj);
	if(state != EINA_TRUE)
		elm_object_disabled_set(data, EINA_FALSE);
	else
		elm_object_disabled_set(data, EINA_TRUE);
}

void
_open_settings(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, const char *em EINA_UNUSED, const char *src EINA_UNUSED)
{
    Evas_Object *win;
	 
    if(settings_on != 1)
    {
         win = elm_win_util_standard_add("enotes-settings", "Enote SETTINGS");
			
			elm_win_title_set(win, gettext("eNotes Settings"));
// 			elm_win_focus_highlight_enabled_set(win, EINA_TRUE);
// 			elm_win_borderless_set(win, EINA_TRUE);
// 			elm_win_alpha_set(win, EINA_TRUE);
// 			elm_win_autodel_set(win, EINA_TRUE);
		
			Evas_Object *bx, *lb, *en_server, *en_user, *en_password, *tb, *sl_refresh, *bn, *o, *list, *ic, *m_check;
			
			bx = elm_box_add(win);
			evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
			evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
         
            lb = elm_label_add(bx);
            elm_object_text_set(lb, "Caldav Settings");
            evas_object_show(lb);
            elm_box_pack_end(bx, lb);
            
            tb = elm_table_add(bx);
            elm_table_padding_set(tb, 5, 0);
            evas_object_size_hint_weight_set(tb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, 0);
            evas_object_show(tb);
            
            lb = elm_label_add(bx);
            elm_object_text_set(lb, "Server URL:");
            evas_object_size_hint_align_set(lb, 1, 0);
            evas_object_show(lb);
            elm_table_pack(tb, lb, 0, 0, 1, 1);
            evas_object_show(lb);
            
/// IMAGE
            char buf[PATH_MAX];
            ic = elm_icon_add(bx);
				snprintf(buf, sizeof(buf), "%s/images/online_notchecked.png", PACKAGE_DATA_DIR);
            evas_object_size_hint_min_set(ic, ELM_SCALE_SIZE(100), ELM_SCALE_SIZE(71));	
				elm_image_file_set(ic, buf, NULL);
				evas_object_size_hint_weight_set(ic, 0, EVAS_HINT_EXPAND);
				evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
            evas_object_data_set(bx, "ic", ic);
            evas_object_show(ic);
            elm_table_pack(tb, ic, 2, 0, 1, 3);
            
            
/// IMAGE
            
            en_server = elm_entry_add(bx);
            elm_config_context_menu_disabled_set(EINA_FALSE);
            elm_object_text_set(en_server, server_url);
            elm_object_part_text_set(en_server, "elm.guide", "Enter caldav server url with calendar name");
            elm_entry_editable_set(en_server, EINA_TRUE);
            elm_entry_single_line_set(en_server, EINA_TRUE);
            evas_object_size_hint_weight_set(en_server, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(en_server, EVAS_HINT_FILL, 0);
            evas_object_data_set(bx, "en_server", en_server);
            elm_table_pack(tb, en_server, 1, 0, 1, 1);
            evas_object_show(en_server);
            
            
            lb = elm_label_add(bx);
            elm_object_text_set(lb, "User:");
            evas_object_size_hint_align_set(lb, 1, 0);
            evas_object_show(lb);
            elm_table_pack(tb, lb, 0, 1, 1, 1);
            evas_object_show(lb);
            
            en_user = elm_entry_add(bx);
            elm_config_context_menu_disabled_set(EINA_FALSE);
            elm_object_text_set(en_user, user);
            elm_object_part_text_set(en_user, "elm.guide", "Enter username");
            elm_entry_editable_set(en_user, EINA_TRUE);
            elm_entry_single_line_set(en_user, EINA_TRUE);
            evas_object_size_hint_weight_set(en_user, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(en_user, EVAS_HINT_FILL, 0);
            evas_object_data_set(bx, "en_user", en_user);
            elm_table_pack(tb, en_user, 1, 1, 1, 1);
            evas_object_show(en_user);
            
            
            lb = elm_label_add(bx);
            elm_object_text_set(lb, "Password:");
            evas_object_size_hint_align_set(lb, 1, 0);
            evas_object_show(lb);
            elm_table_pack(tb, lb, 0, 2, 1, 1);
            
            en_password = elm_entry_add(bx);
            elm_entry_password_set (en_password, EINA_TRUE);
            elm_config_context_menu_disabled_set(EINA_FALSE);
            elm_object_text_set(en_password, password);
            elm_object_part_text_set(en_password, "elm.guide", "Enter Password");
            elm_entry_editable_set(en_password, EINA_TRUE);
            elm_entry_single_line_set(en_password, EINA_TRUE);
            evas_object_size_hint_weight_set(en_password, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(en_password, EVAS_HINT_FILL, 0);
            evas_object_data_set(bx, "en_password", en_password);
            elm_table_pack(tb, en_password, 1, 2, 1, 1);
            evas_object_show(en_password);
            

            lb = elm_label_add(bx);
            elm_object_text_set(lb, "Refresh intervall:");
            evas_object_size_hint_align_set(lb, 1, 0);
            evas_object_show(lb);
            elm_table_pack(tb, lb, 0, 3, 1, 1);
            evas_object_show(lb);
            
            sl_refresh = elm_slider_add(bx);
            evas_object_size_hint_weight_set(sl_refresh, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(sl_refresh, EVAS_HINT_FILL, 0);
            elm_slider_unit_format_set(sl_refresh, "%1.0f min");
            elm_slider_indicator_format_set(sl_refresh, "%1.0f");
            elm_slider_min_max_set(sl_refresh, 1, 60);
//             elm_slider_value_set(sl_refresh, ci_refresh);
            elm_slider_value_set(sl_refresh, 15);
            step = step_size_calculate(1, 60);
            elm_slider_step_set(sl_refresh, step);
            evas_object_show(sl_refresh);
//             evas_object_data_set(tb, "sl_refresh", sl_refresh);
            elm_table_pack(tb, sl_refresh, 1, 3, 1, 1);
            
            m_check = elm_check_add(bx);
            evas_object_size_hint_weight_set(m_check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(m_check, EVAS_HINT_FILL, 0);
            elm_object_text_set(m_check, "manual sync [F11]");
//             elm_slider_value_set(m_check, m_check);
            evas_object_show(m_check);
            evas_object_data_set(tb, "m_check", m_check);
            evas_object_smart_callback_add(m_check, "changed", _m_check_callback, sl_refresh); 
            elm_table_pack(tb, m_check, 1, 4, 1, 1);
			
            elm_box_pack_end(bx, tb);
            
            bn = elm_button_add(bx);
            evas_object_size_hint_weight_set(bn, EVAS_HINT_EXPAND, 0);
            evas_object_size_hint_align_set(bn, EVAS_HINT_FILL, 0);
            elm_object_text_set(bn, gettext("save and check user data"));
            evas_object_data_set(bx, "bn", bn);
            evas_object_show(bn);

            elm_box_pack_end(bx, bn);

            
    evas_object_smart_callback_add(en_server, "changed", _user_data_changed, ic); 
    evas_object_smart_callback_add(en_user, "changed", _user_data_changed, ic); 
    evas_object_smart_callback_add(en_password, "changed", _user_data_changed, ic); 
         evas_object_smart_callback_add(bn, "clicked", _save_and_check_user_data, bx);
            
         evas_object_smart_callback_add(win, "delete,request", _close_settings, win);

            o = elm_separator_add(bx);
            evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(o, EVAS_HINT_FILL, 0);
            elm_separator_horizontal_set(o, EINA_TRUE);
            evas_object_show(o);
            elm_box_pack_end(bx, o);

            lb = elm_label_add(bx);
            elm_object_text_set(lb, "Select Categories to show:");
            evas_object_show(lb);
            elm_box_pack_end(bx, lb);
            
            fill_cat_list();
            
            Eina_List *l;
            const char *cat_name;
               
            printf("SETTINGS CAT LIST %i\n", eina_list_count(cat_list));
            
            list = elm_list_add(bx);
            elm_list_multi_select_set(list, EINA_TRUE);
            elm_list_multi_select_mode_set(list, ELM_OBJECT_MULTI_SELECT_MODE_DEFAULT);
            evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
            evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);

            EINA_LIST_FOREACH(cat_list, l, cat_name)
            {
               elm_list_item_append(list, (char *)cat_name, NULL, NULL, NULL, NULL);
            }

            elm_list_go(list);

            evas_object_show(list);
            elm_box_pack_end(bx, list);

            
            
			evas_object_show(bx);
         
         elm_win_resize_object_add(win, bx);
// 			evas_object_resize(win, 390, 500);
			elm_win_center(win, EINA_TRUE, EINA_TRUE);
			evas_object_show(win);
			elm_object_focus_set(win, EINA_TRUE);
			settings_win = win;
			settings_on = 1;
    }
    else
    {
         settings_on = 0;
			evas_object_del(settings_win);
			settings_win = NULL;
    }
}
