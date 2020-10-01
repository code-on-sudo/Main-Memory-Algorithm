#include <bits/stdc++.h>
using namespace std;

// It is the main memory size
#define MEM_SIZE 2000

vector<string> memory(MEM_SIZE);

// string ' ' means hole Else "1 0" means process 1 and segment 0
void displayMemoryMap()
{
    cout << "\t  Memory map:\n";

    string last = memory[0];
    int ind = 0;

    for (int i = 1; i < MEM_SIZE; i++)
    {
        if (last != memory[i])
        {
            if (last == " ")
                cout << "\t\t" << ind << "-" << i - 1 << " : Hole\n";
            else
            {
                // Retrieving Process and Segment id
                string p, s;
                for (int j = 0; j < last.size(); j++)
                    if (last[j] == ' ')
                    {
                        p = last.substr(0, j);
                        s = last.substr(j + 1, last.size() - j);
                        break;
                    }

                cout << "\t\t" << ind << "-" << i - 1 << " : Process " << p << ", Segment " << s << endl;
            }
            ind = i;
            last = memory[i];
        }
    }

    if (ind != MEM_SIZE - 1)
    {
        if (last == " ")
            cout << "\t\t" << ind << "-" << MEM_SIZE - 1 << " : Hole\n";
        else
        {
            // Retrieving Process and Segment id
            string p, s;
            for (int j = 0; j < last.size(); j++)
                if (last[j] == ' ')
                {
                    p = last.substr(0, j);
                    s = last.substr(j + 1, last.size() - j);
                    break;
                }

            cout << "\t\t" << ind << "-" << MEM_SIZE - 1 << " : Process " << p << ", Segment " << s << endl;
        }
    }
}

// Retrieves Free holes
vector<pair<int, int>> getFreeHoleSpace()
{
    string last = memory[0];
    int ind = 0;

    vector<pair<int, int>> ans;

    for (int i = 1; i < MEM_SIZE; i++)
    {
        if (last != memory[i])
        {
            if (last == " ")
            {
                ans.push_back(make_pair(ind, i - 1));
            }
            ind = i;
            last = memory[i];
        }
    }

    if (ind != MEM_SIZE - 1)
    {
        if (last == " ")
            ans.push_back(make_pair(ind, MEM_SIZE - 1));
    }

    return ans;
}

class Segment
{
public:
    int segmentId;
    int start = -1;
    int end = -1;
    int space;

    Segment()
    {
        start = -1;
        end = -1;
    }

    int getSpace() { return space; }
    int getStart() { return start; }
    int getEnd() { return end; }
};

class Process
{
public:
    int processId;
    int arrivalTime;
    int processDuration;
    vector<int> segmentSpaces;
    vector<Segment> segments;

    Process(int processId, int arrivalTime, int processDuration, int totalPages, vector<int> segmentSpaces)
    {
        this->processId = processId;
        this->arrivalTime = arrivalTime;
        this->processDuration = processDuration;
        this->segmentSpaces = segmentSpaces;

        for (int i = 0; i < totalPages; i++)
        {
            Segment s;
            s.segmentId = i + 1;
            s.space = segmentSpaces[i];

            segments.push_back(s);
        }
    }

    int getProcessId() { return processId; }

    int getArrivalTime() { return arrivalTime; }

    int getProcessDuration() { return processDuration; }

    vector<Segment> getSegments() { return segments; }
};

// Avoids Infinite loop
bool checkValidProcesses(Process process)
{
    int sumSegmentSpaces = 0;
    vector<Segment> segments = process.getSegments();

    for (int i = 0; i < segments.size(); i++)
        sumSegmentSpaces += segments[i].getSpace();

    /* Impossible to load the process in Memory
    So not including it in queue */
    if (sumSegmentSpaces > MEM_SIZE)
    {
        cout << "* Impossible to load the process " << process.getProcessId() << " in Memory as it takes more space then the memory, So not including it in the queue*\n";
        return false;
    }
    return true;
}

bool holesAscendingComparator(pair<int, int> hole1, pair<int, int> hole2)
{
    if (hole1.second - hole1.first < hole2.second - hole2.first)
        return true;
    if (hole1.second - hole1.first == hole2.second - hole2.first && hole1.first < hole2.first)
        return true;

    return false;
}

bool holesDescendingComparator(pair<int, int> hole1, pair<int, int> hole2)
{
    if (hole1.second - hole1.first > hole2.second - hole2.first)
        return true;
    if (hole1.second - hole1.first == hole2.second - hole2.first && hole1.first < hole2.first)
        return true;

    return false;
}

