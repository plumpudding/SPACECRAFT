#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "utils/Singleton.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#ifndef NTWBAR
    #include <AntTweakBar.h>
#endif
#include "render/Camera.h"
#include "render/camera/CameraKeyboardMouse.h"
#include "data/ContentHandler.h"
#include "ui/PlayerUI.h"

#define NUMPRODUCERS (3)

enum appState {
    appReady,
    appInLoop,
    appPaused,
    appExiting
};

class Application : public Singleton<Application> {
    private:
        Application();
        appState state;
        bool fullscreen, vsync, active, wireframe;
        #ifndef NTWBAR
            TwBar *bar;
        #endif
        unsigned int width, height;
        float viewWidth, viewHeight;
        GLFWwindow* window;
        Camera *camera;
        float bgColor[3];
        ContentHandler contentHandler;
        float deltaTime, time, fps;
        int fpsCounter;

        PlayerUI *ui;

        friend class Singleton<Application>;

        void glfwWindowHints();

    public:
        ~Application();
        void run();
        void loop();
        void createWindowInFullscreen(bool fs);
        //void resetVSync();
        inline void exit() { state = appExiting; }
        inline float getWindowRatio() const { return static_cast<float>(width)/static_cast<float>(height); }
        inline GLFWwindow* getWindow() { return window; }

        inline Camera& getCamera() { return *camera; }

        // elapsed time in seconds
        inline float getFrameDeltaTime() const { return deltaTime; }
    };

#endif
