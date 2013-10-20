#define LANCE 0
#define KNIFE 1
#define AXE   2
#define SWORD 3
#define ARROW 4
#define FLAME 5

volatile int counter=0;

void count() { counter++; } END_OF_FUNCTION (Count);

int AR_Clock() { return counter; }

void wait(int ms)
 {
  int endwait;

  endwait=AR_Clock()+ms;
  while(AR_Clock()<endwait) {}
 }

int random(int max) // Return a pseudocasual number from 0 to max (max included)
 {
  max++;
  return(rand()%max);
 }

void fade32(BITMAP *bmp)
 {
  int col,r,g,b;
 	
  for(int i=0;i<bmp->w;i++)
   for(int j=0;j<bmp->h;j++)
    {
     col=getpixel(bmp,i,j);
     r = getr32(col); 
     g = getg32(col);
     b = getb32(col);
     r-=20; g-=20; b-=20; if(r<0) r=0; if(g<0) g=0; if(b<0) b=0;
     //putpixel(bmp,i,j,makecol32(r>>1,g>>1,b>>1));
     putpixel(bmp,i,j,makecol32(r,g,b));
    }
 }

bool hitTest8(BITMAP *obj1,BITMAP *obj2,int x1,int y1,int x2,int y2)
 {
 	textprintf_ex(screen,font,0,128,makecol32(255,0,255),-1,"D1:%d D2:%d",bitmap_color_depth(obj1),bitmap_color_depth(obj2));
 	//BITMAP *o1,*o2;
 	//o1=create_bitmap_ex(8,obj1->h,obj1->w);
 	//o2=create_bitmap_ex(8,obj2->h,obj2->w);
 	
 	//draw_sprite(o1,obj1,x1,y1);
 	//draw_sprite(o2,obj2,x2,y2);
 	
 	//draw_sprite(screen,obj1,x1,y1);
 	//draw_sprite(screen,obj2,x2,y2);
 	
 	//obj1=o1; obj2=o2;
 	
  int xmax1,ymax1,xmin1,ymin1;
  int xmax2,ymax2,xmin2,ymin2;
  int xx1,yy1,xx2,yy2;

  xmin1=x1; ymin1=y1; xmax1=x1+obj1->w; ymax1=y1+obj1->h;
  xmin2=x2; ymin2=y2; xmax2=x2+obj2->w; ymax2=y2+obj2->h;
  if(xmin1<xmax2 && ymin1<ymax2 && xmin2<xmax1 && ymin2<ymax1)
   {
    for(int i1=0;i1<obj1->h;i1++) for(int j1=0;j1<obj1->w;j1++)
     for(int i2=0;i2<obj2->h;i2++) for(int j2=0;j2<obj2->w;j2++)
      {
       if(obj1->line[i1][j1] && obj2->line[i2][j2])
        {
         xx1=x1+j1; yy1=y1+i1;
         xx2=x2+j2; yy2=y2+i2;
         if(xx1==xx2 && yy1==yy2) { /*destroy_bitmap(o1); destroy_bitmap(o2);*/ return true; }
         //if(xx1==xx2 && yy1==yy2) { putpixel(screen,xx1,yy1,makecol(255,0,0)); }
        }
      }
   }
  else { /*destroy_bitmap(o1); destroy_bitmap(o2);*/ return false; }
  //destroy_bitmap(o1);
  //destroy_bitmap(o2);
  return false;
 }

