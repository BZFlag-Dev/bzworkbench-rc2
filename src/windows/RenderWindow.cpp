/* BZWorkbench
 * Copyright (c) 1993 - 2010 Tim Riker
 *
 * This package is free software;  you can redistribute it and/or
 * modify it under the terms of the license found in the file
 * named COPYING that should have accompanied this file.
 *
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#include "windows/RenderWindow.h"
#include <FL/names.h>

// constructor with model
RenderWindow::RenderWindow() : 
    Fl_Gl_Window(DEFAULT_WIDTH, DEFAULT_HEIGHT) 
{
    end();

    // initialize the OSG embedded graphics window
    _gw = new osgViewer::GraphicsWindowEmbedded(x(), y(), DEFAULT_WIDTH, DEFAULT_HEIGHT);

    // make this resizable
    resizable(this);
}

RenderWindow::RenderWindow(int x, int y, int width, int height) : 
    Fl_Gl_Window(x, y, width, height) 
{
    end();

    // initialize the OSG embedded graphics window
    _gw = new osgViewer::GraphicsWindowEmbedded(x,y,width,height);

    // make this resizable
    resizable(this);

}

// resize method
void RenderWindow::resize(int x, int y, int w, int h) {
    // resize the OSG render window
    _gw->getEventQueue()->windowResize(x, y, w, h );
    _gw->resized(x, y, w, h);

    // resize the FLTK window
    Fl_Gl_Window::resize(x,y,w,h);
}


// The osg trackball uses the following conventions to manipulate a view:
// Mouse button 1 down plus move -> rotate
// Mouse button 2 down plus move -> translate
// Mouse button 3 down plus move -> scale
//
// We need those buttons for other stuff however, so instead
// we do the following translation:
//   Mouse button 1  -> Mouse button 1
//   Mouse button 1 + control -> Mouse button 2
//   Mouse button 1 + alt -> Mouse button 3
//   Mouse button 3  -> 'c'
//
// event handler
int RenderWindow::handle(int event) {
    int result = 1;
    // forward FLTK events to OSG

    if ((event == FL_PUSH) || (event == FL_RELEASE)) {
        if (Fl::event_button() == FL_LEFT_MOUSE) {
            int button = 1;
            int state = Fl::event_state();
            if (state & FL_CTRL) {
                button = 2;
            } else if (state & FL_ALT) {
                button = 3;
            }
            switch(event){
                case FL_PUSH:
                    _gw->getEventQueue()->mouseButtonPress(
                            Fl::event_x(), Fl::event_y(), button);
                    break;
                case FL_RELEASE:
                    _gw->getEventQueue()->mouseButtonRelease(
                            Fl::event_x(), Fl::event_y(), button);
                    break;
            }
        } else { // Middle or Right button
            if (event == FL_PUSH) {
                _gw->getEventQueue()->keyPress(
                        (osgGA::GUIEventAdapter::KeySymbol)'c');
            } else {
                _gw->getEventQueue()->keyRelease(
                        (osgGA::GUIEventAdapter::KeySymbol)'c');
            }
        }
    } else {
        switch(event){
            case FL_DRAG:
                _gw->getEventQueue()->mouseMotion(Fl::event_x(), Fl::event_y());
                break;
            case FL_KEYDOWN:
                _gw->getEventQueue()->keyPress(
                        (osgGA::GUIEventAdapter::KeySymbol)Fl::event_key());
                break;
            case FL_KEYUP:
                _gw->getEventQueue()->keyRelease(
                        (osgGA::GUIEventAdapter::KeySymbol)Fl::event_key());
                break;
            case FL_MOVE:
                result = Fl_Gl_Window::handle(event);
                break;
            case FL_NO_EVENT:
                result = 1;
                break;
            default:
                // pass other events to the base class
                result = Fl_Gl_Window::handle(event);
                break;
        }
    }
    return result;
}

