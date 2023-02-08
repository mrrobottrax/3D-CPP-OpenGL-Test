#pragma once

#include <iostream>

inline void HSVtoRGB(float H, float S, float V, float* R, float* G, float* B) {
    if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
        std::cout << "The given HSV values are not in valid range" << std::endl;
        return;
    }
    float s = S / 100;
    float v = V / 100;
    float C = s * v;
    float X = float(C * (1 - abs(fmod(H / 60.0, 2) - 1)));
    float m = v - C;
    float r, g, b;
    if (H >= 0 && H < 60) {
        r = C, g = X, b = 0;
    }
    else if (H >= 60 && H < 120) {
        r = X, g = C, b = 0;
    }
    else if (H >= 120 && H < 180) {
        r = 0, g = C, b = X;
    }
    else if (H >= 180 && H < 240) {
        r = 0, g = X, b = C;
    }
    else if (H >= 240 && H < 300) {
        r = X, g = 0, b = C;
    }
    else {
        r = C, g = 0, b = X;
    }
    *R = r + m;
    *G = g + m;
    *B = b + m;
}

inline void RandomHueColor(float* R, float* G, float* B)
{
    HSVtoRGB(float(rand() % 360), 100, 100, R, G, B);
}