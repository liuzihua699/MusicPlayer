#include<stdio.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include<string.h>


GtkWidget *VisitTable();
GtkWidget *vistwindow();
GtkWidget *firstTextEntry();
GtkWidget *makeButtonBox();
GtkWidget   *window;


// 销毁
void destroy (GtkWidget *widget, gpointer *data)
{
    gtk_main_quit();
}



// 创建按钮组
GtkWidget *makeButtonGroup ()
{
    GtkWidget   *hbox;
    GtkWidget   *button;
    int         i;
    char *titles[10] = {"暂停", "播放", "快进", "快退", "上一首", "下一首", "音量增", "音量减", "静音", "取消静音"};

    hbox = gtk_hbox_new (FALSE,0);
    for (i = 0; i < 10; i++) {    // 绘制
        button = gtk_button_new_with_label (titles[i]);
        // gtk_signal_connect (GTK_OBJECT (button),"clicked",GTK_SIGNAL_FUNC (click_button1),NULL);
        gtk_box_pack_start (GTK_BOX (hbox),button,TRUE,TRUE,10);
    }
    gtk_widget_show_all (window);
    return hbox;
}


// 创建布局
GtkWidget * VisitTable ()
{
    GtkWidget        *Vtable;
    GtkWidget        *image;
    GtkWidget        *checkButtons;
    GtkWidget        *textEntry;
    GtkWidget        *buttonBox;

    Vtable = gtk_table_new (3,2,FALSE);

    gtk_widget_show_all (Vtable);
    image = gtk_image_new_from_file ("2.jpg");
    gtk_box_pack_start (GTK_BOX (Vtable),image,TRUE,TRUE,0);
    gtk_table_attach (GTK_TABLE(Vtable),image,0,1,0,1,GTK_FILL|GTK_EXPAND|GTK_SHRINK,0,0,0);
    gtk_widget_show_all (image);

    gtk_window_set_decorated(GTK_WINDOW (Vtable),FALSE);//设置无边框

    buttonBox = makeButtonGroup();
    gtk_table_attach (GTK_TABLE(Vtable),buttonBox,0,1,1,2,GTK_EXPAND |GTK_FILL|GTK_SHRINK,0,5,10);
    gtk_widget_show (buttonBox);

    gtk_widget_show_all (Vtable);
    return Vtable;
}

// 创建窗口
int  visitwindow(int *argc,char **argv)
{
    GtkWidget    *Vtable;

    gtk_init (argc,&argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "音乐播放器");
    gtk_window_set_default_size (GTK_WINDOW(window),587,468);
    gtk_window_set_opacity (GTK_WINDOW(window),100);//设置透明度函数
    g_signal_connect (GTK_OBJECT (window),"destroy",GTK_SIGNAL_FUNC(destroy),NULL); // 窗口销毁的回调

    Vtable = VisitTable();
    gtk_container_add (GTK_CONTAINER(window),Vtable);
    gtk_widget_show_all (window);
    gtk_main ();
}



int main (int argc, char *argv[])
{
    visitwindow (&argc,argv);
    return 0;
}