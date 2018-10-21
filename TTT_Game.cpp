#include "TTT_Game.h"
#include <stdlib.h>
#include <time.h>
#include <iostream>

/* State members */

char State::checkWin(int turn_count)
{
    /* 0 1 2  /
    /  3 4 5  /
    /  6 7 8 */
    //Check diagonals
    if(( board[0] == board[4] && board[0] == board[8] )
        || ( board[6] == board[4] && board[6] == board[2] ))
    {
        if(board[4] != '-');
            return board[4];
    }

    //Check rows & columns
    for(int i = 0; i < 3; ++i)
    {
        if(board[i * 3] == board[(i * 3) + 1] && board[i * 3] == board[(i * 3) + 2])
        {
            if(board[i * 3] != '-');
                return board[i * 3];
        }
        if(board[i] == board[3 + i] && board[i] == board[6 + i])
        {
           if(board[i] != '-');
                return board[i];
        }
    }

    //Check if game is drawn
    //(by checking for a draw after checking 3's in a row, this prevents returning draw if the game is won on the 9th move)
    if(turn_count == 9)
    {
        return 'D';
    }

    //Otherwise, the game is still going
    return '-';
}


void State::printBoard()
{
    for(int i = 0; i < 9; ++i)
    {
        std::cout << board[i];

        if(i % 3 == 2)
            std::cout << "\n";
    }

    std::cout << "\n";
}

State::State()
{
    static bool seeded = false;
    if(!seeded) {
        srand(time(NULL));
        seeded = true;
    }

    board = {'-', '-', '-', '-', '-', '-', '-', '-', '-'};
    score = 0.5;
    alpha = 0.1;
}

/* Opponent members */

void Opponent::play()
{
    //So I'm thinking the configuration of board[3][3] is s. Thus V(s) somehow determiens the value of that state s.
    //I suppose s' is a state that has already been encountered, which can be accessed from a database of states.
    /*   a. In each episode of the learning phase                                                                                   /
    /    1) Observe a current board state s;                                                                                        /
    /    2) Make a next move based on the distribution of all available V(s') of next moves;                                        /
    /    3) Record s' in a sequence;                                                                                                /
    /    4)  If  the  game  finishes,  it  updates  the  values  of  the  visited  states  in  the  sequence  and  starts  over     /
    /    again; otherwise, go to 1).                                                                                               */


    //Randomize players between [X, O] and [O, X] to simulate games starting with different player
    char players[2];
    std::vector<int> available_moves = { 0, 1, 2, 3, 4, 5, 6, 7, 8};
    int X_turn = rand() % 2;

    players[X_turn] = 'X';
    players[1 - X_turn] = 'O';
    int cur_move = rand() % available_moves.size();
    int turn_count = 1;
    int board_index = 0;

    std::vector<char> cur_board = {'-','-','-','-','-','-','-','-','-'};
    std::vector<int> state_indices;

    std::cout << "New game\n";

    for(int i = 0; i <= 8; ++i)
    {
        //Get legal move
        while(available_moves[cur_move] == -1)
        {
            cur_move = rand() % available_moves.size();
        }

        //Place X or O in board
        cur_board[available_moves[cur_move]] = players[i % 2];
        ++turn_count;

        //Mark move as taken
        available_moves[cur_move] = -1;

        board_index = this->getIndex(cur_board);

        //Cur_board has not been encountered before
        if(board_index == -1)
        {
            std::cout << "New state";
            play_history.push_back(State());

            for(int j = 0; j < 8; ++j)
            {
                //std::cout << cur_board[j] << " ";
                play_history[play_history.size() - 1].board[j] = cur_board[j];
            }

            state_indices.push_back(this->play_history.size() - 1);

            std::cout << "\n";

            if(play_history[play_history.size() - 1].checkWin(turn_count) != '-')
            {
                break;
            }
        }
        else
        {
            state_indices.push_back(board_index);
        }
    }

    switch(state_indices[state_indices.size() - 1])
    {
        case 'X': case 'D':
            this->play_history[board_index].score = 0;
            break;
        case 'O':
            this->play_history[board_index].score = 1;
            break;
    }

    for(int i = state_indices.size() - 2; i >= 0; --i)
    {
        //Current state score += alpha * (successive state's score - current state score)
        this->play_history[state_indices[i]].score += 0.1 * (this->play_history[state_indices[i + 1]].score - this->play_history[state_indices[i]].score);
    }

    //std::cout << cur_state.checkWin(turn_count) << "\n";
    //Evaluate states seen in
}


void Opponent::playNewGames(int n)
{
    for(int i = 0; i < n; ++i)
    {
        //play_history.push_back(State());
        play();
    }
}

void Opponent::playVsUser()
{
    State current_game;
    std::vector<int> available_moves = {0, 1, 2, 3, 4, 5, 6, 7, 8};
    int turn_count = 0;

    while(current_game.checkWin(turn_count) == '-')
    {
        current_game.printBoard();

        if(turn_count % 2 == 0)
        {
            int row = 0, column = 0;
            std::cout << "Enter row: ";
            std::cin >> row;
            std::cout << "Enter column: ";
            std::cin >> column;

            current_game.board[(row * 3) + column] = 'X';
            available_moves[(row * 3) + column] = -1;
        }
        else
        {
            int cur_move = rand() % available_moves.size();
            while(available_moves[cur_move] == -1)
            {
                cur_move = rand() % available_moves.size();
            }

            current_game.board[cur_move] = 'O';
            available_moves[cur_move] = -1;
        }

        ++turn_count;
    }

}

//Returns index of the state containing state_board as its board member in Opponent::play_history. If no state is found, returns -1
int Opponent::getIndex(std::vector<char> state_board)
{
    bool found = false;
    //s should be a State object
    for(int i = 0; i < play_history.size(); ++i)
    {
        for(int j = 0; j < 9; j++)
        {
            if(play_history[i].board[j] != state_board[j])
            {
                found = false;
                break;
            }

            found = true;
        }

        if(found)
        {
            return i;
        }
    }

    return -1;
}

Opponent::Opponent(int training_session)
{
    for(double &d : move_probability)
    {
        d = 0.0;
    }

    playNewGames(training_session);
}
