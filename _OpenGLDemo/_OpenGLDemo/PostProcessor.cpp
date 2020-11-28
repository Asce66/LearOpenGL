#include "PostProcessor.h"
#include<iostream>
PostProcessor::PostProcessor(Shader& shader, GLuint width, GLuint height) :
	PostProcessingShader(shader), Texture(), Width(width), Height(height), Shake(false), chaos(false), Confuse(false)
{
	//生成FBO/RBO/MSFBO
	glGenFramebuffers(1, &MSFBO);
	glGenFramebuffers(1, &FBO);
	glGenRenderbuffers(1, &RBO);

	//设置MSBO/RBO数据
	glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, RBO);
	glRenderbufferStorageMultisample(GL_RENDERBUFFER, 8, GL_RGB, Width, Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, RBO);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "初始化MSFBO失败\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//设置FBO数据
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	Texture.Generate(Width, Height, NULL);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, Texture.ID, 0);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "初始化FBO失败\n";
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//设置渲染数据/uniform参数值
	InitRenderData();
	PostProcessingShader.Use().SetInteger("scene", 0);
	GLfloat offset = 1 / 300.0f;
	GLfloat offsets[9][2] = {
		{ -offset,  offset  },  // top-left
		{  0.0f,    offset  },  // top-center
		{  offset,  offset  },  // top-right
		{ -offset,  0.0f    },  // center-left
		{  0.0f,    0.0f    },  // center-center
		{  offset,  0.0f    },  // center - right
		{ -offset, -offset  },  // bottom-left
		{  0.0f,   -offset  },  // bottom-center
		{  offset, -offset  }   // bottom-right    
	};
	glUniform2fv(glGetUniformLocation(PostProcessingShader.ID, "offsets"), 9, (GLfloat*)offsets);

	GLint edge_kernel[9] = {
	-1, -1, -1,
	-1,  8, -1,
	-1, -1, -1
	};
	glUniform1iv(glGetUniformLocation(PostProcessingShader.ID, "edge_kernel"), 9, edge_kernel);

	GLfloat blur_kernel[9] = {
		1.0 / 16, 2.0 / 16, 1.0 / 16,
		2.0 / 16, 4.0 / 16, 2.0 / 16,
		1.0 / 16, 2.0 / 16, 1.0 / 16
	};
	glUniform1fv(glGetUniformLocation(PostProcessingShader.ID, "blur_kernel"), 9, blur_kernel);

}

void PostProcessor::BeginRender()
{
	glBindFramebuffer(GL_FRAMEBUFFER, MSFBO);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT);
}

void PostProcessor::EndRender()
{
	//从多重采样缓冲到一般缓冲
	glBindFramebuffer(GL_READ_FRAMEBUFFER, MSFBO);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, FBO);
	glBlitFramebuffer(0, 0, Width, Height, 0, 0, Width, Height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);// Binds both READ and WRITE framebuffer to default framebuffer
}

void PostProcessor::Render(GLfloat time)
{
	//设置uniform参数
	PostProcessingShader.Use();
	PostProcessingShader.SetFloat("time", time);
	PostProcessingShader.SetInteger("confuse", Confuse);
	PostProcessingShader.SetInteger("shake", Shake);
	PostProcessingShader.SetInteger("chaos", chaos);

	//绘制铺满屏幕的quad
	glActiveTexture(GL_TEXTURE0);
	Texture.Bind();
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);

}

void PostProcessor::InitRenderData()
{
	GLuint VBO;
	GLfloat vertices[] = {
		// Pos        // Tex
		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f,
		-1.0f,  1.0f, 0.0f, 1.0f,

		-1.0f, -1.0f, 0.0f, 0.0f,
		 1.0f, -1.0f, 1.0f, 0.0f,
		 1.0f,  1.0f, 1.0f, 1.0f
	};

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glBindVertexArray(0);
}
