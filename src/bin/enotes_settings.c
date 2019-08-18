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
  evas_object_hide(settings_win);
//   settings_win = NULL;
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
// 	Evas_Object *ic = evas_object_data_get(bx, "ic");
	Evas_Object *en_user = evas_object_data_get(bx, "en_user");
	Evas_Object *en_password = evas_object_data_get(bx, "en_password");
	Evas_Object *en_calendar = evas_object_data_get(bx, "en_calendar");
	Evas_Object *en_server = evas_object_data_get(bx, "en_server");
	Evas_Object *sl_refresh = evas_object_data_get(bx, "sl_refresh");
	Evas_Object *m_check = evas_object_data_get(bx, "m_check");   
// 	Evas_Object *lb = evas_object_data_get(bx, "lb");
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

   if(user_name != NULL && en_password != NULL && en_calendar != NULL && en_server != NULL)
   {
      if (strcmp(user_name, elm_object_text_get(en_user))) // TODO NULL CHECK
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
   }
   else
   {
      snprintf(buf, sizeof(buf), "%s/images/online_valid.png", PACKAGE_DATA_DIR);
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_FALSE;
   }
      
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
    elm_systray_status_set(item, 0);
  } else {
    elm_systray_status_set(item, 1);
  }
  ci_systray = state;
}


static void
_config_show_categories(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;
	
	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");

   evas_object_show(categories_frame);
   evas_object_hide(advanced_frame);
	evas_object_hide(help_frame);
	evas_object_hide(sync_frame);
}


static void
_config_show_advanced(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;
   
	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");
	
   evas_object_hide(categories_frame);
   evas_object_show(advanced_frame);
	evas_object_hide(help_frame);
	evas_object_hide(sync_frame);
}


static void
_config_show_help(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;

	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");
	
   evas_object_hide(categories_frame);
   evas_object_hide(advanced_frame);
	evas_object_show(help_frame);
	evas_object_hide(sync_frame);
}


static void
_config_show_sync(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
	Evas_Object *tb = data;

	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
   Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");
	
   evas_object_hide(categories_frame);
   evas_object_hide(advanced_frame);
	evas_object_hide(help_frame);
	evas_object_show(sync_frame);
}


void
catlist_to_catlisteet()
{
      Eina_List *l;
      const Eina_List *cat_list_new;
      cat_list_new = elm_list_items_get(list);
      Elm_Object_Item *lit;
      
      cat_list_settings = NULL;
printf("test1\n");
      EINA_LIST_FOREACH((Eina_List*) cat_list_new, l, lit)
      {
         My_Conf_Type_Cat* new;
         new = calloc(1, sizeof(My_Conf_Type_Cat));
         new->cat_name = elm_object_item_text_get(lit);
         new->cat_selected = elm_list_item_selected_get(lit);
         
         if(strcmp(elm_object_item_text_get(lit), "Default"))
         {
            cat_list_settings = eina_list_append(cat_list_settings, new);
         }
      }
}


void
_fill_list_to_notes() //put the settings cat list to each note settings
{
   Eina_List* list_values;
   Eina_List *l1;
   EINA_LIST_FOREACH(enotes_all_objects_list, l1, list_values) // LISTE DER OBJEKTE DURCHGEHEN
   {
      Evas_Object* list_notes = eina_list_nth(list_values, 9);
      Eina_List *tmp = NULL;

      Elm_Object_Item *lit, *lit_tmp, *it;
      
      Eina_List *l, *l_tmp;
      const Eina_List *cat_list_settings = NULL;
//       const Eina_List *cat_list_items = NULL;
      const Eina_List *cat_list_selected_items = NULL;
      
      cat_list_settings = elm_list_items_get(list);
      
//       cat_list_items = elm_list_items_get(list_notes);
      
      cat_list_selected_items = elm_list_selected_items_get(list_notes);


         EINA_LIST_FOREACH((Eina_List*)cat_list_selected_items, l_tmp, lit_tmp)
         {
            tmp = eina_list_append(tmp, elm_object_item_text_get(lit_tmp));
         }

      elm_list_clear(list_notes);

      EINA_LIST_FOREACH((Eina_List*)cat_list_settings, l, lit)
      {
            it = elm_list_item_append(list_notes, elm_object_item_text_get(lit), NULL, NULL, NULL, list_notes);

            const char *name;
            EINA_LIST_FOREACH(tmp, l_tmp, name)
            {
               if(strcmp(elm_object_item_text_get(lit), name) == 0)
               {
                  elm_list_item_selected_set(it, EINA_TRUE);
               }
            }
      }
      printf("\n\n");
      
      elm_list_go(list_notes);
   }
}


