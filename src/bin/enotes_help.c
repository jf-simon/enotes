
#include "enotes.h"


int help_on;



void
enotes_win_help_close(void* data,
                      Evas* e EINA_UNUSED,
                      Evas_Object* obj EINA_UNUSED,
                      void* event_info EINA_UNUSED)
{
  help_on = 0;
  evas_object_del(data);
  help_win = NULL;
}

void
key_down_help(void* data EINA_UNUSED,
              Evas* e EINA_UNUSED,
              Evas_Object* obj EINA_UNUSED,
              void* event_info)
{
  Evas_Event_Key_Down* ev = event_info;
  const char* k = ev->keyname;

  if ((!strcmp(k, "Escape")) || (!strcmp(k, "F1"))) {
    if (help_win != NULL)
      enotes_win_help_close(help_win, NULL, NULL, NULL);
  }
}



static void
_anchor_clicked_cb(void* data EINA_UNUSED,
                   Evas_Object* obj EINA_UNUSED,
                   void* event_info EINA_UNUSED)
{
  Ecore_Exe* exe = ecore_exe_pipe_run(
    "xdg-open \"https://github.com/jf-simon/enotes\"", ECORE_EXE_USE_SH, NULL);

  if (exe != NULL) {
    ecore_exe_free(exe);
  } else {
    //          return 0;
  }
}


