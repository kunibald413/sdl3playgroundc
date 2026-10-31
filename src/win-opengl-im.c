#include <windows.h>
#include <stdio.h>
#include <math.h>

#include <GL/gl.h>

// --------------------------------------------
// 1. Define function pointer types for OpenGL & WGL
// --------------------------------------------
typedef HGLRC (WINAPI *PFN_wglCreateContext)(HDC);
typedef BOOL  (WINAPI *PFN_wglMakeCurrent)(HDC, HGLRC);
typedef BOOL  (WINAPI *PFN_wglDeleteContext)(HGLRC);
typedef PROC  (WINAPI *PFN_wglGetProcAddress)(LPCSTR);

typedef void  (APIENTRY *PFN_glClearColor)(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void  (APIENTRY *PFN_glClear)(GLbitfield);
typedef void  (APIENTRY *PFN_glBegin)(GLenum);
typedef void  (APIENTRY *PFN_glEnd)(void);
typedef void  (APIENTRY *PFN_glVertex2f)(GLfloat, GLfloat);
typedef void  (APIENTRY *PFN_glColor3f)(GLfloat, GLfloat, GLfloat);
typedef void  (APIENTRY *PFN_glViewport)(GLint, GLint, GLsizei, GLsizei);
typedef void  (APIENTRY *PFN_glMatrixMode)(GLenum);
typedef void  (APIENTRY *PFN_glLoadIdentity)(void);
typedef void  (APIENTRY *PFN_glRotatef)(GLfloat, GLfloat, GLfloat, GLfloat);
typedef void  (APIENTRY *PFN_glFlush)(void);

typedef const GLubyte* (APIENTRY *PFN_glGetString)(GLenum);

// --------------------------------------------
// 2. Our global function pointers (like Quake's qgl*!)
// --------------------------------------------
PFN_wglCreateContext     my_wglCreateContext;
PFN_wglMakeCurrent       my_wglMakeCurrent;
PFN_wglDeleteContext     my_wglDeleteContext;

PFN_glClearColor         my_glClearColor;
PFN_glClear              my_glClear;
PFN_glBegin              my_glBegin;
PFN_glEnd                my_glEnd;
PFN_glVertex2f           my_glVertex2f;
PFN_glColor3f            my_glColor3f;
PFN_glViewport           my_glViewport;
PFN_glMatrixMode         my_glMatrixMode;
PFN_glLoadIdentity       my_glLoadIdentity;
PFN_glRotatef            my_glRotatef;
PFN_glFlush              my_glFlush;
PFN_glGetString          my_glGetString;

HMODULE g_hOpenGL = NULL;
HWND    g_hWnd = NULL;
HDC     g_hDC = NULL;
HGLRC   g_hRC = NULL;

// --------------------------------------------
// 3. Load opengl32.dll and grab all functions
// --------------------------------------------
int LoadOpenGL(void) {
    // Just like Quake: LoadLibrary("opengl32")
    g_hOpenGL = LoadLibraryA("opengl32.dll");
    if (!g_hOpenGL) {
        printf("Failed to load opengl32.dll! Error: %lu\n", GetLastError());
        return 0;
    }

    // Grab WGL functions (Windows GL - context management)
    my_wglCreateContext = (PFN_wglCreateContext)GetProcAddress(g_hOpenGL, "wglCreateContext");
    my_wglMakeCurrent   = (PFN_wglMakeCurrent)GetProcAddress(g_hOpenGL, "wglMakeCurrent");
    my_wglDeleteContext = (PFN_wglDeleteContext)GetProcAddress(g_hOpenGL, "wglDeleteContext");

    // Grab Core OpenGL 1.1 functions (these are DIRECTLY exported by opengl32.dll!)
    my_glClearColor   = (PFN_glClearColor)GetProcAddress(g_hOpenGL, "glClearColor");
    my_glClear        = (PFN_glClear)GetProcAddress(g_hOpenGL, "glClear");
    my_glBegin        = (PFN_glBegin)GetProcAddress(g_hOpenGL, "glBegin");
    my_glEnd          = (PFN_glEnd)GetProcAddress(g_hOpenGL, "glEnd");
    my_glVertex2f     = (PFN_glVertex2f)GetProcAddress(g_hOpenGL, "glVertex2f");
    my_glColor3f      = (PFN_glColor3f)GetProcAddress(g_hOpenGL, "glColor3f");
    my_glViewport     = (PFN_glViewport)GetProcAddress(g_hOpenGL, "glViewport");
    my_glMatrixMode   = (PFN_glMatrixMode)GetProcAddress(g_hOpenGL, "glMatrixMode");
    my_glLoadIdentity = (PFN_glLoadIdentity)GetProcAddress(g_hOpenGL, "glLoadIdentity");
    my_glRotatef      = (PFN_glRotatef)GetProcAddress(g_hOpenGL, "glRotatef");
    my_glFlush        = (PFN_glFlush)GetProcAddress(g_hOpenGL, "glFlush");
    my_glGetString    = (PFN_glGetString)GetProcAddress(g_hOpenGL, "glGetString");

    // Check if we got everything (optional)
    if (!my_wglCreateContext || !my_glClear) {
        printf("Failed to get function pointers!\n");
        return 0;
    }

    printf("Successfully loaded opengl32.dll and all functions!\n");
    return 1;
}

// --------------------------------------------
// 4. Window Procedure (handles resize and close)
// --------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE:
            if (my_glViewport) {
                int width = LOWORD(lParam);
                int height = HIWORD(lParam);
                my_glViewport(0, 0, width, height);
            }
            return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

// --------------------------------------------
// 5. Setup the window and OpenGL context
// --------------------------------------------
int CreateOpenGLWindow(int width, int height) {
    WNDCLASS wc = {0};
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = "MyOpenGLClass";

    if (!RegisterClass(&wc)) {
        printf("Failed to register window class!\n");
        return 0;
    }

    g_hWnd = CreateWindowEx(0, "MyOpenGLClass", "My Custom OpenGL (Loaded via LoadLibrary!)",
                            WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                            width, height, NULL, NULL, wc.hInstance, NULL);
    if (!g_hWnd) {
        printf("Failed to create window!\n");
        return 0;
    }

    // Get the Device Context
    g_hDC = GetDC(g_hWnd);
    if (!g_hDC) return 0;

    // Set the Pixel Format (required for OpenGL)
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR), 1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        16, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0
    };
    int pixelFormat = ChoosePixelFormat(g_hDC, &pfd);
    if (!SetPixelFormat(g_hDC, pixelFormat, &pfd)) {
        printf("Failed to set pixel format!\n");
        return 0;
    }

    // Create OpenGL Context using our dynamically loaded wglCreateContext!
    g_hRC = my_wglCreateContext(g_hDC);
    if (!g_hRC) {
        printf("Failed to create OpenGL context!\n");
        return 0;
    }

    // Make it current using our dynamically loaded wglMakeCurrent!
    if (!my_wglMakeCurrent(g_hDC, g_hRC)) {
        printf("Failed to make context current!\n");
        return 0;
    }

    // --------------------------------------------
    // CHECK WHAT WE ARE RUNNING ON!
    // --------------------------------------------
    const char* version = (const char*)my_glGetString(GL_VERSION);
    const char* renderer = (const char*)my_glGetString(GL_RENDERER);
    const char* vendor = (const char*)my_glGetString(GL_VENDOR);

    printf("\n===== OPENGL INFORMATION =====\n");
    printf("Vendor:   %s\n", vendor);
    printf("Renderer: %s\n", renderer);
    printf("Version:  %s\n", version);

    // Determine if it's GPU or CPU
    if (strstr(vendor, "Microsoft") != NULL) {
        printf(">> STATUS: Using SOFTWARE (CPU) rendering! (GDI Generic)\n");
    } else if (strstr(renderer, "llvmpipe") != NULL || strstr(renderer, "Mesa") != NULL) {
        printf(">> STATUS: Using SOFTWARE (CPU) rendering via Mesa!\n");
    } else {
        printf(">> STATUS: Using HARDWARE (GPU) acceleration! (Your graphics card)\n");
    }
    printf("===============================\n\n");

    printf("OpenGL Context created and active!\n");
    ShowWindow(g_hWnd, SW_SHOW);
    return 1;
}

