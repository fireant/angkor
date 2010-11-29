/* Copyright (c) 2010 Mosalam Ebrahimi <m.ebrahimi@ieee.org>
*
*  Permission is hereby granted, free of charge, to any person obtaining a copy
*  of this software and associated documentation files (the "Software"), to deal
*  in the Software without restriction, including without limitation the rights
*  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*  copies of the Software, and to permit persons to whom the Software is
*  furnished to do so, subject to the following conditions:
*
*  The above copyright notice and this permission notice shall be included in
*  all copies or substantial portions of the Software.
*
*  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
*  THE SOFTWARE.
*/

#ifndef TWEAKBARGUI_H
#define TWEAKBARGUI_H

#include <osgGA/GUIEventHandler>

#include <AntTweakBar.h>

class ExportState
{
public:
        ExportState() : state ( false ) {};
		bool state;
};

struct TweakBarEventCallback : public osgGA::GUIEventHandler
{
        TweakBarEventCallback ( ExportState* s );

        /** do customized Event code. */
        virtual bool handle ( const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv );

        virtual void accept ( osgGA::GUIEventHandlerVisitor& v ) {
                v.visit ( *this );
        }

protected:
		ExportState* export_state;
		TwBar* toolsBar;
};

class TweakBarDrawable : public osg::Drawable
{
public:

        TweakBarDrawable();
		
        TweakBarDrawable ( const osg::Drawable& drawable, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY ): 
				Drawable(drawable,copyop) {}

        META_Object ( osg, TweakBarDrawable );

        void drawImplementation ( osg::RenderInfo& renderInfo ) const;

        osg::Camera* createHUD ( int width, int height );

protected:

		virtual ~TweakBarDrawable();
};

#endif // TWEAKBARGUI_H
