#include "pixel.h"

bool Pixel::areEqualRGB(rgba_t* rgb1, rgba_t* rgb2)
{
    if (rgb1->r == rgb2->r &&
        rgb1->g == rgb2->g &&
        rgb1->b == rgb2->b )
    {
        return true;
    }
    return false;
}

Pixel::Pixel()
{
    this->current.r = 0;
    this->current.g = 0;
    this->current.b = 0;
    this->current.a = 0;
    this->target.r = 0;
    this->target.g = 0; 
    this->target.b = 0;
    this->target.a = 0;
    this->startTime = 0;
    this->duration = 0;
    this->hold = 0;
    this->callback = NULL;
    this->complete = false;
    this->loc.row = 0;
    this->loc.col = 0;
    this->adjacency = 0;
}

rgba_t Pixel::getTarget()
{
    return this->target;
}
cval_t Pixel::getTargetR()
{
    return this->target.r;
}
cval_t Pixel::getTargetG()
{
    return this->target.g;
}
cval_t Pixel::getTargetB()
{
    return this->target.b;
}
cval_t Pixel::getTargetA()
{
    return this->target.a;
}

rgba_t Pixel::getCurrent()
{
    return this->current;
}
cval_t Pixel::getCurrentR()
{
    return this->current.r;
}
cval_t Pixel::getCurrentG()
{
    return this->current.g;
}
cval_t Pixel::getCurrentB()
{
    return this->current.b;
}
cval_t Pixel::getCurrentA()
{
    return this->current.a;
}

void Pixel::update(uint32_t timeNow)
{
    if (!this->complete) {
        if (timeNow >= this->startTime + this->duration) {
            // Transition time is up
            this->current = this->target;
            if (timeNow >= this->startTime + this->duration + this->hold) {
                this->complete = true;
                if (this->callback != NULL) {
                    (this->callback)(this);
                }
            }
        } else {
            if (timeNow < this->startTime) {
                /* Still in the waiting period */
                this->current = this->start;
                return;
            }
            uint32_t remaining = (this->startTime + this->duration) - timeNow;
            this->current.r = map(remaining, this->duration, 0, this->start.r, this->target.r);
            this->current.g = map(remaining, this->duration, 0, this->start.g, this->target.g);
            this->current.b = map(remaining, this->duration, 0, this->start.b, this->target.b);
            this->current.a = map(remaining, this->duration, 0, this->start.a, this->target.a);
        }
    }
}

Pixel* Pixel::setDuration(uint32_t d)
{
    this->duration = d;
    return this;
}
Pixel* Pixel::setHold(uint32_t h)
{
    this->hold = h;
    return this;
}

bool Pixel::isTransitionComplete()
{
    return this->complete;
}
Pixel* Pixel::uncomplete()
{
    this->complete = false;
    return this;
}

Pixel* Pixel::setCallback(PixelCallback_t c)
{
    this->callback = c;
    return this;
}

Pixel* Pixel::unsetCallback()
{
    this->callback = NULL;
    return this;
}

Pixel* Pixel::doCallback()
{
    if (this->callback != NULL) {
        (this->callback)(this);
    }
    return this;
}

uint32_t Pixel::getIdealCompleteTime()
{
    return this->startTime + this->duration + this->hold;
}

Pixel* Pixel::stop()
{
    this->target = this->current;
    this->startTime = 0;
    this->complete = 1;
    return this;
}

Pixel* Pixel::off()
{
    rgba_t c = {0,0,0};
    return setColor(&c);
}

Pixel* Pixel::setColor(rgba_t* p_rgb)
{
    this->duration = 0;
    this->hold = 0;
    return setTransition(p_rgb, 0);
}

Pixel* Pixel::setTransition(rgba_t* p_rgb, uint32_t start)
{
    this->startTime = start;
    
    p_rgb->r = min(p_rgb->r, R_VAL_LIMIT);
    p_rgb->g = min(p_rgb->g, G_VAL_LIMIT);
    p_rgb->b = min(p_rgb->b, B_VAL_LIMIT);
    p_rgb->a = min(p_rgb->a, A_VAL_LIMIT);
    
    this->start = this->current;    
    this->target = *p_rgb;    
    this->complete = false;
    
    return this;
}

