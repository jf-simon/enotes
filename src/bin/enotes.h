#ifndef ENOTES_H
#define ENOTES_H

#endif

#include "config.h"
#include <Ecore_Getopt.h>
#include <Elementary.h>
#include <Elementary_Cursor.h>

#include "gettext.h"
#include <Ecore_X.h>

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
  const char* cat;

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

} Note;

Eina_List* note_list;
Eina_List* enotes_all_objects_list;
Eina_List* note_list_put;
Eina_List* note_list_del;

const char* server_url;
const char* user;
const char* password;
Eina_Bool ci_m_check;
const char* cat_settings;
Eina_List* cat_list;
char enotes_running[PATH_MAX];

void
_put_local_data(void* data EINA_UNUSED,
                Evas_Object* obj EINA_UNUSED,
                void* event_info EINA_UNUSED);
void
_get_online_data(void* data EINA_UNUSED,
                 Evas_Object* obj EINA_UNUSED,
                 void* event_info EINA_UNUSED);
void
note_online_to_local(Eina_List* new_notes);
void
_open_settings(void* data EINA_UNUSED,
               Evas_Object* obj EINA_UNUSED,
               const char* em EINA_UNUSED,
               const char* src EINA_UNUSED);
void
_save_and_check_user_data(void* data,
                          Evas_Object* obj EINA_UNUSED,
                          void* event_info EINA_UNUSED);
void
fill_cat_list();
char*
stringReplace(char* search, char* replace, char* string);
char*
find_data(char* string, char* start1, char* end1);
void
_enotes_del_local(void* data,
                  Evas_Object* obj EINA_UNUSED,
                  void* event_info EINA_UNUSED);
void
_put_local_data_online();
void
_del_local_data(void* data,
                Evas_Object* obj EINA_UNUSED,
                void* event_info EINA_UNUSED);
void
_del_local_data_online(void* data);
