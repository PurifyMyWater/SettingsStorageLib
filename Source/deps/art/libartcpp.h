#ifndef LIBARTCPP_H
#define LIBARTCPP_H

#include <cassert>
#include "art.h"

/**
 * @brief A C++ wrapper for the ART library
 * It can store pointers to types of template typename ValueType
 * The user is responsable for the memory management of the values stored in the tree.
 */
template<typename ValueType>
class AdaptiveRadixTree
{
public:
    /**
     * @brief Construct a new Adaptive Radix Tree object
     */
    AdaptiveRadixTree();

    /**
     * @brief Destroy the Adaptive Radix Tree object
     */
    ~AdaptiveRadixTree();

    /**
     * Disallow copying or moving the object.
     */
    AdaptiveRadixTree& operator=(AdaptiveRadixTree&&) = delete;

    uint64_t size();

    ValueType* insert(const char* key, int key_len, ValueType* value);

    ValueType* insertIfNotExists(const char* key, int key_len, ValueType* value);

    ValueType* deleteValue(const char* key, int key_len);

    ValueType* search(const char* key, int key_len);

    int iterateOverAll(art_callback cb, void* data);

    int iterateOverPrefix(const char* prefix, int prefix_len, art_callback cb, void* data);

    ValueType* getMinimumValue();

    ValueType* getMaximumValue();

private:
    art_tree tree{};
};

template<typename ValueType>
AdaptiveRadixTree<ValueType>::AdaptiveRadixTree()
{
    assert(art_tree_init(&tree) == 0);
}

template<typename ValueType>
AdaptiveRadixTree<ValueType>::~AdaptiveRadixTree()
{
    assert(art_tree_destroy(&tree) == 0);
}

template<typename ValueType>
uint64_t AdaptiveRadixTree<ValueType>::size()
{
    return art_size(&tree);
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::insert(const char* key, int key_len, ValueType* value)
{
    return static_cast<ValueType*>(art_insert(&tree, reinterpret_cast<const unsigned char*>(key), key_len, value));
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::insertIfNotExists(const char* key, int key_len, ValueType* value)
{
    return static_cast<ValueType*>(art_insert_no_replace(&tree, reinterpret_cast<const unsigned char*>(key), key_len, value));
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::deleteValue(const char* key, int key_len)
{
    return static_cast<ValueType*>(art_delete(&tree, reinterpret_cast<const unsigned char*>(key), key_len));
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::search(const char* key, int key_len)
{
    return static_cast<ValueType*>(art_search(&tree, reinterpret_cast<const unsigned char*>(key), key_len));
}

template<typename ValueType>
int AdaptiveRadixTree<ValueType>::iterateOverAll(art_callback cb, void* callbackData)
{
    return art_iter(&tree, cb, callbackData);
}

template<typename ValueType>
int AdaptiveRadixTree<ValueType>::iterateOverPrefix(const char* prefix, int prefix_len, art_callback cb, void* callbackData)
{
    return art_iter_prefix(&tree, reinterpret_cast<const unsigned char*>(prefix), prefix_len, cb, callbackData);
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::getMinimumValue()
{
    art_leaf* leaf = art_minimum(&tree);
    return static_cast<ValueType*>(leaf ? leaf->value : nullptr);
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::getMaximumValue()
{
    art_leaf* leaf = art_maximum(&tree);
    return static_cast<ValueType*>(leaf ? leaf->value : nullptr);
}

#endif // LIBARTCPP_H
