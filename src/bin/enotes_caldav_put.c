#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT
#endif

#include <curl/curl.h>

#include "enotes.h"

struct MemoryStruct
{
  char* memory;
  size_t size;
};

static void
parse_curl_put_response(Eina_Strbuf* mybuffer)
{
  // 	char **arr, **arr1;
  // 	int i = 0, y = 0;
  //    Eina_List *new_notes = NULL;
  // 	arr = eina_str_split(eina_strbuf_string_get(mybuffer), "<d:response>",
  // 0); // split into single "responses"
  //
  // 	if(arr == NULL)
  // 		return; // no response data = no vtodo
  //    for (i = 1; arr[i]; i++)
  // 	{
  //    }
  //
  //    free(arr[0]);
  //    free(arr);

  printf("\nRESPONSE %s\n", eina_strbuf_string_get(mybuffer));
  eina_strbuf_reset(mybuffer);
}

static size_t
WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct* mem = (struct MemoryStruct*)userp;

  char* ptr = realloc(mem->memory, mem->size + realsize + 1);
  if (ptr == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

void
_put_local_data(void* data EINA_UNUSED,
                Evas_Object* obj EINA_UNUSED,
                void* event_info EINA_UNUSED)
{
  //    note_list_put = NULL;
  Eina_List* l;
  Note* list_data = NULL;
  EINA_LIST_FOREACH(note_list, l, list_data)
  {
    if (list_data->Note_Sync_Data.online == 0) {
      note_list_put = eina_list_append(note_list_put, list_data);
      printf("PUT LOCAL DATA FUNKTION\n");
    }
  }
  //    // TODO: Vergleich von online und local, dann upload

  // UPLOAD GO //
  if (eina_list_count(note_list_put) > 0)
    _put_local_data_online();
}

void
_put_local_data_online()
{

  Eina_Strbuf *last_modified, *put_data_existing, *newbuf, *logindata,
    *header_url_report, *tmp, *tmp1, *curl_url, *buf, *tmpnew;
  last_modified = eina_strbuf_new();
  put_data_existing = eina_strbuf_new();
  newbuf = eina_strbuf_new();
  logindata = eina_strbuf_new();
  header_url_report = eina_strbuf_new();
  tmp = eina_strbuf_new();
  tmp1 = eina_strbuf_new();
  curl_url = eina_strbuf_new();
  buf = eina_strbuf_new();
  tmpnew = eina_strbuf_new();
  Eina_List* l;
  Note* list_data = NULL;

  struct MemoryStruct chunk;

  chunk.memory = malloc(1);
  chunk.size = 0;
  printf("note_list_put: %i\n", eina_list_count(note_list_put));
  EINA_LIST_FOREACH(note_list_put, l, list_data) // TODO: checken ob href vorhanden, wenn nicht, neue erstellen
                                                   // ansonsten wird es ein update mit gleicher hred
  {
    CURL* curl;
    CURLcode res;
    // request PUT for put download objects
    eina_strbuf_append_printf(
      put_data_existing,
      "BEGIN:VCALENDAR\n"
      "VERSION:2.0\n"
      "PRODID:%s\n"
      "BEGIN:VTODO\n"
      "X-OC-HIDESUBTASKS:0\n"
      "CREATED:%s\n"
      "LAST-MODIFIED:%s\n"
      //                      "DTSTAMP:20190206T200100Z\n"
      "SUMMARY:%s\n"
      "PERCENT-COMPLETE:0\n"
      "STATUS:%s\n"
      "DESCRIPTION:%s\n"
//       "START:20190219T153000Z\n"
//       "DUE:20190220T153000Z\n"
      "DTSTAMP:%s\n"
      "SEQUENCE:1\n"
      "UID:%s\n"
      "CATEGORIES:%s\n"
      "END:VTODO\n"
      "END:VCALENDAR\n",
      list_data->Note_Sync_Data.prodid,
      list_data->Note_Sync_Data.created,
      list_data->Note_Sync_Data.last_modified,
      list_data->note_name,
      list_data->Note_Sync_Data.status,
      list_data->note_text,
      list_data->Note_Sync_Data.last_modified,
      list_data->Note_Sync_Data.uid,
      list_data->Note_Sync_Data.categories);


    eina_strbuf_replace_all(put_data_existing,
                            "<item relsize=24x24 vsize=full href=done></item>",
                            "[x]");
    eina_strbuf_replace_all(put_data_existing,
                            "<item relsize=24x24 vsize=full href=open></item>",
                            "[ ]");
    eina_strbuf_replace_all(put_data_existing, "<br/>", "\\n");
    
    struct curl_slist* put_header_download_objects = NULL;

    curl = curl_easy_init();
    //                   if (!curl) return 2;// TODO: ABFRAGE MACHEN

    eina_strbuf_append_printf(logindata, "%s:%s", user_name, password);
    curl_easy_setopt(curl, CURLOPT_USERPWD, eina_strbuf_string_get(logindata));
    //     curl_easy_setopt (curl, CURLOPT_VERBOSE, "on");

    if (strcmp(list_data->Note_Sync_Data.href, "")) // ceate new note online
    {
      eina_strbuf_append_printf(
        tmp1, "https://enotes.ocloud.de%s", list_data->Note_Sync_Data.href);
      eina_strbuf_append_printf(
        tmp, "PUT %s HTTP/1.1", list_data->Note_Sync_Data.href);

    } else {
      eina_strbuf_append_printf(tmp1,
                                "%s/calendars/%s/%s/enotes_%s.ics",
                                server_url,
                                user_name,
                                calendar_name,
                                list_data->Note_Sync_Data.last_modified);
      eina_strbuf_append_printf(
        tmp,
        "PUT /calendars/%s/%s/enotes_%s HTTP/1.1",
        user_name,
        calendar_name,
        list_data->Note_Sync_Data.last_modified);

      eina_strbuf_append_buffer(tmpnew, tmp);
      eina_strbuf_replace_all(tmpnew, "PUT ", "");
      eina_strbuf_replace_all(tmpnew, " HTTP/1.1", "");
      eina_strbuf_prepend(tmpnew, "/remote.php/dav");
      eina_strbuf_append(tmpnew, ".ics");
      list_data->Note_Sync_Data.href =
        eina_stringshare_add(eina_strbuf_string_get(tmpnew));
        
      printf("\n\n\nhref UPDATE\n");
      
      printf("\n\n\nurl: %s\n", eina_strbuf_string_get(tmp1));
      
      printf("\n\n\nurl: %s\n", eina_strbuf_string_get(tmp));
    }
    //                    printf("upload header:%s\n",
    //                    eina_strbuf_string_get(tmp)); printf("upload
    //                    header1:%s\n", eina_strbuf_string_get(tmp1));

    put_header_download_objects = curl_slist_append(
      put_header_download_objects, eina_strbuf_string_get(tmp));
    put_header_download_objects =
      curl_slist_append(put_header_download_objects,
                        "Content-Type: text/calendar; charset=utf-8");

    curl_easy_setopt(curl, CURLOPT_URL, eina_strbuf_string_get(tmp1));
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, put_header_download_objects);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
    curl_easy_setopt(
      curl, CURLOPT_POSTFIELDS, eina_strbuf_string_get(put_data_existing));

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

    res = curl_easy_perform(curl);

    Eina_Strbuf* response_data;
    response_data = eina_strbuf_new();

    if (res != CURLE_OK) {
      //                      fprintf (stderr, "curl_easy_perform() failed:
      //                      %s\n", curl_easy_strerror (res));
      eina_strbuf_append_printf(response_data, curl_easy_strerror(res));
    } else {

      //                   char *test;
      //                   test = chunk.memory;
      eina_strbuf_append(response_data, (const char*)chunk.memory);
    }

    //                   if(list_data->Note_Sync_Data.href == NULL ||
    //                   !strcmp(list_data->Note_Sync_Data.href, "")) // ceate
    //                   new note online
    //                   {
    //                      eina_strbuf_replace_all(tmp, "PUT ", "");
    //                      eina_strbuf_replace_all(tmp, " HTTP/1.1", "");
    //                      eina_strbuf_prepend(tmp, "/remote.php/dav");
    //                      eina_strbuf_append(tmp, ".ics");
    //                      list_data->Note_Sync_Data.href =
    //                      eina_stringshare_add(eina_strbuf_string_get(tmp));
    //                      printf("href UPDATE\n");
    //                   }

    list_data->Note_Sync_Data.online = (int*)1;

    Eina_List* l1;
    Note* list_data2;

    EINA_LIST_FOREACH(note_list, l1, list_data2)
    {
      if (list_data2->id == list_data->id) {
        list_data2 = list_data;
        printf("\nFOUND AND UPDATE\n");
      }
    }

    parse_curl_put_response(response_data);

    eina_strbuf_free(response_data);

    curl_slist_free_all(put_header_download_objects);
    curl_easy_cleanup(curl);

    //                   return 0;

    eina_strbuf_reset(last_modified);
    eina_strbuf_reset(newbuf);
    eina_strbuf_reset(logindata);
    eina_strbuf_reset(header_url_report);
    eina_strbuf_reset(tmp);
    eina_strbuf_reset(tmp1);
    eina_strbuf_reset(curl_url);
    eina_strbuf_reset(buf);
    eina_strbuf_reset(put_data_existing);
    eina_strbuf_reset(tmpnew);
  }
  eina_list_free(note_list_put);
  //    printf("note_list_put Count END %i\n", eina_list_count(note_list_put));
}
