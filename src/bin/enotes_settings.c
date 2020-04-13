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
_close_settings(void* data EINA_UNUSED,
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
         //          printf("INHALT CHANGED\n");
      }else if (strcmp(password, elm_object_text_get(en_password)))
      {
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_FALSE;
         //          printf("INHALT CHANGED\n");
      }else if (strcmp(calendar_name, elm_object_text_get(en_calendar)))
      {
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_FALSE;
         //          printf("INHALT CHANGED\n");
      }else if (strcmp(server_url, elm_object_text_get(en_server)))
      {
         elm_image_file_set(ic, buf, NULL);
         online_check = EINA_FALSE;
         //          printf("INHALT CHANGED\n");
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

// static void
// _config_show_notes(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
// {
// 	Evas_Object *tb = data;
// 
// 	Evas_Object *advanced_frame = evas_object_data_get(tb, "advanced_frame");
//    Evas_Object *categories_frame = evas_object_data_get(tb, "categories_frame");
// 	Evas_Object *help_frame = evas_object_data_get(tb, "help_frame");
// 	Evas_Object *sync_frame = evas_object_data_get(tb, "sync_frame");
// // 	Evas_Object *test = evas_object_data_get(tb, "test");
// 	
//    evas_object_hide(categories_frame);
//    evas_object_hide(advanced_frame);
// 	evas_object_hide(help_frame);
// 	evas_object_hide(sync_frame);
// // 	evas_object_show(test);
// }

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
   printf("CATLIST TO CATLIST EET\n");
   Eina_List *l;
   const Eina_List *cat_list_new;
   cat_list_new = elm_list_items_get(list);
   Elm_Object_Item *lit;
   
   cat_list_settings = NULL;
   EINA_LIST_FOREACH((Eina_List*) cat_list_new, l, lit)
   {
      My_Conf_Type_Cat* new;
      new = calloc(1, sizeof(My_Conf_Type_Cat));
      new->cat_name = elm_object_item_text_get(lit);
      new->cat_selected = elm_list_item_selected_get(lit);
      
      cat_list_settings = eina_list_append(cat_list_settings, new);
   }
}

static void
update_visible_notes(Eina_List *tmp)
{
  Eina_List *list_values;
  Note *list_data2;
  int id1;
  Eina_List *l, *l1;

  EINA_LIST_FOREACH(enotes_all_objects_list, l,
                    list_values) // LISTE DER OBJEKTE DURCHGEHEN
  {
    Evas_Object *win = eina_list_nth(list_values, 0);
    int *id = eina_list_nth(list_values, 7);
    Evas_Object *en = eina_list_nth(list_values, 3);
    int x = 0, y = 0, w = 0, h = 0;
    evas_object_geometry_get(win, &x, &y, &w, &h);
    int found = 0;

    EINA_LIST_FOREACH(tmp, l1, id1) // LISTE DER OBJEKTE DURCHGEHEN
    {
      if (*id == id1) {
        printf("show id: %i x:%i - y:%i, %s\n", *id, x, y,
               elm_object_text_get(en));
        evas_object_show(win);
        //             evas_object_resize(win, w, h);
        evas_object_move(win, x, y);
        found = 1;
      }
    }
    if (found == 0) {
      printf("hide id: %i x:%i - y:%i, %s\n", *id, x, y,
             elm_object_text_get(en));
      evas_object_hide(win);
    }
  }
}

void
_cat_selected(void *data EINA_UNUSED, Evas_Object *li, void *event_info EINA_UNUSED) // SHOW/HIDE Notes based on aktive cat list items
{
   Note* list_data;
   Eina_List *l;
   Eina_List *tmp = NULL;
   int tmp_id;
   const Eina_List *cat_list_selected_settings = NULL;
   Elm_Object_Item *lit, *lit1;
   
   Elm_Object_Item *selected_cat = elm_list_selected_item_get(li);
   activ_cat =  elm_object_item_text_get(selected_cat);
   printf("ACTIV CAT: %s\n", activ_cat);
   
   EINA_LIST_FOREACH(note_list, l, list_data)
   {
      if(strcmp(list_data->Note_Sync_Data.categories, activ_cat) == 0)
      {
         tmp = eina_list_append(tmp, list_data->id);
      }
   }
   
   if(eina_list_count(tmp) != 0)
      update_visible_notes(tmp);
   else
      _enotes_new();
   
   EINA_LIST_FREE(tmp, tmp_id);
}

