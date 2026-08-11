#include "AppContext.h"
#include <iostream>
#include "../../Textures/stb_image.h"

namespace FTexture
{
    GLuint LoadTexture(const char* InPath)
    {
        GLuint TextureID = 0;
        glGenTextures(1, &TextureID);

        GLint Width = 0, Height = 0, nrChannels = 0;
        GLubyte* TextureData = stbi_load(InPath, &Width, &Height, &nrChannels, 0);

        if (TextureData)
        {
            GLenum Format = 0;
            if (nrChannels == 1)
            {
                Format = GL_RED;
            }
            else if (nrChannels == 3)
            {
                Format = GL_RGB;
            }
            else if (nrChannels == 4)
            {
                Format = GL_RGBA;
            }

            // Bind texture
            glBindTexture(GL_TEXTURE_2D, TextureID);

            // Upload texture to GPU
            glTexImage2D(GL_TEXTURE_2D, 0, Format, Width, Height, 0, Format, GL_UNSIGNED_BYTE, TextureData);
            glGenerateMipmap(GL_TEXTURE_2D);

            // Texture wrapping mode
            glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            // Texture filtering
            glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(TextureData);
        }
        else
        {
            std::cerr << "Texture failed to load at path: " << InPath << "\n";
            stbi_image_free(TextureData);
        }

        return TextureID;
    }
} // namespace FTexture
