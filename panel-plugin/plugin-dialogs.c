#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <libxfce4panel-2.0/libxfce4panel/libxfce4panel.h>
#include <libxfce4ui/libxfce4ui.h>

#include "plugin.h"
#include "plugin-dialogs.h"



#define PACKAGE_VERSION "0.1.1"
#define PACKAGE_NAME "xfce4-moc-plugin"




static void new_label (GtkGrid *grid, guint row, const gchar *labeltext, GtkWidget *target)
{
    GtkWidget *label = gtk_label_new_with_mnemonic (labeltext);
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start (label, 12);
    gtk_label_set_mnemonic_widget(GTK_LABEL(label), target);
    gtk_grid_attach(grid, label, 0, row, 1, 1);
}


static void set_command_run_mocp(GtkWidget *widget, struct MocConfig *config) {
    const gchar *text = gtk_entry_get_text(GTK_ENTRY(widget));
    if (strcmp(text, "") != 0)
        strcpy(config->commandRunMocp, text);
}


static void set_last_dir_mocp(GtkWidget *widget, struct MocConfig *config) {
    strcpy(config->lastDir, gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(widget)));
    FILE *fp = fopen(config->pathFileLastDir, "w");
    fprintf(fp, "%s", config->lastDir);
    fclose(fp);
}

static void set_moc_repeat(GtkWidget *chooser, struct MocConfig *config) {
    config->repeat = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chooser));
}

static void set_moc_shuffle(GtkWidget *chooser, struct MocConfig *config) {
    config->shuffle = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chooser));
}

static void set_moc_autonext(GtkWidget *chooser, struct MocConfig *config) {
    config->autonext = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(chooser));
}


static GtkWidget *setup_chooser_toggle_button(GtkWidget *grid, gint top, gboolean active, const gchar *label) {
    GtkWidget *chooser = gtk_check_button_new();
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(chooser), active);

    new_label (GTK_GRID (grid), top, label, chooser);
    gtk_grid_attach (GTK_GRID (grid), chooser, 1, top, 1, 1);

    return chooser;
}



void moc_configure_create(XfcePanelPlugin *plugin, struct MocConfig *config) {
    GtkWidget *dlg = xfce_titled_dialog_new();
    GtkWidget *chooser, *label, *entry;

    gtk_window_set_title(GTK_WINDOW(dlg), _("Options"));
    gtk_window_set_icon_name(GTK_WINDOW(dlg), "xfce.plugin.moc.icon");

    GtkBox *content = GTK_BOX(gtk_dialog_get_content_area (GTK_DIALOG(dlg)));

    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_column_spacing (GTK_GRID(grid), 12);
    gtk_grid_set_row_spacing (GTK_GRID(grid), 6);
    gtk_container_set_border_width(GTK_CONTAINER(grid), 12);
    gtk_box_pack_start (content, grid, TRUE, TRUE, 0);

    label = gtk_label_new (NULL);
    gtk_label_set_markup (GTK_LABEL (label), _("<b>General</b>"));
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_grid_attach (GTK_GRID (grid), label, 0, 0, 1, 1);
    
    chooser = gtk_file_chooser_button_new (_("Folder selection"), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
    gtk_widget_set_hexpand (chooser, TRUE);
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(chooser), config->lastDir);
    // gtk_widget_set_halign(chooser, GTK_ALIGN_START);
    g_signal_connect(G_OBJECT(chooser), "selection-changed", G_CALLBACK(set_last_dir_mocp), config);
    new_label (GTK_GRID (grid), 1, _("Music folder:"), chooser);
    gtk_grid_attach (GTK_GRID (grid), chooser, 1, 1, 1, 1);

    entry = gtk_entry_new();
    gtk_widget_set_hexpand (entry, TRUE);
    gtk_entry_set_text(GTK_ENTRY(entry), config->commandRunMocp);
    // gtk_widget_set_halign(entry, GTK_ALIGN_START);
    g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(set_command_run_mocp), config);
    new_label (GTK_GRID (grid), 3, _("MOC launch command:"), entry);
    gtk_grid_attach (GTK_GRID (grid), entry, 1, 3, 1, 1);


    label = gtk_label_new (NULL);
    gtk_label_set_markup (GTK_LABEL (label), _("<b>At startup enable...</b>"));
    gtk_widget_set_halign(label, GTK_ALIGN_START);
    gtk_widget_set_valign(label, GTK_ALIGN_CENTER);
    gtk_grid_attach (GTK_GRID (grid), label, 0, 4, 1, 1);

    chooser = setup_chooser_toggle_button(grid, 5, config->repeat, _("Repeat"));
    g_signal_connect(G_OBJECT(chooser), "clicked", G_CALLBACK(set_moc_repeat), config);

    chooser = setup_chooser_toggle_button(grid, 6, config->shuffle, _("Shuffle"));
    g_signal_connect(G_OBJECT(chooser), "clicked", G_CALLBACK(set_moc_shuffle), config);

    chooser = setup_chooser_toggle_button(grid, 7, config->autonext, _("Autonext"));
    g_signal_connect(G_OBJECT(chooser), "clicked", G_CALLBACK(set_moc_autonext), config);


    gtk_widget_show_all(dlg);
}



void moc_show_about(XfcePanelPlugin *plugin, struct MocConfig *config) {
    gtk_show_about_dialog (NULL,
      "logo-icon-name", "xfce.plugin.moc.icon",
      "license", xfce_get_license_text (XFCE_LICENSE_TEXT_LGPL),
      "version", PACKAGE_VERSION,
      "program-name", PACKAGE_NAME,
      "comments", _("Xfce plugin for MOC console audio player"),
      "website", "https:/notabug.org/Ktoto",
      "copyright", "Copyright \302\251 2003-2023 The Xfce development team",
      "author", "Ktoto <ktoto@ktoto.org>", NULL);
}