/* If not possible then return -1
What it does send the index of first fittable process and the while
in firstFitProcessMovePossible will take care for all
passing ans as reference to store the index */
int firstFitProcessMovePossible(vector<Process> processes, int say, int &ans)
{
    for (int t = 0; t < processes.size(); t++)
    {
        vector<pair<int, int>> holes = getFreeHoleSpace();
        Process process = processes[t];

        // This process is Not yet Arrived
        if (process.getArrivalTime() > say)
            continue;

        vector<Segment> segments = process.getSegments();

        int i = 0;
        for (i = 0; i < segments.size(); i++)
        {
            bool flag = true;

            for (int j = 0; j < holes.size(); j++)
            {
                if (holes[j].second - holes[j].first >= segments[i].getSpace() - 1)
                {
                    holes[j].first = holes[j].first + segments[i].getSpace();
                    flag = false;
                    break;
                }
            }

            if (flag)
                break;
        }

        // All the segments of t process can be included
        if (i == segments.size())
            return ans = t;
    }
    return ans = -1;
}

Process firstFitMoveProcess(vector<Process> &processes, int index)
{
    vector<pair<int, int>> holes = getFreeHoleSpace();
    Process process = processes[index];

    processes.erase(next(processes.begin(), index));

    vector<Segment> segments = process.getSegments();

    for (int i = 0; i < segments.size(); i++)
    {
        for (int j = 0; j < holes.size(); j++)
        {
            if (holes[j].second - holes[j].first >= segments[i].getSpace() - 1)
            {
                for (int k = holes[j].first; k < holes[j].first + segments[i].getSpace(); k++)
                    memory[k] = to_string(process.getProcessId()) + " " + to_string(i);

                process.segments[i].start = holes[j].first;
                process.segments[i].end = holes[j].first + segments[i].getSpace() - 1;

                holes[j].first = holes[j].first + segments[i].getSpace();
                break;
            }
        }
    }
    cout << "\tMM moves process " << process.getProcessId() << " to memory\n";

    return process;
}

// Greedily checking if this program can be loaded in MM
void firstFit(map<int, vector<Process>> &events, int last, int arrivalTime, int &turnAroundTime, vector<Process> &processes, map<int, int> &mapAtTime)
{
    // Here i is the time
    for (int i = last; i < arrivalTime; i++)
    {
        // We have a vectors of processes completed at time i
        if (events.find(i) != events.end())
        {
            cout << "t = " << i << ": \n";
            mapAtTime[i] = 10;

            vector<Process> processesAtTime = events[i];

            for (int t = processesAtTime.size() - 1; t >= 0; t--)
            {
                turnAroundTime += (i - processesAtTime[t].getArrivalTime());
                vector<Segment> segments = processesAtTime[t].getSegments();

                for (int j = 0; j < segments.size(); j++)
                    for (int k = segments[j].getStart(); k <= segments[j].getEnd(); k++)
                        memory[k] = " ";

                cout << "\tProcess " << processesAtTime[t].getProcessId() << " completes\n";
                displayMemoryMap();
            }

            // Remove this from map
            events.erase(i);
        }

        // Here ans will keep the value of firstFitProcessMovePossible(processes,i,ans) basically index
        int ans = -1;

        while (processes.size() > 0 && firstFitProcessMovePossible(processes, i, ans) != -1)
        {
            if (mapAtTime[i] == 0)
            {
                cout << "t = " << i << ":\n";
                mapAtTime[i] = 10;
            }

            // Rather than printing top, we'll just print ans'th index
            Process toppi = firstFitMoveProcess(processes, ans);
            events[i + toppi.getProcessDuration()].push_back(toppi);

            displayMemoryMap();
        }
    }
}

int bestFitProcessMovePossible(vector<Process> processes, int say, int &ans)
{
    for (int t = 0; t < processes.size(); t++)
    {
        vector<pair<int, int>> holes = getFreeHoleSpace();
        Process process = processes[t];

        // This process is Not yet Arrived
        if (process.getArrivalTime() > say)
            continue;

        vector<Segment> segments = process.getSegments();

        sort(holes.begin(), holes.end(), holesAscendingComparator);

        int i = 0;
        for (i = 0; i < segments.size(); i++)
        {
            bool flag = true;

            for (int j = 0; j < holes.size(); j++)
            {
                if (holes[j].second - holes[j].first >= segments[i].getSpace() - 1)
                {
                    holes[j].first = holes[j].first + segments[i].getSpace();
                    flag = false;
                    break;
                }
            }

            sort(holes.begin(), holes.end(), holesAscendingComparator);
            if (flag)
                break;
        }

        // All the segments of t process can be included
        if (i == segments.size())
            return ans = t;
    }
    return ans = -1;
}

