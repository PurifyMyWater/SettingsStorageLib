#ifndef SETTINGSSTORAGE_SETTINGS_H
#define SETTINGSSTORAGE_SETTINGS_H

#define CRCPP_USE_CPP11

#include <string>
#include "AtomicLibARTCpp.h"
#include "CRC.h"
#include "OSInterface.h"
#include "SettingsFile.h"
#include "list"

#ifndef CONFIG_SETTINGS_STORAGE_FORCE_DISABLE_PERSISTENT_STORAGE
    #define CONFIG_SETTINGS_STORAGE_FORCE_DISABLE_PERSISTENT_STORAGE false
#endif

constexpr size_t PERMISSION_STRING_SIZE = 34;
constexpr size_t MAX_SETTING_KEY_SIZE   = 128;

/**
 * @brief The permissions that can be granted to a setting.
 *
 * The permissions are stored in a bitfield, where each bit represents a permission.
 * If a bit is set in a flag position, it means that the permission is granted, and if the bit is cleared, the
 * permission is not granted. The permissions are: — SYSTEM: Used for settings that are critical for the system to work.
 * They should not be changed by the end user or admin. — ADMIN: Used for settings that are critical for the system to
 * work, but may need human configuration. They should not be changed by the end user. — USER: Used for settings that
 * are not critical for the system to work. They can be changed by the end user.
 */
enum class SettingPermissions_t : uint8_t
{
    SYSTEM   = 1,
    ADMIN    = 2,
    USER     = 4,
    VOLATILE = 8
}; // Flags enabled enum that stores each setting visibility permissions.

/// Enum that stores the filter modes for the permissions.
enum SettingPermissionsFilterMode_t
{
    MatchSettingsWithAllPermissionsListed = 0,
    MatchSettingsWithAnyPermissionsListed,
    ExcludeSettingsWithAllPermissionsListed,
    ExcludeSettingsWithAnyPermissionsListed
};

/// This operator overload allows the enum SettingPermissions_t to have a bitwise OR operator.
SettingPermissions_t operator|(SettingPermissions_t lhs, SettingPermissions_t rhs);

/// This operator overload allows the enum SettingPermissions_t to have a bitwise AND operator.
SettingPermissions_t operator&(SettingPermissions_t lhs, SettingPermissions_t rhs);

/// All permissions are granted to a setting.
const SettingPermissions_t ALL_PERMISSIONS =
    SettingPermissions_t::USER | SettingPermissions_t::ADMIN | SettingPermissions_t::SYSTEM;

/// All permissions are granted to a setting, and it is marked as volatile.
const SettingPermissions_t ALL_PERMISSIONS_VOLATILE = SettingPermissions_t::USER | SettingPermissions_t::ADMIN |
                                                      SettingPermissions_t::SYSTEM | SettingPermissions_t::VOLATILE;

/// No permissions are granted to a setting.
constexpr SettingPermissions_t NO_PERMISSIONS = static_cast<SettingPermissions_t>(0);

/// This function returns a formatted string of the permissions described in the parameter permission
const char* settingPermissionToString(SettingPermissions_t permission, char* permissionString,
                                      size_t permissionStringSize);

/// This function validates the permissions.
bool validatePermissions(SettingPermissions_t permissions);

class SettingsStorage
{
public:
    /// List of keys that match the provided key prefix.
    using SettingsKeysList_t = std::list<std::string>;

    /// Enum that stores the possible errors returned by the SettingsStorage API.
    typedef enum
    {
        NO_ERROR = 0,
        FATAL_ERROR,
        KEY_NOT_FOUND_ERROR,
        TYPE_MISMATCH_ERROR,
        KEY_EXISTS_ERROR,
        SETTINGS_FILESYSTEM_ERROR,
        INVALID_INPUT_ERROR,
        INSUFFICIENT_BUFFER_SIZE_ERROR
    } SettingError_t;

    /// Enum with the types of data that can be saved.
    typedef enum
    {
        REAL,
        INTEGER,
        STRING,
        MAX_SETTING_VALUE_TYPE_ENUM
    } SettingValueType_t;

