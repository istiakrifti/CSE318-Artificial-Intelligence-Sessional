#include <bits/stdc++.h>
using namespace std;

class Mancala {

    int binNumber;
    int stones;
    int size;
    vector<int> board;
    vector<int> indexVec;

public:

    Mancala(int b, int st) 
    {
        binNumber = b;
        stones = st;
        size = 2 * binNumber + 2;
        board = vector<int>(size + 4, 0);
        indexVec = vector<int>(binNumber,0);
        for (int i = 0; i < binNumber; i++) 
        {
            board[i] = stones;
            board[binNumber + 1 + i] = stones;
            indexVec[i] = i;
        }
    }

    int getStonesOnSide(vector<int> currentBoard, int player) 
    {
        int total = 0;
        int p = (player == 0) ? 0 : binNumber + 1;
        int len = (player == 0) ? binNumber : size - 1;
        for (int i = p; i < len; i++) 
        {
            total += currentBoard[i];
        }
        return total;
    }

    int heuristic1(vector<int> currentBoard, int player) 
    {
        if (player == 0) 
        {
            return currentBoard[binNumber] - currentBoard[size - 1];
        }
        return currentBoard[size - 1] - currentBoard[binNumber];
    }

    int heuristic2(vector<int> currentBoard, int player) 
    {
        int w1 = 50, w2 = 25;
        if (player == 0) 
        {
            return w1 * (currentBoard[binNumber] - currentBoard[size - 1]) + w2 * (getStonesOnSide(currentBoard, player) - getStonesOnSide(currentBoard, 1 - player));
        }
        return w1 * (currentBoard[size - 1] - currentBoard[binNumber]) + w2 * (getStonesOnSide(currentBoard, 1 - player) - getStonesOnSide(currentBoard, player));
    }

    int heuristic3(vector<int> currentBoard, int player) 
    {
        int w1 = 10, w2 = 20, w3 = 30;
        if (player == 0) 
        {
            return w1 * (currentBoard[binNumber] - currentBoard[size - 1]) + w2 * (getStonesOnSide(currentBoard, player) - getStonesOnSide(currentBoard, 1 - player)) +
                   w3 * currentBoard[size];
        }
        return w1 * (currentBoard[size - 1] - currentBoard[binNumber]) + w2 * (getStonesOnSide(currentBoard, 1 - player) - getStonesOnSide(currentBoard, player)) +
               w3 * currentBoard[size + 1];
    }

    int heuristic4(vector<int> currentBoard, int player) 
    {
        int w1 = 15, w2 = 40, w3 = 60, w4 = 80;
        if (player == 0) 
        {
            return w1 * (currentBoard[binNumber] - currentBoard[size - 1]) + w2 * (getStonesOnSide(currentBoard, player) - getStonesOnSide(currentBoard, 1 - player)) +
                   w3 * currentBoard[size] + w4 * currentBoard[size + 2];
        }
        return w1 * (currentBoard[size - 1] - currentBoard[binNumber]) + w2 * (getStonesOnSide(currentBoard, 1 - player) - getStonesOnSide(currentBoard, player)) +
               w3 * currentBoard[size + 1] + w4 * currentBoard[size + 3];
    }

    int evaluate(int h, vector<int> currentBoard, int player) 
    {
        if (h == 1) return heuristic1(currentBoard, player);
        else if (h == 2) return heuristic2(currentBoard, player);
        else if (h == 3) return heuristic3(currentBoard, player);
        else if (h == 4) return heuristic4(currentBoard, player);
        return 0;
    }

    bool isGameOver(vector<int> &Bins) 
    {
        bool bin1 = true;
        bool bin2 = true;

        for (int i = 0; i < binNumber; i++) 
        {
            if (Bins[i] != 0) bin1 = false;
            if (Bins[binNumber + 1 + i] != 0) bin2 = false;
        }

        if(bin1) 
        {
            // Bins[size - 1] += getStonesOnSide(Bins, 1);
            for (int i = binNumber + 1; i < size - 1; i++) 
            {
                Bins[size - 1] += Bins[i];
                Bins[i] = 0;
            }
        }
        if(bin2) 
        {
            for (int i = 0; i < binNumber; i++) 
            {
                Bins[binNumber] += Bins[i];
                Bins[i] = 0;
            }
        }

        return bin1 || bin2;
    }

