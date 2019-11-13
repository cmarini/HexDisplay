#include "layer.h"

Layer* Layer::base = NULL;
Layer* Layer::top = NULL;

uint8_t Layer::layerCount = 0;

Layer::Layer()
{
    layerCount++;
    above = NULL;
    below = NULL;
    if (base == NULL) {
        base = this;
    } else {
        top->above = this;
        below = top;
    }
    top = this;
    
    pixelRowLength_lut_init();
    pixelRowStart_lut_init();
    
    /* Set each pixel's row/col and adjacency */
    for (int row = 0; row < PIXEL_ROW_COUNT; row++) {
        for (int col = 0; col < pixelRowLength_lut[row]; col++) {
            int i = rc_to_i(row,col);
            pixels[i].loc.row = row;
            pixels[i].loc.col = col;
            uint8_t adj = 0;
            bool first_row = row == 0;
            bool last_row = row == HEX_SIDE_LENGTH*2-2;
            bool first_col = col == 0;
            bool last_col = col == pixelRowLength_lut[row]-1;
            bool upper_half_row = row < HEX_SIDE_LENGTH; // Includes middle row
            bool lower_half_row = row >= HEX_SIDE_LENGTH-1; // Excludes middle row
            /* DIR_N */
            if (!(first_row) &&
                !(last_col && upper_half_row)
            ) {
                adj |= DIR_N;
            }
            /* DIR_NE */
            if (!(last_col)
            ) {
                adj |= DIR_NE;
            }
            /* DIR_SE */
            if (!(last_row) &&
                !(last_col && lower_half_row)
            ) {
                adj |= DIR_SE;
            }
            /* DIR_S */
            if (!(last_row) &&
                !(first_col && lower_half_row)
            ) {
                adj |= DIR_S;
            }
            /* DIR_SW */
            if (!(first_col)
            ) {
                adj |= DIR_SW;
            }
            /* DIR_NW */
            if (!(first_row) &&
                !(first_col && upper_half_row)
            ) {
                adj |= DIR_NW;
            }
            pixels[i].adjacency = adj;
        }
    }
}

Layer::~Layer()
{
    layerCount--;
    if (below) {
        if (above) {
            below->above = this->above;
            above->below = this->below;
        }
        else {
            top = below;
            below->above = NULL;
        }
    } 
    else {
        if (above) {
            base = above;
            above->below = NULL;
        }
        else {
            base = NULL;
            top = NULL;
        }
    }
}

uint8_t Layer::getLayerCount()
{
    return layerCount;
}

Layer* Layer::getBase()
{
    return base;
}
Layer* Layer::getTop()
{
    return top;
}

Layer* Layer::getBelow()
{
    return this->below;
}
Layer* Layer::getAbove()
{
    return this->above;
}
Layer* Layer::moveDown()
{
    Layer* b = below;
    if (b == NULL) {
        return NULL;
    }
    if (b->below != NULL) {
        below = b->below;
        b->below->above = this;
    } else {
        base = this;
    }
    b->below = this;
    above = b;
    return this;
}
Layer* Layer::moveUp()
{
    Layer* a = above;
    Layer* b = below;
    if (a == NULL) {
        return NULL;
    }
    if (a->above != NULL) {
        above = a->above;
        a->above->below = this;
    } else {
        top = this;
        above = NULL;
    }
    if (b == NULL) {
        base = a;
    } else {
        b->above = a;
    }
    a->above = this;
    below = a;
    a->below = b;
    return this;
}

Layer* Layer::moveToTop()
{
    while (this->above && this->moveUp());
    return this;
}
Layer* Layer::moveToBottom()
{
    while (this->below && this->moveDown());
    return this;
}

Pixel* Layer::getPixel(uint8_t idx)
{
    if (idx >= PIXEL_COUNT) {
        return NULL;
    }
    return &pixels[idx];
}
Pixel* Layer::getPixel(uint8_t row, uint8_t col)
{
    return getPixel(rc_to_i(row,col));
}

Pixel* Layer::getRandomPixel()
{
    return &pixels[random(PIXEL_COUNT)];
}
Pixel* Layer::getCenterPixel()
{
    return &pixels[PIXEL_COUNT/2];
}

Pixel* Layer::getCornerPixel(dir_t dir)
{
    return getLinAdjacent(getCenterPixel(), dir, HEX_SIDE_LENGTH-1);
}

Pixel* Layer::getAdjacent(Pixel* pixel, dir_t dir)
{
    int row = pixel->loc.row;
    int col = pixel->loc.col;
    int idx = rc_to_i(row, col);
    if (!pixel->hasAdjacent(dir)) {
        return NULL;
    }
    switch(dir) {
        case DIR_N: {
            if (row < HEX_SIDE_LENGTH) { /* Middle row or higher */
                idx -= pixelRowLength_lut[row]-1;
            } else {
                idx -= pixelRowLength_lut[row];
            }
            break;
        }
        case DIR_NE: {
            idx += 1;
            break;
        }
        case DIR_SE: {
            if (row < HEX_SIDE_LENGTH-1) {
                idx += pixelRowLength_lut[row]+1;
            } else {
                idx += pixelRowLength_lut[row];
            }
            break;
        }
        case DIR_S: {
            if (row < HEX_SIDE_LENGTH-1) {
                 idx += pixelRowLength_lut[row];
            } else {
                idx += pixelRowLength_lut[row]-1;
            }
            break;
        }
        case DIR_SW: {
            idx -= 1;
            break;
        }
        case DIR_NW: {
            if (row < HEX_SIDE_LENGTH) { /* Middle row or higher */
                idx -= pixelRowLength_lut[row];
            } else {
                idx -= pixelRowLength_lut[row]+1;
            }
            break;
        }
        default: {
            return NULL;
            // break;
        }
    }
    return &this->pixels[idx];  
}

Pixel* Layer::getLinAdjacent(Pixel* pixel, dir_t dir, unsigned int len)
{
    Pixel* c = pixel;
    for (int i = 0; i < len; i++)
    {
        c = this->getAdjacent(c, dir);
        if (c == NULL) {
            return NULL;
        }
    }
    return c;
}

