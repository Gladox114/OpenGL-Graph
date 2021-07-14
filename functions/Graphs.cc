#include <iostream>
#include "../Windowing.h"
#include "shader_s.h"

#include "../OpenGLStuff.h"
#include "../glad/glad.h"
#include <GLFW/glfw3.h>

#include <ctime>
#include <chrono>
#include <vector>
#include <array>
#include <math.h>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

float offset_x = 0.0;
float scale_x = 1.0;
float offset_y = 0.0;
float scale_y = 1.0;
float wheel_scale_x = 1.0;
int axis_x_offset = 16;
int axis_y_offset = 16;
int axis_Line_Length = 6;
float wheel_val = 0;
float timeSpeed = 1;
float deltaTime = 0.0f; // Time between current frame and last frame
// create an array struct

int SCR_WIDTH = 1600;
int SCR_HEIGHT = 1000;

struct point {
    float x;
    float y;
};


struct pointArray {
    point* data;
    int size;
    float* color;
};

class PlotData {

    public:
        unsigned int* VAOs;
        unsigned int* VBOs;
        pointArray* plotArray;
        int pixels;
        int strength;
        Shader* shader;
        int amount;
        PlotData(Shader* shader, int pixels, int strength, int amount,float color[][3]) : shader(shader), pixels(pixels), strength(strength), amount(amount) {
            plotArray = new pointArray[amount];
            for (int i = 0;i<amount;i++) {
                plotArray[i] = {new point[pixels],pixels,color[i]};
                for (int z = 0;z<pixels;z++){
                    plotArray[i].data[z].x = z/((float)(pixels-1)/2)-1;
                    plotArray[i].data[z].y = 0;
                }
            }

            VAOs = new unsigned int[amount];
            VBOs = new unsigned int[amount];

            glGenVertexArrays(amount,VAOs);
            glGenBuffers(amount, VBOs);

            for (int i = 0;i<amount;i++) {
                glBindVertexArray(VAOs[i]);
                glBindBuffer(GL_ARRAY_BUFFER,VBOs[i]);
                glBufferData(GL_ARRAY_BUFFER, plotArray[i].size*sizeof(float)*2, plotArray[i].data, GL_DYNAMIC_DRAW);
                glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
                glEnableVertexAttribArray(0);
            }
            
        }
        void updateAllData(){
            for (int i = 0;i<amount;i++) {
                glBindVertexArray(VAOs[i]);
                glBindBuffer(GL_ARRAY_BUFFER,VBOs[i]);
                glBufferSubData(GL_ARRAY_BUFFER,0,plotArray[i].size*sizeof(float)*2,plotArray[i].data);
            }
        }
        void updateData(int ID){
            glBindVertexArray(VAOs[ID]);
            glBindBuffer(GL_ARRAY_BUFFER,VBOs[ID]);
            glBufferSubData(GL_ARRAY_BUFFER,0,plotArray[ID].size*sizeof(float)*2,plotArray[ID].data);
        }
        void draw(){
                glLineWidth(2);
                shader->use();
                shader->setFloat("offset_x",offset_x);
                shader->setFloat("scale_x",scale_x);
                shader->setFloat("offset_y",offset_y);
                shader->setFloat("scale_y",scale_y);
                shader->setFloat("wheel_scale_x",wheel_scale_x);
            for (int i = 0;i<amount;i++) {

                shader->setVec3("color",plotArray[i].color[0],plotArray[i].color[1],plotArray[i].color[2]);
                glBindVertexArray(VAOs[i]);
                glDrawArrays(GL_LINE_STRIP, 0, plotArray[i].size);
                glLineWidth(1);
                printf("drawing Lines\n");
            }
        }
};


struct Parable {
    float velocity;
    int height;
    float factor_a;
    float position;
    std::array<uint8_t, 3> color;
    float strengthBuffered;
    float deleteBorder;
    float intensity;
    std::array<uint8_t, 3> intensityColorBuffered;
};

struct WaveData {
    float velocity;
    int height;
    float factor_a;
    float position;
    std::array<uint8_t, 3> color;
    //return_type (*var_name)(arg_type1, arg_type2) = some_function;
    float (*waveFunction)(WaveData,int);
    float strengthBuffered;
    float deleteBorder;
    float intensity;
    std::array<uint8_t, 3> intensityColorBuffered;
};