Process bestFitMoveProcess(vector<Process> &processes, int index)
{
    vector<pair<int, int>> holes = getFreeHoleSpace();
    Process process = processes[index];

    processes.erase(next(processes.begin(), index));

    vector<Segment> segments = process.getSegments();

    sort(holes.begin(), holes.end(), holesAscendingComparator);

    for (int i = 0; i < segments.size(); i++)
    {
        for (int j = 0; j < holes.size(); j++)
        {
            if (holes[j].second - holes[j].first >= segments[i].getSpace() - 1)
            {
                for (int k = holes[j].first; k < holes[j].first + segments[i].getSpace(); k++)
                    memory[k] = to_string(process.getProcessId()) + " " + to_string(i);

                process.segments[i].start = holes[j].first;
                process.segments[i].end = holes[j].first + segments[i].getSpace() - 1;

                holes[j].first = holes[j].first + segments[i].getSpace();
                break;
            }
        }
        sort(holes.begin(), holes.end(), holesAscendingComparator);
    }
    cout << "\tMM moves process " << process.getProcessId() << " to memory\n";

    return process;
}

void bestFit(map<int, vector<Process>> &events, int last, int arrivalTime, int &turnAroundTime, vector<Process> &processes, map<int, int> &mapAtTime)
{
    // Here i is the time
    for (int i = last; i < arrivalTime; i++)
    {
        // We have a vectors of processes completed at time i
        if (events.find(i) != events.end())
        {
            cout << "t = " << i << ": \n";
            mapAtTime[i] = 10;

            vector<Process> processesAtTime = events[i];

            for (int t = processesAtTime.size() - 1; t >= 0; t--)
            {
                turnAroundTime += (i - processesAtTime[t].getArrivalTime());
                vector<Segment> segments = processesAtTime[t].getSegments();

                for (int j = 0; j < segments.size(); j++)
                    for (int k = segments[j].getStart(); k <= segments[j].getEnd(); k++)
                        memory[k] = " ";

                cout << "\tProcess " << processesAtTime[t].getProcessId() << " completes\n";
                displayMemoryMap();
            }

            // Remove this from map
            events.erase(i);
        }

        // Here ans will keep the value of firstFitProcessMovePossible(processes,i,ans) basically index
        int ans = -1;

        while (processes.size() > 0 && bestFitProcessMovePossible(processes, i, ans) != -1)
        {
            if (mapAtTime[i] == 0)
            {
                cout << "t = " << i << ":\n";
                mapAtTime[i] = 10;
            }

            // Rather than printing top, we'll just print ans'th index
            Process toppi = bestFitMoveProcess(processes, ans);
            events[i + toppi.getProcessDuration()].push_back(toppi);

            displayMemoryMap();
        }
    }
}

int worstFitProcessMovePossible(vector<Process> processes, int say, int &ans)
{
    for (int t = 0; t < processes.size(); t++)
    {
        vector<pair<int, int>> holes = getFreeHoleSpace();
        Process process = processes[t];

        // This process is Not yet Arrived
        if (process.getArrivalTime() > say)
            continue;

        vector<Segment> segments = process.getSegments();

        sort(holes.begin(), holes.end(), holesDescendingComparator);

        int i = 0;
        for (i = 0; i < segments.size(); i++)
        {
            bool flag = true;

            for (int j = 0; j < holes.size(); j++)
            {
                if (holes[j].second - holes[j].first >= segments[i].getSpace() - 1)
                {
                    holes[j].first = holes[j].first + segments[i].getSpace();
                    flag = false;
                    break;
                }
            }

            sort(holes.begin(), holes.end(), holesDescendingComparator);
            if (flag)
                break;
        }

        // All the segments of t process can be included
        if (i == segments.size())
            return ans = t;
    }
    return ans = -1;
}

