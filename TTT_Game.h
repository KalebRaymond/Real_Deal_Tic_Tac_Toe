#include <vector>
/* A class representing a game of tic tac toe */
class TTT_Game
{
    public:
        char board[3][3];
        std::vector<int> X_moves,
                         O_moves;
        std::vector<int> available_moves;

        char checkWin();
        void autoPlay();
        void printBoard();
        TTT_Game();

};

/* Class representing player O */
class Opponent
{
    public:
        std::vector<TTT_Game> play_history;
        double move_probability[9];

    //public:

        void playNewGames(int n);
        void playVsUser();
        Opponent(int training_session);

};
