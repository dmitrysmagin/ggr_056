class map
 {
  private:
   int scroll_stop_x,scroll_stop_y;
   int num_col,num_row;
   int tile_h,tile_w;
   int num_pages_x,num_pages_y;
   int tilemax_x,tilemax_y;
   int max_bound_y;
   int *ctrl_x,*ctrl_y;
   int scroll_x,scroll_y,scroll_dx,scroll_dy;
   int tilepos_x,tilepos_y;
   int speed_x,speed_y;
   bool main_scroll,hscroll_on,vscroll_on;
   BITMAP *tile[32*32];
   BITMAP *out;
   mobile **mobilep;
   int data[300][300];
   int platform[300][300];
   int platformdata[100][32];
   int stairs[300][300];
   int wall[300][300];
   int walldata[50][2];
   int n_plat;

   void setOutBitmap(BITMAP *);

  public:
   map(char *,char *,BITMAP *,bool mainscroll=false,char *plat_filename="",char *dat_plat_filename="",char *stairs_filename="",char *wall_filename="",char *dat_wall_filename="",bool loadall=false);
   ~map();
   void drawMap();
   void scrollMap();
   void loadMap(char *,char *);
   void loadPlatform(char *,char *);
   void loadStairs(char *);
   void loadWall(char *,char *);
   void setMobileP(mobile **p,int);
   void setHScroll(bool,int ss=0);
   void setVScroll(bool,int ss=0);
   void setSpeedX(int);
   void setSpeedY(int);
   void setCtrlXP(int *p);
   void setCtrlYP(int *p);
   void setScrollX(int);
   void setScrollY(int);
   int getTileW();
   int getTileH();
   int getNumCol();
   int getNumRow();
   int getTotCol();
   int getTotRow();
   int getScrollX();
   int getActTileX();
   int getTileOffsetX();
   int getScrollY();
   int getActTileY();
   int getTileOffsetY();
   int getActPlatform(int,int);
   int getActPlatLayer(int,int,int,bool);
   int getActPlatLayerValue(int,int,int,int);
   int getActPlatformMobile(int,int);
   int getActWall(bool,int,int);
   int getActWallMobile(bool,int,int);
   int getActStair(int,int);
   int getActPlatformEnemies(int,int,int type=0,bool side=0);
   void debug();
 };

map::map(char *bmp_filename,char *map_filename, BITMAP *out,bool mainscroll,char *plat_filename,char *dat_plat_filename,char *stairs_filename,char *wall_filename,char *dat_wall_filename,bool loadall)
 {
  main_scroll=mainscroll;
  loadMap(bmp_filename,map_filename);
  if(loadall)
   {
   	loadPlatform(plat_filename,dat_plat_filename);
   	loadStairs(stairs_filename);
   	loadWall(wall_filename,dat_wall_filename);
   }
  setOutBitmap(out);
  scroll_stop_x=0; scroll_stop_y=0;
  hscroll_on=1; vscroll_on=1;
  scroll_x=0; scroll_y=0;
 }

map::~map()
 {
  for(int i=0;i<32*32;i++) free(tile[i]);
 }

