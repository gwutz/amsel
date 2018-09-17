#include <glib.h>
#include "aml-application.h"

gint
main (gint   argc,
      gchar *argv[])
{
  g_autoptr(AmlApplication) app;
  g_set_prgname ("amsel");
  g_set_application_name ("Amsel");

  app = aml_application_new ();

  return g_application_run (G_APPLICATION (app), argc, argv);
}
