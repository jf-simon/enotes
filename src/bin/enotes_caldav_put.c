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
	char **arr, **arr1;
	int i = 0, y = 0;
   Eina_List *new_notes = NULL;
	arr = eina_str_split(eina_strbuf_string_get(mybuffer), "<d:response>", 0); // split into single "responses"
	
	if(arr == NULL)
		return; // no response data = no vtodo
   for (i = 1; arr[i]; i++)
	{
      Note *data_add = calloc(1, sizeof(Note));
      data_add->Note_Sync_Data.href = find_data(arr[i], "<d:href>", "</d:href>");           // find <d:href>

      data_add->Note_Sync_Data.etag = find_data(arr[i], "<d:getetag>", "</d:getetag>");  // find <d:getetag>
      stringReplace("&quot;", "", (char *)data_add->Note_Sync_Data.etag);

      arr1 = eina_str_split(find_data(arr[i], "<cal:calendar-data>", "</cal:calendar-data>"), "\n", 0); // split calendar-data into single lines
         for (y = 0; arr1[y]; y++)
         {
            if(strstr(arr1[y], "SUMMARY"))
            {
               stringReplace("SUMMARY:", "", arr1[y]);
//                stringReplace("\\n:", "<br>", arr1[y]);
//                printf("SUMMARY: %s\n", arr1[y]);
               data_add->Note_Sync_Data.summary = arr1[y];
            }
            if(strstr(arr1[y], "DESCRIPTION"))
            {
               stringReplace("DESCRIPTION:", "", arr1[y]);
               stringReplace("\\n:", "<br>", arr1[y]);
//                printf("DESCRIPTION: %s\n", arr1[y]);
               data_add->Note_Sync_Data.description = arr1[y];
            }
            if(strstr(arr1[y], "UID"))
            {
               stringReplace("UID:", "", arr1[y]);
//                printf("UID: %s\n", arr1[y]);
               data_add->Note_Sync_Data.uid = arr1[y];
            }

         }
      
//       data_add->Note_Sync_Data.uid = find_data(arr[i], "<d:status>", "</d:status>");     // <d:status>

      new_notes = eina_list_append(new_notes, data_add);

   }
   note_online_to_local(new_notes);
   
//    eina_list_free(new_notes);
   free(arr[0]);
   free(arr);
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
_put_local_data(void *data EINA_UNUSED, Evas_Object *obj EINA_UNUSED, void *event_info EINA_UNUSED)
{
   note_list_put = NULL;
   Eina_List *l;
   Note *list_data;
   EINA_LIST_FOREACH(note_list, l, list_data)
   {
      if(list_data->Note_Sync_Data.uid == NULL || strcmp(list_data->Note_Sync_Data.uid, "") == 0)
      {
         note_list_put = eina_list_append(note_list_put, list_data);
      }
   }
   if(eina_list_count(note_list_put) > 0)
      _put_local_data_online();
}


