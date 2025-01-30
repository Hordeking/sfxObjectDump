#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <shellapi.h>
#include <gl/GL.h>
#include <algorithm>
#include <cassert>

#include "linmath.h"
#include "Graphics.h"
#include "SFXObject.h"

struct MyVertex {
    float x, y, z;
    float nx, ny, nz;
    float s, t, type;
};

struct Model {
    char* romPath;
    SFXObject* sfxObject;
    std::ifstream* romFile;

    MyVertex* vertices;
    int numFaces;
    int numLines;
    float maxExtent;

    int current;
    int frame;
    int tick;
    int dist;
    float rotation;
};

// Global Variables:
HINSTANCE hInst;
WCHAR szTitle[] = L"SuperFX Viewer";
WCHAR szWindowClass[] = L"SUPERFXVIEWER";
BOOL shouldExit = FALSE;
Model model;
GLuint program, quadProgram;
GLuint vertexBuffer, textBuffer;
GLint modelLocation, viewOrthoLocation, quadMVPLocation, diffuseLocation, paletteLocation;
GLuint vao, quadVao, textVao;
GLuint texture, fboTexture, paletteTexture, fontTexture;
GLuint fbo;
MyVertex textVertices[40 * 30 * 6];

typedef char GLchar;
typedef unsigned long long GLsizeiptr;

#define GL_ARRAY_BUFFER 0x8892
#define GL_VERTEX_SHADER 0x8B31
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_COMPILE_STATUS 0x8B81
#define GL_LINK_STATUS 0x8B82
#define GL_STATIC_DRAW 0x88E4
#define GL_DYNAMIC_DRAW 0x88E8
#define GL_FRAMEBUFFER 0x8D40
#define GL_RENDERBUFFER 0x8D41
#define GL_DEPTH24_STENCIL8 0x88F0
#define GL_DEPTH_STENCIL_ATTACHMENT 0x821A
#define GL_COLOR_ATTACHMENT0 0x8CE0
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#define GL_TEXTURE0 0x84C0

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB             0x2093
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126

