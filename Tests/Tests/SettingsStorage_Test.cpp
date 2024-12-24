#include "SettingsStorage.h"
#include "LinuxOSShim.h"
#include "SettingsParserMock.h"
#include "gtest/gtest.h"

#define NEW_POPULATED_SETTINGS_T(name)                                                                                                                                                                 \
    SettingsStorage::Settings_t(name); \
    double _real1_default = 1.23; \
    SettingsStorage::SettingValue_t _valueSetting1 = {                                                                                                                                                 \
            .settingValueType = SettingsStorage::SettingValueType_t::REAL, .settingValueData = {.real = 1.23}, .settingDefaultValueData = {.real = _real1_default}, .settingPermissions = SettingPermissions_t::USER};                                      \
    (name).insert("menu1/setting1", static_cast<int>(strlen("menu1/setting1")), &_valueSetting1);                                                                                                      \
    int64_t _int2_default = 45; \
    SettingsStorage::SettingValue_t _valueSetting2 = {                                                                                                                                                 \
            .settingValueType = SettingsStorage::SettingValueType_t::INTEGER, .settingValueData = {.integer = 45}, .settingDefaultValueData = {.integer = _int2_default}, .settingPermissions = SettingPermissions_t::USER};                                  \
    (name).insert("menu1/setting2", static_cast<int>(strlen("menu1/setting2")), &_valueSetting2);                                                                                                      \
    char* _string3 = strdup("string3");                                                                                                                                                                \
    char* _string3_default = strdup("string3");                                                                                                                                                                \
    SettingsStorage::SettingValue_t _valueSetting3 = {                                                                                                                                                 \
            .settingValueType = SettingsStorage::SettingValueType_t::STRING, .settingValueData = {.string = _string3}, \
            .settingDefaultValueData = {.string = _string3_default}, .settingPermissions = SettingPermissions_t::USER};                              \
    (name).insert("menu2/setting3", static_cast<int>(strlen("menu2/setting3")), &_valueSetting3)

#define NEW_POPULATED_SETTINGS_STORAGE                                                                                                                                                                 \
    NEW_POPULATED_SETTINGS_T(settings);                                                                                                                                                                \
    const char*(pathToSettingsFile) = "path/to/settings/file";                                                                                                                                         \
    SettingsStorage::SettingError_t result;                                                                                                                                                            \
    SettingsParserMock* settingsParserMock = new SettingsParserMock(pathToSettingsFile);                                                                                                               \
    settingsParserMock->setReadSettingsReturnResult(SettingsParser::NO_ERROR);                                                                                                                         \
    settingsParserMock->setReadSettingsReturnValue(&settings);                                                                                                                                         \
    SettingsStorage settingsStorage(pathToSettingsFile, &result, linuxOSShim, settingsParserMock)

static LinuxOSShim linuxOSShim;

TEST(SettingsStorage, Constructor)
{
    NEW_POPULATED_SETTINGS_T(settings);

    // Want
    const char*(pathToSettingsFile) = "path/to/settings/file";
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR, result;
    SettingsParserMock* settingsParserMock = new SettingsParserMock(pathToSettingsFile);
    settingsParserMock->setReadSettingsReturnResult(SettingsParser::NO_ERROR);
    settingsParserMock->setReadSettingsReturnValue(&settings);

    // When
    SettingsStorage settingsStorage(pathToSettingsFile, &result, linuxOSShim, settingsParserMock);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_TRUE(settingsStorage.isPersistentStorageEnabled());
}

TEST(SettingsStorage, Constructor_NullPath)
{
    NEW_POPULATED_SETTINGS_T(settings);

    // Want
    const char*(pathToSettingsFile) = nullptr;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR, result;
    SettingsParserMock* settingsParserMock = new SettingsParserMock(pathToSettingsFile);
    settingsParserMock->setReadSettingsReturnResult(SettingsParser::INVALID_PATH_ERROR);
    settingsParserMock->setReadSettingsReturnValue(&settings);

    // When
    SettingsStorage settingsStorage(pathToSettingsFile, &result, linuxOSShim, settingsParserMock);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_FALSE(settingsStorage.isPersistentStorageEnabled());
}

