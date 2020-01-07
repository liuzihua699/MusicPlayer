#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>


//将控制指令送到管道中
void send_command(char *cmd);
//按键处理
void deal_clicked(GtkButton *button, gpointer data);
//设置控件字体大小（其实就label）
void set_widget_font_size(GtkWidget *widget, int size);
//设置背景图片
void change_background(GtkWidget *widget,int w, int h, const gchar *path);
//关闭窗口
void close_window(GtkWidget *widget, GdkEvent *event, gpointer data);

//读取歌曲
char *MusicList[10]={NULL};
static int listLength = 0;
//命名管道指示
int nPipe = 0; 
//打开管道
int isOpen = 0;  //open fifo with write
//int rOpen;   //open fifo with read
int fd[2];
int num = 0;
char *songInfo;
char song[100];
pid_t pid;
char cmd[50] ={"\0"};
int vol = 0;
//static char *path = "./songs/";


int main(int argc, char *argv[])
{
	gtk_init(&argc, &argv); //before kj define
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	GtkWidget *Player = gtk_fixed_new();
	GtkWidget *labShowMusicName = gtk_label_new("IdShowMusicName");
	set_widget_font_size(labShowMusicName,22);
	//gtk_widget_set_size_request(labShowMusicName, 550, 100);
	char *btnVal[8] = {"<<上一首","暂 停","下一首>>","|<<快退","快进>>|","音 量+","音 量-","静 音"};

	gtk_container_add(GTK_CONTAINER(window),Player);
	//gtk_label_set_text(GTK_LABEL(labShowMusicName),"this is my label");
	gtk_fixed_put(GTK_FIXED(Player),labShowMusicName,315,160);

	//设置颜色
	GdkColor color;
	//color.red = 0xffff;
	gdk_color_parse ("white", &color);
	gtk_widget_modify_fg(labShowMusicName, GTK_STATE_NORMAL, &color);

	gtk_window_set_title(GTK_WINDOW(window),"Wu Chenyang");
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_container_set_border_width(GTK_CONTAINER(window),10); //set border width for border
  	gtk_widget_set_size_request(window, 950, 750);
	gtk_window_set_resizable(GTK_WINDOW(window),FALSE);
	g_signal_connect(window,"destroy", G_CALLBACK(close_window),NULL);  //exit application

  	change_background(window,950,750,"background.jpg");

  //GtkWidget *image = gtk_image_new_from_file("background.jpg");
  //gtk_container_add(GTK_CONTAINER(window),image);

  	int i;
	int pos = 0;
	for(i = 0; i < 8; i++)
	{	
		GtkWidget *button = gtk_button_new_with_label(btnVal[pos]);
	  	gtk_fixed_put(GTK_FIXED(Player),button,(i+1)*93,7*95);
		gtk_widget_set_size_request(button, 78, 33);
		//set_widget_font_size(button,5);
		g_signal_connect(button,"clicked",G_CALLBACK(deal_clicked),labShowMusicName);
		pos++;
	}


 	DIR *dir;
 	struct dirent *dirt;
 	int count = 0;
 	if((dir=opendir("./songs"))==NULL)
 	{
		printf("open songs dir failed!\n");
		exit(-1);
 	}
 	while((dirt=readdir(dir))!=NULL)
 	{
 		if(dirt->d_type == DT_REG)
 		{
	 		MusicList[count]=(char*)malloc(sizeof(dirt->d_name)+1);
			strcpy(MusicList[count],dirt->d_name);
			printf("%s\n",MusicList[count]);
			count++;
 		}
 		else
 		{
    		continue;
 		}
 	}
  printf("%d\n",count);

  listLength = count;

  int c;
  for(c = 0; c < listLength; c++)
  {
  	printf("%s\n",MusicList[c]);
  }
  closedir(dir);

	gtk_label_set_text(GTK_LABEL(labShowMusicName),MusicList[0]);
	nPipe = mkfifo("/tmp/toPlay",0644); //- rw r r  创建命名管道
  printf("Successful Create toPLay\n");

	if(nPipe < 0 && errno != EEXIST)   //创建命名管道failed  //可以判断是否存在
	{
		perror("fifo error\n");
		//unlink("/tmp/toPlay");
		//nPipe = mkfifo("/tmp/toPlay",0644);
		exit(-1);
	}

	if(pipe(fd)==-1)   //fd[0] read fd[1] write
	{
		perror("pipe error");
		pipe(fd);
	}

	pid = fork();

	//printf("%d\n",pid);

	if(pid > 0)  //father process
	{
		isOpen = open("/tmp/toPlay",O_WRONLY);
		if(isOpen < 0)
		{
			perror("open error!\n");
			exit(-1);
		}

		close(fd[1]);  //关闭写端
		//dup2(fd[0],STDOUT_FILENO);
		write(fd[0],songInfo,sizeof(songInfo)); //将子进程写到管道中的数据读出来 放到char *中
		close(fd[0]);
		//printf("%d",getpid());
		printf("output is %s\n",songInfo);

		gtk_widget_show_all(window);  //show windo
		gtk_main(); //main event loop
		return 0;
	}
	else if(pid == 0)  //child
	{
		//close(fd[0]);   //子进程关闭读端
		dup2(fd[1],1); //重定向mplayer标准输出到写端 STDOUT_FILENO

		sprintf(song,"%s%s","./songs/",MusicList[0]);

		//close(fd[1]);
		
		//char *path = "./songs/"+MusicList[0];
		strcat(cmd,"volume %d 1\n",vol);
		send_command(cmd);
		execlp("mplayer", "mplayer", "-slave", "-quiet","-idle","-input", "file=/tmp/toPlay",song,NULL);
	}
	else
	{
		perror("failed");
		exit(-1);
	}
}

