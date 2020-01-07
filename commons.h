#include <gtk/gtk.h>

typedef struct{
    char *reference_path;
	char *title;
	char *background_image;
    int *argc;
    char **argv;
} mplayer_window_config;