#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <gtk/gtk.h>
enum
{
	COLUMN = 0,
	NUM_COLS
} ;
void createWindow(/*int *,char ****/);
void buttonpressed();
//int button_pressed(GtkWidget *, gpointer , char *);
void getMountedDevices(char []);
int getTotalBlocks();
int getMountedCount();
void getBlockSizeCurrentDisk();
void getBlockSizeUsb();
void RemoveSpaces(char*);
int main(int argc, char *argv[]){
	char mounted[4][500];
	int a;
	gtk_init(&argc, &argv);
	createWindow();
	return (0);
}

void getBadBlocks(){
	int pid;
	char badblock[500];
	FILE *bz;
	char size[1000];
	bz=popen("sudo dumpe2fs -b /dev/loop0","r");
	if(bz==NULL){
		printf("\n ERROR!");
	//return(1);
	}
	while(fgets(size,sizeof(size),bz)!=NULL){
	//printf("%s",size);
		strcpy(badblock,size);
	}
	pclose(bz);
	printf("LAL> %s \n",badblock);
}

int getTotalBlocks(){
	char blocks[500],size[1000];
	int totalblocks;
	FILE *bz;
	bz=popen("sudo dumpe2fs -h /dev/loop0 |grep -w Block|head - -n1|cut -d: -f2","r");
	if(bz==NULL){
		printf("\nERROR");
	}
	while(fgets(blocks,sizeof(blocks)-1,bz)!=NULL){
	//printf("%s",size);
		//strcpy(blocks,size);
		RemoveSpaces(blocks);
	}
	
	totalblocks = atoi (blocks);
	pclose(bz);
	return totalblocks;
}

void getBlocks(){


}

