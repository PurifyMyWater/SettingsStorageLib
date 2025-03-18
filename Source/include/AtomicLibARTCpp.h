#ifndef ATOMICLIBARTCPP_H
#define ATOMICLIBARTCPP_H

#include "OSInterface.h"
#include "libartcpp.h"

extern constexpr uint32_t SETTINGS_STORAGE_MUTEX_TIMEOUT_MS; // Defined in SettingsStorage.cpp

/**
 * @brief A C++ wrapper for the ART library with atomic operations
 * It can store pointers to types of template typename ValueType
 * The user is responsable for the memory management of the values stored in the tree.
 */
template <typename ValueType> class AtomicAdaptiveRadixTree : public AdaptiveRadixTree<ValueType>
{
public:
    /**
     * @brief Construct a new Adaptive Radix Tree object
     */
    explicit AtomicAdaptiveRadixTree(OSInterface& osInterface);

    /**
     * @brief Destroy the Adaptive Radix Tree object
     */
    ~AtomicAdaptiveRadixTree();

    /**
     * Disallow copying or moving the object.
     */
    AtomicAdaptiveRadixTree& operator=(AtomicAdaptiveRadixTree&&) = delete;

    /**
     * @brief Get the size of the tree
     *
     * @return uint64_t size
     */
    uint64_t size() override;

    /**
     * @brief Insert a new value into the art tree
     *
     * @param key The key
     * @param key_len The length of the key
     * @param value opaque value.
     * @return Null if the item was newly inserted, otherwise
     * the old value pointer is returned.
     */
    ValueType* insert(const char* key, int key_len, ValueType* value) override;

    /**
     * @brief Insert a new value into the art tree (no replace)
     *
     * @param key The key
     * @param key_len The length of the key
     * @param value opaque value.
     * @return Null if the item was newly inserted, otherwise
     * the old value pointer is returned.
     */
    ValueType* insertIfNotExists(const char* key, int key_len, ValueType* value) override;

    /**
     * @brief Searches for a value in the ART tree
     *
     * @param key The key
     * @param key_len The length of the key
     * @return NULL if the item was not found, otherwise
     * the value pointer is returned.
     */
    ValueType* deleteValue(const char* key, int key_len) override;

    /**
     * @brief Searches for a value in the ART tree
     *
     * @param key The key
     * @param key_len The length of the key
     * @return NULL if the item was not found, otherwise
     * the value pointer is returned.
     */
    ValueType* search(const char* key, int key_len) override;

    /**
     * Iterates through the entries pairs in the map,
     * invoking a callback for each.
     * The callback gets a key value for each and returns an integer stop value.
     * If the callback returns non-zero, then the iteration stops.
     * @param cb The callback function to invoke
     * @param data Opaque handle passed to the callback
     * @return Zero on success, or the return of the callback.
     */
    int iterateOverAll(art_callback cb, void* data) override;

    /**
     * Iterates through the entry pairs in the map,
     * invoking a callback for each that matches a given prefix.
     * The callback gets a key value for each and returns an integer stop value.
     * If the callback returns non-zero, then the iteration stops.
     * @param prefix The prefix of keys to read
     * @param prefix_len The length of the prefix
     * @param cb The callback function to invoke
     * @param data Opaque handle passed to the callback
     * @return Zero on success, or the return of the callback.
     */
    int iterateOverPrefix(const char* prefix, int prefix_len, art_callback cb, void* data) override;

    /**
     * @brief Returns the minimum valued leaf value in the tree
     *
     * @return The minimum leaf value or NULL
     */
    ValueType* getMinimumValue() override;

    /**
     * @brief Returns the maximum valued leaf value in the tree
     *
     * @return The maximum leaf value or NULL
     */
    ValueType* getMaximumValue() override;

private:
    [[nodiscard]] bool           preWrite() const;
    void                         postWrite() const;
    [[nodiscard]] bool           preRead();
    void                         postRead();
    OSInterface*                 osInterface;
    OSInterface_BinarySemaphore* empty;
    OSInterface_BinarySemaphore* turn;
    OSInterface_Mutex*           readersMutex;
    uint32_t                     readers;
};

template <typename ValueType> AtomicAdaptiveRadixTree<ValueType>::AtomicAdaptiveRadixTree(OSInterface& osInterface) :
    AdaptiveRadixTree<ValueType>()
{
    this->readers     = 0;
    this->osInterface = &osInterface;
    this->empty       = osInterface.osCreateBinarySemaphore();
    assert(this->empty != nullptr && "Semaphore creation failed");
    this->turn = osInterface.osCreateBinarySemaphore();
    assert(this->turn != nullptr && "Semaphore creation failed");
    this->readersMutex = osInterface.osCreateMutex();
    assert(this->readersMutex != nullptr && "Mutex creation failed");

    // The semaphores are initialized to 0, so a signal is needed to allow the first reader/writer to access the tree.
    this->empty->signal();
    this->turn->signal();
}

