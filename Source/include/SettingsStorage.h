#ifndef SETTINGSSTORAGE_SETTINGS_H
#define SETTINGSSTORAGE_SETTINGS_H

#include <string>
#include "OSShim.h"
#include "forward_list"
#include "libartcpp.h"

#define SETTINGS_STORAGE_MUTEX_TIMEOUT_MS 100

class SettingsParser;

constexpr size_t PERMISSION_STRING_SIZE = 22;

/**
 * @brief The permissions that can be granted to a setting.
 *
 *
 * The permissions are stored in a bitfield, where each bit represents a permission.
 * If a bit is set in a flag position, it means that the permission is granted, and if the bit is cleared, the permission is not granted.
 * The permissions are:
 * - SYSTEM: Used for settings that are critical for the system to work. They should not be changed by the end user or admin.
 * - ADMIN: Used for settings that are critical for the system to work, but may need human configuration. They should not be changed by the end user.
 * - USER: Used for settings that are not critical for the system to work. They can be changed by the end user.
 */
enum class SettingPermissions_t : uint8_t
{
    SYSTEM = 1,
    ADMIN = 2,
    USER = 4
}; // Flags enabled enum that stores each setting visibility permissions.

/// Enum that stores the filter modes for the permissions.
enum SettingPermissionsFilterMode_t
{
    MatchAll = 0,
    MatchAny,
    ExcludeAll,
    ExcludeAny
};

/// This operator overload allows the enum SettingPermissions_t to have a bitwise OR operator.
SettingPermissions_t operator|(SettingPermissions_t lhs, SettingPermissions_t rhs);

/// This operator overload allows the enum SettingPermissions_t to have a bitwise AND operator.
SettingPermissions_t operator&(SettingPermissions_t lhs, SettingPermissions_t rhs);

/// This function returns a formatted string of the permissions described in the parameter permission
const char* settingPermissionToString(SettingPermissions_t permission, char* permissionString, size_t permissionStringSize);

class SettingsStorage
{
public:
    /// Enum that stores the possible errors returned by the SettingsStorage API.
    typedef enum
    {
        NO_ERROR = 0,
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
        VOID = 0,
        REAL,
        INTEGER,
        STRING
    } SettingValueType_t;

    /// Union with the types of data that can be saved.
    typedef union
    {
        double real;
        int64_t integer;
        const char* string;
    } SettingValueData_t;

    /// The value of each setting element.
    typedef struct SettingValue_t
    {
        SettingValueType_t settingValueType;
        SettingValueData_t settingValueData;
        SettingPermissions_t settingPermissions;
    } SettingValue_t;

    /// String with the name of the component.
    constexpr static const char* const COMPONENT_TAG = "PurifyMyWater - SettingsStorage";

    /// The representation of an empty element.
    static constexpr SettingValue_t VOID_VALUE = {.settingValueType = VOID, .settingValueData = {0}, .settingPermissions = SettingPermissions_t::SYSTEM};

    /**
     * The function data type that the settings restore functionality uses.
     * All modules that use settings must register a function of this type where they restore the default settings for the module using addSettingKey(...) and putSettingValue(...).
     * It is guaranteed that the structure of the settings map paths used in the restore process do not exist prior to the call to this function.
     */
    typedef void (*RestoreComponentDefaultSettingsCallback_t)();

    /// The value of each component element stored in the component map. It is used to provide the information of the component in the settings restore process.
    typedef struct ComponentInfo_t
    {
        std::string componentName;
        std::forward_list<std::string> componentTopLevelPathsList;
        RestoreComponentDefaultSettingsCallback_t restoreComponentDefaultSettingsCallback;
    } ComponentInfo_t;

    /// The data structure used internally to store the settings.
    typedef AdaptiveRadixTree<SettingValue_t> Settings_t;

    /**
     * @brief Build a new Settings Storage object
     *
     * It will create a new settings storage object and synchronize it with the persistent storage settings file.
     *
     * @param pathToSettingsFile The path to the settings file synchronized with the settings object.
     * @param result The result of the operation
     * @param osShim The OS shim object used to interact with the OS.
     * @param settingsParser The settings parser object used to read and write the settings to the persistent storage. If it is nullptr, the default settings parser will be used.
     *
     * @retval NO_ERROR The settings were successfully loaded.
     * @retval SETTINGS_FILESYSTEM_ERROR The settings file is corrupted or doesn't exist, and default settings were loaded instead of saved ones.
     * @retval INVALID_INPUT_ERROR The pathToSettingsFile is nullptr or "" and default settings were loaded instead of saved ones, and the persistent storage was disabled.
     */
    SettingsStorage(const char* pathToSettingsFile, SettingError_t* result, OSShim& osShim, SettingsParser* settingsParser = nullptr);

    /**
     * @brief Destroy the Settings Storage object and free all the associated memory
     */
    ~SettingsStorage();

    /**
     * @brief Check if the settings can be saved in the persistent storage.
     * @return True if the settings can be saved in the persistent storage, false otherwise.
     */
    [[nodiscard]] bool isPersistentStorageEnabled() const;

    /**
     * @brief Disable the persistent storage for the settings, stopping the settings from being saved in the persistent storage.
     * @return True if the persistent storage was disabled,
     false if an error ocurred during persistent storage disable action.
     Don't assume the persistent storage is enabled nor disabled if this function returns false.
     Use isPersistentStorageEnabled() to check the persistent storage status.
     */
    bool disablePersistentStorage();

