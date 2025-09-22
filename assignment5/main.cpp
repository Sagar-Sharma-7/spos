#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <queue>

using namespace std;

// This structure holds all the information about a single process
struct Process {
    int pid; // Process ID
    int arrivalTime;
    int burstTime;
    int priority;

    // These will be calculated by the algorithms
    int startTime;
    int completionTime;
    int turnaroundTime;
    int waitingTime;
    int remainingTime; // For preemptive algorithms
};

// Function to print the final results table
void printResults(const vector<Process>& processes, const string& algorithmName) {
    int n = processes.size();
    if (n == 0) return;

    float totalWaitingTime = 0;
    float totalTurnaroundTime = 0;

    cout << "\n--- Results for " << algorithmName << " ---\n";
    cout << "--------------------------------------------------------------------------------\n";
    cout << "| PID | Arrival | Burst | Priority | Completion | Turnaround | Waiting |\n";
    cout << "|-----|---------|-------|----------|------------|------------|---------|\n";

    for (const auto& p : processes) {
        cout << "| " << setw(3) << p.pid
             << " | " << setw(7) << p.arrivalTime
             << " | " << setw(5) << p.burstTime
             << " | " << setw(8) << p.priority
             << " | " << setw(10) << p.completionTime
             << " | " << setw(10) << p.turnaroundTime
             << " | " << setw(7) << p.waitingTime << " |\n";
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
    }
    cout << "--------------------------------------------------------------------------------\n";
    cout << fixed << setprecision(2);
    cout << "Average Waiting Time: " << totalWaitingTime / n << endl;
    cout << "Average Turnaround Time: " << totalTurnaroundTime / n << endl;
    cout << endl;
}

// 1. First-Come, First-Served (FCFS)
void fcfs(vector<Process> processes) {
    int n = processes.size();
    // Sort processes by arrival time
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentTime = 0;
    cout << "\nGantt Chart (FCFS):\n|";
    for (int i = 0; i < n; ++i) {
        if (currentTime < processes[i].arrivalTime) {
            currentTime = processes[i].arrivalTime;
        }
        processes[i].startTime = currentTime;
        processes[i].completionTime = currentTime + processes[i].burstTime;
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        
        cout << " P" << processes[i].pid << " (" << processes[i].completionTime << ") |";
        
        currentTime = processes[i].completionTime;
    }
    cout << endl;
    printResults(processes, "First-Come, First-Served");
}

// 2. Shortest Job First (SJF) - Preemptive (also called SRTF)
void sjfPreemptive(vector<Process> processes) {
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;
    
    cout << "\nGantt Chart (Preemptive SJF):\n|";
    while (completed != n) {
        int shortestJobIndex = -1;
        int shortestBurst = 1e9; // A very large number

        // Find the process with the shortest remaining time that has arrived
        for (int i = 0; i < n; ++i) {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0) {
                if (processes[i].remainingTime < shortestBurst) {
                    shortestBurst = processes[i].remainingTime;
                    shortestJobIndex = i;
                }
            }
        }

        if (shortestJobIndex == -1) {
            currentTime++; // No process is ready, CPU is idle
        } else {
            // Execute the shortest job for one time unit
            processes[shortestJobIndex].remainingTime--;
            cout << " P" << processes[shortestJobIndex].pid << " |";
            currentTime++;

            if (processes[shortestJobIndex].remainingTime == 0) {
                processes[shortestJobIndex].completionTime = currentTime;
                processes[shortestJobIndex].turnaroundTime = processes[shortestJobIndex].completionTime - processes[shortestJobIndex].arrivalTime;
                processes[shortestJobIndex].waitingTime = processes[shortestJobIndex].turnaroundTime - processes[shortestJobIndex].burstTime;
                completed++;
            }
        }
    }
    cout << " (end at " << currentTime << ")" << endl;
    printResults(processes, "Preemptive Shortest Job First (SRTF)");
}