bool hitTestFast8(BITMAP *obj1,BITMAP *obj2,int x1,int y1,int x2,int y2,int step=7,int ssx=12,int ssy=5,bool side=0)
 {
  int xmax1,ymax1,xmin1,ymin1;
  int xmax2,ymax2,xmin2,ymin2;
  int xx1,yy1,xx2,yy2;

  //draw_sprite(screen,obj1,x1,y1);
  //draw_sprite(screen,obj2,x2,y2);
  xmin1=x1; ymin1=y1; xmax1=x1+obj1->w; ymax1=y1+obj1->h;
  xmin2=x2; ymin2=y2; xmax2=x2+obj2->w; ymax2=y2+obj2->h;
  if(xmin1<xmax2 && ymin1<ymax2 && xmin2<xmax1 && ymin2<ymax1)
   {
    for(int i1=ssy;i1<obj1->h-ssy;i1+=step) for(int j1=ssx;j1<obj1->w-ssx;j1+=step)
     for(int i2=ssy;i2<obj2->h-ssy;i2++) for(int j2=ssx;j2<obj2->w-ssx;j2++)
      {
       if(obj1->line[i1][j1] && obj2->line[i2][j2])
        {
         xx1=x1+j1; yy1=y1+i1;
         xx2=x2+j2; yy2=y2+i2;
         if(xx1==xx2 && yy1==yy2) return true;
         //if(xx1==xx2 && yy1==yy2) { putpixel(screen,xx1,yy1,makecol(255,0,0)); }
        }
      }
   }
  else return false;
  return false;
 }

bool hitTest24(BITMAP *obj1,BITMAP *obj2,int x1,int y1,int x2,int y2)
 {
  int xmax1,ymax1,xmin1,ymin1;
  int xmax2,ymax2,xmin2,ymin2;
  int xx1,yy1,xx2,yy2;

  xmin1=x1; ymin1=y1; xmax1=x1+obj1->w; ymax1=y1+obj1->h;
  xmin2=x2; ymin2=y2; xmax2=x2+obj2->w; ymax2=y2+obj2->h;
  if(xmin1<xmax2 && ymin1<ymax2 && xmin2<xmax1 && ymin2<ymax1)
   {
    for(int i1=0;i1<obj1->h;i1++) for(int j1=0;j1<obj1->w*4;j1+=4)
     for(int i2=0;i2<obj2->h;i2++) for(int j2=0;j2<obj2->w*4;j2+=4)
      {
       if(makecol32(obj1->line[i1][j1+2],obj1->line[i1][j1+1],obj1->line[i1][j1])!=16711935 &&
          makecol32(obj2->line[i2][j2+2],obj2->line[i2][j2+1],obj2->line[i2][j2])!=16711935) // Transparent 32 bit color*/
       if(obj1->line[i1][j1]!=0 && obj2->line[i2][j2]!=0)
        {
         xx1=x1+(j1>>2); yy1=(y1>>2)+i1;
         xx2=x2+(j2>>2); yy2=(y2>>2)+i2;
         if(xx1==xx2 && yy1==yy2) return true;
         //if(xx1==xx2 && yy1==yy2) { putpixel(screen,xx1,yy1,makecol(255,0,0)); }
        }
      }
   }
  else { return false; }
  return false;
 }

int angle2P(int x0,int y0,int x1,int y1)
 {
 	// L'origine è considerata in (x0,y0)
 	fixed m,ang;

 	m=fixdiv(itofix(y0-y1),itofix(x0-x1));
 	ang=fixatan(m);
 	ang=fixtoi(ang);
 	if(x1>x0 && y1<y0) ang=-ang;
 	if(x1<=x0)         ang=128-ang;
 	if(y1>y0 && x1>x0) ang=255-ang;
  return ang;
 }

void drawBar(int per, int col, int w)
 {
 	rect(screen,1,1,w-2,15,col);
 	w=w-7;
 	per=w*per/100;
 	rectfill(screen,3,3,3+per,13,col);
 }

// Posteffects (experimental)
void _scanline(BITMAP *bmp, int sl)
 {
 	int x,y,r,g,b,col,d;
 	
 	if(sl==1) d=3; // Soft scanlines
 	if(sl==2) d=2; // Normal scanlines
 	if(sl==3) d=1; // Hard scanlines
 	for(x=0;x<bmp->w;x++)
 	 for(y=0;y<bmp->h;y++)
 	  {
 	   if(y%2==0)
 	    {
 	     col=getpixel(bmp,x,y);
 	     r=getr(col); r-=(r>>d);
 	     g=getg(col); g-=(g>>d);
 	     b=getb(col); b-=(b>>d);
 	     putpixel(bmp,x,y,makecol32(r,g,b));
 	    }
 	  }
 }
