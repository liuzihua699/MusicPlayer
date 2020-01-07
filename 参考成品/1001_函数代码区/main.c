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
void thread_init(void)
{
	if( FALSE == g_thread_supported() )	/*测试是否支持多线程*/
	{  
		g_thread_init(NULL); 	//始化多线程支持，g_thread_init()必须放在gdk_threads_init()前面
	}
	gdk_threads_init();	/*初始化GDK多线程支持*/
}




//-----------------------------------------主函数-----------------------------------------
int main(int argc, char *argv[])
{
	gtk_init (&argc, &argv);

	MPLAYER player;/*定义整个Mplayer结构体*/
	memset(&player, 0, sizeof(player));//初始化结构体
	thread_init();//线程初始化
	
	my_course(&player);/*启动Mplayer*/

	gtk_widget_show_all(player.main_window);
	gdk_threads_enter();	// 进入多线程互斥区域
	gtk_main();
	gdk_threads_leave();    // 退出多线程互斥区域
	
	return 0;
}

