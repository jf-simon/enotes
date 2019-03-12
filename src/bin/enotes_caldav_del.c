#ifdef HAVE_CONFIG_H
# include "config.h"
#else
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT
#endif

#include <curl/curl.h>

#include "enotes.h"


struct MemoryStruct {
  char *memory;
  size_t size;
};



static void
parse_curl_put_response(Eina_Strbuf *mybuffer)
{	
// 	char **arr, **arr1;
// 	int i = 0, y = 0;
//    Eina_List *new_notes = NULL;
// 	arr = eina_str_split(eina_strbuf_string_get(mybuffer), "<d:response>", 0); // split into single "responses"
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
                     printf("\nnRESPONSE");
printf("\nRESPONSE %s\n", eina_strbuf_string_get(mybuffer));
	eina_strbuf_reset(mybuffer);
}



static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
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
_del_local_data(void *data, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   
    Eina_List *list_values = data;
//     Evas_Object *win = eina_list_nth(list_values, 2);
    int *del_id = eina_list_nth(list_values, 3);
   
   note_list_del = NULL;
   Eina_List *l;
   Note *list_data;
   EINA_LIST_FOREACH(note_list, l, list_data)
   {
      if(list_data->id == *del_id)
      {
         note_list_del = eina_list_append(note_list_del, list_data);
      }
   }
   printf("delete %i\n", eina_list_count(note_list_del));
   
   if(eina_list_count(note_list_del) > 0)
      _del_local_data_online(data);
}


void
_del_local_data_online(void *data)
{
//    printf("note_list_put Count START %i\n", eina_list_count(note_list_put));
   Eina_Strbuf *tmp, *tmp1, *buf, *curl_url, *logindata, *put_data_existing;
   tmp = eina_strbuf_new();
   tmp1 = eina_strbuf_new();
   put_data_existing = eina_strbuf_new();
   buf = eina_strbuf_new();
   curl_url = eina_strbuf_new();
   logindata = eina_strbuf_new();
   Eina_List *l;
   Note *list_data;
   
   struct MemoryStruct chunk;

   chunk.memory = malloc(1);
   chunk.size = 0;
   
   EINA_LIST_FOREACH(note_list_del, l, list_data)
         {
//                printf("upload SUMMARY:%s\n", list_data->note_text);
               time_t t;
               struct tm * ts;
               t = time(NULL);
               ts = localtime(&t);

                  
                  CURL *curl;
                  CURLcode res;

                  struct curl_slist *put_header_download_objects = NULL;
                  
                   eina_strbuf_append_printf(put_data_existing, "https://enotes.ocloud.de%s", list_data->Note_Sync_Data.href);
                  curl = curl_easy_init ();
                  if (!curl) return 2;

                  eina_strbuf_append_printf(logindata, "%s:%s", user, password);
                  curl_easy_setopt (curl, CURLOPT_USERPWD, eina_strbuf_string_get(logindata));
//                    curl_easy_setopt (curl, CURLOPT_VERBOSE, "on"); 

                     eina_strbuf_append_printf(tmp1, "https://enotes.ocloud.de%s", list_data->Note_Sync_Data.href);

                     put_header_download_objects = curl_slist_append (put_header_download_objects, "Content-Type: text/calendar; charset=utf-8");
                     put_header_download_objects = curl_slist_append (put_header_download_objects, eina_strbuf_string_get(tmp1));

                     printf("tmp1 DATE:\n%s\n", eina_strbuf_string_get(tmp1));

                     curl_easy_setopt (curl, CURLOPT_URL, eina_strbuf_string_get(tmp1));
//                      curl_easy_setopt (curl, CURLOPT_URL, eina_strbuf_string_get(curl_url));
                     curl_easy_setopt (curl, CURLOPT_HTTPHEADER, put_header_download_objects);
                     curl_easy_setopt (curl, CURLOPT_CUSTOMREQUEST, "DELETE");
//                      curl_easy_setopt (curl, CURLOPT_POSTFIELDS, eina_strbuf_string_get(put_data_existing));

                  
                  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
                  
                  
                  res = curl_easy_perform (curl);
                  
                  
                  Eina_Strbuf *response_data;
                  response_data = eina_strbuf_new();
                  
                  
                  if (res != CURLE_OK)
//                      fprintf (stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror (res));
                        eina_strbuf_append_printf(response_data, curl_easy_strerror (res));
                  else {
                  
                  char *test;
                  test = chunk.memory;
                  eina_strbuf_append(response_data, test);
                  _enotes_del_local(data, NULL, NULL);
                  }

                  
                  
                  
                  parse_curl_put_response(response_data);
                  eina_strbuf_free(response_data);


                  curl_slist_free_all(put_header_download_objects);
                  curl_easy_cleanup(curl);

//                    printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
//                    printf("%s bytes retrieved\n", test);
   }
//                   return 0;
                  
                  
                  eina_strbuf_reset(buf);
                  eina_strbuf_reset(tmp);
                  eina_strbuf_reset(tmp1);
                  eina_strbuf_reset(put_data_existing);
                  
   eina_list_free(note_list_del);
//    printf("note_list_delCount END %i\n", eina_list_count(note_list_del));
}
