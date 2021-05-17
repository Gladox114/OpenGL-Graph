#include <iostream>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <math.h>
#include "shader_s.h"
#include <array>
#include <ctime>
#include <chrono>

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int SCR_WIDTH = 1600;
int SCR_HEIGHT = 1000;


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
struct point {
    float x;
    float y;
};

class GraphBody;

class WindowData {

    public:
        GLFWwindow* m_Window;
        GraphBody* graphBody;
        unsigned int* VAO;
        unsigned int* VBO;
        point* Axis;
        point* XRays;
        WindowData(int width,int height, const char* title, GLFWmonitor* monitor, GLFWwindow* share) {
            m_Window = glfwCreateWindow(width,height,title,monitor,share);
            glfwSetWindowUserPointer(m_Window,reinterpret_cast<void *>(this));
        }
        ~WindowData() {
            glfwDestroyWindow(m_Window);
        }
    private:
        GLFWmonitor* m_Monitor;

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
/*
class plotArrayData{
    public:
        plotArrayData(int size) {

        }
}*/


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
            }
        }
};



class GraphBody {
    public:
        unsigned int VAOs[5];
        unsigned int VBOs[5];
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

// first degrade a color by the percentage
// R1 * 0.5

// mixing two colors
// R1 / 2 + R2 / 2
// G1 / 2 + G2 / 2
// B1 / 2 + B2 / 2


struct Parable {
    float velocity;
    int height;
    float factor_a;
    float position;
    int color[3];
    float strengthBuffered;
    float deleteBorder;
};

float get_width(Parable parable){
    // sqrt((y-d)/a)
    float partx = sqrt((0-parable.height) /parable.factor_a);
    float x1 = -partx+parable.position;
    float x2 = partx+parable.position;
    //printf("x1,x2,x1-x2 = %f, %f, %f\n",x1,x2,x2-x1);
    return (x2-x1)/2;
}

void updateDeleteBorder(Parable parable) {
    parable.deleteBorder = get_width(parable);
}

void checkBorder(Parable parable,int border1,int border2) {
    if (parable.position+parable.deleteBorder < border1){}
    
    if (parable.position-parable.deleteBorder > border2){}
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



            //printf("position: %f, factor_a: %f, xe: %f, y: %f, x: %f\n",position,parableArray[iParable].factor_a,xe,y,(float)x);
            // cut the positions if its out of the graph
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
            /*
            for (int color = 0;color<3;color++) {
                // add half of the old color with half of the new color. But before this calculate the brightness with variable y of the new color.
                //colors[color] = (colors[color]/2) + (parableArray[iParable].color[color] * (y/strength));
                int currentColor = parableArray[iParable].color[color] * (y/(float)strength);
                if (colors[color] > 0 && currentColor > 0)
                colors[color] = (colors[color]/2) + currentColor;
                else {
                    int max = std::max(colors[color],currentColor);
                    if (max > 0) colors[color] = max;
                }

                //if (parableArray[iParable].color[color] * (y/strength) != 0)
                //printf("colorVal: %i * strength: %f = %f \n",parableArray[iParable].color[color],(y/strength),parableArray[iParable].color[color] * (y/strength));
            }*/
        }
        for (int color = 0; color<3; color++) {
            // translate the integer into -1 to 1 positions.
            graph[color].data[x].y = colors[color]/((float)strength/2)-1;
        }
    }
    
        

        
        //graph[1].data[x].x = x/((float)(pixels-1)/2)-1;
        //graph[1].data[x].y = y/((float)strength/2)-1;
        // move the parable 
        //parableArray[iParable].position += parableArray[iParable].velocity * deltaTime;
    

}