void
_put_local_data_online()
{
   printf("note_list_put Count START %i\n", eina_list_count(note_list_put));

//                   char last_modified[512];
//                   char put_data_existing[PATH_MAX*4];
//                   char newbuf[1024];
//                   char logindata[PATH_MAX];char header_url_report[PATH_MAX];
//                   char tmp[1024];
//                   char tmp1[1024];
//                   char curl_url[PATH_MAX];
//                   char buf[PATH_MAX];
                  
                  
                  Eina_Strbuf *last_modified, *put_data_existing, *newbuf, *logindata, *header_url_report, *tmp, *tmp1, *curl_url, *buf;
                  last_modified = eina_strbuf_new();
                  put_data_existing = eina_strbuf_new();
                  newbuf = eina_strbuf_new();
                  logindata = eina_strbuf_new();
                  header_url_report = eina_strbuf_new();
                  tmp = eina_strbuf_new();
                  tmp1 = eina_strbuf_new();
                  curl_url = eina_strbuf_new();
                  buf = eina_strbuf_new();
   Eina_List *l;
   Note *list_data;
   EINA_LIST_FOREACH(note_list_put, l, list_data)
         {
//                printf("upload SUMMARY:%s\n", list_data->note_text);
               time_t t;
               struct tm * ts;
               t = time(NULL);
               ts = localtime(&t);
//                snprintf(last_modified, sizeof(last_modified), "%i%02i%02iT%02i%02i%02iZ", ts->tm_year+1900, ts->tm_mon+1, ts->tm_wday+1, ts->tm_hour, ts->tm_min, ts->tm_sec);
               
               eina_strbuf_append_printf(last_modified, "%i%02i%02iT%02i%02i%02iZ", ts->tm_year+1900, ts->tm_mon+1, ts->tm_wday+1, ts->tm_hour, ts->tm_min, ts->tm_sec);
               printf("LAST-MODIFIED:%s\n",eina_strbuf_string_get(last_modified));
               
               struct MemoryStruct chunk;
               
               chunk.memory = malloc(1);  
               chunk.size = 0;
                  
                  CURL *curl;
                  CURLcode res;

//                stringReplace("<br>", "\\n", list_data->note_text);
//                stringReplace("<item relsize=24x24 vsize=full href=done></item>", "[x]", list_data->note_text);
//                stringReplace("<item relsize=24x24 vsize=full href=open></item>", "[ ]", list_data->note_text);
                  // request PUT for put download objects
//                      snprintf(put_data_existing, sizeof(put_data_existing), 
                     eina_strbuf_append_printf(put_data_existing, "BEGIN:VCALENDAR\n"
                     "VERSION:2.0\n"
                     "PRODID:%s\n"
                     "BEGIN:VTODO\n"
                     "X-OC-HIDESUBTASKS:0\n"
                     "CREATED:%s\n"
                     "LAST-MODIFIED:%s\n"
//                      "DTSTAMP:20190206T200100Z\n"
                     "SUMMARY:%s\n"
               //       "PERCENT-COMPLETE:0\n"
                     "STATUS:%s\n"
                     "DESCRIPTION:￼%s\n"
//                      "START:20190219T153000Z\n"
               //       "DUE:20190220T153000Z\n"
               //       "DTSTAMP:20190205T153000Z\n"
                     "SEQUENCE:1\n"
                     "UID:enote_%s\n"
                     "END:VTODO\n"
                     "END:VCALENDAR\n", list_data->Note_Sync_Data.prodid, list_data->Note_Sync_Data.created, eina_strbuf_string_get(last_modified), list_data->note_name, list_data->Note_Sync_Data.status, list_data->note_text, eina_strbuf_string_get(last_modified));

//                      snprintf(newbuf, sizeof(newbuf), "enote_%s", last_modified);
                     eina_strbuf_append_printf(newbuf, "enote_%s", eina_strbuf_string_get(last_modified));
                     list_data->Note_Sync_Data.uid = eina_stringshare_add(eina_strbuf_string_get(newbuf));
                     
                     Eina_List *l1;
                     Note *list_data2;
                     
                     EINA_LIST_FOREACH(note_list, l1, list_data2)
                     {
                        if(list_data2->id == list_data->id)
                        {
                           list_data2 = list_data;
                           printf("FOUND AND UPDATE\n");
                        }
                     }
                  eina_strbuf_replace_all(put_data_existing, "<item relsize=24x24 vsize=full href=done></item>", "[x]");
                  eina_strbuf_replace_all(put_data_existing, "<item relsize=24x24 vsize=full href=open></item>", "[ ]");
                  eina_strbuf_replace_all(put_data_existing, "<br/>", "\\n");
                  printf("PUTDATA:\n%s\n", eina_strbuf_string_get(put_data_existing));
                  struct curl_slist *put_header_download_objects = NULL;
                  

                  curl = curl_easy_init ();
                  if (!curl) return 2;

//                   snprintf(logindata, sizeof(logindata), "%s:%s", user, password);
                  eina_strbuf_append_printf(logindata, "%s:%s", user, password);
                  curl_easy_setopt (curl, CURLOPT_USERPWD, eina_strbuf_string_get(logindata));
               //     curl_easy_setopt (curl, CURLOPT_VERBOSE, "on"); 

                  
//                      snprintf(tmp, sizeof(tmp), "PUT /calendars/enotes/personal/enotes_%s.ics", last_modified);
                     eina_strbuf_append_printf(tmp, "PUT /calendars/enotes/personal/enotes_%s.ics", eina_strbuf_string_get(last_modified));
//                      snprintf(tmp1, sizeof(tmp1), "https://enotes.ocloud.de/remote.php/dav/calendars/enotes/personal/enotes_%s.ics", last_modified);
                     eina_strbuf_append_printf(tmp1, "https://enotes.ocloud.de/remote.php/dav/calendars/enotes/personal/enotes_%s.ics", eina_strbuf_string_get(last_modified));
                     
                     put_header_download_objects = curl_slist_append (put_header_download_objects, eina_strbuf_string_get(tmp));
                     put_header_download_objects = curl_slist_append (put_header_download_objects, "Content-Type: text/calendar; charset=utf-8");
                     
//                      snprintf(curl_url, sizeof(curl_url), "%s/calendars/%s/personal", server_url, user);
                     eina_strbuf_append_printf(curl_url, "%s/calendars/%s/personal", server_url, user);
                     curl_easy_setopt (curl, CURLOPT_URL, eina_strbuf_string_get(tmp1));
                     curl_easy_setopt (curl, CURLOPT_HTTPHEADER, put_header_download_objects);
                     curl_easy_setopt (curl, CURLOPT_CUSTOMREQUEST, "PUT");
                     curl_easy_setopt (curl, CURLOPT_POSTFIELDS, eina_strbuf_string_get(put_data_existing));

                  
                  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
                  
                  
                  res = curl_easy_perform (curl);
                  if (res != CURLE_OK)
                     fprintf (stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror (res));
                  else {
                  
                  char *test;
                  test = chunk.memory;
/*
                  
                  Eina_Strbuf *tmp;
                  tmp = eina_strbuf_new();
                  eina_strbuf_append(tmp, test);
               //    parse_curl_put_response(tmp);
                  eina_strbuf_free(tmp);
                  
                  */
                     printf("\n\n\nbytes retrieved %s \n", (char*)test);
               //     printf("%lu bytes retrieved\n", (unsigned long)chunk.size);

                  curl_slist_free_all(put_header_download_objects);
                  curl_easy_cleanup(curl);

//                   return 0;
                  }
                  eina_strbuf_reset(last_modified);
                  eina_strbuf_reset(newbuf);
                  eina_strbuf_reset(logindata);
                  eina_strbuf_reset(header_url_report);
                  eina_strbuf_reset(tmp);
                  eina_strbuf_reset(tmp1);
                  eina_strbuf_reset(curl_url);
                  eina_strbuf_reset(buf);
                  eina_strbuf_reset(put_data_existing);
   }
   eina_list_free(note_list_put);
   printf("note_list_put Count END %i\n", eina_list_count(note_list_put));
}