static void
_cancel_add_notify(void *data, Evas_Object *li EINA_UNUSED,
                   void *event_info EINA_UNUSED)
{
   evas_object_del(data);
}

static void
_add_add_notify(void *data, Evas_Object *li EINA_UNUSED,
                void *event_info EINA_UNUSED)
{
   Evas_Object *en = data;
   Elm_Object_Item *it;
   Note* list_data;
   Eina_List *l;
   Eina_List *tmp = NULL;
   int tmp_id;
   
   // TODO auf vorhandene gleich einträge checken
   if(strcmp(elm_object_text_get(en), ""))
   {
      it = elm_list_item_append(list, elm_object_text_get(en), NULL, NULL, NULL, list);
      activ_cat = elm_object_text_get(en);
      elm_object_text_set(en, ""); // reset entry
      elm_list_go(list); // update cat list
      elm_list_item_selected_set(it, EINA_TRUE);
      
      EINA_LIST_FOREACH(note_list, l, list_data)
      {
         if(strcmp(list_data->Note_Sync_Data.categories, activ_cat) == 0)
         {
            tmp = eina_list_append(tmp, list_data->id);
         }
      }
      update_visible_notes(tmp);
      catlist_to_catlisteet();
      EINA_LIST_FREE(tmp, tmp_id);
   }
}

static void
_add_add_keydown_notify(void* data,
                        Evas* e EINA_UNUSED,
                        Evas_Object* obj,
                        void* event_info)
{
   Evas_Event_Key_Down* ev = event_info;
   Evas_Object *en = data;
   const char* k = ev->key;
   Elm_Object_Item *it;
   Note* list_data;
   Eina_List *l;
   Eina_List *tmp = NULL;
   int tmp_id;
   
   if(strcmp(elm_object_text_get(en), "") && !strcmp(k, "Return")) // check if the new cat isnt in the list
   {
      it = elm_list_item_append(list, elm_object_text_get(en), NULL, NULL, NULL, list);
      activ_cat = elm_object_text_get(en);
      elm_object_text_set(en, ""); // reset entry
      elm_list_go(list); // update cat list
      elm_list_item_selected_set(it, EINA_TRUE);
      
      EINA_LIST_FOREACH(note_list, l, list_data)
      {
         if(strcmp(list_data->Note_Sync_Data.categories, activ_cat) == 0)
         {
            tmp = eina_list_append(tmp, list_data->id);
         }
      }
      update_visible_notes(tmp);
      catlist_to_catlisteet();
      EINA_LIST_FREE(tmp, tmp_id);
   }
   if(!strcmp(k, "Escape")) // check if the new cat isnt in the list
   {
      evas_object_del(obj);
   }
}