    /// Union with the types of data that can be saved.
    typedef union
    {
        double  real;
        int64_t integer;
        char*   string;
    } SettingValueData_t;

    /// The value of each setting element.
    typedef struct SettingValue_t
    {
        SettingValueType_t   settingValueType;
        SettingValueData_t   settingValueData;
        SettingValueData_t   settingDefaultValueData;
        SettingPermissions_t settingPermissions;
    } SettingValue_t;

    /// String with the name of the component.
    constexpr static const char* const COMPONENT_TAG = "PurifyMyWater - SettingsStorage";

    /// The data structure used internally to store the settings.
    typedef AtomicAdaptiveRadixTree<SettingValue_t> Settings_t;

    /**
     * @brief Build a new empty Settings Storage object.
     *
     * @param osInterface The OS shim object that will be used to interact with the OS.
     * @param settingsFile The settings file object that will be used to interact with the settings file.
     * If it is nullptr, the settings will not be saved in the persistent storage.
     */
    explicit SettingsStorage(OSInterface& osInterface, SettingsFile* settingsFile = nullptr);

    /**
     * @brief Destroy the Settings Storage object and free all the associated memory.
     */
    ~SettingsStorage();

    /**
     * @brief Check if the settings can be saved in the persistent storage.
     * @return True if the settings can be saved in the persistent storage, false otherwise.
     */
    [[nodiscard]] bool isPersistentStorageEnabled() const;

    /**
     * @brief Disable the persistent storage for the settings, stopping the settings from being saved in the persistent
     storage.
     * @return True if the persistent storage was disabled,
     false if an error ocurred during persistent storage disable action.
     Don't assume the persistent storage is enabled nor disabled if this function returns false.
     Use isPersistentStorageEnabled() to check the persistent storage status.
     */
    [[nodiscard]] bool disablePersistentStorage();

    /**
     * @brief Restores the default settings of the settings that match the provided keyPrefix, or all settings if
     * componentName is "".
     *
     * @param keyPrefix The name of the component to restore settings for.
     * @param permissions The permissions filter to apply to the settings restore process.
     * @param filterMode The filter mode to apply to the permissions.
     * @return SettingError_t The result of the restore operation.
     * @retval NO_ERROR The settings were successfully restored.
     * @retval INVALID_INPUT_ERROR If keyPrefix is nullptr.
     * @retval INVALID_INPUT_ERROR The permissions are invalid.
     * @retval INVALID_INPUT_ERROR The filterMode is invalid.
     */
    [[nodiscard]] SettingError_t
    restoreDefaultSettings(const char* keyPrefix, SettingPermissions_t permissions = ALL_PERMISSIONS,
                           SettingPermissionsFilterMode_t filterMode = MatchSettingsWithAnyPermissionsListed) const;

    /**
     * @brief This function saves the settings to the persistent storage, replacing the old copy of them.
     *
     * @note If there are settings in the settingsStorage that are marked as volatile,
     * they will not be saved in the persistent storage.
     *
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The settings were successfully saved.
     * @retval SETTINGS_FILESYSTEM_ERROR The settings filesystem is corrupted, and the settings were not saved.
     * @retval SETTINGS_FILESYSTEM_ERROR The persisten storage is disabled, and the settings were not saved.
     */
    [[nodiscard]] SettingError_t storeSettingsInPersistentStorage() const;

    /**
     * @brief This function loads the settings from the persistent storage, replacing the old copy of them.
     *
     * @note If there are settings in the settingsFile that are not registered by the components,
     * they will be loaded but marked as volatile,
     * which means they will not be saved in the persistent storage.
     *
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The settings were successfully loaded.
     * @retval SETTINGS_FILESYSTEM_ERROR The settings file is corrupted and settings were not modified.
     */
    [[nodiscard]] SettingError_t loadSettingsFromPersistentStorage() const;

