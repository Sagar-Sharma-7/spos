#include <iostream>
#include <vector>
#include <iomanip>
using namespace std;

// Function to display memory blocks after allocation
void display(const vector<int>& blocks, const vector<int>& allocation) {
    cout << "\nProcess No.\tProcess Size\tBlock No.\n";
    for (int i = 0; i < allocation.size(); i++) {
        if (allocation[i] != -1)
            cout << "   " << i + 1 << "\t\t" << setw(8) << allocation[i] << "\t\t" << allocation[i] << endl;
    }
}

// FIRST FIT
void firstFit(vector<int> blocks, vector<int> process) {
    vector<int> allocation(process.size(), -1);

    for (int i = 0; i < process.size(); i++) {
        for (int j = 0; j < blocks.size(); j++) {
            if (blocks[j] >= process[i]) {
                allocation[i] = j + 1;
                blocks[j] -= process[i];
                break;
            }
        }
    }

    cout << "\n--- First Fit Allocation ---\n";
    cout << "Process\tProcess Size\tAllocated Block\n";
    for (int i = 0; i < process.size(); i++) {
        if (allocation[i] != -1)
            cout << i + 1 << "\t" << process[i] << "\t\t" << allocation[i] << endl;
        else
            cout << i + 1 << "\t" << process[i] << "\t\tNot Allocated\n";
    }
}

// NEXT FIT
void nextFit(vector<int> blocks, vector<int> process) {
    vector<int> allocation(process.size(), -1);
    int j = 0, n = blocks.size();

    for (int i = 0; i < process.size(); i++) {
        int count = 0;
        while (count < n) {
            if (blocks[j] >= process[i]) {
                allocation[i] = j + 1;
                blocks[j] -= process[i];
                break;
            }
            j = (j + 1) % n;
            count++;
        }
    }

    cout << "\n--- Next Fit Allocation ---\n";
    cout << "Process\tProcess Size\tAllocated Block\n";
    for (int i = 0; i < process.size(); i++) {
        if (allocation[i] != -1)
            cout << i + 1 << "\t" << process[i] << "\t\t" << allocation[i] << endl;
        else
            cout << i + 1 << "\t" << process[i] << "\t\tNot Allocated\n";
    }
}

// BEST FIT
void bestFit(vector<int> blocks, vector<int> process) {
    vector<int> allocation(process.size(), -1);

    for (int i = 0; i < process.size(); i++) {
        int bestIdx = -1;
        for (int j = 0; j < blocks.size(); j++) {
            if (blocks[j] >= process[i]) {
                if (bestIdx == -1 || blocks[j] < blocks[bestIdx])
                    bestIdx = j;
            }
        }
        if (bestIdx != -1) {
            allocation[i] = bestIdx + 1;
            blocks[bestIdx] -= process[i];
        }
    }

    cout << "\n--- Best Fit Allocation ---\n";
    cout << "Process\tProcess Size\tAllocated Block\n";
    for (int i = 0; i < process.size(); i++) {
        if (allocation[i] != -1)
            cout << i + 1 << "\t" << process[i] << "\t\t" << allocation[i] << endl;
        else
            cout << i + 1 << "\t" << process[i] << "\t\tNot Allocated\n";
    }
}

// WORST FIT
void worstFit(vector<int> blocks, vector<int> process) {
    vector<int> allocation(process.size(), -1);

    for (int i = 0; i < process.size(); i++) {
        int worstIdx = -1;
        for (int j = 0; j < blocks.size(); j++) {
            if (blocks[j] >= process[i]) {
                if (worstIdx == -1 || blocks[j] > blocks[worstIdx])
                    worstIdx = j;
            }
        }
        if (worstIdx != -1) {
            allocation[i] = worstIdx + 1;
            blocks[worstIdx] -= process[i];
        }
    }

    cout << "\n--- Worst Fit Allocation ---\n";
    cout << "Process\tProcess Size\tAllocated Block\n";
    for (int i = 0; i < process.size(); i++) {
        if (allocation[i] != -1)
            cout << i + 1 << "\t" << process[i] << "\t\t" << allocation[i] << endl;
        else
            cout << i + 1 << "\t" << process[i] << "\t\tNot Allocated\n";
    }
}

int main() {
    int nBlocks, nProcess, choice;
    cout << "Enter number of memory blocks: ";
    cin >> nBlocks;

    vector<int> blocks(nBlocks);
    cout << "Enter size of each block:\n";
    for (int i = 0; i < nBlocks; i++)
        cin >> blocks[i];

    cout << "Enter number of processes: ";
    cin >> nProcess;

    vector<int> process(nProcess);
    cout << "Enter size of each process:\n";
    for (int i = 0; i < nProcess; i++)
        cin >> process[i];

    do {
        cout << "\n--- Memory Allocation Strategies ---\n";
        cout << "1. First Fit\n2. Next Fit\n3. Best Fit\n4. Worst Fit\n5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: firstFit(blocks, process); break;
            case 2: nextFit(blocks, process); break;
            case 3: bestFit(blocks, process); break;
            case 4: worstFit(blocks, process); break;
            case 5: cout << "Exiting...\n"; break;
            default: cout << "Invalid choice. Try again.\n";
        }
    } while (choice != 5);

    return 0;
}