    pair<vector<int>, bool> makeMove(vector<int> newBoard, int player, int bin) 
    {
        bool extraMove = false;
        int startIdx = -1;
        if(player == 0) startIdx = bin;
        else startIdx = binNumber + 1 + bin;
        int st = newBoard[startIdx];
        if (st == 0) return {newBoard, extraMove};
        
        
        newBoard[startIdx] = 0;
        int idx = startIdx;

        while (st > 0) 
        {
            idx = (idx + 1) % size;
            if(player == 0 && idx == size - 1) continue;
            else if(player == 1 && idx == binNumber) continue;
            newBoard[idx]++;
            st--;
        }

        if ((player == 0 && idx == binNumber) || (player == 1 && idx == size-1)) 
        {
            extraMove = true;
            if(player == 0) newBoard[size]++;
            else newBoard[size + 1]++;
        }

        if (player == 0 && idx < binNumber && newBoard[idx] == 1 && newBoard[size - 2 - idx] > 0) 
        {
            int x = newBoard[size - 2 - idx] + 1;
            newBoard[binNumber] += x;
            newBoard[idx] = 0;
            newBoard[size - 2 - idx] = 0;
            newBoard[size + 2] += x;
        } 
        else if (player == 1 && idx >= binNumber + 1 && idx < size - 1 && newBoard[idx] == 1 && newBoard[size - 2 - idx] > 0) 
        {
            int x = newBoard[size - 2 - idx] + 1;
            newBoard[size - 1] += x;
            newBoard[idx] = 0;
            newBoard[size - 2 - idx] = 0;
            newBoard[size + 3] += x;
        }

        return {newBoard, extraMove};
    }

    pair<int, int> minimax(vector<int> currentBoard, int depth, int player, int alpha, int beta, int h1, int h2) 
    {
        if (depth == 0 || isGameOver(currentBoard)) 
        {
            if(player == 0) return {evaluate(h1, currentBoard, player), -1};
            else if (player == 1) return {evaluate(h2, currentBoard, player), -1};
        }

        if (player == 0) 
        {
            int maxVal = INT_MIN;
            int bestMove = -1;
            random_device rd;
            mt19937 g(rd());
            shuffle(indexVec.begin(), indexVec.end(), g);
            for (int i = 0; i < binNumber; i++) 
            {
                if (currentBoard[i] != 0)
                {
                    vector<int> newBoard = currentBoard;
                    auto p = makeMove(newBoard, player,indexVec[i]);
                    bool extraMove = p.second;
                    int eval;

                    if (extraMove) 
                    {
                        eval = minimax(p.first, depth, player, alpha, beta, h1, h2).first;
                    }
                    else 
                    {
                        eval = minimax(p.first, depth - 1, 1, alpha, beta, h1, h2).first;
                    }

                    // cout << "Player 1 evaluating move " << i << ": " << eval << endl;

                    if (eval > maxVal) 
                    {
                        maxVal = eval;
                        bestMove = i;
                    }

                    alpha = max(alpha, eval);
                    if (beta <= alpha) 
                    {
                        break;
                    }
                }
            }

            return {maxVal, bestMove};
        } 
        else if (player == 1)
        {
            int minVal = INT_MAX;
            int bestMove = -1;
            random_device rd;
            mt19937 g(rd());
            shuffle(indexVec.begin(), indexVec.end(), g);
            for (int i = 0; i < binNumber; i++) 
            {
                if (currentBoard[binNumber + 1 + i] != 0)
                {
                    vector<int> newBoard = currentBoard;
                    auto p = makeMove(newBoard, player, indexVec[i]);
                    bool extraMove = p.second;
                    int eval;

                    if (extraMove) 
                    {
                        eval = minimax(p.first, depth, player, alpha, beta, h1, h2).first;
                    }
                    else 
                    {
                        eval = minimax(p.first, depth - 1, 0, alpha, beta, h1, h2).first;
                    }

                    // cout << "Player 2 evaluating move " << i << ": " << eval << endl;

                    if (eval < minVal) 
                    {
                        minVal = eval;
                        bestMove = i;
                    }

                    beta = min(beta, eval);
                    if (beta <= alpha) 
                    {
                        break;
                    }
                }
            }

            return {minVal, bestMove};
        }
    }