void map::loadMap(char *bmp_filename,char *map_filename)
 {
  int v,max;
  FILE *fd;
  BITMAP *aux;

  // Legge il file .map
  fd=fopen(map_filename,"r");
  fscanf(fd,"%d",&num_col);     fscanf(fd,"%d",&num_row);     // Dimensione schermata in numero di tile
  fscanf(fd,"%d",&tile_h);      fscanf(fd,"%d",&tile_w);      // Dimensione singolo tile (altezza,larghezza)
  fscanf(fd,"%d",&num_pages_x); fscanf(fd,"%d",&num_pages_y); // Numero totale di schermate in orizzontale e verticale
  fscanf(fd,"%d",&tilemax_x);   fscanf(fd,"%d",&tilemax_y);   // Numero totale di tile in orizzontale e verticale
  max_bound_y=num_row*tile_h*num_pages_y+328;
  //data=(int **)malloc(sizeof(int *)*tilemax_x);
  //for(int x=0;x<tilemax_x;x++) data[x]=(int *)malloc(sizeof(int)*tilemax_y);
  for(int y=0;y<tilemax_y;y++)
   for(int x=0;x<tilemax_x;x++)
    {
     fscanf(fd,"%x",&v);
     data[x][y]=v;
    }
  fclose(fd);

  // Take the maximum number of tiles used in the map
  max=data[0][0];
  for(int y=0;y<tilemax_y;y++)
   for(int x=0;x<tilemax_x;x++)
    if(data[x][y]>max) max = data[x][y];

  // Legge il file.bmp
  aux=load_bmp(bmp_filename,NULL); 
  for(int j=0;j<32;j++)
   for(int i=0;i<32;i++)
    {
     if(j*32+i<=max) // Allocate only used tile
      {
       tile[j*32+i]=create_bitmap(32,32);
       blit(aux,tile[j*32+i],32*i+i,33*j,0,0,tile[j*32+i]->w,tile[j*32+i]->h);
      }
    }
  destroy_bitmap(aux);
 }

void map::loadPlatform(char *plat_filename,char *bmp_plat_filename)
 {
  int v;
  FILE *fd;

  // Legge il file .map con i riferimeti alle piattaforme
  fd=fopen(plat_filename,"r");
  //data=(int **)malloc(sizeof(int *)*tilemax_x);
  //for(int x=0;x<tilemax_x;x++) data[x]=(int *)malloc(sizeof(int)*tilemax_y);
  for(int y=0;y<tilemax_y;y++)
   for(int x=0;x<tilemax_x;x++)
    {
     fscanf(fd,"%d",&v);
     platform[x][y]=v;
    }
  fclose(fd);

  // Legge il file .dat con le definizioni delle singole piattaforme
  int n;
  fd=fopen(bmp_plat_filename,"r");
  fscanf(fd,"%d",&n); // Numero di piattaforme definite (righe nel file)
  //data=(int **)malloc(sizeof(int *)*tilemax_x);
  //for(int x=0;x<tilemax_x;x++) data[x]=(int *)malloc(sizeof(int)*tilemax_y);
  for(int x=0;x<n;x++)
   for(int y=0;y<tile_w;y++)
    {
     fscanf(fd,"%d",&v);
     platformdata[x][y]=v;
    }
  fclose(fd);
 }

void map::loadStairs(char *stairs_filename)
 {
 	int v;
 	FILE *fd;

  // Legge il file .map con i riferimeti alle stairs
  fd=fopen(stairs_filename,"r");
 	//data=(int **)malloc(sizeof(int *)*tilemax_x);
 	//for(int x=0;x<tilemax_x;x++) data[x]=(int *)malloc(sizeof(int)*tilemax_y);
 	for(int y=0;y<tilemax_y;y++)
 	 for(int x=0;x<tilemax_x;x++)
 	  {
 	   fscanf(fd,"%d",&v);
 	   stairs[x][y]=v;
 	  }
 	fclose(fd);
 }

void map::loadWall(char *wall_filename,char *dat_wall_filename)
 {
 	int v;
 	FILE *fd;

  // Legge il file .map con i riferimeti ai muri
  fd=fopen(wall_filename,"r");
 	//data=(int **)malloc(sizeof(int *)*tilemax_x);
 	//for(int x=0;x<tilemax_x;x++) data[x]=(int *)malloc(sizeof(int)*tilemax_y);
 	for(int y=0;y<tilemax_y;y++)
 	 for(int x=0;x<tilemax_x;x++)
 	  {
 	   fscanf(fd,"%d",&v);
 	   wall[x][y]=v;
 	  }
 	fclose(fd);

  // Legge il file .dat con le definizioni dei singoli muri (posizione orizzontale e altezza)
  int n;
  fd=fopen(dat_wall_filename,"r");
  fscanf(fd,"%d",&n); // Numero di muri definiti (righe nel file)
 	//data=(int **)malloc(sizeof(int *)*tilemax_x);
 	//for(int x=0;x<tilemax_x;x++) data[x]=(int *)malloc(sizeof(int)*tilemax_y);
 	for(int x=0;x<n;x++)
 	 for(int y=0;y<2;y++)
 	  {
 	   fscanf(fd,"%d",&v);
 	   walldata[x][y]=v;
 	  }
 	fclose(fd);
 }

