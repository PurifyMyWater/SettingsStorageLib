#include "SettingsStorage.h"

#include <SettingsParser.h>
#include <cstring>

// This operator overload allows the enum SettingPermissions_t to have a bitwise OR operator.
SettingPermissions_t operator|(SettingPermissions_t lhs, SettingPermissions_t rhs) { return static_cast<SettingPermissions_t>(static_cast<std::byte>(lhs) | static_cast<std::byte>(rhs)); }

// This operator overload allows the enum SettingPermissions_t to have a bitwise AND operator.
SettingPermissions_t operator&(SettingPermissions_t lhs, SettingPermissions_t rhs) { return static_cast<SettingPermissions_t>(static_cast<std::byte>(lhs) & static_cast<std::byte>(rhs)); }

// This function returns a formatted string of the permissions described in the parameter permission.
const char* settingPermissionToString(const SettingPermissions_t permission, char* permissionString, const size_t permissionStringSize)
{
    if (permissionString == nullptr || permissionStringSize < PERMISSION_STRING_SIZE || !validatePermissions(permission))
    {
        return nullptr;
    }
    if (static_cast<bool>(permission & SettingPermissions_t::SYSTEM))
    {
        strcpy(permissionString, "SYSTEM | ");
    }
    else
    {
        strcpy(permissionString, "       | ");
    }
    if (static_cast<bool>(permission & SettingPermissions_t::ADMIN))
    {
        strcat(permissionString, "ADMIN | ");
    }
    else
    {
        strcat(permissionString, "      | ");
    }
    if (static_cast<bool>(permission & SettingPermissions_t::USER))
    {
        strcat(permissionString, "USER");
    }
    else
    {
        strcat(permissionString, "    ");
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
        restoreDefaultSettings(""); // Restore all settings
        return;
    }

    this->settingsParser = settingsParser;
    if (settingsParser == nullptr)
    {
        this->settingsParser = new SettingsParser(pathToSettingsFile);
    }

    SettingsParser::ParserError_t parserError;
    this->settings = this->settingsParser->readSettingsFromPersistentStorage(&parserError); // Always returns a valid pointer, with either the settings or an empty tree.

    if (parserError != SettingsParser::NO_ERROR)
    {
        *result = SETTINGS_FILESYSTEM_ERROR;
        restoreDefaultSettings(""); // Restore all settings
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

SettingsStorage::SettingError_t SettingsStorage::restoreDefaultSettings(const char* keyPrefix, SettingPermissions_t permissions, SettingPermissionsFilterMode_t filterMode) const
{
    if (keyPrefix == nullptr)
    {
        return INVALID_INPUT_ERROR;
    }

    SettingsKeysList_t outputKeys;

    SettingError_t result = listSettingsKeys(keyPrefix, permissions, filterMode, outputKeys);
    if (result != NO_ERROR)
    {
        return result;
    }

    for (const auto& key: outputKeys)
    {
        SettingValue_t* outputValue;
        result = getSettingValue(key.c_str(), outputValue);
        if (result != NO_ERROR)
        {
            return result;
        }

        outputValue->settingValueData = outputValue->settingDefaultValueData;
    }

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::storeSettingsInPersistentStorage() {}
SettingsStorage::SettingError_t SettingsStorage::loadSettingsFromPersistentStorage() {}

int SettingsStorage::listSettingsKeysCallback(void* data, const unsigned char* key, uint32_t key_len, void* value)
{
    auto* callbackData = static_cast<SettingsListCallbackData_t*>(data);
    SettingPermissions_t permissions = std::get<0>(*callbackData);
    SettingPermissionsFilterMode_t filterMode = std::get<1>(*callbackData);
    SettingsKeysList_t* outputKeys = std::get<2>(*callbackData);
    auto const* settingValue = static_cast<SettingValue_t* const>(value);

    switch (filterMode)
    {
        case MatchSettingsWithAnyPermissionsListed:
        {
            if (static_cast<uint32_t>(settingValue->settingPermissions & permissions) > 0) // If a bit is set in both, the result is greater than 0.
            {
                outputKeys->emplace_back(reinterpret_cast<const char*>(key), key_len);
            }
            return NO_ERROR;
        }

        case MatchSettingsWithAllPermissionsListed:
        {
            if (settingValue->settingPermissions == permissions)
            {
                outputKeys->emplace_back(reinterpret_cast<const char*>(key), key_len);
            }
            return NO_ERROR;
        }

        case ExcludeSettingsWithAllPermissionsListed:
        {
            if (settingValue->settingPermissions != permissions)
            {
                outputKeys->emplace_back(reinterpret_cast<const char*>(key), key_len);
            }
            return NO_ERROR;
        }

        case ExcludeSettingsWithAnyPermissionsListed:
        {
            if (static_cast<uint32_t>(settingValue->settingPermissions & permissions) == 0)
            {
                outputKeys->emplace_back(reinterpret_cast<const char*>(key), key_len);
            }
            return NO_ERROR;
        }
        default:
            return INVALID_INPUT_ERROR;
    }
}

SettingsStorage::SettingError_t SettingsStorage::listSettingsKeys(const char* keyPrefix, SettingPermissions_t permissions, SettingPermissionsFilterMode_t filterMode,
                                                                  SettingsKeysList_t& outputKeys) const
{
    if (keyPrefix == nullptr)
    {
        return INVALID_INPUT_ERROR;
    }

    if (!validatePermissions(permissions))
    {
        return INVALID_INPUT_ERROR;
    }

    SettingsListCallbackData_t callbackData = std::make_tuple(permissions, filterMode, &outputKeys);
    int res = settings->iterateOverPrefix(keyPrefix, static_cast<int>(strlen(keyPrefix)), listSettingsKeysCallback, &callbackData);
    return static_cast<SettingError_t>(res);
}

SettingsStorage::SettingError_t SettingsStorage::getSettingAsInt(const char* key, int64_t& outputValue, SettingPermissions_t* outputPermissions) const
{
    SettingValue_t* value;
    if (SettingError_t result = getSettingValue(key, value); result != NO_ERROR)
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

SettingsStorage::SettingError_t SettingsStorage::getSettingAsReal(const char* key, double& outputValue, SettingPermissions_t* outputPermissions) const
{
    SettingValue_t* value;
    if (SettingError_t result = getSettingValue(key, value); result != NO_ERROR)
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

SettingsStorage::SettingError_t SettingsStorage::getSettingAsString(const char* key, char* outputValueBuffer, const size_t outputValueSize, SettingPermissions_t* outputPermissions) const
{
    if (outputValueBuffer == nullptr)
    {
        return INVALID_INPUT_ERROR;
    }

    SettingValue_t* value;
    if (SettingError_t result = getSettingValue(key, value); result != NO_ERROR)
    {
        return result;
    }

    if (value->settingValueType != STRING)
    {
        return TYPE_MISMATCH_ERROR;
    }
    if (strlen(value->settingValueData.string) >= outputValueSize) // Only allow the string to be copied if it fits in the buffer. (The == is to account for the null terminator)
    {
        return INSUFFICIENT_BUFFER_SIZE_ERROR;
    }

    if (outputPermissions != nullptr)
    {
        *outputPermissions = value->settingPermissions;
    }
    strcpy(outputValueBuffer, value->settingValueData.string);
    outputValueBuffer[outputValueSize] = '\0';

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::addSettingAsInt(const char* key, const SettingPermissions_t permissions, const int64_t defaultValue) const
{
    if (key == nullptr || key[0] == '\0' || !validatePermissions(permissions))
    {
        return INVALID_INPUT_ERROR;
    }

    auto* newValue = new SettingValue_t();
    newValue->settingPermissions = permissions;
    newValue->settingValueType = INTEGER;
    newValue->settingValueData.integer = defaultValue;
    newValue->settingDefaultValueData.integer = defaultValue;
    if (this->settings->insertIfNotExists(key, static_cast<int>(strlen(key)), newValue) != nullptr)
    {
        delete newValue;
        return KEY_EXISTS_ERROR;
    }
    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::addSettingAsReal(const char* key, const SettingPermissions_t permissions, const double defaultValue) const
{
    if (key == nullptr || key[0] == '\0' || !validatePermissions(permissions))
    {
        return INVALID_INPUT_ERROR;
    }

    auto* newValue = new SettingValue_t();
    newValue->settingPermissions = permissions;
    newValue->settingValueType = REAL;
    newValue->settingValueData.real = defaultValue;
    newValue->settingDefaultValueData.real = defaultValue;
    if (this->settings->insertIfNotExists(key, static_cast<int>(strlen(key)), newValue) != nullptr)
    {
        delete newValue;
        return KEY_EXISTS_ERROR;
    }
    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::addSettingAsString(const char* key, const SettingPermissions_t permissions, const char* defaultValue) const
{
    if (key == nullptr || key[0] == '\0' || !validatePermissions(permissions) || defaultValue == nullptr)
    {
        return INVALID_INPUT_ERROR;
    }

    auto* newValue = new SettingValue_t();
    newValue->settingPermissions = permissions;
    newValue->settingValueType = STRING;
    newValue->settingValueData.string = strdup(defaultValue);
    newValue->settingDefaultValueData.string = strdup(defaultValue);

    if (this->settings->insertIfNotExists(key, static_cast<int>(strlen(key)), newValue) != nullptr)
    {
        free(newValue->settingValueData.string);
        free(newValue->settingDefaultValueData.string);
        delete newValue;

        return KEY_EXISTS_ERROR;
    }
    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::putSettingValueAsInt(const char* key, const int64_t value) const
{
    SettingValue_t* outputValue;

    if (SettingError_t result = getSettingValue(key, outputValue); result != NO_ERROR)
    {
        return result;
    }

    if (outputValue->settingValueType != INTEGER)
    {
        return TYPE_MISMATCH_ERROR;
    }

    outputValue->settingValueData.integer = value;

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::putSettingValueAsReal(const char* key, const double value) const
{
    SettingValue_t* outputValue;

    if (SettingError_t result = getSettingValue(key, outputValue); result != NO_ERROR)
    {
        return result;
    }

    if (outputValue->settingValueType != REAL)
    {
        return TYPE_MISMATCH_ERROR;
    }

    outputValue->settingValueData.real = value;

    return NO_ERROR;
}

SettingsStorage::SettingError_t SettingsStorage::putSettingValueAsString(const char* key, const char* value) const
{
    if (value == nullptr)
    {
        return INVALID_INPUT_ERROR;
    }

    SettingValue_t* outputValue;

    if (SettingError_t result = getSettingValue(key, outputValue); result != NO_ERROR)
    {
        return result;
    }

    if (outputValue->settingValueType != STRING)
    {
        return TYPE_MISMATCH_ERROR;
    }

    free(outputValue->settingValueData.string);
    outputValue->settingValueData.string = strdup(value);

    return NO_ERROR;
}

bool validatePermissions(const SettingPermissions_t permissions) { return permissions <= ALL_PERMISSIONS; }

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

void SettingsStorage::freeSettingValue(const SettingValue_t* settingValue)
{
    if (settingValue->settingValueType == STRING)
    {
        free(settingValue->settingValueData.string);
    }
    delete settingValue;
}
