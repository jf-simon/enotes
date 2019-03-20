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
parse_curl_get_response(Eina_Strbuf* mybuffer)
{
  char **arr, **arr1;
  int i = 0, y = 0;
  Eina_List* new_notes = NULL;
  arr = eina_str_split(eina_strbuf_string_get(mybuffer),
                       "<d:response>",
                       0); // split into single "responses"

  Eina_Strbuf *arry_split, *summary_tmp;

  arry_split = eina_strbuf_new();
  summary_tmp = eina_strbuf_new();
  if (arr == NULL)
    return; // no response data = no vtodo
  for (i = 1; arr[i]; i++) {
    Note* data_add = calloc(1, sizeof(Note));
    data_add->Note_Sync_Data.href =
      find_data(arr[i], "<d:href>", "</d:href>"); // find <d:href>
    //                printf("HREF: %s\n",  data_add->Note_Sync_Data.href);

    data_add->Note_Sync_Data.etag =
      find_data(arr[i], "<d:getetag>", "</d:getetag>"); // find <d:getetag>
    stringReplace("&quot;", "", (char*)data_add->Note_Sync_Data.etag);

    arr1 = eina_str_split(
      find_data(arr[i], "<cal:calendar-data>", "</cal:calendar-data>"),
      "\n",
      0); // split calendar-data into single lines
    for (y = 0; arr1[y]; y++) {
      eina_strbuf_append(arry_split, arr1[y]);

      if (strstr(arr1[y], "SUMMARY")) {
        eina_strbuf_replace_all(arry_split, "SUMMARY:", "");
        eina_strbuf_replace_all(arry_split, "<br>", " ");
        data_add->Note_Sync_Data.summary =
          eina_stringshare_add(eina_strbuf_string_get(arry_split));

      } else if (strstr(arr1[y], "DESCRIPTION")) {
        eina_strbuf_append_buffer(summary_tmp, arry_split);

      } else if (strstr(arr1[y], "UID")) {
        eina_strbuf_replace_all(arry_split, "UID:", "");
        data_add->Note_Sync_Data.uid =
          eina_stringshare_add(eina_strbuf_string_get(arry_split));

      } else if (strstr(arr1[y], "PRODID")) {
        eina_strbuf_replace_all(arry_split, "PRODID:", "");
        data_add->Note_Sync_Data.prodid =
          eina_stringshare_add(eina_strbuf_string_get(arry_split));

      } else if (strstr(arr1[y], "CREATED")) {
        eina_strbuf_replace_all(arry_split, "CREATED:", "");
        data_add->Note_Sync_Data.created =
          eina_stringshare_add(eina_strbuf_string_get(arry_split));

      } else if (strstr(arr1[y], "LAST-MODIFIED")) {
        eina_strbuf_replace_all(arry_split, "LAST-MODIFIED:", "");
        data_add->Note_Sync_Data.last_modified =
          eina_stringshare_add(eina_strbuf_string_get(arry_split));
        data_add->Note_Sync_Data.last_modified_server =
          eina_stringshare_add(eina_strbuf_string_get(arry_split));

      } else if (strstr(arr1[y], "PERCENT-COMPLETE")) {
        eina_strbuf_replace_all(arry_split, "PERCENT-COMPLETE:", "");
        data_add->Note_Sync_Data.percent_complete =
          eina_stringshare_add(eina_strbuf_string_get(arry_split));

      } else if (strstr(arr1[y], " ")) {
        eina_strbuf_ltrim(arry_split);
        eina_strbuf_append_buffer(summary_tmp, arry_split);
      }

      eina_strbuf_reset(arry_split);
    }

    eina_strbuf_replace_all(summary_tmp, "DESCRIPTION:", "");
    eina_strbuf_replace_all(summary_tmp, "\\n", "<br/>");
    eina_strbuf_replace_all(
      summary_tmp, "[x]", "<item relsize=24x24 vsize=full href=done></item>");
    eina_strbuf_replace_all(
      summary_tmp, "[ ]", "<item relsize=24x24 vsize=full href=open></item>");

    data_add->Note_Sync_Data.description =
      eina_stringshare_add(eina_strbuf_string_get(summary_tmp));
    eina_strbuf_reset(summary_tmp);

    data_add->Note_Sync_Data.online = (int*)1;

    new_notes = eina_list_append(new_notes, data_add);
  }
  if (eina_list_count(new_notes) == 0)
    _put_local_data(NULL, NULL, NULL);
  else
    note_online_to_local(new_notes);

  //    eina_list_free(new_notes);
  free(arr[0]);
  free(arr);
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
_get_online_data(void* data EINA_UNUSED,
                 Evas_Object* obj EINA_UNUSED,
                 void* event_info EINA_UNUSED)
{
  char last_modified[512];
  time_t t;
  struct tm* ts;
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

  struct MemoryStruct chunk;

  chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
  chunk.size = 0;           /* no data at this point */

  CURL* curl;
  CURLcode res;

  // request data for receiving calendar information
  const char* request_data_download_objects =
    "<c:calendar-query xmlns:d=\"DAV:\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\">"
    "<d:prop>"
    "<d:getetag />"
    "<c:calendar-data />"
    "</d:prop>"
    "<c:filter>"
    "<c:comp-filter name=\"VCALENDAR\">"
    "<c:comp-filter name=\"VTODO\" />"
    "</c:comp-filter>"
    "</c:filter>"
    "</c:calendar-query>";

  struct curl_slist* put_header_download_objects = NULL;

  curl = curl_easy_init();
  // 	if (!curl) return 2; //TODO: ABFRAGE MACHEN

  char logindata[PATH_MAX];
  snprintf(logindata, sizeof(logindata), "%s:%s", user_name, password);
  curl_easy_setopt(curl, CURLOPT_USERPWD, logindata);
  //     curl_easy_setopt (curl, CURLOPT_VERBOSE, "on");

  char header_url_report[PATH_MAX];
  snprintf(header_url_report,
           sizeof(header_url_report),
           "REPORT /calendars/%s/%s/ HTTP/1.1",
           user_name,
           calendar_name
          );

  put_header_download_objects =
    curl_slist_append(put_header_download_objects, header_url_report);
  put_header_download_objects =
    curl_slist_append(put_header_download_objects, "Depth: 1");
  put_header_download_objects =
    curl_slist_append(put_header_download_objects, "Prefer: return-minimal");
  put_header_download_objects =
    curl_slist_append(put_header_download_objects,
                      "Content-Type: application/xml; charset=utf-8");

  char curl_url[PATH_MAX];
  snprintf(
    curl_url, sizeof(curl_url), "%s/calendars/%s/%s", server_url, user_name, calendar_name);
  printf("CURL_URL: %s\n\n", curl_url);
  curl_easy_setopt(curl, CURLOPT_URL, curl_url);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, put_header_download_objects);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "REPORT");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data_download_objects);

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

  res = curl_easy_perform(curl);
  if (res != CURLE_OK)
    fprintf(
      stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
  else {
    /*
     * Now, our chunk.memory points to a memory block that is chunk.size
     * bytes big and contains the remote file.
     *
     * Do something nice with it!
     */

    Eina_Strbuf* response_data;
    response_data = eina_strbuf_new();
    eina_strbuf_append(response_data, (const char*)chunk.memory);
    parse_curl_get_response(response_data);
    eina_strbuf_free(response_data);

    //       printf("\n\n\nbytes retrieved %s \n", test);
    //     printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  }
  curl_slist_free_all(put_header_download_objects);
  curl_easy_cleanup(curl);
  // 	return 0;
}