static void
_it_clicked_add_cb(void *data, Evas_Object *li EINA_UNUSED,
                   void *event_info EINA_UNUSED)
{
   Evas_Object *win = data;
   Evas_Object *bxv, *bx, *o, *notify, *en_add = NULL;
   notify = elm_notify_add(win);
   elm_notify_allow_events_set(notify, EINA_FALSE);
   evas_object_smart_callback_add(notify, "block,clicked", _cancel_add_notify, notify);
   elm_notify_align_set(notify, 0.5, 0.5);
   
   bx = elm_box_add(notify);
   elm_box_horizontal_set(bx, EINA_FALSE);
   elm_object_content_set(notify, bx);
   
   o = elm_label_add(bx);
   elm_object_text_set(o, gettext("Add new category:"));
   evas_object_show(o);
   elm_box_pack_end(bx, o);
   
   en_add = elm_entry_add(bx);
   evas_object_size_hint_weight_set(en_add, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(en_add, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_entry_editable_set(en_add, EINA_TRUE);
   elm_entry_single_line_set(en_add, EINA_TRUE);
   evas_object_show(en_add);
   elm_box_pack_end(bx, en_add);
   
   bxv = elm_box_add(notify);
   elm_box_horizontal_set(bxv, EINA_TRUE);
   
   o = elm_button_add(bxv);
   elm_object_text_set(o, gettext("add"));
   evas_object_smart_callback_add(o, "clicked", _add_add_notify, en_add);
   evas_object_show(o);
   elm_box_pack_end(bxv, o);
   
   o = elm_button_add(bxv);
   elm_object_text_set(o, gettext("close"));
   evas_object_smart_callback_add(o, "clicked", _cancel_add_notify, notify);
   evas_object_show(o);
   elm_box_pack_end(bxv, o);
   evas_object_show(bxv);
   
   elm_box_pack_end(bx, bxv);
   
   evas_object_event_callback_add(notify, EVAS_CALLBACK_KEY_DOWN, _add_add_keydown_notify, en_add);
   evas_object_show(notify);
   evas_object_show(bx);
   
}

static void
_block_clicked_cb(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Evas_Object *popup = data;
   if (popup)
      evas_object_del(popup);
   
   evas_object_del(obj);
}

static void
_it_clicked_del_final_cb(void *data, Evas_Object *li EINA_UNUSED,
                         void *event_info EINA_UNUSED)
{
   Eina_List *l;
   const Eina_List *cat_list_del = NULL;
   
   
   cat_list_del = elm_list_selected_items_get(data);
   Elm_Object_Item *lit = NULL;
   EINA_LIST_FOREACH((Eina_List*)cat_list_del, l, lit)
   {
      elm_object_item_del(lit);
   }
   elm_list_go(list);
}

static void
_hoversel_clicked_move_notes_cb(void *data, Evas_Object *obj,
                                void *event_info EINA_UNUSED)
{
   Eina_List *l, *tmp = NULL;
   Evas_Object *popup = evas_object_data_get(obj, "popup");
   Note* list_data;
   int tmp_id;
   Elm_Object_Item *selected_item;
   Elm_Object_Item *to_it = data;
   
   selected_item = elm_list_selected_item_get(list);
   
   EINA_LIST_FOREACH(note_list, l, list_data)
   {
      if(strcmp(list_data->Note_Sync_Data.categories, elm_object_item_text_get(selected_item)) == 0)
      {
         printf("move ID: %i to: %s\n", list_data->id, elm_object_item_text_get(to_it));
         list_data->Note_Sync_Data.categories = eina_stringshare_add(elm_object_item_text_get(to_it));
         tmp = eina_list_append(tmp, list_data->id);
         
         printf("NEW CAT %s\n", list_data->Note_Sync_Data.categories);
         activ_cat = elm_object_item_text_get(to_it);
      }
   }
   
   //    save_enotes_all_objects(NULL, NULL, NULL, "0");
   
   update_visible_notes(tmp);
   
   EINA_LIST_FREE(tmp, tmp_id);
   
   selected_item = elm_list_selected_item_get(list);
   elm_object_item_del(selected_item);
   elm_list_item_selected_set(to_it, EINA_TRUE);
   //     elm_list_go(list);
   catlist_to_catlisteet();
   //     fill_list_in_settings(); // Update cat list in settings window
   
   if (popup)
      evas_object_del(popup);
}

static void
_hoversel_clicked_delete_notes_cb(void *data, Evas_Object *obj,
                                  void *event_info EINA_UNUSED)
{
   if(eina_list_count(elm_list_items_get(list)) == 1)
      return;
   
   Evas_Object *popup = data;
   Eina_List *l, *l1, *tmp = NULL;
   Note* list_data;
   int *tmp_id;
   Elm_Object_Item *selected_item = NULL, *it;
   
   selected_item = elm_list_selected_item_get(list);
   const char *name = elm_object_item_text_get(selected_item);
   
   char buf[PATH_MAX];
   snprintf(buf, sizeof(buf), "%s", name);
   
   printf("item to delete 1: %s\n", buf);
   EINA_LIST_FOREACH(note_list, l, list_data)
   {
      if(strcmp(list_data->Note_Sync_Data.categories, buf) == 0)
         tmp = eina_list_append(tmp, list_data->id);
   }
   
   EINA_LIST_FOREACH(tmp, l1, tmp_id)
   {
      _del_local_data(tmp_id);
   }
   EINA_LIST_FREE(tmp, tmp_id);
   
   //    selected_item = elm_list_selected_item_get(list);
   printf("TEST DELETE\n");
   printf("item to delete 2: %s\n", buf); //WARNING elm_wdg_item_part_text_get 
   
   elm_object_item_del(elm_list_selected_item_get(list));
   //    elm_list_go(list);
   
   it = elm_list_first_item_get(list);
   elm_list_item_selected_set(it, EINA_TRUE);
   
   //    elm_list_go(list);
   //     catlist_to_catlisteet();
   //     save_enotes_all_objects(NULL, NULL, NULL, "0");
   //    fill_list_in_settings(); // Update cat list in settings window
   
   if (popup)
      evas_object_del(popup);
}

static void
_cat_clicked_del_cb(void *data, Evas_Object *li EINA_UNUSED,
                    void *event_info EINA_UNUSED)
{
   Evas_Object *popup, *btn_d, *btn_c, *box, *lb, *hoversel, *sep;
   popup = elm_popup_add(data);
   evas_object_smart_callback_add(popup, "block,clicked", _block_clicked_cb, NULL);
   
   box = elm_box_add(popup);
   evas_object_show(box);
   
   elm_object_part_text_set(popup, "title,text", "Choose your option");
   
   lb = elm_label_add(box);
   elm_object_text_set(lb, "Delete Categorie and delete all notes within?");
   evas_object_show(lb);
   elm_box_pack_end(box, lb);
   
   btn_d = elm_button_add(box);
   elm_object_text_set(btn_d, "Delete");
   evas_object_show(btn_d);
   elm_box_pack_end(box, btn_d);
   
   sep = elm_separator_add(box);
   elm_separator_horizontal_set(sep, EINA_TRUE);
   evas_object_show(sep);
   elm_box_pack_end(box, sep);
   
   lb = elm_label_add(box);
   elm_object_text_set(lb, "Move notes to Categorie");
   evas_object_show(lb);
   elm_box_pack_end(box, lb);
   
   hoversel = elm_hoversel_add(box);
   elm_hoversel_auto_update_set(hoversel, EINA_TRUE);
   elm_hoversel_hover_parent_set(hoversel, popup);
   elm_object_text_set(hoversel, "Categories");
   
   const Eina_List *l, *items;
   Elm_Object_Item *list_it, *selected_item, *it;
   
   items = elm_list_items_get(list);
   selected_item = elm_list_selected_item_get(list);
   
   EINA_LIST_FOREACH(items, l, list_it){
      Elm_Object_Item *it;
      evas_object_data_set(hoversel, "popup", popup);
      if(strcmp(elm_object_item_text_get(list_it), elm_object_item_text_get(selected_item)))
         it = elm_hoversel_item_add(hoversel, elm_object_item_text_get(list_it), NULL, ELM_ICON_NONE, _hoversel_clicked_move_notes_cb, list_it);
   }
   
   evas_object_show(hoversel);
   elm_box_pack_end(box, hoversel);
   
   sep = elm_separator_add(box);
   elm_separator_horizontal_set(sep, EINA_TRUE);
   evas_object_show(sep);
   elm_box_pack_end(box, sep);
   
   lb = elm_label_add(box);
   elm_object_text_set(lb, "Do nothing and go back");
   evas_object_show(lb);
   elm_box_pack_end(box, lb);
   
   btn_c = elm_button_add(box);
   elm_object_text_set(btn_c, "Cancel");
   evas_object_show(btn_c);
   elm_box_pack_end(box, btn_c);
   
   elm_object_content_set(popup, box);
   evas_object_smart_callback_add(btn_c, "clicked", _block_clicked_cb, popup);
   
   evas_object_smart_callback_add(btn_d, "clicked", _hoversel_clicked_delete_notes_cb, popup);
   
   evas_object_show(popup);
}

void
_key_down_settings(void* data,
                   Evas* e EINA_UNUSED,
                   Evas_Object* obj EINA_UNUSED,
                   void* event_info)
{
   Evas_Event_Key_Down* ev = event_info;
   const char* k = ev->keyname;
   Eina_Bool ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");
   printf("keydown win\n");
   if (ctrl) {
      if (!strcmp(k, "q")) // close enotes
      {
         _close_final(data);
      }
   }
   if (!strcmp(k, "Escape")) {
      ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   }
   if (!strcmp(k, "F5")) {
      _enotes_new();
   }
   
}

void
_esc_check(void* data EINA_UNUSED,
           Evas* e EINA_UNUSED,
           Evas_Object* obj EINA_UNUSED,
           void* event_info)
{
   printf("keydown list\n");
   Evas_Event_Key_Down* ev = event_info;
   const char* k = ev->keyname;
   
   if (!strcmp(k, "Escape")) {
      ev->event_flags |= EVAS_EVENT_FLAG_ON_HOLD;
   }
   
}

int
count_notes_per_category(const char* cat_name)
{
   int count = 0;
   Eina_List *l;
   Note* list_data;
   
   EINA_LIST_FOREACH(note_list, l, list_data)
   {
      if(strcmp(list_data->Note_Sync_Data.categories, cat_name) == 0)
         count++;
   }
   return count;
}

void
fill_list_in_settings()
{
   Eina_List *l;
   My_Conf_Type_Cat* new;
   new = calloc(1, sizeof(My_Conf_Type_Cat));
   
   //    catlist_to_catlisteet();
   elm_list_clear(list);
   EINA_LIST_FOREACH(cat_list_settings, l, new)
   {
      Elm_Object_Item *it;
      //       int count = 0;
      //       char buf[PATH_MAX];
      
      //       count = count_notes_per_category(new->cat_name);
      //       snprintf(buf, sizeof(buf), gettext("<color=white>%i Note(s)</color>"), count);
      
      //       Evas_Object *lb = elm_label_add(list);
      //       elm_object_text_set(lb, buf);
      //       evas_object_color_set(lb, 255, 255, 255, 255);
      //       evas_object_show(lb);
      
      it = elm_list_item_append(list, new->cat_name, NULL, /*buf*/NULL, NULL, NULL);
      elm_list_item_selected_set(it, new->cat_selected);
      
   }
   elm_list_go(list);
}

void
fill_list_in_settings1(void* data EINA_UNUSED,
                       Evas_Object* obj EINA_UNUSED,
                       const char* em EINA_UNUSED,
                       const char* src EINA_UNUSED)
{
   fill_list_in_settings();
}

void
_open_settings(void* data,
               Evas_Object* obj EINA_UNUSED,
               const char* em EINA_UNUSED,
               const char* src EINA_UNUSED)
{
   Evas_Object *bx, *lb, *en_server, *en_user, *en_password, *tb, *tb_settings, *sl_refresh, *en_calendar, *hbx, *separator;
   Evas_Object *advanced_frame, *help_frame, *sync_frame, *bn, *ic, *m_check, *en_help, *systray_check, *check_sync_enabled, *check_border_enabled/*, *bt_all*/, *bt_add, *bt_del/*, *entry_add*/;

   // List Objects
   Elm_Object_Item *list1;
   Evas_Object *it, *win_s;
   Eina_List* list_values = data;

   Eina_List* list_values_check = NULL;

   if (settings_on != 1) {
      win_s = elm_win_util_standard_add("enotes-settings", "Enote SETTINGS");
      elm_win_title_set(win_s, gettext("eNotes Settings"));
      
      evas_object_smart_callback_add(win_s, "delete,request", _close_settings, win_s);
      
      tb_settings = elm_table_add(win_s);
      evas_object_size_hint_weight_set(tb_settings, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_show(tb_settings);
      
      list1 = elm_list_add(tb_settings);
      evas_object_size_hint_weight_set(list1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(list1, EVAS_HINT_FILL, EVAS_HINT_FILL);
      elm_table_pack(tb_settings, list1, 0, 1, 1, 1);
      
      elm_list_select_mode_set(list1, ELM_OBJECT_SELECT_MODE_ALWAYS);
      
      elm_scroller_content_min_limit(list1, 1, 1);
      
      it = elm_list_item_append(list1, gettext("Categories"), NULL, NULL,
                                _config_show_categories, tb_settings);
      elm_list_item_selected_set(it, 1);
      
      //    it = elm_list_item_append(list1, gettext("Sync"), NULL, NULL,
      //                              _config_show_sync, tb_settings);
      
      it = elm_list_item_append(list1, gettext("Advanced"), NULL, NULL,
                                _config_show_advanced, tb_settings);
      
      it = elm_list_item_append(list1, gettext("Help"), NULL, NULL,
                                _config_show_help, tb_settings);
      
      // 	it = elm_list_item_append(list1, gettext("Notes"), NULL, NULL,
      //                              _config_show_notes, tb_settings);
      elm_list_go(list1);
      
      evas_object_event_callback_add(list1, EVAS_CALLBACK_KEY_DOWN, _esc_check, NULL);
      evas_object_show(list1);
      
      /*
       * 
       * /// TEST FRAME ///
       *	test = elm_frame_add(win_s);
       *   elm_object_text_set(test, "Alle Notes");
       *	evas_object_size_hint_weight_set(test, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
       *	evas_object_size_hint_align_set(test, EVAS_HINT_FILL, EVAS_HINT_FILL);
       *   elm_table_pack(tb_settings, test, 1, 1, 1, 1);
       * 
       *                  bx = elm_box_add(test);
       *                  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
       *                  evas_object_size_hint_align_set(bx, 0, 0);
       * 
       *                    /////////////TEST
       *                  
       *                  Note* new;
       *                  new = calloc(1, sizeof(Note));
       *                  Eina_List *l1;
       *                  EINA_LIST_FOREACH(note_list, l1, new) // LISTE DER OBJEKTE DURCHGEHEN
       *                  {
       *                              Evas_Object *text;
       *                              text = elm_label_add(bx);
       *                              elm_object_text_set(text, new->note_name);
       *                              evas_object_show(text);
       *                              elm_box_pack_end(bx, text);
       *      
   }
   ///////////////////TEST END
   
   evas_object_show(bx);
   
   //    elm_object_scale_set(test, 0.5);
   elm_object_content_set(test, bx);
   evas_object_data_set(tb_settings, "test", test);
   /// TEST FRAME END ///  
   */
      
      /// CATEGORIES FRAME ///
      categories_frame = elm_frame_add(win_s);
      elm_object_text_set(categories_frame, gettext("Categories"));
      evas_object_size_hint_weight_set(categories_frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(categories_frame, EVAS_HINT_FILL, EVAS_HINT_FILL);
      elm_table_pack(tb_settings, categories_frame, 1, 1, 1, 1);
      
      bx_c = elm_box_add(categories_frame);
      //                   evas_object_size_hint_weight_set(bx_c, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(bx_c, 0, 0);
      
      list = elm_list_add(bx_c);
      elm_list_multi_select_set(list, EINA_FALSE);
      elm_list_multi_select_mode_set(list, ELM_OBJECT_MULTI_SELECT_MODE_DEFAULT);
      elm_list_select_mode_set(list, ELM_OBJECT_SELECT_MODE_ALWAYS);
      evas_object_size_hint_weight_set(list, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(list, EVAS_HINT_FILL, EVAS_HINT_FILL);
      
      elm_list_mode_set(list, ELM_LIST_EXPAND);
      
      fill_list_in_settings(); // FILL LIST IN SETTINGS
      
      evas_object_smart_callback_add(list, "selected", _cat_selected, NULL);
      
      evas_object_show(list);
      elm_box_pack_end(bx_c, list);
      
      hbx = elm_box_add(bx_c);
      elm_box_horizontal_set(hbx, EINA_TRUE);
      evas_object_size_hint_weight_set(hbx, EVAS_HINT_EXPAND, 0);
      evas_object_size_hint_align_set(hbx, EVAS_HINT_FILL, 0);
      
      bt_add = elm_button_add(hbx);
      //                         evas_object_size_hint_weight_set(bt_add, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      //                         evas_object_size_hint_align_set(bt_add, EVAS_HINT_FILL, EVAS_HINT_FILL);
      elm_object_text_set(bt_add, gettext("Add"));
      evas_object_smart_callback_add(bt_add, "clicked", _it_clicked_add_cb, win_s);
      evas_object_show(bt_add);
      elm_box_pack_end(hbx, bt_add);
      
      separator = elm_separator_add(hbx);
      elm_separator_horizontal_set(separator, EINA_FALSE);
      evas_object_show(separator);
      elm_box_pack_end(hbx, separator);
      
      bt_del = elm_button_add(hbx);
      //                         evas_object_size_hint_weight_set(bt_del, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      //                         evas_object_size_hint_align_set(bt_del, EVAS_HINT_FILL, EVAS_HINT_FILL);
      elm_object_text_set(bt_del, gettext("Delete selected"));
      evas_object_smart_callback_add(bt_del, "clicked", _cat_clicked_del_cb, win_s);
      evas_object_show(bt_del);
      elm_box_pack_end(hbx, bt_del);
      
      evas_object_show(hbx);
      
      elm_box_pack_end(bx_c, hbx);
      evas_object_show(bx_c);
      
      
      elm_object_content_set(categories_frame, bx_c);
      evas_object_data_set(tb_settings, "categories_frame", categories_frame);
      /// CATEGORIES FRAME END ///   
      
      /// SYNC FRAME ///
      sync_frame = elm_frame_add(win_s);
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
      elm_object_text_set(lb, gettext("Server URL:"));
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
         en_server, "elm.guide", gettext("Enter caldav server url with calendar name"));
      elm_entry_editable_set(en_server, EINA_TRUE);
      elm_entry_single_line_set(en_server, EINA_TRUE);
      evas_object_size_hint_weight_set(
         en_server, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(en_server, EVAS_HINT_FILL, 0);
      evas_object_data_set(bx, "en_server", en_server);
      elm_table_pack(tb, en_server, 1, 0, 1, 1);
      evas_object_show(en_server);
      
      lb = elm_label_add(bx);
      elm_object_text_set(lb, gettext("Calendar Name:"));
      evas_object_size_hint_align_set(lb, 1, 0);
      evas_object_show(lb);
      elm_table_pack(tb, lb, 0, 1, 1, 1);
      evas_object_show(lb);
      
      en_calendar = elm_entry_add(bx);
      elm_config_context_menu_disabled_set(EINA_FALSE);
      elm_object_text_set(en_calendar, calendar_name);
      elm_object_part_text_set(en_calendar, "elm.guide", gettext("Enter Calendar Name"));
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
      elm_object_part_text_set(en_user, "elm.guide", gettext("Enter username"));
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
      elm_object_part_text_set(en_password, "elm.guide", gettext("Enter Password"));
      elm_entry_editable_set(en_password, EINA_TRUE);
      elm_entry_single_line_set(en_password, EINA_TRUE);
      evas_object_size_hint_weight_set(
         en_password, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(en_password, EVAS_HINT_FILL, 0);
      evas_object_data_set(bx, "en_password", en_password);
      elm_table_pack(tb, en_password, 1, 3, 1, 1);
      evas_object_show(en_password);
      
      lb = elm_label_add(bx);
      elm_object_text_set(lb, gettext("Refresh intervall:"));
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
      elm_object_text_set(m_check, gettext("manual sync [F11]"));
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
      help_frame = elm_frame_add(win_s);
      elm_object_text_set(help_frame, "Help");
      evas_object_size_hint_weight_set(help_frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(help_frame, EVAS_HINT_FILL, EVAS_HINT_FILL);
      elm_table_pack(tb_settings, help_frame, 1, 1, 1, 1);
      
      bx = elm_box_add(help_frame);
      evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, EVAS_HINT_FILL);
      
      en_help = elm_entry_add(bx);
      elm_entry_item_provider_append(en_help, item_provider, NULL);
      elm_config_context_menu_disabled_set(EINA_FALSE);
      elm_entry_scrollable_set(en_help, EINA_TRUE);
      elm_object_text_set(en_help,  gettext("Welcome to enotes! <br><br>"
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
      "<item relsize=24x24 vsize=full href=open></item><br/>"
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
      "Author: Simon Tischer [jf_simon on irc.freenode.net #e.de]"));
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
      advanced_frame = elm_frame_add(win_s);
      elm_object_text_set(advanced_frame, gettext("Advanced"));
      evas_object_size_hint_weight_set(advanced_frame, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(advanced_frame, EVAS_HINT_FILL, EVAS_HINT_FILL);
      elm_table_pack(tb_settings, advanced_frame, 1, 1, 1, 1);
      
      bx = elm_box_add(advanced_frame);
      evas_object_size_hint_weight_set(bx, 0, 0);
      evas_object_size_hint_align_set(bx, 0, 0);
      
      lb = elm_label_add(bx);
      elm_object_text_set(lb, gettext("Systray Icon:"));
      //                   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(lb, 0, 0);
      evas_object_show(lb);
      elm_box_pack_end(bx, lb);
      
      systray_check = elm_check_add(bx);
      //                   evas_object_size_hint_weight_set(systray_check, EVAS_HINT_EXPAND, 0);
      evas_object_size_hint_align_set(systray_check, 0, 0);
      elm_object_text_set(systray_check, gettext("Disable Systray Icon"));
      elm_check_state_set(systray_check, ci_systray);
      evas_object_show(systray_check);
      evas_object_data_set(tb, "systray_check", systray_check);
      evas_object_smart_callback_add(
         systray_check, "changed", _systray_callback, NULL);

      evas_object_show(systray_check);
      elm_box_pack_end(bx, systray_check);

      separator = elm_separator_add(bx);
      //                   evas_object_size_hint_weight_set(separator, EVAS_HINT_EXPAND, 0);
      evas_object_size_hint_align_set(separator, 0, 0);
      elm_separator_horizontal_set(separator, EINA_TRUE);
      evas_object_show(separator);
      elm_box_pack_end(bx, separator);


      lb = elm_label_add(bx);
      elm_object_text_set(lb, gettext("Note Look:"));
      //                   evas_object_size_hint_weight_set(lb, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(lb, 0, 0);
      evas_object_show(lb);
      elm_box_pack_end(bx, lb);
      
      check_border_enabled = elm_check_add(bx);
      //                   evas_object_size_hint_weight_set(m_check, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
      evas_object_size_hint_align_set(check_border_enabled, 0, 0);
      elm_object_text_set(check_border_enabled, gettext("Show Window Border"));
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

      evas_object_event_callback_add(win_s, EVAS_CALLBACK_MOUSE_OUT, catlist_to_catlisteet, NULL);
      evas_object_event_callback_add(win_s, EVAS_CALLBACK_KEY_DOWN, _key_down_settings, win_s);
      evas_object_event_callback_add(list, EVAS_CALLBACK_KEY_DOWN, _esc_check, NULL);

      elm_win_resize_object_add(win_s, tb_settings);
      evas_object_resize(win_s, 550, 0);

      _config_show_categories(tb_settings, NULL, NULL);

      elm_win_center(win_s, EINA_TRUE, EINA_TRUE);
      evas_object_show(win_s);
      elm_object_focus_set(win_s, EINA_TRUE);
      settings_win = win_s;
      settings_on = 1;
   } else
   {
      settings_on = 0;
      evas_object_del(settings_win);
      settings_win = NULL;
   }
}
