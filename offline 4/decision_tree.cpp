#include <bits/stdc++.h>
using namespace std;

class Instance
{
public:
    vector<string> attributes;
    string className;
};

class Node
{
public:
    bool isLeaf = false;
    string attributeName = "";
    string value = "";
    int attributeIndex = -1;
    map<string, Node *> children;

    ~Node()
    {
        for (auto pair : children)
        {
            delete pair.second;
        }
    }
};

class DecisionTree
{
private:
    Node *root;
    vector<string> attributeNames;
    vector<set<string>> attributeValues;
    static mt19937 gen;
    map<string, int> globalClassCount; // To store global class distribution

    double calculateEntropy(vector<Instance> &instances)
    {
        if (instances.empty())
            return 0.0;

        map<string, int> classCount;
        for (auto instance : instances)
        {
            classCount[instance.className]++;
        }

        double entropy = 0.0;
        double total = instances.size();
        for (auto pair : classCount)
        {
            double prob = pair.second / total;
            if (prob > 0)
            {
                entropy -= prob * log2(prob);
            }
        }
        return entropy;
    }

    double calculateGini(vector<Instance> &instances)
    {
        if (instances.empty())
            return 0.0;

        map<string, int> classCount;
        for (auto instance : instances)
        {
            classCount[instance.className]++;
        }

        double gini = 1.0;
        double total = instances.size();
        for (auto pair : classCount)
        {
            double prob = pair.second / total;
            gini -= prob * prob;
        }
        return gini;
    }

    static bool compareScores(const pair<double, int> &a, const pair<double, int> &b)
    {
        return a.first > b.first;
    }

    static bool compareClasses(const pair<string, int> &a, const pair<string, int> &b)
    {
        return a.second < b.second;
    }

    int selectBestAttribute(vector<Instance> &instances, vector<bool> &usedAttributes, bool useGini, bool randomFromTop3)
    {
        vector<pair<double, int>> scores;
        double baseImpurity;
        
        if(useGini)
        {
            baseImpurity = calculateGini(instances);
        }
        else
        {
            baseImpurity = calculateEntropy(instances);
        } 

        for (int i = 0; i < attributeNames.size(); i++)
        {
            if (usedAttributes[i])
                continue;

            map<string, vector<Instance>> splits;
            for (auto instance : instances)
            {
                splits[instance.attributes[i]].push_back(instance);
            }

            double weightedSum = 0.0;
            for (auto split : splits)
            {
                double weight = (double)split.second.size() / instances.size();
                double impurity;
                if(useGini)
                {
                    impurity = calculateGini(split.second);
                }
                else
                {
                    impurity = calculateEntropy(split.second);
                } 
                weightedSum += weight * impurity;
            }

            double gain = baseImpurity - weightedSum;
            scores.push_back({gain, i});
        }

        if (scores.empty()) return -1;

        sort(scores.begin(), scores.end(), compareScores);

        if (randomFromTop3)
        {
            int topN = min(3, (int)scores.size());
            uniform_int_distribution<> dist(0, topN - 1);
            return scores[dist(gen)].second;
        }

        return scores[0].second;
    }

    Node *buildTree(vector<Instance> &instances, vector<bool> &usedAttributes, bool useGini, bool randomFromTop3, map<string, int> &parentClassCount)
    {
        Node *node = new Node();

        if (instances.empty())
        {
            node->isLeaf = true;
            auto maxClass = max_element(parentClassCount.begin(), parentClassCount.end(), compareClasses);
            node->value = maxClass->first;
            return node;
        }

        map<string, int> classCount;
        for (auto instance : instances)
        {
            classCount[instance.className]++;
        }

        if (classCount.size() == 1)
        {
            node->isLeaf = true;
            node->value = instances[0].className;
            return node;
        }

        int selectedAttr = selectBestAttribute(instances, usedAttributes, useGini, randomFromTop3);
        if (selectedAttr == -1)
        {
            node->isLeaf = true;
            auto maxClass = max_element(classCount.begin(), classCount.end(), compareClasses);
            node->value = maxClass->first;
            return node;
        }

        node->attributeIndex = selectedAttr;
        node->attributeName = attributeNames[selectedAttr];
        usedAttributes[selectedAttr] = true;

        map<string, vector<Instance>> splits;
        for (auto instance : instances)
        {
            splits[instance.attributes[selectedAttr]].push_back(instance);
        }

        for (auto value : attributeValues[selectedAttr])
        {
            if (splits[value].empty())
            {
                Node *leaf = new Node();
                leaf->isLeaf = true;
                auto maxClass = max_element(classCount.begin(), classCount.end(), compareClasses);
                leaf->value = maxClass->first;
                node->children[value] = leaf;
            }
            else
            {
                node->children[value] = buildTree(splits[value], usedAttributes, useGini, randomFromTop3, classCount);
            }
        }

        usedAttributes[selectedAttr] = false;
        return node;
    }

    string classify(Instance &instance, Node *node)
    {
        if (node->isLeaf)
        {
            return node->value;
        }

        string value = instance.attributes[node->attributeIndex];
        auto it = node->children.find(value);
        if (it == node->children.end())
        {
            return globalClassCount.empty() ? "unacc" : max_element(globalClassCount.begin(), globalClassCount.end(), compareClasses)->first;
        }

        return classify(instance, it->second);
    }

public:
    DecisionTree(vector<string> &attrNames, vector<set<string>> &attrValues)
    {
        attributeNames = attrNames;
        attributeValues = attrValues;
    }

