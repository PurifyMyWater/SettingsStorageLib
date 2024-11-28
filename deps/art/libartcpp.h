#ifndef LIBARTCPP_H
#define LIBARTCPP_H

#include "art.h"

/**
 * @brief A C++ wrapper for the ART library
 * It can store pointers to types of template typename ValueType
 * The user is responsable for the memory management of the values stored in the tree
 */
template <typename ValueType> class AdaptiveRadixTree
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
     * @brief Get the size of the tree
     *
     * @return uint64_t size
     */
    uint64_t size();

    /**
     * @brief Insert a new value into the art tree
     *
     * @param key the key
     * @param key_len the length of the key
     * @param value opaque value.
     * @return null if the item was newly inserted, otherwise
     * the old value pointer is returned.
     */
    ValueType* insert(const char* key, int key_len, ValueType* value);
    
    /**
     * @brief Insert a new value into the art tree (no replace)
     *
     * @param key the key
     * @param key_len the length of the key
     * @param value opaque value.
     * @return null if the item was newly inserted, otherwise
     * the old value pointer is returned.
     */
    ValueType* insertIfNotExists(const char* key, int key_len, ValueType* value);
    
    /**
     * @brief Searches for a value in the ART tree
     *
     * @param key The key
     * @param key_len The length of the key
     * @return NULL if the item was not found, otherwise
     * the value pointer is returned.
     */
    ValueType* deleteValue(const char* key, int key_len);

    /**
     * @brief Searches for a value in the ART tree
     *
     * @param key The key
     * @param key_len The length of the key
     * @return NULL if the item was not found, otherwise
     * the value pointer is returned.
     */
    ValueType* search(const char* key, int key_len);

    /**
     * Iterates through the entries pairs in the map,
     * invoking a callback for each. The call back gets a
     * key, value for each and returns an integer stop value.
     * If the callback returns non-zero, then the iteration stops.
     * @param cb The callback function to invoke
     * @param data Opaque handle passed to the callback
     * @return 0 on success, or the return of the callback.
     */
    int iterateOverAll(art_callback cb, void *data);

    /**
     * Iterates through the entries pairs in the map,
     * invoking a callback for each that matches a given prefix.
     * The call back gets a key, value for each and returns an integer stop value.
     * If the callback returns non-zero, then the iteration stops.
     * @param prefix The prefix of keys to read
     * @param prefix_len The length of the prefix
     * @param cb The callback function to invoke
     * @param data Opaque handle passed to the callback
     * @return 0 on success, or the return of the callback.
     */
    int iterateOverPrefix(const char* prefix, int prefix_len, art_callback cb, void *data);

    /**
     * @brief Returns the minimum valued leaf value in the tree
     *
     * @return The minimum leaf value or NULL
     */
    ValueType* getMinimumValue();

    /**
     * @brief Returns the maximum valued leaf value in the tree
     *
     * @return The maximum leaf value or NULL
     */
    ValueType* getMaximumValue();
    

private:
    art_tree tree{};
};


#include <cassert>

template <typename ValueType>
AdaptiveRadixTree<ValueType>::AdaptiveRadixTree()
{
  assert(art_tree_init(&tree) == 0);
}

template <typename ValueType>
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
  return reinterpret_cast<ValueType*>(art_insert(&tree, reinterpret_cast<const unsigned char *>(key), key_len, value));
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::insertIfNotExists(const char* key, int key_len, ValueType* value)
{
  return reinterpret_cast<ValueType*>(art_insert_no_replace(&tree, reinterpret_cast<const unsigned char *>(key), key_len, value));
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::deleteValue(const char* key, int key_len)
{
  return reinterpret_cast<ValueType*>(art_delete(&tree, reinterpret_cast<const unsigned char *>(key), key_len));
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::search(const char* key, int key_len)
{
  return reinterpret_cast<ValueType*>(art_search(&tree, reinterpret_cast<const unsigned char *>(key), key_len));
}

template<typename ValueType>
int AdaptiveRadixTree<ValueType>::iterateOverAll(art_callback cb, void* data)
{
  return art_iter(&tree, cb, data);
}

template<typename ValueType>
int AdaptiveRadixTree<ValueType>::iterateOverPrefix(const char* prefix, int prefix_len, art_callback cb, void* data)
{
  return art_iter_prefix(&tree, reinterpret_cast<const unsigned char *>(prefix), prefix_len, cb, data);
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::getMinimumValue()
{
  art_leaf* leaf = art_minimum(&tree);
  return reinterpret_cast<ValueType*>(leaf ? leaf->value : nullptr);
}

template<typename ValueType>
ValueType* AdaptiveRadixTree<ValueType>::getMaximumValue()
{
  art_leaf* leaf = art_maximum(&tree);
  return reinterpret_cast<ValueType*>(leaf ? leaf->value : nullptr);
}

#endif //LIBARTCPP_H