Process worstFitMoveProcess(vector<Process> &processes, int index)
{
    vector<pair<int, int>> holes = getFreeHoleSpace();
    Process process = processes[index];

    processes.erase(next(processes.begin(), index));

    vector<Segment> segments = process.getSegments();

    sort(holes.begin(), holes.end(), holesDescendingComparator);

    for (int i = 0; i < segments.size(); i++)
    {
        for (int j = 0; j < holes.size(); j++)
        {
            if (holes[j].second - holes[j].first >= segments[i].getSpace() - 1)
            {
                for (int k = holes[j].first; k < holes[j].first + segments[i].getSpace(); k++)
                    memory[k] = to_string(process.getProcessId()) + " " + to_string(i);

                process.segments[i].start = holes[j].first;
                process.segments[i].end = holes[j].first + segments[i].getSpace() - 1;

                holes[j].first = holes[j].first + segments[i].getSpace();
                break;
            }
        }
        sort(holes.begin(), holes.end(), holesDescendingComparator);
    }
    cout << "\tMM moves process " << process.getProcessId() << " to memory\n";

    return process;
}

void worstFit(map<int, vector<Process>> &events, int last, int arrivalTime, int &turnAroundTime, vector<Process> &processes, map<int, int> &mapAtTime)
{
    // Here i is the time
    for (int i = last; i < arrivalTime; i++)
    {
        // We have a vectors of processes completed at time i
        if (events.find(i) != events.end())
        {
            cout << "t = " << i << ": \n";
            mapAtTime[i] = 10;

            vector<Process> processesAtTime = events[i];

            for (int t = processesAtTime.size() - 1; t >= 0; t--)
            {
                turnAroundTime += (i - processesAtTime[t].getArrivalTime());
                vector<Segment> segments = processesAtTime[t].getSegments();

                for (int j = 0; j < segments.size(); j++)
                    for (int k = segments[j].getStart(); k <= segments[j].getEnd(); k++)
                        memory[k] = " ";

                cout << "\tProcess " << processesAtTime[t].getProcessId() << " completes\n";
                displayMemoryMap();
            }

            // Remove this from map
            events.erase(i);
        }

        // Here ans will keep the value of firstFitProcessMovePossible(processes,i,ans) basically index
        int ans = -1;

        while (processes.size() > 0 && worstFitProcessMovePossible(processes, i, ans) != -1)
        {
            if (mapAtTime[i] == 0)
            {
                cout << "t = " << i << ":\n";
                mapAtTime[i] = 10;
            }

            // Rather than printing top, we'll just print ans'th index
            Process toppi = worstFitMoveProcess(processes, ans);
            events[i + toppi.getProcessDuration()].push_back(toppi);

            displayMemoryMap();
        }
    }
}

void printProcessQueue(vector<Process> processes, Process p)
{
    cout << "\t"
         << "Process " << p.processId << " arrives\n";

    cout << "\tInput Queue:[";

    while (processes.size() > 0)
    {
        cout << processes[0].getProcessId() << " ";
        processes.erase(processes.begin());
    }

    cout << "]" << endl;
}

void firstFitAlgorithm()
{
    int processCount, turnAroundTime = 0;
    cin >> processCount;

    map<int, int> mapAtTime;

    for (int i = 0; i < MEM_SIZE; i++)
        memory[i] = " ";

    // Here first parameter is the time when the event will finish
    map<int, vector<Process>> eventsOnProcess;

    vector<Process> processes;

    int lastTime = 0;

    for (int i = 0; i < processCount; i++)
    {
        int processId;
        cin >> processId;

        int arrivalTime, processDuration;
        cin >> arrivalTime >> processDuration;

        int spaceCount;
        cin >> spaceCount;

        vector<int> segmentSpace(spaceCount);
        for (int j = 0; j < spaceCount; j++)
            cin >> segmentSpace[j];

        Process p(processId, arrivalTime, processDuration, spaceCount, segmentSpace);

        // Removes the case of infinite loop when given wrong inputs
        if (checkValidProcesses(p))
            processes.push_back(p);

        // if arrivalTime is different than the previous one, check all events
        if (lastTime != arrivalTime)
            firstFit(eventsOnProcess, lastTime, arrivalTime, turnAroundTime, processes, mapAtTime);

        // Arrived Now
        if (mapAtTime[arrivalTime] == 0)
        {
            cout << "t = " << arrivalTime << ":\n";
            mapAtTime[arrivalTime] = 10;
        }

        printProcessQueue(processes, p);

        displayMemoryMap();

        lastTime = arrivalTime;
    }

    while ((processes.size() > 0 || eventsOnProcess.size() > 0))
    {
        firstFit(eventsOnProcess, lastTime, lastTime + 1, turnAroundTime, processes, mapAtTime);
        lastTime += 1;
    }

    double averageTurnAroundTime = turnAroundTime * 1.0 / processCount;
    cout << "Average turnaround: " << averageTurnAroundTime << " (" << turnAroundTime << "/" << processCount << ")\n";
}

