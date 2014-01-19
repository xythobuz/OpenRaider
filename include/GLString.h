/*!
 * \file include/GLString.h
 * \brief Open GL rendering font/string class
 *
 * \author Mongoose
 * \author xythobuz
 */

#ifndef _GLSTRING_H_
#define _GLSTRING_H_

/*!
 * \brief Internal data structure representing GL Strings
 */
typedef struct gl_string_s {
    int x; //!< X Coordinate
    int y; //!< Y Coordinate
    float scale; //!< Scale factor
    char *text; //!< Text buffer
    bool active; //!< active state
    unsigned short int len; //!< length
} gl_string_t;

/*!
 * \brief Open GL rendering font/string class
 */
class GLString {
public:

    /*!
     * \brief Constructs an object of GLString
     */
    GLString();

    /*!
     * \brief Deconstructs an object of GLString
     */
    ~GLString();

    /*!
     * \brief Set max number of strings
     * \param max_strings maximum number of strings
     */
    void Init(unsigned int max_strings);

    /*!
     * \brief Sets a single byte in a string
     * \param string valid gl string id
     * \param pos position in that gl string to set
     * \param c character to set in gl string
     */
    void SetChar(unsigned int string, unsigned int pos, char c);

    /*!
     * \brief Gets number of bytes in a string buffer
     * \param string valid gl string id
     * \returns length/number of bytes of string
     */
    unsigned int GetStringLen(unsigned int string);

    /*!
     * \brief Returns string buffer
     * \param string valid gl string id
     * \returns pointer to string buffer
     */
    char *GetBuffer(unsigned int string);


    void setActive(unsigned int string, bool active);

    /*!
     * \brief Sets text in a string.
     * It will be truncated as needed to fit
     * \param string valid string id
     * \param s format string and args like for printf
     */
    void SetString(unsigned int string, const char *s, ...);

    /*!
     * \brief Sets default text scaling
     * \param scale new scale factor > 0.0
     */
    void Scale(float scale);

    /*!
     * \brief Generates a new string and renders it to the gl target
     * \param x valid X screen coordinate
     * \param y valid Y screen coordinate
     * \param string valid format string with args like for printf
     * \returns 0 on success, -1 on invalid string, -2 on full string list
     */
    int glPrintf(int x, int y, const char *string, ...);

    /*!
     * \brief Renders strings over GL scene.
     * Should be called after scene is rendered.
     * GL Culling should be disabled.
     * \param width width of GL context
     * \param height height of GL context
     */
    void Render(int width, int height);

    /*!
     * \brief Get the String data structure for a string id
     * \param id valid string id
     * \returns string for id, or NULL if it does not exist
     */
    gl_string_t *GetString(unsigned int id);


private:
    unsigned int _num_string_max; //!< Max number of strings buffered
    unsigned int _num_string; //!< Current number of strings buffered
    gl_string_t *_string; //!< Buffered strings and their properities
    float _scale; //!< Default scale factor for new strings
};

#endif
