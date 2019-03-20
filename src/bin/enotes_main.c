#ifdef HAVE_CONFIG_H
#include "config.h"
// #else
// #define EFL_BETA_API_SUPPORT
// #define EFL_EO_API_SUPPORT
#endif

/* NOTE: Respecting header order is important for portability.
 * Always put system first, then EFL, then your public header,
 * and finally your private one. */

#include "enotes.h"
#include "/home/simon/efl_src/efl/src/lib/elementary/elm_systray_eo.h"

#define COPYRIGHT                                                              \
  "Copyright © 2017 Simon Tischer <simon@t-tischer.de> for the enotes app  "  \
  "and various contributors (see AUTHORS) "

static void
_enotes_iconify(void* data,
                Evas_Object* o,
                const char* emission,
                const char* source);
static void
_popup_delete_cb(void* data,
                 Evas_Object* o,
                 const char* emission,
                 const char* source);
// static void             _show_help(void *data, Evas_Object *obj EINA_UNUSED,
// const char *em EINA_UNUSED, const char *src EINA_UNUSED);
static void
save_enotes_all_objects(void* data,
                        Evas* e EINA_UNUSED,
                        Evas_Object* obj EINA_UNUSED,
                        void* event_info);
void
resize_menu_off(void* data,
                Evas_Object* obj EINA_UNUSED,
                const char* emission EINA_UNUSED,
                const char* source EINA_UNUSED);
static void
_close_all2(void* data EINA_UNUSED,
            Evas* e EINA_UNUSED,
            Evas_Object* obj EINA_UNUSED,
            void* event_info);
static void
_save_notes_eet();

Eina_Stringshare* config_path;
char enotes_running[PATH_MAX];

const char* tcolor_default;
int dcolor_r, dcolor_g, dcolor_b, dcolor_a;

static char home_dir[PATH_MAX];
Eina_Strbuf* home_file;
Eina_Bool clean_start;
int help_on;

Evas_Object* help_win = NULL;
Evas_Object* popup_delete_tmp = NULL;
Evas_Object* popup_close_tmp = NULL;
Evas_Object* popup_backup_tmp = NULL;

typedef struct
{
  const char* first;
  int first1;
  const char* tcolor_default;
  int dcolor_r, dcolor_g, dcolor_b, dcolor_a;
  Eina_List* note_list1;
  const char* server_url;
  const char* user_name;
  const char* password;
  const char* calendar_name;
  Eina_Bool ci_m_check;
  Eina_Bool ci_systray;
  Eina_Bool ci_sync_enabled;
  Eina_Bool ci_border_enabled;
} Note_List_Eet;

typedef struct
{
  const char* note_name;
  int id;
  int x;
  int y;
  int h;
  int w;
  int h_m;
  int w_m;
  int h_mtmp;
  int w_mtmp;
  int h_cs;
  int w_cs;
  int h_cstmp;
  int w_cstmp;
  int color_r, color_g, color_b, color_a;
  int text_color;
  const char* tcolor;
  int text_size;
  Eina_Bool iconify;
  Eina_Bool sticky;
  const char* menu;
  const char* blur;
  const char* theme;
  const char* note_text;

  struct
  {
    const char* etag;
    const char* prodid;
    const char* created;
    const char* last_modified;
    const char* last_modified_server;
    const char* summary;
    const char* percent_complete;
    const char* status;
    const char* description;
    const char* dtstamp;
    const char* uid;
    const char* href;
    int* online;
  } Note_Sync_Data;

} My_Conf_Type;

static const char MY_CONF_FILE_ENTRY[] = "config";

static Eet_Data_Descriptor* _my_conf_descriptor;
static Eet_Data_Descriptor* _my_conf_sub_descriptor;

static void
_my_conf_descriptor_init(void)
{
  Eet_Data_Descriptor_Class eddc;
  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, Note_List_Eet);
  _my_conf_descriptor = eet_data_descriptor_stream_new(&eddc);

  EET_EINA_STREAM_DATA_DESCRIPTOR_CLASS_SET(&eddc, My_Conf_Type);
  _my_conf_sub_descriptor = eet_data_descriptor_stream_new(&eddc);

