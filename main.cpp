#include <iostream>
#include <vector>
#include <iomanip>

using namespace std;

// ============================================================
// CACHE LINE
// ============================================================

struct CacheLine {
    int blockNumber;
    int tag;
    bool valid;
    int lastUsed;
    int insertionOrder;

    CacheLine() {
        blockNumber = -1;
        tag = -1;
        valid = false;
        lastUsed = 0;
        insertionOrder = 0;
    }
};

// ============================================================
// CACHE SIMULATOR
// ============================================================

class CacheSimulator {

private:

    int memorySize;
    int cacheLines;
    int blockSize;

    int hits;
    int misses;
    int replacements;

    int accessCounter;
    int insertionCounter;

    vector<CacheLine> cache;

public:

    // Constructor
    CacheSimulator(int memSize, int lines, int blkSize) {

        memorySize = memSize;
        cacheLines = lines;
        blockSize = blkSize;

        hits = 0;
        misses = 0;
        replacements = 0;

        accessCounter = 0;
        insertionCounter = 0;

        cache.resize(cacheLines);
    }

    // --------------------------------------------------------
    // RESET CACHE
    // --------------------------------------------------------

    void reset() {

        for (int i = 0; i < cacheLines; i++) {
            cache[i] = CacheLine();
        }

        hits = 0;
        misses = 0;
        replacements = 0;

        accessCounter = 0;
        insertionCounter = 0;
    }

    // --------------------------------------------------------
    // DISPLAY CACHE
    // --------------------------------------------------------

    void displayCache() {

        cout << "\n---------------- CACHE STATUS ----------------\n";

        for (int i = 0; i < cacheLines; i++) {

            cout << "Index " << i << " -> ";

            if (!cache[i].valid) {
                cout << "EMPTY";
            }
            else {
                cout << "Block: " << cache[i].blockNumber
                     << " | Tag: " << cache[i].tag
                     << " | Valid: " << cache[i].valid;
            }

            cout << endl;
        }

        cout << "-----------------------------------------------\n";
    }

    // --------------------------------------------------------
    // DIRECT MAPPING
    // --------------------------------------------------------

    void directMapping(int address) {

        int blockNumber = address / blockSize;
        int offset = address % blockSize;

        int index = blockNumber % cacheLines;
        int tag = blockNumber / cacheLines;

        cout << "\nAddress Breakdown:\n";
        cout << "Block Number : " << blockNumber << endl;
        cout << "Tag          : " << tag << endl;
        cout << "Index        : " << index << endl;
        cout << "Offset       : " << offset << endl;

        accessCounter++;

        if (cache[index].valid &&
            cache[index].tag == tag) {

            cout << "Result       : HIT\n";

            hits++;
            cache[index].lastUsed = accessCounter;
        }
        else {

            cout << "Result       : MISS\n";

            misses++;

            if (cache[index].valid) {
                replacements++;
            }

            cache[index].blockNumber = blockNumber;
            cache[index].tag = tag;
            cache[index].valid = true;

            cache[index].lastUsed = accessCounter;
            cache[index].insertionOrder = ++insertionCounter;
        }

        displayCache();
    }

    // --------------------------------------------------------
    // FULLY ASSOCIATIVE + FIFO
    // --------------------------------------------------------

    void fullyAssociativeFIFO(int address) {

        int blockNumber = address / blockSize;
        int offset = address % blockSize;

        cout << "\nAddress Breakdown:\n";
        cout << "Block Number : " << blockNumber << endl;
        cout << "Offset       : " << offset << endl;

        accessCounter++;

        // Search entire cache
        for (int i = 0; i < cacheLines; i++) {

            if (cache[i].valid &&
                cache[i].blockNumber == blockNumber) {

                cout << "Result       : HIT\n";

                hits++;
                cache[i].lastUsed = accessCounter;

                displayCache();
                return;
            }
        }

        // MISS
        cout << "Result       : MISS\n";
        misses++;

        // Find empty cache line
        for (int i = 0; i < cacheLines; i++) {

            if (!cache[i].valid) {

                cache[i].blockNumber = blockNumber;
                cache[i].tag = blockNumber;
                cache[i].valid = true;

                cache[i].lastUsed = accessCounter;
                cache[i].insertionOrder = ++insertionCounter;

                displayCache();
                return;
            }
        }

        // Cache full - FIFO replacement
        int replaceIndex = 0;

        for (int i = 1; i < cacheLines; i++) {

            if (cache[i].insertionOrder <
                cache[replaceIndex].insertionOrder) {

                replaceIndex = i;
            }
        }

        cout << "FIFO Replacement: Index "
             << replaceIndex << endl;

        replacements++;

        cache[replaceIndex].blockNumber = blockNumber;
        cache[replaceIndex].tag = blockNumber;
        cache[replaceIndex].valid = true;

        cache[replaceIndex].lastUsed = accessCounter;
        cache[replaceIndex].insertionOrder = ++insertionCounter;

        displayCache();
    }

