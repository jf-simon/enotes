
/*
struct _request
{
   long size;
};
static Eina_Bool
_url_progress_cb(void *data EINA_UNUSED, int type EINA_UNUSED, void *event_info)
{
   Ecore_Con_Event_Url_Progress *url_progress = event_info;
   float percent;
   if (url_progress->down.total > 0)
     {
        struct _request *req = ecore_con_url_data_get(url_progress->url_con);
        req->size = url_progress->down.now;
        percent = (url_progress->down.now / url_progress->down.total) * 100;
        printf("Total of download complete: %0.1f (%0.0f)%%\n",
               percent, url_progress->down.now);
     }
   printf("status: %d\n", ecore_con_url_status_code_get(url_progress->url_con));
	
	printf("Total of download complet");
   return EINA_TRUE;
}*/


static Eina_Bool
_url_complete_cb(void *data EINA_UNUSED, int type EINA_UNUSED, void *event_info)
{

            
   Ecore_Con_Event_Url_Complete *url_complete = event_info;
   const Eina_List *headers, *l;
   char *str;
   printf("\n");
   printf("download completed with status code: %d\n", url_complete->status);
   headers = ecore_con_url_response_headers_get(url_complete->url_con);
   EINA_LIST_FOREACH(headers, l, str)
     printf("header: %s\n", str);
   
//    char *buf = ecore_con_url_data_get(ec_url);
//     printf("TEST: %c\n", buf);
//    ecore_main_loop_quit();
   return EINA_TRUE;
	
}

static Eina_Bool
_url_data_cb(void *data, int type EINA_UNUSED, void *event_info)
{
	
	    Eina_List *list_values = data;
    Evas_Object *en = eina_list_nth(list_values, 0);
    Evas_Object *en1 = eina_list_nth(list_values, 4);
	 
   Ecore_Con_Event_Url_Data *url_data = event_info;
	
    char buf[url_data->size];
    char buf1[PATH_MAX];
//     char buf2[PATH_MAX];
//     char *buf2;
   int i;
   for (i = 0; i < url_data->size; i++)
	{
		snprintf(buf1, sizeof(buf1), "%c", url_data->data[i]);
		strcat(buf, buf1);
	}


char delimiter[] = "/\n";
char *ptr;
char needle[] = "DESCRIPTION:";
char needle1[] = "SUMMARY:";

ptr = strtok(buf, delimiter);

while(ptr != NULL) {
	
	if(strstr(ptr, needle)) 
		{
// 			ptr1 = strtok(ptr, "DESCRIPTION:");
			elm_object_text_set(en, ptr);
			printf(" DES Abschnitt gefunden: %s\n", ptr);
			
		}
	if(strstr(ptr, needle1)) 
		{
// 			ptr1 = strtok(ptr, "DESCRIPTION:");
			elm_object_text_set(en1, ptr);
			printf(" SUM Abschnitt gefunden: %s\n", ptr);
			
		}
		
		printf(" Abschnitt gefunden: %s\n", ptr);
 	ptr = strtok(NULL, delimiter);
}
// free(ptr);
// 	printf("%s\n", ptr);

// char url[] = "http://jfsimon:Duff14beer@jfsimon.ocloud.de/remote.php/dav/jfsimon";

// printf("SERVER OPTION %i\n", caldav_enabled_resource(url, NULL));
// char *buf2 = caldav_get_server_options(url, NULL);
	


// printf("SERVER OPTION %c\n", *buf2);
   return EINA_TRUE;
}

static void
_caldav_sync(void *data EINA_UNUSED)
{
// 				   const char *type, *post_data = "";
					Ecore_Con_Url *ec_url = NULL;
// 					Eina_Bool r;
					
   ecore_con_init();
   ecore_con_url_init();
	
   ec_url = ecore_con_url_custom_new("http://jfsimon.ocloud.de/remote.php/dav/calendars/jfsimon/caldavtest/20c16a97-0dba-4c05-bcab-fc36a6c2ef21.1507125291887.ics", "GET");
	
//    ecore_event_handler_add(ECORE_CON_EVENT_URL_COMPLETE, _url_complete_cb, NULL);   
		ecore_event_handler_add(ECORE_CON_EVENT_URL_DATA, _url_data_cb, list_values);
// 	   ecore_event_handler_add(ECORE_CON_EVENT_URL_PROGRESS, _url_progress_cb, NULL);

	
   ecore_con_url_httpauth_set(ec_url, "jfsimon", "Duff14beer", EINA_FALSE);
	
     ecore_con_url_get(ec_url);
   
//   ecore_con_url_post(ec_url, put, strlen(put), "text/xml");
	  
	  
//    ecore_con_url_free(ec_url);
//    ecore_con_url_shutdown();
//    ecore_con_shutdown();
}