TEST(SettingsStorage, Constructor_EmptyPath)
{
    NEW_POPULATED_SETTINGS_T(settings);

    // Want
    const char*(pathToSettingsFile) = "";
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR, result;
    SettingsParserMock* settingsParserMock = new SettingsParserMock(pathToSettingsFile);
    settingsParserMock->setReadSettingsReturnResult(SettingsParser::INVALID_PATH_ERROR);
    settingsParserMock->setReadSettingsReturnValue(&settings);

    // When
    SettingsStorage settingsStorage(pathToSettingsFile, &result, linuxOSShim, settingsParserMock);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_FALSE(settingsStorage.isPersistentStorageEnabled());
}

TEST(SettingsStorage, Constructor_InvalidPath)
{
    NEW_POPULATED_SETTINGS_T(settings);

    // Want
    const char*(pathToSettingsFile) = "inexistant/path/to/settings/file";
    SettingsStorage::SettingError_t expected_result = SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result;
    SettingsParserMock* settingsParserMock = new SettingsParserMock(pathToSettingsFile);
    settingsParserMock->setReadSettingsReturnResult(SettingsParser::FILE_NOT_FOUND_ERROR);
    settingsParserMock->setReadSettingsReturnValue(&settings);

    // When
    SettingsStorage settingsStorage(pathToSettingsFile, &result, linuxOSShim, settingsParserMock);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_TRUE(settingsStorage.isPersistentStorageEnabled());
}

TEST(SettingsStorage, DisablePersistentStorage)
{
    NEW_POPULATED_SETTINGS_T(settings);

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR, result;
    SettingsParserMock* settingsParserMock = new SettingsParserMock("path/to/settings/file");
    settingsParserMock->setReadSettingsReturnResult(SettingsParser::NO_ERROR);
    settingsParserMock->setReadSettingsReturnValue(&settings);

    // Prepare
    SettingsStorage settingsStorage("path/to/settings/file", &result, linuxOSShim, settingsParserMock);
    EXPECT_EQ(expected_result, result);
    EXPECT_TRUE(settingsStorage.isPersistentStorageEnabled());

    // When
    settingsStorage.disablePersistentStorage();

    // Then
    EXPECT_FALSE(settingsStorage.isPersistentStorageEnabled());
}

