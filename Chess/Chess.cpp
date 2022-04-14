// Chess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Board.h"
#include <iostream>
#include <chrono>


int main()
{
    Board b("1r2kbnr/pb1qpppp/Q1n5/1Np5/2P5/1K1B3N/PP1P2PP/R1B2q1R w kQ - 0 13");
    std::cout << b.ToString();
    Move m(Coordinates(6,6), Coordinates(4,6));
    b.MakeMove(m);
    std::cout << b.ToString();

    Move m2(Coordinates(4, 6), Coordinates(3, 6));
    b.MakeMove(m2);
    std::cout << b.ToString();

    Move m3(Coordinates(1,5), Coordinates(3,5));
    b.MakeMove(m3);
    std::cout << b.ToString();

    Move m4(Coordinates(3, 6), Coordinates(2, 5));
    b.MakeMove(m4);
    std::cout << b.ToString();

    Move m5(Coordinates(5, 1), Coordinates(5, 2));
    b.MakeMove(m5);
    std::cout << b.ToString();

    Move m6(Coordinates(5, 2), Coordinates(0,7));
    b.MakeMove(m6);
    std::cout << b.ToString();


    b.Pop();
    std::cout << b.ToString();
    b.Pop();
    std::cout << b.ToString();
    b.Pop();
    std::cout << b.ToString();
    b.Pop();
    std::cout << b.ToString();
    b.Pop();
    std::cout << b.ToString();
    b.Pop();
    std::cout << b.ToString();

    Board b2{ "r3k2r/pppppppp/1nbq1bn1/8/8/1NBQ1BN1/PPPPPPPP/R3K2R w KQkq - 0 1"};
    // K
    std::cout << b2.ToString();
    b2.MakeMove(Move{Coordinates(7,4), Coordinates(7, 6)});
    std::cout << b2.ToString();
    b2.Pop();
    // Q   
    std::cout << b2.ToString();
    b2.MakeMove(Move{ Coordinates(7,4), Coordinates(7, 2) });
    std::cout << b2.ToString();
    b2.Pop();
    //k
    std::cout << b2.ToString();
    b2.MakeMove(Move{ Coordinates(0,4), Coordinates(0, 6) });
    std::cout << b2.ToString();
    b2.Pop();
    //q
    std::cout << b2.ToString();
    b2.MakeMove(Move{ Coordinates(0,4), Coordinates(0, 2) });
    std::cout << b2.ToString();
    b2.Pop();

    Board b3("rnb2bnr/ppp1pppp/8/3k4/3p2q1/1Q1R4/PPPPPPPP/1NB1KBNR b K - 1 1");
    std::cout << b3.ToString();

    b3.CalculateLegalMoves();
    std::cout << b3.AttackedFieldsToString();
    std::cout << "byebye";

    auto start = std::chrono::high_resolution_clock::now();
    Board b4("r1b1k3/p1p2ppp/1pnp1q1r/1Q2pn2/PbNP2B1/1P2NPR1/2PBPKPP/1R6 w q - 1 1");
    for (int i = 0; i < 1000000; i++) {
        for (int row = 0; row < 8;row++) {
            for (int column = 0; column < 8;column++) {
                b4.attackedFields[0][row][column] = false;
                b4.attackedFields[1][row][column] = false;
                b4.defendedFields[0][row][column] = false;
                b4.defendedFields[1][row][column] = false;
            }
        }
        b4.attackedLines[0].clear();
        b4.attackedLines[1].clear();
        b4.CalculateLegalMoves();
    }
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << duration.count() << std::endl;
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