/*
 * ==========================================================
 * RENDER NOTE: Immediate Mode (glBegin/glEnd) vs. VBOs
 * ==========================================================
 * 
 * WHAT WE ARE USING:
 *   Immediate Mode (glBegin/glEnd) - OpenGL 1.1 style.
 *   We send vertices one-by-one from the CPU to the GPU every frame.
 * 
 * WHY WE USE IT HERE:
 *   - It requires ZERO modern extensions (just core opengl32.dll exports).
 *   - Minimal code needed to get a triangle on screen.
 *   - Perfect for learning the absolute basics.
 * 
 * WHY WE WOULD USE VBOS (Vertex Buffer Objects) INSTEAD:
 *   - Performance: Upload vertex data to GPU memory ONCE, not every frame.
 *   - Batching: Send thousands of triangles in a single call (glDrawArrays).
 *   - Modern Pipelines: Required for using Shaders (GLSL) and OpenGL 3.0+.
 * 
 * Immediate Mode is officially deprecated but remains the simplest
 * way to experiment with raw OpenGL calls.
 * ==========================================================
 */
// --------------------------------------------
// 6. Render a cool rotating triangle!
// --------------------------------------------
void Render(float angle) {
    // Clear the screen to a dark blue
    my_glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    my_glClear(GL_COLOR_BUFFER_BIT); // GL_COLOR_BUFFER_BIT = 0x4000

    // Set up the matrix
    my_glMatrixMode(GL_PROJECTION); // GL_PROJECTION = 0x1701
    my_glLoadIdentity();
    my_glMatrixMode(GL_MODELVIEW); // GL_MODELVIEW = 0x1700
    my_glLoadIdentity();

    // Rotate based on time
    my_glRotatef(angle, 0.0f, 0.0f, 1.0f);

    // Draw a colorful triangle
    my_glBegin(GL_TRIANGLES); // GL_TRIANGLES = 0x0004
        my_glColor3f(1.0f, 0.0f, 0.0f); // Red
        my_glVertex2f(-0.5f, -0.5f);
        
        my_glColor3f(0.0f, 1.0f, 0.0f); // Green
        my_glVertex2f(0.5f, -0.5f);
        
        my_glColor3f(0.0f, 0.0f, 1.0f); // Blue
        my_glVertex2f(0.0f, 0.5f);
    my_glEnd();

    // Swap the front and back buffers (SwapBuffers is from GDI32, so we just call it normally)
    SwapBuffers(g_hDC);
}

// --------------------------------------------
// 7. Main entry point
// --------------------------------------------
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrev, LPSTR lpCmdLine, int nCmdShow) {
    // Step 1: Load the DLL and get function pointers
    if (!LoadOpenGL()) {
        return 1;
    }

    // Step 2: Create a window and OpenGL context
    if (!CreateOpenGLWindow(640, 480)) {
        return 1;
    }

    // Step 3: Main message loop
    MSG msg = {0};
    float angle = 0.0f;
    while (1) {
        // Check for window messages (close, resize, etc.)
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            // Step 4: Render loop! 
            angle += 0.5f; // Rotate 0.5 degree per frame
            if (angle > 360.0f) angle -= 360.0f;
            Render(angle);

            // A little sleep to save CPU
            Sleep(1000/60); // ~60 FPS
        }
    }

    // Step 5: Cleanup
    my_wglMakeCurrent(NULL, NULL);
    my_wglDeleteContext(g_hRC);
    ReleaseDC(g_hWnd, g_hDC);
    DestroyWindow(g_hWnd);
    FreeLibrary(g_hOpenGL);

    return 0;
}