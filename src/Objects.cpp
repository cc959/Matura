#include "Objects.h"

void Objects::reset() {
    _objects.clear();
    _objectIDs.clear();
    _objectNames.clear();
}

int Objects::addObject(Object3D *object, const string &name) {
    int id = _objects.size();
    addObject(object, id, name);
    return id;
}

void Objects::addObject(Object3D *object, int id, string name) {
    int num = -1;
    while (_objectIDs.count(name + (num == -1 ? "" : to_string(num)))) {
        num++;
    }

    assert(_objects.count(id) == 0);

    name = name + (num == -1 ? "" : to_string(num));

    _objects[id] = object;
    _objectNames[object] = name;
    _objectIDs[name] = id;
}

Object3D *Objects::getObject(string name) {
    return _objects[_objectIDs[name]];
}

Object3D *Objects::getObject(int id) {
    return _objects[id];
}

string Objects::getName(int id) {
    return _objectNames[_objects[id]];
}

string Objects::getName(Object3D *object) {
    return _objectNames[object];
}

int Objects::getID(string name) {
    return _objectIDs[name];
}

int Objects::getID(Object3D *object) {
    return _objectIDs[_objectNames[object]];
}

int Objects::objectCount() {
    return _objects.size();
}
