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
_save_and_check_user_data(void* data,
                          Evas_Object* obj EINA_UNUSED,
                          void* event_info EINA_UNUSED)
{
  Evas_Object* en_server = evas_object_data_get(data, "en_server");
  Evas_Object* en_password = evas_object_data_get(data, "en_password");
  Evas_Object* en_user = evas_object_data_get(data, "en_user");
  Evas_Object* bn = evas_object_data_get(data, "bn");
  Evas_Object* ic = evas_object_data_get(data, "ic");
  Evas_Object* en_calendar = evas_object_data_get(data, "en_calendar");

  server_url = elm_object_text_get(en_server);
  calendar_name = elm_object_text_get(en_calendar);
  password = elm_object_text_get(en_password);
  user_name = elm_object_text_get(en_user);

  
  struct MemoryStruct chunk;

  chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
  chunk.size = 0;           /* no data at this point */

  CURL* curl;
  CURLcode res;

  // request data for receiving calendar information
  const char* propfind_data =
    "<d:propfind xmlns:d=\"DAV:\" xmlns:cs=\"http://calendarserver.org/ns/\">"
    "<d:prop>"
    "<d:displayname />"
    "<cs:getctag />"
    "</d:prop>"
    "</d:propfind>";

  struct curl_slist* request_header_download_objects = NULL;

  curl = curl_easy_init();
  // 	if (!curl) return 2;// TODO: ABFRAGE CHECKEN

  char logindata[PATH_MAX];
  snprintf(logindata, sizeof(logindata), "%s:%s", user_name, password);
  curl_easy_setopt(curl, CURLOPT_USERPWD, logindata);
  //     curl_easy_setopt (curl, CURLOPT_VERBOSE, "on");

  char header_url_propfind[PATH_MAX];
  snprintf(header_url_propfind,
           sizeof(header_url_propfind),
           "PROPFIND /calendars/%s/%s/ HTTP/1.1",
           user_name,
           calendar_name);

  request_header_download_objects =
    curl_slist_append(request_header_download_objects, header_url_propfind);
  request_header_download_objects =
    curl_slist_append(request_header_download_objects, "Depth: 0");
  request_header_download_objects = curl_slist_append(
    request_header_download_objects, "Prefer: return-minimal");
  request_header_download_objects =
    curl_slist_append(request_header_download_objects,
                      "Content-Type: application/xml; charset=utf-8");

  char curl_url[PATH_MAX];
  snprintf(
    curl_url, sizeof(curl_url), "%s/calendars/%s/%s", server_url, user_name, calendar_name);
  curl_easy_setopt(curl, CURLOPT_URL, curl_url);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, request_header_download_objects);
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PROPFIND");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, propfind_data);

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

    char* test;
    test = chunk.memory;
    char buf[PATH_MAX];

    if (strstr(test, "200 OK")) {
      printf("login data valid\n");
      elm_object_text_set(bn, "login data valid - click for recheck");
      snprintf(
        buf, sizeof(buf), "%s/images/online_valid.png", PACKAGE_DATA_DIR);
      elm_image_file_set(ic, buf, NULL);
      online_check = EINA_TRUE;
    } else {
      printf("login data wrong\n");
      elm_object_text_set(bn, "login data wrong - click for recheck");
      snprintf(
        buf, sizeof(buf), "%s/images/online_wrong.png", PACKAGE_DATA_DIR);
      elm_image_file_set(ic, buf, NULL);
      online_check = EINA_FALSE;
    }

    Eina_Strbuf* tmp;
    tmp = eina_strbuf_new();
    eina_strbuf_append(tmp, test);
    //    parse_curl_response(tmp);
    eina_strbuf_free(tmp);

    //       printf("\n\n\nbytes retrieved %s \n", test);
    //     printf("%lu bytes retrieved\n", (unsigned long)chunk.size);
  }
  curl_slist_free_all(request_header_download_objects);
  curl_easy_cleanup(curl);
  // 	return 0;// TODO: ABFRAGE CHECKEN
}