void map::setOutBitmap(BITMAP *bmp) { out=bmp; }

void map::drawMap()
 {
 	for(int x=0;x<=num_col;x++)
 	 for(int y=0;y<=num_row;y++)
 	  {
 	   //blit(getTile(y+tilepos_y,x),out,0,0,x*tdim_x,(max_ty-1-y)*tdim_y+scroll_dy,scr_x,scr_y);
 	   //draw_sprite(out,tile[data[x+tilepos_x][num_row-1-y]],x*tile_w-scroll_dx,(num_row-1-y)*tile_h);
 	   
 	   draw_sprite(out,tile[data[x+tilepos_x][y+tilepos_y]],x*tile_w-scroll_dx,y*tile_h-scroll_dy);
 	   
 	   //textprintf_ex(out,font,0+x*25,0+y*25,makecol32(255,0,0),-1,"%d",data[x][y]);
 	   //textprintf_ex(out,font,0+x*25,0+y*25,makecol32(255,0,0),-1,"%d",data[x+tilepos_x][num_row-1-y]);
 	  }
 	//scrollMap();
 }

void map::scrollMap()
 {
  // Horizontal scroll
  if(!hscroll_on) scroll_x=scroll_stop_x;
  else scroll_x=*ctrl_x/speed_x;
 	scroll_dx=scroll_x%tile_w; tilepos_x=scroll_x/tile_w;
  // Vertical scroll
  if(!vscroll_on) scroll_y=scroll_stop_y;
  else scroll_y=*ctrl_y/speed_y;
 	scroll_dy=scroll_y%tile_h; tilepos_y=scroll_y/tile_h;
 }

void map::setHScroll(bool v,int ss) { hscroll_on=v; scroll_stop_x=ss; }

void map::setVScroll(bool v,int ss) { vscroll_on=v; scroll_stop_y=ss; }
 	 
void map::setSpeedX(int sp) { if(sp==1) speed_x=2; else speed_x=1; }

void map::setSpeedY(int sp) { if(sp==1) speed_y=2; else speed_y=1; }

void map::setMobileP(mobile **p, int n) { mobilep=p; n_plat=n; }

void map::setCtrlXP(int *p) { ctrl_x=p; }

void map::setCtrlYP(int *p) { ctrl_y=p; }

void map::setScrollX(int sx) { scroll_x=sx; }

void map::setScrollY(int sy) { scroll_x=sy; }

int map::getTileW() { return tile_w; }

int map::getTileH() { return tile_h; }

int map::getNumCol() { return num_col; }

int map::getNumRow() { return num_row; }

int map::getTotCol() { return num_col*num_pages_x; }

int map::getTotRow() { return num_row*num_pages_y; }

int map::getScrollX() { return scroll_x; }

int map::getActTileX() { int x=*ctrl_x; return (scroll_x+x)/tile_w; }

int map::getTileOffsetX() { int x=*ctrl_x; return (scroll_x+x)%tile_w; }

int map::getScrollY() { return scroll_y; }

int map::getActTileY() { int y=*ctrl_y; return (scroll_y+y)/tile_h; /*return y/tile_h;*/ }

int map::getTileOffsetY() { int y=*ctrl_y; return (scroll_y+y)%tile_h; /*return y%tile_h;*/ }

int map::getActPlatform(int x,int y)
 {
 	int v,val;
 	bool fl=0;

  if(y/tile_h<0) return max_bound_y;
	for(int i=0;i<num_row*num_pages_y;i++)
 	 {
 	 	v=platform[x/tile_w][i];
 	  if(v!=-1 && v!=-2)
 	   {
 	 	  fl=1;	 	  
 	 	  val=i*32-platformdata[v][x%tile_w];
 	 	  if(y<=val+8) return val;
 	   }
 	 }
 	return max_bound_y;
 }

