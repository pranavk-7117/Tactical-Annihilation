#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include <vector>
#include <algorithm>
#include <stdexcept>

// ============================================================================
// [OOP CONCEPT: Generic Programming (Templates)]
// Generic Min-Priority Queue implementing a binary heap data structure.
// Meets the requirements for Unit 6 (Templates & Generics).
// ============================================================================
template <typename T, typename Comparator>
class MinPriorityQueue {
private:
    std::vector<T> heap;
    Comparator comp;

    void siftUp(size_t index) {
        while (index > 0) {
            size_t parent = (index - 1) / 2;
            if (comp(heap[index], heap[parent])) {
                std::swap(heap[index], heap[parent]);
                index = parent;
            } else {
                break;
            }
        }
    }

    void siftDown(size_t index) {
        size_t size = heap.size();
        while (true) {
            size_t leftChild = 2 * index + 1;
            size_t rightChild = 2 * index + 2;
            size_t smallest = index;

            if (leftChild < size && comp(heap[leftChild], heap[smallest])) {
                smallest = leftChild;
            }
            if (rightChild < size && comp(heap[rightChild], heap[smallest])) {
                smallest = rightChild;
            }

            if (smallest != index) {
                std::swap(heap[index], heap[smallest]);
                index = smallest;
            } else {
                break;
            }
        }
    }

public:
    MinPriorityQueue() : comp(Comparator()) {}
    explicit MinPriorityQueue(Comparator c) : comp(c) {}

    bool empty() const {
        return heap.empty();
    }

    size_t size() const {
        return heap.size();
    }

    void push(const T& item) {
        heap.push_back(item);
        siftUp(heap.size() - 1);
    }

    T top() const {
        if (heap.empty()) {
            throw std::out_of_range("MinPriorityQueue::top() called on empty queue");
        }
        return heap.front();
    }

    T pop() {
        if (heap.empty()) {
            throw std::out_of_range("MinPriorityQueue::pop() called on empty queue");
        }
        T topItem = heap.front();
        heap.front() = heap.back();
        heap.pop_back();
        if (!heap.empty()) {
            siftDown(0);
        }
        return topItem;
    }

    void clear() {
        heap.clear();
    }
};

#endif // PRIORITYQUEUE_H
