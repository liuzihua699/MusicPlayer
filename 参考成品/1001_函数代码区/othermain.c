//================千锋教育——用良心做教育==================
//========      http://www.mobiletrain.org/       ========
//========================================================
//=========== 智能物联网+嵌入式系统开发工程师 ============
//--------------------by：Jason Zhang-------------------

#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "othermain.h"
#include "mplayer_gtk.h"
#include "lyrics_process.h"


//修改图片的函数
void load_image(GtkWidget *tmp, const gchar *file_name, gint w, gint h)
{
	//清空原有图片既清空控件内容
	gtk_image_clear(GTK_IMAGE(tmp));

	//创建一个图片资源控件
	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(file_name,NULL);

	//修改图片大小
	GdkPixbuf *dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf,w,h,GDK_INTERP_BILINEAR);
	//将图片资源设置到图片控件中
	gtk_image_set_from_pixbuf(GTK_IMAGE(tmp),dst_pixbuf);

	//释放图片资源
	g_object_unref(src_pixbuf);
	g_object_unref(dst_pixbuf);

	return;
}
/* 设置label字体的大小
  * label: 需要操作的label
  * size:  字体的大小
  */
void set_label_font_size(GtkWidget *label, int size)
{
	PangoFontDescription *font;  	// 字体指针
	font = pango_font_description_from_string("");          //参数为字体名字，任意即可
	// #define PANGO_SCALE 1024
	// 设置字体大小   
	pango_font_description_set_size(font, size*PANGO_SCALE);
	
	gtk_widget_modify_font(label, font);  // 改变label的字体大小
	pango_font_description_free(font);	  // 释放字体指针分配的空间
}

//显示button文字
// void deal_button(GtkWidget *widget, gpointer data)
// {
	
// 	static gboolean is_press = TRUE;
// 	GtkWidget *image = gtk_button_get_image( GTK_BUTTON(widget) ); // 获得按钮上面的图标
	
// 	if(TRUE == is_press){
// 		load_image(image, "./button/000.png",80,30);	// 自动图标换图标
// 		is_press = FALSE;
		
// 	}
// 	else
// 	{
// 		load_image(image, "./button/001.png",80,30);	// 暂停图标换图标
// 		is_press = TRUE;
		
// 	}
	



//---------------------------------------------------进度条-----------------------------------------
void progress_bar_add( gpointer user_data,gdouble bar)
{
	gdouble value =0.0;
	//给进度条设置新的值
	value += (0.1*bar);

	gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(user_data),value);

}


//暂停播放
void callback(GtkWidget *widget, gpointer data)
{
	int fd =0;
	fd = ((MPLAYER *)data)->fd1;
	char cmd[128] ="";//cmd为向子进程发送的控制mplayer信号

	static gboolean is_press = TRUE;
	GtkWidget *image = gtk_button_get_image( GTK_BUTTON(widget) ); // 获得按钮上面的图标
	
	if(TRUE == is_press){
		load_image(image, "./button/close.png",100,100);	// 自动图标换成暂停图标
		is_press = FALSE;
		((MPLAYER *)data)->signal_i = 1;
	}
	else
	{
		load_image(image, "./button/open.png",100,100);	// 暂停图标换成自动图标
		is_press = TRUE;
		((MPLAYER *)data)->signal_i = 0;
	}

	strcpy(cmd,"pause\n");
	printf("ffdd%d\n",fd );
	write(fd,cmd,strlen(cmd));

}


//------------------------------------------------静音----------------------------------------------
void callback1(GtkWidget *widget, gpointer data)
{

	int fd =0;
	fd = ((MPLAYER *)data)->fd1;
	char cmd[128] ="";//cmd为向子进程发送的控制mplayer信号 

	static gboolean is_press = TRUE;
	GtkWidget *image = gtk_button_get_image( GTK_BUTTON(widget) ); // 获得按钮上面的图标
	
	if(TRUE == is_press){
		load_image(image, "./button/on.png",80,30);	// 自动图标换图标
		is_press = FALSE;
		strcpy(cmd,"mute 1\n");
		((MPLAYER *)data)->signal_a = 0;
	}
	else
	{
		load_image(image, "./button/off.png",80,30);	// 暂停图标换图标
		is_press = TRUE;
		strcpy(cmd,"mute 0\n");
		((MPLAYER *)data)->signal_a = 1;
	}
	if (((MPLAYER *)data)->signal_i == 0)
	{
		write(fd,cmd,strlen(cmd));
	}
	

}


//----------------------------------点击分栏列表 获取所点击目标---------------------------------

