#ifndef TEXT_H
#define TEXT_H

#include <glad/glad.h>
#include <map>
#include <vector>
#include <string>

#include "material.h"

class Text
{
private:
	// DEPRECATED
	struct Character {
		GLuint TextureID;	// ID handle of the glyph texture
		GLuint Width;		// Width of glyph
		GLuint Height;		// Height of glyph
		GLint BearingX;	// OffsetX from baseline to left of glyph
		GLint BearingY;	// OffsetY from baseline to top of glyph
		GLint Advance;		// Offset to advance to next glyph
	};

	struct CharInfo {
		GLuint width;		// Width of glyph
		GLuint height;		// Height of glyph
		GLint bearingX;		// OffsetX from baseline to left of glyph
		GLint bearingY;		// OffsetY from baseline to top of glyph
		GLint advanceX;		// Offset in x direction to advance to next glyph
		GLint advanceY;		// Offset in y direction to advance to next glyph
		GLfloat texXOffset;	// Texture offset in x direction
	};

	GLuint mCharTexAtlas{ 0 };
	GLuint mAtlasWidth{ 0 }, mAtlasHeight{ 0 };
	GLuint mVAO{ 0 }, mVBO{ 0 };

	static const GLuint charOffset{ 32 };
	CharInfo characterInfo[128 - charOffset]{};

	std::string mFontName;

	std::map<GLchar, Character> Characters;
	std::vector<Character> mCharacterList;
	void loadCharacters(std::string fontName = "consola");
	Material mMaterial{"text"};

	void generateTextureAtlas(std::string fontName = "consola");
	void loadImage(std::string file);

	void saveToFile();
	void readFromFile();

public:
	Text (std::string font = "consola");
	bool isFontInFile(std::string font);
	void setTextColor(gsl::vec4 color);

	void write(std::string text, GLfloat x, GLfloat y, GLfloat size = 1);

	// Various helper functions
	/*
	 * Case Insensitive String Comparision
	 */
	static bool caseInSensStringCompare(std::string& str1, std::string& str2);
};

#endif

