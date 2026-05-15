#include "kern.h"

void wait_seconds(int seconds) {

    unsigned long count = seconds * 100000 * 1000; 
    for (unsigned long i = 0; i < count; i++) {
        __asm__ volatile("nop");
    }

}

void helloScreen()
{
    print("          *           \n");
    print("        * /*          \n");
    print("      *  /  *         \n");
    print("     *   /   *        \n");
    print("    *    /    *       \n");
    print("     **  |  **        \n");
    print("       * * *          \n");
    print("        **            \n");
    print("        *             \n");
    print("                      \n");
    print("         ______                 ______     \n");
    print(" |    |  |      |       |      |      |    \n");
    print(" |____|  |____  |       |      |      |    \n");
    print(" |    |  |      |       |      |      |    \n");
    print(" |    |  |____  |_____  |_____ |______|    \n");

}