#ifndef __PLUGIN_COMMANDS__
#define __PLUGIN_COMMANDS__

G_BEGIN_DECLS

void execute_command(GtkWidget *widget, gpointer data);

gboolean update_info(struct UpdateData *data);

void run_tui_mocp(GtkWidget *widget, struct MocConfig *config);

void run_server_mocp(GtkWidget *widget, struct MocConfig *config);

void run_server_mocp_playlist(GtkWidget *widget, struct MocConfig *config);

G_END_DECLS

#endif