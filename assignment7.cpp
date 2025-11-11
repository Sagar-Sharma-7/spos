#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
using namespace std;

void fifoPageReplacement(const vector<int>& pages, int frames) {
    queue<int> q;
    unordered_map<int, bool> inFrame;
    int pageFaults = 0, hits = 0;

    cout << "\n--- FIFO Page Replacement ---\n";
    for (int p : pages) {
        bool hit = inFrame[p];
        if (!hit) {
            pageFaults++;
            if (q.size() == frames) {
                int removed = q.front(); q.pop();
                inFrame[removed] = false;
            }
            q.push(p);
            inFrame[p] = true;
        } else hits++;

        cout << "Page " << setw(2) << p << " -> [ ";
        queue<int> temp = q;
        while (!temp.empty()) { cout << temp.front() << " "; temp.pop(); }
        cout << "] " << (hit ? "Hit" : "Page Fault") << endl;
    }
    double hitRate = (double)hits / pages.size() * 100.0;
    double faultRate = (double)pageFaults / pages.size() * 100.0;
    cout << "\nTotal Page Faults = " << pageFaults
         << "\nTotal Hits = " << hits
         << "\nHit Rate = " << fixed << setprecision(2) << hitRate << "%"
         << "\nFault Rate = " << fixed << setprecision(2) << faultRate << "%\n";
}

void lruPageReplacement(const vector<int>& pages, int frames) {
    vector<int> memory;                 // left = LRU, right = MRU
    unordered_map<int, int> lastUsed;   // last index used
    int pageFaults = 0, hits = 0;

    cout << "\n--- LRU Page Replacement ---\n";
    for (int i = 0; i < pages.size(); ++i) {
        int p = pages[i];
        auto it = find(memory.begin(), memory.end(), p);
        bool hit = (it != memory.end());

        if (hit) {
            // Move the page to MRU (end of vector)
            memory.erase(it);
            memory.push_back(p);
            hits++;
        } else {
            pageFaults++;
            if (memory.size() == frames) {
                // find LRU by smallest lastUsed (robust if pages repeated)
                int lruPage = memory[0];
                int minIdx = lastUsed.count(lruPage) ? lastUsed[lruPage] : -1;
                for (int x : memory) {
                    int idx = lastUsed.count(x) ? lastUsed[x] : -1;
                    if (idx < minIdx) { minIdx = idx; lruPage = x; }
                }
                memory.erase(remove(memory.begin(), memory.end(), lruPage), memory.end());
            }
            memory.push_back(p);
        }

        lastUsed[p] = i;
        cout << "Page " << setw(2) << p << " -> [ ";
        for (int x : memory) cout << x << " ";
        cout << "] " << (hit ? "Hit" : "Page Fault") << endl;
    }
    double hitRate = (double)hits / pages.size() * 100.0;
    double faultRate = (double)pageFaults / pages.size() * 100.0;
    cout << "\nTotal Page Faults = " << pageFaults
         << "\nTotal Hits = " << hits
         << "\nHit Rate = " << fixed << setprecision(2) << hitRate << "%"
         << "\nFault Rate = " << fixed << setprecision(2) << faultRate << "%\n";
}

void optimalPageReplacement(const vector<int>& pages, int frames) {
    vector<int> memory;
    int pageFaults = 0, hits = 0;

    cout << "\n--- Optimal Page Replacement ---\n";
    for (int i = 0; i < pages.size(); ++i) {
        int p = pages[i];
        bool hit = find(memory.begin(), memory.end(), p) != memory.end();

        if (!hit) {
            pageFaults++;
            if (memory.size() == frames) {
                int farthest = i, victim = -1;
                for (int j = 0; j < memory.size(); ++j) {
                    int k;
                    for (k = i + 1; k < pages.size(); ++k)
                        if (pages[k] == memory[j]) break;
                    if (k > farthest) { farthest = k; victim = j; }
                }
                if (victim == -1) victim = 0;
                memory[victim] = p;
            } else {
                memory.push_back(p);
            }
        } else hits++;

        cout << "Page " << setw(2) << p << " -> [ ";
        for (int x : memory) cout << x << " ";
        cout << "] " << (hit ? "Hit" : "Page Fault") << endl;
    }
    double hitRate = (double)hits / pages.size() * 100.0;
    double faultRate = (double)pageFaults / pages.size() * 100.0;
    cout << "\nTotal Page Faults = " << pageFaults
         << "\nTotal Hits = " << hits
         << "\nHit Rate = " << fixed << setprecision(2) << hitRate << "%"
         << "\nFault Rate = " << fixed << setprecision(2) << faultRate << "%\n";
}

int main() {
    int n, frames, choice;
    cout << "Enter number of pages in reference string: ";
    cin >> n;
    vector<int> pages(n);
    cout << "Enter the page reference string:\n";
    for (int i = 0; i < n; i++) cin >> pages[i];
    cout << "Enter number of frames: ";
    cin >> frames;

    do {
        cout << "\n--- MENU ---\n1. FIFO\n2. LRU\n3. Optimal\n4. Exit\nEnter your choice: ";
        cin >> choice;
        switch (choice) {
            case 1: fifoPageReplacement(pages, frames); break;
            case 2: lruPageReplacement(pages, frames); break;
            case 3: optimalPageReplacement(pages, frames); break;
            case 4: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 4);
    return 0;
}
