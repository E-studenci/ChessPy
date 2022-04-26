// Chess.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "Board.h"
#include "Coordinates.h"
#include "Algorithms.h"

#include <iostream>
#include <chrono>
#include <set>
#include "Coordinates.h"

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

    Board b5{ "1n2k1b1/pppppppp/r1qnr2b/3PB3/4K3/6R1/PPP1PPPP/1N1Q1BNR w - - 0 1" };
    std::cout << b5.ToString();

    b5.GetAllLegalMoves();
    std::cout << b5.LegalMovesToString();
    std::cout << b5.AttackedFieldsToString();

    std::cout << "\nperft, motherfuckers\n";
    Algorithms alg;
    std::cout << alg.PerftStarterSingleThread(new Board{"rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"}, 1) << "\n";
    std::cout << alg.PerftStarterSingleThread(new Board{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" }, 2) << "\n";
    std::cout << alg.PerftStarterSingleThread(new Board{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" }, 3) << "\n";
    std::cout << alg.PerftStarterSingleThread(new Board{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" }, 4) << "\n";
    std::cout << alg.PerftStarterSingleThread(new Board{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" }, 5) << "\n";
    std::cout << alg.PerftStarterSingleThread(new Board{ "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" }, 6) << "\n";


    /*auto start = std::chrono::high_resolution_clock::now();
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
    std::cout << duration.count() << std::endl;*/
    return 0;
}