    /**
     * @brief This lists the settings keys that match the provided key prefix.
     * @param keyPrefix The prefix of the keys to list. An empty string will list all keys.
     * @param permissions The permissions filter to apply to the keys.
     * @param filterMode The filter mode to apply to the permissions.
     * @param outputKeys The list of keys that match the provided key prefix ordered in lexical order.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The settings were successfully listed.
     * @retval INVALID_INPUT_ERROR The keyPrefix is nullptr.
     * @retval INVALID_INPUT_ERROR The permissions are invalid.
     * @retval INVALID_INPUT_ERROR The filterMode is invalid.
     */
    [[nodiscard]] SettingError_t listSettingsKeys(const char* keyPrefix, SettingPermissions_t permissions,
                                                  SettingPermissionsFilterMode_t filterMode,
                                                  SettingsKeysList_t&            outputKeys) const;

    /**
     * @brief This function returns the value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValue The value of the setting.
     * @param outputPermissions Optional output parameter to store the permissions of the setting. If it is nullptr, the
     * permissions are not returned.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     */
    [[nodiscard]] SettingError_t getSettingAsInt(const char* key, int64_t& outputValue,
                                                 SettingPermissions_t* outputPermissions = nullptr) const;

    /**
     * @brief This function returns the value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValue The value of the setting.
     * @param outputPermissions Optional output parameter to store the permissions of the setting. If it is nullptr, the
     * permissions are not returned.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     */
    [[nodiscard]] SettingError_t getSettingAsReal(const char* key, double& outputValue,
                                                  SettingPermissions_t* outputPermissions = nullptr) const;

    /**
     * @brief This function returns the value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValueBuffer The value of the setting. Must be a buffer with enough space to store the value.
     * @param outputValueSize The size of the outputValueBuffer.
     * @param outputPermissions Optional output parameter to store the permissions of the setting. If it is nullptr, the
     * permissions are not returned.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval INVALID_INPUT_ERROR The outputValueBuffer is nullptr.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     * @retval INSUFFICIENT_BUFFER_SIZE_ERROR The outputValueBuffer is null or not big enough to store the value.
     */
    [[nodiscard]] SettingError_t getSettingAsString(const char* key, char* outputValueBuffer, size_t outputValueSize,
                                                    SettingPermissions_t* outputPermissions = nullptr) const;

    /**
     * @brief This function creates an empty setting located at the specified path, with the provided permissions.
     * @param key The key of the setting to create. It must not contain the tab (\t) character.
     * @param permissions The set of permissions associated with the setting.
     * @param defaultValue The default value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully created.
     * @retval KEY_EXISTS_ERROR The setting with the provided key already exists.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval INVALID_INPUT_ERROR The permissions are invalid.
     */
    [[nodiscard]] SettingError_t registerSettingAsInt(const char* key, SettingPermissions_t permissions,
                                                      int64_t defaultValue) const;

    /**
     * @brief This function creates an empty setting located at the specified path, with the provided permissions.
     * @param key The key of the setting to create. It must not contain the tab (\t) character.
     * @param permissions The set of permissions associated with the setting.
     * @param defaultValue The default value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully created.
     * @retval KEY_EXISTS_ERROR The setting with the provided key already exists.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval INVALID_INPUT_ERROR The permissions are invalid.
     */
    [[nodiscard]] SettingError_t registerSettingAsReal(const char* key, SettingPermissions_t permissions,
                                                       double defaultValue) const;

    /**
     * @brief This function creates an empty setting located at the specified path, with the provided permissions.
     * @param key The key of the setting to create. It must not contain the tab (\t) character.
     * @param permissions The set of permissions associated with the setting.
     * @param defaultValue The default value of the setting. It will be copied to SettingsStorage memory.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully created.
     * @retval KEY_EXISTS_ERROR The setting with the provided key already exists.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval INVALID_INPUT_ERROR The permissions are invalid.
     * @retval INVALID_INPUT_ERROR The defaultValue is nullptr.
     */
    [[nodiscard]] SettingError_t registerSettingAsString(const char* key, SettingPermissions_t permissions,
                                                         const char* defaultValue) const;

    /**
     * @brief This function updates the value of the setting with the provided key.
     * @param key The key of the setting to update.
     * @param value The new value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully updated.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type or void.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     */
    [[nodiscard]] SettingError_t putSettingValueAsInt(const char* key, int64_t value) const;

