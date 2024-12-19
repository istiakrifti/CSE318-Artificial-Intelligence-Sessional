#include<bits/stdc++.h>
using namespace std;

class Node
{
    vector<vector<string>> board;
    int stepCost;
    int heuristicCost;
    int totalCost;
    Node* parent;

    public:

    Node(vector<vector<string>> b,int s,int h,Node *p)
    {
        board = b;
        stepCost = s;
        heuristicCost = h;
        totalCost = s+h;
        parent = p;
    }

    int getTotalCost() 
    {
        return totalCost;
    }

    int getStepCost() 
    {
        return stepCost;
    }

    int getHeuristicCost() 
    {
        return heuristicCost;
    }

    Node* getParent()
    {
        return parent;
    }

    vector<vector<string>> getBoard() 
    {
        return board;
    }
};

class Npuzzle
{
    int k;
    vector<vector<string>> initialBoard;
    vector<vector<string>> goalBoard;

    vector<vector<string>> createGoalBoard()
    {
        int val = 1;
        
        vector<vector<string>> board(k,vector<string>(k));

        for(int i=0;i<k;i++)
        {
            for(int j=0;j<k;j++)
            {
                if(i==k-1 && j==k-1) 
                {
                    board[i][j] = "*";
                }
                else 
                {
                    board[i][j] = to_string(val);
                    val++;
                }
            }
        }  

        return board;    
    }

    vector<string> makeFlatBoard()
    {
        vector<string> flatBoard;

        for(int i=0;i<k;i++)
        {
            for(int j=0;j<k;j++) 
            {
                if(initialBoard[i][j]!="*") flatBoard.push_back(initialBoard[i][j]);
            }
        }

        return flatBoard;
    }

    int countInversion()
    {
        vector<string> flatBoard = makeFlatBoard();
        int s = flatBoard.size();

        int invCount = 0;
        for(int i=0;i<s;i++)
        {
            for(int j=i+1;j<s;j++) 
            {
                if(flatBoard[i]>flatBoard[j]) invCount++;
            }
        }

        return invCount;
    }

    pair<int,int> getBlankPosition(vector<vector<string>> initialBoard)
    {
        for(int i=0;i<k;i++)
        {
            for(int j=0;j<k;j++) 
            {
                if(initialBoard[i][j]=="*") return {i,j};
            }
        }
    }

    int HammingCost(vector<vector<string>> currentBoard)
    {
        int cost = 0;

        for(int i=0;i<k;i++)
        {
            for(int j=0;j<k;j++) 
            {
                if(currentBoard[i][j]!="*" && currentBoard[i][j]!=goalBoard[i][j])
                {
                    cost++;
                }
            }
        }

        return cost;
    }

    int ManhattanCost(vector<vector<string>> currentBoard)
    {
        int cost = 0;

        for(int i=0;i<k;i++)
        {
            for(int j=0;j<k;j++) 
            {
                if(currentBoard[i][j]!="*" && currentBoard[i][j]!=goalBoard[i][j])
                {
                    int value = stoi(currentBoard[i][j]);
                    int actualrow = (value-1)/k;
                    int actualcol = (value-1)%k; 

                    cost += (abs(actualrow-i)+abs(actualcol-j));
                }
            }
        }

        return cost;
    }

    bool isSameBoard(vector<vector<string>> currentBoard)
    {
        for(int i=0;i<k;i++)
        {
            for(int j=0;j<k;j++) 
            {
                if(currentBoard[i][j]!=goalBoard[i][j])
                {
                    return false;
                } 
            }
        }

        return true;
    }

    vector<vector<vector<string>>> neighbors(vector<vector<string>> currentBoard)
    {
        vector<vector<vector<string>>> result;
        pair<int,int> blankPos = getBlankPosition(currentBoard);
        int row = blankPos.first;
        int col = blankPos.second;

        vector<pair<int, int>> directions = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        for(int i = 0; i<directions.size();i++) 
        {
            pair<int, int> p = directions[i];
            int r = row + p.first;
            int c = col + p.second;

            if (r >= 0 && r < k && c >= 0 && c < k) 
            {
                vector<vector<string>> newBoard = currentBoard;
                string temp = newBoard[row][col];
                newBoard[row][col] = newBoard[r][c];
                newBoard[r][c] = temp;

                result.push_back(newBoard);
            }
        }
        return result;
    }

