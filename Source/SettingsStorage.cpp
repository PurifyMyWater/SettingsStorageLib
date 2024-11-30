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
        this->settings = new Settings_t();
        restoreComponentDefaultSettings(""); // Restore all settings
        return;
    }

    if (settingsParser == nullptr)
    {
        settingsParser = new SettingsParser(pathToSettingsFile);
    }
    this->settingsParser = settingsParser;

    SettingsParser::ParserError_t parserError;
    this->settings = this->settingsParser->readSettingsFromPersistentStorage(&parserError); // Always returns a valid pointer, with either the settings or an empty tree.

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

SettingsStorage::SettingError_t SettingsStorage::getSettingAsReal(const char* key, double& outputValue, SettingPermissions_t* outputPermissions) const
{
    SettingValue_t* value;
    SettingError_t result = getSettingValue(key, value);
    if (result != NO_ERROR)
    {
        return result;
    }

    if (value->settingValueType != REAL)
    {
        return TYPE_MISMATCH_ERROR;
    }

    if (outputPermissions != nullptr)
    {
        *outputPermissions = value->settingPermissions;
    }
    outputValue = value->settingValueData.real;

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::getSettingAsInt(const char* key, int64_t& outputValue, SettingPermissions_t* outputPermissions) const
{
    SettingValue_t* value;
    SettingError_t result = getSettingValue(key, value);
    if (result != NO_ERROR)
    {
        return result;
    }

    if (value->settingValueType != INTEGER)
    {
        return TYPE_MISMATCH_ERROR;
    }

    if (outputPermissions != nullptr)
    {
        *outputPermissions = value->settingPermissions;
    }
    outputValue = value->settingValueData.integer;

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::getSettingAsString(const char* key, char* outputValueBuffer, const size_t outputValueSize, SettingPermissions_t* outputPermissions) const
{
    if (outputValueBuffer == nullptr)
    {
        return INVALID_INPUT_ERROR;
    }

    SettingValue_t* value;
    SettingError_t result = getSettingValue(key, value);
    if (result != NO_ERROR)
    {
        return result;
    }

    if (value->settingValueType != STRING)
    {
        return TYPE_MISMATCH_ERROR;
    }
    size_t valueLen = strlen(value->settingValueData.string);
    if (valueLen >= outputValueSize)
    {
        return INSUFFICIENT_BUFFER_SIZE_ERROR;
    }

    if (outputPermissions != nullptr)
    {
        *outputPermissions = value->settingPermissions;
    }
    strncpy(outputValueBuffer, value->settingValueData.string, outputValueSize);
    outputValueBuffer[valueLen] = '\0';

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::addSettingKey(const char* key, const SettingPermissions_t permissions) const
{
    if (key == nullptr || key[0] == '\0' || !validatePermissions(permissions))
    {
        return INVALID_INPUT_ERROR;
    }

    SettingValue_t* newValue = new SettingValue_t();
    newValue->settingPermissions = permissions;
    newValue->settingValueType = VOID;
    newValue->settingValueData = {0};
    if (this->settings->insertIfNotExists(key, static_cast<int>(strlen(key)), newValue) != nullptr)
    {
        delete newValue;
        return KEY_EXISTS_ERROR;
    }
    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::putSettingValueAsString(const char* key, const char* value) const
{
    if (value == nullptr)
    {
        return INVALID_INPUT_ERROR;
    }

    SettingValue_t* outputValue;
    SettingError_t result = getSettingValue(key, outputValue);

    if (result != NO_ERROR)
    {
        return result;
    }

    if (outputValue->settingValueType == VOID)
    {
        outputValue->settingValueType = STRING;
    }
    else if (outputValue->settingValueType != STRING)
    {
        return TYPE_MISMATCH_ERROR;
    }
    else
    {
        free(outputValue->settingValueData.string);
    }

    outputValue->settingValueData.string = strdup(value);

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::putSettingValueAsInt(const char* key, const int64_t value) const
{
    SettingValue_t* outputValue;
    SettingError_t result = getSettingValue(key, outputValue);

    if (result != NO_ERROR)
    {
        return result;
    }

    if (outputValue->settingValueType == VOID)
    {
        outputValue->settingValueType = INTEGER;
    }
    else if (outputValue->settingValueType != INTEGER)
    {
        return TYPE_MISMATCH_ERROR;
    }

    outputValue->settingValueData.integer = value;

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::putSettingValueAsReal(const char* key, const double value) const
{
    SettingValue_t* outputValue;
    SettingError_t result = getSettingValue(key, outputValue);

    if (result != NO_ERROR)
    {
        return result;
    }

    if (outputValue->settingValueType == VOID)
    {
        outputValue->settingValueType = REAL;
    }
    else if (outputValue->settingValueType != REAL)
    {
        return TYPE_MISMATCH_ERROR;
    }

    outputValue->settingValueData.real = value;

    return NO_ERROR;
}

bool SettingsStorage::validatePermissions(const SettingPermissions_t permissions) { return permissions <= ALL_PERMISSIONS; }

SettingsStorage::SettingError_t SettingsStorage::getSettingValue(const char* key, SettingValue_t*& outputValue) const
{
    if (key == nullptr || key[0] == '\0')
    {
        return INVALID_INPUT_ERROR;
    }

    outputValue = this->settings->search(key, static_cast<int>(strlen(key)));
    if (outputValue == nullptr)
    {
        return KEY_NOT_FOUND_ERROR;
    }

    return NO_ERROR;
}
