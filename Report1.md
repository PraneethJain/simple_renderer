# Assignment 1: Bounding Volume Hierarchies

# Question 1

The discrepancy in the renderings is caused due to floating point precision. All the coordinates have been shifted by 1 million, but since the relative distance between the pixels is still very small, adding this distance to the large overall coordinates doesn't make a difference, thus losing the depth of the image.

This snippet demonstrates the loss in precision
```cpp
#include <iostream>

int main()
{
    float large{1e6};
    float small{0.1};

    float new_large{large + 0.01f};
    float new_small{small + 0.01f};
    std::cout << new_large - large << ' ' << new_small - small << std::endl;

    return 0;
}
```
> 0 0.01

We would expect the output to be `0.01 0.01`, but instead we get `0 0.01`.

The solution is to shift the origin of the entire scene to the camera. Through this, all the coordinates will be adjusted according to the camera, thus reducing their values to be small enough for high precision arithmetic.

Another approach would be to shift all the coordinates by the average of all the vertices of the scene. This minimizes the floating point magnitudes the most.