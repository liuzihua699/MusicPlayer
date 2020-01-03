#include<stdio.h>
#include <stdio.h>
#include <gtk/gtk.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>

GtkWidget *makeTextEntry();
GtkWidget *vistwindow();
GtkWidget *firstTextEntry();
GtkWidget *VisitTable();
GtkWidget *makeButtonBox();
int       make_dialog ();
int       readbuf();
int       clear ();
GtkWidget   *window;

GtkWidget   *first;
GtkWidget   *last;
gchar       buf1[1024];
gchar       buf2[1024];
GtkWidget   *dialog;
char        reve_buf[1024];


void gtk_widget_hide(GtkWidget *window);


/*当系统回调时，关闭窗口，自动调用*/
void destroy (GtkWidget *widget, gpointer *data)
{
    gtk_main_quit();
}

int  click_button1 (GtkWidget *widget,gpointer*data)
{
    char *str = "";
    clear ();
    if (strcmp(buf1, str) == 0 || strcmp(buf2, str) == 0) {
        strcpy (reve_buf,"不能查询为空的站点");
        make_dialog ();
    }
    if  (strcmp (buf1,str)!=0 && strcmp (buf2,str)!=0) {
        strcpy (reve_buf, "hello world");
        make_dialog ();
    }
}

int clear ()
{
    strcpy (buf1, gtk_entry_get_text (GTK_ENTRY (first)));
    strcpy (buf2, gtk_entry_get_text(GTK_ENTRY(last)));
}

int  click_button2 (GtkWidget *widget, gpointer *data)
{
    gtk_entry_set_text (GTK_ENTRY (first), "");
    gtk_entry_set_text (GTK_ENTRY (last), "");
    memset (buf1, 0, sizeof (buf1));
    memset (buf2, 0, sizeof (buf2));
    memset (reve_buf, 0, sizeof (reve_buf));
    gtk_widget_hide (dialog);
}
int  click_button3 (GtkWidget *widget,gpointer *data)
{
    exit (0);
}

int   make_dialog ()
{
    GtkWidget     *label;
    GtkWidget     *button;
    GtkWidget     *hbox;
    GtkWidget     *vbox;
    GtkWidget     *table;
    char          string[1024];
    GtkWidget     *image;

    dialog = gtk_dialog_new ();
    vbox = GTK_DIALOG (dialog)->vbox;


    image = gtk_image_new_from_file ("gtk1.png");
    gtk_box_pack_start (GTK_BOX (vbox),image,TRUE,TRUE,0);
    gtk_table_attach (GTK_BOX(vbox),image,0,1,0,1,GTK_FILL|GTK_EXPAND|GTK_SHRINK,0,0,0);
    gtk_widget_show_all (image);

    label= gtk_label_new (reve_buf);
    gtk_table_attach (GTK_BOX(vbox),label,0,1,0,1,GTK_FILL|GTK_EXPAND|GTK_SHRINK,0,0,0);


    gtk_box_pack_start(GTK_BOX (vbox),label,TRUE,TRUE,0);
    gtk_widget_show_all (label);
    vbox = GTK_DIALOG (dialog) ->action_area;
    button = gtk_button_new_with_label ("yes");
    gtk_signal_connect (GTK_OBJECT (button),"clicked",GTK_SIGNAL_FUNC (click_button2),"yes");
    gtk_box_pack_start (GTK_BOX (vbox),button,FALSE,FALSE,0);

    gtk_widget_show_all (dialog);
    return 0;
}

int  visitwindow(int *argc,char **argv)
{
    GtkWidget    *Vtable;

    gtk_init (argc,&argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (window), "上海铁路查询系统");
    gtk_window_set_default_size (GTK_WINDOW(window),587,468);
    gtk_window_set_opacity (GTK_WINDOW(window),100);//设置透明度函数
    g_signal_connect (GTK_OBJECT (window),"destroy",GTK_SIGNAL_FUNC(destroy),NULL);

    Vtable = VisitTable();
    gtk_container_add (GTK_CONTAINER(window),Vtable);
    gtk_widget_show_all (window);
    gtk_main ();
}


GtkWidget * VisitTable ()
{
    GtkWidget        *Vtable;
    GtkWidget        *image;
    GtkWidget        *checkButtons;
    GtkWidget        *textEntry;
    GtkWidget        *buttonBox;

    Vtable = gtk_table_new (3,3,FALSE);
    gtk_widget_show_all (Vtable);
    image = gtk_image_new_from_file ("gtk.png");
    gtk_box_pack_start (GTK_BOX (Vtable),image,TRUE,TRUE,0);
    gtk_table_attach (GTK_TABLE(Vtable),image,0,1,0,1,GTK_FILL|GTK_EXPAND|GTK_SHRINK,0,0,0);
    gtk_widget_show_all (image);

    gtk_window_set_decorated(GTK_WINDOW (Vtable),FALSE);//设置无边框

    textEntry =firstTextEntry();
    gtk_table_attach (GTK_TABLE(Vtable),textEntry,0,1,0,1,GTK_FILL|GTK_EXPAND|GTK_SHRINK,0,0,0);
    gtk_widget_show (textEntry);
    buttonBox = makeButtonBox();

    gtk_table_attach (GTK_TABLE(Vtable),buttonBox,0,1,1,2,GTK_EXPAND |GTK_FILL|GTK_SHRINK,0,5,10);
    gtk_widget_show (buttonBox);
    gtk_widget_show_all (Vtable);
    return Vtable;
}


GtkWidget *firstTextEntry ()
{
    GtkWidget   *label1,*label ,*label2;
    GtkWidget   *table;

    table = gtk_table_new (2,2,FALSE);
    first= gtk_entry_new_with_max_length (30);
    gtk_label_set_justify(GTK_LABEL(first),GTK_JUSTIFY_CENTER);
    gtk_entry_set_text (GTK_ENTRY (first), "");
    gtk_table_attach (GTK_TABLE(table),first,0,1,0,1,(GtkAttachOptions)(GTK_FILL),(GtkAttachOptions)(0),0,0);
    gtk_widget_show (table);
    gtk_widget_show (first);
    last = gtk_entry_new_with_max_length (30);
    gtk_label_set_justify(GTK_LABEL(last),GTK_JUSTIFY_CENTER);
    gtk_entry_set_text (GTK_ENTRY (last), "");
    gtk_table_attach (GTK_TABLE(table),last,0,1,1,2,(GtkAttachOptions)(GTK_FILL),(GtkAttachOptions)(0),0,0);
    gtk_widget_show (last);
    return  table;
}

GtkWidget *makeButtonBox()
{
    GtkWidget   *hbox;
    GtkWidget   *button;

    hbox = gtk_hbox_new (FALSE,0);

    button = gtk_button_new_with_label ("确定");
    gtk_signal_connect (GTK_OBJECT (button),"clicked",GTK_SIGNAL_FUNC (click_button1),"确定");
    gtk_box_pack_start (GTK_BOX (hbox),button,TRUE,TRUE,10);
    gtk_widget_show (button);

    button = gtk_button_new_with_label ("退出");
    gtk_signal_connect(GTK_OBJECT(button),"clicked",GTK_SIGNAL_FUNC (click_button3),"退出");
    gtk_box_pack_start (GTK_BOX (hbox),button,TRUE,TRUE,40);
    gtk_widget_show (button);
    return hbox;
}

int main (int argc,char *argv[])
{
    visitwindow (&argc,argv);
    return  0;
}