#define MY_CONF_ADD_BASIC(member, eet_type)                                    \
  EET_DATA_DESCRIPTOR_ADD_BASIC(                                               \
    _my_conf_descriptor, Note_List_Eet, #member, member, eet_type)
#define MY_CONF_SUB_ADD_BASIC(member, eet_type)                                \
  EET_DATA_DESCRIPTOR_ADD_BASIC(                                               \
    _my_conf_sub_descriptor, My_Conf_Type, #member, member, eet_type)

  MY_CONF_SUB_ADD_BASIC(note_name, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(id, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(x, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(y, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(h, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(w, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(h_m, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(w_m, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(h_mtmp, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(w_mtmp, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(h_cs, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(w_cs, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(h_cstmp, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(w_cstmp, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(color_r, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(color_g, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(color_b, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(color_a, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(text_color, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(tcolor, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(text_size, EET_T_INT);
  MY_CONF_SUB_ADD_BASIC(iconify, EET_T_UCHAR);
  MY_CONF_SUB_ADD_BASIC(sticky, EET_T_UCHAR);
  MY_CONF_SUB_ADD_BASIC(menu, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(blur, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(theme, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(note_text, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.etag, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.prodid, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.created, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.last_modified, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.last_modified_server, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.summary, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.percent_complete, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.status, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.description, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.dtstamp, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.uid, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.href, EET_T_STRING);
  MY_CONF_SUB_ADD_BASIC(Note_Sync_Data.online, EET_T_INT);

  MY_CONF_ADD_BASIC(first, EET_T_STRING);
  MY_CONF_ADD_BASIC(first1, EET_T_INT);
  MY_CONF_ADD_BASIC(tcolor_default, EET_T_STRING);
  MY_CONF_ADD_BASIC(dcolor_r, EET_T_INT);
  MY_CONF_ADD_BASIC(dcolor_g, EET_T_INT);
  MY_CONF_ADD_BASIC(dcolor_b, EET_T_INT);
  MY_CONF_ADD_BASIC(dcolor_a, EET_T_INT);
  MY_CONF_ADD_BASIC(server_url, EET_T_STRING);
  MY_CONF_ADD_BASIC(user_name, EET_T_STRING);
  MY_CONF_ADD_BASIC(password, EET_T_STRING);
  MY_CONF_ADD_BASIC(calendar_name, EET_T_STRING);
  MY_CONF_ADD_BASIC(ci_m_check, EET_T_UCHAR);
  MY_CONF_ADD_BASIC(ci_systray, EET_T_UCHAR);
  MY_CONF_ADD_BASIC(ci_sync_enabled, EET_T_UCHAR);
  MY_CONF_ADD_BASIC(ci_border_enabled, EET_T_UCHAR);

  // And add the sub descriptor as a linked list at 'subs' in the main struct
  EET_DATA_DESCRIPTOR_ADD_LIST(_my_conf_descriptor,
                               Note_List_Eet,
                               "note_list1",
                               note_list1,
                               _my_conf_sub_descriptor);

#undef MY_CONF_ADD_BASIC
#undef MY_CONF_SUB_ADD_BASIC
}

static void
_my_conf_descriptor_shutdown(void)
{
  eet_data_descriptor_free(_my_conf_sub_descriptor);
  eet_data_descriptor_free(_my_conf_descriptor);
}


static void
_disable_bt1(void* data,
             Evas_Object* obj EINA_UNUSED,
             const char* emission EINA_UNUSED,
             const char* source EINA_UNUSED)
{
  if (eina_list_count(enotes_all_objects_list) == 1)
    elm_object_disabled_set(data, EINA_TRUE);
  else
    elm_object_disabled_set(data, EINA_FALSE);
}

const char*
get_text_color(Elm_Entry* entry)
{
  char **split, **split1;
  const char* style;
  style = elm_entry_text_style_user_peek(entry);

  split = eina_str_split(style, "color=", 0);
  split1 = eina_str_split(split[1], " ", 0);

  return split1[0];

  free(split[0]);
  free(split);
}

const char*
get_text_color1(Elm_Entry* entry)
{
  char **split, **split1;
  const char* style;
  style = elm_entry_text_style_user_peek(entry);

  split = eina_str_split(style, "color=#", 0);
  split1 = eina_str_split(split[1], " ", 0);

  return split1[0];

  free(split[0]);
  free(split);
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

static void
_open_backup_textfile(void* data,
                      Evas_Object* obj EINA_UNUSED,
                      void* event_info EINA_UNUSED)
{
  char buf[PATH_MAX];
  char* backup_textfile = data;
  snprintf(buf, sizeof(buf), "xdg-open %s", backup_textfile);
  Ecore_Exe* exe = ecore_exe_pipe_run(buf, ECORE_EXE_USE_SH, NULL);

  if (exe != NULL) {
    ecore_exe_free(exe);
  } else {
    //          return 0;
  }
}

static void
_backup_to_file_all_dismiss_cb(void* data,
                               Evas_Object* o EINA_UNUSED,
                               void* event EINA_UNUSED)
{
  evas_object_del(data);
  popup_backup_tmp = NULL;
}

static void
_popup_close_cb(void* data,
                Evas_Object* obj EINA_UNUSED,
                void* event_info EINA_UNUSED)
{
  evas_object_del(data);
  popup_close_tmp = NULL;
  popup_delete_tmp = NULL;
}

static void
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
_delete_dialogs_cs(void* data,
                   Evas* e EINA_UNUSED,
                   Evas_Object* obj EINA_UNUSED,
                   void* event_info EINA_UNUSED)
{
  if (popup_close_tmp != NULL)
    _popup_close_cb(popup_close_tmp, NULL, NULL);

  if (popup_delete_tmp != NULL)
    _popup_close_cb(popup_delete_tmp, NULL, NULL);

  if (popup_backup_tmp != NULL)
    _backup_to_file_all_dismiss_cb(popup_backup_tmp, NULL, NULL);

  if (data != NULL) {
    if (help_win != NULL)
      enotes_win_help_close(help_win, NULL, NULL, NULL);
  }

  if (data != NULL) {
    if (strcmp(edje_object_part_state_get(data, "color_swallow", NULL),
               "default"))
      edje_object_signal_emit(data, "mouse,clicked,1", "cs_icon");
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
  elm_win_borderless_set(win, EINA_TRUE);
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


void _hide_show_all_notes(void *data, Evas_Object *obj, void *event_info EINA_UNUSED)
{
  Eina_List* list_values;
  Eina_List* l;

  EINA_LIST_FOREACH(enotes_all_objects_list,
                    l,
                    list_values) // LISTE DER OBJEKTE DURCHGEHEN
  {
   Evas_Object* win = eina_list_nth(list_values, 0);
  
   if(all_hidden == EINA_TRUE)
   {
      elm_win_iconified_set(win, 0);
      elm_obj_systray_status_set(item, 1);
//       elm_object_item_text_set(data, "hide all notes");
   }
   else
   {
      elm_win_iconified_set(win, 1);
      elm_obj_systray_status_set(item, 2);
//       elm_object_item_text_set(data, "show all notes");
   }
     
  }
  
   if(all_hidden == EINA_TRUE)
      all_hidden = EINA_FALSE;
   else
      all_hidden = EINA_TRUE;
}
/*

void _show_all_notes()
{
  Eina_List* list_values;
  Eina_List* l;

  EINA_LIST_FOREACH(enotes_all_objects_list,
                    l,
                    list_values) // LISTE DER OBJEKTE DURCHGEHEN
  {
   Evas_Object* win = eina_list_nth(list_values, 0);
   elm_win_iconified_set(win, 0);
  }
}*/

static void
_bt_colorset_save(void* data,
                  Evas_Object* obj EINA_UNUSED,
                  void* event_info EINA_UNUSED)
{
  int r, g, b, a;
  Eina_List* list_values = data;
  Evas_Object* background = eina_list_nth(list_values, 8);
  Evas_Object* entry_notecontent = eina_list_nth(list_values, 0);
  tcolor_default = get_text_color(entry_notecontent);

  evas_object_color_get(background, &r, &g, &b, &a);
  evas_color_argb_unpremul(a, &r, &g, &b);
  dcolor_r = r;
  dcolor_g = g;
  dcolor_b = b;
  dcolor_a = a;

  save_enotes_all_objects(NULL, NULL, NULL, "0");
}

static void
_bt_colorset_to_all(void* data,
                    Evas_Object* obj EINA_UNUSED,
                    void* event_info EINA_UNUSED)
{
  Eina_List* l1;
  Eina_List* data1 = NULL;
  int r, g, b, a;
  char buf[PATH_MAX];
  char buf1[PATH_MAX];

  Eina_List* list_values = data;
  Evas_Object* background = eina_list_nth(list_values, 8);
  Evas_Object* entry_notecontent = eina_list_nth(list_values, 0);

  evas_object_color_get(background, &r, &g, &b, &a);
  evas_color_argb_unpremul(a, &r, &g, &b);

  snprintf(buf1,
           sizeof(buf1),
           "DEFAULT='font=Sans:style=Regular color=%s font_size=16'",
           get_text_color(entry_notecontent));

  EINA_LIST_FOREACH(enotes_all_objects_list, l1, data1)
  {
    int* textsize = eina_list_nth(data1, 8);
    snprintf(buf,
             sizeof(buf),
             "DEFAULT='font=Sans:style=Regular color=%s font_size=%i'",
             get_text_color(entry_notecontent),
             *textsize);
    evas_color_argb_premul(a, &r, &g, &b);
    evas_object_color_set(eina_list_nth(data1, 4), r, g, b, a);
    elm_entry_text_style_user_push(eina_list_nth(data1, 2), buf);
    elm_entry_text_style_user_push(eina_list_nth(data1, 3), buf1);
  }
}

/*
static void
_restack_block_clicked_cb(void *data EINA_UNUSED, Evas_Object *obj, void
*event_info EINA_UNUSED)
{
    evas_object_del(obj);
    popup_close_tmp = NULL;
    popup_delete_tmp = NULL;
}*/

static void
_backup_to_file_selected(void* data,
                         Evas_Object* obj EINA_UNUSED,
                         void* event_info EINA_UNUSED)
{
  Eina_Strbuf* backup_file;
  backup_file = eina_strbuf_new();
  Eina_List* list_values = data;
  Evas_Object* win = eina_list_nth(list_values, 2);
  int* id = eina_list_nth(list_values, 3);
  //     char backup_file[PATH_MAX];

  config_path = efreet_documents_dir_get();
  snprintf(home_dir, sizeof(home_dir), "%s/enotes", config_path);

  if (ecore_file_exists(efreet_documents_dir_get())) {
    if (!ecore_file_exists(home_dir)) {
      ecore_file_mkdir(home_dir);
    }
  } else {
    // EDIALOG "CANNOT WRITE FILE"
  }

  if (ecore_file_exists(home_dir)) {
    char buf_time[PATH_MAX];

    struct tm* local_time;
    time_t current_time;
    Eina_List* l;
    Note* list_data2;

    current_time = time(NULL);
    local_time = localtime(&current_time);
    strftime(buf_time, sizeof(buf_time), "%Y-%m-%d_%H-%M-%S", local_time);
    //    snprintf(backup_file, sizeof(backup_file), "%s_%s.txt", home_dir,
    //    buf_time);
    eina_strbuf_append_printf(backup_file, "%s_%s.txt", home_dir, buf_time);
    FILE* fp;

    fp = fopen(eina_strbuf_string_get(backup_file), "w");

    if (fp == NULL) {
      //             printf("Datei konnte NICHT geoeffnet werden.\n");
    } else {
      //             printf("Datei konnte geoeffnet werden.\n");

      EINA_LIST_FOREACH(note_list, l, list_data2)
      {
        if (list_data2->id == *id) {
          fprintf(fp, "-----------------------------------\n");
          fprintf(
            fp, "%s\n", elm_entry_markup_to_utf8(list_data2->note_name));
          fprintf(
            fp, "%s\n", elm_entry_markup_to_utf8(list_data2->note_text));
          fprintf(fp, "-----------------------------------\n\n\n");
        }
      }
    }

    fclose(fp);
  }
  Evas_Object *notify, *bx, *bxv, *o;

  notify = elm_notify_add(win);

  bx = elm_box_add(notify);
  elm_box_horizontal_set(bx, EINA_FALSE);

  o = elm_label_add(bx);
  elm_object_text_set(o, gettext("File written to:"));
  evas_object_show(o);
  elm_box_pack_end(bx, o);

  o = elm_entry_add(bx);
  elm_entry_editable_set(o, EINA_FALSE);
  elm_entry_single_line_set(o, EINA_TRUE);
  elm_object_text_set(o, eina_strbuf_string_get(backup_file));
  evas_object_show(o);
  elm_box_pack_end(bx, o);

  bxv = elm_box_add(notify);
  elm_box_horizontal_set(bxv, EINA_TRUE);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("close"));
  evas_object_smart_callback_add(
    o, "clicked", _backup_to_file_all_dismiss_cb, notify);
  evas_object_show(o);
  elm_box_pack_end(bxv, o);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("open"));
  evas_object_smart_callback_add(o,
                                 "clicked",
                                 _open_backup_textfile,
                                 strdup(eina_strbuf_string_get(backup_file)));
  evas_object_show(o);
  elm_box_pack_end(bxv, o);
  evas_object_show(bxv);

  elm_box_pack_end(bx, bxv);

  evas_object_show(bx);
  elm_object_content_set(notify, bx);
  evas_object_show(notify);
  popup_backup_tmp = notify;

  eina_strbuf_free(backup_file);
}

static void
_backup_to_file_all(void* data,
                    Evas_Object* obj EINA_UNUSED,
                    void* event_info EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* win = eina_list_nth(list_values, 2);
  Eina_Strbuf* backup_file;
  backup_file = eina_strbuf_new();

  Eina_Stringshare* config_path = efreet_documents_dir_get();
  snprintf(home_dir, sizeof(home_dir), "%s/enotes", config_path);

  if (ecore_file_exists(efreet_documents_dir_get())) {
    if (!ecore_file_exists(home_dir)) {
      ecore_file_mkdir(home_dir);
    }
  } else {
    // EDIALOG "CANNOT WRITE FILE"
  }

  if (ecore_file_exists(home_dir)) {
    char buf_time[PATH_MAX];

    struct tm* local_time;
    time_t current_time;
    Eina_List* l;
    Note* list_data2;

    current_time = time(NULL);
    local_time = localtime(&current_time);
    strftime(buf_time, sizeof(buf_time), "%Y-%m-%d_%H-%M-%S", local_time);
    eina_strbuf_append_printf(backup_file, "%s_%s.txt", home_dir, buf_time);

    FILE* fp;

    fp = fopen(eina_strbuf_string_get(backup_file), "w");

    if (fp == NULL) {
      //                   printf("Datei konnte NICHT geoeffnet werden.\n");
    } else {
      //                   printf("Datei konnte geoeffnet werden.\n");

      EINA_LIST_FOREACH(note_list, l, list_data2)
      {
        fprintf(fp, "-----------------------------------\n");
        fprintf(
          fp, "%s\n", elm_entry_markup_to_utf8(list_data2->note_name));
        fprintf(
          fp, "%s\n", elm_entry_markup_to_utf8(list_data2->note_text));
        fprintf(fp, "-----------------------------------\n\n\n");
      }
    }

    fclose(fp);
  }
  Evas_Object *notify, *bx, *bxv, *o;

  notify = elm_notify_add(win);

  bx = elm_box_add(notify);
  elm_box_horizontal_set(bx, EINA_FALSE);

  o = elm_label_add(bx);
  elm_object_text_set(o, gettext("File written to:"));
  evas_object_show(o);
  elm_box_pack_end(bx, o);

  o = elm_entry_add(bx);
  elm_entry_editable_set(o, EINA_FALSE);
  elm_entry_single_line_set(o, EINA_TRUE);
  elm_object_text_set(o, eina_strbuf_string_get(backup_file));
  evas_object_show(o);
  elm_box_pack_end(bx, o);

  bxv = elm_box_add(notify);
  elm_box_horizontal_set(bxv, EINA_TRUE);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("close"));
  evas_object_smart_callback_add(
    o, "clicked", _backup_to_file_all_dismiss_cb, notify);
  evas_object_show(o);
  elm_box_pack_end(bxv, o);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("open"));
  evas_object_smart_callback_add(
    o, "clicked", _open_backup_textfile, strdup(eina_strbuf_string_get(backup_file)));
  evas_object_show(o);
  elm_box_pack_end(bxv, o);
  evas_object_show(bxv);

  elm_box_pack_end(bx, bxv);
  evas_object_show(bx);
  elm_object_content_set(notify, bx);
  evas_object_show(notify);
  popup_backup_tmp = notify;

  eina_strbuf_free(backup_file);
}

static void
_backup_to_file(void* data)
{
  Eina_List* list_values = data;
  Evas_Object* win = eina_list_nth(list_values, 2);
  Evas_Object *notify, *bx, *bxv, *o, *sep;

  notify = elm_notify_add(win);
  bx = elm_box_add(notify);
  elm_box_horizontal_set(bx, EINA_FALSE);

  o = elm_label_add(bx);
  elm_object_text_set(o, gettext("What do you want to write to file?"));
  evas_object_show(o);
  elm_box_pack_end(bx, o);

  bxv = elm_box_add(notify);
  elm_box_horizontal_set(bxv, EINA_TRUE);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("all notes"));
  evas_object_smart_callback_add(o, "clicked", _backup_to_file_all, data);
  evas_object_smart_callback_add(
    o, "clicked", _backup_to_file_all_dismiss_cb, notify);
  evas_object_show(o);
  elm_box_pack_end(bxv, o);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("this note"));
  evas_object_smart_callback_add(o, "clicked", _backup_to_file_selected, data);
  evas_object_smart_callback_add(
    o, "clicked", _backup_to_file_all_dismiss_cb, notify);
  evas_object_show(o);
  elm_box_pack_end(bxv, o);
  evas_object_show(bxv);

  elm_box_pack_end(bx, bxv);

  sep = elm_separator_add(bx);
  elm_separator_horizontal_set(sep, EINA_TRUE);
  evas_object_size_hint_weight_set(sep, EVAS_HINT_EXPAND, 0.0);
  evas_object_size_hint_align_set(sep, EVAS_HINT_FILL, 0.0);
  evas_object_show(sep);
  elm_box_pack_end(bx, sep);

  o = elm_button_add(bx);
  elm_object_text_set(o, gettext("cancel"));
  evas_object_smart_callback_add(
    o, "clicked", _backup_to_file_all_dismiss_cb, notify);
  evas_object_show(o);
  elm_box_pack_end(bx, o);

  evas_object_show(bx);
  elm_object_content_set(notify, bx);

  if (popup_backup_tmp != NULL)
    _backup_to_file_all_dismiss_cb(popup_backup_tmp, NULL, NULL);

  if (popup_close_tmp != NULL)
    _popup_close_cb(popup_close_tmp, NULL, NULL);

  if (popup_delete_tmp != NULL)
    _popup_close_cb(popup_delete_tmp, NULL, NULL);

  evas_object_show(notify);
  popup_backup_tmp = notify;
}

static void
_home_dir()
{
  home_file = eina_strbuf_new();
  Eina_Stringshare* config_path = efreet_config_home_get();
  snprintf(home_dir, sizeof(home_dir), "%s/enotes", config_path);
  eina_strbuf_append_printf(home_file, "%s/enotes.eet", home_dir);

  if (ecore_file_exists(efreet_config_home_get())) {
    if (!ecore_file_exists(home_dir)) {
      ecore_file_mkdir(home_dir);
    }
  } else {
    // EDIALOG "CANNOT WRITE FILE"
  }
}

static void
_read_notes_eet()
{
  Eet_File* ef;

  Note_List_Eet* my_conf;

  eet_init();

  //     printf("home_file: %s\n", home_file);

  ef = eet_open(eina_strbuf_string_get(home_file), EET_FILE_MODE_READ);
  if (!ef) {
    printf("ERROR: could not open '%s' for read\n",
           eina_strbuf_string_get(home_file));
    enotes_win_help(NULL, NULL, NULL, NULL);
    clean_start = EINA_TRUE;
    return;
  }

  clean_start = EINA_FALSE;

  my_conf = eet_data_read(ef, _my_conf_descriptor, MY_CONF_FILE_ENTRY);

  note_list = my_conf->note_list1;
  dcolor_r = my_conf->dcolor_r;
  dcolor_g = my_conf->dcolor_g;
  dcolor_b = my_conf->dcolor_b;
  dcolor_a = my_conf->dcolor_a;
  tcolor_default = my_conf->tcolor_default;
  server_url = my_conf->server_url;
  user_name = my_conf->user_name;
  password = my_conf->password;
  calendar_name = my_conf->calendar_name;
  ci_m_check = my_conf->ci_m_check;
  ci_systray = my_conf->ci_systray;
  ci_sync_enabled = my_conf->ci_sync_enabled;

  eet_close(ef);
  eet_shutdown();
}

static void
_save_notes_eet()
{
  Eet_File* ef;
  eet_init();
  ef = eet_open(eina_strbuf_string_get(home_file), EET_FILE_MODE_WRITE);

  if (!ef) {
    printf("ERROR\n");
    fprintf(stderr,
            "ERROR: could not open '%s' for write\n",
            eina_strbuf_string_get(home_file));
    //            return EINA_FALSE;   //TODO got elm_main -> END
  } else {
    Note_List_Eet* my_conf = calloc(1, sizeof(Note_List_Eet));

    if (!my_conf) {
      fprintf(stderr, "ERROR: could not calloc My_Conf_Type\n");
      //                 return NULL;   //TODO got elm_main -> END
    }

    //         my_conf->note_list1 = note_list;
    my_conf->note_list1 = note_list;

    my_conf->tcolor_default = tcolor_default;
    my_conf->dcolor_r = dcolor_r;
    my_conf->dcolor_g = dcolor_g;
    my_conf->dcolor_b = dcolor_b;
    my_conf->dcolor_a = dcolor_a;
    my_conf->server_url = server_url;
    my_conf->user_name = user_name;
    my_conf->password = password;
    my_conf->calendar_name = calendar_name;
    my_conf->ci_m_check = ci_m_check;
    my_conf->ci_systray = ci_systray;
    my_conf->ci_sync_enabled = ci_sync_enabled;

    eet_data_write(
      ef, _my_conf_descriptor, MY_CONF_FILE_ENTRY, my_conf, EINA_TRUE);
  }

  eet_close(ef);
  eet_shutdown();
}

static void
_layout_change(void* data,
               Evas_Object* obj EINA_UNUSED,
               const char* em EINA_UNUSED,
               const char* src EINA_UNUSED)
{
  char buf[PATH_MAX];
  const char* layout;

  Eina_List* list_values = data;
  Evas_Object* entry_notecontent = eina_list_nth(list_values, 0);
  Evas_Object* ly = eina_list_nth(list_values, 5);

  Evas_Object* edje_obj = elm_layout_edje_get(ly);

  elm_layout_file_get(edje_obj, NULL, &layout);

  const char* state_blur;
  const char* state_menu;
  state_blur = edje_object_part_state_get(edje_obj, "blur", NULL);
  state_menu = edje_object_part_state_get(edje_obj, "rect_overleft", NULL);

  snprintf(buf, sizeof(buf), "%s/themes/enotes.edj", elm_app_data_dir_get());

  if (strcmp(layout, "layout1") == 0) {
    elm_layout_file_set(edje_obj, buf, "layout2");
  } else if (strcmp(layout, "layout2") == 0) {
    elm_layout_file_set(edje_obj, buf, "layout3");
  } else if (strcmp(layout, "layout3") == 0) {
    elm_layout_file_set(edje_obj, buf, "layout1");
  } else
    elm_layout_file_set(edje_obj, buf, "layout1");

  //     elm_layout_part_cursor_set(ly, "tape", ELM_CURSOR_HAND2);
  //     elm_layout_part_cursor_set(ly, "rect_over", ELM_CURSOR_HAND2);
  //     elm_layout_part_cursor_set(ly, "shadow_corner",
  //     ELM_CURSOR_BOTTOM_RIGHT_CORNER);

  if (strcmp(state_blur, "on") == 0) {
    edje_object_signal_emit(edje_obj, "blur_on", "");
  }

  if (strcmp(state_menu, "on") == 0) {
    edje_object_signal_emit(edje_obj, "mouse_in_rect_overleft_in", "");
  }

  evas_object_show(entry_notecontent);
}

static Evas_Object*
item_provider(void* images EINA_UNUSED,
              Evas_Object* entry_notecontent,
              const char* item)
{
  Evas_Object* o = NULL;
  char buf1[1024];

  if (!strcmp(item, "done")) {
    snprintf(
      buf1, sizeof(buf1), "%s/themes/enotes.edj", elm_app_data_dir_get());
    o = edje_object_add(evas_object_evas_get(entry_notecontent));
    edje_object_file_set(o, buf1, "done");
  }
  if (!strcmp(item, "important")) {
    snprintf(
      buf1, sizeof(buf1), "%s/themes/enotes.edj", elm_app_data_dir_get());
    o = edje_object_add(evas_object_evas_get(entry_notecontent));
    edje_object_file_set(o, buf1, "important");
  }
  if (!strcmp(item, "open")) {
    snprintf(
      buf1, sizeof(buf1), "%s/themes/enotes.edj", elm_app_data_dir_get());
    o = edje_object_add(evas_object_evas_get(entry_notecontent));
    edje_object_file_set(o, buf1, "open");
  }
  return o;
}

static void
_insert_done_icon(void* data,
                  Evas_Object* obj EINA_UNUSED,
                  const char* em EINA_UNUSED,
                  const char* src EINA_UNUSED)
{
  Evas_Object* entry_notecontent = data;
  int pos;
  if (elm_object_focus_get(entry_notecontent)) {
    pos = elm_entry_cursor_pos_get(entry_notecontent);
    elm_entry_cursor_line_begin_set(entry_notecontent);
    elm_entry_entry_insert(entry_notecontent,
                           "<item relsize=24x24 vsize=full href=done></item> ");
    elm_entry_cursor_pos_set(entry_notecontent, pos + 2);
  }
}

static void
_insert_open_icon(void* data,
                  Evas_Object* obj EINA_UNUSED,
                  const char* em EINA_UNUSED,
                  const char* src EINA_UNUSED)
{
  Evas_Object* entry_notecontent = data;

  if (elm_object_focus_get(entry_notecontent))
    elm_entry_entry_insert(entry_notecontent,
                           "<item relsize=24x24 vsize=full href=open></item> ");
}

static void
_textsize_change_cb_increase(void* data,
                             Evas_Object* obj EINA_UNUSED,
                             const char* em EINA_UNUSED,
                             const char* src EINA_UNUSED)
{
  Eina_List* list_text = data;
  Evas_Object* entry_notecontent = eina_list_nth(list_text, 0);
  int* textsize = eina_list_nth(list_text, 2);
  Evas_Object* ly = eina_list_nth(list_text, 4);

  Evas_Object* edje_obj = elm_layout_edje_get(ly);

  if (strcmp(edje_object_part_state_get(edje_obj, "color_swallow", NULL),
             "on")) {
    *textsize = *textsize + 1;
    char buf[PATH_MAX];
    snprintf(buf,
             sizeof(buf),
             "DEFAULT='font=Sans:style=Regular color=%s font_size=%i'",
             get_text_color(entry_notecontent),
             *textsize);
    elm_entry_text_style_user_push(entry_notecontent, buf);
  }
}

static void
_textsize_change_cb_decrease(void* data,
                             Evas_Object* obj EINA_UNUSED,
                             const char* em EINA_UNUSED,
                             const char* src EINA_UNUSED)
{
  Eina_List* list_text = data;
  Evas_Object* entry_notecontent = eina_list_nth(list_text, 0);
  int* textsize = eina_list_nth(list_text, 2);
  Evas_Object* ly = eina_list_nth(list_text, 4);

  Evas_Object* edje_obj = elm_layout_edje_get(ly);

  if (strcmp(edje_object_part_state_get(edje_obj, "color_swallow", NULL),
             "on")) {
    *textsize = *textsize - 1;
    char buf[PATH_MAX];
    snprintf(buf,
             sizeof(buf),
             "DEFAULT='font=Sans:style=Regular color=%s font_size=%i'",
             get_text_color(entry_notecontent),
             *textsize);
    elm_entry_text_style_user_push(entry_notecontent, buf);
  }
}

static void
_textsize_change_cb_normal(void* data,
                           Evas_Object* obj EINA_UNUSED,
                           const char* em EINA_UNUSED,
                           const char* src EINA_UNUSED)
{
  Eina_List* list_text = data;
  Evas_Object* entry_notecontent = eina_list_nth(list_text, 0);
  int* textsize = eina_list_nth(list_text, 2);
  Evas_Object* ly = eina_list_nth(list_text, 4);

  Evas_Object* edje_obj = elm_layout_edje_get(ly);

  if (strcmp(edje_object_part_state_get(edje_obj, "color_swallow", NULL),
             "on")) {
    *textsize = 11;
    char buf[PATH_MAX];
    snprintf(buf,
             sizeof(buf),
             "DEFAULT='font=Sans:style=Regular color=%s font_size=%i'",
             get_text_color(entry_notecontent),
             *textsize);
    elm_entry_text_style_user_push(entry_notecontent, buf);
  }
}

static void
save_enotes_all_objects(void* data EINA_UNUSED,
                        Evas* e EINA_UNUSED,
                        Evas_Object* obj EINA_UNUSED,
                        void* event_info EINA_UNUSED)
{
  Eina_List* l;
  Eina_List* list_values;

  char last_modified[512];
  time_t t;
  struct tm* ts;

  EINA_LIST_FOREACH(enotes_all_objects_list,
                    l,
                    list_values) // LISTE DER OBJEKTE DURCHGEHEN
  {
    int r, g, b, a;
    const char* state;
    const char* blur;
    const char* theme;
    Evas_Object* edje_obj;
    Evas_Coord w, h, x, y;

    Note* list_data2;

    Evas_Object* win = eina_list_nth(list_values, 0);
    Evas_Object* ly = eina_list_nth(list_values, 1);
    Evas_Object* entry_notecontent = eina_list_nth(list_values, 2);
    Evas_Object* entry_title = eina_list_nth(list_values, 3);
    Evas_Object* background = eina_list_nth(list_values, 4);
    int* old_id = eina_list_nth(list_values, 7);

    // get layout of win
    elm_layout_file_get(ly, NULL, &theme);

    // get menu state
    edje_obj = elm_layout_edje_get(ly);
    state = edje_object_part_state_get(edje_obj, "rect_overleft", NULL);

    // get blur state
    blur = edje_object_part_state_get(edje_obj, "blur", NULL);

    // get color of "background"
    evas_object_color_get(background, &r, &g, &b, &a);
    evas_color_argb_unpremul(a, &r, &g, &b);

    // get size and position of "win"
    evas_object_geometry_get(win, &x, &y, &w, &h);

    // set new wintitle of "win"
    elm_win_title_set(win, elm_object_text_get(entry_title));

    Eina_List* l1;
    EINA_LIST_FOREACH(note_list, l1, list_data2)
    {
      if (list_data2->id == *old_id) {
        list_data2->id = *old_id;
        list_data2->x = x;
        list_data2->y = y;
        list_data2->w = w;
        list_data2->h = h;

        if (strcmp(event_info, "1") == 0) {
          list_data2->w_m = w;
          list_data2->h_m = h;
          list_data2->w_mtmp = w;
          list_data2->h_mtmp = h;
        }
        if (strcmp(event_info, "4") == 0) {
          list_data2->w_mtmp = w;
          list_data2->h_mtmp = h;
        }
        if (strcmp(event_info, "7") == 0) {
          tcolor_default = get_text_color(entry_notecontent);
          dcolor_r = r;
          dcolor_g = g;
          dcolor_b = b;
          dcolor_a = a;
        }
        if (strcmp(event_info, "5") == 0) {
          list_data2->w_cstmp = w;
          list_data2->h_cstmp = h;
        }
        if (strcmp(event_info, "2") == 0) {
          list_data2->w_cs = w;
          list_data2->h_cs = h;
        }

        if (strcmp(event_info, "3") == 0) {
          list_data2->iconify = EINA_TRUE;
          //                                 printf("ICONIFY EINA_TRUE \n");
        } else {
          list_data2->iconify = EINA_FALSE;
          //                                 printf("ICONIFY EINA_FALSE \n");
        }

        list_data2->color_r = r;
        list_data2->color_g = g;
        list_data2->color_b = b;
        list_data2->color_a = a;
        list_data2->tcolor = get_text_color(entry_notecontent);
        list_data2->sticky = elm_win_sticky_get(win);
        list_data2->theme = eina_stringshare_add(theme);
        list_data2->menu = eina_stringshare_add(state);
        list_data2->blur = eina_stringshare_add(blur);
//         list_data2->cat = eina_stringshare_add(elm_object_text_get(cat));

        t = time(NULL);
        ts = localtime(&t);
        snprintf(last_modified,
                 sizeof(last_modified),
                 "%i%02i%02iT%02i%02i%02iZ",
                 ts->tm_year + 1900,
                 ts->tm_mon + 1,
                 ts->tm_mday,
                 ts->tm_hour,
                 ts->tm_min,
                 ts->tm_sec);

        if (list_data2->Note_Sync_Data.last_modified == NULL ||
            !strcmp(list_data2->Note_Sync_Data.last_modified, "")) {
          list_data2->Note_Sync_Data.last_modified =
            eina_stringshare_add(last_modified);
        } else {
          if (entry_notecontent != NULL && list_data2->note_text != NULL) {
            if (strcmp(list_data2->note_text,
                       elm_object_text_get(entry_notecontent))) {
              list_data2->note_text =
                eina_stringshare_add(elm_object_text_get(entry_notecontent));
              list_data2->Note_Sync_Data.last_modified =
                eina_stringshare_add(last_modified);
                list_data2->Note_Sync_Data.online = (int*)0;
              printf("INHALT CHANGED\n");
            }
          }
          if (entry_title != NULL && list_data2->note_name != NULL) {
            if (strcmp(list_data2->note_name,
                       elm_object_text_get(entry_title))) {
              list_data2->note_name =
                eina_stringshare_add(elm_object_text_get(entry_title));
              list_data2->Note_Sync_Data.last_modified =
                eina_stringshare_add(last_modified);
                list_data2->Note_Sync_Data.online = (int*)0;
              printf("TITEL CHANGED\n");
            }
          }
        }

        if (list_data2->Note_Sync_Data.prodid == NULL ||
            !strcmp(list_data2->Note_Sync_Data.prodid, ""))
          list_data2->Note_Sync_Data.prodid = eina_stringshare_add("-//Enotes");

        if (list_data2->Note_Sync_Data.created == NULL ||
            !strcmp(list_data2->Note_Sync_Data.created, ""))
          list_data2->Note_Sync_Data.created =
            eina_stringshare_add(last_modified);

        if (list_data2->Note_Sync_Data.status == NULL ||
            !strcmp(list_data2->Note_Sync_Data.status, ""))
          list_data2->Note_Sync_Data.status =
            eina_stringshare_add("NEEDS-ACTION");

        if (list_data2->Note_Sync_Data.etag == NULL ||
            !strcmp(list_data2->Note_Sync_Data.etag, ""))
          list_data2->Note_Sync_Data.etag = eina_stringshare_add("");

        if (list_data2->Note_Sync_Data.summary == NULL ||
            !strcmp(list_data2->Note_Sync_Data.summary, ""))
          list_data2->Note_Sync_Data.summary =
            eina_stringshare_add(list_data2->note_name);

        if (list_data2->Note_Sync_Data.percent_complete == NULL ||
            !strcmp(list_data2->Note_Sync_Data.percent_complete, ""))
          list_data2->Note_Sync_Data.percent_complete =
            eina_stringshare_add("");

        if (list_data2->Note_Sync_Data.description == NULL ||
            !strcmp(list_data2->Note_Sync_Data.description, ""))
          list_data2->Note_Sync_Data.description =
            eina_stringshare_add(list_data2->note_text);

        if (list_data2->Note_Sync_Data.dtstamp == NULL ||
            !strcmp(list_data2->Note_Sync_Data.dtstamp, ""))
          list_data2->Note_Sync_Data.dtstamp = eina_stringshare_add("");

        if (list_data2->Note_Sync_Data.href == NULL ||
            !strcmp(list_data2->Note_Sync_Data.href, ""))
          list_data2->Note_Sync_Data.href = eina_stringshare_add("");
      }
    }
  }
  _save_notes_eet();
}

static void
_enotes_move(void* data,
             Evas_Object* obj EINA_UNUSED,
             const char* emission EINA_UNUSED,
             const char* source EINA_UNUSED)
{
  elm_win_move_resize_start(data, ELM_WIN_MOVE_RESIZE_MOVE);
}

static void
_enotes_resize(void* data EINA_UNUSED,
               Evas_Object* obj EINA_UNUSED,
               const char* emission EINA_UNUSED,
               const char* source EINA_UNUSED)
{
  elm_win_move_resize_start(
    data, ELM_WIN_MOVE_RESIZE_BOTTOM | ELM_WIN_MOVE_RESIZE_RIGHT);
}

unsigned int
createHEX(int r, int g, int b, int a)
{
  return ((r & 0xff) << 24) + ((g & 0xff) << 16) + ((b & 0xff) << 8) +
         (a & 0xff);
}
/*

struct RGBA createRGBA(const char color)
{
    struct RGBA {
        int r;
        int g;
        int b;
        int a;
    }rgbColor;

    char *str = "ffba00ff";

    sscanf(str, "%02x%02x%02x%02x", &rgbColor.r, &rgbColor.g, &rgbColor.b,
&rgbColor.a); printf("rgbColor %i %i %i %i\n", rgbColor.r, rgbColor.g,
rgbColor.b, rgbColor.a);

    return rgbColor;


}*/

static void
_colorselector_changed_cb(void* data,
                          Evas_Object* obj,
                          void* event_info EINA_UNUSED)
{
  Eina_List* color_change = data;
  Evas_Object* background = eina_list_nth(color_change, 1);
  Evas_Object* entry_title = eina_list_nth(color_change, 2);
  Evas_Object* entry_notecontent = eina_list_nth(color_change, 3);
  int* textcolor = eina_list_nth(color_change, 4);
  Evas_Object* tg = eina_list_nth(color_change, 6);

  int r, g, b, a;

  elm_colorselector_color_get(obj, &r, &g, &b, &a);

  if (!elm_check_state_get(tg)) {
    // 			r = (r * a) / 255;
    // 			g = (g * a) / 255;
    // 			b = (b * a) / 255;
    // 			a = a;
    evas_color_argb_premul(a, &r, &g, &b);
    evas_object_color_set(background, r, g, b, a);
  } else {
    char buf[PATH_MAX];
    char buf1[PATH_MAX];
    char buf2[PATH_MAX];

    snprintf(buf1,
             sizeof(buf1),
             "%d %d %d %d",
             (r & 0xff) >> 24,
             (g & 0xff) >> 16,
             (b & 0xff) >> 8,
             a & 0xff);

    unsigned int rgba = createHEX(r, g, b, a);

    snprintf(buf,
             sizeof(buf),
             "DEFAULT='font=Sans:style=Regular color=#%06x font_size=16'",
             rgba);
    snprintf(buf2,
             sizeof(buf2),
             "DEFAULT='font=Sans:style=Regular color=#%06x font_size=%i'",
             rgba,
             *textcolor);

    elm_entry_text_style_user_push(entry_title, buf);
    elm_entry_text_style_user_push(entry_notecontent, buf2);
  }
}

static void
_enotes_del_request(void* data,
                    Evas_Object* obj EINA_UNUSED,
                    void* event_info EINA_UNUSED)
{
  //     Eina_List *list_values = data;
  //     Evas_Object *win = eina_list_nth(list_values, 2);
  //     int *del_id = eina_list_nth(list_values, 3);

  _del_local_data(data, NULL, NULL);
}

void
_enotes_del_local(void* data,
                  Evas_Object* obj EINA_UNUSED,
                  void* event_info EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* win = eina_list_nth(list_values, 2);
  int* del_id = eina_list_nth(list_values, 3);
  Eina_List* l;
  Eina_List* l1;
  Note* list_data2;

  //     delete data out of master eina_list which holds all notes

  EINA_LIST_FOREACH(note_list, l, list_data2)
  {
    if (list_data2->id == *del_id) {
      note_list = eina_list_remove(note_list, list_data2);
    }
  }

  // delete Evas_Objects out of list for "all enotes enotes_all_objects_list"
  EINA_LIST_FOREACH(enotes_all_objects_list, l, l1)
  {
    int* id = eina_list_nth(l1, 8);
    if (*id == *del_id) {
      enotes_all_objects_list = eina_list_remove(enotes_all_objects_list, l1);
    }
  }
  printf("note_list Count %i\n", eina_list_count(note_list));
  printf("enotes_all_objects_list Count %i\n",
         eina_list_count(enotes_all_objects_list));

  if (eina_list_count(note_list) == 0) {
    _enotes_new();
  }

  save_enotes_all_objects(NULL, NULL, NULL, "0");

  evas_object_hide(win);
}

static void
_enotes_iconify(void* data,
                Evas_Object* obj EINA_UNUSED,
                const char* emission EINA_UNUSED,
                const char* source EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* win = eina_list_nth(list_values, 2);

  elm_win_iconified_set(win, 1);

  save_enotes_all_objects(data, NULL, NULL, "3");
}

static void
_enotes_sticky(void* data,
               Evas_Object* obj EINA_UNUSED,
               const char* emission EINA_UNUSED,
               const char* source EINA_UNUSED)
{
  Eina_Bool flag = elm_win_sticky_get(data);
  if (!flag) {
    elm_win_sticky_set(data, EINA_TRUE);
    elm_win_layer_set(data, 6);
  } else {
    elm_win_sticky_set(data, EINA_FALSE);
    elm_win_layer_set(data, 4);
  }
}

void
_hide_entry_notecontent(void* data,
                        Evas_Object* obj EINA_UNUSED,
                        const char* emission EINA_UNUSED,
                        const char* source EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* entry_notecontent = eina_list_nth(list_values, 0);
  Evas_Object* ly = eina_list_nth(list_values, 5);

  elm_object_part_content_unset(ly, "content");
  evas_object_hide(entry_notecontent);
}

void
_show_entry_notecontent(void* data,
                        Evas_Object* obj EINA_UNUSED,
                        const char* emission EINA_UNUSED,
                        const char* source EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* entry_notecontent = eina_list_nth(list_values, 0);
  Evas_Object* ly = eina_list_nth(list_values, 5);

  elm_object_part_content_set(ly, "content", entry_notecontent);
  evas_object_show(entry_notecontent);
}

void
_swallow_cs(void* data,
            Evas_Object* obj EINA_UNUSED,
            const char* emission EINA_UNUSED,
            const char* source EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* ly = eina_list_nth(list_values, 5);
  Evas_Object* bx = eina_list_nth(list_values, 9);

  save_enotes_all_objects(list_values, NULL, NULL, "2");

  elm_object_part_content_set(ly, "color_swallow", bx);

  save_enotes_all_objects(list_values, NULL, NULL, "5");

  _delete_dialogs_cs(NULL, NULL, NULL, NULL);
}

void
_unswallow_cs(void* data,
              Evas_Object* obj EINA_UNUSED,
              const char* emission EINA_UNUSED,
              const char* source EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* win = eina_list_nth(list_values, 2);
  int* id = eina_list_nth(list_values, 3);
  Evas_Object* ly = eina_list_nth(list_values, 5);

  elm_object_part_content_unset(ly, "color_swallow");

  _delete_dialogs_cs(NULL, NULL, NULL, NULL);

  Eina_List* l;
  Note* list_data2;
  EINA_LIST_FOREACH(note_list, l, list_data2)
  {
    if (list_data2->id == *id) {
      if ((list_data2->h_m != 0) /* || (list_data2->w_m != 0)*/) {
        int w, h;
        evas_object_geometry_get(win, NULL, NULL, &w, &h);
        if ((h == list_data2->h_cstmp) || (w == list_data2->w_cstmp)) {
          evas_object_resize(win, list_data2->w_cs, list_data2->h_cs);
        }
      }
    }
  }
}

void
resize_menu_on(void* data,
               Evas_Object* obj EINA_UNUSED,
               const char* emission EINA_UNUSED,
               const char* source EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* win = eina_list_nth(list_values, 2);
  int* id = eina_list_nth(list_values, 3);

  Eina_List* l;
  Note* list_data2;

  save_enotes_all_objects(list_values, NULL, NULL, "1");

  EINA_LIST_FOREACH(note_list, l, list_data2)
  {
    if (list_data2->id == *id) {
      if (list_data2->h_m <= 270) {
        evas_object_resize(win, list_data2->w_m, 270);
      }
    }
  }

  save_enotes_all_objects(list_values, NULL, NULL, "4");
}

void
resize_menu_off(void* data,
                Evas_Object* obj EINA_UNUSED,
                const char* emission EINA_UNUSED,
                const char* source EINA_UNUSED)
{
  Eina_List* list_values = data;
  Evas_Object* win = eina_list_nth(list_values, 2);
  int* id = eina_list_nth(list_values, 3);

  Eina_List* l;
  Note* list_data2;
  _unswallow_cs(data, NULL, NULL, NULL);
  _show_entry_notecontent(data, NULL, NULL, NULL);

  EINA_LIST_FOREACH(note_list, l, list_data2)
  {
    if (list_data2->id == *id) {
      if (list_data2->h_m != 0) {
        int w, h;
        evas_object_geometry_get(win, NULL, NULL, &w, &h);
        if (h == list_data2->h_mtmp) {
          evas_object_resize(win, list_data2->w_m, list_data2->h_m);
        }
      }
    }
  }
}

void
_enotes_exit(void* data EINA_UNUSED,
             Evas_Object* obj EINA_UNUSED,
             void* event_info EINA_UNUSED)
{
  save_enotes_all_objects(NULL, NULL, NULL, "0");
  eina_list_free(enotes_all_objects_list);
  _my_conf_descriptor_shutdown();
  ecore_file_unlink(enotes_running);
  eina_strbuf_free(home_file);
  //     elm_shutdown();
  elm_exit();
}

static void
_close_final(void* data)
{

  Evas_Object *notify, *bx, *bxv, *o;

  notify = elm_notify_add(data);

  bx = elm_box_add(notify);
  elm_box_horizontal_set(bx, EINA_FALSE);

  o = elm_label_add(bx);
  elm_object_text_set(o, gettext("Close enotes?"));
  evas_object_show(o);
  elm_box_pack_end(bx, o);

  bxv = elm_box_add(notify);
  elm_box_horizontal_set(bxv, EINA_TRUE);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("No"));
  evas_object_smart_callback_add(o, "clicked", _popup_close_cb, notify);
  evas_object_show(o);
  elm_box_pack_end(bxv, o);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("Yes"));
  evas_object_smart_callback_add(o, "clicked", _enotes_exit, NULL);
  evas_object_show(o);
  elm_box_pack_end(bxv, o);
  evas_object_show(bxv);

  elm_box_pack_end(bx, bxv);

  evas_object_show(bx);
  elm_object_content_set(notify, bx);

  if (popup_backup_tmp != NULL)
    _backup_to_file_all_dismiss_cb(popup_backup_tmp, NULL, NULL);

  if (popup_close_tmp != NULL)
    _popup_close_cb(popup_close_tmp, NULL, NULL);

  if (popup_delete_tmp != NULL)
    _popup_close_cb(popup_delete_tmp, NULL, NULL);

  popup_close_tmp = notify;
  evas_object_show(notify);
}

static void
_close_all(void* data,
           Evas_Object* obj EINA_UNUSED,
           void* event_info EINA_UNUSED)
{
  _close_final(data);
}

static void
_close_all1(void* data,
            Evas_Object* obj EINA_UNUSED,
            const char* emission EINA_UNUSED,
            const char* source EINA_UNUSED)
{
  _close_final(data);
}

static void
_close_all2(void* data,
            Evas* e EINA_UNUSED,
            Evas_Object* obj EINA_UNUSED,
            void* event_info EINA_UNUSED)
{
  _close_final(data);
}

void
key_down(void* data,
         Evas* e EINA_UNUSED,
         Evas_Object* obj EINA_UNUSED,
         void* event_info)
{
  Eina_List* list_values = data;
  Evas_Object* entry_notecontent = eina_list_nth(list_values, 0);
  Evas_Object* win = eina_list_nth(list_values, 2);
  Evas_Object* ly = eina_list_nth(list_values, 5);

  Evas_Object* edje_obj = elm_layout_edje_get(ly);

  Evas_Event_Key_Down* ev = event_info;
  Eina_Bool ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");
  const char* k = ev->key;

  if (ctrl) {
    if (!strcmp(k, "q")) // close enotes
    {
      _close_all(win, NULL, NULL);
    }
    if (!strcmp(k, "m")) // show menu
    {
      edje_object_signal_emit(edje_obj, "menu_toogle", "menu_toogle");
    }
    if (!strcmp(k, "h")) // show menu
    {
      _hide_show_all_notes(NULL, NULL, NULL);
    }
  }

  if (!strcmp(k, "Escape")) // dismiss all dialogs and show note
  {
    _delete_dialogs_cs(edje_obj, NULL, NULL, NULL);
  }

  if (!strcmp(k, "F1")) // show help
  {
    enotes_win_help(list_values, NULL, NULL, NULL);
  }

  if (!strcmp(k, "F2")) // blur
  {
    if (strcmp(edje_object_part_state_get(edje_obj, "blur", NULL), "default")) {
      edje_object_signal_emit(edje_obj, "blur_off", "");
    } else {
      edje_object_signal_emit(edje_obj, "blur_on", "");
    }
  }

  if (!strcmp(k, "F3")) // insert done image
  {
    _insert_done_icon(entry_notecontent, NULL, NULL, NULL);
  }

  if (!strcmp(k, "F4")) // insert important image
  {
    _insert_open_icon(entry_notecontent, NULL, NULL, NULL);
  }

  if (!strcmp(k, "F5")) // new note
  {
    _enotes_new();
  }

  if (!strcmp(k, "F6")) // delete note
  {
    _popup_delete_cb(list_values, NULL, NULL, NULL);
  }

  if (!strcmp(k, "F7")) // set sticky and "always on top"
  {
    _enotes_sticky(win, NULL, NULL, NULL);
  }

  if (!strcmp(k, "F8")) // set iconify
  {
    _enotes_iconify(data, NULL, NULL, NULL);
  }

  if (!strcmp(k, "F9")) // change layout
  {
    _layout_change(data, NULL, NULL, NULL);
  }

  if (!strcmp(k, "F10")) // backup
  {
    _backup_to_file(list_values);
  }
  if (!strcmp(k, "F11")) // Start Syncing
  {
    save_enotes_all_objects(NULL, NULL, NULL, "0");
    _get_online_data(NULL, NULL, NULL);
  }

  if (!strcmp(k, "F12")) // backup
  {
    _open_settings(data, NULL, NULL, NULL);
  }

  if (!strcmp(k, "0")) {
    Eina_List* list_text_tmp = NULL;
    list_text_tmp =
      eina_list_append(list_text_tmp, eina_list_nth(list_values, 0));
    list_text_tmp =
      eina_list_append(list_text_tmp, eina_list_nth(list_values, 6));
    list_text_tmp =
      eina_list_append(list_text_tmp, eina_list_nth(list_values, 7));
    list_text_tmp =
      eina_list_append(list_text_tmp, eina_list_nth(list_values, 4));
    list_text_tmp =
      eina_list_append(list_text_tmp, eina_list_nth(list_values, 5));

    _textsize_change_cb_normal(list_text_tmp, NULL, NULL, NULL);

    eina_list_free(list_text_tmp);
  }

  // return ECORE_CALLBACK_PASS_ON;
}

void
_mouse_wheel(void* data,
             Evas* e EINA_UNUSED,
             Evas_Object* obj EINA_UNUSED,
             void* event_info)
{
  Evas_Event_Mouse_Wheel* ev = event_info;
  Eina_Bool ctrl = evas_key_modifier_is_set(ev->modifiers, "Control");

  if (ctrl) {
    if (ev->z == -1) {
      _textsize_change_cb_increase(data, NULL, NULL, NULL);
    } else {
      _textsize_change_cb_decrease(data, NULL, NULL, NULL);
    }
  }
}

static void
_popup_delete_cb(void* data,
                 Evas_Object* obj EINA_UNUSED,
                 const char* emission EINA_UNUSED,
                 const char* source EINA_UNUSED)
{
  Eina_List* list_delete = data;
  Evas_Object* win = eina_list_nth(list_delete, 2);
  Evas_Object* entry_title = eina_list_nth(list_delete, 4);
  char buf[PATH_MAX];
  snprintf(buf,
           sizeof(buf),
           "<custom color=#ff0000>Delete</custom> note<br>\'%s\'",
           elm_object_text_get(entry_title));
  Evas_Object *notify, *bx, *bxv, *o, *o1;

  notify = elm_notify_add(win);

  bx = elm_box_add(notify);
  elm_box_horizontal_set(bx, EINA_FALSE);
  //     evas_object_smart_callback_add(notify,
  //     "block,clicked",_restack_block_clicked_cb, NULL);

  o = elm_label_add(bx);
  elm_object_text_set(o, gettext(buf));
  evas_object_show(o);
  elm_box_pack_end(bx, o);

  bxv = elm_box_add(notify);
  elm_box_horizontal_set(bxv, EINA_TRUE);

  o1 = elm_button_add(bxv);
  elm_object_text_set(o1, gettext("No"));
  evas_object_smart_callback_add(o1, "clicked", _popup_close_cb, notify);
  evas_object_show(o1);
  elm_box_pack_end(bxv, o1);

  o = elm_button_add(bxv);
  elm_object_text_set(o, gettext("Yes"));
  evas_object_smart_callback_add(o, "clicked", _enotes_del_request, data);
  evas_object_show(o);
  elm_box_pack_end(bxv, o);
  evas_object_show(bxv);

  elm_box_pack_end(bx, bxv);

  evas_object_show(bx);
  elm_object_content_set(notify, bx);

  if (popup_backup_tmp != NULL)
    _backup_to_file_all_dismiss_cb(popup_backup_tmp, NULL, NULL);

  if (popup_close_tmp != NULL)
    _popup_close_cb(popup_close_tmp, NULL, NULL);

  if (popup_delete_tmp != NULL)
    _popup_close_cb(popup_delete_tmp, NULL, NULL);

  popup_delete_tmp = notify;
  evas_object_show(notify);

  elm_object_focus_set(o1, EINA_TRUE);
}

static void
_tg_changed_cb(void* data, Evas_Object* obj, void* event_info EINA_UNUSED)
{
  Eina_List* tg_change = data;
  Evas_Object* background = eina_list_nth(tg_change, 0);
  Evas_Object* cs = eina_list_nth(tg_change, 1);
  Evas_Object* entry_title = eina_list_nth(tg_change, 2);
  int r, g, b, a;

  if (!elm_check_state_get(obj)) {
    evas_object_color_get(background, &r, &g, &b, &a);
    evas_color_argb_premul(a, &r, &g, &b);
    elm_colorselector_color_set(cs, r, g, b, a);
  } else {
    const char* str = get_text_color1(entry_title);
    sscanf(str, "%02x%02x%02x%02x", &r, &g, &b, &a);
    evas_color_argb_premul(a, &r, &g, &b);
    elm_colorselector_color_set(cs, r, g, b, a);
  }
}

static void
enotes_win_setup(Note* list_data)
{
  Evas_Object *win, *ly, *entry_notecontent, *entry_title, *edje_obj, *o;

  char buf[PATH_MAX];
  int r, g, b, a;

  // create window and set parms //
  win = elm_win_add(NULL, "Enotes", ELM_WIN_BASIC);
  elm_win_borderless_set(win, EINA_TRUE);
  elm_win_alpha_set(win, EINA_TRUE);
  elm_win_title_set(win, list_data->note_name);

  elm_win_focus_highlight_enabled_set(win, EINA_FALSE);
  elm_object_focus_set(win, EINA_TRUE);


  // LAYOUT CREATE //
  ly = elm_layout_add(win);
  snprintf(buf, sizeof(buf), "%s/themes/enotes.edj", elm_app_data_dir_get());
  elm_layout_file_set(ly, buf, list_data->theme);
  evas_object_size_hint_weight_set(ly, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

  //     elm_layout_part_cursor_set(ly, "tape", ELM_CURSOR_HAND2);
  //     elm_layout_part_cursor_set(ly, "rect_over", ELM_CURSOR_HAND2);
  //     elm_layout_part_cursor_set(ly, "shadow_corner",
  //     ELM_CURSOR_BOTTOM_RIGHT_CORNER);

  snprintf(buf,
           sizeof(buf),
           "%s/themes/entry_theme.edj",
           elm_app_data_dir_get()); // OVERLAY OF ENTRY SCROLLER BUG
  elm_theme_overlay_add(NULL, buf);
  
  elm_win_resize_object_add(win, ly);

  evas_object_show(ly);
  // LAYOUT CREATE END//

  // create edje_obj for callbacks //
  edje_obj = elm_layout_edje_get(ly);

  // set blur state
  if ((list_data->blur) && (!strcmp(list_data->blur, "on")))
    edje_object_signal_emit(edje_obj, "blur_on", "");

  // set menu visible
  if ((list_data->menu) && (!strcmp(list_data->menu, "on")))
    edje_object_signal_emit(edje_obj, "mouse_in_rect_overleft_in", "");

  // Window icon
  Evas_Object* icon;
  icon = evas_object_image_add(evas_object_evas_get(win));
  evas_object_image_file_set(icon, buf, "background");
  elm_win_icon_object_set(win, icon);
  evas_object_show(icon);

  // create elm_entry //
  entry_notecontent = elm_entry_add(win);
  elm_entry_line_wrap_set(entry_notecontent, ELM_WRAP_WORD);
  elm_object_text_set(entry_notecontent, list_data->note_text);
  elm_entry_scrollable_set(entry_notecontent, EINA_TRUE);
  evas_object_size_hint_weight_set(
    entry_notecontent, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(
    entry_notecontent, EVAS_HINT_FILL, EVAS_HINT_FILL);
  evas_object_show(entry_notecontent);
  elm_object_part_content_set(ly, "content", entry_notecontent);

  // create elm_entry name//
  entry_title = elm_entry_add(win);
  elm_entry_line_wrap_set(entry_title, ELM_WRAP_WORD);
  elm_object_text_set(entry_title, list_data->note_name);
  evas_object_size_hint_weight_set(
    entry_title, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(entry_title, 1.0, 1.0);
  elm_object_part_content_set(ly, "name_content", entry_title);

  // BACKGROUND SELECT //
  Evas_Object* background;

  background = evas_object_rectangle_add(evas_object_evas_get(win));
  evas_object_size_hint_weight_set(
    background, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(background, EVAS_HINT_FILL, EVAS_HINT_FILL);

  /* Fix Alpha pre multiplication by edje */
  r = list_data->color_r;
  g = list_data->color_g;
  b = list_data->color_b;

  a = list_data->color_a;

  evas_color_argb_premul(a, &r, &g, &b);
  evas_object_color_set(background, r, g, b, a);

  elm_object_part_content_set(ly, "note_swallow", background);
  // BACKGROUND SELECT END//

  Evas_Object *bx, *cs, *bt, *bt1;
  Evas_Object *tg, *cat_text;

  Eina_List* tg_change = NULL;
  tg_change = eina_list_append(tg_change, background);

  bx = elm_box_add(win);
  evas_object_size_hint_weight_set(bx, EVAS_HINT_EXPAND, 0);
  evas_object_size_hint_align_set(bx, EVAS_HINT_FILL, 0);
  //    evas_object_size_hint_min_set(bx, 400, 400);
  evas_object_show(bx);

  tg = elm_check_add(bx);
  elm_object_style_set(tg, "toggle");
  elm_object_text_set(tg, gettext("Change color for: "));
  elm_object_part_text_set(tg, "on", gettext("Textcolor"));
  elm_object_part_text_set(tg, "off", gettext("Background"));
  evas_object_smart_callback_add(tg, "changed", _tg_changed_cb, tg_change);
  elm_box_pack_end(bx, tg);
  evas_object_show(tg);

  // COLOR SELECT //
  cs = elm_colorselector_add(bx);
  evas_object_size_hint_weight_set(cs, EVAS_HINT_EXPAND, 0);
  evas_object_size_hint_align_set(cs, EVAS_HINT_FILL, 0);

  elm_colorselector_mode_set(cs, ELM_COLORSELECTOR_BOTH);

  elm_colorselector_palette_name_set(cs, "enotes");

  elm_colorselector_palette_color_add(cs, 253, 232, 82, 255);
  elm_colorselector_palette_color_add(cs, 255, 186, 0, 255);
  elm_colorselector_palette_color_add(cs, 223, 147, 37, 255);
  elm_colorselector_palette_color_add(cs, 238, 119, 99, 255);
  elm_colorselector_palette_color_add(cs, 164, 182, 166, 255);
  elm_colorselector_palette_color_add(cs, 146, 175, 29, 255);
  elm_colorselector_palette_color_add(cs, 41, 68, 59, 255);
  elm_colorselector_palette_color_add(cs, 0, 109, 128, 255);
  elm_colorselector_palette_color_add(cs, 11, 54, 71, 255);
  elm_colorselector_palette_color_add(cs, 41, 46, 68, 255);
  elm_colorselector_palette_color_add(cs, 116, 13, 14, 255);
  elm_colorselector_palette_color_add(cs, 101, 33, 44, 255);
  elm_colorselector_palette_color_add(cs, 132, 50, 118, 255);
  elm_colorselector_palette_color_add(cs, 68, 40, 55, 255);
  elm_colorselector_palette_color_add(cs, 63, 37, 60, 255);
  elm_colorselector_palette_color_add(cs, 115, 87, 63, 255);
  elm_colorselector_palette_color_add(cs, 66, 70, 73, 255);
  elm_colorselector_palette_color_add(cs, 255, 255, 255, 255);
  elm_colorselector_palette_color_add(cs, 0, 0, 0, 255);
  evas_color_argb_premul(a, &r, &g, &b);
  elm_colorselector_color_set(cs, r, g, b, a);

  elm_box_pack_end(bx, cs);
  evas_object_show(cs);

  tg_change = eina_list_append(tg_change, cs);
  tg_change = eina_list_append(tg_change, entry_title);

  // create List for Evas_Objects and other "win" values
  Eina_List* list_values = NULL;
  list_values = eina_list_append(list_values, entry_notecontent);
  list_values = eina_list_append(list_values, cs);
  list_values = eina_list_append(list_values, win);
  list_values = eina_list_append(list_values, &list_data->id);
  list_values = eina_list_append(list_values, entry_title);
  list_values = eina_list_append(list_values, ly);
  list_values = eina_list_append(list_values, &list_data->text_color);
  list_values = eina_list_append(list_values, &list_data->text_size);
  list_values = eina_list_append(list_values, background);
  list_values = eina_list_append(list_values, bx);

  bt = elm_button_add(bx);
  elm_object_text_set(bt, gettext("Save colorset as default"));
  evas_object_size_hint_align_set(bt, 0.5, 0);
  evas_object_smart_callback_add(bt, "clicked", _bt_colorset_save, list_values);
  elm_box_pack_end(bx, bt);
  evas_object_show(bt);

  bt1 = elm_button_add(bx);
  elm_object_text_set(bt1, gettext("Set colorset to all notes"));
  evas_object_size_hint_align_set(bt1, 0.5, 0);

  evas_object_smart_callback_add(
    bt1, "clicked", _bt_colorset_to_all, list_values);
  elm_box_pack_end(bx, bt1);
  evas_object_show(bt1);
  
  o = elm_separator_add(bx);
  evas_object_size_hint_weight_set(o, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  elm_separator_horizontal_set(o, EINA_TRUE);
  elm_box_pack_end(bx, o);
  evas_object_show(o);

  cat_text = elm_label_add(bx);
  elm_object_text_set(cat_text, "select categories for THIS note");
  elm_box_pack_end(bx, cat_text);
  evas_object_show(cat_text);
  
  Evas_Object *list1 = elm_list_add(bx);
  elm_list_multi_select_set(list1, EINA_TRUE);
  elm_list_multi_select_mode_set(list1, ELM_OBJECT_MULTI_SELECT_MODE_DEFAULT);
  evas_object_size_hint_weight_set(list1, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
  evas_object_size_hint_align_set(list1, EVAS_HINT_FILL, EVAS_HINT_FILL);

  elm_list_mode_set(list1, ELM_LIST_EXPAND);
   
  Eina_List *l;
  Elm_Object_Item *lit;
  EINA_LIST_FOREACH(cat_list, l, lit)
   {
      elm_list_item_append(list1, elm_object_item_text_get(lit), NULL, NULL, NULL, NULL);
   }
//   elm_list_item_append(list1, "Privat", NULL, NULL, NULL, list1);
//   elm_list_item_append(list1, "Work", NULL, NULL, NULL, list1);
//   elm_list_item_append(list1, "EJW FDS", NULL, NULL, NULL, list1);
//   elm_list_item_append(list1, "Pfalzgrafenweiler", NULL, NULL, NULL, list1);
//   elm_list_item_append(list1, "Enotes", NULL, NULL, NULL, list1);
  elm_list_go(list1);

  evas_object_show(list1);  
  elm_box_pack_end(bx, list1);

  elm_object_part_content_set(ly, "color_swallow", bx);

  // COLOR SELECT END //

  // CALLBACK NEW/ICONIFY/DELETE NOTES //
  edje_object_signal_callback_add(edje_obj, "new", "new", _enotes_new, NULL);
  edje_object_signal_callback_add(
    edje_obj, "iconify", "iconify", _enotes_iconify, list_values);

  // create List for Evas_Object and other "win" delete request
  Eina_List* list_delete = NULL;
  list_delete = eina_list_append(list_delete, win);
  list_delete = eina_list_append(list_delete, &list_data->id);

  edje_object_signal_callback_add(
    edje_obj, "delete", "delete", _popup_delete_cb, list_values);

  // CALLBACK für theme
  //     edje_object_signal_callback_add(edje_obj, "layout", "layout",
  //     _layout_change, list_values);

  // CALLBACK was passiert wenn das Fenster geschloss wird
  evas_object_smart_callback_add(win, "delete,request", _enotes_exit, NULL);

  // create List for text size and text color
  Eina_List* list_text = NULL;
  list_text = eina_list_append(list_text, entry_notecontent);
  list_text = eina_list_append(list_text, list_data->tcolor);
  list_text = eina_list_append(list_text, &list_data->text_size);
  list_text = eina_list_append(list_text, entry_title);
  list_text = eina_list_append(list_text, ly);

  // CALLBACK für die Schriftgröße
  char buf_entry_notecontent[PATH_MAX];
  char buf_entry_title[PATH_MAX];

  snprintf(buf_entry_notecontent,
           sizeof(buf_entry_notecontent),
           "DEFAULT='font=Sans:style=Regular color=%s font_size=%i'",
           list_data->tcolor,
           list_data->text_size);
  snprintf(buf_entry_title,
           sizeof(buf_entry_title),
           "DEFAULT='font=Sans:style=Regular color=%s font_size=16'",
           list_data->tcolor);

  elm_entry_text_style_user_push(entry_notecontent, buf_entry_notecontent);
  elm_entry_text_style_user_push(entry_title, buf_entry_title);

  //     edje_object_signal_callback_add(edje_obj, "size", "normal",
  //     _textsize_change_cb_normal, list_text);
  //     edje_object_signal_callback_add(edje_obj, "size", "increase",
  //     _textsize_change_cb_increase, list_text);
  //     edje_object_signal_callback_add(edje_obj, "size", "decrease",
  //     _textsize_change_cb_decrease, list_text);

  // CALLBACK für done Icon
  edje_object_signal_callback_add(
    edje_obj, "help", "help", enotes_win_help, list_values);

  // CALLBACK für _unswallow_cs
  edje_object_signal_callback_add(
    edje_obj, "unswallow_cs", "1", _unswallow_cs, list_values);
  edje_object_signal_callback_add(
    edje_obj, "unswallow_cs", "1", _show_entry_notecontent, list_values);

  // CALLBACK für _swallow_cs
  edje_object_signal_callback_add(
    edje_obj, "swallow_cs", "1", _swallow_cs, list_values);
  edje_object_signal_callback_add(
    edje_obj, "swallow_cs", "1", _hide_entry_notecontent, list_values);
  edje_object_signal_callback_add(
    edje_obj, "swallow_cs", "1", _disable_bt1, bt1);

  // CALLBACK für resize_menu_on
  edje_object_signal_callback_add(
    edje_obj, "resize_menu_on", "1", resize_menu_on, list_values);

  // CALLBACK für resize_menu_on
  edje_object_signal_callback_add(
    edje_obj, "resize_menu_off", "1", resize_menu_off, list_values);

  // CALLBACK für close all / close enotes
  edje_object_signal_callback_add(edje_obj, "close", "close", _close_all1, win);

  // CALLBACK for move note
  edje_object_signal_callback_add(edje_obj, "move", "move", _enotes_move, win);

  // CALLBACK for resize note
  edje_object_signal_callback_add(
    edje_obj, "resize", "resize", _enotes_resize, win);

  // change color callback
  // create List for radio choose and cs/text
  Eina_List* color_change = NULL;
  color_change = eina_list_append(color_change, win);
  color_change = eina_list_append(color_change, background);
  color_change = eina_list_append(color_change, entry_title);
  color_change = eina_list_append(color_change, entry_notecontent);
  color_change = eina_list_append(color_change, &list_data->text_size);
  color_change = eina_list_append(color_change, list_values);
  color_change = eina_list_append(color_change, tg);

  evas_object_smart_callback_add(
    cs, "changed", _colorselector_changed_cb, color_change);

  // save note_text on mouse out
  //     evas_object_event_callback_add(entry_notecontent,
  //     EVAS_CALLBACK_MOUSE_OUT, save_enotes_all_objects, NULL);
  // 	 evas_object_event_callback_add(entry_title, EVAS_CALLBACK_MOUSE_OUT,
  // save_enotes_all_objects, NULL);
  //     evas_object_event_callback_add(edje_obj, EVAS_CALLBACK_MOUSE_OUT,
  //     save_enotes_all_objects, NULL);
  evas_object_event_callback_add(
    win, EVAS_CALLBACK_MOUSE_OUT, save_enotes_all_objects, NULL);

  // callback for key down/hide/show text //
  // create List for text size and text color
  Eina_List* list_keydown = NULL;
  list_keydown = eina_list_append(list_keydown, win);
  list_keydown = eina_list_append(list_keydown, &list_data->id);
  list_keydown = eina_list_append(list_keydown, entry_notecontent);
  list_keydown = eina_list_append(list_keydown, edje_obj);

  evas_object_event_callback_add(
    ly,
    EVAS_CALLBACK_KEY_DOWN,
    key_down,
    list_values); // TODO: CALLBACK sollte auf obj win gehen. bug in efl

  evas_object_event_callback_add(
    entry_notecontent, EVAS_CALLBACK_MOUSE_WHEEL, _mouse_wheel, list_text);

  elm_entry_item_provider_append(entry_notecontent, item_provider, NULL);
  elm_entry_item_provider_append(entry_title, item_provider, NULL);

  /// TODO CATEGORIES COMPARE
  //     if(strstr(list_data->cat, "Privat"))
  //     {
  evas_object_show(win);
  //     }

  if (list_data->sticky)
    _enotes_sticky(win, NULL, NULL, NULL);

  if (list_data->iconify) {
    elm_win_iconified_set(win, 1);
  }

  evas_object_resize(win, list_data->w, list_data->h);
  evas_object_move(win, list_data->x, list_data->y);

  // LISTE NEUE SPEICHERMETHODE
  Eina_List* enotes_all_objects = NULL;
  enotes_all_objects = eina_list_append(enotes_all_objects, win);
  enotes_all_objects = eina_list_append(enotes_all_objects, ly);
  enotes_all_objects = eina_list_append(enotes_all_objects, entry_notecontent);
  enotes_all_objects = eina_list_append(enotes_all_objects, entry_title);
  enotes_all_objects = eina_list_append(enotes_all_objects, background);
  enotes_all_objects = eina_list_append(enotes_all_objects, tg);
  enotes_all_objects = eina_list_append(enotes_all_objects, cs);
  enotes_all_objects = eina_list_append(enotes_all_objects, &list_data->id);
  enotes_all_objects =
    eina_list_append(enotes_all_objects, &list_data->text_size);

  enotes_all_objects_list =
    eina_list_append(enotes_all_objects_list, enotes_all_objects);
}

void
note_online_to_local(Eina_List* new_notes)
{
  Note *list_data1 = NULL, *list_data = NULL;
  //     Eina_List *note_list_tmp = NULL;
  Eina_List *l, *l1, *l2, *l3, *list_data3;
  int s = 0;
  int u = 0;
  Evas_Object *entry_notecontent, *entry_title;
  Eina_Strbuf *last_modified_local, *last_modified_online;
  last_modified_local = eina_strbuf_new();
  last_modified_online = eina_strbuf_new();

  //     note_list_tmp = eina_list_clone(note_list);
  note_list_put = NULL;

  Note* data_get;

  EINA_LIST_FOREACH(new_notes, l, data_get)
  {
    EINA_LIST_FOREACH(note_list, l1, list_data)
    {
      if (strcmp(list_data->Note_Sync_Data.uid, data_get->Note_Sync_Data.uid) ==
          0) {
        //                      printf("SCHLEIFE GLEICHE UID\n");
        eina_strbuf_append(last_modified_local,
                           list_data->Note_Sync_Data.last_modified);
        eina_strbuf_append(last_modified_online,
                           data_get->Note_Sync_Data.last_modified);
        eina_strbuf_replace_all(last_modified_local, "T", "");
        eina_strbuf_replace_all(last_modified_local, "Z", "");
        eina_strbuf_replace_all(last_modified_online, "T", "");
        eina_strbuf_replace_all(last_modified_online, "Z", "");

        if (atol(eina_strbuf_string_get(last_modified_local)) <
            atol(eina_strbuf_string_get(last_modified_online))) {
          EINA_LIST_FOREACH(enotes_all_objects_list, l2, list_data3)
          {
            int* id = eina_list_nth(list_data3, 7);
            if (list_data->id == *id) {
              entry_title = eina_list_nth(list_data3, 3);
              entry_notecontent = eina_list_nth(list_data3, 2);
              elm_object_text_set(entry_title,
                                  data_get->Note_Sync_Data.summary);
              elm_object_text_set(entry_notecontent,
                                  data_get->Note_Sync_Data.description);
            }
          }
          printf("\n\nLOCAL ÜBERSCHREIBEN id:%i name:%s\n",
                 data_get->id,
                 data_get->Note_Sync_Data.summary);
        }
        if (atol(eina_strbuf_string_get(last_modified_local)) >
                   atol(eina_strbuf_string_get(last_modified_online))) {
          //                         printf("\n\nAUF DIE LISTE -> NEUER
          //                         UPLOAD\n");
          note_list_put = eina_list_append(note_list_put, list_data); // upload local data, because last_modified is newer
          printf("\n\nAUF DIE LISTE -> NEUER UPLOAD id:%i inhalt:%s\n",
                 data_get->id,
                 list_data->note_text);
        }
        eina_strbuf_reset(last_modified_local);
        eina_strbuf_reset(last_modified_online);
        list_data->Note_Sync_Data.online = (int*)3;
        u = 1;
      }
    }
    if (u == 1) {
      u = 0;
      continue;
    }

    //       if(u == 0 && list_data->Note_Sync_Data.online == 1)
    //       {
    //          printf("DELETE %i %s\n", data_get->id,
    //          data_get->Note_Sync_Data.summary);
    //       }

    if (u == 0 &&  data_get->Note_Sync_Data.online == (int*)1) {
      printf("\n\nLOCAL NEU ERSTELLEN id:%i name:%s\n",
             data_get->id,
             data_get->Note_Sync_Data.summary);
      Note* defaultnote;
      defaultnote = calloc(1, sizeof(Note));

      //                   printf("\n\nLOCAL NEU ERSTELLEN\n");
      // check for a free ID //
      int n = 0;
      int x1 = 0;
      do {
        int z = 0;
        EINA_LIST_FOREACH(note_list, l3, list_data1)
        {
          if (list_data1->id == n) {
            z = 1;
          }
        }
        if (z == 0) {
          defaultnote->id = n;
          x1 = 2;
        }
        n++;
      } while (x1 != 2);

      defaultnote->x = 200 + s * 50;
      defaultnote->y = 300 + s * 50;
      defaultnote->w = 260;
      defaultnote->h = 270;
      defaultnote->w_m = 260;
      defaultnote->h_m = 270;
      defaultnote->w_cs = 260;
      defaultnote->h_cs = 270;
      defaultnote->note_name =
        eina_stringshare_add(data_get->Note_Sync_Data.summary);
      defaultnote->note_text =
        eina_stringshare_add(data_get->Note_Sync_Data.description);
      defaultnote->Note_Sync_Data.etag =
        eina_stringshare_add(data_get->Note_Sync_Data.etag);
      defaultnote->Note_Sync_Data.href =
        eina_stringshare_add(data_get->Note_Sync_Data.href);
      defaultnote->Note_Sync_Data.uid =
        eina_stringshare_add(data_get->Note_Sync_Data.uid);
      defaultnote->Note_Sync_Data.online = (int*)1;
      defaultnote->text_color = 0;

      if (dcolor_a) {
        defaultnote->tcolor = tcolor_default;
        defaultnote->color_r = dcolor_r;
        defaultnote->color_g = dcolor_g;
        defaultnote->color_b = dcolor_b;
        defaultnote->color_a = dcolor_a;
      } else {
        defaultnote->tcolor = eina_stringshare_add("#ffffff");
        defaultnote->color_r = 40;
        defaultnote->color_g = 75;
        defaultnote->color_b = 99;
        defaultnote->color_a = 255;
      }

      defaultnote->text_size = 11;
      defaultnote->iconify = EINA_FALSE;
      defaultnote->sticky = EINA_FALSE;

      if (eina_list_count(note_list) >= 1)
        defaultnote->menu = eina_stringshare_add("default");
      else
        defaultnote->menu = eina_stringshare_add("on");

      defaultnote->blur = eina_stringshare_add("default");
      defaultnote->theme = eina_stringshare_add("layout1");

      s++;
      note_list = eina_list_append(note_list, defaultnote);
      enotes_win_setup(defaultnote);

      u = 0;
    }
  }

  u = 0;
  _put_local_data(NULL, NULL, NULL);
  //       _put_local_data_online();

  //       eina_list_free(note_list_tmp);
}

void
_enotes_new()
{
  Note* defaultnote;
  defaultnote = calloc(1, sizeof(Note));

  Note* list_data1 = NULL;
  Eina_List* l;
  int x, y;
  int n = 0;
  int x1 = 0;

  ecore_x_pointer_xy_get(ecore_x_window_root_first_get(), &x, &y);

  // check for a free ID //
  do {
    int z = 0;
    EINA_LIST_FOREACH(note_list, l, list_data1)
    {
      if (list_data1->id == n) {
        z = 1;
      }
    }
    if (z == 0) {
      defaultnote->id = n;
      x1 = 2;
    }
    n++;
  } while (x1 != 2);

  list_data1 = eina_list_data_get(eina_list_last(note_list));

  if (clean_start == EINA_TRUE) {
    clean_start = EINA_FALSE;
    defaultnote->note_text = eina_stringshare_add(gettext(
      "Welcome to eNotes! <br><br>"
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
      "Author: Simon Tischer [jf_simon on irc.freenode.net #e.de]"));
    //         if(x)
    //         {
    //             defaultnote->x = x-130;
    //             defaultnote->y = y-3;
    //         }else
    //         {
    defaultnote->x = 200;
    defaultnote->y = 300;
    //         }
    //         defaultnote->x = 200;
    //         defaultnote->y = 300;
    defaultnote->w = 350;
    defaultnote->h = 450;
    defaultnote->w_m = 350;
    defaultnote->h_m = 380;
    defaultnote->w_cs = 350;
    defaultnote->h_cs = 380;
  } else {
    if (x) {
      defaultnote->x = x - 130;
      defaultnote->y = y - 3;
    } else {
      defaultnote->x = 200;
      defaultnote->y = 300;
    }

    defaultnote->note_text = eina_stringshare_add("");
    defaultnote->w = 260;
    defaultnote->h = 270;
    defaultnote->w_m = 260;
    defaultnote->h_m = 270;
    defaultnote->w_cs = 260;
    defaultnote->h_cs = 270;
  }

  defaultnote->note_name = eina_stringshare_add("eNotes: [title]");

  defaultnote->text_color = 0;

  if (dcolor_a) {
    defaultnote->tcolor = tcolor_default;
    defaultnote->color_r = dcolor_r;
    defaultnote->color_g = dcolor_g;
    defaultnote->color_b = dcolor_b;
    defaultnote->color_a = dcolor_a;
  } else {
    defaultnote->tcolor = eina_stringshare_add("#ffffff");
    defaultnote->color_r = 40;
    defaultnote->color_g = 75;
    defaultnote->color_b = 99;
    defaultnote->color_a = 255;
  }

  defaultnote->text_size = 11;
  defaultnote->iconify = EINA_FALSE;
  defaultnote->sticky = EINA_FALSE;

  if (eina_list_count(note_list) >= 1)
    defaultnote->menu = eina_stringshare_add("default");
  else
    defaultnote->menu = eina_stringshare_add("on");

  defaultnote->blur = eina_stringshare_add("default");
  defaultnote->theme = eina_stringshare_add("layout1");

  time_t t;
  struct tm* ts;
  t = time(NULL);
  ts = localtime(&t);
  Eina_Strbuf* newbuf;
  newbuf = eina_strbuf_new();
  eina_strbuf_append_printf(newbuf,
                            "enotes_%i%02i%02iT%02i%02i%02iZ",
                            ts->tm_year + 1900,
                            ts->tm_mon + 1,
                            ts->tm_mday,
                            ts->tm_hour,
                            ts->tm_min,
                            ts->tm_sec);
  defaultnote->Note_Sync_Data.uid =
    eina_stringshare_add(eina_strbuf_string_get(newbuf));
  defaultnote->Note_Sync_Data.online = (int*)0;
  eina_strbuf_free(newbuf);

  note_list = eina_list_append(note_list, defaultnote);

  enotes_win_setup(defaultnote);
}

/*
static const Ecore_Getopt optdesc = {
    "enotes",
    "%prog [options] [command]\n\n",
//     "    inapphelp\t\t  show keybindings\n",
    PACKAGE_VERSION,
    COPYRIGHT,
    "BSD with advertisement clause",
    "An EFL Note program; use F1 for inapp help",
    0,
    {
        ECORE_GETOPT_LICENSE('l', "license"),
        ECORE_GETOPT_COPYRIGHT('c', "copyright"),
        ECORE_GETOPT_VERSION('v', "version"),
        ECORE_GETOPT_HELP('h', "help"),
        ECORE_GETOPT_SENTINEL
    }
};*/

EAPI_MAIN int
elm_main(int argc EINA_UNUSED, char** argv EINA_UNUSED)
{
  //     int args;
  //     Eina_Bool quit_option = EINA_FALSE;
  //
  //     Ecore_Getopt_Value values[] = {
  //         ECORE_GETOPT_VALUE_BOOL(quit_option),
  //         ECORE_GETOPT_VALUE_BOOL(quit_option),
  //         ECORE_GETOPT_VALUE_BOOL(quit_option),
  //         ECORE_GETOPT_VALUE_BOOL(quit_option),
  //         ECORE_GETOPT_VALUE_NONE
  //    };
  //
  elm_app_compile_bin_dir_set(PACKAGE_BIN_DIR);
  elm_app_compile_lib_dir_set(PACKAGE_LIB_DIR);
  elm_app_compile_data_dir_set(PACKAGE_DATA_DIR);

#if ENABLE_NLS
  setlocale(LC_ALL, "");
  bindtextdomain(PACKAGE, LOCALEDIR);
  bind_textdomain_codeset(PACKAGE, "UTF-8");
  textdomain(PACKAGE);
#endif

  //     enotes_init();
  _my_conf_descriptor_init();

  /*
      args = ecore_getopt_parse(&optdesc, values, argc, argv);
      if (args < 0)
      {
          EINA_LOG_CRIT("Could not parse arguments.");
          goto end;
      }
      else if (quit_option)
      {
          goto end;
      }
       */
  // set app informations
  elm_app_name_set("eNotes");
  elm_app_desktop_entry_set("enotes.desktop");
  elm_app_info_set(elm_main, "enotes", "");

  Eina_List* l;
  Note* list_data;

  config_path = efreet_config_home_get();
  snprintf(enotes_running,
           sizeof(enotes_running),
           "%s/enotes/enotes_running.pid",
           config_path);

  if (!ecore_file_exists(enotes_running)) {
    FILE* fp;

    _home_dir();
    _read_notes_eet();

    fp = fopen(enotes_running, "w");
    fprintf(fp,
            "%s",
            "If this file exists, a enotes instance is running.\nIf no other "
            "instance is running, please remove this file, if you have "
            "problems starting enotes");
    fclose(fp);
    
    enotes_systray(NULL, NULL, NULL);
    
    all_hidden = EINA_FALSE;
    
if (eina_list_count(note_list) == 0) {
      _enotes_new();
    } else {
      EINA_LIST_FOREACH(note_list, l, list_data)
      {
        enotes_win_setup(list_data);
      }
    }
  } else {
    printf("Dont't start Enotes twice - closing ...\nIf no other instance is "
           "running, please remove ~/.config/enotes/enotes_running.pid\n");
    goto end_double_start;
  }

  elm_run();

end_double_start:
  elm_exit();

  return 0;
}
ELM_MAIN()
