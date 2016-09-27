# Spherical Harmonics Playground

This is a simple application that loads an OBJ file and an environment map and calculates & displays the corresponding 3rd order spherical harmonics coefficients for it.
The coefficients can also be modified at runtime to see the effect of each coefficient.


Here is a dog statue (that I attempted to scan a few years ago) in a room environment:
![dog statue in room environment](https://github.com/ands/spherical_harmonics_playground/raw/master/result_images/dog.jpg)


And here is a simple sphere in an outdoor environment:
![sphere in outdoor environment](https://github.com/ands/spherical_harmonics_playground/raw/master/result_images/sphere.jpg)


Linux dependencies for glfw: xorg-dev libgl1-mesa-dev
```
git clone --recursive https://github.com/ands/spherical_harmonics_playground.git
cd spherical_harmonics_playground
cmake .
make
./playground
```

dickyjim has collected various resources regarding spherical harmonics on his [blog](https://dickyjim.wordpress.com/2013/09/04/spherical-harmonics-for-beginners/).