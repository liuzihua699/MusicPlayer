#include <gtk/gtk.h>
#include <string.h>
#include <pthread.h>
typedef struct{
	GtkWidget *background;
	GtkWidget *background_fixed;
	GtkWidget *background_image;
	GtkWidget *toggle_button_led1;
	GtkWidget *toggle_button_led2;
	GtkWidget *toggle_button_led3;
    GtkWidget *toggle_button_led4;
	GtkWidget *toggle_button_led5;
}
gtk_ui_t;
/* 
	初始化背景窗口
	包括窗口的创建，背景图片设置
*/
void background_init(gtk_ui_t *ui)
{
	GdkPixbuf *imge_buf_src;
	GdkPixbuf *imge_buf_dest;
	ui->background = gtk_window_new(GTK_WINDOW_TOPLEVEL);//创建窗口
	g_signal_connect(ui->background, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	gtk_window_set_position(GTK_WINDOW(ui->background), GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(ui->background), 800, 480);
	ui->background_fixed = gtk_fixed_new();
	gtk_container_add(GTK_CONTAINER(ui->background), ui->background_fixed);
	imge_buf_src = gdk_pixbuf_new_from_file("./back.jpg", NULL);//背景图片的设置
	imge_buf_dest = gdk_pixbuf_scale_simple(imge_buf_src, 800, 480, GDK_INTERP_BILINEAR);
	g_object_unref(imge_buf_src);
	ui->background_image = gtk_image_new_from_pixbuf(imge_buf_dest);
	g_object_unref(imge_buf_dest);
	gtk_fixed_put(GTK_FIXED(ui->background_fixed), ui->background_image, 0, 0);
}
void button_callback(GtkWidget *widget, gpointer data)
{
	gtk_ui_t *ui = data;
	const gchar *str = NULL;
	str = gtk_button_get_label(GTK_BUTTON(widget));
	if(0 == strncmp(str, "快进", strlen("快进")))
	{
		printf("toggle_button_led1 pressed  快进\n");
	}
	
}
void button_init(gtk_ui_t *ui)
{
	ui->toggle_button_led1 = gtk_toggle_button_new_with_label("快进");
	gtk_fixed_put(GTK_FIXED(ui->background_fixed), ui->toggle_button_led1, 600, 350);
	gtk_widget_set_size_request(ui->toggle_button_led1, 100, 60);
	g_signal_connect(G_OBJECT(ui->toggle_button_led1), "clicked",	G_CALLBACK(button_callback), ui);
	
}
void gtk_ui(void)
{
	static gtk_ui_t ui;
	memset(&ui, 0, sizeof(ui));
	background_init(&ui);
	button_init(&ui);
	gtk_widget_show_all(ui.background);
}
int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv);
	gtk_ui();
	gtk_main();
	return 0;
}