typedef HGLRC(*WGLCREATECONTEXTATTRIBSARBPROC)(HDC hDC, HGLRC hshareContext, const int* attribList);
WGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB;
typedef GLuint (*GLCREATESHADERPROC)(GLenum shaderType);
GLCREATESHADERPROC glCreateShader;
typedef void (*GLSHADERSOURCEPROC)(GLuint shader, GLsizei count, const GLchar** string, const GLint* length);
GLSHADERSOURCEPROC glShaderSource;
typedef void (*GLCOMPILESHADERPROC)(GLuint shader);
GLCOMPILESHADERPROC glCompileShader;
typedef void (*GLGETSHADERINFOLOGPROC)(GLuint shader, GLsizei maxLength, GLsizei* length, GLchar* infoLog);
GLGETSHADERINFOLOGPROC glGetShaderInfoLog;
typedef void (*GLGETSHADERIVPROC)(GLuint shader, GLenum pname, GLint* params);
GLGETSHADERIVPROC glGetShaderiv;
typedef GLuint(*GLCREATEPROGRAMPROC)(void);
GLCREATEPROGRAMPROC glCreateProgram;
typedef void (*GLATTACHSHADERPROC)(GLuint program, GLuint shader);
GLATTACHSHADERPROC glAttachShader;
typedef void (*GLLINKPROGRAMPROC)(GLuint);
GLLINKPROGRAMPROC glLinkProgram;
typedef void (*GLGETPROGRAMIVPROC)(GLuint program, GLenum pname, GLint* params);
GLGETPROGRAMIVPROC glGetProgramiv;
typedef GLint (*GLGETUNIFORMLOCATIONPROC)(GLuint program, const GLchar* name);
GLGETUNIFORMLOCATIONPROC glGetUniformLocation;
typedef void (*GLUNIFORMMATRIX4FVPROC)(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
GLUNIFORMMATRIX4FVPROC glUniformMatrix4fv;
typedef void (*GLUNIFORM1IPROC)(GLint location, GLint v0);
GLUNIFORM1IPROC glUniform1i;
typedef void (*GLUSEPROGRAMPROC)(GLuint);
GLUSEPROGRAMPROC glUseProgram;
typedef void (*GLGENBUFFERSPROC)(GLsizei n, GLuint* buffers);
GLGENBUFFERSPROC glGenBuffers;
typedef void (*GLBINDBUFFERPROC)(GLenum, GLuint);
GLBINDBUFFERPROC glBindBuffer;
typedef void (*GLBUFFERDATAPROC)(GLenum target, GLsizeiptr size, const void* data, GLenum usage);
GLBUFFERDATAPROC glBufferData;
typedef void (*GLGENVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);
GLGENVERTEXARRAYSPROC glGenVertexArrays;
typedef void (*GLBINDVERTEXARRAYPROC)(GLuint);
GLBINDVERTEXARRAYPROC glBindVertexArray;
typedef void (*GLENABLEVERTEXATTRIBARRAYPROC)(GLuint);
GLENABLEVERTEXATTRIBARRAYPROC glEnableVertexAttribArray;
typedef void (*GLVERTEXATTRIBPOINTERPROC)(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void* pointer);
GLVERTEXATTRIBPOINTERPROC glVertexAttribPointer;
typedef void (*GLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* ids);
GLGENFRAMEBUFFERSPROC glGenFramebuffers;
typedef void (*GLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
GLBINDFRAMEBUFFERPROC glBindFramebuffer;
typedef void (*GLGENRENDERBUFFERSPROC)(GLsizei n, GLuint* renderbuffers);
GLGENRENDERBUFFERSPROC glGenRenderbuffers;
typedef void (*GLBINDRENDERBUFFERPROC)(GLenum target, GLuint renderbuffer);
GLBINDRENDERBUFFERPROC glBindRenderbuffer;
typedef void (*GLRENDERBUFFERSTORAGEPROC)(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
GLRENDERBUFFERSTORAGEPROC glRenderbufferStorage;
typedef void (*GLFRAMEBUFFERRENDERBUFFERPROC)(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
GLFRAMEBUFFERRENDERBUFFERPROC glFramebufferRenderbuffer;
typedef void (*GLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2D;
typedef GLenum (*GLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
GLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatus;
typedef void (*GLACTIVETEXTUREPROC)(GLenum texture);
GLACTIVETEXTUREPROC glActiveTexture;

const GLchar* vertexShaderSource =
"#version 130\n"
"uniform mat4 viewOrtho;\n"
"uniform mat4 model;\n"
"in vec3 vPos;\n"
"in vec3 vNormal;\n"
"in vec3 vMaterial;\n"
"out vec3 material;\n"
"out float tOffset;"
"out float tDither;"
"void main()\n"
"{\n"
"    float type = vMaterial.p;\n"
"    if (type == 0.0) { // 0 = normal lit\n"
"        tOffset = floor( max(dot(normalize(mat3(model) * vNormal), vec3(-0.577, -0.577, 0.577)), 0.0) * 10.0 ) * 1.0/256.0;\n"
"        tDither = 1.0 / 512.0;\n"
"    } else if (type == 1.0) { // 1 = flat lit\n"
"        tOffset = 0.0;\n"
"        tDither = 1.0 / 512.0;\n"
"    } else { // 2 = textured\n"
"        tOffset = 0.0;\n"
"        tDither = 0.0;\n"
"    }\n"
"    material = vMaterial;\n"
"    gl_Position = viewOrtho * model * vec4(vPos, 1.0);\n"
"}\n";

const GLchar* fragmentShaderSource =
"#version 130\n"
"uniform sampler2D diffuse;\n"
"uniform sampler2D palette;\n"
"in vec3 material;\n"
"in float tOffset;\n"
"in float tDither;\n"
"void main()\n"
"{\n"
"    float dither = (int(gl_FragCoord.x + gl_FragCoord.y) & 1) * tDither;\n"
"    float color = texture(diffuse, vec2(material.s + tOffset + dither, material.t)).r;\n"
"    if (color == 0.0) discard;\n"
"    gl_FragColor = texture(palette, vec2(color + 1.0/32.0, 0.5));\n"
"}\n";

const GLchar* quadVertexShaderSource =
"#version 130\n"
"uniform mat4 MVP;\n"
"in vec3 vPos;\n"
"in vec3 vUv;\n"
"out vec3 uv;\n"
"void main()\n"
"{\n"
"    gl_Position = MVP * vec4(vPos, 1.0);\n"
"    uv = vUv;\n"
"}\n";

const GLchar* quadFragmentShaderSource =
"#version 130\n"
"uniform sampler2D diffuse;\n"
"in vec3 uv;\n"
"void main()\n"
"{\n"
//"    gl_FragColor = mod(gl_FragCoord.y, 2.0) * texture2D(diffuse, uv.st);\n" // Scanline effect
"    gl_FragColor = texture2D(diffuse, uv.st);\n"
"}\n";

MyVertex quad[6] = {
    // Full-Screen Quad
    { -1.0f, -1.0f, 0.0f,  0.f, 0.f },
    {  1.0f, -1.0f, 0.0f,  1.f, 0.f },
    {  1.0f,  1.0f, 0.0f,  1.f, 1.f },

    { -1.0f, -1.0f, 0.0f,  0.f, 0.f },
    {  1.0f,  1.0f, 0.0f,  1.f, 1.f },
    { -1.0f,  1.0f, 0.0f,  0.f, 1.f },
};

void setMaterial(MyVertex* vertex, int material, int corner) {
    int type = (material >> 8) & 0xFF;
    int extra = material & 0xFF;

    if (type < 10) {
        // Face-normal color
        vertex->s = 0.75f + 1 / 1024.0;
        vertex->t = (extra + 4) * 1/256.0 + 1/512.0;
        vertex->type = 0.0f;
    } else if (type == 0x3E) {
        // Constant color
        vertex->s = 0.75f + extra * (2.0f * 1 / 512.0) + 1 / 1024.0;
        vertex->t = 1.0/256.0 * model.dist + 1.0/512.0;
        vertex->type = 1.0f;
    } else if (type >= 0x40 && type <= 0x4F) {
        // Textured (Currently only supports 32x32 and *no* flipping neither hor. or vert.)

        GLuint address;
        bool hiPage = false;
        float offset = 0.0;

        // See http://web.archive.org/web/20050517170102/http://board.acmlm.org/thread.php?id=6876
        if (extra >= 0x80) {
            hiPage = true;
            extra -= 0x80;
            address = texture_offsets[extra];
            offset = 0.5;
        }
        else {
            address = texture_offsets[extra];
        }

        // See https://github.com/Hordeking/sfxObjectDump/issues/4#issuecomment-1936847741
        if (address > 0x130000) {
            address -= 0x130000;
        }
        else {
            address -= 0x128000;
        }
        
        int row = address / 256;
        int col = address % 256;

        float u = col / 512.0 + offset;
        float v = 1.0 - row / 256.0;

        // Corners are top-left clockwise
        if (corner == 3) {
            v -= 32.0 / 256.0;
        }
        else if (corner == 2) {
            u += 32.0 / 512.0;
            v -= 32.0 / 256.0;
        }
        else if (corner == 1) {
            u += 32.0 / 512.0;
        }

        // TODO: Need to calculate face normal to offset vertices to prevent z-fighting (e.g. see Tall_Building_With)
        vertex->s = u;
        vertex->t = v;
        vertex->type = 2.0f;
    }
    else {
        // HACK: for now, set animated and other types to red
        setMaterial(vertex, 0x3E0b, 0);
    }
}

void Print(int r, int c, const std::string& str) {
    for (const char& chr : str) {
        char letter = chr;
        int offset = 0;

        if (islower(letter)) {
            letter -= 0x20;
            offset = 10 - 'A';
        }
        else if (isupper(letter)) {
            offset = 10 - 'A';
        }
        else if (isdigit(letter)) {
            letter -= '0';
        }
        else {
            letter = 47;
        }

        int cr = (letter + offset) / 16;
        int cc = (letter + offset) % 16;
        float s = cc / 16.0;
        float t = 1.0 - cr / 3.0;

        int index = r * 40 + c;

        MyVertex* vert = &textVertices[index * 6];
        vert->x = c / 20.0 - 1.0; 
        vert->y = r / 15.0 - 1.0; 
        vert->z = 0.0; 
        vert->s = s; vert->t = t - 0.3333; vert->type = 2.0;
        vert++;

        vert->x = c / 20.0 - 1.0 + 1.0 / 20.0; 
        vert->y = r / 15.0 - 1.0; 
        vert->z = 0.0; 
        vert->s = s + 0.0625; vert->t = t - 0.3333; vert->type = 2.0;
        vert++;

        vert = &textVertices[index * 6 + 2];
        vert->x = c / 20.0 - 1.0 + 1.0 / 20.0; vert->y = r / 15.0 - 1.0 + 1.0 / 15.0; vert->z = 0.0; vert->s = s + 0.0625; vert->t = t; vert->type = 2.0;
        vert = &textVertices[index * 6 + 3];
        vert->x = c / 20.0 - 1.0; vert->y = r / 15.0 - 1.0; vert->z = 0.0; vert->s = s; vert->t = t - 0.3333; vert->type = 2.0;
        vert = &textVertices[index * 6 + 4];
        vert->x = c / 20.0 - 1.0 + 1.0 / 20.0; vert->y = r / 15.0 - 1.0 + 1.0 / 15.0; vert->z = 0.0; vert->s = s + 0.0625; vert->t = t; vert->type = 2.0;
        vert = &textVertices[index * 6 + 5];
        vert->x = c / 20.0 - 1.0; vert->y = r / 15.0 - 1.0 + 1.0 / 15.0; vert->z = 0.0; vert->s = s; vert->t = t; vert->type = 2.0;

        c++;
    }

    glBindVertexArray(textVao);
    glBindBuffer(GL_ARRAY_BUFFER, textBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * 40 * 30 * 6, textVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)(sizeof(float) * 6));
}

void setVertex(const Vertex& vertex, MyVertex* current, uint16_t material, uint8_t corner) {
    float x, y, z;

    std::tie(x, y, z) = vertex.getFloatCoords();
    current->x = x;
    current->y = z;
    current->z = -y;

    setMaterial(current, material, corner);
}

void computeFaceNormal(MyVertex* v0, MyVertex* v1, MyVertex* v2) {
    vec3 v01 = { v1->x - v0->x, v1->y - v0->y, v1->z - v0->z };
    vec3 v02 = { v2->x - v0->x, v2->y - v0->y, v2->z - v0->z };
    vec3 result;
    vec3_mul_cross(result, v01, v02);
    vec3_norm(result, result);

    v0->nx = result[0];
    v0->ny = result[1];
    v0->nz = result[2];

    v1->nx = result[0];
    v1->ny = result[1];
    v1->nz = result[2];

    v2->nx = result[0];
    v2->ny = result[1];
    v2->nz = result[2];
}

void LoadModel() {
    if (!model.sfxObject->isValid()) {
        Print(1, 1, "Invalid Model                "); // Erase old name if longer
        return;
    }

    Print(1, 1, "                             "); // Erase old name if longer
    Print(1, 1, starfoxModels[model.current].name);
    Print(28, 1, "          ");
    Print(28, 1, "Frames " + std::to_string(model.sfxObject->frameCount()));
    Print(27, 1, "       ");
    Print(27, 1, "Dist " + std::to_string(model.dist));

    auto faces = model.sfxObject->getFaceList();
    std::sort(faces.begin(), faces.end(), [](const auto& lhs, const auto& rhs) -> bool {
        return lhs.nVerts > rhs.nVerts;
    });

    int numFaces = 0;
    int numLines = 0;
    for (const auto& face : faces) {
        if (face.nVerts >= 3) {
            numFaces += face.nVerts - 2;
        }
        else if (face.nVerts == 2) {
            numLines += 1;
        }
    }

    model.numFaces = numFaces;
    model.numLines = numLines;
    model.vertices = new MyVertex[numFaces * 3 + numLines * 2];
    MyVertex* current = model.vertices;

    const auto& vertexList = model.sfxObject->getVertexList(model.frame);

    float x, y, z;
    float maxExtent = 0.0f;
    for (const auto& vertex : vertexList) {
        std::tie(x, y, z) = vertex.getFloatCoords();
        if (fabs(x) > maxExtent)
            maxExtent = fabs(x);
        if (fabs(y) > maxExtent)
            maxExtent = fabs(y);
        if (fabs(z) > maxExtent)
            maxExtent = fabs(z);
    }
    model.maxExtent = maxExtent * 640.0f / 480.0f;

    for (const auto& face : faces) {
        int material = model.sfxObject->getMaterial(face.material);

        if (face.nVerts >= 3) {
            for (int i = 1; i < face.nVerts - 1; ++i) {
                float x, y, z;
                Vertex vertex = vertexList.at(face.vertex[0]);
                setVertex(vertex, current++, material, 0);
                vertex = vertexList.at(face.vertex[i]);
                setVertex(vertex, current++, material, i);
                vertex = vertexList.at(face.vertex[i + 1]);
                setVertex(vertex, current++, material, i + 1);

                if (material < 0x0A) {
                    computeFaceNormal(current - 3, current - 2, current - 1);
                }
            }
        }
        else if (face.nVerts == 2) {
            Vertex vertex = vertexList.at(face.vertex[0]);
            setVertex(vertex, current++, material, 0);
            vertex = vertexList.at(face.vertex[1]);
            setVertex(vertex, current++, material, 0);
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(MyVertex) * (model.numFaces * 3 + model.numLines * 2), model.vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    delete[] model.vertices;
}

void SwitchModel() {
    auto romfile = std::ifstream(model.romPath, std::ifstream::binary);
    delete model.sfxObject;
    model.sfxObject = new SFXObject(romfile, starfoxModels[model.current].vert_address, starfoxModels[model.current].face_address);
    model.frame = 0;
    LoadModel();
    romfile.close();
}

void UpdateAnimatedModel() {
    if (model.sfxObject == nullptr) return;

    model.rotation += 0.01f;

    model.tick++;
    int previousFrame = model.frame;
    if (model.tick % 10 == 0) {
        model.frame++;
        model.frame = model.frame % model.sfxObject->frameCount();
    }
    if (model.frame != previousFrame) {
        LoadModel();
    }
}

void HandleInput(WPARAM key) {
    if (key == VK_ESCAPE) {
        PostQuitMessage(0);
        shouldExit = TRUE;
        return;
    }

    // Cycle through different models
    if (key == VK_RIGHT) {
        model.current++;
    }
    else if (key == VK_LEFT) {
        model.current--;
    }

    if (model.current < 0)
        model.current = num_models;
    else if (model.current >= num_models)
        model.current = 0;

    if (key == VK_RIGHT || key == VK_LEFT) {
        SwitchModel();
    }

    // Change Distance (constant color attenuation)
    if (key == 0x44 /* D key */) {
        model.dist++;
        model.dist = model.dist % 4;

        // Currently hard-coded into each vertex, but *should* be set by shader uniform
        SwitchModel();
    }
}

void Render() {
    glClearColor(90.0f / 255.0f, 156.0f / 255.0f, 123.0f / 255.0f, 0.0f);

    GLfloat aspect = 640.0f / 480.0f;
    mat4x4 m, view, ortho, temp, temp2;
    //mat4x4_ortho(p, -0.5 * aspect, 0.5 * aspect, -0.5, 0.5, -1, 100);
    mat4x4_ortho(ortho, -model.maxExtent * aspect, model.maxExtent * aspect, -model.maxExtent, model.maxExtent, -1, 100);
    vec3 eye = { -5, -5, 5 };
    vec3 center = { 0, 0, 0 };
    vec3 up = { 0, 0, 1 };
    mat4x4_look_at(view, eye, center, up);
    mat4x4_identity(m);
    mat4x4_rotate_Z(m, m, model.rotation);
    mat4x4_mul(temp, ortho, view);
    //mat4x4_mul(temp2, temp, m);

    glEnable(GL_DEPTH_TEST);

    glUseProgram(program);
    glUniformMatrix4fv(viewOrthoLocation, 1, GL_FALSE, (GLfloat*)temp);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (GLfloat*)m);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(diffuseLocation, 0);
    glActiveTexture(GL_TEXTURE0 + 1);
    glBindTexture(GL_TEXTURE_2D, paletteTexture);
    glUniform1i(paletteLocation, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 320, 240);
    glBindVertexArray(vao);
    if (model.numFaces > 0)
        glDrawArrays(GL_TRIANGLES, 0, model.numFaces * 3);
    if (model.numLines > 0)
        glDrawArrays(GL_LINES, model.numFaces * 3, model.numLines * 2);

    glDisable(GL_DEPTH_TEST);

    mat4x4_identity(m);
    mat4x4_identity(temp);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glUniform1i(diffuseLocation, 0);
    glUniformMatrix4fv(viewOrthoLocation, 1, GL_FALSE, (GLfloat*)temp);
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, (GLfloat*)m);
    glBindVertexArray(textVao);
    glDrawArrays(GL_TRIANGLES, 0, 40 * 30 * 6);

    glUseProgram(quadProgram);
    mat4x4_identity(temp);
    glUniformMatrix4fv(quadMVPLocation, 1, GL_FALSE, (GLfloat*)temp);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glUniform1i(diffuseLocation, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glViewport(0, 0, 640, 480);
    glBindVertexArray(quadVao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void CreateOpenGLContext(HWND hWnd) {
    PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Depth
        8,                    // Stencil
        0,                    // Aux
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    HDC hdc = GetDC(hWnd);

    int  pixelFormat;
    pixelFormat = ChoosePixelFormat(hdc, &pfd);
    SetPixelFormat(hdc, pixelFormat, &pfd);

    HGLRC context = wglCreateContext(hdc);
    wglMakeCurrent(hdc, context);

    wglCreateContextAttribsARB = (WGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

    wglMakeCurrent(hdc, NULL);
    wglDeleteContext(context);
    context = NULL;

    int iContextAttribs[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
        WGL_CONTEXT_MINOR_VERSION_ARB, 2,
        WGL_CONTEXT_FLAGS_ARB, 0, // 0x0001 | 0x0002, for debug + forward compat
        0
    };
    context = wglCreateContextAttribsARB(hdc, 0, iContextAttribs);
    wglMakeCurrent(hdc, context);

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    glCreateShader = (GLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
    glShaderSource = (GLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
    glCompileShader = (GLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
    glGetShaderInfoLog = (GLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
    glGetShaderiv = (GLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");
    glCreateProgram = (GLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
    glAttachShader = (GLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
    glLinkProgram = (GLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
    glGetProgramiv = (GLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
    glUseProgram = (GLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGenBuffers = (GLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
    glBindBuffer = (GLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
    glBufferData = (GLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
    glEnableVertexAttribArray = (GLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
    glVertexAttribPointer = (GLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
    glGetUniformLocation = (GLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
    glUniformMatrix4fv = (GLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
    glUniform1i = (GLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
    glGenVertexArrays = (GLGENVERTEXARRAYSPROC)wglGetProcAddress("glGenVertexArrays");
    glBindVertexArray = (GLBINDVERTEXARRAYPROC)wglGetProcAddress("glBindVertexArray");
    glGenFramebuffers = (GLGENFRAMEBUFFERSPROC)wglGetProcAddress("glGenFramebuffers");
    glBindFramebuffer = (GLBINDFRAMEBUFFERPROC)wglGetProcAddress("glBindFramebuffer");
    glGenRenderbuffers = (GLGENRENDERBUFFERSPROC)wglGetProcAddress("glGenRenderbuffers");
    glBindRenderbuffer = (GLBINDRENDERBUFFERPROC)wglGetProcAddress("glBindRenderbuffer");
    glRenderbufferStorage = (GLRENDERBUFFERSTORAGEPROC)wglGetProcAddress("glRenderbufferStorage");
    glFramebufferRenderbuffer = (GLFRAMEBUFFERRENDERBUFFERPROC)wglGetProcAddress("glFramebufferRenderbuffer");
    glFramebufferTexture2D = (GLFRAMEBUFFERTEXTURE2DPROC)wglGetProcAddress("glFramebufferTexture2D");
    glCheckFramebufferStatus = (GLCHECKFRAMEBUFFERSTATUSPROC)wglGetProcAddress("glCheckFramebufferStatus");
    glActiveTexture = (GLACTIVETEXTUREPROC)wglGetProcAddress("glActiveTexture");

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, (const GLchar**) &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    GLint status;
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar log[256];
        glGetShaderInfoLog(vertexShader, 255, NULL, log);
        assert(false);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, (const GLchar**)&fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar log[256];
        glGetShaderInfoLog(fragmentShader, 255, NULL, log);
        assert(false);
    }

    GLuint quadVertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(quadVertexShader, 1, (const GLchar**)&quadVertexShaderSource, NULL);
    glCompileShader(quadVertexShader);
    glGetShaderiv(quadVertexShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar log[256];
        glGetShaderInfoLog(quadVertexShader, 255, NULL, log);
        assert(false);
    }

    GLuint quadFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(quadFragmentShader, 1, (const GLchar**)&quadFragmentShaderSource, NULL);
    glCompileShader(quadFragmentShader);
    glGetShaderiv(quadFragmentShader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        GLchar log[256];
        glGetShaderInfoLog(quadFragmentShader, 255, NULL, log);
        assert(false);
    }

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        assert(false);
    }

    modelLocation = glGetUniformLocation(program, "model");
    viewOrthoLocation = glGetUniformLocation(program, "viewOrtho");
    diffuseLocation = glGetUniformLocation(program, "diffuse");
    paletteLocation = glGetUniformLocation(program, "palette");

    quadProgram = glCreateProgram();
    glAttachShader(quadProgram, quadVertexShader);
    glAttachShader(quadProgram, quadFragmentShader);
    glLinkProgram(quadProgram);
    glGetProgramiv(quadProgram, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        assert(false);
    }
    quadMVPLocation = glGetUniformLocation(quadProgram, "MVP");

    glGenVertexArrays(1, &textVao);
    glGenBuffers(1, &textBuffer);

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)(sizeof(float) * 3));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)(sizeof(float) * 6));

    glGenVertexArrays(1, &quadVao);
    glBindVertexArray(quadVao);
    GLuint quadBuffer;
    glGenBuffers(1, &quadBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, quadBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(MyVertex), (void*)(sizeof(float) * 3));

    glGenTextures(1, &paletteTexture);
    glBindTexture(GL_TEXTURE_2D, paletteTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 16, 1, 0, GL_RGB, GL_UNSIGNED_BYTE, (GLvoid*)palette);

    glGenTextures(1, &fontTexture);
    glBindTexture(GL_TEXTURE_2D, fontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    for (int i = 0; i < 128 * 24; ++i)
        debug_font[i] *= 0x10 * 0x0E;
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 128, 24, 0, GL_RED, GL_UNSIGNED_BYTE, (GLvoid*)debug_font);

    GLubyte* texture_out = new GLubyte[512 * 256];
    for (int i = 0; i < 256 * 256; ++i) {
        GLubyte pair = texture_data[i];
        GLubyte hi = pair >> 4;
        GLubyte lo = pair & 0x0F;
        int row = 255 - i / 256; // Skip the 4 rows of "flat" colors
        int col = i % 256;
        // Lo Page
        texture_out[row * 512 + col]       = lo * 0x10;
        // Hi Page
        texture_out[row * 512 + col + 256] = hi * 0x10;
    }
    // HACK: Place the flat and shader color map into the slot machine texture ¯\_(ツ)_/¯
    // Constant (attenuates w/ distance, but this needs to be set in a shader uniform)
    for (int i = 0; i < 128; ++i) {
        GLubyte pair = constant_colors[i];
        GLubyte hi = pair >> 4;
        GLubyte lo = pair & 0x0F;
        int row = i / 32;
        int col = i % 32 + 192;
        texture_out[row * 512 + col * 2] = lo * 0x10;
        texture_out[row * 512 + col * 2 + 1] = hi * 0x10;
    }
    // Face-Normal Shaded
    for (int i = 0; i < 100; ++i) {
        GLubyte pair = flat_shaded_ramps[i];
        GLubyte hi = pair >> 4;
        GLubyte lo = pair & 0x0F;
        int row = i / 10 + 4; // Skip the 4 rows of constant colors
        int col = i % 10 + 192;
        texture_out[row * 512 + col * 2] = lo * 0x10;
        texture_out[row * 512 + col * 2 + 1] = hi * 0x10;
    }
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, 512, 256, 0, GL_RED, GL_UNSIGNED_BYTE, (GLvoid*)texture_out);

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    GLuint rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 320, 240);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glGenTextures(1, &fboTexture);
    glBindTexture(GL_TEXTURE_2D, fboTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 320, 240, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fboTexture, 0);
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    assert(status == GL_FRAMEBUFFER_COMPLETE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    assert(glGetError() == GL_NO_ERROR);

    // Global State
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        CreateOpenGLContext(hWnd);
        break;
    case WM_KEYDOWN:
        HandleInput(wParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        shouldExit = TRUE;
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

ATOM MyRegisterClass(HINSTANCE hInstance) {
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = NULL;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = NULL;

    return RegisterClassExW(&wcex);
}

HWND InitInstance(HINSTANCE hInstance, int nCmdShow) {
    hInst = hInstance;

    RECT rect = { 0, 0, 640, 480 };
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        return 0;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return hWnd;
}

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    
    MyRegisterClass(hInstance);
    HWND hWnd = InitInstance(hInstance, nCmdShow);
    HDC hdc = GetDC(hWnd);

    int pNumArgs = 0;
    LPWSTR* args = CommandLineToArgvW(GetCommandLineW(), &pNumArgs);
    if (pNumArgs < 2) {
        MessageBox(hWnd, L"Path to ROM not specified!  Exiting.", NULL, MB_OK);
        return 1;
    }
    LPWSTR second = args[1];
    int length = WideCharToMultiByte(CP_ACP, 0, second, -1, 0, 0, NULL, NULL);
    model.romPath = new char[length];
    WideCharToMultiByte(CP_ACP, 0, second, -1, model.romPath, length, NULL, NULL);

    model.sfxObject = nullptr;
    int i;
    for (i = 0; i < num_models; ++i) if (strcmp("High-Poly_Arwing", starfoxModels[i].name) == 0) break;
    model.current = i;
    model.frame = 0;
    model.tick = 0;
    model.dist = 0;
    model.rotation = 0.0f;
    SwitchModel();

    while (!shouldExit) {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        UpdateAnimatedModel();
        Render();
        SwapBuffers(hdc);
    }

    return 0;
}