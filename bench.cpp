#include<iostream>

using namespace std;

// A function to sort the data set.
void Sort(int a[], int n) {
    int i, j, temp;

    for (i = 0; i < n; i++) {
        for (j = i + 1; j < n; j++) {
            if (a[i] > a[j]) {
                temp = a[i];
                a[i] = a[j];
                a[j] = temp;
            }
        }
    }
}

// A function to print all combination of a given length from the given array.
void GenSubset(int a[], int reqLen, int start, int currLen, bool check[], int len) {
    // Return if the currLen is more than the required length.
    if (currLen > reqLen)
        return;
        // If currLen is equal to required length then print the sequence.
    else if (currLen == reqLen) {
        cout << "\t";
        cout << "{ ";
        for (int i = 0; i < len; i++) {
            if (check[i] == true) {
                cout << a[i] << " ";
            }
        }
        cout << "}\n";
        return;
    }
    // If start equals to len then return since no further element left.
    if (start == len) {
        return;
    }
    // For every index we have two options.
    // First is, we select it, means put true in check[] and increment currLen and start.
    check[start] = true;
    GenSubset(a, reqLen, start + 1, currLen + 1, check, len);
    // Second is, we don't select it, means put false in check[] and only start incremented.
    check[start] = false;
    GenSubset(a, reqLen, start + 1, currLen, check, len);
}

int main() {
    int i, n;
    bool check[n];
    cout << "Enter the number of element array have: ";
    cin >> n;

    int arr[n];
    cout << "\n";

    // Take the input of the array.
    for (i = 0; i < n; i++) {
        cout << "Enter " << i + 1 << " element: ";
        cin >> arr[i];
        check[i] = false;
    }

    // Sort the array.
    Sort(arr, n);

    cout << "\nThe subset in the lexicographic order: \n";
    cout << "\t{ }\n";
    for (i = 1; i <= n; i++) {
        GenSubset(arr, i, 0, 0, check, n);
    }
    return 0;
}