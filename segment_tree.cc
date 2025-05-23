#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <cassert>

using namespace std;

class SegmentTree {
private:
    vector<int> tree; // Stores the sum of the range for each node
    vector<int> lazy; // Stores the pending update value for each node
    int n;                // Size of the original array (elements are 0-indexed)
    const int ROOT_NODE = 1; // Root of the segment tree is at index 1

    // Helper function to push lazy updates down to children
    // node: current segment tree node index
    // start, end: range covered by this node
    void push(int node, int start, int end) {
        if (lazy[node] != 0) {
            tree[node] += lazy[node] * (int)(end - start + 1);

            if (start != end) {
                lazy[2 * node] += lazy[node];
                lazy[2 * node + 1] += lazy[node];
            }
            lazy[node] = 0;
        }
    }

    // Recursive function to build the segment tree
    // arr: initial array
    // node: current segment tree node index
    // start, end: range covered by this node (0-indexed based on original array)
    void build_recursive(const vector<int>& arr, int node, int start, int end) {
        if (start == end) {
            tree[node] = (int)arr[start];
            return;
        }
        int mid = start + (end - start) / 2;
        build_recursive(arr, 2 * node, start, mid);
        build_recursive(arr, 2 * node + 1, mid + 1, end);
        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }

    // Recursive function for range updates
    // node: current segment tree node index
    // start, end: range covered by this node
    // l, r: update range query (0-indexed)
    // val: value to add
    void update_recursive(int node, int start, int end, int l, int r, int val) {
        push(node, start, end);

        // Case 1: Current segment [start, end] is completely outside the update range [l, r]
        if (start > end || start > r || end < l) {
            return;
        }

        // Case 2: Current segment [start, end] is completely inside the update range [l, r]
        if (l <= start && end <= r) {
            tree[node] += (int)val * (end - start + 1);
            if (start != end) {
                lazy[2 * node] += val;
                lazy[2 * node + 1] += val;
            }
            return;
        }

        // Case 3: Partial overlap. Recurse on children.
        int mid = start + (end - start) / 2;
        update_recursive(2 * node, start, mid, l, r, val);
        update_recursive(2 * node + 1, mid + 1, end, l, r, val);

        tree[node] = tree[2 * node] + tree[2 * node + 1];
    }

    // Recursive function for range queries
    // node: current segment tree node index
    // start, end: range covered by this node
    // l, r: query range (0-indexed)
    int query_recursive(int node, int start, int end, int l, int r) {
        // Case 1: Current segment [start, end] is completely outside the query range [l, r]
        if (start > end || start > r || end < l) {
            return 0;
        }

        push(node, start, end);

        // Case 2: Current segment [start, end] is completely inside the query range [l, r]
        if (l <= start && end <= r) {
            return tree[node];
        }

        // Case 3: Partial overlap. Recurse on children and combine results.
        int mid = start + (end - start) / 2;
        int p1 = query_recursive(2 * node, start, mid, l, r);
        int p2 = query_recursive(2 * node + 1, mid + 1, end, l, r);
        return p1 + p2;
    }

public:
    // Constructor
    // arr: initial array
    // Time Complexity: O(N)
    // Space Complexity: O(N) for tree and lazy arrays.
    SegmentTree(const vector<int>& arr) {
        n = arr.size();
        if (n == 0) return;
        tree.resize(4 * n); // Segment tree needs up to 4*n space
        lazy.resize(4 * n, 0); // Initialize lazy values to 0 (no pending update)
        build_recursive(arr, ROOT_NODE, 0, n - 1);
    }

    // Public method for range update
    // Adds 'val' to all elements in arr[l...r]
    // Time complexity: O(log N) where N is the size of the original array.
    void updateRange(int l, int r, int val) {
        if (n == 0 || l < 0 || r >= n || l > r) {
            return;
        }
        update_recursive(ROOT_NODE, 0, n - 1, l, r, val);
    }

    // Public method for range query
    // Returns sum of elements in arr[l...r]
    // Time complexity: O(log N) where N is the size of the original array.
    int queryRange(int l, int r) {
        if (n == 0 || l < 0 || r >= n || l > r) {
            return 0;
        }
        return query_recursive(ROOT_NODE, 0, n - 1, l, r);
    }
};