void bestFitAlgorithm()
{
    int processCount, turnAroundTime = 0;
    cin >> processCount;

    map<int, int> mapAtTime;

    for (int i = 0; i < MEM_SIZE; i++)
        memory[i] = " ";

    // Here first parameter is the time when the event will finish
    map<int, vector<Process>> eventsOnProcess;

    vector<Process> processes;

    int lastTime = 0;

    for (int i = 0; i < processCount; i++)
    {
        int processId;
        cin >> processId;

        int arrivalTime, processDuration;
        cin >> arrivalTime >> processDuration;

        int spaceCount;
        cin >> spaceCount;

        vector<int> segmentSpace(spaceCount);
        for (int j = 0; j < spaceCount; j++)
            cin >> segmentSpace[j];

        Process p(processId, arrivalTime, processDuration, spaceCount, segmentSpace);

        // Removes the case of infinite loop when given wrong inputs
        if (checkValidProcesses(p))
            processes.push_back(p);

        // if arrivalTime is different than the previous one, check all events
        if (lastTime != arrivalTime)
            bestFit(eventsOnProcess, lastTime, arrivalTime, turnAroundTime, processes, mapAtTime);

        // Arrived Now
        if (mapAtTime[arrivalTime] == 0)
        {
            cout << "t = " << arrivalTime << ":\n";
            mapAtTime[arrivalTime] = 10;
        }

        printProcessQueue(processes, p);

        displayMemoryMap();

        lastTime = arrivalTime;
    }
    //Incrementing time by 1 
    while ((processes.size() > 0 || eventsOnProcess.size() > 0))
    {
        bestFit(eventsOnProcess, lastTime, lastTime + 1, turnAroundTime, processes, mapAtTime);
        lastTime += ;
    }

    double averageTurnAroundTime = turnAroundTime * 1.0 / processCount;
    cout << "Average turnaround: " << averageTurnAroundTime << " (" << turnAroundTime << "/" << processCount << ")\n";
}

void worstFitAlgorithm()
{
    int processCount, turnAroundTime = 0;
    cin >> processCount;

    map<int, int> mapAtTime;

    for (int i = 0; i < MEM_SIZE; i++)
        memory[i] = " ";

    // Here first parameter is the time when the event will finish
    map<int, vector<Process>> eventsOnProcess;

    vector<Process> processes;

    int lastTime = 0;

    for (int i = 0; i < processCount; i++)
    {
        int processId;
        cin >> processId;

        int arrivalTime, processDuration;
        cin >> arrivalTime >> processDuration;

        int spaceCount;
        cin >> spaceCount;

        vector<int> segmentSpace(spaceCount);
        for (int j = 0; j < spaceCount; j++)
            cin >> segmentSpace[j];

        Process p(processId, arrivalTime, processDuration, spaceCount, segmentSpace);

        // Removes the case of infinite loop when given wrong inputs
        if (checkValidProcesses(p))
            processes.push_back(p);

        // if arrivalTime is different than the previous one, check all events
        if (lastTime != arrivalTime)
            worstFit(eventsOnProcess, lastTime, arrivalTime, turnAroundTime, processes, mapAtTime);

        // Arrived Now
        if (mapAtTime[arrivalTime] == 0)
        {
            cout << "t = " << arrivalTime << ":\n";
            mapAtTime[arrivalTime] = 10;
        }

        printProcessQueue(processes, p);

        displayMemoryMap();

        lastTime = arrivalTime;
    }

    while ((processes.size() > 0 || eventsOnProcess.size() > 0))
    {
        worstFit(eventsOnProcess, lastTime, lastTime + 1000, turnAroundTime, processes, mapAtTime);
        lastTime += 1000;
    }

    double averageTurnAroundTime = turnAroundTime * 1.0 / processCount;
    cout << "Average turnaround: " << averageTurnAroundTime << " (" << turnAroundTime << "/" << processCount << ")\n";
}

int main()
{
    int fitChoice = -1;

    int t;

    cin >> t;
    while (t--)
    {
        cin >> fitChoice;

        switch (fitChoice)
        {
        case 1:
        {
            firstFitAlgorithm();
            break;
        }
        case 2:
        {
            bestFitAlgorithm();
            break;
        }
        case 3:
        {
            worstFitAlgorithm();
            break;
        }
        default:
        {
            cout << "Invalid Choice";
        }
        }
    }
    return 0;
}
