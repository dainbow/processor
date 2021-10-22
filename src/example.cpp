#include <stdio.h>
#include <TXLib.h>

int main() {
    txCreateWindow(500, 500);
    for (int x = 0; x < 500; x++) {
        for (int y = 0; y < 500; y++) {
            long circle = (x - 250)*(x - 250) + (y - 250)*(y - 250);
            if ((circle >= 500) && (circle <= 1000)) {
                txSetPixel(x, y, TX_GREEN);
            }
        }
    }
}