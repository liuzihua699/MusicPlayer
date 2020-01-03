#include <gtk/gtk.h>
#include <string.h>
#include<stdio.h>

#define LEN 2


/**
 *  "暂停" 功能回调
 */
static void pauses (GtkObject *object, gpointer user_data)
{
    gtk_main_quit();
}

/**
 *  "播放" 功能回调
 */
static void play (GtkObject *object, gpointer user_data)
{
    g_print(user_data);
    g_print("\n");
}


// 回调函数数组
static void (*back_group[LEN])(GtkObject *object, gpointer user_data) = {
    pauses, play
};

static char *titles[10] = {"暂停", "播放", "快进", "快退", "上一首", "下一首", "音量增", "音量减", "静音", "取消静音"};


/**
 *  绑定按钮组
 */
void bind_button_group (GtkBuilder *builder)
{
    GObject                                 *btn;
    int i;
    char*   buf[100] = {0};

    for (i = 0; i < LEN; i++) {
        sprintf (buf, "button%d", i+1);
        btn = gtk_builder_get_object(builder, buf);                                      // 获取button1控件
        g_signal_connect (btn, "clicked", G_CALLBACK (back_group[i]), titles[i]);                         // 绑定"暂停"点击事件
    }
}




// 创建窗口
int  visitwindow(int *argc,char **argv)
{
    GtkBuilder                              *builder;
    GtkWidget                               *window;
    GObject                                 *btn;

    gtk_init (&argc, &argv);                                        // gtk初始化

    builder = gtk_builder_new ();                                   // 指针分配空间
    gtk_builder_add_from_file (builder, "music.xml", NULL);         // 读取music.xml文件

    window = gtk_builder_get_object (builder, "window1");                                  // 获取window1控件
    g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);                // 绑定窗口销毁事件(必选)

    bind_button_group (builder);                                                             // 绑定所有的按钮点击事件

    g_object_unref (G_OBJECT (builder));                                                   // 释放xml内存空间
    gtk_widget_show ((GtkWindow*) window);                                                 // 显示窗体
    gtk_main ();                                                                           // 事件响应函数
}



int main (int argc, char *argv[])
{
    visitwindow (&argc,argv);
    return 0;
}