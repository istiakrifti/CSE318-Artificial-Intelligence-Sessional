#include<bits/stdc++.h>
using namespace std;

class City 
{
    public:

    int id;
    double x, y;

    City(int id, double x, double y) 
    {
        this->id = id;
        this->x = x;
        this->y = y;
    }
};

class TSP 
{
    vector<City> cities;

    double distance(City a, City b) 
    {
        return sqrt(pow(a.x - b.x, 2) + pow(a.y - b.y, 2));
    }

    public:

    TSP(vector<City>& cities) 
    {
        this->cities = cities;
    }

    double calculateTourDistance(vector<int>& tour)
    {
        double totalDistance = 0.0;
        for (int i = 0; i < tour.size(); ++i) 
        {
            totalDistance += distance(cities[tour[i]], cities[tour[(i + 1) % tour.size()]]);
        }
        return totalDistance;
    }

    // Nearest Neighbor Heuristic
    vector<int> nearestNeighbor()
    {
        vector<int> tour;
        vector<bool> visited(cities.size(), false);

        int startCity = 0;
        int currentCity = startCity;
        tour.push_back(currentCity);
        visited[currentCity] = true;

        for (int i = 1; i < cities.size(); ++i) 
        {
            double minDistance = numeric_limits<double>::max();
            int nearestCity = -1;

            for (int j = 0; j < cities.size(); ++j) 
            {
                if (!visited[j]) 
                {
                    double dist = distance(cities[currentCity], cities[j]);
                    if (dist < minDistance) 
                    {
                        minDistance = dist;
                        nearestCity = j;
                    }
                }
            }

            currentCity = nearestCity;
            visited[currentCity] = true;
            tour.push_back(currentCity);
        }

        tour.push_back(startCity);
        return tour;
    }

    // Cheapest Insertion
    vector<int> cheapestInsertion() 
    {
        vector<int> tour = {0, 1, 0};
        vector<bool> inTour(cities.size(), false);
        inTour[0] = inTour[1] = true;

        vector<vector<double>> dist(cities.size(), vector<double>(cities.size(), 0));
        for (int i = 0; i < cities.size(); ++i) 
        {
            for (int j = i + 1; j < cities.size(); ++j) 
            {
                dist[i][j] = dist[j][i] = distance(cities[i], cities[j]);
            }
        }

        while (tour.size() < cities.size() + 1) 
        {
            int bestCity = -1;
            int bestPosition = -1;
            double minIncrease = numeric_limits<double>::max();

            for (int city = 0; city < cities.size(); city++) 
            {
                if (!inTour[city]) 
                {
                    for (int pos = 0; pos < tour.size() - 1; pos++) 
                    { 
                        int prevCity = tour[pos];
                        int nextCity = tour[pos + 1];
                        
                        double increase = dist[prevCity][city] + dist[city][nextCity] - dist[prevCity][nextCity];

                        if (increase < minIncrease) 
                        {
                            minIncrease = increase;
                            bestCity = city;
                            bestPosition = pos + 1;
                        }
                    }
                }
            }

            tour.insert(tour.begin() + bestPosition, bestCity);
            inTour[bestCity] = true;
        }

        return tour;
    }

    // Greedy Heuristic
    vector<int> greedyHeuristic() 
    {
        vector<pair<double, pair<int, int>>> edges;
        for (int i = 0; i < cities.size(); i++) 
        {
            for (int j = i + 1; j < cities.size(); j++) 
            {
                edges.push_back({distance(cities[i], cities[j]), {i, j}});
            }
        }
        sort(edges.begin(), edges.end());

        vector<int> degree(cities.size(), 0);    
        vector<int> parent(cities.size());        
        iota(parent.begin(), parent.end(), 0);    
        vector<pair<int, int>> tourEdges;         

        
        function<int(int)> find = [&](int x) {
            if (parent[x] != x) parent[x] = find(parent[x]);
            return parent[x];
        };

        auto unionSet = [&](int x, int y) 
        {
            int rootX = find(x);
            int rootY = find(y);
            if (rootX != rootY) parent[rootY] = rootX;
        };

        for (const auto& edge : edges) 
        {
            int u = edge.second.first;
            int v = edge.second.second;

            if (degree[u] < 2 && degree[v] < 2 && find(u) != find(v)) 
            {
                tourEdges.push_back({u, v});
                degree[u]++;
                degree[v]++;
                unionSet(u, v);

                if (tourEdges.size() == cities.size() - 1) break;
            }
        }

        for (const auto& edge : edges)
        {
            int u = edge.second.first;
            int v = edge.second.second;
            if (degree[u] < 2 && degree[v] < 2 && find(u) == find(v)) {
                tourEdges.push_back({u, v});
                break;
            }
        }

        vector<int> tour;
        unordered_map<int, vector<int>> adjList;
        for (const auto& e : tourEdges) 
        {
            adjList[e.first].push_back(e.second);
            adjList[e.second].push_back(e.first);
        }

        int currentCity = 0;
        unordered_set<int> visitedCities;
        while (tour.size() < cities.size()) 
        {
            tour.push_back(currentCity);
            visitedCities.insert(currentCity);
            for (int neighbor : adjList[currentCity]) 
            {
                if (visitedCities.find(neighbor) == visitedCities.end()) 
                {
                    currentCity = neighbor;
                    break;
                }
            }
        }

        return tour;
    }


