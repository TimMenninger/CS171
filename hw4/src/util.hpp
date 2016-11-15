#ifndef UTIL_HPP
#define UTIL_HPP

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <utility> // pair
#include <vector>
#include <math.h>
#include <unordered_map>

/**************************** Graphics Util Functions *************************/

inline float UTIL_degrees(float radians) {
    return radians / M_PI * 180;
}
inline float UTIL_radians(float degrees) {
    return degrees / 180 * M_PI;
}

/******************************* ETC STD lib stuff ****************************/

// binary heap
template<typename T, class Compare = std::less<T>, class Hash = std::hash<T> >
// TODO: check that Compare is in fact a compare concept?
class UTIL_binary_heap {
private:
    template<typename E>
    struct node_t {
        E elem;

        node_t<E> *parent;
        node_t<E> *left;
        node_t<E> *right;

        explicit node_t<E>() : elem(), parent(NULL), left(NULL), right(NULL) {
            //
        }

        void reset() {
            this->elem = E();
            this->parent = NULL;
            this->left = NULL;
            this->right = NULL;
        }
    };
    typedef typename std::unordered_multimap<T, node_t<T>*, Hash>::iterator
        etn_it;

    Compare comp;
    std::vector<node_t<T>*> levels;
    std::unordered_multimap<T, node_t<T>*, Hash> elem_to_node;
    int cursor;

    inline const int get_curr_level() {
        return this->levels.size() - 1;
    }

    void swap_node(node_t<T> *node_0, node_t<T> *node_1) {
        assert(node_0 != NULL && node_1 != NULL);
        etn_it etn_entry_0 = find_etn_entry(node_0);
        etn_it etn_entry_1 = find_etn_entry(node_1);
        assert(etn_entry_0 != this->elem_to_node.end());
        assert(etn_entry_1 != this->elem_to_node.end());

        // first swap elem values
        T temp_elem = node_0->elem;
        node_0->elem = node_1->elem;
        node_1->elem = temp_elem;

        // update elem_to_node
        etn_entry_0->second = node_1;
        etn_entry_1->second = node_0;
    }

    etn_it find_etn_entry(node_t<T> *node) {
        const T &key = node->elem;

        // find the key, node pair in elem_to_node and return the match if any
        std::pair<etn_it, etn_it> key_matches =
            this->elem_to_node.equal_range(key);
        etn_it perfect_match = this->elem_to_node.end();
        for (etn_it it = key_matches.first; it != key_matches.second; it++) {
            if (it->second == node) {
                perfect_match = it;
                break;
            }
        }
        return perfect_match;
    }

    void extract(node_t<T> *node) {
        // move cursor back
        this->cursor--;
        assert(this->cursor > -1);

        // swap top elem with last elem and remove the new last elem
        swap_node(node, this->levels.back() + this->cursor);
        node_t<T> *parent_node = this->levels.back()[this->cursor].parent;
        if (parent_node != NULL) {
            if (this->cursor % 2 == 0) {
                parent_node->left = NULL;
            } else {
                parent_node->right = NULL;
            }
            node_t<T> *obsolete_node = this->levels.back() + this->cursor;
            etn_it etn_entry = find_etn_entry(obsolete_node);
            if (etn_entry != this->elem_to_node.end()) {
                this->elem_to_node.erase(etn_entry);
            }
            obsolete_node->reset();
        }

        // move top node down until it is smaller than both of its children
        node_t<T> *temp = this->levels.front();
        while (true) {
            int side = 0;
            if (temp->left != NULL) {
                if (comp(temp->left->elem, temp->elem)) {
                    side = -1;
                }
            }
            if (temp->right != NULL) {
                if (comp(temp->right->elem, temp->elem)) {
                    if (side == -1) {
                        if (comp(temp->right->elem, temp->left->elem)) {
                            side = 1;
                        }
                    } else {
                        side = 1;
                    }
                }
            }

            if (side == -1) {
                swap_node(temp, temp->left);
                temp = temp->left;
            } else if (side == 1) {
                swap_node(temp, temp->right);
                temp = temp->right;
            } else {
                break;
            }
        }

        // if last level has been exhausted, delete last level
        if (this->cursor == 0) {
            delete[] this->levels.back();
            this->levels.pop_back();
            this->cursor = 1 << get_curr_level();;
        }
    }

public:
    explicit UTIL_binary_heap(const Compare &comp) :
        comp(comp),
        levels(),
        elem_to_node(),
        cursor(0)
    {
        //
    }
    ~UTIL_binary_heap() {
        clear();
    }

    bool is_empty() {
        return this->levels.size() == 0;
    }

    // TODO: change this to const reference
    const T top() {
        if (is_empty()) {
            return T();
        }
        return this->levels.front()->elem;
    }

    void push(const T &elem) {
        // if bottom level is full, create new level
        int cur_level = get_curr_level();
        if (cur_level == -1) {                              // empty heap case
            levels.push_back(new node_t<T>[1]);
            this->cursor = 0;
        } else if (this->cursor >= 1 << cur_level) {        // non-empty case
            levels.push_back(new node_t<T>[1 << (cur_level + 1)]);
            this->cursor = 0;
        }

        // insert into lowest level
        cur_level = get_curr_level();
        node_t<T> *new_node = levels.back() + this->cursor;
        node_t<T> *parent_node = cur_level == 0 ?
            NULL : levels.at(cur_level - 1) + this->cursor / 2;
        new_node->elem = elem;
        new_node->parent = parent_node;
        if (parent_node != NULL) {
            if (this->cursor % 2 == 0) {
                parent_node->left = new_node;
            } else {
                parent_node->right = new_node;
            }
        }
        this->elem_to_node.insert({elem, new_node});
        this->cursor++;

        // swap until heap is in correct order
        while (parent_node != NULL && comp(new_node->elem, parent_node->elem)) {
            swap_node(new_node, parent_node);
            new_node = parent_node;
            parent_node = new_node->parent;
        }
    }

    void pop() {
        if (is_empty()) {
            return;
        }
        extract(this->levels.front());
    }

    void remove(const T &elem) {
        if (this->elem_to_node.find(elem) != this->elem_to_node.end()) {
            extract(this->elem_to_node.find(elem)->second);
        }
    }

    void clear() {
        for (const auto &level : this->levels) {
            delete[] level;
        }
        this->levels.clear();
        this->elem_to_node.clear();
        this->cursor = 0;
    }
};

#endif