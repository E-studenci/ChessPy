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
    //Board b("1r2kbnr/pb1qpppp/Q1n5/1Np5/2P5/1K1B3N/PP1P2PP/R1B2q1R w kQ - 0 13");
    //std::cout << b.ToString();
    //Move m(Coordinates(6,6), Coordinates(4,6));
    //b.MakeMove(m);
    //std::cout << b.ToString();

    //Move m2(Coordinates(4, 6), Coordinates(3, 6));
    //b.MakeMove(m2);
    //std::cout << b.ToString();

    //Move m3(Coordinates(1,5), Coordinates(3,5));
    //b.MakeMove(m3);
    //std::cout << b.ToString();

    //Move m4(Coordinates(3, 6), Coordinates(2, 5));
    //b.MakeMove(m4);
    //std::cout << b.ToString();

    //Move m5(Coordinates(5, 1), Coordinates(5, 2));
    //b.MakeMove(m5);
    //std::cout << b.ToString();

    //Move m6(Coordinates(5, 2), Coordinates(0,7));
    //b.MakeMove(m6);
    //std::cout << b.ToString();


    //b.Pop();
    //std::cout << b.ToString();
    //b.Pop();
    //std::cout << b.ToString();
    //b.Pop();
    //std::cout << b.ToString();
    //b.Pop();
    //std::cout << b.ToString();
    //b.Pop();
    //std::cout << b.ToString();
    //b.Pop();
    //std::cout << b.ToString();

    //Board b2{ "r3k2r/pppppppp/1nbq1bn1/8/8/1NBQ1BN1/PPPPPPPP/R3K2R w KQkq - 0 1"};
    //// K
    //std::cout << b2.ToString();
    //b2.MakeMove(Move{Coordinates(7,4), Coordinates(7, 6)});
    //std::cout << b2.ToString();
    //b2.Pop();
    //// Q   
    //std::cout << b2.ToString();
    //b2.MakeMove(Move{ Coordinates(7,4), Coordinates(7, 2) });
    //std::cout << b2.ToString();
    //b2.Pop();
    ////k
    //std::cout << b2.ToString();
    //b2.MakeMove(Move{ Coordinates(0,4), Coordinates(0, 6) });
    //std::cout << b2.ToString();
    //b2.Pop();
    ////q
    //std::cout << b2.ToString();
    //b2.MakeMove(Move{ Coordinates(0,4), Coordinates(0, 2) });
    //std::cout << b2.ToString();
    //b2.Pop();

    //Board b5{ "1n2k1b1/pppppppp/r1qnr2b/3PB3/4K3/6R1/PPP1PPPP/1N1Q1BNR w - - 0 1" };
    //std::cout << b5.ToString();

    //b5.GetAllLegalMoves();

    //Board b6{ "rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 3" };
    //std::cout << b6.ToString();
    //b6.MakeMove(Move{ Coordinates{3,4}, Coordinates{2,3} });
    //std::cout << b6.ToString();
    //b6.Pop();
    //std::cout << b6.ToString();


    //auto legalMoves = b5.GetAllLegalMoves();
    //b5.MakeMove(Move{ Coordinates{4,4}, Coordinates{5,5} });
    //b5.GetAllLegalMoves();
    //for (const std::pair<const Coordinates, std::vector<Move>>& keyValuePair : legalMoves)
    //    for (const Move& move : keyValuePair.second)
    //        std::cout << "xx";
    //
 /*   std::cout << b5.LegalMovesToString();
    std::cout << b5.AttackedFieldsToString();*/
    /* Board b7{ "rnbq1bnr/pppp1ppp/8/8/1k2p1R1/8/PPPPPPPP/RNBQKB1N w Q - 0 7" };
     b7.GetAllLegalMoves();
     b7.MakeMove(Move{ Coordinates{6,3}, Coordinates{4,3} });

     Board b8{ "rnbq1bnr/pp1ppppp/8/8/R1p3k1/8/PPPPPPPP/RNBQKB1N w Q - 0 7" };
     b8.GetAllLegalMoves();
     b8.MakeMove(Move{ Coordinates{6,3}, Coordinates{4,3} });

     Board b9{ "rnbq1bnr/pp1ppppp/2R5/8/2p5/8/P1kPPPPP/RNB1KB1N w Q - 0 7" };
     b9.GetAllLegalMoves();
     b9.MakeMove(Move{ Coordinates{6,3}, Coordinates{4,3} });*/

     //std::cout << "\nperft, motherfuckers\n";
     //Algorithms alg;
     //std::string perft1 = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
     /// <summary>
     /// 0 	1 	
     /// 1 	20 
     /// 2 	400 
     /// 3 	8, 902 
     /// 4 	197, 281
     /// 5 	4, 865, 609 
     /// 6 	119, 060, 324 
     /// 7 	3, 195, 901, 860
     /// </summary>
     /// <returns></returns>
     //std::string perft2 = "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1";
     /// <summary>
     /// 0 	48 	
     /// 1 	2039 
     /// 2 	97862 
     /// 3 	4085603
     /// 4 	193690690
     /// 5 	8031647685
     /// </summary>
     /// <returns></returns>
     //std::string perft3 = "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1";
     /// <summary>
     /// 0 	14 	
     /// 1 	191 
     /// 2 	2812 
     /// 3 	43238
     /// 4 	674624
     /// 5 	11030083
     /// 6   178633661
     /// </summary>
     /// <returns></returns>
     //std::string perft4 = "r3k2r/Pppp1ppp/1b3nbN/nP6/BBP1P3/q4N2/Pp1P2PP/R2Q1RK1 w kq - 0 1";
     //std::string perft4b = "r2q1rk1/pP1p2pp/Q4n2/bbp1p3/Np6/1B3NBn/pPPP1PPP/R3K2R b KQ - 0 1";
     //std::string perft5 = "rnbq1k1r/pp1Pbppp/2p5/8/2B5/8/PPP1NnPP/RNBQK2R w KQ - 1 8";
     //std::string perft6 = "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10";

     //Board* b = new Board{ perft1 };
    auto start = std::chrono::high_resolution_clock::now();
    //std::cout << b->ToString();
    //b->MakeMove(Move{ Coordinates{"g1"}, Coordinates{"h1"} });
    //std::cout << b->ToString();
    //std::cout << b->LegalMovesToString();
    //b->MakeMove(Move{ Coordinates{"b2"}, Coordinates{"a1"}, 11 });
    //std::cout << b->ToString();
    //b->MakeMove(Move{ Coordinates{"d1"}, Coordinates{"a1"} });
    //std::cout << b->ToString();
    //b->GetAllLegalMoves();


    //b->GetAllLegalMoves();
    //std::cout << alg.PerftStarterSingleThread(b,1, true) << "\n";

    //std::cout << alg.PerftStarterSingleThread(b, 1) << "\n";
    //std::cout << alg.PerftStarterSingleThread(b, 2) << "\n";
    //std::cout << alg.PerftStarterSingleThread(b, 3) << "\n";
    //std::cout << alg.PerftStarterSingleThread(b, 4) << "\n";
    //std::cout << alg.PerftStarterSingleThread(b, 5) << "\n";
    //std::cout << alg.PerftStarterSingleThread(b, 6) << "\n";
    /*std::cout << alg.PerftStarterSingleThread(b, 7) << "\n";
    std::cout << alg.PerftStarterSingleThread(b, 8) << "\n";
    std::cout << alg.PerftStarterSingleThread(b, 9) << "\n";
    std::cout << alg.PerftStarterSingleThread(b, 10) << "\n";
    std::cout << alg.PerftStarterSingleThread(b, 11) << "\n";*/
    //std::cout << alg.PerftStarterSingleThread(b, 12) << "\n";



    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    //std::cout << duration.count() << std::endl;
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
    std::cout << "ALPHA BETA\n";

    //Board bb3{ "Q2qkbnr/p2p1ppp/b3p3/3Q4/p1p5/4PN2/R1P2PPP/1NB1KB1R b Kk - 0 1" };
    //std::cout << "\n hash: " << bb3.hash.Verify(bb3);
    //bb3.MakeMove(Move{ Coordinates{0,4}, Coordinates{1,4} });
    //bb3.Pop();
    //std::cout << "\n hash: " << bb3.hash.Verify(bb3);
    //bb3.MakeMove(Move{ Coordinates{0,4}, Coordinates{1,4} });
    //bb3.Pop();
    //std::cout << "\n hash: " << bb3.hash.Verify(bb3);
    //bb3.MakeMove(Move{ Coordinates{0,4}, Coordinates{1,4} });
    //bb3.Pop();
    //std::cout << "\n hash: " << bb3.hash.Verify(bb3);

    Board bb{ "1rbqkbnr/p1pp1ppp/4p3/3Q4/p7/4PN2/1PP2PPP/RNB1KB1R w KQk - 0 7" };
    //Board bb{ "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10" };
    Algorithms algg;
    start = std::chrono::high_resolution_clock::now();
    std::cout << algg.Root(&bb, 5).first.ToString();
    stop = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "\ntime: " << duration.count() << std::endl;
    std::cout << algg.count;
    std::cout << "\n quiesce depth: " << algg.max_depth;

    Board bb4{ "1rbqkbnr/p1pp1ppp/4p3/3Q4/p7/4PN2/1PP2PPP/RNB1KB1R b KQk - 0 7" };
    std::cout << "\n hash: " << bb.hash.Verify(bb4);

    std::cout<< algg.PerftStarterSingleThread(&bb, 5) << "\n";
    std::cout << algg.count;

    return 0;
}