TEST(SettingsStorage, GetSettingAsRealValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    double expectedValue = _valueSetting1.settingValueData.real;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting1.settingPermissions, outputPermissions;

    // When
    double outputValue;
    result = settingsStorage.getSettingAsReal("menu1/setting1", outputValue, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, GetSettingAsRealInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    double outputValue;
    result = settingsStorage.getSettingAsReal(nullptr, outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsRealVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    double outputValue;
    result = settingsStorage.getSettingAsReal("", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsRealKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    double outputValue;
    result = settingsStorage.getSettingAsReal("menu1/setting3", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsRealTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    double outputValue;
    result = settingsStorage.getSettingAsReal("menu1/setting2", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsIntValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    int64_t expectedValue = _valueSetting2.settingValueData.integer;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting2.settingPermissions, outputPermissions;

    // When
    int64_t outputValue;
    result = settingsStorage.getSettingAsInt("menu1/setting2", outputValue, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, GetSettingAsIntInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage.getSettingAsInt(nullptr, outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsIntVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage.getSettingAsInt("", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsIntKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage.getSettingAsInt("menu1/setting3", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsIntTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage.getSettingAsInt("menu1/setting1", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsStringValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    const char* expectedValue = _valueSetting3.settingValueData.string;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting3.settingPermissions, outputPermissions;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage.getSettingAsString("menu2/setting3", outputValueBuffer, outputValueSize, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValueBuffer);
}

TEST(SettingsStorage, GetSettingAsStringInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage.getSettingAsString(nullptr, outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsStringVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage.getSettingAsString("", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsStringKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage.getSettingAsString("menu2/setting4", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsStringTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage.getSettingAsString("menu1/setting1", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, GetSettingAsStringInsufficientBufferSize)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INSUFFICIENT_BUFFER_SIZE_ERROR;

    // When
    char outputValueBuffer[5];
    size_t outputValueSize = 5;
    result = settingsStorage.getSettingAsString("menu2/setting3", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsIntValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    int64_t expectedValue = 12;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting2.settingPermissions, outputPermissions;
    const char* key = "menu1/setting2";

    // When
    result = settingsStorage.putSettingValueAsInt(key, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    int64_t outputValue;
    result = settingsStorage.getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, PutSettingValueAsIntInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    int64_t value = 12;

    // When
    result = settingsStorage.putSettingValueAsInt(nullptr, value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsIntVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    int64_t value = 12;

    // When
    result = settingsStorage.putSettingValueAsInt("", value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsIntKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;
    int64_t value = 12;

    // When
    result = settingsStorage.putSettingValueAsInt("menu1/setting3", value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsIntTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting1.settingPermissions, outputPermissions;
    int64_t value = 12;
    const char* key = "menu1/setting1";
    double expectedValue = _valueSetting1.settingValueData.real, outputValue;

    // When
    result = settingsStorage.putSettingValueAsInt(key, value);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage.getSettingAsReal(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, PutSettingValueAsRealValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    double expectedValue = 12.34;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting1.settingPermissions, outputPermissions;
    const char* key = "menu1/setting1";

    // When
    result = settingsStorage.putSettingValueAsReal(key, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    double outputValue;
    result = settingsStorage.getSettingAsReal(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, PutSettingValueAsRealInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    double value = 12.34;

    // When
    result = settingsStorage.putSettingValueAsReal(nullptr, value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsRealVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    double value = 12.34;

    // When
    result = settingsStorage.putSettingValueAsReal("", value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsRealKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;
    double value = 12.34;

    // When
    result = settingsStorage.putSettingValueAsReal("menu1/setting3", value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsRealTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting2.settingPermissions, outputPermissions;
    double value = 12.34;
    const char* key = "menu1/setting2";
    int64_t expectedValue = _valueSetting2.settingValueData.integer, outputValue;

    // When
    result = settingsStorage.putSettingValueAsReal(key, value);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage.getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, PutSettingValueAsStringValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    const char* expectedValue = "new string";
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting3.settingPermissions, outputPermissions;
    const char* key = "menu2/setting3";

    // When
    result = settingsStorage.putSettingValueAsString(key, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    char outputValueBuffer[12];
    size_t outputValueSize = 12;
    result = settingsStorage.getSettingAsString(key, outputValueBuffer, outputValueSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValueBuffer);
}

TEST(SettingsStorage, PutSettingValueAsStringInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    const char* value = "new string";

    // When
    result = settingsStorage.putSettingValueAsString(nullptr, value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsStringVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    const char* value = "new string";

    // When
    result = settingsStorage.putSettingValueAsString("", value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsStringInvalidValue)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    const char* key = "menu2/setting3";

    // When
    result = settingsStorage.putSettingValueAsString(key, nullptr);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsStringKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;
    const char* value = "new string";

    // When
    result = settingsStorage.putSettingValueAsString("menu2/setting4", value);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, PutSettingValueAsStringTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting2.settingPermissions, outputPermissions;
    const char* value = "new string";
    const char* key = "menu1/setting2";
    int64_t expectedValue = _valueSetting2.settingValueData.integer, outputValue;

    // When
    result = settingsStorage.putSettingValueAsString(key, value);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage.getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, AddSettingKeyAsIntValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = SettingPermissions_t::USER, outputPermissions;
    const char* key = "menu2/setting4";
    int64_t expectedValue = 12, outputValue;

    // When
    result = settingsStorage.addSettingAsInt(key, expectedPermissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage.getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    expectedValue++;

    result = settingsStorage.putSettingValueAsInt(key, expectedValue); // Add a value to the key to see if the value is updated.
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    result = settingsStorage.getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, AddSettingKeyAsIntInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    int64_t expectedValue = 12;

    // When
    result = settingsStorage.addSettingAsInt(nullptr, permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsIntVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    int64_t expectedValue = 12;

    // When
    result = settingsStorage.addSettingAsInt("", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsIntKeyExists)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_EXISTS_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    int64_t expectedValue = 12;

    // When
    result = settingsStorage.addSettingAsInt("menu1/setting1", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsIntInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS) + 1);
    int64_t expectedValue = 12;

    // When
    result = settingsStorage.addSettingAsInt("menu2/setting4", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsRealValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = SettingPermissions_t::USER, outputPermissions;
    const char* key = "menu2/setting4";
    double expectedValue = 12.07, outputValue;

    // When
    result = settingsStorage.addSettingAsReal(key, expectedPermissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage.getSettingAsReal(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    expectedValue++;

    result = settingsStorage.putSettingValueAsReal(key, expectedValue); // Add a value to the key to see if the value is updated.
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    result = settingsStorage.getSettingAsReal(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);
}

TEST(SettingsStorage, AddSettingKeyAsRealInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    double expectedValue = 12.07;

    // When
    result = settingsStorage.addSettingAsReal(nullptr, permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsRealVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    double expectedValue = 12.07;

    // When
    result = settingsStorage.addSettingAsReal("", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsRealKeyExists)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_EXISTS_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    double expectedValue = 12.07;

    // When
    result = settingsStorage.addSettingAsReal("menu1/setting1", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsRealInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS) + 1);
    double expectedValue = 12.07;

    // When
    result = settingsStorage.addSettingAsReal("menu2/setting4", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsStringValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = SettingPermissions_t::USER, outputPermissions;
    const char* key = "menu2/setting4";
    constexpr size_t outputBufferSize = 32;
    const char* expectedValue = "new string";
    char outputValue[outputBufferSize];

    // When
    result = settingsStorage.addSettingAsString(key, expectedPermissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage.getSettingAsString(key, outputValue, outputBufferSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValue);

    expectedValue = "new string 2";

    result = settingsStorage.putSettingValueAsString(key, expectedValue); // Add a value to the key to see if the value is updated.
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    result = settingsStorage.getSettingAsString(key, outputValue, outputBufferSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValue);
}

TEST(SettingsStorage, AddSettingKeyAsStringInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    const char* expectedValue = "new string";

    // When
    result = settingsStorage.addSettingAsString(nullptr, permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsStringVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    const char* expectedValue = "new string";

    // When
    result = settingsStorage.addSettingAsString("", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsStringKeyExists)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_EXISTS_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    const char* expectedValue = "new string";

    // When
    result = settingsStorage.addSettingAsString("menu1/setting1", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, AddSettingKeyAsStringInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS) + 1);
    const char* expectedValue = "new string";

    // When
    result = settingsStorage.addSettingAsString("menu2/setting4", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, listSettingsKeysVoidKeyPrefix)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage.listSettingsKeys("", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting1", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting2", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu2/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAllPermissionsListedResultAny)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage.addSettingAsInt("menu1/setting3", ALL_PERMISSIONS, expectedValue);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage.listSettingsKeys("menu", ALL_PERMISSIONS, MatchSettingsWithAllPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAllPermissionsListedResultNone)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage.listSettingsKeys("menu", ALL_PERMISSIONS, MatchSettingsWithAllPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAnyPermissionsListedResultAll)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage.addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage.listSettingsKeys("menu", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting1", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting2", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting3", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu2/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAnyPermissionsListedResultNone)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage.listSettingsKeys("menu", NO_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAnyPermissionsListedResultMixed)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage.addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage.listSettingsKeys("menu", SettingPermissions_t::ADMIN | SettingPermissions_t::SYSTEM, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAllPermissionsListedResultAll)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage.listSettingsKeys("menu", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, ExcludeSettingsWithAllPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting1", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting2", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu2/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAllPermissionsListedResultNone)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage.listSettingsKeys("menu", SettingPermissions_t::USER, ExcludeSettingsWithAllPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAllPermissionsListedResultMixed)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage.addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage.listSettingsKeys("menu", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, ExcludeSettingsWithAllPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting1", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting2", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu2/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAnyPermissionsListedResultMixed)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage.addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage.listSettingsKeys("menu", SettingPermissions_t::ADMIN, ExcludeSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting1", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting2", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu2/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAnyPermissionsListedResultAll)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage.addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage.listSettingsKeys("menu", SettingPermissions_t::SYSTEM, ExcludeSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting1", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting2", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting3", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu2/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}
TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAnyPermissionsListedResultNone)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage.addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage.listSettingsKeys("menu", SettingPermissions_t::USER | SettingPermissions_t::SYSTEM, ExcludeSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_EQ(outputKeys.end(), it);
}

TEST(SettingsStorage, listSettingsKeysInvalidKeyPrefix)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    result = settingsStorage.listSettingsKeys(nullptr, ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, listSettingsKeysInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    result = settingsStorage.listSettingsKeys("menu1", static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS) + 1), MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsStorage, listSettingsKeysInvalidFilterMode)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    result = settingsStorage.listSettingsKeys("menu1", ALL_PERMISSIONS, static_cast<SettingPermissionsFilterMode_t>(-1), outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);
}


TEST(SettingsStorage, listSettingsKeysEmptyPrefix)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage.listSettingsKeys("", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting1", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting2", it->c_str());
    ++it;

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu2/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);
}
