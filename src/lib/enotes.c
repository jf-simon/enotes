#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "Enotes.h"

#include "enotes_private.h"

static int _enotes_init = 0;
int _enotes_lib_log_dom = -1;

EAPI int
enotes_init(void)
{
   _enotes_init++;
   if (_enotes_init > 1) return _enotes_init;

   eina_init();

   _enotes_lib_log_dom = eina_log_domain_register("enotes", EINA_COLOR_CYAN);
   if (_enotes_lib_log_dom < 0)
     {
	EINA_LOG_ERR("Enotes can not create its log domain.");
	goto shutdown_eina;
     }

   // Put here your initialization logic of your library

   eina_log_timing(_enotes_lib_log_dom, EINA_LOG_STATE_STOP, EINA_LOG_STATE_INIT);

   return _enotes_init;

 shutdown_eina:
   eina_shutdown();
   _enotes_init--;

   return _enotes_init;
}

EAPI int
enotes_shutdown(void)
{
   _enotes_init--;
   if (_enotes_init != 0) return _enotes_init;

   eina_log_timing(_enotes_lib_log_dom,
		   EINA_LOG_STATE_START,
		   EINA_LOG_STATE_SHUTDOWN);

   // Put here your shutdown logic

   eina_log_domain_unregister(_enotes_lib_log_dom);
   _enotes_lib_log_dom = -1;

   eina_shutdown();

   return _enotes_init;
}

EAPI void
enotes_library_call(void)
{
   INF("Not really doing anything useful.");
}