    /**
     * @brief This function updates the value of the setting with the provided key.
     * @param key The key of the setting to update.
     * @param value The new value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully updated.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type or void.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     */
    [[nodiscard]] SettingError_t putSettingValueAsReal(const char* key, double value) const;

    /**
     * @brief This function updates the value of the setting with the provided key.
     * @param key The key of the setting to update.
     * @param value The new value of the setting. It must not contain the tab (\t) character.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully updated.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type or void.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval INVALID_INPUT_ERROR The value is nullptr.
     */
    [[nodiscard]] SettingError_t putSettingValueAsString(const char* key, const char* value) const;

    /**
     * @brief This function returns the default value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValue The default value of the setting.
     * @param outputPermissions Optional output parameter to store the permissions of the setting. If it is nullptr, the
     * permissions are not returned.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     */
    [[nodiscard]] SettingError_t getDefaultSettingAsInt(const char* key, int64_t& outputValue,
                                                        SettingPermissions_t* outputPermissions = nullptr) const;

    /**
     * @brief This function returns the default value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValue The default value of the setting.
     * @param outputPermissions Optional output parameter to store the permissions of the setting. If it is nullptr, the
     * permissions are not returned.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     */
    [[nodiscard]] SettingError_t getDefaultSettingAsReal(const char* key, double& outputValue,
                                                         SettingPermissions_t* outputPermissions = nullptr) const;

    /**
     * @brief This function returns the default value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValueBuffer The default value of the setting. Must be a buffer with enough space to store the value.
     * @param outputValueSize The size of the outputValueBuffer.
     * @param outputPermissions Optional output parameter to store the permissions of the setting. If it is nullptr, the
     * permissions are not returned.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval INVALID_INPUT_ERROR The outputValueBuffer is nullptr.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     * @retval INSUFFICIENT_BUFFER_SIZE_ERROR The outputValueBuffer is null or not big enough to store the value.
     */
    [[nodiscard]] SettingError_t getDefaultSettingAsString(const char* key, char* outputValueBuffer,
                                                           size_t                outputValueSize,
                                                           SettingPermissions_t* outputPermissions = nullptr) const;

    /**
     * Disallow copying or moving the object.
     */
    SettingsStorage& operator=(SettingsStorage&&) = delete;

private:
    typedef std::tuple<SettingPermissions_t, SettingPermissionsFilterMode_t, SettingsKeysList_t*>
                                                                                   SettingsListCallbackData_t;
    typedef std::tuple<SettingsFile*, uint32_t*, bool*, CRC::Table<unsigned, 32>*> SettingsStoreCallbackData_t;
    using TypeofSettingValue = enum { Value, DefaultValue };

    OSInterface_Mutex* moduleConfigMutex;
    SettingsFile*      settingsFile;
    bool               persistentStorageEnabled;
    Settings_t*        settings;
    OSInterface*       osInterface;

    static int listSettingsKeysCallback(void* data, const unsigned char* key, uint32_t key_len, void* value);
    static int freeSettingValuesCallback(void* data, const unsigned char* key, uint32_t key_len, void* value);
    static int storeSettingsInPersistentStorageCallback(void* data, const unsigned char* key, uint32_t key_len,
                                                        void* value);
    [[nodiscard]] SettingError_t validateChecksum() const;

    SettingError_t               getSettingValue(const char* key, SettingValue_t*& outputValue) const;
    [[nodiscard]] SettingError_t getSettingValueAsInt(TypeofSettingValue type, const char* key, int64_t& outputValue,
                                                      SettingPermissions_t* outputPermissions = nullptr) const;
    [[nodiscard]] SettingError_t getSettingValueAsReal(TypeofSettingValue type, const char* key, double& outputValue,
                                                       SettingPermissions_t* outputPermissions = nullptr) const;
    [[nodiscard]] SettingError_t getSettingValueAsString(TypeofSettingValue type, const char* key,
                                                         char* outputValueBuffer, size_t outputValueSize,
                                                         SettingPermissions_t* outputPermissions = nullptr) const;

    static void freeSettingValue(const SettingValue_t* settingValue);
};

#endif // SETTINGSSTORAGE_SETTINGS_H
