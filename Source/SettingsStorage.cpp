#include "SettingsStorage.h"

#include <SettingsParser.h>
#include <cstring>

// This operator overload allows the enum SettingPermissions_t to have a bitwise OR operator.
SettingPermissions_t operator|(SettingPermissions_t lhs, SettingPermissions_t rhs)
{
    using SettingPermissionType = std::underlying_type_t<SettingPermissions_t>;
    return static_cast<SettingPermissions_t>(static_cast<SettingPermissionType>(lhs) | static_cast<SettingPermissionType>(rhs));
}

// This operator overload allows the enum SettingPermissions_t to have a bitwise AND operator.
SettingPermissions_t operator&(SettingPermissions_t lhs, SettingPermissions_t rhs)
{
    using SettingPermissionType = std::underlying_type_t<SettingPermissions_t>;
    return static_cast<SettingPermissions_t>(static_cast<SettingPermissionType>(lhs) & static_cast<SettingPermissionType>(rhs));
}

// This function returns a formatted string of the permissions described in the parameter permission.
const char* settingPermissionToString(SettingPermissions_t permission, char* permissionString, size_t permissionStringSize)
{
    if (permissionString == nullptr || permissionStringSize < PERMISSION_STRING_SIZE)
    {
        return nullptr;
    }
    if (permission == SettingPermissions_t::SYSTEM)
    {
        strcpy(permissionString, "SYSTEM | ");
    }
    else
    {
        strcpy(permissionString, "       | ");
    }
    if (permission == SettingPermissions_t::ADMIN)
    {
        strcat(permissionString, "ADMIN | ");
    }
    else
    {
        strcat(permissionString, "      | ");
    }
    if (permission == SettingPermissions_t::USER)
    {
        strcat(permissionString, "USER");
    }
    return permissionString;
}
SettingsStorage::SettingsStorage(const char* pathToSettingsFile, SettingError_t* result, OSShim& osShim, SettingsParser* settingsParser)
{
    *result = NO_ERROR;
    this->osShim = &osShim;
    this->moduleConfigMutex = osShim.osCreateMutex();
    assert(this->moduleConfigMutex != nullptr && "Mutex creation failed");

    this->persistentStorageEnabled = true;
    this->settings = nullptr;

    if (pathToSettingsFile == nullptr || pathToSettingsFile[0] == '\0')
    {
        *result = INVALID_INPUT_ERROR;
        disablePersistentStorage();
        restoreComponentDefaultSettings(""); // Restore all settings
        return;
    }

    if (settingsParser == nullptr)
    {
        settingsParser = new SettingsParser(pathToSettingsFile);
    }
    this->settingsParser = settingsParser;

    SettingsParser::ParserError_t parserError;
    this->settings = this->settingsParser->readSettingsFromPersistentStorage(&parserError);

    if (parserError != SettingsParser::NO_ERROR)
    {
        *result = SETTINGS_FILESYSTEM_ERROR;
        restoreComponentDefaultSettings(""); // Restore all settings
        return;
    }
}

SettingsStorage::~SettingsStorage() // TODO Clean up settings
{
}

bool SettingsStorage::isPersistentStorageEnabled() const
{
    bool result = false;
    if (moduleConfigMutex->wait(SETTINGS_STORAGE_MUTEX_TIMEOUT_MS))
    {
        result = this->persistentStorageEnabled;
        moduleConfigMutex->signal();
    }
    return result;
}

bool SettingsStorage::disablePersistentStorage()
{
    bool res = false;
    if (moduleConfigMutex->wait(SETTINGS_STORAGE_MUTEX_TIMEOUT_MS))
    {
        this->persistentStorageEnabled = false;
        moduleConfigMutex->signal();
        res = true;
    }
    return res;
}

SettingsStorage::SettingError_t SettingsStorage::registerComponent(ComponentInfo_t& componentInfo) {}
std::forward_list<SettingsStorage::ComponentInfo_t> SettingsStorage::listRegisteredComponents() {}
SettingsStorage::SettingError_t SettingsStorage::restoreComponentDefaultSettings(const char* componentName) { return NO_ERROR; }
SettingsStorage::SettingError_t SettingsStorage::storeSettingsInPersistentStorage() {}
SettingsStorage::SettingError_t SettingsStorage::loadSettingsFromPersistentStorage() {}
SettingsStorage::SettingError_t SettingsStorage::listSettingsKeys(const char* keyPrefix, SettingPermissions_t permissions, SettingPermissionsFilterMode_t filterMode,
                                                                  std::forward_list<std::string>& outputKeys)
{
}
SettingsStorage::SettingError_t SettingsStorage::getSettingAsReal(const char* key, double& outputValue) {}
SettingsStorage::SettingError_t SettingsStorage::getSettingAsInt(const char* key, int64_t& outputValue) {}
SettingsStorage::SettingError_t SettingsStorage::getSettingAsString(const char* key, char* outputValueBuffer, size_t& outputValueSize) {}
SettingsStorage::SettingError_t SettingsStorage::addSettingKey(const char* key, SettingPermissions_t permissions) {}
SettingsStorage::SettingError_t SettingsStorage::putSettingValue(const char* key, const char* value) {}
SettingsStorage::SettingError_t SettingsStorage::putSettingValue(const char* key, int64_t value) {}
SettingsStorage::SettingError_t SettingsStorage::putSettingValue(const char* key, double value) {}