void calcAxis(point* axis,float x_offset,float y_offset) {

    axis[0].x = x_offset;
    axis[0].y = 1.0;
    axis[1].x = x_offset;
    axis[1].y = y_offset;

    axis[2].x = x_offset;
    axis[2].y = y_offset;
    axis[3].x = 1.0;
    axis[3].y = y_offset;
}

void calcXAxisRays(point* rays,float x_offset,float y_offset,int ray_num,float length) {
    
    float path = 2-(x_offset+1);
    float distance = path/((ray_num-2)/2);

    for (int i = 0; i<ray_num;i+=2) {
        float position = distance * -((i+1)/2)+1;
        rays[i].x = position;
        rays[i].y = y_offset;
        rays[i+1].x = position;
        rays[i+1].y = y_offset-length;
    }
}

void calcYAxisRays(point* rays,float x_offset,float y_offset,int ray_num,float length) {
    
    // get the whole path between the beginning (that can be in the middle of the screen) and the right edge 
    float path = 2-(y_offset+1);
    // get the distance between each line
    float distance = path/((ray_num-2)/2);

    for (int i = 0; i<ray_num;i+=2) {
        //printf("ray_num: %i \ncurrent_num: %i\n",ray_num,i);
        float position = distance * -((i+1)/2)+1;
        //printf("fullDistance: %f\n",position);
        //printf("heightdif: %f and %f and %f\n",x_offset-length,x_offset,length);
        rays[i].x = x_offset-length;
        rays[i].y = position;
        rays[i+1].x = x_offset;
        rays[i+1].y = position;
    }
}


void calcXAxisRayLines(point* rayLines, float x_offset, float y_offset, int ray_num) {

    float path = 2-(x_offset+1);
    float distance = path/((ray_num-2)/2);

    for (int i = 0; i<ray_num;i+=2) {
        float position = distance * -((i+1)/2)+1;
        rayLines[i].x = position;
        rayLines[i].y = y_offset;
        rayLines[i+1].x = position;
        rayLines[i+1].y = 1;

    }

}

void calcYAxisRayLines(point* rayLines, float x_offset, float y_offset, int ray_num) {

    float path = 2-(y_offset+1);

    float distance = path/((ray_num-2)/2);

    for (int i = 0; i<ray_num;i+=2) {
        float position = distance * -((i+1)/2)+1;
        rayLines[i].x = x_offset;
        rayLines[i].y = position;
        rayLines[i+1].x = 1;
        rayLines[i+1].y = position;

    }

}

void printPlot(point plot[],unsigned int size) {

    printf("size: %i\n",size);
    for (int i = 0; i<size;i++) {
        printf("x,y: %f, %f\n",plot[i].x,plot[i].y);
    }
}

float calculateLinear(WaveData wave,int x) {
    return std::abs(wave.factor_a*(x-wave.position))*-1+wave.height;
}

float calculateParabola(WaveData wave,int x) {
    float xe = (float)x-wave.position;    
    float y = wave.factor_a * (xe*xe) + (float)wave.height;
    return y;
}

float calculateSine(WaveData wave,int x) {
    return std::sin(x);
}

float get_width(Parable parable){
    // sqrt((y-d)/a)
    float partx = sqrt((0-parable.height) /parable.factor_a);
    float x1 = -partx+parable.position;
    float x2 = partx+parable.position;
    printf("x1,x2,x1-x2 = %f, %f, %f\n",x1,x2,x2-x1);
    return (x2-x1)/2;
}

