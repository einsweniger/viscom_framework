//
// Created by bone on 09.07.18.
//

#ifndef GLSLVIEWER_DRAWABLE_H
#define GLSLVIEWER_DRAWABLE_H


struct drawable {
    virtual void draw2D() = 0;
};

struct updates_value {
    virtual bool update_value() = 0;
};

struct uploads_value {
    virtual bool upload_value() = 0;
};

struct contains_value {

};


#endif //GLSLVIEWER_DRAWABLE_H