// 3. Priority Scheduling (Non-Preemptive)
void priorityNonPreemptive(vector<Process> processes) {
    int n = processes.size();
    int completed = 0;
    int currentTime = 0;
    vector<bool> isCompleted(n, false);
    
    cout << "\nGantt Chart (Non-Preemptive Priority):\n|";
    while(completed != n) {
        int highestPriorityIndex = -1;
        int highestPriority = 1e9; // Lower number means higher priority

        // Find the available process with the highest priority
        for(int i = 0; i < n; i++) {
            if(processes[i].arrivalTime <= currentTime && !isCompleted[i]) {
                if(processes[i].priority < highestPriority) {
                    highestPriority = processes[i].priority;
                    highestPriorityIndex = i;
                }
            }
        }

        if (highestPriorityIndex == -1) {
            currentTime++;
        } else {
            processes[highestPriorityIndex].startTime = currentTime;
            processes[highestPriorityIndex].completionTime = currentTime + processes[highestPriorityIndex].burstTime;
            processes[highestPriorityIndex].turnaroundTime = processes[highestPriorityIndex].completionTime - processes[highestPriorityIndex].arrivalTime;
            processes[highestPriorityIndex].waitingTime = processes[highestPriorityIndex].turnaroundTime - processes[highestPriorityIndex].burstTime;
            
            currentTime = processes[highestPriorityIndex].completionTime;
            
            cout << " P" << processes[highestPriorityIndex].pid << " (" << currentTime << ") |";
            
            isCompleted[highestPriorityIndex] = true;
            completed++;
        }
    }
    cout << endl;
    printResults(processes, "Non-Preemptive Priority");
}

// 4. Round Robin (RR)
void roundRobin(vector<Process> processes, int timeQuantum) {
    int n = processes.size();
    queue<int> readyQueue;
    int currentTime = 0;
    int completed = 0;
    
    // Sort by arrival time to handle process arrivals correctly
    sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        return a.arrivalTime < b.arrivalTime;
    });

    int currentProcessIndex = 0;
    cout << "\nGantt Chart (Round Robin with TQ=" << timeQuantum << "):\n|";

    readyQueue.push(currentProcessIndex++); // Push the first process

    while(completed < n) {
        if (readyQueue.empty()) {
            currentTime++;
            // Check if new processes have arrived during idle time
            while (currentProcessIndex < n && processes[currentProcessIndex].arrivalTime <= currentTime) {
                readyQueue.push(currentProcessIndex++);
            }
            continue;
        }

        int processIdx = readyQueue.front();
        readyQueue.pop();

        int executionTime = min(timeQuantum, processes[processIdx].remainingTime);
        
        for (int i = 0; i < executionTime; ++i) {
             cout << " P" << processes[processIdx].pid << " |";
        }
        
        processes[processIdx].remainingTime -= executionTime;
        currentTime += executionTime;

        // Check for new arrivals during the execution of the current process
        while (currentProcessIndex < n && processes[currentProcessIndex].arrivalTime <= currentTime) {
            readyQueue.push(currentProcessIndex++);
        }

        if (processes[processIdx].remainingTime > 0) {
            readyQueue.push(processIdx); // Put it back in the queue
        } else {
            processes[processIdx].completionTime = currentTime;
            processes[processIdx].turnaroundTime = processes[processIdx].completionTime - processes[processIdx].arrivalTime;
            processes[processIdx].waitingTime = processes[processIdx].turnaroundTime - processes[processIdx].burstTime;
            completed++;
        }
    }
    cout << " (end at " << currentTime << ")" << endl;
    printResults(processes, "Round Robin");
}


int main() {
    int n;
    cout << "Enter the number of processes: ";
    cin >> n;

    vector<Process> processes(n);
    cout << "Enter process details (Arrival Time, Burst Time, Priority):\n";
    for (int i = 0; i < n; ++i) {
        processes[i].pid = i + 1;
        cout << "Process " << i + 1 << ": ";
        cin >> processes[i].arrivalTime >> processes[i].burstTime >> processes[i].priority;
        processes[i].remainingTime = processes[i].burstTime;
    }

    int choice;
    do {
        cout << "\nCPU Scheduling Algorithms Menu:\n";
        cout << "1. First-Come, First-Served (FCFS)\n";
        cout << "2. Preemptive Shortest Job First (SJF/SRTF)\n";
        cout << "3. Non-Preemptive Priority\n";
        cout << "4. Round Robin (RR)\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                fcfs(processes);
                break;
            case 2:
                sjfPreemptive(processes);
                break;
            case 3:
                priorityNonPreemptive(processes);
                break;
            case 4: {
                int timeQuantum;
                cout << "Enter Time Quantum for Round Robin: ";
                cin >> timeQuantum;
                roundRobin(processes, timeQuantum);
                break;
            }
            case 5:
                cout << "Exiting...\n";
                break;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    } while (choice != 5);

    return 0;
}
