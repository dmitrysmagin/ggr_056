class _sprite
 {
  protected:
   int start_x,start_y;
   int X,Y;
   int index;
   int action;
   int rows,columns,width,height; // Sprite frame size in bitmap file
   int act_frame;                 // Sprite current frame
   bool enabled;
   BITMAP *screen;
   BITMAP **frames;               // Normal frames
   BITMAP **frames_collision;     // Collision frames

  public:
   _sprite(char *,BITMAP *,int,int,int,_sprite *);
   ~_sprite();
   void loadSprite(char *,_sprite *);
   void loadData(char *);
   void loadFrames(char *);
   void setStartX(int);
   void setStartY(int);
   void setX(int);
   void setY(int);
   void setCurrentFrame(int);
   void setScreen(BITMAP *);
   int getRows();
   int getColumns();
   int getWidth();
   int getHeight();
   int getX();
   int getY();
   int getCurrentFrame();
   BITMAP *getFrame(int);
   bool isEnabled();
 };

_sprite::_sprite(char *filename,BITMAP *bmp,int sx,int sy,int i,_sprite *p)
 {
  index = i;
  loadSprite(filename,p);
  setScreen(bmp);
  start_x = sx; start_y = sy;
 }

_sprite::~_sprite() { }

void _sprite::loadSprite(char *filename,_sprite *p)
 {
  if(!index)
   {
    loadData(filename);
    loadFrames(filename);
   }
  else
   {
	rows    = p->getRows();
	columns = p->getColumns();
	width   = p->getWidth();
    height  = p->getHeight();
    frames           = p->frames;
    frames_collision = p->frames_collision;
   }
 }

void _sprite::loadData(char *filename)
 {
  FILE *fd;
  char str[255];

  fd = fopen(strcat(strcpy(str,filename),".txt"),"r");
  if(!fd) { allegro_message("File \"%s\" not found.\n\nGame closed!",str); exit(0); }
  fscanf(fd,"%d%s%d%s%d%s%d",&rows,str,&columns,str,&width,str,&height);
  fclose(fd);
 }

void _sprite::loadFrames(char *filename)
 {
  BITMAP *aux;
  char str[255];

  frames           = (BITMAP **)malloc(sizeof(BITMAP *)*(rows*columns));
  frames_collision = (BITMAP **)malloc(sizeof(BITMAP *)*(rows*columns));

  aux = load_bmp(strcat(strcpy(str,filename),".bmp"),NULL);
  if(!aux) { allegro_message("File \"%s\" not found.\n\nGame closed!",str); exit(0); }
  for(int i=0;i<rows;i++)
   for(int j=0;j<columns;j++)
    {
     frames[j+columns*i]           = create_bitmap(width,height);
     frames_collision[j+columns*i] = create_bitmap_ex(8,width,height);
     //blit(aux,frames[j+columns*i],width*j+j,height*i+i,0,0,frames[j+columns*i]->w,frames[j+columns*i]->h);
     blit(aux,frames[j+columns*i],width*j,height*i,0,0,frames[j+columns*i]->w,frames[j+columns*i]->h);
     clear_to_color(frames_collision[j+columns*i],0);
     for(int h=0;h<frames[j+columns*i]->h;h++)
      for(int w=0;w<frames[j+columns*i]->w*4;w+=4)
       if(makecol32(frames[j+columns*i]->line[h][w+2],
                    frames[j+columns*i]->line[h][w+1],
                    frames[j+columns*i]->line[h][w]) != 16711935) frames_collision[j+columns*i]->line[h][w>>2] = 1;
    }
  destroy_bitmap(aux);
 }

void _sprite::setStartX(int sx)      { start_x = sx; }

void _sprite::setStartY(int sy)      { start_y = sy; }

void _sprite::setX(int x)            { X = x; }

void _sprite::setY(int y)            { Y = y; }

void _sprite::setCurrentFrame(int f) { act_frame = f; }

void _sprite::setScreen(BITMAP *bmp) { screen = bmp; }

int _sprite::getRows()         { return rows;    }

int _sprite::getColumns()      { return columns; }

int _sprite::getWidth()        { return width;   }

int _sprite::getHeight()       { return height;  }

int _sprite::getX()            { return X; }

int _sprite::getY()            { return Y; }

int _sprite::getCurrentFrame() { return act_frame; }

BITMAP *_sprite::getFrame(int i) { return frames[i]; }

bool _sprite::isEnabled() { return enabled; }
