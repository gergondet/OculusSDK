/************************************************************************************

Filename    :   RenderTiny_GL_Device.h
Content     :   RenderDevice implementation header for OpenGL (tiny version)
Created     :   September 10, 2012
Authors     :   Andrew Reisse, Artem Bolgar

Copyright   :   Copyright 2012 Oculus VR, Inc. All Rights reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.

************************************************************************************/

#ifndef OVR_Render_GL_Device_h
#define OVR_Render_GL_Device_h

#include "RenderTiny_Device.h"

#include "GL/glew.h"
#include "GLFW/glfw3.h"

namespace OVR { namespace RenderTiny { namespace GL {

class RenderDevice;

class Buffer : public RenderTiny::Buffer
{
public:
    RenderDevice* Ren;
    size_t        Size;
    GLenum        Use;
    GLuint        GLBuffer;

public:
    Buffer(RenderDevice* r) : Ren(r), Size(0), Use(0), GLBuffer(0) {}
    ~Buffer();

    GLuint         GetBuffer() { return GLBuffer; }

    virtual size_t GetSize() { return Size; }
    virtual void*  Map(size_t start, size_t size, int flags = 0);
    virtual bool   Unmap(void *m);
    virtual bool   Data(int use, const void* buffer, size_t size);
};

class Texture : public RenderTiny::Texture
{
public:
    RenderDevice* Ren;
    GLuint        TexId;
    int           Width, Height;

    Texture(RenderDevice* r, int w, int h);
    ~Texture();

    virtual int GetWidth() const { return Width; }
    virtual int GetHeight() const { return Height; }

    virtual void SetSampleMode(int);

    virtual void Set(int slot, ShaderStage stage = Shader_Fragment) const;
};

class Shader : public RenderTiny::Shader
{
public:
    GLuint      GLShader;

    Shader(RenderDevice*, ShaderStage st, GLuint s) : RenderTiny::Shader(st), GLShader(s) {}
    Shader(RenderDevice*, ShaderStage st, const char* src) : RenderTiny::Shader(st), GLShader(0)
    {
        Compile(src);
    }
    ~Shader()
    {
        if (GLShader)
            glDeleteShader(GLShader);
    }
    bool Compile(const char* src);

    GLenum GLStage() const
    {
        switch (Stage)
        {
        default:  OVR_ASSERT(0); return GL_NONE;
        case Shader_Vertex: return GL_VERTEX_SHADER;
        case Shader_Fragment: return GL_FRAGMENT_SHADER;
        }
    }

    //void Set(PrimitiveType prim) const;
    //void SetUniformBuffer(Render::Buffer* buffers, int i = 0);
};

class ShaderSet : public RenderTiny::ShaderSet
{
public:
    GLuint Prog;

    struct Uniform
    {
        String Name;
        int    Location, Size;
        int    Type; // currently number of floats in vector
    };
    Array<Uniform> UniformInfo;

    int     ProjLoc, ViewLoc;
    int     TexLoc[8];
    bool    UsesLighting;
    int     LightingVer;

    ShaderSet();
    ~ShaderSet();

    virtual void SetShader(RenderTiny::Shader *s)
    {
        Shaders[s->GetStage()] = s;
        Shader* gls = (Shader*)s;
        glAttachShader(Prog, gls->GLShader);
        if (Shaders[Shader_Vertex] && Shaders[Shader_Fragment])
            Link();
    }
    virtual void UnsetShader(int stage)
    {
        Shader* gls = (Shader*)(RenderTiny::Shader*)Shaders[stage];
        if (gls)
            glDetachShader(Prog, gls->GLShader);
        Shaders[stage] = NULL;
        Link();
    }

    virtual void Set(PrimitiveType prim) const;

    // Set a uniform (other than the standard matrices). It is undefined whether the
    // uniforms from one shader occupy the same space as those in other shaders
    // (unless a buffer is used, then each buffer is independent).
    virtual bool SetUniform(const char* name, int n, const float* v);
    virtual bool SetUniform4x4f(const char* name, const Matrix4f& m);

    bool Link();
};

 class RBuffer : public RefCountBase<RBuffer>
{
 public:
    int    Width, Height;
    GLuint BufId;

    RBuffer(GLenum format, GLint w, GLint h);
    ~RBuffer();
};

class RenderDevice : public RenderTiny::RenderDevice
{
    Ptr<Shader>        VertexShaders[VShader_Count];
    Ptr<Shader>        FragShaders[FShader_Count];

    Ptr<ShaderFill> DefaultFill;

    Matrix4f    Proj;

    Ptr<Texture>             CurRenderTarget;
    Array<Ptr<RBuffer> >     DepthBuffers;
    GLuint                   CurrentFbo;

    const LightingParams*    Lighting;
    GLFWwindow* window;


public:
    RenderDevice(const RendererParams& p, GLFWwindow * oswnd);
    virtual void Present();

    // Implement static initializer function to create this class.
    static RenderTiny::RenderDevice* CreateDevice(const RendererParams& rp, void* oswnd);

    virtual void SetRealViewport(const Viewport& vp);

    //virtual void SetScissor(int x, int y, int w, int h);

    virtual void Clear(float r = 0, float g = 0, float b = 0, float a = 1, float depth = 1);
    virtual void Rect(float left, float top, float right, float bottom) { OVR_UNUSED4(left,top,right,bottom); }

    virtual void BeginRendering();
    virtual void SetDepthMode(bool enable, bool write, CompareFunc func = Compare_Less);
    virtual void SetWorldUniforms(const Matrix4f& proj);

    RBuffer* GetDepthBuffer(int w, int h, int ms);

    virtual void SetRenderTarget(RenderTiny::Texture* color,
                                 RenderTiny::Texture* depth = NULL, RenderTiny::Texture* stencil = NULL);

    virtual void SetLighting(const LightingParams* lt);

    virtual void Render(const Matrix4f& matrix, Model* model);
    virtual void Render(const ShaderFill* fill, RenderTiny::Buffer* vertices, RenderTiny::Buffer* indices,
                        const Matrix4f& matrix, int offset, int count, PrimitiveType prim = Prim_Triangles);

    virtual Buffer* CreateBuffer();
    virtual Texture* CreateTexture(int format, int width, int height, const void* data, int mipcount=1);
    virtual ShaderSet* CreateShaderSet() { return new ShaderSet; }

    virtual ShaderFill *CreateSimpleFill() { return DefaultFill; }

    virtual Shader *LoadBuiltinShader(ShaderStage stage, int shader);

    void SetTexture(RenderTiny::ShaderStage, int slot, const Texture* t);
};

}}}

#endif