int map::getActPlatLayerValue(int x,int y,int lay,int alt)
 {
  int n,v,val;

  n=0;
  if(y/tile_h<0) return max_bound_y;
  for(int i=num_row*num_pages_y-1;i>=0;i--)
   {
   	v=platform[x/tile_w][i];
    if(v!=-1)
     {
      n++;
 	  if(n==lay)
 	   {
 	   	if(v==-2) return max_bound_y;
 	 	val=i*32-platformdata[v][x%tile_w];
 	 	if(y<=val+alt && y>val+10) return n; else return max_bound_y;
   	   }
 	 }
   }
  return max_bound_y;
 }

int map::getActPlatLayer(int x,int y,int ll,bool dis)
 {
  int n,v,val;

  n=0;
  if(y/tile_h<0) return 0;
	for(int i=num_row*num_pages_y-1;i>=0;i--)
 	 {
 	 	v=platform[x/tile_w][i];
 	  if(v!=-1)
 	   {
 	   	n++;
 	   	if(dis) if(n>ll) return ll; // disabilita i layer maggiori di ll (ll=1 abilita solo layer 1) (variarlo a secondo dello stage)
 	   	if(v==-2) val=max_bound_y;
 	   	else val=i*32-platformdata[v][x%tile_w];
 	 	  if(y>=val-38) return n;
 	   }
 	 }
  return 0;
 }

int map::getActPlatformMobile(int x,int y)
 {
 	int val;

  //mobile[0]->getActFrameColP();  
  if(y/tile_h<0) return max_bound_y;
  for(int i=0;i<n_plat;i++) // Per tutte le piattaforme mobili (le più in alto devono avere indice basso)
   if(mobilep[i]->isActive())
    if(x+23>=*mobilep[i]->getPosXP()-1 && x+23<=*mobilep[i]->getPosXP()+48-1) // c'e una piattaforma mobile sotto (o sopra) il giocatore
     {
      val=*mobilep[i]->getPosYP()-mobilep[i]->getMobileH();
      if(y<=val+8) return val;
     }
	return max_bound_y;
 }

int map::getActWall(bool side, int x, int y)
 {
  int wx,wy,wyy,posx,alte;
  
  if(y/tile_h<0) return 0;
  wx=x%tile_w;
  wy=wall[x/tile_w][y/tile_h];
  posx=walldata[wy][0];
  alte=walldata[wy][1];
  if(wy<0) return 0;
  //textprintf_ex(out,font,0,242,makecol32(255,0,0),-1,">(Wx:%d Posx:%d) y:%d <= Alte:%d V:%d",wx,posx,tile_h-getTileOffsetY(),alte,wy);
  if(alte<=32) // Muro verso l'alto
   {
	if(wx==posx+2 &&  side) if(tile_h-y%tile_h<=alte) return 1; // <-
 	if(wx==posx-2 && !side) if(tile_h-y%tile_h<=alte) return 1; // ->

    // Risolve bug attraversamento non consentito del muro sinistro
	if(wx==posx && x%tile_w==0 && side)  if(tile_h-y%tile_h<=alte) return 1; // <-
   }
  if(alte>32) // Muro verso il basso
   {
   	alte-=32;
	if(wx==posx+2 &&  side) if(y%tile_h<=alte) return 1; // <-
 	if(wx==posx-2 && !side) if(y%tile_h<=alte) return 1; // ->	

    // Risolve bug attraversamento non consentito del muro sinistro
	if(wx==posx && x%tile_w==0 && side) if(y%tile_h<=alte) return 1; // <-
   }
  return 0;
 }

