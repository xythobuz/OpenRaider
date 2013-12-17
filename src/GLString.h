/* -*- Mode: C++; tab-width: 3; indent-tabs-mode: t; c-basic-offset: 3 -*- */
/*================================================================
 *
 * Project : Mtk
 * Author  : Terry 'Mongoose' Hendrix II
 * Website : http://www.westga.edu/~stu7440/
 * Email   : stu7440@westga.edu
 * Object  : GLString
 * License : No use w/o permission (C) 2002 Mongoose
 * Comments: Open GL rendering font/string class
 *
 *
 *           This file was generated using Mongoose's C++
 *           template generator script.  <stu7440@westga.edu>
 *
 *-- History ------------------------------------------------
 *
 * 2002.01.01:
 * Mongoose - Created
 ================================================================*/


#ifndef __MTK_MONGOOSE_GLSTRING_H_
#define __MTK_MONGOOSE_GLSTRING_H_

typedef struct gl_string_s
{
	int x;
	int y;
	int font;
	float scale;
	char *text;
	bool active;
	unsigned short int len;

} gl_string_t;


class GLString
{
 public:

	GLString();
	/*------------------------------------------------------
	 * Pre  :
	 * Post : Constructs an object of GLString
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.01.01:
	 * Mongoose - Created
	 ------------------------------------------------------*/

	~GLString();
	/*------------------------------------------------------
	 * Pre  : GLString object is allocated
	 * Post : Deconstructs an object of GLString
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.01.01:
	 * Mongoose - Created
	 ------------------------------------------------------*/

   void Init(unsigned int max_strings, unsigned int max_fonts, int *tex_map);
   /*------------------------------------------------------
    * Pre  : Set max number of strings and font faces
	 *
	 *        Pass an int array as a map of font texture ids
	 *        and it's size should be 'max_fonts' since
	 *        you need a texture per font  =)
	 *
    * Post :
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

	void SetChar(unsigned int string, unsigned int pos, char c);
   /*------------------------------------------------------
    * Pre  : String is valid gl_string id
	 *        Pos is position in that gl_string to set
	 *        to character C
	 *
    * Post : Sets a single byte in a string
    *
    *-- History ------------------------------------------
    *
    * 2002.03.30:
    * Mongoose - Created
    ------------------------------------------------------*/

	unsigned int GetStringLen(unsigned int string);
   /*------------------------------------------------------
    * Pre  : String is valid gl_string id
	 *
    * Post : Gets num bytes in string buffer
    *
    *-- History ------------------------------------------
    *
    * 2002.03.30:
    * Mongoose - Created
    ------------------------------------------------------*/

	char *GetBuffer(unsigned int string);
   /*------------------------------------------------------
    * Pre  : String is valid gl_string id
	 *
    * Post : Returns a pointer to string buffer
    *
    *-- History ------------------------------------------
    *
    * 2002.03.30:
    * Mongoose - Created
    ------------------------------------------------------*/

	void setActive(unsigned int string, bool active);

	void SetString(unsigned int string, char *s, ...);
   /*------------------------------------------------------
    * Pre  : String is valid gl_string id
	 *        Args form the string to fill String buffer
	 *        Args are same as printf format
	 *
    * Post : Sets text in a string, which will be
	 *        truncated as needed to fit
    *
    *-- History ------------------------------------------
    *
    * 2002.03.30:
    * Mongoose - Created
    ------------------------------------------------------*/

   void Scale(float scale);
   /*------------------------------------------------------
    * Pre  : scale is > 0.0
    * Post : Sets default text scaling
    *
    *-- History ------------------------------------------
    *
    * 2002.01.03:
    * Mongoose - Created
    ------------------------------------------------------*/

   int BuildFontList(int index);
   /*------------------------------------------------------
    * Pre  : Index is valid index into the font base list
    * Post : Adds a new font face to font list
	 *
	 *        Returns index of font on no error
	 *        Returns -1 on full font list
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

   int glPrintf(int x, int y, int font, const char *string, ...);
   /*------------------------------------------------------
    * Pre  : X, Y are valid screen coor
	 *        Font is valid font index
	 *        String is valid string with args
	 *
	 *        Returns  0 on no error
	 *        Returns -1 on invalid string
	 *        Returns -2 on full string list
	 *        Returns -3 on full font list
	 *
    * Post : Generates a new string
	 *        Renders string to gl target
    *
    *-- History ------------------------------------------
    *
    * 2001.12.31:
    * Mongoose - Created
    ------------------------------------------------------*/

   void Render(int width, int height);
   /*------------------------------------------------------
    * Pre  : Width and Height for the GL context
	 *        Called after scene is rendered
	 *
	 *        GL Culling disabled
	 *
    * Post : Renders strings over GL scene
    *
    *-- History ------------------------------------------
    *
    * 2002.01.01:
    * Mongoose - Created
    ------------------------------------------------------*/

   gl_string_t *GetString(unsigned int id);
   /*------------------------------------------------------
    * Pre  : Id is valid string id
    * Post : Return string with id, or NULL if DNE
    *
    *-- History ------------------------------------------
    *
    * 2002.01.04:
    * Mongoose - Created
    ------------------------------------------------------*/


#ifdef __TEST__
	int _RegressionTest(int argc, char *argv[]);
	/*------------------------------------------------------
	 * Pre  : argc and argv are valid
	 * Post : Tests GLString
	 *
	 *        Returns 0 on sucess, or an error id
	 *
	 *-- History ------------------------------------------
	 *
	 * 2002.01.01:
	 * Mongoose - Created
	 ------------------------------------------------------*/
#endif

 private:

	unsigned int _num_string_max;	   /* Max number of strings buffered */

	unsigned int _num_font_max;	   /* Max number of font faces */

	unsigned int _num_font;	         /* Current number of font faces */

	unsigned int _num_string;	      /* Current number of strings buffered */

	int *_font_texture;	            /* Font texture mapping to actual
												   texture index */

	int *_font_base;	               /* Font GL list, base index list */

	gl_string_t *_string;            /* Buffered strings and their
													properities */

	float _scale;                    /* Default scale factor for new strings */
};

#endif