    void printSteps(Node *current)
    {
        if(current==0) return;
        printSteps(current->getParent());
        vector<vector<string>> board = current->getBoard();
        for(int i=0;i<board.size();i++)
        {
            for(int j=0;j<board.size();j++) 
            {
                cout<<board[i][j]<<" ";
            }
            cout<<endl;
        }
        cout<<endl;
    }

    public:

    Npuzzle(int size,vector<vector<string>> board)
    {
        k = size;
        initialBoard = board;
        goalBoard = createGoalBoard();
    }

    bool isSolvable()
    {
        int invertionCount = countInversion();

        if(k%2!=0) 
        {
            if(invertionCount%2!=0) return false;
            return true;
        }
        else
        {
            pair<int,int> blankPosition = getBlankPosition(initialBoard);
            int row = k - blankPosition.first;

            if(row%2==0) 
            {
                if(invertionCount%2==0) return false;
                return true;
            }
            else 
            {
                if(invertionCount%2!=0) return false;
                return true;
            }
        }
    }

    void AstarAlgo(string hst)
    {
        int h = 0;
        if(hst=="h") h = HammingCost(initialBoard);
        else if(hst=="m") h = ManhattanCost(initialBoard);
        else return;

        int explored = 0,expanded = 0;

        Node *node = new Node(initialBoard,0,h,0);

        priority_queue<pair<int,Node*>,vector<pair<int,Node*>>,greater<>> nodesQueue;
        unordered_set<string> visited;
        
        int t = node->getTotalCost();
        nodesQueue.push({t,node});

        while(!nodesQueue.empty())
        {
            pair<int,Node*> current = nodesQueue.top();
            nodesQueue.pop();

            vector<vector<string>> currentBoard = current.second->getBoard();
            string boardString = "";

            for(int i=0;i<currentBoard.size();i++)
            {
                for(int j=0;j<currentBoard.size();j++) 
                {
                    boardString += currentBoard[i][j];
                }
            }

            if(isSameBoard(currentBoard)) 
            {
                cout<<"Step Cost: "<<current.second->getStepCost()<<endl;
                cout<<"Steps: "<<endl;
                printSteps(current.second);
                cout<<"Explored nodes: "<<explored<<endl;
                cout<<"Expanded nodes: "<<expanded<<endl<<endl;
                
                return;
            }

            if(visited.find(boardString)!=visited.end()) continue;
            visited.insert(boardString);
            expanded++;

            vector<vector<vector<string>>> neighbor = neighbors(currentBoard);

            for(int i=0;i<neighbor.size();i++)
            {
                int hc = 0;
                if(hst=="h") hc = HammingCost(neighbor[i]);
                else if(hst=="m") hc = ManhattanCost(neighbor[i]);
                
                string bString = "";

                for(int j=0;j<neighbor[i].size();j++)
                {
                    for(int p=0;p<neighbor[i].size();p++) 
                    {
                        bString += neighbor[i][j][p];
                    }
                }
                if(visited.find(bString)==visited.end())
                {
                    Node *newNode = new Node(neighbor[i],current.second->getStepCost()+1,hc,current.second);
                
                    nodesQueue.push({newNode->getTotalCost(),newNode});
                    explored++;
                }
                
            }
        }
    }
};



int main()
{
    int k;
    cout<<"Enter the grid size:"<<endl;
    cin>>k;

    vector<vector<string>> initialBoard(k,vector<string>(k));
    cout<<"Enter the initial board"<<endl;
    for(int i=0;i<k;i++)
    {
        for(int j=0;j<k;j++) 
        {
            string x;
            cin>>x;
            initialBoard[i][j] = x;
        }
    }
    
    Npuzzle puzzle(k,initialBoard);

    if(puzzle.isSolvable()) 
    {
        cout<<"Using Hamming:"<<endl;
        puzzle.AstarAlgo("h");
        cout<<"Using Manhattan:"<<endl;
        puzzle.AstarAlgo("m");
    }
    else cout<<"Not Solvable"<<endl;


    return 0;
}