template <typename ValueType> AtomicAdaptiveRadixTree<ValueType>::~AtomicAdaptiveRadixTree()
{
    delete this->empty;
    delete this->turn;
    delete this->readersMutex;
}

template <typename ValueType> uint64_t AtomicAdaptiveRadixTree<ValueType>::size()
{
    if (preRead())
    {
        const uint64_t size = AdaptiveRadixTree<ValueType>::size();
        postRead();
        return size;
    }
    return 0;
}

template <typename ValueType>
ValueType* AtomicAdaptiveRadixTree<ValueType>::insert(const char* key, int key_len, ValueType* value)
{
    if (preWrite())
    {
        ValueType* result = AdaptiveRadixTree<ValueType>::insert(key, key_len, value);
        postWrite();
        return result;
    }
    return nullptr;
}

template <typename ValueType>
ValueType* AtomicAdaptiveRadixTree<ValueType>::insertIfNotExists(const char* key, int key_len, ValueType* value)
{
    if (preWrite())
    {
        ValueType* result = AdaptiveRadixTree<ValueType>::insertIfNotExists(key, key_len, value);
        postWrite();
        return result;
    }
    return nullptr;
}

template <typename ValueType> ValueType* AtomicAdaptiveRadixTree<ValueType>::deleteValue(const char* key, int key_len)
{
    if (preWrite())
    {
        ValueType* result = AdaptiveRadixTree<ValueType>::deleteValue(key, key_len);
        postWrite();
        return result;
    }
    return nullptr;
}

template <typename ValueType> ValueType* AtomicAdaptiveRadixTree<ValueType>::search(const char* key, int key_len)
{
    if (preRead())
    {
        ValueType* result = AdaptiveRadixTree<ValueType>::search(key, key_len);
        postRead();
        return result;
    }
    return nullptr;
}

template <typename ValueType> int AtomicAdaptiveRadixTree<ValueType>::iterateOverAll(art_callback cb, void* data)
{
    if (preRead())
    {
        const int result = AdaptiveRadixTree<ValueType>::iterateOverAll(cb, data);
        postRead();
        return result;
    }
    return -1;
}

template <typename ValueType> int
AtomicAdaptiveRadixTree<ValueType>::iterateOverPrefix(const char* prefix, int prefix_len, art_callback cb, void* data)
{
    if (preRead())
    {
        const int result = AdaptiveRadixTree<ValueType>::iterateOverPrefix(prefix, prefix_len, cb, data);
        postRead();
        return result;
    }
    return -1;
}

template <typename ValueType> ValueType* AtomicAdaptiveRadixTree<ValueType>::getMinimumValue()
{
    if (preRead())
    {
        ValueType* result = AdaptiveRadixTree<ValueType>::getMinimumValue();
        postRead();
        return result;
    }
    return nullptr;
}

template <typename ValueType> ValueType* AtomicAdaptiveRadixTree<ValueType>::getMaximumValue()
{
    if (preRead())
    {
        ValueType* result = AdaptiveRadixTree<ValueType>::getMaximumValue();
        postRead();
        return result;
    }
    return nullptr;
}

template <typename ValueType> bool AtomicAdaptiveRadixTree<ValueType>::preWrite() const
{
    if (!turn->wait(SETTINGS_STORAGE_MUTEX_TIMEOUT_MS))
    {
        // ReSharper disable once CppDFAUnreachableCode False positive
        return false;
    }
    // ReSharper disable once CppDFAUnreachableCode False positive
    return empty->wait(SETTINGS_STORAGE_MUTEX_TIMEOUT_MS);
}

template <typename ValueType> void AtomicAdaptiveRadixTree<ValueType>::postWrite() const
{
    turn->signal();
    empty->signal();
}

template <typename ValueType> bool AtomicAdaptiveRadixTree<ValueType>::preRead()
{
    if (!turn->wait(SETTINGS_STORAGE_MUTEX_TIMEOUT_MS))
    {
        // ReSharper disable once CppDFAUnreachableCode False positive
        return false;
    }
    // ReSharper disable once CppDFAUnreachableCode False positive
    turn->signal();
    if (!readersMutex->wait(SETTINGS_STORAGE_MUTEX_TIMEOUT_MS))
    {
        return false;
    }
    readers++;
    if (readers == 1)
    {
        if (!empty->wait(SETTINGS_STORAGE_MUTEX_TIMEOUT_MS))
        {
            return false;
        }
    }
    readersMutex->signal();
    return true;
}

template <typename ValueType> void AtomicAdaptiveRadixTree<ValueType>::postRead()
{
    if (!readersMutex->wait(SETTINGS_STORAGE_MUTEX_TIMEOUT_MS))
    {
        assert(false && "readersMutex wait failed in postRead");
    }
    // ReSharper disable once CppDFAUnreachableCode False positive
    readers--;
    if (readers == 0)
    {
        empty->signal();
    }
    readersMutex->signal();
}

#endif // ATOMICLIBARTCPP_H
