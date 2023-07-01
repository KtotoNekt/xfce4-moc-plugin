#include <libxfce4panel-2.0/libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>
#include <stdio.h>
#include <string.h>

#include "plugin.h"
#include "commands.h"


#define MOCP_COMMAND   "/usr/bin/mocp "


static char* execute_command_and_get_result(char *command) {
    gchar *result;

    g_spawn_command_line_sync(command, &result, NULL, 0, NULL);

    char *p = strchr(result, '\n');
    if (p != NULL) *p = '\0';

    return result;
}

static enum State get_state() {
    char *str_state = execute_command_and_get_result(MOCP_COMMAND "-Q %state");
    
    if (strcmp(str_state, "PLAY") == 0) {
        return PLAY;
    } else if (strcmp(str_state, "PAUSE") == 0) {
        return PAUSE;
    } else  {
        return STOP;
    }
}

static struct MocInfo get_info() {
    struct MocInfo info;

    info.state = get_state();
    info.currentTime = execute_command_and_get_result(MOCP_COMMAND "-Q %ct");
    info.totalTime = execute_command_and_get_result(MOCP_COMMAND "-Q %tt");
    info.file = execute_command_and_get_result(MOCP_COMMAND "-Q %file");

    return info;
}

static void update_state(GtkButton *btn, enum State state) {
    GtkWidget *image;

    switch (state) {
        case PLAY:
            image = gtk_image_new_from_icon_name("xfce.plugin.moc.pause", GTK_ICON_SIZE_BUTTON);
            break;
        case PAUSE:
            image = gtk_image_new_from_icon_name("xfce.plugin.moc.play", GTK_ICON_SIZE_BUTTON);
            break;
        case STOP:
            image = gtk_image_new_from_icon_name("xfce.plugin.moc.not-run", GTK_ICON_SIZE_BUTTON);
            break;
    }

    gtk_button_set_image(btn, image);
}




void execute_command(GtkWidget *widget, gpointer data) {
    switch (GPOINTER_TO_INT(data)) {
        case 0:
            g_spawn_command_line_async(MOCP_COMMAND "-f", NULL);
            break;
        case 1:
            g_spawn_command_line_async(MOCP_COMMAND "-G", NULL);
            break;
        case 2:
            g_spawn_command_line_async(MOCP_COMMAND "-r", NULL);
            break;
    }
}

gboolean update_info(struct UpdateData *data) {
    struct MocInfo info = get_info();

    update_state(data->btn_toggle_play, info.state);

    if (info.state != STOP) {
        gchar *markup = g_markup_printf_escaped("%s\n%s - %s", info.file, info.currentTime, info.totalTime);
    
        gtk_widget_set_tooltip_markup(GTK_WIDGET(data->plugin), markup);
        g_free (markup);
    } else {
        gtk_widget_set_tooltip_text(GTK_WIDGET(data->plugin), _("State: STOP"));
    }

    return TRUE;
}

static void exec_command_set_option_moc(gboolean option, char *option_str) {
    char command[512];
    
    if (option) {
        snprintf(command, sizeof command, "%s%s%s", MOCP_COMMAND, "-o ", option_str);
    } else {
        snprintf(command, sizeof command, "%s%s%s", MOCP_COMMAND, "-u ", option_str);
    }

    system(command);
}

static void init_moc(struct MocConfig *config) {
    exec_command_set_option_moc(config->repeat, "repeat");

    exec_command_set_option_moc(config->shuffle, "shuffle");

    exec_command_set_option_moc(config->autonext, "autonext");
}

void run_tui_mocp(GtkWidget *widget, struct MocConfig *config) {
    system(config->commandRunMocp);

    init_moc(config);
}

void run_server_mocp(GtkWidget *widget, struct MocConfig *config) {
    char command[512];
    snprintf(command, sizeof command, "%s%s%s%s", MOCP_COMMAND, "-l ", config->lastDir, "/*");

    system(MOCP_COMMAND "-S");
    system(command);

    init_moc(config);
}

void run_server_mocp_playlist(GtkWidget *widget, struct MocConfig *config) {
    system(MOCP_COMMAND "-S");
    system(MOCP_COMMAND "-p");

    init_moc(config);
}