    ~DecisionTree()
    {
        delete root;
    }

    void train(vector<Instance> &instances, bool useGini, bool randomFromTop3)
    {
        globalClassCount.clear();
        for (auto &instance : instances)
        {
            globalClassCount[instance.className]++;
        }

        vector<bool> usedAttributes(attributeNames.size(), false);
        root = buildTree(instances, usedAttributes, useGini, randomFromTop3, globalClassCount);
    }

    string predict(Instance &instance)
    {
        return classify(instance, root);
    }
};

mt19937 DecisionTree::gen(random_device{}());

vector<Instance> loadData(string filename)
{
    vector<Instance> dataset;
    ifstream file(filename);

    if (!file.is_open())
    {
        cout << "Error opening file: " << filename << endl;
        return dataset;
    }

    string line;
    while (getline(file, line))
    {
        istringstream iss(line);
        string value;
        Instance instance;

        for (int i = 0; i < 6; i++)
        {
            if (getline(iss, value, ','))
            {
                instance.attributes.push_back(value);
            }
        }

        if (getline(iss, value))
        {
            instance.className = value;
        }

        if (instance.attributes.size() == 6)
        {
            dataset.push_back(instance);
        }
    }

    return dataset;
}

pair<vector<Instance>, vector<Instance>> splitDataset(vector<Instance> &dataset, double trainRatio)
{
    vector<Instance> shuffledDataset = dataset;
    static mt19937 gen(random_device{}());

    shuffle(shuffledDataset.begin(), shuffledDataset.end(), gen);

    int trainSize = (int)(dataset.size() * trainRatio);
    vector<Instance> trainSet(shuffledDataset.begin(), shuffledDataset.begin() + trainSize);
    vector<Instance> testSet(shuffledDataset.begin() + trainSize, shuffledDataset.end());

    return {trainSet, testSet};
}

double calculateAverage(vector<double> &v)
{
    double sum = 0.0;
    for (int i = 0; i < v.size(); i++)
    {
        sum += v[i];
    }
    return sum / v.size();
}

int main()
{
    vector<string> attributeNames = {
        "buying", "maint", "doors", "persons", "lug_boot", "safety"};

    vector<set<string>> attributeValues = {
        {"vhigh", "high", "med", "low"},
        {"vhigh", "high", "med", "low"},
        {"2", "3", "4", "5more"},
        {"2", "4", "more"},
        {"small", "med", "big"},
        {"low", "med", "high"}};

    vector<Instance> dataset = loadData("car.data");
    if (dataset.empty())
    {
        cout << "Failed to load dataset" << endl;
        return 1;
    }

    int numRuns = 20;
    vector<double> accuracies_ig_best(numRuns);
    vector<double> accuracies_gini_best(numRuns);
    vector<double> accuracies_ig_random(numRuns);
    vector<double> accuracies_gini_random(numRuns);

    for (int run = 0; run < numRuns; run++)
    {
        cout << "Running " << (run + 1) << "/" << numRuns << endl;

        auto split = splitDataset(dataset, 0.8);
        vector<Instance> trainSet = split.first;
        vector<Instance> testSet = split.second;

        DecisionTree treeIG_Best(attributeNames, attributeValues);
        treeIG_Best.train(trainSet, false, false);
        int correct = 0;
        for (auto instance : testSet)
        {
            if (treeIG_Best.predict(instance) == instance.className)
                correct++;
        }
        accuracies_ig_best[run] = 100.0 * correct / testSet.size();

        DecisionTree treeGini_Best(attributeNames, attributeValues);
        treeGini_Best.train(trainSet, true, false);
        correct = 0;
        for (auto instance : testSet)
        {
            if (treeGini_Best.predict(instance) == instance.className)
                correct++;
        }
        accuracies_gini_best[run] = 100.0 * correct / testSet.size();

        DecisionTree treeIG_Random(attributeNames, attributeValues);
        treeIG_Random.train(trainSet, false, true);
        correct = 0;
        for (auto instance : testSet)
        {
            if (treeIG_Random.predict(instance) == instance.className)
                correct++;
        }
        accuracies_ig_random[run] = 100.0 * correct / testSet.size();

        DecisionTree treeGini_Random(attributeNames, attributeValues);
        treeGini_Random.train(trainSet, true, true);
        correct = 0;
        for (auto instance : testSet)
        {
            if (treeGini_Random.predict(instance) == instance.className)
                correct++;
        }
        accuracies_gini_random[run] = 100.0 * correct / testSet.size();
    }

    cout << "\nResults over " << numRuns << " runs:\n\n";
    cout << fixed << setprecision(2);
    cout << "Strategy                                 | Information gain | Gini impurity\n";
    cout << "----------------------------------------------------------------------------\n";
    cout << "Always select best                       | "
        << calculateAverage(accuracies_ig_best) << "%           | "
        << calculateAverage(accuracies_gini_best) << "%\n";
    cout << "Select one randomly from top 3           | "
        << calculateAverage(accuracies_ig_random) << "%           | "
        << calculateAverage(accuracies_gini_random) << "%\n";

    return 0;
}