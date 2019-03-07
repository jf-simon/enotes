#ifdef HAVE_CONFIG_H
# include "config.h"
#else
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT
#endif

#include <curl/curl.h>

#include "enotes.h"


char*
find_data(char *string, char *start1, char *end1)
{
	char *string1 = calloc(strlen(string)+1, sizeof(char));
		
	if((strstr(string, start1) == NULL) || (strstr(string, end1) == NULL))
	{
		return "";
	}
	else
	{
		int start_len = strlen(start1);
		
		strncpy(string1, strstr(string, start1)+start_len, strstr(string, end1)-strstr(string, start1)-strlen(start1));
	
			return string1;
	}
	free(string1);
}
 
 
char 
*stringReplace(char *search, char *replace, char *string)
{
   while(strstr(string, search) != NULL)
   {
		
	char *tempString, *searchStart;
	   int len=0;
	   // Speicher reservieren
	   tempString = (char*) malloc(strlen(string) * sizeof(char)*10);
	
	   searchStart = strstr(string, search);

		// temporaere Kopie anlegen
   	strcpy(tempString, string);

	   // ersten Abschnitt in String setzen
	   len = searchStart - string;
      string[len] = '\0';

	   // zweiten Abschnitt anhaengen
	   strcat(string, replace);

	   // dritten Abschnitt anhaengen
	   len += strlen(search);
	   strcat(string, (char*)tempString+len);

	   // Speicher freigeben
	   free(tempString);
  }
	return string;
}
