#include <libxfce4panel-2.0/libxfce4panel/libxfce4panel.h>
#include "commands-mocp.h"
#include "configure.h"


static struct MocConfig *setup_config_moc(XfcePanelPlugin *plugin);

static struct UpdateData *setup_update_data(GtkWidget *btn, XfcePanelPlugin *plugin);

static void setup_menu_item(XfcePanelPlugin *plugin, GtkWidget *widget, void (*func)(GtkWidget*, gpointer), gpointer data);

static void plugin_save(XfcePanelPlugin *plugin, struct MocConfig *config);



static void constructor(XfcePanelPlugin *plugin)  {
    GtkWidget *window = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    
    GtkWidget *button_toggle_play = gtk_button_new();
    GtkWidget *button_next_song = gtk_button_new();
    GtkWidget *button_previous_song = gtk_button_new();

    struct UpdateData *info = setup_update_data(button_toggle_play, plugin);
    struct MocConfig *config = setup_config_moc(plugin);

    gtk_button_set_image(GTK_BUTTON(button_next_song), gtk_image_new_from_icon_name("xfce.plugin.moc.next", 32));
    gtk_button_set_image(GTK_BUTTON(button_previous_song), gtk_image_new_from_icon_name("xfce.plugin.moc.previous", 32));

    gtk_container_add(GTK_CONTAINER(window), button_previous_song);
    gtk_container_add(GTK_CONTAINER(window), button_toggle_play);
    gtk_container_add(GTK_CONTAINER(window), button_next_song);
    gtk_container_add(GTK_CONTAINER(plugin), window);

    g_timeout_add(500, update_info, info);

    g_signal_connect(button_previous_song, "clicked", G_CALLBACK(execute_command), GINT_TO_POINTER(2));
    g_signal_connect(button_toggle_play, "clicked", G_CALLBACK(execute_command), GINT_TO_POINTER(1));
    g_signal_connect(button_next_song, "clicked", G_CALLBACK(execute_command), GINT_TO_POINTER(0));

    xfce_panel_plugin_menu_show_about(plugin);
    xfce_panel_plugin_menu_show_configure(plugin);

    g_signal_connect(plugin, "configure-plugin", G_CALLBACK(moc_configure_create), config);
    g_signal_connect(plugin, "about", G_CALLBACK(moc_show_about), config);
    g_signal_connect(G_OBJECT (plugin), "save", G_CALLBACK(plugin_save), config);
    
    GtkWidget *run_server_layout = gtk_menu_item_new_with_label (_("Запустить сервер MOC"));
    GtkWidget *open_launch_layout = gtk_menu_item_new_with_label(_("Открыть MOC"));

    setup_menu_item(plugin, run_server_layout, run_server_mocp, config);
    setup_menu_item(plugin, open_launch_layout, run_tui_mocp, config);

    gtk_widget_show_all(window);
}



static struct UpdateData *setup_update_data(GtkWidget *btn, XfcePanelPlugin *plugin) {
    struct UpdateData *info = g_new0(struct UpdateData, 1);
    info->btn_toggle_play = GTK_BUTTON(btn);
    info->plugin = plugin;

    return info;
}


static void setup_menu_item(XfcePanelPlugin *plugin, GtkWidget *widget, void (*func)(GtkWidget*, gpointer), gpointer data) {
    gtk_widget_show (widget);
    xfce_panel_plugin_menu_insert_item (plugin, GTK_MENU_ITEM (widget));
    g_signal_connect(G_OBJECT (widget), "activate", G_CALLBACK(func), data);
}

void plugin_save(XfcePanelPlugin *plugin, struct MocConfig *config) {
    XfceRc *rc;
    gchar *file;
    
    file = xfce_panel_plugin_save_location(config->plugin, TRUE);

    if (G_UNLIKELY (file == NULL))
    {
       return;
    }

    rc = xfce_rc_simple_open(file, FALSE);
    g_free(file);

    if (G_LIKELY(rc != NULL)) {
        g_print("%s\n", config->commandRunMocp);
        xfce_rc_write_entry(rc, "command-run-mocp", config->commandRunMocp);
        xfce_rc_close(rc);
    }
}

static void plugin_read(struct MocConfig *config) {
    XfceRc *rc;
    gchar *file;
    const gchar *command;

    file = xfce_panel_plugin_save_location (config->plugin, TRUE);

    if (G_LIKELY (file != NULL)) {
        rc = xfce_rc_simple_open (file, TRUE);
        
        g_free(file);
        
        if (G_LIKELY (rc != NULL)) {
            command = xfce_rc_read_entry(rc, "command-run-mocp", "xfce4-terminal -e mocp");
            strcpy(config->commandRunMocp, (const char*)command);
            xfce_rc_close (rc);

            return;
        }
    }

    strcpy(config->commandRunMocp, "xfce4-terminal -e mocp");
}


static struct MocConfig *setup_config_moc(XfcePanelPlugin *plugin) {
    struct MocConfig *config = g_new0(struct MocConfig, 1);
    const char *homeDir = (const char *)g_get_home_dir();
    char dir[strlen(homeDir)+50]; 

    strcpy(dir, homeDir);
    strcat(dir, "/.moc/");
    strcat(dir, "last_directory");

    config->plugin = plugin;

    strcpy(config->pathFileLastDir, dir);

    FILE *fp = fopen(dir, "r");
    fgets(config->lastDir, 100, fp);
    fclose(fp);

    plugin_read(config);

    return config;
}



XFCE_PANEL_PLUGIN_REGISTER(constructor);