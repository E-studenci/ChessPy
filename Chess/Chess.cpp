// Chess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Board.h"

int main()
{
    
    Board b("1r2kbnr/pb1qpppp/Q1n5/1Np5/2P5/1K1B3N/PP1P2PP/R1B2q1R w k - 0 13");
    std::cout << b.ToString();
    Move m(new int[] {6, 6}, new int[] {4, 6});
    b.MakeMove(m);
    std::cout << b.ToString();

    Move m2(new int[] {4, 6}, new int[] {3, 6});
    b.MakeMove(m2);
    std::cout << b.ToString();
    Move m3(new int[] {1, 5}, new int[] {3, 5});
    b.MakeMove(m3);
    std::cout << b.ToString();
    Move m4(new int[] {3, 6}, new int[] {2, 5});
    b.MakeMove(m4);
    std::cout << b.ToString();
    Move m5(new int[] {5, 1}, new int[] {5, 2});
    b.MakeMove(m5);
    std::cout << b.ToString();
    Move m6(new int[] {5, 2}, new int[] {0, 7});
    b.MakeMove(m6);
    std::cout << b.ToString();
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
