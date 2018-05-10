#include <stdio.h>
#include <string.h>
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
    //0 : normal, 1 : attack, 2 : defense
    int attack = 0;
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
    //direction : true - toward left
    bool direction,predirection;
    int state;
    int w,h;
    int rx=0,ry=0;
    int dx=0,dy=0;
    int down_delay=0;
    int motion_delay=0;
    //player : true - player 1
    bool player;
    vecbox colbox[9];
    SDL_Texture *tex;

    obj(SDL_Texture *_tex,int _x,int _y,bool dir,bool player){
        this->tex = _tex;
        this->x = _x;
        this->y= _y;
        this->direction = dir;
        this->predirection = dir;
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
            this->colbox[i].attack = 0;
        }
    }
    void gravity(){
        this->v+=3;
        this->y+=v;
        if(this->y>base_y)this->y = base_y;
    }
    void move(){
        this->gravity();
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
            if(keystate[SDL_SCANCODE_G] && this->motion_delay<0){
                this->state = 2;
                this->motion_delay = 10;
            }
            if(keystate[SDL_SCANCODE_H] && this->motion_delay<0){
                if(this->down_delay>0)this->state = 4;
                else this->state = 3;
                this->motion_delay = 10;
            }
            if(keystate[SDL_SCANCODE_J] && this->motion_delay<0){
                if(this->down_delay>0)this->state = 6;
                else this->state = 5;
                this->motion_delay = 10;
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
            if(keystate[SDL_SCANCODE_KP_1] && this->motion_delay<0){
                this->state = 2;
                this->motion_delay = 10;
            }
            if(keystate[SDL_SCANCODE_KP_2] && this->motion_delay<0){
                if(this->down_delay>0)this->state = 4;
                else this->state = 3;
                this->motion_delay = 10;
            }
            if(keystate[SDL_SCANCODE_KP_3] && this->motion_delay<0){
                if(this->down_delay>0)this->state = 6;
                else this->state = 5;
                this->motion_delay = 10;
            }
        }

        for(int i=0;i<9;i++){
            this->colbox[i].move(this->x,this->y);
        }
        this->down_delay--;
        this->motion_delay--;
        if(this->down_delay<0 && this->motion_delay <0)this->state=0;

    }

    void draw(){
        char cs[10];
        switch(this->state){
            case 0 : //common state
                this->rx=0;this->ry=0;
                strcpy(cs,"010010010");
                for(int i=0;i<9;i++){
                    if(cs[i]=='0')this->colbox[i].On = false;
                    else this->colbox[i].On = true;
                }
                break;
            case 1 : //sit down
                this->rx=1;this->ry=0;
                strcpy(cs,"000010010");
                for(int i=0;i<9;i++){
                    if(cs[i]=='0')this->colbox[i].On = false;
                    else this->colbox[i].On = true;
                }
                break;
            case 2 : //punch
                this->rx=2;this->ry=0;
                if(this->direction)strcpy(cs,"010011110");
                else strcpy(cs,"010110011");
                for(int i=0;i<9;i++){
                    if(cs[i]=='0')this->colbox[i].On = false;
                    else this->colbox[i].On = true;
                }
                break;
            case 3 : //high-kick
                this->rx=3;this->ry=0;
                if(this->direction)strcpy(cs,"001110010");
                else strcpy(cs,"100011010");
                for(int i=0;i<9;i++){
                    if(cs[i]=='0')this->colbox[i].On = false;
                    else this->colbox[i].On = true;
                }
                break;
            case 4 : //low-kick
                this->rx=0;this->ry=1;
                if(this->direction)strcpy(cs,"000010011");
                else strcpy(cs,"000010110");
                for(int i=0;i<9;i++){
                    if(cs[i]=='0')this->colbox[i].On = false;
                    else this->colbox[i].On = true;
                }
                break;
            case 5 : //guard on body
                this->rx=1;this->ry=1;
                if(this->direction)strcpy(cs,"011011010");
                else strcpy(cs,"110110010");
                for(int i=0;i<9;i++){
                    if(cs[i]=='0')this->colbox[i].On = false;
                    else this->colbox[i].On = true;
                }
                break;
            case 6 : //guard on leg
                this->rx=2;this->ry=1;
                if(this->direction)strcpy(cs,"010011010");
                else strcpy(cs,"010110010");
                for(int i=0;i<9;i++){
                    if(cs[i]=='0')this->colbox[i].On = false;
                    else this->colbox[i].On = true;
                }
                break;
            case 7 : // uppercut
                this->rx=3;this->ry=1;
                if(this->direction)strcpy(cs,"011011110");
                else strcpy(cs,"110110011");
                for(int i=0;i<9;i++){
                    if(cs[i]=='0')this->colbox[i].On = false;
                    else this->colbox[i].On = true;
                }
                break;
            default : ;
        }

        rec.x=192*this->rx,rec.y=192*this->ry,rec.w=192,rec.h=192;
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

        for(int i=0;i<9;i++){
            for(int j=0;j<9;j++){
                if(p1->colbox[i].On && p2->colbox[j].On){
                    double p1_y[2] = {(p1->colbox[i].y-32),(p2->colbox[i].y+32)};
                    double p2_y[2] = {(p2->colbox[j].y-32),(p2->colbox[j].y+32)};
                    if(((p2->colbox[j].x-32) < (p1->colbox[i].x-32)) && ((p1->colbox[i].x-32) < (p2->colbox[j].x+32))){
                        if(((p2->colbox[j].y-32) <= (p1->colbox[i].y-32)) && ((p1->colbox[i].y-32) <= (p2->colbox[j].y+32))){
                            printf("collision\n");
                        }
                        if(((p2->colbox[j].y-32) <= (p1->colbox[i].y+32)) && ((p1->colbox[i].y+32) <= (p2->colbox[j].y+32))){
                            printf("collision\n");
                        }
                    }
                    if(((p2->colbox[j].x-32) < (p1->colbox[i].x+32)) && ((p1->colbox[i].x+32) < (p2->colbox[j].x+32))){
                        if(((p2->colbox[j].y-32) <= (p1->colbox[i].y-32)) && ((p1->colbox[i].y-32) <= (p2->colbox[j].y+32))){
                            printf("collision\n");
                        }
                        if(((p2->colbox[j].y-32) <= (p1->colbox[i].y+32)) && ((p1->colbox[i].y+32) <= (p2->colbox[j].y+32))){
                            printf("collision\n");
                        }
                    }
                }
            }
        }
        p1->update();
        p2->update();

        SDL_SetRenderDrawColor(renderer,255,255,157,0);
        SDL_RenderPresent(renderer);
        SDL_RenderClear(renderer);

        SDL_Delay(30);
    }
    return 0;
}