    /**
     * @brief Registers a component with the provided component information.
     *
     * @param componentInfo The information of the component to register.
     * @return SettingError_t The result of the registration operation.
     * @retval NO_ERROR The component was successfully registered.
     * @retval INVALID_INPUT_ERROR If componentName is nullptr or "".
     * @retval COMPONENT_ALREADY_REGISTERED_ERROR If componentName is already registered.
     * @retval INVALID_INPUT_ERROR If componentInfo contains invalid information (an empty componentTopLevelPathsList or a null restoreComponentDefaultSettingsCallback).
     */
    SettingError_t registerComponent(ComponentInfo_t& componentInfo);

    /**
     * @brief Get a list of all the components registered in the settings storage component.
     * @return a list of all the components registered in the settings storage component.
     */
    std::forward_list<ComponentInfo_t> listRegisteredComponents();

    /**
     * @brief Restores the default settings of the provided component, or all settings if componentName is "".
     *
     * @param componentName The name of the component to restore settings for.
     * @return SettingError_t The result of the restore operation.
     * @retval NO_ERROR The settings were successfully restored.
     * @retval INVALID_INPUT_ERROR If componentName is nullptr.
     * @retval KEY_NOT_FOUND_ERROR If componentName is not previously registered using registerComponent().
     *
     * @note This function will call the restoreComponentDefaultSettingsCallback of the component to restore the default settings.
     */
    SettingError_t restoreComponentDefaultSettings(const char* componentName);

    /**
     * @brief This function saves the settings to the persistent storage, replacing the old copy ot them.
     *
     * If delayed write is enabled, it will also cancel the timer if it is active.
     * If the persisten storage is disabled, it will do nothing and return SETTINGS_FILESYSTEM_ERROR.
     *
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The settings were successfully saved.
     * @retval SETTINGS_FILESYSTEM_ERROR The settings filesystem is corrupted and the settings were not saved.
     * @retval SETTINGS_FILESYSTEM_ERROR The persisten storage is disabled and the settings were not saved.
     */
    SettingError_t storeSettingsInPersistentStorage();

    /**
     * @brief This function loads the settings from the persistent storage, replacing the old copy ot them.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The settings were successfully loaded.
     * @retval SETTINGS_FILESYSTEM_ERROR The settings file is corrupted and settings were not modified.
     */
    SettingError_t loadSettingsFromPersistentStorage();

    /**
     * @brief This list the settings keys that match the provided key prefix.
     * @param keyPrefix The prefix of the keys to list.
     * @param permissions The permissions filter to apply to the keys.
     * @param filterMode The filter mode to apply to the permissions.
     * @param outputKeys The list of keys that match the provided key prefix.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The settings were successfully listed.
     * @retval INVALID_INPUT_ERROR The keyPrefix is nullptr or "".
     * @retval INVALID_INPUT_ERROR The permissions are invalid.
     * @retval INVALID_INPUT_ERROR The filterMode is invalid.
     */
    SettingError_t listSettingsKeys(const char* keyPrefix, SettingPermissions_t permissions, SettingPermissionsFilterMode_t filterMode, std::forward_list<std::string>& outputKeys);

    /**
     * @brief This function returns the value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValue The value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     */
    SettingError_t getSettingAsReal(const char* key, double& outputValue);

    /**
     * @brief This function returns the value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValue The value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     */
    SettingError_t getSettingAsInt(const char* key, int64_t& outputValue);

    /**
     * @brief This function returns the value of the setting with the provided key.
     * @param key The key of the setting to get.
     * @param outputValueBuffer The value of the setting. Must be a buffer with enough space to store the value.
     * @param outputValueSize The size of the outputValueBuffer.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully retrieved.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type.
     * @retval INSUFFICIENT_BUFFER_SIZE_ERROR The outputValueBuffer is null or not big enough to store the value.
     */
    SettingError_t getSettingAsString(const char* key, char* outputValueBuffer, size_t& outputValueSize);

    /**
     * @brief This function creates an empty setting located at the specified path, with the provided permissions.
     * @param key The key of the setting to create.
     * @param permissions The set of permissions associated with the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully created.
     * @retval KEY_EXISTS_ERROR The setting with the provided key already exists.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval INVALID_INPUT_ERROR The permissions are invalid.
     *
     * @note If delayed write is enabled, it will also start or reset the write timer.
     */
    SettingError_t addSettingKey(const char* key, SettingPermissions_t permissions);

    /**
     * @brief This function updates the value of the setting with the provided key.
     * @param key The key of the setting to update.
     * @param value The new value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully updated.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type or void.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     * @retval INVALID_INPUT_ERROR The value is nullptr.
     *
     * @note If delayed write is enabled, it will also start or reset the write timer.
     */
    SettingError_t putSettingValue(const char* key, const char* value);

    /**
     * @brief This function updates the value of the setting with the provided key.
     * @param key The key of the setting to update.
     * @param value The new value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully updated.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type or void.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     *
     * @note If delayed write is enabled, it will also start or reset the write timer.
     */
    SettingError_t putSettingValue(const char* key, int64_t value);

    /**
     * @brief This function updates the value of the setting with the provided key.
     * @param key The key of the setting to update.
     * @param value The new value of the setting.
     * @return SettingError_t The result of the operation.
     * @retval NO_ERROR The setting was successfully updated.
     * @retval KEY_NOT_FOUND_ERROR The setting with the provided key was not found.
     * @retval TYPE_MISMATCH_ERROR The setting with the provided key is not of the expected type or void.
     * @retval INVALID_INPUT_ERROR The key is nullptr or "".
     *
     * @note If delayed write is enabled, it will also start or reset the write timer.
     */
    SettingError_t putSettingValue(const char* key, double value);

private:
    OSShim_Mutex* moduleConfigMutex;
    SettingsParser* settingsParser;
    bool persistentStorageEnabled;
    Settings_t* settings;
    OSShim* osShim;
};

#endif // SETTINGSSTORAGE_SETTINGS_H