bool Pixel::setTransitionIf(rgba_t* p_rgb, uint32_t start, uint32_t duration, PIXEL_SET_CONDITION_t condition)
{
    
    if (condition == DIFF_DURATION || condition == DIFF_BOTH) {
        if (duration == this->duration) {
            return false;
        }
    }
    if (p_rgb->r == this->target.r && 
        p_rgb->g == this->target.g && 
        p_rgb->b == this->target.b && 
        p_rgb->a == this->target.a )
    {
        if (condition == DIFF_COLOR || condition == DIFF_BOTH) {
            return false;
        }
        if (condition == DIFF_EITHER && duration == this->duration) {
            return false;
        }
    }
    setTransition(p_rgb, start);
    this->duration = duration;
    return true;
}

Pixel* Pixel::setColor(hsva_t* p_hsv)
{
    this->duration = 0;
    this->hold = 0;
    return setTransition(p_hsv, 0);
}

Pixel* Pixel::setTransition(hsva_t* p_hsv, uint32_t start)
{
    rgba_t rgb;
    hsv_to_rgb(p_hsv, &rgb);
    return setTransition(&rgb, start);
}

bool Pixel::setTransitionIf(hsva_t* p_hsv, uint32_t start, uint32_t duration, PIXEL_SET_CONDITION_t condition)
{
    rgba_t rgb;
    hsv_to_rgb(p_hsv, &rgb);
    return setTransitionIf(&rgb, start, duration, condition);
}

bool Pixel::hasAdjacent(dir_t d)
{
    return !!(d & this->adjacency);
}





bool Pixel::isEdge()
{
    return (this->adjacency != (DIR_N|DIR_NE|DIR_SE|DIR_S|DIR_SW|DIR_NW));
}

int Pixel::getIndex()
{
    return rc_to_i(this->loc.row, this->loc.col);
}


rgba_t* hsv_to_rgb(hsva_t* p_hsv, rgba_t* p_rgb)
{
    p_rgb->a = p_hsv->a;
    // Determine the Hue
    while (p_hsv->h >= H_VAL_LIMIT) {
        p_hsv->h -= H_VAL_LIMIT;
    }
    
    //if (p_hsv->h >= 0 && p_hsv->h < H_VAL_SECTION) {
    if (p_hsv->h < H_VAL_SECTION) { // unnecessary comparison (p_hsv->h >= 0)
        p_rgb->r = R_VAL_LIMIT;
        p_rgb->g = map(p_hsv->h, H_VAL_SECTION*0, H_VAL_SECTION*1, 0, G_VAL_LIMIT);
        p_rgb->b = 0;
    } else if (p_hsv->h < H_VAL_SECTION*2) {
        p_rgb->r = map(p_hsv->h, H_VAL_SECTION*1, H_VAL_SECTION*2, R_VAL_LIMIT, 0);
        p_rgb->g = G_VAL_LIMIT;
        p_rgb->b = 0;
    } else if (p_hsv->h < H_VAL_SECTION*3) {
        p_rgb->r = 0;
        p_rgb->g = G_VAL_LIMIT;
        p_rgb->b = map(p_hsv->h, H_VAL_SECTION*2, H_VAL_SECTION*3, 0, B_VAL_LIMIT);
    } else if (p_hsv->h < H_VAL_SECTION*4) {
        p_rgb->r = 0;
        p_rgb->g = map(p_hsv->h, H_VAL_SECTION*3, H_VAL_SECTION*4, G_VAL_LIMIT, 0);
        p_rgb->b = B_VAL_LIMIT;
    } else if (p_hsv->h < H_VAL_SECTION*5) {
        p_rgb->r = map(p_hsv->h, H_VAL_SECTION*4, H_VAL_SECTION*5, 0, R_VAL_LIMIT);
        p_rgb->g = 0;
        p_rgb->b = B_VAL_LIMIT;
    } else if (p_hsv->h < H_VAL_SECTION*6) {
        p_rgb->r = R_VAL_LIMIT;
        p_rgb->g = 0;
        p_rgb->b = map(p_hsv->h, H_VAL_SECTION*5, H_VAL_SECTION*6, B_VAL_LIMIT, 0);
    } else {
        p_rgb->r = 0;
        p_rgb->g = 0;
        p_rgb->b = 0;
    }
    
    // Modify for Saturation
    p_hsv->s = min(p_hsv->s, S_VAL_LIMIT);
    if(p_hsv->s < S_VAL_LIMIT) {
        p_rgb->r = map(p_hsv->s, 0, S_VAL_LIMIT, R_VAL_LIMIT, p_rgb->r); 
        p_rgb->g = map(p_hsv->s, 0, S_VAL_LIMIT, G_VAL_LIMIT, p_rgb->g); 
        p_rgb->b = map(p_hsv->s, 0, S_VAL_LIMIT, B_VAL_LIMIT, p_rgb->b); 
    }
    
    // Modify for Vibrancy
    p_hsv->v = min(p_hsv->v, V_VAL_LIMIT);
    if (p_hsv->v < V_VAL_LIMIT) {
        p_rgb->r = map(p_hsv->v, 0, V_VAL_LIMIT, 0, p_rgb->r); 
        p_rgb->g = map(p_hsv->v, 0, V_VAL_LIMIT, 0, p_rgb->g); 
        p_rgb->b = map(p_hsv->v, 0, V_VAL_LIMIT, 0, p_rgb->b); 
    }
    return p_rgb;
}

