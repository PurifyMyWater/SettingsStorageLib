#include "SettingsStorage.h"
#include "LinuxOSShim.h"
#include "SettingsParserMock.h"
#include "gtest/gtest.h"

#define NEW_POPULATED_SETTINGS_T(name)                                                                                                                                                                 \
    SettingsStorage::Settings_t(name);                                                                                                                                                                 \
    SettingsStorage::SettingValue_t _valueSetting1 = {                                                                                                                                                 \
            .settingValueType = SettingsStorage::SettingValueType_t::REAL, .settingValueData = {.real = 1.23}, .settingPermissions = SettingPermissions_t::USER};                                      \
    (name).insert("menu1/setting1", static_cast<int>(strlen("menu1/setting1")), &_valueSetting1);                                                                                                                        \
    SettingsStorage::SettingValue_t _valueSetting2 = {                                                                                                                                                 \
            .settingValueType = SettingsStorage::SettingValueType_t::INTEGER, .settingValueData = {.integer = 45}, .settingPermissions = SettingPermissions_t::USER};                                  \
    (name).insert("menu1/setting2", static_cast<int>(strlen("menu1/setting2")), &_valueSetting2);                                                                                                                        \
    const char* _string3 = "string3";                                                                                                                                                                  \
    SettingsStorage::SettingValue_t _valueSetting3 = {                                                                                                                                                 \
            .settingValueType = SettingsStorage::SettingValueType_t::STRING, .settingValueData = {.string = _string3}, .settingPermissions = SettingPermissions_t::USER};                              \
    (name).insert("menu2/setting3", static_cast<int>(strlen("menu2/setting3")), &_valueSetting3)

#define NEW_POPULATED_SETTINGS_STORAGE                                                                                                                                                                 \
    NEW_POPULATED_SETTINGS_T(settings);                                                                                                                                                                \
    const char*(pathToSettingsFile) = "path/to/settings/file";                                                                                                                                         \
    SettingsStorage::SettingError_t result;                                                                                                               \
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

    // When
    double outputValue;
    result = settingsStorage.getSettingAsReal("menu1/setting1", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
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

    // When
    int64_t outputValue;
    result = settingsStorage.getSettingAsInt("menu1/setting2", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
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

// TEST(SettingsStorage, AddSettingKey)
// {
//     NEW_POPULATED_SETTINGS_STORAGE;
//
//     settingsStorage.addSettingKey()
// }
