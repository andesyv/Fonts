#include "text.h"
#include <iostream>
#include <iomanip>
#include <fstream>
#include "Math/vector4d.h"

// FreeType: Font building library
#include <ft2build.h>
#include FT_FREETYPE_H

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

Text::Text(std::string font)
{
	if (mMaterial.mShader->mName != "text") {
		auto shader = ShaderManager::get().find("text");
		if (!shader) {
			shader = ShaderManager::get().make(std::move(Shader{
			{ PROJECT_DIR + std::string{"Shaders\\text.vert"} },
			{ PROJECT_DIR + std::string{"Shaders\\text.frag"} }
				}.setName("text")));
		}
		mMaterial.mShader = shader;
	}
	mMaterial.enableParams(Material::PARAM::COLOR);

	// Setup drawing plane
	glGenVertexArrays(1, &mVAO);
	glBindVertexArray(mVAO);

	glGenBuffers(1, &mVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	/// Dynamic draw because we will be updating the vbo a lot.
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), nullptr);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);


	// Texture setup
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, &mCharTexAtlas);
	glBindTexture(GL_TEXTURE_2D, mCharTexAtlas);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (isFontInFile(font)) {
		readFromFile();
	} else {
		generateTextureAtlas(font);
	}
}

bool Text::isFontInFile(std::string font)
{
	/// Read character data:
	std::ifstream inf{ "Fonts\\fontData.dat", std::ios::in };
	if (!inf) {
		return false;
	}

	std::string fontName;

	// Read in fontname from top of file
	inf >> fontName;

	inf.close();

	return caseInSensStringCompare(font, fontName);
}

void Text::setTextColor(gsl::vec4 color)
{
	mMaterial.mColor = color;
}

void Text::generateTextureAtlas(std::string fontName)
{
	mFontName = fontName;
	std::cout << "Generating font atlas from file." << std::endl;

	std::string fontPath{ "Fonts/" + fontName + ".ttf" };

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Font size
	if (FT_Set_Pixel_Sizes(face, 0, 48))
		std::cout << "ERROR::FREETYPE: Failed to set pixel sizes" << std::endl;

	unsigned int totalWidth{ 0 }, maxHeight{ 0 }, maxWidth{ 0 }, numChars{ 0 };
	for (char c = 32; c < 127; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		++numChars;
		maxHeight = (face->glyph->bitmap.rows > maxHeight) ? face->glyph->bitmap.rows : maxHeight;
		maxWidth = (face->glyph->bitmap.width > maxWidth) ? face->glyph->bitmap.width : maxWidth;
		totalWidth += face->glyph->bitmap.width;
	}

	/*int totalWidth{ 0 }, maxHeight{ 0 }, maxWidth{ 0 }, numChars{ 0 };
	for (auto it = Characters.begin(); it != Characters.end(); it++) {
		auto glyph = it->second;
		++numChars;
		maxHeight = (glyph.Height > maxHeight) ? glyph.Height : maxHeight;
		maxWidth = (glyph.Width > maxWidth) ? glyph.Width : maxWidth;
		totalWidth += glyph.Width;
	}*/

	glBindTexture(GL_TEXTURE_2D, mCharTexAtlas);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, totalWidth, maxHeight, 0, GL_RED, GL_UNSIGNED_BYTE, 0);

	mAtlasWidth = totalWidth;
	mAtlasHeight = maxHeight;

	/*int x{ 0 };
	for (auto it = mCharacterList.begin(); it != mCharacterList.end(); it++) {
		
		glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, it->Width, it->Height, GL_ALPHA, GL_UNSIGNED_BYTE, it->
);

		x += it->Width;
	}*/

	unsigned int x{ 0 };
	for (char c{ charOffset }; c < /*127*/127; c++) {
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}

		CharInfo info = {
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x,
			face->glyph->advance.y,
			x / static_cast<float>(totalWidth)
		};

		characterInfo[c - charOffset] = info;
		/*std::cout << "characterInfo:";
		std::cout << "width: " << characterInfo[c - charOffset].width << ", ";
		std::cout << "height: " << characterInfo[c - charOffset].height << ", ";
		std::cout << "OffsetX: " << characterInfo[c - charOffset].bearingX << ", ";
		std::cout << "OffsetY: " << characterInfo[c - charOffset].bearingY << ", ";
		std::cout << "advanceX: " << characterInfo[c - charOffset].advanceX << ", ";
		std::cout << "advanceY: " << characterInfo[c - charOffset].advanceY << ", ";
		std::cout << "texXOffset: " << characterInfo[c - charOffset].texXOffset << ", ";
		std::cout << "x is: " << static_cast<int>(x) << std::endl;*/
		if (info.width > 0 && info.height > 0) {
			/*glBindTexture(GL_TEXTURE_2D, mCharTexAtlas);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);*/
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, 0, info.width, info.height, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			// glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, info.width, info.height, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			// glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, info.width, info.height, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
			/*std::string filename{ "Textures\\"};
			filename.append(1, c);
			filename.append(".bmp");
			stbi_write_bmp(filename.c_str(), info.width, info.height, 1, face->glyph->bitmap.buffer);*/
		}

		x += info.width;
	}

	saveToFile();
}

