# 4re5-engine

A simple 3d mesh viewer written in pure C (only X11 lib imported, not even standard libraries).

In the future will be a game engine.


## How it works?
Based on projection matrixes to display 3d points on a 2d screen.


## Future improvements
- [ ] Handle any input obj in `__ARESengine__loadObjectFromFile`
- [ ] Secure all type conversion in [conversion.c](./engine/conversions.c)
- [ ] Optimize all operations to maximum in `__ARESengine__displayUpdate`
- [ ] Transform every polygons into triangles when parsing obj files