hsva_t* rgb_to_hsv(rgba_t* p_rgb, hsva_t* p_hsv)
{
    p_hsv->a = p_rgb->a;
    cval_t maxVal = max(p_rgb->r, max(p_rgb->g, p_rgb->b));
    cval_t minVal = min(p_rgb->r, min(p_rgb->g, p_rgb->b));
    
    // Vibrancy
    p_hsv->v = map(maxVal, 0, RGB_VAL_LIMIT, 0, V_VAL_LIMIT);
    // p_hsv->v = constrain(p_hsv->v, 0, V_VAL_LIMIT);
    p_hsv->v = min(p_hsv->v, V_VAL_LIMIT);
    
    // Saturation
    p_hsv->s = map(minVal, 0, maxVal, S_VAL_LIMIT, 0);
    p_hsv->s = min(p_hsv->s, S_VAL_LIMIT);
    
    // Hue
    if (p_rgb->b == minVal && p_rgb->r == maxVal)
    {
        p_hsv->h = map(p_rgb->g, minVal, maxVal, H_VAL_SECTION*0, H_VAL_SECTION*1);
    }
    else if (p_rgb->b == minVal && p_rgb->g == maxVal)
    {
        p_hsv->h = map(p_rgb->r, maxVal, minVal, H_VAL_SECTION*1, H_VAL_SECTION*2);
    }
    else if (p_rgb->r == minVal && p_rgb->g == maxVal)
    {
        p_hsv->h = map(p_rgb->b, minVal, maxVal, H_VAL_SECTION*2, H_VAL_SECTION*3);
    }
    else if (p_rgb->r == minVal && p_rgb->b == maxVal)
    {
        p_hsv->h = map(p_rgb->g, maxVal, minVal, H_VAL_SECTION*3, H_VAL_SECTION*4);
    }
    else if (p_rgb->g == minVal && p_rgb->b == maxVal)
    {
        p_hsv->h = map(p_rgb->r, minVal, maxVal, H_VAL_SECTION*4, H_VAL_SECTION*5);
    }
    else if (p_rgb->g == minVal && p_rgb->r == maxVal)
    {
        p_hsv->h = map(p_rgb->b, maxVal, minVal, H_VAL_SECTION*5, H_VAL_SECTION*6);
    }
    p_hsv->h = min(p_hsv->h, H_VAL_LIMIT);
    
    return p_hsv;
}