int map::getActWallMobile(bool side,int x,int y)
 {
 	int posx,alte;
  
  if(y/tile_h<0) return 0;
 	for(int i=0;i<n_plat;i++)
 	 if(mobilep[i]->isActive())
 	  if(x+23>=*mobilep[i]->getPosXP()-4 && x+23<=*mobilep[i]->getPosXP()+48+2) // c'e una piattaforma mobile sotto (o sopra) il giocatore
 	   {
 	 	  if(!side) // --->
 	 	   {
 	 	    posx=*mobilep[i]->getPosXP()+mobilep[i]->getWallLeft()-24;
 	 	    alte=mobilep[i]->getWallHLeft();
 	 	    if(x==posx-2 || x==posx) if(y>=*mobilep[i]->getPosYP()-alte && y<=*mobilep[i]->getPosYP()) return 1; // ->
 	     }
 	    if(side) // <---
 	 	   {
 	 	    posx=*mobilep[i]->getPosXP()-mobilep[i]->getWallRight()+24;
 	 	    alte=mobilep[i]->getWallHRight();
 	 	    if(x==posx+2 || x==posx) if(y>=*mobilep[i]->getPosYP()-alte && y<=*mobilep[i]->getPosYP()) return 1; // <-
 	     }
 	   }
 	return 0;
 }

int map::getActStair(int x, int y)
 {
  int sy,sx;

  if(y/tile_h<0) return 0;
  sx=x%tile_w;
  if(sx<=4 || sx>=30) return 0;
  sy=stairs[x/tile_w][y/tile_h];		
  if(sy>=0  && sy<=31) { if(y%tile_h<=sy%tile_h) return 1; } // Basso
  if(sy>=32 && sy<=63) { return 1; }                         // In mezzo
  if(sy>=64 && sy<=95) { if(y%tile_h>=sy%tile_h) return 2; } // Alto
  return 0;
 }

int map::getActPlatformEnemies(int p_x,int p_y,int type,bool side)
 {
 	int x,y,v,val;
 	bool fl=0;

 	x=p_x; y=p_y;
 	if(type==1) // Stairs
 	 {
 	 	int sy,sx;

 	 	if(y/tile_h<0) return 0;
    //if(getActTileY()<0) return 0;
    sx=(scroll_x+x)%tile_w;
    if(sx<=4 || sx>=30) return 0;
    
    sy=stairs[x/tile_w][y/tile_h];
    //sy=stairs[getActTileX()][getActTileY()];

 	 	textprintf_ex(out,font,0,230,makecol32(255,0,0),-1,"S: %d  ymod(32):%d",sy,y%32);

 	 	if(sy>=0  && sy<=31) { if(y%tile_h<=sy%tile_h) return 1; } // Basso
 	 	if(sy>=32 && sy<=63) { return 1; }                         // In mezzo
 		if(sy>=64 && sy<=95) { if(y%tile_h>=sy%tile_h) return 2; } // Alto
 		return 0;
 	 }
 	if(type==2) // Wall
 	 {
 	 	int wx,wy,wyy,posx,alte;

    //if(getActTileY()<0) return 0;
    if(y/tile_h<0) return 0;
    wx=x%tile_w;
    //wy=wall[getActTileX()][getActTileY()];
 	 	wy=wall[x/tile_w][y/tile_h];
 	 	posx=walldata[wy][0];
 	 	alte=walldata[wy][1];
    if(wy<0) return 0;
    if(alte<=32) // Muro verso l'alto
     {
	 	  if((wx==posx+2 || wx==posx+1) &&  side) if(tile_h-y%tile_h<=alte) return 1; // <-
 	    if((wx==posx-2 || wx==posx-1) && !side) if(tile_h-y%tile_h<=alte) return 1; // ->	
	 	  //if(wx==posx+2 &&  side) if(tile_h-getTileOffsetY()<=alte) return 1; // <-
 	    //if(wx==posx-2 && !side) if(tile_h-getTileOffsetY()<=alte) return 1; // ->	
 	   }
    if(alte>32) // Muro verso il basso
     {
     	alte-=32;
	 	  if((wx==posx+2 || wx==posx+1) &&  side) if(y%tile_h<=alte) return 1; // <-
 	    if((wx==posx-2 || wx==posx-1) && !side) if(y%tile_h<=alte) return 1; // ->	
	 	  //if(wx==posx+2 &&  side) if(getTileOffsetY()<=alte) return 1; // <-
 	    //if(wx==posx-2 && !side) if(getTileOffsetY()<=alte) return 1; // ->	
 	   }
 	  return 0;
 	 }

 	for(int i=0;i<num_row*num_pages_y;i++)
 	 {
	 	if(getActTileY()<0) return max_bound_y;
 	 	v=platform[x/tile_w][i];
 	  if(v!=-1 && v!=-2)
 	   {
 	 	  fl=1;
      val=i*32-platformdata[v][x%tile_w];
 	 	  if(y<=val+8) return val;
 	   }
 	 }
 	return max_bound_y;
 }