    // 2-opt optimization
    vector<int> twoOpt(vector<int> tour)
    {
        bool improvement = true;
        while (improvement) {
            improvement = false;
            for (int i = 1; i < tour.size() - 1; i++) 
            {
                for (int j = i + 1; j < tour.size(); j++) 
                {
                    if (j == tour.size() - 1 && i == 0) continue;
                    double currentDist = distance(cities[tour[i - 1]], cities[tour[i]]) +
                                        distance(cities[tour[j]], cities[tour[(j + 1) % tour.size()]]);
                    double newDist = distance(cities[tour[i - 1]], cities[tour[j]]) +
                                    distance(cities[tour[i]], cities[tour[(j + 1) % tour.size()]]);
                    if (newDist < currentDist) 
                    {
                        reverse(tour.begin() + i, tour.begin() + j + 1);
                        improvement = true;
                    }
                }
            }
        }
        return tour;
    }

    // Node Shift
    vector<int> nodeShift(vector<int> tour)
    {
        bool improvement = true;
        double bestDistance = calculateTourDistance(tour);

        while (improvement) 
        {
            improvement = false;

            for (int i = 1; i < tour.size() - 1; i++) 
            {
                int city = tour[i];
                tour.erase(tour.begin() + i); 

                int bestPosition = i;  
                double currentBestDistance = bestDistance;

                for (int j = 1; j < tour.size(); j++) 
                {
                    tour.insert(tour.begin() + j, city);
                    double newDistance = calculateTourDistance(tour);

                    if (newDistance < currentBestDistance) 
                    {
                        currentBestDistance = newDistance;
                        bestPosition = j;
                        improvement = true;
                    }

                    tour.erase(tour.begin() + j);
                }

                tour.insert(tour.begin() + bestPosition, city);

                if (improvement) 
                {
                    bestDistance = currentBestDistance;
                    break;
                }
            }
        }

        return tour;
    }




    // Node Swap
    vector<int> nodeSwap(vector<int> tour) 
    {
        bool improvement = true;
        double bestDistance = calculateTourDistance(tour);

        while (improvement) 
        {
            improvement = false;

            for (int i = 1; i < tour.size() - 1; i++) 
            {
                for (int j = i + 1; j < tour.size() - 1; j++) 
                {
                    swap(tour[i], tour[j]);

                    double newDistance = calculateTourDistance(tour);

                    if (newDistance < bestDistance) 
                    {
                        bestDistance = newDistance;
                        improvement = true;
                    } 
                    else 
                    {
                        swap(tour[i], tour[j]);
                    }
                }
            }
        }

        return tour;
    }

};

vector<City> readTSPFile(const string& filename) 
{
    ifstream file(filename);
    vector<City> cities;

    if (!file.is_open()) 
    {
        cerr << "Error: Could not open file " << filename << endl;
        return cities;
    }

    string line;
    bool nodeSection = false;
    while (getline(file, line)) 
    {
        if (line.find("NODE_COORD_SECTION") != string::npos) {
            nodeSection = true;
            continue;
        }
        if (line.find("EOF") != string::npos) break;

        if (nodeSection) 
        {
            stringstream ss(line);
            int id;
            double x, y;
            ss >> id >> x >> y;
            cities.emplace_back(id, x, y);
        }
    }
    file.close();
    return cities;
}

void generateCSVReport(const string& filename, const string& constructionModel, const string& perturbativeModel, double initialCost, double optimizedCost, double delta) 
{
    static bool headerWritten = false;
    ofstream csvFile("tsp_reports.csv", ios::app);

    if (!headerWritten) 
    {
        csvFile << "Filename,Construction Model,Perturbative Model,Initial Cost,Optimized Cost,Delta\n";
        headerWritten = true;
    }

    csvFile << filename << "," << constructionModel << "," << perturbativeModel << ","
            << initialCost << "," << optimizedCost << "," << delta << "\n";

    csvFile.close();
}

