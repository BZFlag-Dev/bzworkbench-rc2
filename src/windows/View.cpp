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
#include <osgDB/ReadFile>

#include "windows/View.h"
#include "dialogs/MenuBar.h"
#include "objects/waterLevel.h"

const double View::DEFAULT_ZOOM = 75.0;

// view constructor
View::View(Model* m, MainWindow* _mw, int _x, int _y, int _w, int _h, const char *_label) : RenderWindow(_x,_y,_w,_h) {

    // set OSG viewport
    this->getCamera()->setViewport(new osg::Viewport(0,0,_w,_h));
    this->getCamera()->setProjectionMatrixAsPerspective(30.0f, static_cast<double>(_w)/static_cast<double>(_h), 1.0f, 10000.0f);
    this->getCamera()->setGraphicsContext(getGraphicsWindow());
    this->setThreadingModel(osgViewer::Viewer::SingleThreaded);

    // add the root node to the scene
    osg::ref_ptr<osg::Node> loadedModel = osgDB::readNodeFile("../Sandbox/SimpleView/OpenSceneGraph-Data/cessna.osg");
    this->setSceneData( loadedModel.get() );

    // give the View a trackball manipulator
    osgGA::TrackballManipulator* cameraManipulator = new osgGA::TrackballManipulator();
    this->setCameraManipulator(cameraManipulator);
    this->model = m;
    this->addEventHandler(new osgViewer::StatsHandler);

    // assign the parent reference
    this->mw = _mw;
}


// build the mouse button map
void View::buildMouseButtonMap() {
    mouseButtonMap = map< unsigned int, unsigned int > ();

    // default mapppings

    mouseButtonMap[ FL_MIDDLE_MOUSE ] = FL_LEFT_MOUSE;		// middle mouse drags in FLTK should translate to left mouse drags in OSG

}

// destructor
View::~View() {
    if(eventHandlers)
        delete eventHandlers;
}


// draw method (really simple)
void View::draw(void) {
    frame();
}

// scale the selection based on the distance from the camera to the center to ensure it stays the same size
void View::updateSelection( float newDistance ) {
//    this->selection->setScale( osg::Vec3( 0.01 * newDistance, 0.01 * newDistance, 0.01 * newDistance ) );
}

void View::updateSelection() {
//    // get the distance from the eyepoint to the center of the trackball
//    float dist = this->cameraManipulatorRef->getDistance();
//    this->updateSelection( dist );
//    // refresh
    redraw();
}

// handle events
int View::handle(int event) {
    int result = 1;
    unsigned int e_x = Fl::event_x();
    unsigned int e_y = Fl::event_y();
    printf("  >>View::handle(int event)\n");
    result = RenderWindow::handle(event);
    printf("  <<View::handle(int event)\n");
    return result;
}

// update method (inherited from Observer)
void View::update( Observable* obs, void* data ) {
#if 0
    // refresh the selection
    selection->update( obs, data );

    // process data
    if( data != NULL ) {
        // get the message
        ObserverMessage* obs_msg = (ObserverMessage*)(data);

        // process the message
        switch( obs_msg->type ) {
            // add an object to the scene
            case ObserverMessage::ADD_OBJECT : 
                {
                    bz2object* obj = (bz2object*)(obs_msg->data);

                    if( getRootNode()->getNumChildren() > 0 )
                        getRootNode()->insertChild( 1, obj );	// insert the child directly after the Ground object
                    else
                        getRootNode()->addChild( obj );


                    break;
                }
                                               // remove an object from the scene
            case ObserverMessage::REMOVE_OBJECT : 
                {
                    bz2object* obj = (bz2object*)(obs_msg->data);
                    getRootNode()->removeChild( obj );

                    break;
                }
                // update the world size
            case ObserverMessage::UPDATE_WORLD : 
                {
                    // in this case, the data will contain a pointer to the modified world object
                    world* bzworld = (world*)(obs_msg->data);

                    root->removeChild( ground );
                    ground = new Ground( bzworld->getSize(), model->getWaterLevelData()->getHeight() );
                    root->insertChild(0, ground);

                    break;
                }
                // update an object (i.e. it's selection value changed, etc.)
            case ObserverMessage::UPDATE_OBJECT : 
                {
                    bz2object* obj = (bz2object*)(obs_msg->data);
                    if( obj->isSelected() )
                        SceneBuilder::markSelectedAndPreserveStateSet( obj );
                    else
                        SceneBuilder::markUnselectedAndRestoreStateSet( obj );

                    break;
                }
            default:
                break;
        }
    }
#endif
    // refresh the scene
    redraw();
}

// is a button pressed?
bool View::isPressed( int value ) {
	return modifiers[ value ];
}

bool View::isSelected( bz2object* obj ) { return this->model->isSelected( obj ); }
/**
 * Tell the model to select an object
 */
void View::setSelected( bz2object* object ) {

	this->model->_setSelected( object );
}

/**
 * Tell the model to unselect an object
 */
void View::setUnselected( bz2object* object ) {

	this->model->_setUnselected( object );
}


/**
 * Tell the model to unselect all selected objects
 */
void View::unselectAll() {

	this->model->_unselectAll();
}

