#include "TextRenderer.h"

#include"resource_manager.h"

#include<ft2build.h>
#include FT_FREETYPE_H

#include<iostream>

TextRenderer::TextRenderer(GLuint width, GLuint heigth)
{
	TextShader = ResourceManager::LoadShader("textver.txt", "textfrag.txt", nullptr,"text"); 
	glm::mat4 projection = glm::ortho(0.0f, (float)width, (float)heigth, 0.0f, -1.0f, 1.0f);

	TextShader.Use();
	TextShader.SetMatrix4("projection", projection);
	TextShader.SetInteger("text", 0);

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, 6 * 4 * sizeof(float), NULL, GL_DYNAMIC_DRAW);//��������ΪGL_DYNAMIC_DRAW����Ҫ��̬����
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(float) * 4, (void*)0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void TextRenderer::Load(std::string fontFile, GLuint fontSize)
{
	Characters.clear();

	FT_Library fT_Library;
	if (FT_Init_FreeType(&fT_Library))
		std::cout << "��ʼ�������ʧ��\n";
	FT_Face face;
	if (FT_New_Face(fT_Library, fontFile.c_str(), 0, &face))
		std::cout << "��������ʧ�ܣ�" << fontFile << std::endl;
	//���������С�����Ϊ0���������Ÿ߶ȶ�̬�ı䣩
	FT_Set_Pixel_Sizes(face, 0, fontSize);
	//ȡ��Ĭ�ϵ�������뷽ʽ
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	
	//��ʼ��128��ASIIC�벢����
	for (int c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "�����ַ�ʧ�ܣ�" << (char)c << std::endl;
			continue;
		}
		//����������ͼ
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
			face->glyph->bitmap.width, face->glyph->bitmap.rows, 
			0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		Character character = {
			texture,
			glm::vec2(face->glyph->bitmap.width,face->glyph->bitmap.rows),
			glm::vec2(face->glyph->bitmap_left,face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters[c] = character;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	FT_Done_Face(face);
	FT_Done_FreeType(fT_Library);
}

void TextRenderer::RenderText(std::string text, glm::vec2 position, GLfloat scale, glm::vec3 color)
{
	TextShader.Use();
	TextShader.SetVector3f("textColor", color);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::string::const_iterator c;
	for (c = text.begin(); c != text.end();c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = position.x + ch.Bearing.x* scale;
		GLfloat ypos = position.y + (Characters['H'].Bearing.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;

		//����VBO����
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 },
			{ xpos,     ypos,       0.0, 0.0 },

			{ xpos,     ypos + h,   0.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 1.0 },
			{ xpos + w, ypos,       1.0, 0.0 }
		};
		//��������ͼ
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		//����VBO
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		//������һ���ַ���x
		position.x += (ch.Advance >> 6) * scale;
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