/**Funciones de la ventana**/
void button_pressed(GtkWidget *widget, gpointer window){
	gchar *str,*str1;
	FILE *size,*b,*r,*k,*bs;
	char ssize[200],used[200],popenstr[800],selDevice[200],mountPath[200],logicalblocksize[200],blocksize1[200],totalblc[200],totalbloc[200];
	strcpy(ssize," ");
	strcpy(used," ");
	strcpy(popenstr," ");
	strcpy(selDevice," ");
	strcpy(mountPath," ");
	sprintf(popenstr,"sudo df -h| grep %s -| cut  -b16-20 -",gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
	size=popen(popenstr,"r");
	if(size==NULL){
		printf("nope");
	}
	while(fgets(ssize,sizeof(ssize),size)!=NULL){
		RemoveSpaces(ssize);
	}
	sprintf(selDevice,"sudo df -h| grep %s -| cut  -b23-26 -",gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
	r=popen(selDevice,"r");
	if(r==NULL){
		printf("nope");
	}
	while(fgets(used,sizeof(used),r)!=NULL){
		RemoveSpaces(used);
	}
	if(strcmp(gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)),"/dev/loop0")==0){
		sprintf(logicalblocksize,"sudo dumpe2fs -h %s |grep -w Block|head - -n1|cut -d: -f2",gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
		sprintf(totalblc,"sudo dumpe2fs -h %s|grep count|head -n2 -|tail -n1|cut -d':' -f 2",gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
	}else{
		sprintf(logicalblocksize,"sudo fdisk -l %s|grep size|grep logical|cut -d':' -f2|cut -d'/' -f2",gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
		sprintf(totalblc,"sudo fdisk -l %s|grep total|cut -d',' -f4|cut -d' ' -f 3",gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)));
	}
	k=popen(logicalblocksize,"r");
	if(k==NULL){
	}
	while(fgets(blocksize1,sizeof(blocksize1),k)!=NULL){
		RemoveSpaces(blocksize1);
	}
	bs=popen(totalblc,"r");
	if(bs==NULL){
	}
	while(fgets(totalbloc,sizeof(totalbloc),bs)!=NULL){
		RemoveSpaces(totalbloc);
	}
	str = g_strdup_printf("Dispositivo: %s || Espacio total: %s || Espacio utilizado: %s|| Bloques totales:%s ||Block size: %s", 
		gtk_combo_box_get_active_text(GTK_COMBO_BOX(widget)),ssize,used,totalbloc,blocksize1);
	gtk_statusbar_push(GTK_STATUSBAR(window),
		gtk_statusbar_get_context_id(GTK_STATUSBAR(window), str), str);
	g_free(str);
	pclose(k);
}

GdkPixbuf *create_pixbuf(const gchar * filename){
	GdkPixbuf *pixbuf;
	GError *error = NULL;
	pixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if(!pixbuf) {
		fprintf(stderr, "%s\n", error->message);
		g_error_free(error);
	}
	return pixbuf;
}

void RemoveSpaces(char* source){
	char* i = source;
	char* j = source;
	while(*j != 0)
	{
		*i = *j++;
		if(!isspace(*i))
			i++;
	}
	*i = 0;
}

//crear y llenar el modelo para treeview
static GtkTreeModel * create_and_fill_model (char *device){
	FILE *mountpoint,*size,*barn;
	char devi[200],ssize[200],used[200],popenstr[200],bby[200],mountedIn[200],mountdirection[200];
	GtkTreeStore *treestore;
	GtkTreeIter toplevel, child;
	treestore = gtk_tree_store_new(NUM_COLS,G_TYPE_STRING);
	gtk_tree_store_append(treestore, &toplevel, NULL);
	strcpy(devi," ");
	strcpy(ssize," ");
	strcpy(used," ");
	strcpy(popenstr," ");
	strcpy(bby," ");
	strcpy(mountedIn," ");
	strcpy(mountdirection," ");
	sprintf(mountdirection,"grep %s /proc/mounts | cut -d ' ' -f 2",device); 
     mountpoint=popen(mountdirection,"r");//regresa el folder donde esta montado el dispositivo
     if(mountpoint==NULL){
     	printf("nope");
     }
     fgets(mountedIn,sizeof(mountedIn),mountpoint);
     RemoveSpaces(mountedIn);
     sprintf(devi,"find %s -maxdepth 1 -type d",mountedIn);//cambio al directorio donde esta montado el dispositivo y busca solo los folders
     size=popen(devi,"r");
     if(size==NULL){
     	printf("nope");
     }
     while(fgets(used,sizeof(used),size)!=NULL){
     	RemoveSpaces(used);
     	gtk_tree_store_append(treestore, &toplevel, NULL);
		gtk_tree_store_set(treestore, &toplevel,COLUMN, used,-1);//pone los folders como primer nivel
		RemoveSpaces(used);
		sprintf(popenstr,"find %s -maxdepth 1 -type f",used);
		barn=popen(popenstr,"r");
		if(barn==NULL){
			printf("nope");
		}
		while(fgets(bby,sizeof(bby),barn)!=NULL){
			RemoveSpaces(bby);
			gtk_tree_store_append(treestore, &child, &toplevel);
			gtk_tree_store_set(treestore, &child,COLUMN, bby,-1);//pone el contenido de cada folder 
		}
	}
	return GTK_TREE_MODEL(treestore);
}

static GtkWidget * create_view_and_model (char *selDevice){
	GtkTreeViewColumn *col;
	GtkCellRenderer *renderer;
	GtkWidget *view;
	GtkTreeModel *model;
	view = gtk_tree_view_new();
	col = gtk_tree_view_column_new();
	gtk_tree_view_column_set_title(col, "Lista de archivos");
	gtk_tree_view_append_column(GTK_TREE_VIEW(view), col);
	renderer = gtk_cell_renderer_text_new();
	gtk_tree_view_column_pack_start(col, renderer, TRUE);
	gtk_tree_view_column_add_attribute(col, renderer, "text", COLUMN);
	model = create_and_fill_model(selDevice);
	gtk_tree_view_set_model(GTK_TREE_VIEW(view), model);
	g_object_unref(model); 
	return view;
}



/**Creamos la ventana!**/
void createWindow(){
	FILE *bz,*b,*op;
	int a,blocksize,r,s,k;
	int h=0;
	gchar *str1;
	GtkWidget *window,*fixed,*combo,*label, *button1,*buttont,*statusbar,*table,*vbox,*view,*scwin,*scwin2,*hbox,*fix;
	GtkTreeSelection *selection;
	char selDevice[200],mountPath[200],path[200],*devv,yoo[200],size[1000],mounted,buttonlabel[50],mpth[200];
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window),"Proyecto Final Sistemas Operativos");
	gtk_window_set_default_size(GTK_WINDOW(window),640,480);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	gtk_window_set_icon(GTK_WINDOW(window), create_pixbuf("mameshiba.png"));
	hbox=gtk_hbox_new(TRUE,2);
	vbox = gtk_vbox_new(FALSE, 2);//statusbar
	fixed = gtk_fixed_new();
	fix=gtk_fixed_new();
	combo = gtk_combo_box_new_text();
	gtk_container_add(GTK_CONTAINER(window), vbox);
	gtk_box_pack_start(GTK_BOX(vbox), fixed, TRUE, TRUE, 1);
	strcpy(mpth," ");
	/*poner los devices en combo box*/;
	bz=popen("sudo df -h|grep /dev/ -|grep -v Disk -|cut -d' ' -f1 -","r");
	if(bz==NULL){
		printf("\n ERROR!");
	}
	while(fgets(size,sizeof(size),bz)!=NULL){
		RemoveSpaces(size);
		gtk_combo_box_append_text(GTK_COMBO_BOX(combo), size);
	}
	pclose(bz);
	/*Termina de poner los devices en el combobox*/

    gtk_combo_box_set_active(GTK_COMBO_BOX(combo),2);  //Hace el primer label del combobox el elemento activo
	gtk_fixed_put(GTK_FIXED(fixed), combo, 20, 50);//posicion de combobox
	gtk_widget_set_size_request (combo, 120, 30);//sets the size of the combobox!
	gtk_container_add(GTK_CONTAINER(window), fixed);
	label = gtk_label_new("Dispositivos disponibles");//hace un label nuevo
	gtk_fixed_put(GTK_FIXED(fixed), label, 20, 35);//posicion del label
	scwin = gtk_scrolled_window_new(NULL, NULL);
	scwin2=gtk_scrolled_window_new(NULL,NULL);
	table = gtk_table_new (10, 12, TRUE);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scwin2),table);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scwin2),GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);
	statusbar = gtk_statusbar_new();
	str1=gtk_combo_box_get_active_text(GTK_COMBO_BOX(combo));
	printf("\n%s",str1);
	sprintf(mountPath,"df|grep %s|cut -b51-105 -",str1);
	op=popen(mountPath,"r");
	g_free(str1);
	if(op==NULL){
		printf("\n ERROR!");
	}
	while(fgets(mpth,sizeof(mpth),op)!=NULL){
		RemoveSpaces(mpth);
		printf("%s",mpth);
		printf("\n%s",mpth);
	}
	view = create_view_and_model(mpth);
	gtk_container_add(GTK_CONTAINER(scwin), view);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scwin), GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
	gtk_box_pack_start(GTK_BOX(vbox), scwin2, FALSE, TRUE, 1);   
	for(r=0;r<10;r++){
		for(s=0;s<12;s++){
			sprintf(buttonlabel,"%d",h);
			buttont=gtk_toggle_button_new_with_label(buttonlabel);
			gtk_table_attach_defaults(GTK_TABLE(table),buttont,r,r+1,s,s+1);
			gtk_widget_show(buttont);
			h++;
		}
	}
	g_signal_connect_swapped(G_OBJECT(window), "destroy",G_CALLBACK(gtk_main_quit), G_OBJECT(window));
	g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(button_pressed), G_OBJECT(statusbar));
	gtk_widget_show(scwin2);
	gtk_widget_show(table);
	
	gtk_box_pack_start(GTK_BOX(vbox), scwin, TRUE, TRUE, 1);
	gtk_box_pack_start(GTK_BOX(vbox), statusbar, FALSE, TRUE, 1);
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(view));
	gtk_widget_show_all(window);
	gtk_main();
}