void
enotes_win_help(void* data,
                Evas_Object* obj EINA_UNUSED,
                const char* em EINA_UNUSED,
                const char* src EINA_UNUSED)
{
  char buf[PATH_MAX];
  Evas_Object* win;
  Eina_List* list_values = data;
  Evas_Object* win1 = eina_list_nth(list_values, 2);
  Evas_Object* ly = eina_list_nth(list_values, 5);

  Evas_Object* edje_obj = elm_layout_edje_get(ly);

  win = elm_win_add(NULL, "Enote HELP", ELM_WIN_BASIC);

  elm_win_title_set(win, gettext("eNotes Help"));
  elm_win_focus_highlight_enabled_set(win, EINA_FALSE);
  elm_win_borderless_set(win, EINA_FALSE);
  elm_win_alpha_set(win, EINA_TRUE);
  elm_win_autodel_set(win, EINA_TRUE);

  // LAYOUT CREATE //
  ly = elm_layout_add(win);
  snprintf(buf, sizeof(buf), "%s/themes/enotes.edj", elm_app_data_dir_get());
  elm_layout_file_set(ly, buf, "help");
  evas_object_size_hint_weight_set(ly, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

  elm_win_resize_object_add(win, ly);
  evas_object_show(ly);

  /////
  Evas_Object *tb, *lb, *ic;

  tb = elm_table_add(win);
  elm_table_padding_set(tb, 5, 0);
  elm_table_homogeneous_set(tb, EINA_TRUE);
  evas_object_size_hint_align_set(tb, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(tb);
  lb = elm_entry_add(win);
  elm_entry_context_menu_disabled_set(lb, EINA_TRUE);
  elm_entry_editable_set(lb, EINA_FALSE);
  elm_entry_single_line_set(lb, EINA_TRUE);
  evas_object_smart_callback_add(
    lb, "anchor,clicked", _anchor_clicked_cb, NULL);
  elm_object_text_set(
    lb,
    gettext("<bigger>eNotes help!</bigger><br>"
            "<b>Download and updates: <a "
            "href=anc-02>https://github.com/jf-simon/enotes</a> "
            "Author: Simon Tischer [jf_simon on irc #e.de]</b>"));
  evas_object_size_hint_align_set(lb, 0.5, 0.5);

  elm_table_pack(tb, lb, 0, 0, 16, 1);
  evas_object_show(lb);

  char buf1[1024];
  snprintf(buf1, sizeof(buf1), "%s/images/esc.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_event_callback_add(
    ic, EVAS_CALLBACK_MOUSE_UP, _delete_dialogs_cs, edje_obj);

  elm_table_pack(tb, ic, 0, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Dismiss dialogs"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 0, 2, 1, 1);
  evas_object_show(lb);
  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f1.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_event_callback_add(
    ic, EVAS_CALLBACK_MOUSE_UP, enotes_win_help_close, win);

  elm_table_pack(tb, ic, 1, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Show/hide<br>this help"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 1, 2, 1, 1);
  evas_object_show(lb);
  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f2.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 2, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Blur text"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 2, 2, 1, 1);
  evas_object_show(lb);

  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f3.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
  ;

  elm_table_pack(tb, ic, 3, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Insert done icon"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 3, 2, 1, 1);
  evas_object_show(lb);
  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f4.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 4, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Insert important icon"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 4, 2, 1, 1);
  evas_object_show(lb);
  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f5.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_event_callback_add(ic, EVAS_CALLBACK_MOUSE_UP, _enotes_new, NULL);

  elm_table_pack(tb, ic, 5, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Create a new note"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 5, 2, 1, 1);
  evas_object_show(lb);
  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f6.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 6, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Delete selected note"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 6, 2, 1, 1);
  evas_object_show(lb);
  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f7.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 7, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Set note sticky"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 7, 2, 1, 1);
  evas_object_show(lb);

  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f8.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 8, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Iconify note"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 8, 2, 1, 1);
  evas_object_show(lb);

  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f9.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 9, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Toogle designs"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 9, 2, 1, 1);
  evas_object_show(lb);

  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f10.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 10, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Write backup<br>textfile"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 10, 2, 1, 1);
  evas_object_show(lb);
  ///
  
  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f11.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 11, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Start Sync"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 11, 2, 1, 1);
  evas_object_show(lb);
  ///

  ///
  snprintf(buf1, sizeof(buf1), "%s/images/f12.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 12, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Settings"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 12, 2, 1, 1);
  evas_object_show(lb);
  ///

  snprintf(buf1, sizeof(buf1), "%s/images/ctl+m.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_table_pack(tb, ic, 13, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Toogle menu"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 13, 2, 1, 1);
  evas_object_show(lb);

  ///
  snprintf(buf1, sizeof(buf1), "%s/images/ctl+q.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_event_callback_add(ic, EVAS_CALLBACK_MOUSE_UP, _close_all2, win1);

  elm_table_pack(tb, ic, 14, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("Close enotes"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 14, 2, 1, 1);
  evas_object_show(lb);
  ///
  snprintf(
    buf1, sizeof(buf1), "%s/images/ctl+mousewheel.png", elm_app_data_dir_get());
  ic = elm_icon_add(win);
  elm_image_file_set(ic, buf1, NULL);
  evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(ic, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_event_callback_add(ic, EVAS_CALLBACK_MOUSE_UP, _close_all2, win1);

  elm_table_pack(tb, ic, 15, 1, 1, 1);
  evas_object_show(ic);

  lb = elm_label_add(win);
  elm_label_line_wrap_set(lb, ELM_WRAP_WORD);
  elm_object_text_set(lb, gettext("in/decrease font<br>ctrl+0 to reset"));
  evas_object_size_hint_weight_set(lb, 0.0, 0.0);
  evas_object_size_hint_align_set(lb, EVAS_HINT_FILL, 0);

  elm_table_pack(tb, lb, 15, 2, 1, 1);
  evas_object_show(lb);

  elm_object_part_content_set(ly, "table", tb);

  evas_object_event_callback_add(
    win, EVAS_CALLBACK_KEY_DOWN, key_down_help, NULL);

  if (help_on != 1) {
    evas_object_size_hint_align_set(win, 0.5, 0);

    evas_object_size_hint_min_set(win, 600, 150);
    evas_object_resize(win, 900, 150);
    elm_win_center(win, EINA_TRUE, EINA_FALSE);
    evas_object_show(win);
    elm_object_focus_set(win, EINA_FALSE);
    help_win = win;
    help_on = 1;
  } else {
    enotes_win_help_close(help_win, NULL, NULL, NULL);
  }
}