//将控制指令送到管道中
void send_command(char *cmd)
{
	/*>0,表示写入了多少字节 =0,没有写入字节 =-1,出错了*/
	int r = write(isOpen,cmd,strlen(cmd));
	if(r != strlen(cmd))   //没成功写入
	{
		perror("write error");
	}
	else
	{
		printf("write Successful: %s\n",cmd);
	}
}

//按键处理
void deal_clicked(GtkButton *button, gpointer data)
{
	const gchar *btnVal = gtk_button_get_label(button);
	g_print("button_text = %s\n",btnVal);

	if(strcmp(btnVal,"<<上一首") == 0)   // num: index of musiclist elements
	{
	 	num--;
	 	if(num == -1)
		 	num = listLength - 1;

	 	g_print("MusicList[%d]:%s\n",num,MusicList[num]);
		gtk_label_set_text(GTK_LABEL(data),MusicList[num]);
		sprintf(song,"%s%s","./songs/",MusicList[num]);
		sprintf(cmd,"%s%s\n","loadfile ",song);
		send_command(cmd);
	}
	else if(strcmp(btnVal,"暂 停") == 0)   //
 	{
 		gtk_button_set_label(button,"播 放"); 
		send_command("pause\n");
 	}
 	else if(strcmp(btnVal,"播 放") == 0)   //
 	{
 		gtk_button_set_label(button,"暂 停");
		send_command("pause\n");
	}
 	else if(strcmp(btnVal,"下一首>>") == 0)   //
 	{
	 	num++;
 		if(num==listLength)
 			num = 0;

 		g_print("MusicList[%d]:%s\n",num,MusicList[num]);
	 	gtk_label_set_text(GTK_LABEL(data),MusicList[num]);
	  sprintf(song,"%s%s","./songs/",MusicList[num]);	
	  sprintf(cmd,"%s%s\n","loadfile ",song);
		send_command(cmd);
  } 
	else if(strcmp(btnVal,"|<<快退") == 0)   //
	{
		send_command("seek -10\n");
	}
	else if(strcmp(btnVal,"快进>>|") == 0)   //
	{
	 		//printf("%d\n",listLength);
		send_command("seek 10\n");
	}
	else if(strcmp(btnVal,"音 量+") == 0)   //
	{
		vol += 10;
		if(vol > 100)
		{
			printf("音量已经达到最大!");
			vol -= 10;
		}
		sprintf(cmd,"volume %d 1\n",vol);
		send_command(cmd);

	}
	else if(strcmp(btnVal,"音 量-") == 0)   //
	{
		vol -= 10;
		if(vol < 0)
		{
			printf("音量已经达到最小!");
			vol += 10;
		}
		sprintf(cmd,"volume %d 1\n",vol);
		send_command(cmd);
	}
	else if(strcmp(btnVal,"静 音") == 0)   //
	{
	 	gtk_button_set_label(button,"非静音"); 
	 	send_command("mute 1\n");
	}
	else if(strcmp(btnVal,"非静音") == 0)   //
	{
	 	gtk_button_set_label(button,"静 音"); 
	 	send_command("mute 0\n");
	}
	//g_print("button_text = %s; user_data=%s\n", text, (gchar*)data);
}

//设置控件字体大小（其实就label）
void set_widget_font_size(GtkWidget *widget, int size)
{
	PangoFontDescription *font;  	// 字体指针
	font = pango_font_description_from_string("Sans"); //参数为字体名字，任意即可
	// #define PANGO_SCALE 1024
	pango_font_description_set_size(font, size*PANGO_SCALE);// 设置字体大小
	gtk_widget_modify_font(widget, font);  // 改变label的字体大小
	pango_font_description_free(font);	  // 释放字体指针分配的空间
}

//设置背景图片
void change_background(GtkWidget *widget,int w, int h, const gchar *path)
{
	gtk_widget_set_app_paintable(widget,TRUE); //allow paint
	gtk_widget_realize(widget);

	gtk_widget_queue_draw(widget);

	//resource object
	GdkPixbuf *src_pixbuf = gdk_pixbuf_new_from_file(path,NULL);

	GdkPixbuf *dst_pixbuf = gdk_pixbuf_scale_simple(src_pixbuf,w,h,GDK_INTERP_BILINEAR);

	GdkPixmap *pixmap = NULL;

	gdk_pixbuf_render_pixmap_and_mask(dst_pixbuf,&pixmap,NULL,128);

	gdk_window_set_back_pixmap(widget->window,pixmap,FALSE);

	g_object_unref(src_pixbuf);
	g_object_unref(dst_pixbuf);
	g_object_unref(pixmap);
}

//关闭窗口
void close_window(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	//send_command("stop");
	send_command("quit\n");
	//删除管道
	unlink("/tmp/toPlay");
	gtk_main_quit();
}



