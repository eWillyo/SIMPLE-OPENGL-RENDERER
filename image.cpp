#include "image.h"
#include <stdio.h>
#include <math.h>

int gl_max_texture_size = 0;
int gl_max_texture_max_anisotropy_ext = 0;


FIBITMAP* load_bitmap(const char* path_to_image, int& width, int& height, int& bpp)
{
    FREE_IMAGE_FORMAT fif = FreeImage_GetFileType(path_to_image);

    if (fif == FIF_UNKNOWN)
    {
        fif = FreeImage_GetFIFFromFilename(path_to_image);
    }

    if (fif == FIF_UNKNOWN)
    {
        return NULL;
    }

    FIBITMAP* dib = NULL;

    if (FreeImage_FIFSupportsReading(fif))
    {
        dib = FreeImage_Load(fif, path_to_image);
    }

    if (dib != NULL)
    {
        int OriginalWidth = FreeImage_GetWidth(dib);
        int OriginalHeight = FreeImage_GetHeight(dib);

        width = OriginalWidth;
        height = OriginalHeight;

        if (width == 0 || height == 0)
        {
            FreeImage_Unload(dib);
            return NULL;
        }

        bpp = FreeImage_GetBPP(dib);

        if (width > gl_max_texture_size)
            width = gl_max_texture_size;
        if (height > gl_max_texture_size)
            height = gl_max_texture_size;

        if (!GLEW_ARB_texture_non_power_of_two)
        {
            width = 1 << (int)floor((log((float)width) / log(2.0f)) + 0.5f);
            height = 1 << (int)floor((log((float)height) / log(2.0f)) + 0.5f);
        }

        if (width != OriginalWidth || height != OriginalHeight)
        {
            FIBITMAP* rdib = FreeImage_Rescale(dib, width, height, FILTER_BICUBIC);
            FreeImage_Unload(dib);
            dib = rdib;
        }
    }

    return dib;
}

GLuint load_2D_texture(const char* path_to_image)
{
    int width, height, BPP;

    glEnable(GL_TEXTURE_2D);
    // get necessary extensions
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &gl_max_texture_size);
    if (GLEW_EXT_texture_filter_anisotropic)
    {
        glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &gl_max_texture_max_anisotropy_ext);
    }

    FIBITMAP* dib = load_bitmap(path_to_image, width, height, BPP);

    if (dib == NULL)
    {
        fprintf(stderr, "Error: loading texture (%s).", path_to_image);
        return 0;
    }

    GLenum Format = 0;

    if (BPP == 32) Format = GL_BGRA;
    if (BPP == 24) Format = GL_BGR;

    if (Format == 0)
    {
        fprintf(stderr, "Unsupported texture format (%s)!\r\n", path_to_image);
        FreeImage_Unload(dib);
        return 0;
    }

    BYTE* Data = FreeImage_GetBits(dib);

    if (!GLEW_VERSION_1_2)
    {
        int Pitch = FreeImage_GetPitch(dib);

        if (Format == GL_BGRA) Format = GL_RGBA;
        if (Format == GL_BGR) Format = GL_RGB;

        int bpp = BPP / 8;

        BYTE* line = Data;

        for (int y = 0; y < height; y++)
        {
            BYTE* pixel = line;

            for (int x = 0; x < width; x++)
            {
                BYTE Temp = pixel[0];
                pixel[0] = pixel[2];
                pixel[2] = Temp;

                pixel += bpp;
            }

            line += Pitch;
        }
    }

    // load texture
    GLuint Texture;

    glGenTextures(1, &Texture);
    glBindTexture(GL_TEXTURE_2D, Texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GLEW_VERSION_1_4 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (GLEW_EXT_texture_filter_anisotropic)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, gl_max_texture_max_anisotropy_ext);
    }

    if (GLEW_VERSION_1_4)
    {
        glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    }

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, Format, GL_UNSIGNED_BYTE, Data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    FreeImage_Unload(dib);

    return Texture;
}

void unload_2D_texture(GLuint& texture)
{
    glDeleteTextures(1, &texture);
    texture = 0;
}