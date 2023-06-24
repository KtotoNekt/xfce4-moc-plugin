#ifndef __PLUGIN_H__
#define __PLUGIN_H__

G_BEGIN_DECLS

enum State {
    STOP,
    PAUSE,
    PLAY
};

struct UpdateData {
    GtkButton *btn_toggle_play;
    XfcePanelPlugin *plugin;
};

struct MocConfig {
    XfcePanelPlugin *plugin;
    char lastDir[100];
    char pathFileLastDir[100];
    char commandRunMocp[100];
    gboolean repeat;
    gboolean shuffle;
    gboolean autonext;
};

struct MocInfo {
    char *file;
    char *totalTime;
    char *currentTime;
    enum State state;
};


void plugin_save(XfcePanelPlugin *plugin, struct MocConfig *config);

G_END_DECLS

#endif