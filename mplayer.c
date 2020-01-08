#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include "commons.h"
#include <dirent.h>
#include <stdlib.h>


#define LEN 10
#define MUSIC_MAX_COUNT 1000
#define N 50
#define MUSIC_FOLDER "./music"

GtkBuilder                              *builder;
GtkWidget                               *window;              // 主窗口
GtkWidget                               *entry;               // 正在播放的歌曲显示
GObject *btn_pause;       // 暂停按钮
GObject *btn_play;       // 播放按钮
GObject *btn_vol_stop;       // 禁音按钮
GObject *btn_vol_unstop;       // 取消禁音按钮
GObject *btn_prev;       // 上一首按钮
GObject *btn_next;       // 下一首按钮


int                                     music_index = 0;         // 当前播放的索引
int                                     music_length = 0;        // 歌曲数量


int                                     fd = 0;
int                                     fd_[2];

char                                    buf[N] = {"\0"};
char                                    cmd[N] = {"\0"};

static int                                     status_code = 0;          // 状态码，0表示正在播放，1表示暂停
static int                                     mute = 0;                 // 禁音状态，0表示非禁音，1表示禁音
static int                                     vol = 100;                // 音量大小



char* ReplaceSubStr(const char* str, const char* srcSubStr, const char* dstSubStr, char* out);      // 字符串全局替换



// static char *music[MUSIC_COUNT] = {
//     "Mallow Flower.mp3", "奇迹の山.mp3", "魅夜的探戈.mp3", "沐春阳.mp3",
//     "Sunflower.mp3", "Landscape.mp3", "黄昏.mp3"
// };
char *music[MUSIC_MAX_COUNT] = {NULL};


// 标题集
char *titles[10] = {"暂停", "播放", "快进", "快退", "上一首", "下一首", "音量增", "音量减", "静音", "取消静音"};


int readFileList(char *basePath)
 {
    DIR *dir;
    struct dirent *ptr;
    char base[1000];

    if ((dir=opendir(basePath)) == NULL) {
        perror("Open dir error...");
        exit(1);
    }

    int i = 0;
    while ((ptr=readdir(dir)) != NULL) {
        if(strcmp(ptr->d_name,".")==0 || strcmp(ptr->d_name,"..")==0)    //current dir OR parrent dir
            continue;
        else if(ptr->d_type == 8) {    // DT_REG
            music[i]=(char*)malloc(sizeof(ptr->d_name) + 1);
            strcpy(music[i], ptr->d_name);
            i++;
        }
        else if(ptr->d_type == 10)    // DT_LNK
            printf("d_name:%s/%s\n",basePath,ptr->d_name);
        else if(ptr->d_type == 4)    // DT_DIR
        {
            memset(base,'\0',sizeof(base));
            strcpy(base,basePath);
            strcat(base,"/");
            strcat(base,ptr->d_name);
            readFileList(base);
        }
    }
    music_length = i;
    closedir(dir);
    return 1;
}


// ************************************** 功能按钮 ************************************** //
void pauses (GtkObject *object, gpointer user_data)  // 暂停
{
    if (status_code == 0) {
        send_command("\npause\n");
        status_code = 1;
        gtk_widget_set_sensitive(btn_pause, FALSE);
        gtk_widget_set_sensitive(btn_play, TRUE);
    } else {
        printf("%s已暂停...\n", music[music_index]);
    }
}