void Text::saveToFile()
{
	/// Print character atlas to file
	// Allocate memory for pixel data
	GLubyte* pixels = new GLubyte[4 * mAtlasWidth * mAtlasHeight]{};
	
	// Copy pixel data
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
	// Save pixel data to file
	stbi_write_bmp("Fonts\\fontAtlas.bmp", mAtlasWidth, mAtlasHeight, 1, pixels);

	// Deallocate memory
	delete pixels;


	/// Save character data to file:
	std::ofstream outf{ "Fonts\\fontData.dat", std::ios::out };
	if (!outf) {
		std::cout << "Could'nt write to Fonts/fontData.dat" << std::endl;
		return;
	}

	// If other general information needs to be known about font atlas,
	// it should go in the top of the file. Here.

	outf << mFontName << std::endl;
	
	for (unsigned int i{ 0 }; i < 128 - charOffset; ++i) {
		CharInfo info = characterInfo[i];
		outf
			<< info.width << " " << info.height << " "
			<< info.bearingX << " " << info.bearingY << " "
			<< info.advanceX << " " << info.advanceY << " "
			<< info.texXOffset
			<< std::endl;
	}

	outf.close();
}

void Text::readFromFile()
{
	/// Read character data:
	std::ifstream inf{ "Fonts\\fontData.dat", std::ios::in };
	if (!inf) {
		std::cout << "Could'nt read file Fonts/fontData.dat" << std::endl;
		return;
	}

	// Read in fontname from top of file
	inf >> mFontName;

	for (unsigned int i{ 0 }; i < 128 - charOffset && inf; ++i) {
		CharInfo info;
		inf >> info.width >> info.height >> info.bearingX >> info.bearingY >> info.advanceX >> info.advanceY >> info.texXOffset;
		characterInfo[i] = info;
	}

	inf.close();

	/// Read pixels
	loadImage("Fonts\\fontAtlas.bmp");
}

void Text::loadImage(std::string file)
{
	int width, height, nrChannels;
	unsigned char* data = stbi_load(file.c_str(), &width, &height, &nrChannels, 0);

	std::cout << std::hex;
	for (unsigned int i{ 0 }; i < 50; ++i) {
		std::cout << std::hex << +data[i] << " ";
	}
	std::cout << std::endl;


	std::cout << "image channels: " << nrChannels << std::endl;

	glBindTexture(GL_TEXTURE_2D, mCharTexAtlas);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	mAtlasWidth = width;
	mAtlasHeight = height;

	stbi_image_free(data);
}

void Text::write(std::string text, GLfloat x, GLfloat y, GLfloat size)
{
	mMaterial.use();

	glBindVertexArray(mVAO);

	// Render glyph texture over quad
	glBindTexture(GL_TEXTURE_2D, mCharTexAtlas);

	for (auto c : text) {
		auto info = characterInfo[c - charOffset];

		GLfloat xpos = x + info.bearingX * size;
		GLfloat ypos = y - (info.height - info.bearingY) * size;

		xpos /= 800;
		ypos /= 600;


		GLfloat w = info.width * size;
		GLfloat h = info.height * size;

		w /= 800;
		h /= 600;

		auto texXEnd = info.texXOffset + info.width / static_cast<float>(mAtlasWidth);
		// std::cout << "UV start: " << info.texXOffset << ", UV stop: " << texXEnd << std::endl;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			// Note: Flipping y uv's to make up for that OpenGL uses it's y-axis on images reverse.
			{ xpos,     ypos,		info.texXOffset, 1.0 },
			{ xpos + w, ypos,       texXEnd, 1.0 },
			{ xpos + w, ypos + h,   texXEnd, 0.0 },

			{ xpos + w, ypos + h,   texXEnd, 0.0 },
			{ xpos,		ypos + h,   info.texXOffset, 0.0 },
			{ xpos,		ypos,		info.texXOffset, 1.0 }
		};

		// std::cout << "w is: " << w << ", and h is: " << h << std::endl;

		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, mVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (info.advanceX >> 6) * size; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

bool compareChar(char& c1, char& c2)
{
	if (c1 == c2)
		return true;
	else if (std::toupper(c1) == std::toupper(c2))
		return true;
	return false;
}

bool Text::caseInSensStringCompare(std::string& str1, std::string& str2)
{
	return ((str1.size() == str2.size()) &&
		std::equal(str1.begin(), str1.end(), str2.begin(), &compareChar));
}

void Text::loadCharacters(std::string fontName)
{
	std::string fontPath{ "Fonts/" + fontName + ".ttf" };

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
		std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

	FT_Face face;
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
		std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

	// Font size
	FT_Set_Pixel_Sizes(face, 0, 48);


	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	mCharacterList.reserve(127 - 32);
	for (char c = 32; c < 127; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
			continue;
		}
		// Generate texture
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
		// Set texture options
		gsl::vec4 borderColor{ 0.f, 0.f, 0.f, 1.f };
		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor.xP());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			face->glyph->bitmap_left,
			face->glyph->bitmap_top,
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
		mCharacterList.push_back(character);
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}
