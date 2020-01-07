
#ifndef __OTHERMAIN_H_
#define __OTHERMAIN_H_

typedef struct _window //结构体 方便进行传地址（内容）
{
	GtkWidget *main_window;
	GtkWidget *table;
	GtkWidget *image;
	GtkWidget *image01;
	GtkWidget *image02;
	GtkWidget *image03;
	GtkWidget *image04;
	GtkWidget *button;
	GtkWidget *button_off;//静音键
	GtkWidget *button_open;//备用键
	GtkWidget *button_back; //下一曲键
	GtkWidget *button_menu;//菜单
	GtkWidget *button_front;//上一曲键
	GtkWidget *button_play;//播放暂停键
	GtkWidget *lable_song_name;//歌曲名称
	GtkWidget *lable_song_lrc;//专辑
	GtkWidget *lable_song_artist;//艺术家
	GtkWidget *lable_song_time;
	GtkWidget *lable_now_time;
	GtkWidget *progress_bar;
	GtkWidget *progress_bar2;
	GtkWidget *image_background;	
	GtkWidget *image05;
	GtkWidget *fixed;//
	GtkWidget *clist;//分栏列表
	GtkWidget *lable_lrc;
	gchar buf[128];//现在放的歌曲名称格式为   xxxx。MP3
	gchar buff[128];//进行对比所用的歌名   如果切换歌曲buf！=buff   如果没切歌 buf=buff
	gchar song_name[128];   
	gchar *song_list[100];//歌曲名称   
	gchar nowtime[64];//歌曲播放的当前时间（字符串形式）
	gchar song_lrc[128];
	gchar song[128];
	gchar singer[128];
	gint now_time;
	gint lrc_time;
	gint fd1;//命名管道
	gint fd2;//备用管道
	gint ii;  //当前播放歌曲编号
	gint max;//歌曲总数
	gint fd[2];//无名管道
	gint file_flag;
	gint signal_i;
	gint signal_a;
	gint flags;
	gint now_time_s;
	gint tal_time;

	//gint xx;//进度条横坐标
	float xx;
	float yy;


}MPLAYER; //重命名为WINDOW

// extern void deal_button(GtkButton *button, gpointer user_data);
extern void load_image(GtkWidget *tmp, const gchar *file_name, gint w, gint h);
extern void progress_bar_add( gpointer user_data,gdouble bar);
extern void callback(GtkWidget *widget, gpointer data);
extern void callback1(GtkWidget *widget, gpointer data);
extern int get_menu(MPLAYER *player);
extern void set_label_font_size(GtkWidget *label, int size);
extern void my_select_row(GtkCList *clist, gint arg1, gint arg2, GdkEvent *arg3, MPLAYER *player) ; 
extern void next_song(GtkWidget *widget, gpointer data);
extern void last_song(GtkWidget *widget, gpointer data);//上一曲
extern gpointer the_lrc(gpointer arg);
extern int sungtk_widget_set_font_color(GtkWidget *widget, const char *color_buf, gboolean is_button);
extern int sungtk_color_get(const char *color_buf, GdkColor *color);
//extern int sungtk_clist_select_row(SunGtkCList *clist, gint row, const gchar *color_buf);
#endif