void play (GtkObject *object, gpointer user_data)    // 播放
{
    if (status_code == 1) {
        send_command("\npause\n");
        status_code = 0;
        gtk_widget_set_sensitive(btn_pause, TRUE);
        gtk_widget_set_sensitive(btn_play, FALSE);
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
    if (mute == 0) {
        send_command("mute 1\n");
        mute = 1;
        gtk_widget_set_sensitive(btn_vol_stop, FALSE);
        gtk_widget_set_sensitive(btn_vol_unstop, TRUE);
    }
}

void unstop_volume (GtkObject *object, gpointer user_data)    // 取消禁音
{
    if (mute == 1) {
        send_command("mute 0\n");
        mute = 0;
        gtk_widget_set_sensitive(btn_vol_stop, TRUE);
        gtk_widget_set_sensitive(btn_vol_unstop, FALSE);
    }
}

void prev (GtkObject *object, gpointer user_data)    // 上一首
{
    char*   strbuf[100] = {0};
    music_index = music_index - 1;
    if (music_index <= 0) music_index = 0;
    sprintf (strbuf, "正在播放：%s", music[music_index]);
    gtk_entry_set_text(GTK_ENTRY(entry), strbuf);

    if (music_index <= 0) {
        gtk_widget_set_sensitive(btn_prev, FALSE);
        gtk_widget_set_sensitive(btn_next, TRUE);
    } else if (music_index == music_length-1) {
        gtk_widget_set_sensitive(btn_prev, TRUE);
        gtk_widget_set_sensitive(btn_next, FALSE);
    } else {
        gtk_widget_set_sensitive(btn_prev, TRUE);
        gtk_widget_set_sensitive(btn_next, TRUE);
    }

    status_code = 0;

    char song[100];
    sprintf(song,"%s%s","./music/",music[music_index]);

    char format_song[100];
    ReplaceSubStr(song, " ", "\\ ", format_song);   // 格式化文件路径

    sprintf(cmd,"%s%s\n","loadfile ",format_song);
    send_command(cmd);
}

void next (GtkObject *object, gpointer user_data)    // 下一首
{
    char*   strbuf[100] = {0};
    music_index = (music_index + 1) % music_length;
    sprintf (strbuf, "正在播放：%s", music[music_index]);
    gtk_entry_set_text (GTK_ENTRY(entry), strbuf);

    if (music_index <= 0) {
        gtk_widget_set_sensitive(btn_prev, FALSE);
        gtk_widget_set_sensitive(btn_next, TRUE);
    } else if (music_index == music_length-1) {
        gtk_widget_set_sensitive(btn_prev, TRUE);
        gtk_widget_set_sensitive(btn_next, FALSE);
    } else {
        gtk_widget_set_sensitive(btn_prev, TRUE);
        gtk_widget_set_sensitive(btn_next, TRUE);
    }

    status_code = 0;

    char song[100];
    sprintf(song,"%s%s","./music/",music[music_index]);

    char format_song[100];
    ReplaceSubStr(song, " ", "\\ ", format_song);   // 格式化文件路径

    sprintf (cmd,"%s%s\n","loadfile ",format_song);
    send_command (cmd);
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

    if (pid > 0) {
        fd = open("./fifo_cmd",O_WRONLY);
        if (fd < 0) {
            perror("open error");
            exit(-1);
        }
        gtk_widget_show_all(window);            // 显示所有窗口

        gtk_widget_set_sensitive(btn_play, FALSE);  // 默认设置"播放"按钮变灰
        gtk_widget_set_sensitive(btn_vol_unstop, FALSE);  // 默认设置"暂停"按钮变灰
        gtk_widget_set_sensitive(btn_prev, FALSE);  // 默认设置"上一首"按钮变灰
        gtk_main(); 
    } else if (pid == 0){
        dup2(fd_[1],1);

        char song[100];
        sprintf (song,"%s%s","./music/",music[music_index]);

        // char format_song[100];
        // ReplaceSubStr(song, " ", "\\ ", format_song);   // 格式化文件路径

        execlp ("mplayer", "mplayer", "-slave", "-quiet","-idle","-input", "file=./fifo_cmd",song,NULL);
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
    GObject                                 *btn;

    gtk_init (&argc, &argv);                                        // gtk初始化

    builder = gtk_builder_new ();                                   // 指针分配空间
    gtk_builder_add_from_file (builder, "music.xml", NULL);         // 读取music.xml文件

    window = gtk_builder_get_object (builder, "window1");                                 // 获取window1控件
    g_signal_connect (window, "destroy", G_CALLBACK (close_window), NULL);                // 绑定窗口销毁事件(必选)

    entry = gtk_builder_get_object(builder, "entry1");
    char*   strbuf[100] = {0};
    sprintf (strbuf, "正在播放：%s", music[music_index]);
    gtk_entry_set_text(GTK_ENTRY(entry), strbuf);

    bind_button_group (builder);                                                           // 绑定所有的按钮点击事件

    btn_pause = gtk_builder_get_object(builder, "button1");                                 // 获取"暂停"按钮
    btn_play = gtk_builder_get_object(builder, "button2");                                  // 获取"播放"按钮
    btn_vol_stop = gtk_builder_get_object(builder, "button9");                              // 获取"禁音"按钮
    btn_vol_unstop = gtk_builder_get_object(builder, "button10");                           // 获取"取消禁音"按钮
    btn_prev = gtk_builder_get_object(builder, "button5");                           // 获取"上一首"按钮
    btn_next = gtk_builder_get_object(builder, "button6");                           // 获取"下一首"按钮

    g_object_unref (G_OBJECT (builder));                                                    // 释放xml内存空间

    fifo_init ();

    // gtk_widget_show ((GtkWindow*) window);                                                 // 显示窗体
    // gtk_main();
}


// 初始化音乐列表
void init_music_folder (char *musicPath)
{
    DIR *dir;
    char basePath[1000];
    memset(basePath,'\0',sizeof(basePath));
    getcwd(basePath, 999);

    memset(basePath, '\0', sizeof(basePath));
    strcpy(basePath, musicPath);
    readFileList(basePath);
}


// 字符串全局替换
char* ReplaceSubStr(const char* str, const char* srcSubStr, const char* dstSubStr, char* out)
{
    char *p;
    char *_out = out;
    const char *_str = str;
    const char *_src = srcSubStr;
    const char *_dst = dstSubStr;
    int src_size = strlen(_src);
    int dst_size = strlen(_dst);
    int len = 0;
    do {
        p = strstr(_str, _src);
        if(p == 0) {
            strcpy(_out, _str);
            return out;
        }
        len = p - _str;
        memcpy(_out, _str, len);
        memcpy(_out + len, _dst, dst_size);
        _str = p + src_size;
        _out = _out + len + dst_size;
    } while(p);

    return out;
}


int main (int argc, char *argv[])
{
    // char str[] = "Mellow F lo wer.mp4"; 
    // char out[100];
    // ReplaceSubStr(str, " ", "\\ ", out);
    // printf("%s\n", out);

    init_music_folder (MUSIC_FOLDER);
    printf("音乐数量:%d\n", music_length);
    visitwindow (&argc, &argv);
    return 0;
}