void my_select_row(GtkCList *clist, gint arg1, gint arg2, GdkEvent *arg3, MPLAYER *player) 
{
	//开发板 人为更新 对A8有效
	gtk_widget_queue_draw(GTK_WIDGET(clist));
	
	//根据arg1与arg2来获取当前行的内容
	gchar *text=NULL;
	gchar buf[128] ="";

	gtk_clist_get_text(GTK_CLIST(clist),arg1,arg2,&text);

	sscanf(text, "%[^.]", buf);//将字符串取出
	sprintf(player->song_name, "../music/%s.mp3",buf);//组包
	sprintf(player->buf, "../lrc/%s.lrc", buf);//组包
	player->ii = arg1;//将歌曲所在行数赋值给ii，表示点击歌曲的编号
	
	char song_name[128] ="";
	sprintf(song_name, "loadfile %s\n", player->song_name);//再次组包为播放新歌曲命令
	
	write(player->fd1,song_name,strlen(song_name));//写入管道
	
	return;
}

//-------------------------------------------------------下一曲---------------------------------------

void next_song(GtkWidget *widget, gpointer data)
{
	MPLAYER *player = (MPLAYER *)data;//转
	int i =0;
	char song_name[128] ="";//用于接歌名
	strcpy(song_name, player->song_name);
	
	if (player->max == player->ii)//判断歌曲编号是否为最大max
	{
		i = 0;//改变当前歌曲编号
		sprintf(song_name, "loadfile %s\n", player->song_list[i]);//命令组包
	}
	else
	{
		i =  player->ii;//改变当前歌曲编号
		i++;//进行下一曲操作
		
		sprintf(song_name, "loadfile ../music/%s\n", player->song_list[i]);//命令组包
	}

	player->ii = i;//改变当前歌曲编号
	
	write(player->fd1, song_name, strlen(song_name));//写入命令至管道

}



//------------------------------------------------上一曲--------------------------------------------
void last_song(GtkWidget *widget, gpointer data)
{
	MPLAYER *player = (MPLAYER *)data;

	int i =0;
	
	char song_name[128] ="";
	strcpy(song_name, player->song_name);
	
	if (player->ii == 0)
	{
		i = player->max;
		//strcpy(song_name,player->song_list[i]);
		sprintf(song_name, "loadfile %s\n", player->song_list[i]);
	}
	else
	{
		i =  player->ii;
		i--;
		//strcpy(song_name,player->song_list[i]);
		sprintf(song_name, "loadfile ../music/%s\n", player->song_list[i]);

	}

	player->ii = i;
	if (player->signal_i == 0)
	{
		write(player->fd1, song_name, strlen(song_name));
	}
	


}

// ------------------------------------------歌词处理--------------------------------------------

gpointer the_lrc(gpointer arg)
{
	MPLAYER *player = (MPLAYER *)arg;
	LRC_PTR lrc;  
	LRC *head = NULL;


	while(1)
	{	

		if (player->signal_i == 0)
		{
			
			if (player->file_flag = 1)
			{
				sleep(1);
				head = dispose_lrc(player->buf, &lrc);
				
				if(head == NULL)//解析歌词失败
				{
					printf("No this lyrics\n");
				}
				else
				{
					int i;
					
					for(i=0;i<lrc.lrc_arry_size;i++)//时间是毫秒ms
					{
						
						if(lrc.lrc_arry[i]->time ==  player->lrc_time)
						{
							gdk_threads_enter();	// 进入多线程互斥区域
							gtk_label_set_text(GTK_LABEL(player->lable_song_lrc), lrc.lrc_arry[i]->lrc);
							gdk_threads_leave();	// 退出多线程互斥区域
							break;
						}
					
					}
				
				}
				player->file_flag =0;
				
			}

		}
		
		
	}
			
	free_lrc_arry(&lrc);//释放歌词解析(dispose_lrc)后的资源
	return;

}

int sungtk_widget_set_font_color(GtkWidget *widget, const char *color_buf, gboolean is_button)
{
	if(widget == NULL && color_buf==NULL)
		return -1;
	
	GdkColor color;
	GtkWidget *labelChild = NULL;
	sungtk_color_get(color_buf, &color);
	if(is_button == TRUE){
		labelChild = gtk_bin_get_child(GTK_BIN(widget));//取出GtkButton里的label  
		gtk_widget_modify_fg(labelChild, GTK_STATE_NORMAL, &color);
		gtk_widget_modify_fg(labelChild, GTK_STATE_SELECTED, &color);
		gtk_widget_modify_fg(labelChild, GTK_STATE_PRELIGHT, &color);
	}else{
		gtk_widget_modify_fg(widget, GTK_STATE_NORMAL, &color);
	}
	return 0;
}
int sungtk_color_get(const char *color_buf, GdkColor *color)
{
	gdk_color_parse(color_buf, color);
	return 0;
}

//----------------------------------------------------高亮显示------------------------------------
// int sungtk_clist_select_row(SunGtkCList *clist, gint row, const gchar *color_buf)
// {
// 	if(clist == NULL)
// 		return -1;
// 	int i = 0;
// 	CListLink *temp_head = clist->head;
// 	while(temp_head)
// 	{
// 		sungtk_clist_set_label_color(temp_head->label, clist->font_color);
// 		if(i==row){
// 			clist->select_row = i;
// 			sungtk_clist_set_label_color(temp_head->label, color_buf);
// 		}
// 		temp_head = temp_head->next;
// 		++i;
// 	}
// 	return i;
// }