void
_it_clicked_cb(void *data, Evas_Object *li EINA_UNUSED,
                 void *event_info EINA_UNUSED) // SHOW/HIDE Notes based on aktive cat list items
{
   printf("CHANGE ON SETTINGS CAT LIST\n");
   
   Eina_List* list_values;
   Eina_List *l;
   Eina_List *l1, *l2;
   const Eina_List *cat_list_selected_settings = NULL;
   Elm_Object_Item *lit, *lit1;

   cat_list_selected_settings = elm_list_selected_items_get(data);


      EINA_LIST_FOREACH(enotes_all_objects_list, l, list_values) // LISTE DER OBJEKTE DURCHGEHEN
      {
         Evas_Object *win = eina_list_nth(list_values, 0);
         Evas_Object *cat_list = eina_list_nth(list_values, 9);
         const Eina_List *cat_list_selected_note = NULL;
         int z = 0;

            
            cat_list_selected_note = elm_list_selected_items_get(cat_list);
            

            if(eina_list_count((Eina_List*)elm_list_items_get(list)) == 0)
            {
               evas_object_show(win);
               continue;
            }
            
            if((eina_list_count((Eina_List*)cat_list_selected_note) == 0) && (eina_list_count((Eina_List*)cat_list_selected_settings) != 0)) // show if note has no Categorie
            {
               evas_object_show(win);
               continue;
            }
            
            if(eina_list_count((Eina_List*)cat_list_selected_settings) == 0)
            {
               evas_object_hide(win);
               continue;
            }
            

                  EINA_LIST_FOREACH((Eina_List*)cat_list_selected_settings, l1, lit)
                  {
                                    EINA_LIST_FOREACH((Eina_List*)cat_list_selected_note, l2, lit1)
                                    {
                                       if((strcmp(elm_object_item_text_get(lit), elm_object_item_text_get(lit1)) == 0)
                                       {
                                          evas_object_show(win);
                                          z = 1;
                                          continue;
                                       }
                                    }
                  }  
            
            if(z != 1)
            evas_object_hide(win);
      }
      
      if(eina_list_count((Eina_List*)cat_list_selected_settings) == 0) // select first list item to avoid hiding all notes -> no way to come back to enotes
      {
         elm_list_item_selected_set(elm_list_first_item_get(data), EINA_TRUE);
      }
}


static void
_showall_clicked_cb(void *data EINA_UNUSED, Evas_Object *li EINA_UNUSED,
                 void *event_info EINA_UNUSED)
{
   Eina_List* list_values;
   Eina_List *l;
               
      EINA_LIST_FOREACH(enotes_all_objects_list, l, list_values) // LISTE DER OBJEKTE DURCHGEHEN
      {
         printf("SHOW ALL\n");
         Evas_Object *win = eina_list_nth(list_values, 0);
         evas_object_show(win);
      }
      
      
}


static void
_it_clicked_select_all_cb(void *data, Evas_Object *li EINA_UNUSED,
                 void *event_info EINA_UNUSED)
{
   Eina_List *l;
   const Eina_List *cat_list_all = NULL;
   cat_list_all = elm_list_items_get(data);
   Elm_Object_Item *lit = NULL;
   EINA_LIST_FOREACH((Eina_List*)cat_list_all, l, lit)
   {
      if(strcmp(elm_object_item_text_get(lit), "Show all"))
         elm_list_item_selected_set(lit, EINA_TRUE);
   }
   elm_list_go(list);
}


static void
_it_clicked_add_cb(void *data, Evas_Object *li EINA_UNUSED,
                 void *event_info EINA_UNUSED)
{
      printf("ADD: %s", elm_object_text_get(data));
   if(strcmp(elm_object_text_get(data), "Show all"))
   {
      elm_list_item_append(list, elm_object_text_get(data), NULL, NULL, NULL, list);
      elm_list_go(list);
      _fill_list_to_notes();
   }
}


static void
_it_clicked_del_cb(void *data, Evas_Object *li EINA_UNUSED,
                 void *event_info EINA_UNUSED)
{
   Eina_List *l;
   const Eina_List *cat_list_del = NULL;
   cat_list_del = elm_list_selected_items_get(data);
   Elm_Object_Item *lit = NULL;
   EINA_LIST_FOREACH((Eina_List*)cat_list_del, l, lit)
   {
      printf("DEL: %s", elm_object_item_text_get(lit));
//       if(strcmp(elm_object_item_text_get(lit), "Show all"))
         elm_object_item_del(lit);
   }
   elm_list_go(list);
   _fill_list_to_notes();
   _it_clicked_cb(data, NULL, NULL); // update the visible state of all notes, to be sure to display notes without a cat
}


void
_esc_check(void* data EINA_UNUSED,
              Evas* e EINA_UNUSED,
              Evas_Object* obj EINA_UNUSED,
              void* event_info)
{
  Evas_Event_Key_Down* ev = event_info;
  const char* k = ev->keyname;
printf("test\n");
  if (!strcmp(k, "Escape")) {
    ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
  }
}


void
_open_settings(void* data,
               Evas_Object* obj EINA_UNUSED,
               const char* em EINA_UNUSED,
               const char* src EINA_UNUSED)
{
  Evas_Object* win;
  Evas_Object *bx, *lb, *en_server, *en_user, *en_password, *tb, *tb_settings, *sl_refresh, *en_calendar, *hbx, *separator;
  Evas_Object *categories_frame, *advanced_frame, *help_frame, *sync_frame, *bn, *ic, *m_check, *en_help, *systray_check, *check_sync_enabled, *check_border_enabled, *bt_all, *bt_add, *bt_del, *entry_add;

	// List Objects
	Elm_Object_Item *list1;
	Evas_Object *it;
  Eina_List* list_values = data;

  Eina_List* list_values_check = NULL;
  

  if (settings_on != 1) {
   win = elm_win_util_standard_add("enotes-settings", "Enote SETTINGS");
   
//   win = elm_win_add(NULL, "Enote SETTINGS", ELM_WIN_BASIC);
   

   elm_win_title_set(win, gettext("eNotes Settings"));

   evas_object_smart_callback_add(win, "delete,request", _close_settings, win);
    
	tb_settings = elm_table_add(win);
	evas_object_size_hint_weight_set(tb_settings, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(tb_settings);
	
	list1 = elm_list_add(tb_settings);
	evas_object_size_hint_weight_set(list1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(list1, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb_settings, list1, 0, 1, 1, 1);
	
   elm_list_select_mode_set(list1, ELM_OBJECT_SELECT_MODE_ALWAYS);
	
   elm_scroller_content_min_limit(list1, 1, 1);

   it = elm_list_item_append(list1, "Categories", NULL, NULL,
                             _config_show_categories, tb_settings);
   elm_list_item_selected_set(it, 1);
	
   it = elm_list_item_append(list1, "Sync", NULL, NULL,
                             _config_show_sync, tb_settings);

   it = elm_list_item_append(list1, "Advanced", NULL, NULL,
                             _config_show_advanced, tb_settings);
	
	it = elm_list_item_append(list1, "Help", NULL, NULL,
                             _config_show_help, tb_settings);
   elm_list_go(list1);
   evas_object_show(list1);
   
   
/// CATEGORIES FRAME ///
	categories_frame = elm_frame_add(win);
   elm_object_text_set(categories_frame, "Categories");
	evas_object_size_hint_weight_set(categories_frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
	evas_object_size_hint_align_set(categories_frame, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_table_pack(tb_settings, categories_frame, 1, 1, 1, 1);

                  bx = elm_box_add(categories_frame);
//                   evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(bx, 0, 0);

                  lb = elm_label_add(bx);
                  elm_object_text_set(lb, "Select Categories to show: <small>[Notes with no categories are always shown]</small>");
                  evas_object_show(lb);
                  elm_box_pack_end(bx, lb);
                  
//                   Elm_Object_Item *li1;
                  list_all = elm_button_add(bx);
//                   lb = elm_label_add(list_all);
                  elm_object_text_set(list_all, "Show all notes");
//                   evas_object_show(lb);
                  elm_box_pack_end(bx, lb);
//                   elm_list_multi_select_set(list_all, EINA_TRUE);
//                   elm_list_select_mode_set(list_all, ELM_OBJECT_SELECT_MODE_ALWAYS);
                  
//                   elm_list_multi_select_mode_set(list_all, ELM_OBJECT_MULTI_SELECT_MODE_DEFAULT);
                  
                  evas_object_size_hint_weight_set(list_all, EVAS_HINT_EXPAND, 0);
                  evas_object_size_hint_align_set(list_all, EVAS_HINT_FILL, EVAS_HINT_FILL);

//                   elm_list_mode_set(list_all,   ELM_LIST_EXPAND);
//                   elm_list_item_append(list_all, "Show all", NULL, NULL, NULL, list_all);
//                   elm_list_go(list_all);
                  
                  
                  evas_object_show(list_all);
                  elm_box_pack_end(bx, list_all);

                  separator = elm_separator_add(bx);
                  elm_separator_horizontal_set(separator, EINA_TRUE);
                  evas_object_show(separator);
                  elm_box_pack_end(bx, separator);
                  
//                   Elm_Object_Item *li, *li1;
                  list = elm_list_add(bx);
                  elm_list_multi_select_set(list, EINA_TRUE);
                  elm_list_multi_select_mode_set(list, ELM_OBJECT_MULTI_SELECT_MODE_DEFAULT);
                  evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                  evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);

                  elm_list_mode_set(list,   ELM_LIST_EXPAND);
                  
                  Eina_List *l;
                  Elm_Object_Item *it;
                  My_Conf_Type_Cat* new;
                  new = calloc(1, sizeof(My_Conf_Type_Cat));

//                   elm_list_scroller_policy_set(list, ELM_SCROLLER_POLICY_OFF, ELM_SCROLLER_POLICY_AUTO);
                     
                  EINA_LIST_FOREACH(cat_list_settings, l, new)
                  {
                     it = elm_list_item_append(list, new->cat_name, NULL, NULL, NULL, NULL);
                     elm_list_item_selected_set(it, new->cat_selected);
                  }
                  elm_list_go(list);
                  
                  
                  evas_object_smart_callback_add(list, "selected", _it_clicked_cb, list);
                  evas_object_smart_callback_add(list, "unselected", _it_clicked_cb, list); // BUG: mit ESC hängt sich enotes in einer schleife auf
                  evas_object_show(list);
                  elm_box_pack_end(bx, list);
                  
                        hbx = elm_box_add(bx);
                        elm_box_horizontal_set(hbx, EINA_TRUE);
                        evas_object_size_hint_weight_set(hbx, EVAS_HINT_EXPAND, 0);
                        evas_object_size_hint_align_set(hbx, EVAS_HINT_FILL, 0);
                        
                        entry_add = elm_entry_add(hbx);
                        evas_object_size_hint_weight_set(entry_add, EVAS_HINT_EXPAND, 0);
                        evas_object_size_hint_align_set(entry_add, EVAS_HINT_FILL, 0);
                        elm_entry_single_line_set(entry_add, EINA_TRUE);
                        elm_object_part_text_set(entry_add, "elm.guide", "Enter new Categorie");
                        evas_object_show(entry_add);
                        elm_box_pack_end(hbx, entry_add);
                        
                       Evas_Object *rect = evas_object_rectangle_add(entry_add);
//                         evas_object_resize(rect, w, h);
//                         evas_object_move(rect, x, y);
                        evas_object_size_hint_weight_set(rect, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
                        evas_object_size_hint_align_set(rect, EVAS_HINT_FILL, EVAS_HINT_FILL);
                        evas_object_color_set(rect, 128, 128, 128, 255);
                        evas_object_show(rect);
                        
                        bt_add = elm_button_add(hbx);
//                         evas_object_size_hint_weight_set(bt_add, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//                         evas_object_size_hint_align_set(bt_add, EVAS_HINT_FILL, EVAS_HINT_FILL);
                        elm_object_text_set(bt_add, "Add new");
                        evas_object_smart_callback_add(bt_add, "clicked", _it_clicked_add_cb, entry_add);
                        evas_object_show(bt_add);
                        elm_box_pack_end(hbx, bt_add);
                        
                        separator = elm_separator_add(hbx);
                        elm_separator_horizontal_set(separator, EINA_FALSE);
                        evas_object_show(separator);
                        elm_box_pack_end(hbx, separator);
                        
                        bt_del = elm_button_add(hbx);
//                         evas_object_size_hint_weight_set(bt_del, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//                         evas_object_size_hint_align_set(bt_del, EVAS_HINT_FILL, EVAS_HINT_FILL);
                        elm_object_text_set(bt_del, "Delete selected");
                        evas_object_smart_callback_add(bt_del, "clicked", _it_clicked_del_cb, list);
                        evas_object_show(bt_del);
                        elm_box_pack_end(hbx, bt_del);
                        
                        bt_all = elm_button_add(hbx);
//                         evas_object_size_hint_weight_set(bt_all, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
//                         evas_object_size_hint_align_set(bt_all, EVAS_HINT_FILL, EVAS_HINT_FILL);
                        elm_object_text_set(bt_all, "Select all");
                        evas_object_smart_callback_add(bt_all, "clicked", _it_clicked_select_all_cb, list);
                        evas_object_show(bt_all);
                        elm_box_pack_end(hbx, bt_all);
                        
                        
                        evas_object_show(hbx);
                  
                  elm_box_pack_end(bx, hbx);
                  evas_object_show(bx);

                  evas_object_smart_callback_add(list_all, "clicked", _showall_clicked_cb, NULL);
                  evas_object_smart_callback_add(list_all, "clicked", _it_clicked_select_all_cb, list);
   elm_object_content_set(categories_frame, bx);
	evas_object_data_set(tb_settings, "categories_frame", categories_frame);
/// CATEGORIES FRAME END ///   
   
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
//                   char buf[PATH_MAX];
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

   
	evas_object_event_callback_add(win, EVAS_CALLBACK_MOUSE_OUT, catlist_to_catlisteet, NULL);
   evas_object_event_callback_add(list, EVAS_CALLBACK_KEY_DOWN, _esc_check, NULL);
    
   _fill_list_to_notes();
   
   
    elm_win_resize_object_add(win, tb_settings);
    evas_object_resize(win, 550, 0);
    
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