class GraphBody {
    public:
        unsigned int VAOs[5];
        unsigned int VBOs[5];
        PlotData* plotData;
        point* xRibbles;
        int xRibble_size;
        point* xRibbleLines;
        int xRibbleLine_size;
        point* yRibbles;
        int yRibble_size;
        point* yRibbleLines;
        int yRibbleLine_size;
        point axis[4];
        int axis_size = 4;
        Shader* shader;
    GraphBody(Shader* shader, int xRibble_size, int yRibble_size, int width, int height) : shader(shader), xRibble_size(xRibble_size*2+2), yRibble_size(yRibble_size*2+2) {
        xRibbleLine_size = xRibble_size*2+2;
        yRibbleLine_size = yRibble_size*2+2;
        xRibble_size = xRibble_size*2+2;
        yRibble_size = yRibble_size*2+2;
        
        xRibbles = new point[xRibble_size];
        yRibbles = new point[yRibble_size];
        xRibbleLines = new point[xRibbleLine_size];
        yRibbleLines = new point[yRibbleLine_size];

        //printf("test: %s\n",typeid(VAOs).name());

        generateData(width,height);
        //printf("test %i\n",VAOs[0]);
        glGenVertexArrays(5,VAOs);
        glGenBuffers(5, VBOs);
        //printPlot(xRibbleLines,xRibbleLine_size);


        glBindVertexArray(VAOs[0]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[0]);
        glBufferData(GL_ARRAY_BUFFER, axis_size*sizeof(float)*2, axis, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[1]);
        glBufferData(GL_ARRAY_BUFFER, xRibble_size*sizeof(float)*2, xRibbles, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(VAOs[2]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[2]);
        glBufferData(GL_ARRAY_BUFFER, yRibble_size*sizeof(float)*2, yRibbles, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
        
        glBindVertexArray(VAOs[3]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[3]);
        glBufferData(GL_ARRAY_BUFFER, xRibbleLine_size*sizeof(float)*2, xRibbleLines, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(VAOs[4]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[4]);
        glBufferData(GL_ARRAY_BUFFER, yRibbleLine_size*sizeof(float)*2, yRibbleLines, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);
    }
    
    void generateData(int width, int height) {
        //float axis_x_offset_translated = (float)axis_x_offset/width*2-1.;
        //float axis_y_offset_translated = (float)axis_y_offset/height*2-1.;
        
        //offset_x = (axis_x_offset_translated+1)/2;
        //offset_y = (axis_y_offset_translated+1)/2;

        offset_x = (float)axis_x_offset/width;
        offset_y = (float)axis_y_offset/height;

        float axis_x_offset_translated = offset_x*2-1;
        float axis_y_offset_translated = offset_y*2-1;

        scale_x = 1. - offset_x;
        scale_y = 1. - offset_y;

        calcAxis(axis,axis_x_offset_translated,axis_y_offset_translated);
        // translate the pixel width to floating variable width
        float Xlength = (float)axis_Line_Length/height*2;
        calcXAxisRays(xRibbles,axis_x_offset_translated,axis_y_offset_translated,xRibble_size,Xlength);

        float Ylength = (float)axis_Line_Length/width*2;
        calcYAxisRays(yRibbles,axis_x_offset_translated,axis_y_offset_translated,yRibble_size,Ylength);
        
        calcXAxisRayLines(xRibbleLines,axis_x_offset_translated,axis_y_offset_translated,xRibbleLine_size);

        calcYAxisRayLines(yRibbleLines,axis_x_offset_translated,axis_y_offset_translated,yRibbleLine_size);
        
    }
    void draw() {
        shader->use();
        shader->setFloat("offset_x",offset_x);
        shader->setFloat("scale_x",scale_x);
        shader->setVec4("color",0.5,0.5,0.5,1);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_LINES, 0, axis_size);
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_LINES, 0, xRibble_size);
        glBindVertexArray(VAOs[2]);
        glDrawArrays(GL_LINES, 0, yRibble_size);
    }
    void drawGrid() {
        shader->use();
        //shader->setFloat("offset_x",offset_x);
        //shader->setFloat("scale_x",scale_x);
        shader->setVec4("color",0.0,0.0,0.0,0.06);
        glBindVertexArray(VAOs[3]);
        glDrawArrays(GL_LINES, 0, xRibbleLine_size);
        glBindVertexArray(VAOs[4]);
        glDrawArrays(GL_LINES, 0, yRibbleLine_size);
    }
    void updateAxis() {
        glBindVertexArray(VAOs[0]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[0]);
        glBufferSubData(GL_ARRAY_BUFFER,0,axis_size*sizeof(float)*2,axis);

        glBindVertexArray(VAOs[1]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[1]);
        glBufferSubData(GL_ARRAY_BUFFER,0,xRibble_size*sizeof(float)*2,xRibbles);

        glBindVertexArray(VAOs[2]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[2]);
        glBufferSubData(GL_ARRAY_BUFFER,0,yRibble_size*sizeof(float)*2,yRibbles);
        
        glBindVertexArray(VAOs[3]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[3]);
        glBufferSubData(GL_ARRAY_BUFFER,0,xRibbleLine_size*sizeof(float)*2,xRibbleLines);

        glBindVertexArray(VAOs[4]);
        glBindBuffer(GL_ARRAY_BUFFER,VBOs[4]);
        glBufferSubData(GL_ARRAY_BUFFER,0,yRibbleLine_size*sizeof(float)*2,yRibbleLines);
        
    }
};

/*
void updateDeleteBorder(Parable parable) {
    parable.deleteBorder = get_width(parable);
}*/

bool checkBorder(Parable parable,int border1,int border2) {
    if (parable.position+parable.deleteBorder < border1) {
        printf("is %f smaller than %i\n",parable.position+parable.deleteBorder,border1);
        return true;
    }
    
    else if (parable.position-parable.deleteBorder > border2){
        printf("is %f larger than %i\n",parable.position-parable.deleteBorder,border2);
        return true;
    }
    
    return false;
}

void borderPatrol(std::vector<Parable> &parableArray,int border1,int border2) {
    for (int iParable = 0; iParable<parableArray.size();iParable++)
        if (checkBorder(parableArray[iParable],border1,border2)) {
            parableArray.erase(parableArray.begin()+iParable);
            printf("erase %i\n",iParable);
        }
}

void somefunc(point* graph,int pixels,int strength) {
    for (int i = 0; i < pixels; i++) {
        float x = (i - pixels/2) / 100.0;
        graph[i].x = x;
        graph[i].y = sin(x * 10.0) / (1.0 + x * x);
    }
}

void somefunc2(point* graph,int pixels,int strength) {
    for (int i = 0; i < pixels; i++) {
        float x = (i - pixels/2) / 100.0;
        graph[i].x = x;
        graph[i].y = cos(x * 10.0) / (1.0 + x * x);
    }
}

void line(point* graph, int pixels, int strength, double time){
    for (int x = 0; x < pixels; x++){
        int y = x-time*8;

        if (y < 0)
            y = 0;
        else if (y > strength)
            y = strength;
        graph[x].x = x/((float)(pixels-1)/2)-1;
        graph[x].y = y/((float)strength/2)-1;
    }
}


void calcParable(pointArray* graph,Parable* parableArray,int amount, int pixels, int strength){
    
    // 1
    // for each x calculate the parable y position. 
    // Calculate all the colors with the y strength. 
    // Add them alltogether (mix them). 
    // Write them to the array.

    // 2
    // for each parable calculate the y positions for all x positions and store them.
    // calculate for each x position with y the RGB colors and store them.
    // calculate for each x position all the arrays with colors (mix them).
    // store the final mixed colors into the final array.

    for (int x = 0; x < pixels; x++){

        float colors[3] = {0,0,0};
        for (int iParable = 0; iParable < amount; iParable++) {
            // calculate the parable simply  (y = a*(x-e)²+d)
            //float position = parableArray[iParable].position+parableArray[iParable].velocity*wheel_val;
            float xe = (float)x- parableArray[iParable].position+parableArray[iParable].velocity*wheel_val;
            //xe *= xe;
            float y = parableArray[iParable].factor_a*(xe*xe)+(float)parableArray[iParable].height;

            if (y < 0)
                y = 0;

            for (int color = 0; color<3; color++) {
                if (parableArray[iParable].color[color] *(y/strength) > 0){
                    //colors[color] += parableArray[iParable].color[color]*(y/strength);
                    colors[color] = std::max(colors[color],parableArray[iParable].color[color]*(y/strength));
                    if (colors[color] > strength)
                        colors[color] = strength;
                }
            }

        }
        for (int color = 0; color<3; color++) {
            // translate the integer into -1 to 1 positions.
            graph[color].data[x].y = colors[color]/((float)strength/2)-1;
        }
    }


}



void calcParableMixColors(pointArray* graph,std::vector<Parable> &parableArray,int amount, int pixels, int strength) {
    for (int x = 0; x < pixels; x++){
        float colors[3] = {0,0,0};
        float sumOfStrength = 0;
        for (int iParable = 0; iParable < amount; iParable++) {
            // calculate the parable simply  (y = a*(x-e)²+d)
            float xe = (float)x-parableArray[iParable].position + parableArray[iParable].velocity * wheel_val;
            float y = parableArray[iParable].factor_a * (xe*xe) + (float)parableArray[iParable].height;
            // clip the bottom and top to fit in the graph
            if (y < 0) y = 0;
            else if (y > strength) y = strength;
            // save the y as strength
            parableArray[iParable].strengthBuffered = y/strength;
        }
        for (int iParable = 0; iParable < amount; iParable++){
            sumOfStrength += parableArray[iParable].strengthBuffered;
        }
        if (sumOfStrength < 1) sumOfStrength = 1;
        for (int iParable = 0; iParable < amount; iParable++) {
            float newStrength = parableArray[iParable].strengthBuffered/sumOfStrength;
            for (int color = 0; color<3; color++) {
                colors[color] += parableArray[iParable].color[color]*newStrength;
            }
        }

        for (int color = 0; color<3; color++) {
            // translate the integer into -1 to 1 positions.
            graph[color].data[x].y = colors[color]/((float)strength/2)-1;
        }
        
    }

}

void calcWaveMixColors(pointArray* graph,std::vector<WaveData> &waves, int pixels, int strength) {
    for (int x = 0; x < pixels; x++){
        float colors[3] = {0,0,0};
        float sumOfStrength = 0;
        int anount = waves.size();
        for (int iWave = 0; iWave < anount; iWave++) {
            // calculate the wave with it's own function
            float y = waves[iWave].waveFunction(waves[iWave],x+waves[iWave].velocity * wheel_val);
            // clip the bottom and top to fit in the graph
            if (y < 0) y = 0;
            else if (y > strength) y = strength;
            // save the y as strength
            waves[iWave].strengthBuffered = y/strength;
        }
        for (int iWave = 0; iWave < anount; iWave++){
            sumOfStrength += waves[iWave].strengthBuffered;
        }
        if (sumOfStrength < 1) sumOfStrength = 1;
        for (int iWave = 0; iWave < anount; iWave++) {
            float newStrength = waves[iWave].strengthBuffered/sumOfStrength;
            for (int color = 0; color<3; color++) {
                colors[color] += waves[iWave].color[color]*newStrength;
            }
        }

        for (int color = 0; color<3; color++) {
            // translate the integer into -1 to 1 positions.
            graph[color].data[x].y = colors[color]/((float)strength/2)-1;
        }
        
    }

}


void calcParableMixColorsWithIntensity(pointArray* graph,std::vector<Parable> &parableArray,int amount, int pixels, int strength) {
    for (int x = 0; x < pixels; x++){
        float colors[3] = {0,0,0};
        float sumOfStrength = 0;
        for (int iParable = 0; iParable < amount; iParable++) {
            // calculate the parable simply  (y = a*(x-e)²+d)

            float xe = (float)x-parableArray[iParable].position + parableArray[iParable].velocity * wheel_val;
            float y = parableArray[iParable].factor_a * (xe*xe) + (float)parableArray[iParable].height;

            // clip the bottom and top to fit in the graph
            if (y < 0) y = 0;
            else if (y > strength) y = strength;
            // save the y as strength
            parableArray[iParable].strengthBuffered = y/strength;
        }
        for (int iParable = 0; iParable < amount; iParable++){
            sumOfStrength += parableArray[iParable].strengthBuffered;
        }
        if (sumOfStrength < 1) sumOfStrength = 1;
        for (int iParable = 0; iParable < amount; iParable++) {
            float newStrength = parableArray[iParable].strengthBuffered/sumOfStrength;
            for (int color = 0; color<3; color++) {
                colors[color] += parableArray[iParable].intensityColorBuffered[color]*newStrength;
            }
        }

        for (int color = 0; color<3; color++) {
            // translate the integer into -1 to 1 positions.
            graph[color].data[x].y = colors[color]/((float)strength/2)-1;
        }
        
    }
    
}

void moveParables_time(std::vector<Parable> &parableArray, int amount,float deltaTime){
    for (int iParable = 0; iParable < amount; iParable++) {
        parableArray[iParable].position += parableArray[iParable].velocity *timeSpeed * deltaTime;
        //printf("pos: %f\n",parableArray[iParable].position);
    }
}

void createParable(std::vector<Parable> &parableArray, float velocity,float factor_a, float position, std::array<uint8_t, 3> color) {
    parableArray.push_back({velocity,255,factor_a,position,color});
    parableArray[parableArray.size()-1].deleteBorder = get_width(parableArray[parableArray.size()-1]);
}

void createParable_AtBorder(std::vector<Parable> &parableArray,
                            bool spawn_left,int border1,int border2,
                            float velocity,float factor_a,std::array<uint8_t, 3> color) {
    
    Parable parable = {velocity,255,factor_a,0,color,1};
    parable.deleteBorder = get_width(parable);
    if (spawn_left) {
        parable.position = border1-parable.deleteBorder;
        printf("TEEEEST!\n");}
    else {
        parable.position = border2+parable.deleteBorder;
        parable.velocity = -velocity;}
    parableArray.push_back(parable);

}

void spawnRandomParable(std::vector<Parable> &parableArray,int border1, int border2,
                        float* velocity, float* factor_a, std::array<uint8_t, 3> color) {
    float test_vel = float(rand())/float(RAND_MAX)*velocity[1]+velocity[0];
    float test_a = float(rand())/float(RAND_MAX)*factor_a[1]+factor_a[0];
    createParable_AtBorder(parableArray,rand() % 2,border1,border2,
                                test_vel,
                                -test_a,
                                color);
    printf("debug1: %f, %f \n",test_vel,test_a);
}

int width;
int height;
float deltaTime2;
using ms = std::chrono::duration<float, std::milli>;
//-------Shaders--------
Shader *axisShader;
Shader *plotShader;

std::vector<Parable> parable = {
    {10,255,-0.4,0,{60,0,0}},
    {-10,255,-0.4,400,{0,60,0}}
    //{-10,200,-0.4,400,{200,0,0}}
};

std::vector<WaveData> waves = {
    {10,255,4,0,{200,0,30},calculateLinear},
    {-10,255,-0.2,400,{0,50,200},calculateParabola}
};

extern "C" {
    int init(Windowing::WindowData* window) {
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
            std::cout << "Failed to initialize GLAD" << std::endl;
            //exit(-1);
            return -1;
        }
        glfwGetFramebufferSize(window->m_Window, &width, &height);

        axisShader = new Shader("functions/shader/Axis.vs","functions/shader/Axis.fs");
        plotShader = new Shader("functions/shader/plot.vs","functions/shader/plot.fs");

        // create Graphbody and save it into the window
        window->dataPointer = new GraphBody(axisShader,400,255,width,height);
        // pre-calculate the frame with the width and height
        ((GraphBody*)(window->dataPointer))->generateData(width,height);

        float colors[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
        // create plotData and save it into Graphbody
        ((GraphBody*)(window->dataPointer))->plotData = new PlotData(plotShader, 400, 255, 3,colors);
        
        for (int i = 0; i<parable.size(); i++) {
            parable[i].deleteBorder = get_width(parable[i]);
            printf("width = %f\n",parable[i].deleteBorder);
        }

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glfwSwapInterval(0);
        /*
        continue Work: 
        change other functions so it passes the window object to init.
        save plotdata in grapbody and graphbody in windowObj as void*
        */

        return 0;
    }

    void mainFunc(Windowing::WindowData* window) {
        auto start = std::chrono::system_clock::now();

        // input
        // -----
        processInput(window->m_Window);

        // rendering commands
        // ------------------
        glClearColor(1,1,1,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        ((GraphBody*)(window->dataPointer))->drawGrid();

        PlotData* plotData = ((GraphBody*)(window->dataPointer))->plotData; 

        calcWaveMixColors(plotData->plotArray, waves, plotData->pixels, plotData->strength);

        plotData->updateAllData();
        plotData->draw();

        ((GraphBody*)(window->dataPointer))->draw();

        glfwGetFramebufferSize(window->m_Window, &width, &height);

        std::cout << "I am " << window->m_ID << std::endl;

        auto stop = std::chrono::system_clock::now();
        deltaTime2 = std::chrono::duration_cast<ms>(stop - start).count();
    }
}

// this will only be used when the script is called locally.
int main() {
    // init
    if (OGLS::initGLFW()) exit(1);
    // create window object
    Windowing::WindowData WindowObj = Windowing::WindowData(900,500,"test",NULL,NULL);
    if (OGLS::checkWindow(WindowObj.m_Window)) exit(1);
    // init GLAD and the rest
    init(&WindowObj);



    while(!glfwWindowShouldClose(WindowObj.m_Window)) {
        mainFunc(&WindowObj);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
               
        glfwSwapBuffers(WindowObj.m_Window);
        glfwPollEvents();
    }

    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window){
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, 1);
    float speed;
    if(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speed = 5.0f;
    } else {
        speed = 1.0f;
    }

    if(glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
        wheel_val = 0;
    
    if(glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
        timeSpeed = 0;
    

    
    

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        timeSpeed += speed*deltaTime;
        //std::cout << timeSpeed << std::endl;
        }
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        timeSpeed -= speed*deltaTime;
        //std::cout << timeSpeed << std::endl;
        }

}