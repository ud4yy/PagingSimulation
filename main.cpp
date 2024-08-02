#include <bits/stdc++.h>
using namespace std;

/*
This is a project simulating Paging Techniques, LRU, FIFO, Optimal, LFU.
*/

// LRU Cache Implementation
class LRUCache {
    class node {
    public:
        node *prev, *next;
        int data, key;

        node(int key, int val) {
            this->data = val;
            this->key = key;
            prev = nullptr;
            next = nullptr;
        }
    };

    node *head, *tail;
    int cap;
    unordered_map<int, node*> mpp;

public:
    LRUCache(int capacity) : cap(capacity) {
        head = new node(-1, -1);
        tail = new node(-1, -1);
        head->next = tail;
        tail->prev = head;
    }

    void Remove(node *n) {
        node *p = n->prev;
        node *z = n->next;
        p->next = z;
        z->prev = p;
    }

    void Add(node *n) {
        n->next = head->next;
        n->prev = head;
        head->next->prev = n;
        head->next = n;
    }

    int get(int key) {
        if (mpp.find(key) != mpp.end()) {
            node *n = mpp[key];
            int data = n->data;
            Remove(n);
            Add(n);
            mpp[key] = head->next;
            return data;
        }
        return -1;
    }

    void put(int key, int value) {
        if (mpp.find(key) != mpp.end()) {
            node *n = mpp[key];
            Remove(n);
            mpp.erase(key);
        }
        if (mpp.size() == cap) {
            node *p = tail->prev;
            mpp.erase(p->key);
            Remove(p);
        }
        node *X = new node(key, value);
        Add(X);
        mpp[key] = head->next;
    }

    void displayFrames() {
        node *curr = head->next;
        while (curr != tail) {
            cout << curr->data << " ";
            curr = curr->next;
        }
        cout << endl;
    }
};

// FIFO Cache Implementation
class FIFOCache {
    int capacity;
    queue<int> q;
    unordered_set<int> pages;

public:
    FIFOCache(int cap) : capacity(cap) {}

    bool refer(int page) {
        if (pages.find(page) == pages.end()) {
            if (q.size() == capacity) {
                int oldest=q.front();
                q.pop();
                pages.erase(oldest);
            }
            q.push(page);
            pages.insert(page);
            return false; // Page fault
        }
        return true; // Page hit
    }

    void displayFrames() {
        queue<int> temp = q;
        while (!temp.empty()) {
            cout << temp.front() << " ";
            temp.pop();
        }
        cout << endl;
    }
};

// Optimal Cache Implementation
class OptimalCache {
    int capacity;
    unordered_set<int> pages;

public:
    OptimalCache(int cap) : capacity(cap) {}

    bool refer(int page, vector<int>& ref, int index) {

        if (pages.find(page) == pages.end()) { //if it is not present
            
            if (pages.size() == capacity) {
                int farthest = index, victim = -1;
                for(int p : pages) {
                    auto it = find(ref.begin() + index + 1, ref.end(), p);
                    if (it == ref.end()) {
                        victim = p;
                        break;

                    } else if (it - ref.begin() > farthest) {
                        farthest = it - ref.begin();
                        victim = p;
                    }
                }
                pages.erase(victim);
            }
            pages.insert(page);
            return false; // Page fault
        }
        return true; // Page hit
    }

    void displayFrames() {
        for (int p : pages) {
            cout << p << " ";
        }
        cout << endl;
    }
};

// LFU Cache Implementation
class LFUCache {
private:
    struct Node {
        int key, value, freq;
        Node(int k, int v, int f) : key(k), value(v), freq(f) {}
    };

    int capacity;
    int minFreq;
    unordered_map<int, list<Node>::iterator> keyMap;
    unordered_map<int, list<Node>> freqMap;

    void updateFreq(list<Node>::iterator node) {
        int key = node->key;
        int value = node->value;
        int freq = node->freq;

        // remove node from freqMap
        freqMap[freq].erase(node);
        if (freqMap[freq].empty()) {
            freqMap.erase(freq);
            if (minFreq == freq) minFreq++;
        }
        freq++;
        freqMap[freq].push_front(Node(key, value, freq));
        keyMap[key] = freqMap[freq].begin();
    }

public:
    LFUCache(int capacity) : capacity(capacity), minFreq(0) {}

