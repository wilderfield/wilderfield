#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"
#include "wilderfield/priority_map.hpp"

#include <vector>
#include <unordered_map>
#include <unordered_set>

#include <cstdlib> // For std::rand and std::srand
#include <ctime>   // For std::time

TEST_CASE("PriorityMap operations are tested", "[PriorityMap]") {

    wilderfield::PriorityMap<int, int> pmap;

    SECTION("Checking empty() and erase()") {
        REQUIRE(pmap.Empty());
        ++pmap[7];
        REQUIRE(!pmap.Empty());
        pmap.Erase(7);
        REQUIRE(pmap.Empty());
    }

    SECTION("Checking size()") {
        ++pmap[7];
        REQUIRE(pmap.Size() == 1);
    }

    SECTION("Checking count()") {
        ++pmap[7];
        REQUIRE(pmap.Count(7) == 1);
        REQUIRE(pmap.Count(8) == 0);
    }

    SECTION("Checking access by operator[]") {
        ++pmap[7];
        ++pmap[7];
        REQUIRE(pmap[7] == 2);
    }

    SECTION("Check default with operator[]") {
        REQUIRE(pmap[7] == 0);
    }

    SECTION("Check assignment") {
        pmap[7] = 456;
        REQUIRE(pmap[7] == 456);
    }

    SECTION("Check decrement") {
        --pmap[7];
        ++pmap[9];
        REQUIRE(pmap[7] == -1);
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(maxKey == 9);
            REQUIRE(maxVal == 1);
        }
    }

    SECTION("Check decrement 2") {
        --pmap[7];
        ++pmap[8];
        ++pmap[9];
        ++pmap[9];
        REQUIRE(pmap[7] == -1);
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(maxKey == 9);
            REQUIRE(maxVal == 2);
        }
    }

    SECTION("Checking top()") {
        ++pmap[7];
        ++pmap[7];
        ++pmap[7];
        ++pmap[11];
        ++pmap[11];
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(maxKey == 7);
            REQUIRE(maxVal == 3);
        }
        --pmap[7];
        --pmap[7];
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(maxKey == 11);
            REQUIRE(maxVal == 2);
        }
    }

    SECTION("Checking pop()") {
        ++pmap[7];
        ++pmap[7];
        ++pmap[7];
        ++pmap[11];
        ++pmap[11];
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(maxKey == 7);
            REQUIRE(maxVal == 3);
            pmap.Pop();
        }
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(maxKey == 11);
            REQUIRE(maxVal == 2);
            pmap.Pop();
        }
        REQUIRE(pmap.Empty());
    }

    SECTION("Checking frequency map") {

        wilderfield::PriorityMap<char, int> pmap;
        std::unordered_map<char, int> umap;

        std::string s = "supercalifragilisticexpialidocious";

        for (auto c : s) {
            ++pmap[c];
            ++umap[c];
        }
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(pmap[maxKey] == umap[maxKey]);
            REQUIRE(maxKey == 'i');
            REQUIRE(maxVal == 7);
        }
    }

    SECTION("Checking khan's algo") {

        wilderfield::PriorityMap<int, int, std::less<int>> pmap;

        // Test graph
        std::vector<std::vector<int>> graph(6);
        graph[0].push_back(1);
        graph[0].push_back(3);
        graph[2].push_back(0);
        graph[2].push_back(4);
        graph[3].push_back(1);
        graph[4].push_back(3);
        graph[4].push_back(5);
        graph[5].push_back(1);

        // Initialize pmap to have all nodes with ingress 0
        for (int u = 0; u < graph.size(); u++) {
            pmap[u] = 0;
        }

        // Calculate indegrees for each node
        for (int u = 0; u < graph.size(); u++) {
            for (auto v : graph[u]) {
                ++pmap[v];
            }
        }

        std::vector<int> topological;

        // Valid topological sorts for this graph
        std::vector<std::vector<int>> golden = {{2,0,4,3,5,1},{2,4,0,3,5,1},{2,4,5,0,3,1}};

        while (!pmap.Empty()) {
            auto [u, minVal] = pmap.Top(); pmap.Pop();
            REQUIRE(minVal == 0);
            topological.push_back(u);
            for (auto v : graph[u]) {
                --pmap[v];
            }
        }

	bool valid = false;
        for (auto& gr : golden) {
            if (topological == gr) {
                valid |= true;
            }
        }

        REQUIRE(valid);

    }

    SECTION("Lots of keys") {
        for (int i = 0; i < 1000; i++) {
            ++pmap[i];
        }
        REQUIRE(pmap.Size() == 1000);
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(maxKey < 1000);
            REQUIRE(maxVal == 1);
        }
        ++pmap[7];
        {
            auto [maxKey, maxVal] = pmap.Top();
            REQUIRE(maxKey == 7);
            REQUIRE(maxVal == 2);
        }

    }

    SECTION("Multiple key updates and retrieval") {
        pmap[1] = 50;
        pmap[2] = 50;
        pmap[3] = 100;
        REQUIRE(pmap[1] == 50);
        REQUIRE(pmap[2] == 50);
        REQUIRE(pmap[3] == 100);
        auto [maxKey, maxVal] = pmap.Top();
        REQUIRE(maxKey == 3);
        REQUIRE(maxVal == 100);
    }

    SECTION("Repeated increments and decrements") {
        ++pmap[10];
        ++pmap[10];
        --pmap[10];
        REQUIRE(pmap[10] == 1);
        ++pmap[5];
        ++pmap[5];
        ++pmap[5];
        --pmap[5];
        --pmap[5];
        REQUIRE(pmap[5] == 1);
        auto [maxKey, maxVal] = pmap.Top();
        REQUIRE((maxKey == 10 || maxKey == 5));
        REQUIRE(maxVal == 1);
    }


    SECTION("Stress Test with Random Keys") {
        std::srand(std::time(nullptr)); // Seed the random number generator
        wilderfield::PriorityMap<int, int> pmap;

        int currentMaxVal = std::numeric_limits<int>::min();
        std::unordered_set<int> currentMaxKeys;

        for (int i = 0; i < 1000; ++i) {
            int key = std::rand() % 100; // keys between 0 and 99

            // Insert random key with random value if it doesn't exist
            if (pmap.Count(key) == 0) {
                int val = std::rand() % 100; // values between 0 and 99
                pmap[key] = val;
                if (val > currentMaxVal) {
                    currentMaxVal = val;
                    currentMaxKeys = {key};
                }
                else if (val == currentMaxVal) {
                    currentMaxKeys.insert(key);
                }
            } else {
                ++pmap[key];
                if (pmap[key] > currentMaxVal) {
                    currentMaxVal = pmap[key];
                    currentMaxKeys = {key};
                }
                else if (pmap[key] == currentMaxVal) {
                    currentMaxKeys.insert(key);
                }
            }
        }

        // Verify the top element is the same as the calculated maximum
        auto [maxKey, maxVal] = pmap.Top();
        REQUIRE(maxVal == currentMaxVal);
        REQUIRE(currentMaxKeys.count(maxKey) >= 1);
    }

    SECTION("Stress Test with Random Keys MinHeap") {
        std::srand(std::time(nullptr)); // Seed the random number generator
        wilderfield::PriorityMap<int, int, std::less<int>> pmap;

        int currentMinVal = std::numeric_limits<int>::max();
        std::unordered_set<int> currentMinKeys;

        for (int i = 0; i < 1000; ++i) {
            int key = std::rand() % 100; // keys between 0 and 99

            // Insert random key with random value if it doesn't exist
            if (pmap.Count(key) == 0) {
                int val = std::rand() % 100; // values between 0 and 99
                pmap[key] = val;
                if (val < currentMinVal) {
                    currentMinVal = val;
                    currentMinKeys = {key};
                }
                else if (val == currentMinVal) {
                    currentMinKeys.insert(key);
                }
            } else {
                --pmap[key];
                if (pmap[key] < currentMinVal) {
                    currentMinVal = pmap[key];
                    currentMinKeys = {key};
                }
                else if (pmap[key] == currentMinVal) {
                    currentMinKeys.insert(key);
                }
            }
        }

        // Verify the top element is the same as the calculated minimum
        auto [minKey, minVal] = pmap.Top();
        REQUIRE(minVal == currentMinVal);
        REQUIRE(currentMinKeys.count(minKey) >= 1);
    }


}