void map::debug()
 {
  // Tile
  for(int x=0;x<=num_col;x++)
   for(int y=0;y<=num_row;y++)
    putpixel(out,x*tile_w-scroll_dx,y*tile_h-scroll_dy+32,makecol32(255,255,255));

  // Platform
  for(int x=0;x<=num_col;x++)
   for(int y=0;y<=num_row;y++)
    {
     int v,val;

 	 v=platform[x+tilepos_x][y+tilepos_y];
     if(v!=-1 && v!=-2)
      {
       for(int i=0;i<=32;i++)
        {
         val=platformdata[v][i];
         putpixel(out,x*tile_w-scroll_dx+i,y*tile_h-scroll_dy-val+32,makecol32(255,255,0));
        }
      }
    } 	  

  // Stair
  for(int x=0;x<=num_col;x++)
   for(int y=0;y<=num_row;y++)
    {
     int v,val;

 	 v = stairs[x+tilepos_x][y+tilepos_y];
	 if(v>=0 && v<64) // Scala di mezzo/bassa (da 0 a 31, >= a 32 è come se fosse 31)
	  {
	   if(v>=32) v=31;
       for(int i=0;i<=v;i++)
        {
         putpixel(out,x*tile_w-scroll_dx+15,y*tile_h-scroll_dy+i+32,makecol32(0,255,255));
         putpixel(out,x*tile_w-scroll_dx+17,y*tile_h-scroll_dy+i+32,makecol32(0,255,255));
        }
      }
     if(v>=64) // Scala alta (da 64 a 96)
	  {
	   v=96-v;
       for(int i=1;i<=v;i++)
        {
         putpixel(out,x*tile_w-scroll_dx+15,y*tile_h-scroll_dy-i+64,makecol32(0,255,0));
         putpixel(out,x*tile_w-scroll_dx+17,y*tile_h-scroll_dy-i+64,makecol32(0,255,0));
		}
      }
    } 

  // Wall  
  for(int x=0;x<=num_col;x++)
   for(int y=0;y<=num_row;y++)
    {
     int v,posx,alte;

     v=wall[x+tilepos_x][y+tilepos_y];
     if(v>=0)
      {
       posx=walldata[v][0]; alte=walldata[v][1];
       if(alte<=32) 
        {
         line(out,x*tile_w-scroll_dx+posx,y*tile_h-scroll_dy+63,x*tile_w-scroll_dx+posx,y*tile_h-scroll_dy+64-alte,makecol32(255,0,0));
         if(posx==0)
          line(out,x*tile_w-scroll_dx+posx,y*tile_h-scroll_dy+48,x*tile_w-scroll_dx+posx+3,y*tile_h-scroll_dy+48,makecol32(255,0,0));
         if(posx==32)
          line(out,x*tile_w-scroll_dx+posx,y*tile_h-scroll_dy+48,x*tile_w-scroll_dx+posx-3,y*tile_h-scroll_dy+48,makecol32(255,0,0));
        }
       if(alte>32)
        {
         line(out,x*tile_w-scroll_dx+posx,y*tile_h-scroll_dy+32,x*tile_w-scroll_dx+posx,y*tile_h-scroll_dy+alte,makecol32(255,0,255));         
         if(posx==0)  line(out,x*tile_w-scroll_dx+posx,y*tile_h-scroll_dy+48,x*tile_w-scroll_dx+posx+3,y*tile_h-scroll_dy+48,makecol32(255,0,255));
         if(posx==32) line(out,x*tile_w-scroll_dx+posx,y*tile_h-scroll_dy+48,x*tile_w-scroll_dx+posx-3,y*tile_h-scroll_dy+48,makecol32(255,0,255));
        }
      }
    }
  }