    void printBoard() 
    {
        // cout<<"Present Situation"<<endl;
        cout << "     Player 2" << endl;
        cout << "   ";
        for (int i = binNumber - 1; i >= 0; i--) 
        {
            cout << board[binNumber + 1 + i] << " ";
        }
        cout << endl;
        cout << board[size - 1] << "\t\t " << board[binNumber] << endl;
        cout << "   ";
        for (int i = 0; i < binNumber; i++) 
        {
            cout << board[i] << " ";
        }
        cout << endl;
        cout << "     Player 1" << endl << endl;
    }

    int PlayGround(int depth, int heuristic1, int heuristic2, int player) 
    {
        while (!isGameOver(board)) 
        {
            // cout<<"Player " << player + 1 <<"'s turn"<<endl;
            // printBoard();

            auto result = minimax(board, depth, player, INT_MIN, INT_MAX, heuristic1, heuristic2);
            int bestMove = result.second;

            if (bestMove != -1) 
            {
                auto p = makeMove(board, player, bestMove);
                board = p.first;
                bool extraMove = p.second;

                if (!extraMove) 
                {
                    player = 1 - player;
                }
            } 
            else 
            {
                cout << "No valid moves for player " << player + 1 << "!" << endl;
                break;
            }
        }

        // printBoard();
        // cout << "Game Over! Player 1: " << board[binNumber] << " stones, Player 2: " << board[size -1] << " stones." << endl;

        // if (board[binNumber] > board[2 * binNumber + 1])
        //     cout << "Player 1 wins!" << endl;
        // else if (board[binNumber] < board[2 * binNumber + 1])
        //     cout << "Player 2 wins!" << endl;
        // else
        //     cout << "It's a tie!" << endl;

        if (board[binNumber] > board[2 * binNumber + 1]) return 1;
        else if (board[binNumber] < board[2 * binNumber + 1]) return 2;
        else return 0;
    }

    void PlayGround2(int depth) 
    {
        int player = rand() % 2;
        int heuristic2 = 4;
        while (!isGameOver(board)) 
        {
            printBoard();
            cout<<"Player " << player + 1 <<"'s turn"<<endl;

            if(player == 0)
            {
                int x;
                cout<<"Select a bin:";
                cin>>x;

                auto p = makeMove(board, player, x);
                board = p.first;
                bool extraMove = p.second;

                if (!extraMove) 
                {
                    player = 1 - player;
                }
            }
            else 
            {
                auto result = minimax(board, depth, player, INT_MIN, INT_MAX, 0, heuristic2);
                int bestMove = result.second;

                if (bestMove != -1) 
                {
                    auto p = makeMove(board, player, bestMove);
                    board = p.first;
                    bool extraMove = p.second;

                    if (!extraMove) 
                    {
                        player = 1 - player;
                    }
                } 
            }

        }

        printBoard();
        cout << "Game Over! Player 1: " << board[binNumber] << " stones, Player 2: " << board[size -1] << " stones." << endl;

        if (board[binNumber] > board[2 * binNumber + 1])
            cout << "Player 1 wins!" << endl;
        else if (board[binNumber] < board[2 * binNumber + 1])
            cout << "Player 2 wins!" << endl;
        else
            cout << "It's a tie!" << endl;


    }

};

int main() 
{
    int binNumber = 6;
    int stones = 4;
    int depth = 6;
    int numOfMatch = 100;
    int startingPlayer;

    for(int i = 1; i<=4; i++)
    {
        for(int j = i+1; j<=4; j++)
        {
            int win1 = 0;
            int win2 = 0;
            int draw = 0;
            for(int k = 1; k<=numOfMatch; k++)
            {
                startingPlayer = rand() % 2;
                Mancala m(binNumber, stones);
                int val = m.PlayGround(depth,i,j,startingPlayer);
                if(val == 1) win1++;
                else if(val == 2) win2++;
                else  draw++;
            }
            cout<<"Heuristic"<<i<< " vs Heuristic"<<j<<endl;
            cout<<"Heuristic"<<i<<" wins: "<<win1<<", Heuristic"<<j<<" wins: "<<win2<<", Draw: "<<draw<<endl;
            cout<<"Heuristic"<<i<<" winning ratio: "<<((double)win1)/numOfMatch<<", "<<"Heuristic"<<j<<" winning ratio: "<<((double)win2)/numOfMatch<<endl<<endl;
        }
    }
    // m.PlayGround(depth);

    return 0;
}
