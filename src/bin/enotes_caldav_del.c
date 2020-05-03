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

  /*                  if(!strstr(test, ""))
                    {
                       printf("\nerfolg");
  //                      _enotes_del_local(data, NULL, NULL);
                    }else
                    {
                       printf("\nkein erfolg");
                    }
                    */
  //    free(arr[0]);
  //    free(arr);
  printf("\nnRESPONSE\n");
  printf(" %s\n", eina_strbuf_string_get(mybuffer));
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
_del_local_data(int del_id)
{
   
//TODO: check for online and offline notes using Note_Sync_Data.online

//    printf("delete ID II %i\n", del_id);
   Eina_List* l;
   Note* list_data;
   EINA_LIST_FOREACH(note_list, l, list_data)
   {
      if (list_data->id == del_id) {
         if(list_data->Note_Sync_Data.online != 0)
            note_list_del = eina_list_append(note_list_del, list_data);
         else
            _enotes_del_local(del_id);
      }
   }
   printf("test delete %i\n", eina_list_count(note_list_del));

   if (eina_list_count(note_list_del) > 0)
      _del_local_data_online();

      update_visible_notes();
//    catlist_to_catlisteet();
//    fill_list_in_settings(); // Update cat list in settings window
}

void
_del_local_data_online()
{
  Eina_Strbuf *header, *buf, *logindata;
  header = eina_strbuf_new();
  buf = eina_strbuf_new();
  logindata = eina_strbuf_new();
  Eina_List* l, *l_next;
  Note* list_data;

  struct MemoryStruct chunk;

  chunk.memory = malloc(1);
  chunk.size = 0;

  EINA_LIST_FOREACH_SAFE(note_list_del, l, l_next, list_data)
  {
    CURL* curl;
    CURLcode res;

    struct curl_slist* put_header_download_objects = NULL;

    curl = curl_easy_init();
    //                   if (!curl) return 2; //TODO: ABFRAGE CHECKEN

    eina_strbuf_append_printf(logindata, "%s:%s", user_name, password);
    curl_easy_setopt(curl, CURLOPT_USERPWD, eina_strbuf_string_get(logindata));

    eina_strbuf_append_printf(
      header, "https://enotes.ocloud.de%s", list_data->Note_Sync_Data.href);

    put_header_download_objects =
      curl_slist_append(put_header_download_objects,
                        "Content-Type: text/calendar; charset=utf-8");
    put_header_download_objects = curl_slist_append(
      put_header_download_objects, eina_strbuf_string_get(header));
//     printf("header DATE:\n%s\n", eina_strbuf_string_get(header));

    curl_easy_setopt(curl, CURLOPT_URL, eina_strbuf_string_get(header));
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, put_header_download_objects);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

    res = curl_easy_perform(curl);

    Eina_Strbuf* response_data;
    response_data = eina_strbuf_new();

    if (res == CURLE_OK) //online deletion OK
    {      eina_strbuf_append(response_data, chunk.memory);
      
//       printf("delete ID III %i\n", list_data->id);
      // delete local note
      if(list_data->id != -1)
         _enotes_del_local(list_data->id); 
            
      // REMOVE LISTE ENTRY FROM note_list_del
      free(list_data);
      note_list_del = eina_list_remove_list(note_list_del, l);

      
      printf("ALL OK: Deleted LOCAL & ONLINE\n");
    }
    else { // delete local, but save data for next attempetion for online deletion

      eina_strbuf_append_printf(response_data, curl_easy_strerror(res));
      
//       printf("delete ID III %i\n", list_data->id);
      _enotes_del_local(list_data->id);
      list_data->id = -1;
      printf("LOCAL: Deleted ONLY LOCAL");
    }

    parse_curl_put_response(response_data);
    eina_strbuf_free(response_data);

    curl_slist_free_all(put_header_download_objects);
    curl_easy_cleanup(curl);
    
    eina_strbuf_reset(buf);
    eina_strbuf_reset(header);
    eina_strbuf_reset(logindata);
    
  }
  //                   return 0;// TODO: WAS DAMIT MACHEN

  eina_strbuf_reset(buf);
  eina_strbuf_reset(header);
  eina_strbuf_reset(logindata);
  
//   if (eina_list_count(note_list_del) == 0)
//       eina_list_free(note_list_del); // TODO CHECK IF THIS WORKS
}
