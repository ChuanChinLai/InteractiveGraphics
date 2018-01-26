#pragma once 

#include "LOpenGL.h"
#include <stdio.h>

//Screen Constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;


bool InitGL();

void Update();
void Input(unsigned char i_Key, int i_MouseX, int i_MouseY);
void Render();