void calcParableMixColors(pointArray* graph,Parable* parableArray,int amount, int pixels, int strength) {
    for (int x = 0; x < pixels; x++){
        float colors[3] = {0,0,0};
        float sumOfStrength = 0;
        for (int iParable = 0; iParable < amount; iParable++) {
            // calculate the parable simply  (y = a*(x-e)²+d)
            float xe = (float)x-parableArray[iParable].position+parableArray[iParable].velocity*wheel_val;
            float y = parableArray[iParable].factor_a*(xe*xe)+(float)parableArray[iParable].height;
            //
            if (y < 0) y = 0;
            else if (y > strength) y = strength;
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

void moveParables_time(Parable* parableArray, int amount,float deltaTime){
    for (int iParable = 0; iParable < amount; iParable++) {
        parableArray[iParable].position += parableArray[iParable].velocity *timeSpeed * deltaTime;
    }
}

/*void moveParables_wheel(Parable* parableArray, int amount) {
    for (int iParable = 0; iParable < amount; iParable++) {
        parableArray[iParable].position = parableArray[iParable].velocity * wheel_val;
    }
}*/

int main() {
    // glfw: initialize and configure
    // ------------------------------
    if (!glfwInit()) {
        printf("Couldn't initialise glfw");
        exit(EXIT_FAILURE);
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create main Window
    WindowData Window(SCR_WIDTH,SCR_HEIGHT, "Graph",NULL,NULL);
    GLFWwindow* window = Window.m_Window;
    if (window == NULL){
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
        //return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
        //return -1;
    } 
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetScrollCallback(window, scroll_callback);
    int width,height;
    glfwGetFramebufferSize(window, &width, &height);
    
    //int graphSize = 2000;
    //point graph[graphSize];

    /*
    for (int i = 0; i < graphSize; i++) {
        float x = (i - graphSize/2) / 100.0;
        graph[i].x = x;
        graph[i].y = sin(x * 10.0) / (1.0 + x * x);
    }*/ /*
    for (int i = 0; i < graphSize; i++) {
        float x = (i - (graphSize/2)) / 1000.0;
        graph[i].x = x;
        graph[i].y = sin(x * 10.0) / (1.0 + x * x);
    }*/
    /*
    float axis_x_offset_translated = (float)axis_x_offset/width*2-1.;
    float axis_y_offset_translated = (float)axis_y_offset/height*2-1.;

    offset_x = (axis_x_offset_translated+1);
    offset_y = (axis_y_offset_translated+1);
    //offset_x = axis_x_offset_translated;
    //offset_y = axis_x_offset_translated;

    scale_x = 1. - offset_x;
    scale_y = 1. - offset_y;
    */
    //offset_x = (float)axis_x_offset/width*2

    

    /*
    point axis[4];printf("offset x,y : %f,%f\n",offset_x,offset_y);is_y_offset_translated);

    point Xrays[255*2];
    Window.XRays = Xrays;
    float Xlength = (float)axis_Line_Length/width;
    calcXAxisRays(Xrays,axis_x_offset_translated,axis_y_offset_translated,255,Xlength);
    //printPlot(Window.Axis,4);
    //printPlot(Xrays,255*2);

    point Yrays[200*2];
    float Ylength = (float)axis_Line_Length/height;
    calcYAxisRays(Yrays,axis_x_offset_translated,axis_y_offset_translated,200,Ylength);
    printPlot(Yrays,200*2);
    //printPlot(graph,graphSize);
    */
       //-------Shaders--------
    Shader axisShader("shader/Axis.vs","shader/Axis.fs");
    Shader plotShader("shader/plot.vs","shader/plot.fs");

    GraphBody graphBody(&axisShader,400,255,width,height);
    float colors[3][3] = {{1,0,0},{0,1,0},{0,0,1}};
    PlotData plotData(&plotShader, 400, 255, 3,colors);

    //somefunc(plotData.plotArray[2].data,plotData.pixels,plotData.strength);
    //printPlot(plotData.plotArray[2].data,plotData.plotArray[2].size);
    //plotData.updateData(2);

    //somefunc2(plotData.plotArray[1].data,plotData.pixels,plotData.strength);
    //plotData.updateData(1);

    unsigned int vbo[3],vao[3];
    Window.graphBody = &graphBody;
    Window.graphBody->generateData(width,height);
    printf("offset x,y : %f,%f\n",offset_x,offset_y);
    // each thing has 4 bytes and there are 7 of it. 28
    int parableAmount = 2;
    Parable parable[200] = {
        {10,255,-0.4,0,{0,0,255}},
        {-10,255,-0.4,400,{255,50,0}}
    };

    printf("yees: %i\n",parableAmount);
    for (int i = 0; i<parableAmount; i++) {
        updateDeleteBorder(parable[i]);
    }
    /*
    Parable parable[3] = {
        {10,255,-0.2,0,{255,50,0}},
        {10,255,-0.2,200,{22,50,200}},
        {-10,255,-0.2,400,{200,0,5}}
    };*/

    /*
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    */

    /*
    glGenVertexArrays(3,vao);
    glGenBuffers(3, vbo);
    
    
    glBindVertexArray(vao[0]);
    glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(graph), graph, GL_STATIC_DRAW);
    glVertexAttribPointer(
        0,                   // attribute
        2,                   // number of elements per vertex, here (x,y)
        GL_FLOAT,            // the type of each element
        GL_FALSE,            // take our values as-is
        0,                   // no space between values
        (void*)0              // use the vertex buffer object
    );
    glEnableVertexAttribArray(0);
    */

    /*
    glBindVertexArray(Window.VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER,vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);


    //printf("sizeof rays: %i\n",sizeof(axis));
    glBindVertexArray(Window.VAO[2]);
    float deleteBorder;
    glBindBuffer(GL_ARRAY_BUFFER,vbo[2]);
    printf("siteof Xrays: %i\n",sizeof(Yrays));
    glBufferData(GL_ARRAY_BUFFER, sizeof(Xrays), Xrays, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glBindVertexArray(Window.VAO[3]);
    glBindBuffer(GL_ARRAY_BUFFER,vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Yrays), Yrays, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    */

    int nbFrames = 0;
    double lastTime_FPS = glfwGetTime();
    float lastFrame = 0.0f; // Time of last frame
    using ms = std::chrono::duration<float, std::milli>;
    float deltaTime2;
    //printf("VAOData 1,2: %i,%i\n",vao[1],Window.VAO[1]);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glfwSwapInterval(0);
    while(!glfwWindowShouldClose(window)) {
        auto start = std::chrono::system_clock::now();
        double currentTime = glfwGetTime();


        deltaTime = currentTime - lastFrame;
        lastFrame = currentTime;
        // FPS
        // ---
        nbFrames++;
        if ( currentTime - lastTime_FPS >= 1.0 ){ // If last printf() was more than 1 sec ago
            // printf and reset timer
            printf("%f ms/frame, %dFPS\n", 1000.0/(double) nbFrames,nbFrames);
            nbFrames = 0;
            lastTime_FPS += 1.0;
        }
        get_width(parable[1]);
        // input
        // -----
        processInput(window);

        // rendering commands
        // ------------------
        glClearColor(1,1,1,1.0f);
        //glClearColor(0.2f,0.2f,0.2f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwGetFramebufferSize(window, &width, &height);
        
        graphBody.drawGrid();
        
        //printf("time: %f\n",deltaTime2);


        //printf("sizeof parable: %i\n",sizeof(parable)/28);
        //calcParable(plotData.plotArray, parable, sizeof(parable)/28, plotData.pixels, plotData.strength);
        calcParableMixColors(plotData.plotArray, parable, parableAmount, plotData.pixels, plotData.strength);
        //printf("time: %lf\n",currentTime);
        //printf("latest x: %f at %i\n",plotData.plotArray[0].data[199].x,plotData.pixels);
        //plotData.updateData(0);
        //printPlot(plotData.plotArray[0].data,plotData.plotArray[0].size);
        plotData.updateAllData();
        plotData.draw();

        moveParables_time(parable, parableAmount,deltaTime);

        /*
        plotShader.use();
        plotShader.setFloat("offset_x",offset_x);
        plotShader.setFloat("scale_x",scale_x);
        plotShader.setFloat("offset_y",offset_y);
        plotShader.setFloat("scale_y",scale_y);
        plotShader.setFloat("wheel_scale_x",wheel_scale_x);
        plotShader.setVec3("color",0,0,1);

        
        glBindVertexArray(vao[0]);
        //glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        
        glDrawArrays(GL_LINE_STRIP, 0, 2000);
        //glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
        */
        
        graphBody.draw();
        /*
        float axis_x_offset_translated = (float)axis_x_offset/width-1.;
        float axis_y_offset_translated = (float)axis_y_offset/height-1.;

        offset_x = (axis_x_offset_translated+1)/2;
        offset_y = (axis_y_offset_translated+1)/2;


        scale_x = 1. - offset_x;
        scale_y = 1. - offset_y;

        
        calcAxis(axis,axis_x_offset_translated,axis_y_offset_translated);
        printPlot(axis,4);*/
        
        /*
        axisShader.use();
        axisShader.setFloat("offset_x",offset_x);
        axisShader.setFloat("scale_x",scale_x);
        axisShader.setVec3("color",1.0f,1,1);
        
        glBindVertexArray(graphBody.VAOs[0]);
        //glBindBuffer(GL_ARRAY_BUFFER,vbo[1]);
        //glBufferSubData(GL_ARRAY_BUFFER,0,32,axis);
        glDrawArrays(GL_LINES, 0, 4);
        
        glBindVertexArray(graphBody.VAOs[1]);
        glDrawArrays(GL_LINES, 0, graphBody.xRibble_size);

        glBindVertexArray(graphBody.VAOs[2]);
        glDrawArrays(GL_LINES, 0, graphBody.yRibble_size);
        */
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
        auto stop = std::chrono::system_clock::now();
        deltaTime2 = std::chrono::duration_cast<ms>(stop - start).count();

    }


    glfwTerminate();
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
        std::cout << timeSpeed << std::endl;}
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        timeSpeed -= speed*deltaTime;
        std::cout << timeSpeed << std::endl;}

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0,0,width,height);
    WindowData* Window = reinterpret_cast<WindowData*>(glfwGetWindowUserPointer(window));
    Window->graphBody->generateData(width,height);
    Window->graphBody->updateAxis();
    /*
    float axis_x_offset_translated = (float)axis_x_offset/width-1.;
    float axis_y_offset_translated = (float)axis_y_offset/height-1.;

    offset_x = (axis_x_offset_translated+1)/2;
    offset_y = (axis_y_offset_translated+1)/2;


    scale_x = 1. - offset_x;
    scale_y = 1. - offset_y;

    
    point* axis = Window->Axis;
    calcAxis(axis,axis_x_offset_translated,axis_y_offset_translated);
    printPlot(axis,4);
    glBindVertexArray(Window->VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER,Window->VBO[1]);
    glBufferSubData(GL_ARRAY_BUFFER,0,32,axis);
    */
    /*
    glBindVertexArray(vao[1]);
    glBindBuffer(GL_ARRAY_BUFFER,vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axis), axis, GL_DYNAMIC_DRAW);*/
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    /*
    if (wheel_scale_x <1.0f)
        wheel_scale_x -= (float)yoffset/300;
    else
        wheel_scale_x -= (float)yoffset/100;
    
    if (wheel_scale_x < 0.02f)
        wheel_scale_x = 0.02f;
    if (wheel_scale_x > 10.0f)
        wheel_scale_x = 10.0f; */
    wheel_val += (float)yoffset/10;
    //printf("%f\n",wheel_scale_x);

}