int main() {

    vector<string> tspFiles = {
        "st70.tsp"
    };

    for (int i = 0; i < tspFiles.size(); i++) 
    {
        string filename = "./TSP_data/" + tspFiles[i];
        vector<City> cities = readTSPFile(filename);

        if (cities.empty()) {
            cout << "No cities were loaded from the file." << endl;
            return 1;
        }

        TSP tsp(cities);

        vector<int> nn = tsp.nearestNeighbor();
        vector<int> ci = tsp.cheapestInsertion();
        vector<int> gr = tsp.greedyHeuristic();

        vector<int> topt = tsp.twoOpt(nn);
        vector<int> ns = tsp.nodeShift(nn);
        vector<int> nsw = tsp.nodeSwap(nn);

        double nnInitialCost = tsp.calculateTourDistance(nn);
        double nnOptimizedCost = tsp.calculateTourDistance(topt);
        double nnDelta = nnOptimizedCost - nnInitialCost;
        generateCSVReport(tspFiles[i], "Nearest Neighbour Heuristic", "2-Opt", nnInitialCost, nnOptimizedCost, nnDelta);
        double nnSwapOptimizedCost = tsp.calculateTourDistance(nsw);
        double nnSwapDelta = nnSwapOptimizedCost - nnInitialCost;
        generateCSVReport(tspFiles[i], "Nearest Neighbour Heuristic", "Node Swap", nnInitialCost, nnSwapOptimizedCost, nnSwapDelta);
        double nnShiftOptimizedCost = tsp.calculateTourDistance(ns);
        double nnShiftDelta = nnShiftOptimizedCost - nnInitialCost;
        generateCSVReport(tspFiles[i], "Nearest Neighbour Heuristic", "Node Shift", nnInitialCost, nnShiftOptimizedCost, nnShiftDelta);

        vector<int> topt1 = tsp.twoOpt(ci);
        vector<int> ns1 = tsp.nodeShift(ci);
        vector<int> nsw1 = tsp.nodeSwap(ci);

        double ciInitialCost = tsp.calculateTourDistance(ci);
        double ciOptimizedCost = tsp.calculateTourDistance(topt1);
        double ciDelta = ciOptimizedCost - ciInitialCost;
        generateCSVReport(tspFiles[i], "Cheapest Neighbour Insertion", "2-Opt", ciInitialCost, ciOptimizedCost, ciDelta);
        double ciSwapOptimizedCost = tsp.calculateTourDistance(nsw1);
        double ciSwapDelta = ciSwapOptimizedCost - ciInitialCost;
        generateCSVReport(tspFiles[i], "Cheapest Neighbour Insertion", "Node Swap", ciInitialCost, ciSwapOptimizedCost, ciSwapDelta);
        double ciShiftOptimizedCost = tsp.calculateTourDistance(ns1);
        double ciShiftDelta = ciShiftOptimizedCost - ciInitialCost;
        generateCSVReport(tspFiles[i], "Cheapest Neighbour Insertion", "Node Shift", ciInitialCost, ciShiftOptimizedCost, ciShiftDelta);

        vector<int> topt2 = tsp.twoOpt(gr);
        vector<int> ns2 = tsp.nodeShift(gr);
        vector<int> nsw2 = tsp.nodeSwap(gr);

        double grInitialCost = tsp.calculateTourDistance(gr);
        double grOptimizedCost = tsp.calculateTourDistance(topt2);
        double grDelta = grOptimizedCost - grInitialCost;
        generateCSVReport(tspFiles[i], "Greedy", "2-Opt", grInitialCost, grOptimizedCost, grDelta);
        double grSwapOptimizedCost = tsp.calculateTourDistance(nsw2);
        double grSwapDelta = grSwapOptimizedCost - grInitialCost;
        generateCSVReport(tspFiles[i], "Greedy", "Node Swap", grInitialCost, grSwapOptimizedCost, grSwapDelta);
        double grShiftOptimizedCost = tsp.calculateTourDistance(ns2);
        double grShiftDelta = grShiftOptimizedCost - grInitialCost;
        generateCSVReport(tspFiles[i], "Greedy", "Node Shift", grInitialCost, grShiftOptimizedCost, grShiftDelta);

    }

    return 0;
}