    // --------------------------------------------------------
    // FULLY ASSOCIATIVE + LRU
    // --------------------------------------------------------

    void fullyAssociativeLRU(int address) {

        int blockNumber = address / blockSize;
        int offset = address % blockSize;

        cout << "\nAddress Breakdown:\n";
        cout << "Block Number : " << blockNumber << endl;
        cout << "Offset       : " << offset << endl;

        accessCounter++;

        // Search entire cache
        for (int i = 0; i < cacheLines; i++) {

            if (cache[i].valid &&
                cache[i].blockNumber == blockNumber) {

                cout << "Result       : HIT\n";

                hits++;
                cache[i].lastUsed = accessCounter;

                displayCache();
                return;
            }
        }

        // MISS
        cout << "Result       : MISS\n";
        misses++;

        // Find empty cache line
        for (int i = 0; i < cacheLines; i++) {

            if (!cache[i].valid) {

                cache[i].blockNumber = blockNumber;
                cache[i].tag = blockNumber;
                cache[i].valid = true;

                cache[i].lastUsed = accessCounter;
                cache[i].insertionOrder = ++insertionCounter;

                displayCache();
                return;
            }
        }

        // Cache full - LRU replacement
        int replaceIndex = 0;

        for (int i = 1; i < cacheLines; i++) {

            if (cache[i].lastUsed <
                cache[replaceIndex].lastUsed) {

                replaceIndex = i;
            }
        }

        cout << "LRU Replacement: Index "
             << replaceIndex << endl;

        replacements++;

        cache[replaceIndex].blockNumber = blockNumber;
        cache[replaceIndex].tag = blockNumber;
        cache[replaceIndex].valid = true;

        cache[replaceIndex].lastUsed = accessCounter;
        cache[replaceIndex].insertionOrder = ++insertionCounter;

        displayCache();
    }

    // --------------------------------------------------------
    // SET ASSOCIATIVE + LRU
    // --------------------------------------------------------

    void setAssociative(int address, int ways) {

        int blockNumber = address / blockSize;
        int offset = address % blockSize;

        int numberOfSets = cacheLines / ways;

        int setIndex = blockNumber % numberOfSets;
        int tag = blockNumber / numberOfSets;

        int start = setIndex * ways;
        int end = start + ways;

        cout << "\nAddress Breakdown:\n";
        cout << "Block Number : " << blockNumber << endl;
        cout << "Tag          : " << tag << endl;
        cout << "Set Index    : " << setIndex << endl;
        cout << "Offset       : " << offset << endl;

        accessCounter++;

        // Search inside selected set
        for (int i = start; i < end; i++) {

            if (cache[i].valid &&
                cache[i].tag == tag) {

                cout << "Result       : HIT\n";

                hits++;
                cache[i].lastUsed = accessCounter;

                displayCache();
                return;
            }
        }

        // MISS
        cout << "Result       : MISS\n";
        misses++;

        // Find empty line inside set
        for (int i = start; i < end; i++) {

            if (!cache[i].valid) {

                cache[i].blockNumber = blockNumber;
                cache[i].tag = tag;
                cache[i].valid = true;

                cache[i].lastUsed = accessCounter;
                cache[i].insertionOrder = ++insertionCounter;

                displayCache();
                return;
            }
        }

        // Set full - LRU replacement
        int replaceIndex = start;

        for (int i = start + 1; i < end; i++) {

            if (cache[i].lastUsed <
                cache[replaceIndex].lastUsed) {

                replaceIndex = i;
            }
        }

        cout << "LRU Replacement: Index "
             << replaceIndex << endl;

        replacements++;

        cache[replaceIndex].blockNumber = blockNumber;
        cache[replaceIndex].tag = tag;
        cache[replaceIndex].valid = true;

        cache[replaceIndex].lastUsed = accessCounter;
        cache[replaceIndex].insertionOrder = ++insertionCounter;

        displayCache();
    }

    // --------------------------------------------------------
    // PROCESS ADDRESS
    // --------------------------------------------------------

