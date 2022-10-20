#pragma once

#include "GlobalIncludes.h"

class Objects {

    map<int, Object3D *> _objects;
    map<Object3D *, string> _objectNames;
    map<string, int> _objectIDs;

public:

    void reset();

    int addObject(Object3D *object, const string &name);

    void addObject(Object3D *object, int id, string name);

    Object3D *getObject(string name);

    Object3D *getObject(int id);

    string getName(int id);

    string getName(Object3D *object);

    int getID(string name);

    int getID(Object3D *object);

    int objectCount();
};

