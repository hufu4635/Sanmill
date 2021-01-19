#include <cstdio>
#include <iostream>
#include <windows.h>
#include "mill.h"
#include "miniMaxAI.h"
#include "randomAI.h"
#include "perfectAI.h"

#include "config.h"

using namespace std;

unsigned int	startTestFromLayer = 0;
unsigned int	endTestAtLayer = NUM_LAYERS - 1;
#ifdef _DEBUG
char		databaseDirectory[] = "D:\\database";
#elif _RELEASE_X64
char		databaseDirectory[] = "";
#endif
bool			calculateDatabase = false;

void main(void)
{
    // locals
    bool			playerOneHuman = false;
    bool			playerTwoHuman = false;
    char			tmpChar[100];
    unsigned int	pushFrom, pushTo;
    Mill *myGame = new Mill();
    perfectAI *myKI = new perfectAI(databaseDirectory);

    SetPriorityClass(GetCurrentProcess(), BELOW_NORMAL_PRIORITY_CLASS);
    srand(GetTickCount());

    // intro
    cout << "*************************" << endl;
    cout << "* Muehle                *" << endl;
    cout << "*************************" << endl << endl;

    myKI->setDatabasePath(databaseDirectory);

    // begin
#ifdef SELF_PLAY
    myGame->beginNewGame(myKI, myKI, fieldStruct::playerOne);
#else
    myGame->beginNewGame(myKI, myKI, (rand() % 2) ? fieldStruct::playerOne : fieldStruct::playerTwo);
#endif // SELF_PLAY

    if (calculateDatabase) {

        // calculate
        myKI->calculateDatabase(MAX_DEPTH_OF_TREE, false);

        // test database
        cout << endl << "Begin test starting from layer: ";     startTestFromLayer;
        cout << endl << "End test at layer: ";                  endTestAtLayer;
        myKI->testLayers(startTestFromLayer, endTestAtLayer);

    } else {


#ifdef SELF_PLAY
        int moveCount = 0;
#else
        cout << "Is Player 1 human? (y/n):"; cin >> tmpChar;	if (tmpChar[0] == 'y') playerOneHuman = true;
        cout << "Is Player 2 human? (y/n):"; cin >> tmpChar;	if (tmpChar[0] == 'y') playerTwoHuman = true;
#endif // SELF_PLAY

        // play
        do {
            // print field
            cout << "\n\n\n\n\n\n\n\n\n\n\n";
            myGame->getComputersChoice(&pushFrom, &pushTo);
            cout << "\n\n";
            cout << "\nlast move was from " << (char)(myGame->getLastMoveFrom() + 97) << " to " << (char)(myGame->getLastMoveTo() + 97) << "\n\n";

#ifdef SELF_PLAY
            moveCount++;
            if (moveCount > 99) {
                goto out;
            }
#endif // SELF_PLAY

            myGame->printField();

            // Human
            if ((myGame->getCurrentPlayer() == fieldStruct::playerOne && playerOneHuman)
                || (myGame->getCurrentPlayer() == fieldStruct::playerTwo && playerTwoHuman)) {
                do {
                    // Show text
                    if (myGame->mustStoneBeRemoved())	cout << "\n   Which stone do you want to remove? [a-x]: \n\n\n";
                    else if (myGame->inSettingPhase())	cout << "\n   Where are you going? [a-x]: \n\n\n";
                    else								cout << "\n   Your train? [a-x][a-x]: \n\n\n";

                    // get input
                    cin >> tmpChar;
                    if ((tmpChar[0] >= 'a') && (tmpChar[0] <= 'x'))		pushFrom = tmpChar[0] - 'a';	else pushFrom = fieldStruct::size;

                    if (myGame->inSettingPhase()) {
                        if ((tmpChar[0] >= 'a') && (tmpChar[0] <= 'x'))	pushTo = tmpChar[0] - 'a';	else pushTo = fieldStruct::size;
                    } else {
                        if ((tmpChar[1] >= 'a') && (tmpChar[1] <= 'x'))	pushTo = tmpChar[1] - 'a';	else pushTo = fieldStruct::size;
                    }

                    // undo
                    if (tmpChar[0] == 'u' && tmpChar[1] == 'n' && tmpChar[2] == 'd' && tmpChar[3] == 'o') {

                        // undo moves until a human player shall move
                        do {
                            myGame->undoLastMove();
                        } while (!((myGame->getCurrentPlayer() == fieldStruct::playerOne && playerOneHuman)
                                   || (myGame->getCurrentPlayer() == fieldStruct::playerTwo && playerTwoHuman)));

                        // reprint field
                        break;
                    }

                } while (myGame->moveStone(pushFrom, pushTo) == false);

                // Computer
            } else {
                cout << "\n";
                myGame->moveStone(pushFrom, pushTo);
            }

        } while (myGame->getWinner() == 0);

        // end
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n";

        myGame->printField();

        if (myGame->getWinner() == fieldStruct::playerOne)	cout << "\n   Player 1 (o) won after " << myGame->getMovesDone() << " move.\n\n";
        else if (myGame->getWinner() == fieldStruct::playerTwo)	cout << "\n   Player 2 (x) won after " << myGame->getMovesDone() << " move.\n\n";
        else if (myGame->getWinner() == fieldStruct::gameDrawn)	cout << "\n   Draw!\n\n";
        else												    cout << "\n   A program error has occurred!\n\n";
    }

 out:
    char end;
    cin >> end;
}
