Prophets of the Decieved / Zennia(Devon Rutledge) & Jammss
MP / Midterm Project
In the program the user can explore a world with different objects and lights.
Players can be controlled by turning with a&d and moving forward ad back with w&s. The camera can be controlled by the mouse.
Pressing x will cycle which hero you control.
Hero 1/Jammss also moves forward if they press s.
Pressing c will cycle which camera view you use.
Camera 1 is arcball, you can use shift+mouse to zoom.
Camera 2 is a free camera, you can use WASD to turn and space/shift+space to move forward and back.
Camera 3 is a first person camera, the camera will follow the hero and you have no other control.
I compile & run on windows via
```
mkdir build
cd build
cmake -GNinja ..
cd ..
cmake --build build
./build/lab05.exe
```
The lighting is a very high in the middle. The shading is bad in some places due to low poly.
Zennia - I did the base code, my character, the lighting code, and the scenery and fixed the first person camera.
Jammss - Did the freecam, their character, and character and camera switching.
The third person in our team dropped the class.
5 hours
5
8