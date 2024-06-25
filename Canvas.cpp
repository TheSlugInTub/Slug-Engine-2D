#include <Canvas.h>
#include <stb_image.h>

Canvas::Canvas(int width, int height) : m_width(width), m_height(height) {
    InitFreeType();
    InitOpenGL();
    LoadShader();
}

Canvas::~Canvas() {
    FT_Done_FreeType(ft);
    delete shader;
}

void Canvas::InitFreeType() {
    if (FT_Init_FreeType(&ft)) {
        std::cerr << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
    }
}

void Canvas::LoadFont(const std::string& fontPath) {
    if (Fonts.find(fontPath) != Fonts.end()) {
        // Font already loaded
        return;
    }

    FT_Face face;
    if (FT_New_Face(ft, fontPath.c_str(), 0, &face)) {
        std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return;
    }
    FT_Set_Pixel_Sizes(face, 0, 48);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    std::map<char, Character> characters;
    for (GLubyte c = 0; c < 128; c++) {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            std::cerr << "ERROR::FREETYPE: Failed to load Glyph" << std::endl;
            continue;
        }
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        characters.insert(std::pair<char, Character>(c, character));
    }
    Fonts.insert(std::pair<std::string, std::map<char, Character>>(fontPath, characters));

    FT_Done_Face(face);
}

void Canvas::InitOpenGL() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    projection = glm::ortho(0.0f, static_cast<float>(m_width), 0.0f, static_cast<float>(m_height));

    float vertices[] = {
        // positions         // texture coords
         0.5f,  0.5f, 0.0f,  1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,  1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,  0.0f, 1.0f  // top left 
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &imageVAO);
    glGenBuffers(1, &imageVBO);
    glGenBuffers(1, &imageEBO);

    glBindVertexArray(imageVAO);

    glBindBuffer(GL_ARRAY_BUFFER, imageVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, imageEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Canvas::LoadShader() {
    shader = new Shader("Shaders/textvertex.shad", "Shaders/textfragment.shad");
    shader->use();
    shader->setMat4("projection", projection);

    imageShader = new Shader("Shaders/vertex2d.shad", "Shaders/fragment2d.shad");
}

void Canvas::AddText(const std::string& text, const std::string& fontPath, float x, float y, float scale, glm::vec3 color) {
    LoadFont(fontPath);
    m_texts.push_back(std::make_tuple(text, fontPath, x, y, scale, color));
}

void Canvas::AddImage(unsigned int texture, float x, float y, float width, float height, float angle, glm::vec4 color) {
    m_images.push_back({ texture, x, y, width, height, angle, color });
}

void Canvas::RenderCanvas(const glm::mat4& view, Camera& camera) {

    shader->use();
    glBindVertexArray(VAO);

    for (const auto& item : m_texts) 
    {
        const std::string& text = std::get<0>(item);
        const std::string& fontPath = std::get<1>(item);
        float x = std::get<2>(item);
        float y = std::get<3>(item);
        float scale = std::get<4>(item);
        glm::vec3 color = std::get<5>(item);

        shader->setVec3("textColor", color);

        std::string::const_iterator c;
        for (c = text.begin(); c != text.end(); c++) {
            Character ch = Fonts[fontPath][*c];

            float xpos = x + ch.Bearing.x * scale;
            float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

            float w = ch.Size.x * scale;
            float h = ch.Size.y * scale;
            float vertices[6][4] = {
                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos,     ypos,       0.0f, 1.0f },
                { xpos + w, ypos,       1.0f, 1.0f },

                { xpos,     ypos + h,   0.0f, 0.0f },
                { xpos + w, ypos,       1.0f, 1.0f },
                { xpos + w, ypos + h,   1.0f, 0.0f }
            };
            glBindTexture(GL_TEXTURE_2D, ch.TextureID);
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glDrawArrays(GL_TRIANGLES, 0, 6);

            x += (ch.Advance >> 6) * scale;
        }
    }

    imageShader->use();
    glBindVertexArray(imageVAO);

    for (const auto& image : m_images)
    {
        float xpos = image.x;
        float ypos = image.y;
        float w = image.Width;
        float h = image.Height;
        float angle = image.Angle;
        glm::vec4 color = image.Color;

        imageShader->setTexture2D("texture1", image.Texture, 0);

        glm::mat4 transform = glm::mat4(1.0f);
        transform = glm::translate(transform, glm::vec3(xpos, ypos, camera.Position.z));
        transform = glm::rotate(transform, glm::radians(angle), glm::vec3(0.0f, 0.0f, 1.0f));
        transform = glm::scale(transform, glm::vec3(w, h, 1.0f));

        imageShader->setMat4("transform", transform);
        imageShader->setMat4("view", view);
        imageShader->setMat4("projection", projection);
        imageShader->setVec4("ourColor", color);
        imageShader->setBool("isWhite", false);

        glBindVertexArray(imageVAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Canvas::EditText(size_t index, const std::string& newText, float x, float y, float scale, glm::vec3 color) {
    if (index >= m_texts.size()) {
        std::cerr << "ERROR::CANVAS: Text index out of range" << std::endl;
        return;
    }
    std::get<0>(m_texts[index]) = newText;
    std::get<2>(m_texts[index]) = x;
    std::get<3>(m_texts[index]) = y;
    std::get<4>(m_texts[index]) = scale;
    std::get<5>(m_texts[index]) = color;
}

void Canvas::EditImage(int index, unsigned int newTexture, float newX, float newY, float newWidth, float newHeight, float newAngle, glm::vec4 newColor)
{
    if (index >= m_images.size()) {
        std::cerr << "ERROR::CANVAS: Image index out of range" << std::endl;
        return;
    }
    if (newTexture != NULL) { m_images[index].Texture = newTexture; }
    m_images[index].x = newX;
    m_images[index].y = newY;
    m_images[index].Width = newWidth;
    m_images[index].Height = newHeight;
    if (newAngle != NULL) { m_images[index].Angle = newAngle; }
    m_images[index].Color = newColor; 
}