    int get(int key) {
        if (keyMap.find(key) == keyMap.end()) {
            return -1;
        }

        auto node = keyMap[key];
        int value = node->value;
        updateFreq(node);
        return value;
    }

    void put(int key, int value) {
        if (capacity == 0) return;

        if (keyMap.find(key) != keyMap.end()) {
            auto node = keyMap[key];
            node->value = value;
            updateFreq(node);
        } else {
            if (keyMap.size() == capacity) {
                auto lastNode = freqMap[minFreq].back();
                keyMap.erase(lastNode.key);
                freqMap[minFreq].pop_back();
                if (freqMap[minFreq].empty()) {
                    freqMap.erase(minFreq);
                }
            }

            minFreq = 1;
            freqMap[minFreq].push_front(Node(key, value, minFreq));
            keyMap[key] = freqMap[minFreq].begin();
        }
    }

    void displayFrames() {
        for (auto& f : freqMap) {
            for (auto& node : f.second) {
                cout << node.value << " ";
            }
        }
        cout << endl;
    }
};

void getInput(vector<int>& ref, int& numFrames) {
    int n;
    cout << "Enter the number of frames: ";
    cin >> numFrames;
    cout << "Enter the number of pages in the reference string: ";
    cin >> n;
    ref.resize(n);
    cout << "Enter the reference string: ";
    for (int i = 0; i < n; i++) {
        cin >> ref[i];
    }
}

void displayResults(const string& algoName, int faults, int hits) {
    int total = faults + hits;
    cout << algoName << endl;
    cout << "Page Faults: " << faults << ", Page Hits: " << hits << endl;
    cout << "Miss Ratio: " << (faults * 100.0) / total << "%, Hit Ratio: " << (hits * 100.0) / total << "%" << endl << endl;
}

void runSimulations(vector<int>& ref, int numFrames) {
    LRUCache lruCache(numFrames);
    FIFOCache fifoCache(numFrames);
    OptimalCache optimalCache(numFrames);
    LFUCache lfuCache(numFrames);

    int lruFaults = 0, fifoFaults = 0, optimalFaults = 0, lfuFaults = 0;
    int lruHits = 0, fifoHits = 0, optimalHits = 0, lfuHits = 0;

    cout << "First-In First-Out" << endl;
    for (int i = 0; i < ref.size(); i++) {
        if (!fifoCache.refer(ref[i])) {
            fifoFaults++;
        } else {
            fifoHits++;
        }
        cout << ref[i] << " : ";
        fifoCache.displayFrames();
    }
    displayResults("FIFO", fifoFaults, fifoHits);
    
    cout << "Least Recently Used" << endl;
    for (int i = 0; i < ref.size(); i++) {
        if (lruCache.get(ref[i]) == -1) {
            lruFaults++;
            lruCache.put(ref[i], ref[i]);
        } else {
            lruHits++;
        }
        cout << ref[i] << " : ";
        lruCache.displayFrames();
    }
    displayResults("LRU", lruFaults, lruHits);

    cout << "Least Frequently Used" << endl;
    for (int i = 0; i < ref.size(); i++) {
        if (lfuCache.get(ref[i]) == -1) {
            lfuFaults++;
            lfuCache.put(ref[i], ref[i]);
        } else {
            lfuHits++;
        }
        cout << ref[i] << " : ";
        lfuCache.displayFrames();
    }
    displayResults("LFU", lfuFaults, lfuHits);

    cout << "Optimal" << endl;
    for (int i = 0; i < ref.size(); i++) {
        if (!optimalCache.refer(ref[i], ref, i)) {
            optimalFaults++;
        } else {
            optimalHits++;
        }
        cout << ref[i] << " : ";
        optimalCache.displayFrames();
    }
    displayResults("Optimal", optimalFaults, optimalHits);
}

int main() {
    vector<int> ref;
    int numFrames;
    getInput(ref, numFrames);
    runSimulations(ref, numFrames);
    return 0;
}