    void processAddress(int address, int mapping, int ways = 1) {

        if (address < 0 || address >= memorySize) {

            cout << "Invalid memory address!\n";
            cout << "Valid addresses are 0 to "
                 << memorySize - 1 << endl;

            return;
        }

        if (mapping == 1) {
            directMapping(address);
        }
        else if (mapping == 2) {
            fullyAssociativeFIFO(address);
        }
        else if (mapping == 3) {
            fullyAssociativeLRU(address);
        }
        else if (mapping == 4) {
            setAssociative(address, ways);
        }
    }

    // --------------------------------------------------------
    // DISPLAY STATISTICS
    // --------------------------------------------------------

    void displayStatistics() {

        int total = hits + misses;

        cout << "\n====================================\n";
        cout << "          CACHE STATISTICS          \n";
        cout << "====================================\n";

        cout << "Total Accesses : " << total << endl;
        cout << "Total Hits     : " << hits << endl;
        cout << "Total Misses   : " << misses << endl;
        cout << "Replacements   : " << replacements << endl;

        if (total > 0) {

            double hitRate =
                (double)hits / total * 100;

            double missRate =
                (double)misses / total * 100;

            cout << fixed << setprecision(2);

            cout << "Hit Rate       : "
                 << hitRate << "%" << endl;

            cout << "Miss Rate      : "
                 << missRate << "%" << endl;
        }
        else {

            cout << "Hit Rate       : 0%\n";
            cout << "Miss Rate      : 0%\n";
        }

        cout << "====================================\n";
    }
};

// ============================================================
// MAIN
// ============================================================

int main() {

    int memorySize;
    int cacheLines;
    int blockSize;

    cout << "====================================\n";
    cout << "          CACHE SIMULATOR            \n";
    cout << "====================================\n";

    // Memory size
    cout << "Enter memory size (bytes): ";
    cin >> memorySize;

    if (memorySize <= 0) {
        cout << "Invalid memory size!\n";
        return 0;
    }

    // Cache size
    cout << "Enter cache size (number of lines): ";
    cin >> cacheLines;

    if (cacheLines <= 0) {
        cout << "Invalid cache size!\n";
        return 0;
    }

    // Block size
    cout << "Enter block size (bytes): ";
    cin >> blockSize;

    if (blockSize <= 0) {
        cout << "Invalid block size!\n";
        return 0;
    }

    if (memorySize % blockSize != 0) {

        cout << "Memory size must be divisible "
             << "by block size!\n";

        return 0;
    }

    int memoryBlocks = memorySize / blockSize;

    cout << "\nMemory Blocks: "
         << memoryBlocks << endl;

    // Create simulator
    CacheSimulator simulator(
        memorySize,
        cacheLines,
        blockSize
    );

    // ========================================================
    // MAIN MENU
    // ========================================================

    while (true) {

        cout << "\n====================================\n";
        cout << "             MAIN MENU              \n";
        cout << "====================================\n";

        cout << "1. Direct Mapping\n";
        cout << "2. Fully Associative + FIFO\n";
        cout << "3. Fully Associative + LRU\n";
        cout << "4. Set Associative + LRU\n";
        cout << "5. Display Statistics\n";
        cout << "6. Display Cache\n";
        cout << "7. Reset Cache\n";
        cout << "8. Exit\n";

        cout << "Enter your choice: ";

        int choice;
        cin >> choice;

        // Exit
        if (choice == 8) {
            break;
        }

        // Statistics
        if (choice == 5) {

            simulator.displayStatistics();
            continue;
        }

        // Display cache
        if (choice == 6) {

            simulator.displayCache();
            continue;
        }

        // Reset
        if (choice == 7) {

            simulator.reset();

            cout << "\nCache and statistics reset!\n";

            continue;
        }

        // Invalid choice
        if (choice < 1 || choice > 7) {

            cout << "Invalid choice!\n";

            continue;
        }

        // Set associative configuration
        int ways = 1;

        if (choice == 4) {

            cout << "\nEnter number of ways: ";
            cin >> ways;

            if (ways <= 0 ||
                cacheLines % ways != 0) {

                cout << "Invalid number of ways!\n";
                cout << "Ways must divide cache size exactly.\n";

                continue;
            }
        }

        // Address input
        if (choice >= 1 && choice <= 4) {

            while (true) {

                int address;

                cout << "\nEnter memory address "
                     << "(-1 to return to menu): ";

                cin >> address;

                if (address == -1) {
                    break;
                }

                simulator.processAddress(
                    address,
                    choice,
                    ways
                );
            }
        }
    }

    // Final statistics
    simulator.displayStatistics();

    cout << "\nThank you for using Cache Simulator!\n";

    return 0;
}
