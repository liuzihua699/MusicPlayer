#include <unistd.h>
#include<stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "commons.h"


#define LEN 10
#define MUSIC_COUNT 3
#define N 32

GtkWidget                               *window;              // 主窗口
GtkWidget                               *entry;               // 正在播放的歌曲显示


char                                    *songInfo;
int                                     fd = 0;
int                                     fd_[2];

char                                    buf[N] = {'\0'};
char                                    cmd[N] = {'\0'};

int                                     music_index = 0;          // 当前播放的索引
int                                     status_code = 0;          // 状态码，0表示正在播放，1表示暂停
int                                     mute = 0;                 // 禁音状态，0表示非禁音，1表示禁音
int                                     vol = 100;                // 音量大小


static char *music[MUSIC_COUNT] = {
    "Mallow Flower.mp3", "奇迹の山.mp3", "魅夜的探戈.mp3", "沐春阳.mp3",
    "Sunflower.mp3", "Landscape.mp3", "黄昏.mp3"
};


// 标题集
static char *titles[10] = {"暂停", "播放", "快进", "快退", "上一首", "下一首", "音量增", "音量减", "静音", "取消静音"};


// ************************************** 功能按钮 ************************************** //

void pauses (GtkObject *object, gpointer user_data)  // 暂停
{
    if (status_code == 0) {
        send_command("pause\n");
        status_code = 1;
    } else {
        printf("%s已暂停...", music[music_index]);
    }
}

void play (GtkObject *object, gpointer user_data)    // 播放
{
    if (status_code == 1) {
        send_command("pause\n");
        status_code = 0;
    } else {
        printf("正在播放%s中...\n", music[music_index]);
    }
    
}

void fast (GtkObject *object, gpointer user_data)    // 快进
{
    send_command("seek 10\n");
    status_code = 0;
}

void slow (GtkObject *object, gpointer user_data)    // 快退
{
    send_command("seek -10\n");
    status_code = 0;
}

void add_volume (GtkObject *object, gpointer user_data)    // 音量增
{
    if (vol < 100) {
        vol += 10;
    }
    sprintf(cmd,"volume %d 1\n",vol);
    send_command(cmd);
}

void sub_volume (GtkObject *object, gpointer user_data)    // 音量减
{
    if (vol > 0) {
        vol -= 10;
    }
    sprintf(cmd,"volume %d 1\n",vol);
    send_command(cmd);
}

void stop_volume (GtkObject *object, gpointer user_data)    // 禁音
{
    printf("asd");
    if (mute == 0) {
        send_command("mute 1\n");
        mute = 1;
    }
}

void unstop_volume (GtkObject *object, gpointer user_data)    // 取消禁音
{
    if (mute == 1) {
        send_command("mute 0\n");
        mute = 0;
    }
}

void prev (GtkObject *object, gpointer user_data)    // 上一首
{
    char*   strbuf[100] = {0};
    music_index = music_index - 1;
    if (music_index < 1) music_index = MUSIC_COUNT - 1;
    sprintf (strbuf, "正在播放：%s", music[music_index]);
    gtk_entry_set_text(GTK_ENTRY(entry), strbuf);

}

void next (GtkObject *object, gpointer user_data)    // 下一首
{
    char*   strbuf[100] = {0};
    music_index = (music_index + 1) % MUSIC_COUNT;
    sprintf (strbuf, "正在播放：%s", music[music_index]);
    gtk_entry_set_text(GTK_ENTRY(entry), strbuf);
}

// ************************************** 功能按钮 ************************************** //



void fifo_init () 
{   
    int pip = mkfifo("./fifo_cmd",0644);
    if(pip < 0 && errno != EEXIST){
        perror("mkfifo");
        return -1;
    }

    if(pipe(fd_)==-1)   //fd[0] read fd[1] write
    {
        perror("pipe error");
        pipe(fd_);
    }

    pid_t pid = fork();

    // g_print(pid);

    if (pid > 0) {
        fd = open("./fifo_cmd",O_WRONLY);
        if (fd < 0) {
            perror("open error");
            exit(-1);
        }

        gtk_widget_show_all(window);
        gtk_main(); 
    } else if (pid == 0){
        execlp("mplayer", "mplayer", "-slave", "-quiet","-idle","-input", "file=./fifo_cmd","music/Sunflower.mp3",NULL);
    } else {
        perror("failed");
        exit(-1);
    }
}

void send_command(char *cmd)
{
    int r = write(fd,cmd,strlen(cmd));
    if(r != strlen(cmd))   //没成功写入
    {
        perror("失败写入");
    }
    else
    {
        printf("write Successful: %s\n",cmd);
    }
}


//关闭窗口
void close_window(GtkWidget *widget, GdkEvent *event, gpointer data)
{
    send_command("quit\n");
    unlink("./fifo_cmd");
    pid_t pid = getpid();
    printf("当前进程ID:%d,已退出.\n", pid);
    gtk_main_quit();
    printf("\n");
}

// 回调函数集
static void (*back_group[LEN])(GtkObject *object, gpointer user_data) = {pauses, play, fast, slow, prev, next, add_volume, sub_volume, stop_volume, unstop_volume};



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
        btn = gtk_builder_get_object(builder, buf);                                                       // 获取button1控件
        g_signal_connect (btn, "clicked", G_CALLBACK (back_group[i]), titles[i]);                         // 绑定"暂停"点击事件
    }
}





// 创建窗口
int  visitwindow(int *argc,char **argv)
{
    GtkBuilder                              *builder;
    GObject                                 *btn;

    gtk_init (&argc, &argv);                                        // gtk初始化

    

    builder = gtk_builder_new ();                                   // 指针分配空间
    gtk_builder_add_from_file (builder, "music.xml", NULL);         // 读取music.xml文件

    window = gtk_builder_get_object (builder, "window1");                                  // 获取window1控件
    g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);                // 绑定窗口销毁事件(必选)

    entry = gtk_builder_get_object(builder, "entry1");
    char*   strbuf[100] = {0};
    sprintf (strbuf, "正在播放：%s", music[music_index]);
    gtk_entry_set_text(GTK_ENTRY(entry), strbuf);

    bind_button_group (builder);                                                           // 绑定所有的按钮点击事件

    g_object_unref (G_OBJECT (builder));                                                   // 释放xml内存空间


    fifo_init ();


    // gtk_widget_show ((GtkWindow*) window);                                                 // 显示窗体
    // gtk_main();
}




int main (int argc, char *argv[])
{

    visitwindow (&argc, &argv);
    return 0;
}