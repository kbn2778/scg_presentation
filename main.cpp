#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
SDL_Window *window = NULL;
SDL_Surface *src = NULL;
SDL_Renderer *renderer;
SDL_Event event;
SDL_Rect rec;
SDL_Rect dst;
SDL_Texture *tex;
int winW = 900;
int winH = 900;
int base_y = winH-150;
const Uint8 *keystate;

class vecbox{
public:
    int w=64,h=64;
    double x,y;
    int rx,ry;
    bool On = false;
public:
    vecbox(){}
    void set(int rx,int ry){
        this->rx = rx;
        this->ry = ry;
    }
    void move(double px,double py){
        px = px-96;
        py = py-96;
        this->x = px+rx*64;
        this->y = py+ry*64;
    }
};
class obj{
public:
    double x,y,v=0;
    bool direction;
    int state;
    int w,h;
    int rx=0,ry=0;
    int down_delay=0;
    bool player;
    vecbox colbox[9];
    SDL_Texture *tex;

    obj(SDL_Texture *_tex,int _x,int _y,bool dir,bool player){
        this->tex = _tex;
        this->x = _x;
        this->y= _y;
        this->direction = dir;
        this->player = player;
        this->state = 0;
        SDL_QueryTexture(this->tex,NULL,NULL,&this->w,&this->h);
        int n=0;
        for(int i=0;i<3;i++){
            for(int j=0;j<3;j++){
                this->colbox[n].set(j,i);
                n++;
            }
        }
        for(int i=1;i<9;i+=3){
            this->colbox[i].On = true;
        }
    }
    void gravity(){
        this->v+=3;
        this->y+=v;
        if(this->y>base_y)this->y = base_y;
    }
    void move(){
        this->gravity();
        if(this->down_delay>0)this->down_delay--;
        if(this->player){
            if(keystate[SDL_SCANCODE_A]){
                this->x-=5;
                this->direction = false;
            }
            if(keystate[SDL_SCANCODE_D]){
                this->x+=5;
                this->direction = true;
            }
            if(keystate[SDL_SCANCODE_W] && this->y>=base_y)this->v=-30;
            if(keystate[SDL_SCANCODE_S] && this->down_delay<0){
                this->state = 1;
                this->down_delay = 20;
            }
        }
        else{
            if(keystate[SDL_SCANCODE_LEFT]){
                this->x-=5;
                this->direction = false;
            }
            if(keystate[SDL_SCANCODE_RIGHT]){
                this->x+=5;
                this->direction = true;
            }
            if(keystate[SDL_SCANCODE_UP] && this->y>=base_y)this->v=-30;
            if(keystate[SDL_SCANCODE_DOWN] && this->down_delay<0){
                this->state = 1;
                this->down_delay = 20;
            }
        }

        for(int i=0;i<9;i++){
            this->colbox[i].move(this->x,this->y);
        }
        this->down_delay--;
        if(this->down_delay<0)this->state=0;

    }

    void draw(){
        int *cs= new int[9];
        switch(this->state){
            case 0 :
                this->rx=0;this->ry=0;
                //cs = {0,1,0,0,1,0,0,1,0};
                for(int i=0;i<9;i++){
                    this->colbox[i].On = cs[i];
                }
                break;
            case 1 :
                this->rx=1;this->ry=0;
                //int cs[9] = {0,0,0,0,1,0,0,1,0};
                //for(int i=0;i<9;i++){
                //    this->colbox[i].On = cs[i];
                //}
                break;
            default : ;
        }
        rec.x=192*rx,rec.y=192*ry,rec.w=192,rec.h=192;
        dst.x=this->x-96,dst.y=this->y-96,dst.w=192,dst.h=192;
        if(this->direction)SDL_RenderCopyEx(renderer,this->tex,&rec,&dst,0,NULL,SDL_FLIP_NONE);
        else SDL_RenderCopyEx(renderer,this->tex,&rec,&dst,0,NULL,SDL_FLIP_HORIZONTAL);
    }

    void update(){
        this->move();
        this->draw();
    }
};
int main(int argc,char** argv)
{
    bool running = true;

    SDL_Init(SDL_INIT_EVERYTHING);
    window = SDL_CreateWindow("SDL_tutorial",240,120,winW,winH,0);
    renderer=SDL_CreateRenderer(window,-1,0);
    printf("setting");
    obj *p1 = new obj(IMG_LoadTexture(renderer,"p1.png"),120,base_y,true,true);
    obj *p2 = new obj(IMG_LoadTexture(renderer,"p2.png"),720,base_y,false,false);
    while(running){
        while(SDL_PollEvent(&event)){
            switch(event.type){
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode){
                        case SDL_SCANCODE_ESCAPE : printf("exit");running = false;
                            break;
                        default : ;
                    }
                    break;
                case SDL_QUIT : running= false;
                    break;
            }
        }
        keystate = SDL_GetKeyboardState(0);
        p1->update();
        p2->update();
        SDL_SetRenderDrawColor(renderer,255,255,157,0);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        SDL_Delay(30);
    }
    return 0;
}