void runSegmentTreeTests() {
    cout << "\nRunning Segment Tree Tests..." << endl;

    // Test Case 1: Basic build and query
    {
        vector<int> arr = {1, 2, 3, 4, 5};
        SegmentTree st(arr);
        assert(st.queryRange(0, 4) == 15);
        assert(st.queryRange(1, 3) == 9);
        assert(st.queryRange(0, 0) == 1);
        assert(st.queryRange(4, 4) == 5);
        assert(st.queryRange(2, 2) == 3);
        cout << "Test 1 passed." << endl;
    }

    // Test Case 2: Basic update and query
    {
        vector<int> arr = {1, 2, 3, 4, 5};
        SegmentTree st(arr);
        st.updateRange(1, 3, 10);
        assert(st.queryRange(0, 4) == 45);
        assert(st.queryRange(1, 3) == 39);
        assert(st.queryRange(0, 1) == 13);
        assert(st.queryRange(3, 4) == 19);
        assert(st.queryRange(2, 2) == 13);
        cout << "Test 2 passed." << endl;
    }

    // Test Case 3: Multiple updates
    {
        vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8};
        SegmentTree st(arr);
        st.updateRange(0, 7, 1);
        st.updateRange(2, 5, -2);
        // Expected arr: {2, 3, 2, 3, 4, 5, 8, 9}
        assert(st.queryRange(0, 7) == (1+8)*8/2 + 8 + (-2)*4);
        assert(st.queryRange(0, 1) == 2 + 3);
        assert(st.queryRange(2, 5) == 2 + 3 + 4 + 5);
        assert(st.queryRange(6, 7) == 8 + 9);
        assert(st.queryRange(3, 4) == 3 + 4);
        cout << "Test 3 passed." << endl;
    }

    // Test Case 4: Edge cases - single element array
    {
        vector<int> arr = {100};
        SegmentTree st(arr);
        assert(st.queryRange(0, 0) == 100);
        st.updateRange(0, 0, 50);
        assert(st.queryRange(0, 0) == 150);
        cout << "Test 4 passed." << endl;
    }

    // Test Case 5: Edge cases - update/query invalid ranges
    {
        vector<int> arr = {1, 2, 3};
        SegmentTree st(arr);
        assert(st.queryRange(-1, 1) == 0);
        assert(st.queryRange(2, 5) == 0);
        assert(st.queryRange(3, 4) == 0);
        st.updateRange(-1, 1, 10);
        assert(st.queryRange(0, 2) == 1 + 2 + 3);
        st.updateRange(2, 5, 5);
        assert(st.queryRange(0, 2) == 1 + 2 + 3);
        cout << "Test 5 passed." << endl;
    }

    cout << "All Segment Tree tests passed." << endl;
}


void runSegmentTreeSample() {
    cout << "Running Segment Tree Sample..." << endl;
    vector<int> arr = {1, 2, 3, 4, 5, 6, 7, 8};
    int n_elements = arr.size();

    SegmentTree st(arr);

    cout << "Initial sum [0, 7]: " << st.queryRange(0, n_elements - 1) << endl;
    cout << "Sum [2, 5]: " << st.queryRange(2, 5) << endl;

    cout << "\nUpdate: Add 10 to range [1, 4]" << endl;
    st.updateRange(1, 4, 10);

    cout << "Sum [0, 7] after update: " << st.queryRange(0, n_elements - 1) << endl;
    cout << "Sum [2, 5] after update: " << st.queryRange(2, 5) << endl;
    cout << "Sum [0, 1] after update: " << st.queryRange(0, 1) << endl;
    cout << "Sum [4, 6] after update: " << st.queryRange(4, 6) << endl;

    cout << "\nUpdate: Add -5 to range [3, 6]" << endl;
    st.updateRange(3, 6, -5);

    cout << "Sum [0, 7] after update: " << st.queryRange(0, n_elements - 1) << endl;
    cout << "Sum [2, 5] after update: " << st.queryRange(2, 5) << endl;

}

int main() {
    runSegmentTreeTests();
    runSegmentTreeSample();
    return 0;
}