/**************************************************************************
 *
 * Copyright 2011 Jose Fonseca
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 **************************************************************************/

#ifndef _GLSTATE_HPP_
#define _GLSTATE_HPP_


#include <ostream>

#include "glimports.hpp"


class JSONWriter;


namespace image {
    class Image;
}


namespace glstate {

struct TextureInfo {
    TextureInfo() : width(0), height(0), depth(0), format(0) {}
    TextureInfo(GLint w, GLint h, GLint d, GLint f) :
        width(w), height(h), depth(d), format(f) {}
    GLint width;
    GLint height;
    GLint depth;
    GLint format;
};

const char *enumToString(GLenum pname);

void dumpEnum(JSONWriter &json, GLenum pname);

void dumpParameters(JSONWriter &json);

void dumpCurrentContext(std::ostream &os);

image::Image *
getDrawBufferImage(void);


} /* namespace glstate */


#endif /* _GLSTATE_HPP_ */
