#include "SettingsStorage.h"
#include "LinuxOSShim.h"
#include "SettingsFileMock.h"
#include "gtest/gtest.h"

constexpr char defaultSettingsFile[] = "menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\n\r1874197929\n";
constexpr int64_t defaultSettingsFileSize = sizeof(defaultSettingsFile) + 5000;

void menu1RegisterSettigsCallback(SettingsStorage& settingsStorage)
{
    settingsStorage.addSettingAsReal("menu1/setting1", SettingPermissions_t::USER, 1.23);
    settingsStorage.addSettingAsInt("menu1/setting2", SettingPermissions_t::USER, 45);
}

void menu2RegisterSettigsCallback(SettingsStorage& settingsStorage) { settingsStorage.addSettingAsString("menu2/setting3", SettingPermissions_t::USER, "string3"); }

#define NEW_POPULATED_SETTINGS_T(name)                                                                                                                                                                 \
    SettingsStorage::Settings_t(name){linuxOSShim};                                                                                                                                                    \
    double _real1_default = 1.23;                                                                                                                                                                      \
    SettingsStorage::SettingValue_t _valueSetting1 = {.settingValueType = SettingsStorage::SettingValueType_t::REAL,                                                                                   \
                                                      .settingValueData = {.real = 1.23},                                                                                                              \
                                                      .settingDefaultValueData = {.real = _real1_default},                                                                                             \
                                                      .settingPermissions = SettingPermissions_t::USER};                                                                                               \
    (name).insert("menu1/setting1", static_cast<int>(strlen("menu1/setting1")), &_valueSetting1);                                                                                                      \
    int64_t _int2_default = 45;                                                                                                                                                                        \
    SettingsStorage::SettingValue_t _valueSetting2 = {.settingValueType = SettingsStorage::SettingValueType_t::INTEGER,                                                                                \
                                                      .settingValueData = {.integer = 45},                                                                                                             \
                                                      .settingDefaultValueData = {.integer = _int2_default},                                                                                           \
                                                      .settingPermissions = SettingPermissions_t::USER};                                                                                               \
    (name).insert("menu1/setting2", static_cast<int>(strlen("menu1/setting2")), &_valueSetting2);                                                                                                      \
    char* _string3 = strdup("string3");                                                                                                                                                                \
    char* _string3_default = strdup("string3");                                                                                                                                                        \
    SettingsStorage::SettingValue_t _valueSetting3 = {.settingValueType = SettingsStorage::SettingValueType_t::STRING,                                                                                 \
                                                      .settingValueData = {.string = _string3},                                                                                                        \
                                                      .settingDefaultValueData = {.string = _string3_default},                                                                                         \
                                                      .settingPermissions = SettingPermissions_t::USER};                                                                                               \
    (name).insert("menu2/setting3", static_cast<int>(strlen("menu2/setting3")), &_valueSetting3)

#define TEAR_DOWN_NEW_POPULATED_SETTINGS_T                                                                                                                                                             \
    free(_string3);                                                                                                                                                                                    \
    free(_string3_default)

#define NEW_POPULATED_SETTINGS_STORAGE                                                                                                                                                                 \
    NEW_POPULATED_SETTINGS_T(settings);                                                                                                                                                                \
    SettingsStorage::SettingError_t result;                                                                                                                                                            \
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;                                                                                                                      \
    registerSettingsCallbackList.push_back(menu1RegisterSettigsCallback);                                                                                                                              \
    registerSettingsCallbackList.push_back(menu2RegisterSettigsCallback);                                                                                                                              \
    SettingsFileMock* settingsFileMock = new SettingsFileMock(defaultSettingsFile, defaultSettingsFileSize);                                                                                           \
    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock)

#define TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE                                                                                                                                                       \
    delete settingsStorage;                                                                                                                                                                            \
    delete settingsFileMock;                                                                                                                                                                           \
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T

static LinuxOSShim linuxOSShim;

TEST(SettingPermissions, OperatorOr)
{
    SettingPermissions_t permission1 = SettingPermissions_t::SYSTEM;
    SettingPermissions_t permission2 = SettingPermissions_t::ADMIN;
    SettingPermissions_t expected_result = static_cast<SettingPermissions_t>(static_cast<uint8_t>(permission1) | static_cast<uint8_t>(permission2));

    EXPECT_EQ(expected_result, permission1 | permission2);
}

TEST(SettingPermissions, OperatorAnd)
{
    SettingPermissions_t permission1 = SettingPermissions_t::SYSTEM;
    SettingPermissions_t permission2 = SettingPermissions_t::ADMIN;
    SettingPermissions_t expected_result = static_cast<SettingPermissions_t>(static_cast<uint8_t>(permission1) & static_cast<uint8_t>(permission2));

    EXPECT_EQ(expected_result, permission1 & permission2);
}

TEST(SettingPermissions, ToString_InvalidBuffer)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = nullptr;

    const char* result = settingPermissionToString(SettingPermissions_t::SYSTEM, nullptr, PERMISSION_STRING_SIZE);
    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("", buffer);
}

TEST(SettingPermissions, ToString_InvalidSize)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = nullptr;

    const char* result = settingPermissionToString(SettingPermissions_t::SYSTEM, buffer, PERMISSION_STRING_SIZE - 1);
    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("", buffer);
}

TEST(SettingPermissions, ToString_InvalidBufferAndSize)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = nullptr;

    const char* result = settingPermissionToString(SettingPermissions_t::SYSTEM, nullptr, PERMISSION_STRING_SIZE - 1);
    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("", buffer);
}

TEST(SettingPermissions, ToString_InvalidPermission)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = nullptr;

    const char* result = settingPermissionToString(static_cast<SettingPermissions_t>(1 + static_cast<uint8_t>(ALL_PERMISSIONS_VOLATILE)), buffer, PERMISSION_STRING_SIZE);
    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("", buffer);
}

TEST(SettingPermissions, ToString_System)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = "SYSTEM |       |      |         ";
    const char* result = settingPermissionToString(SettingPermissions_t::SYSTEM, buffer, PERMISSION_STRING_SIZE);

    EXPECT_NE(nullptr, result);
    EXPECT_STREQ(expected_result, buffer);
}

TEST(SettingPermissions, ToString_Admin)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = "       | ADMIN |      |         ";
    const char* result = settingPermissionToString(SettingPermissions_t::ADMIN, buffer, PERMISSION_STRING_SIZE);

    EXPECT_NE(nullptr, result);
    EXPECT_STREQ(expected_result, buffer);
}

TEST(SettingPermissions, ToString_User)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = "       |       | USER |         ";
    const char* result = settingPermissionToString(SettingPermissions_t::USER, buffer, PERMISSION_STRING_SIZE);

    EXPECT_NE(nullptr, result);
    EXPECT_STREQ(expected_result, buffer);
}

TEST(SettingPermissions, ToString_Volatile)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = "       |       |      | VOLATILE";
    const char* result = settingPermissionToString(SettingPermissions_t::VOLATILE, buffer, PERMISSION_STRING_SIZE);

    EXPECT_NE(nullptr, result);
    EXPECT_STREQ(expected_result, buffer);
}

TEST(SettingPermissions, ToString_All)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = "SYSTEM | ADMIN | USER |         ";
    const char* result = settingPermissionToString(ALL_PERMISSIONS, buffer, PERMISSION_STRING_SIZE);

    EXPECT_NE(nullptr, result);
    EXPECT_STREQ(expected_result, buffer);
}

TEST(SettingPermissions, ToString_All_Volatile)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = "SYSTEM | ADMIN | USER | VOLATILE";
    const char* result = settingPermissionToString(ALL_PERMISSIONS_VOLATILE, buffer, PERMISSION_STRING_SIZE);

    EXPECT_NE(nullptr, result);
    EXPECT_STREQ(expected_result, buffer);
}

TEST(SettingPermissions, ToString_None)
{
    char buffer[PERMISSION_STRING_SIZE] = "";
    const char* expected_result = "       |       |      |         ";
    const char* result = settingPermissionToString(NO_PERMISSIONS, buffer, PERMISSION_STRING_SIZE);

    EXPECT_NE(nullptr, result);
    EXPECT_STREQ(expected_result, buffer);
}

TEST(SettingPermissions, ValidatePermissions)
{
    EXPECT_TRUE(validatePermissions(SettingPermissions_t::SYSTEM));
    EXPECT_TRUE(validatePermissions(SettingPermissions_t::ADMIN));
    EXPECT_TRUE(validatePermissions(SettingPermissions_t::USER));
    EXPECT_TRUE(validatePermissions(SettingPermissions_t::VOLATILE));
    EXPECT_TRUE(validatePermissions(ALL_PERMISSIONS));
    EXPECT_TRUE(validatePermissions(ALL_PERMISSIONS_VOLATILE));
    EXPECT_TRUE(validatePermissions(NO_PERMISSIONS));
    EXPECT_FALSE(validatePermissions(static_cast<SettingPermissions_t>(1 + static_cast<uint8_t>(ALL_PERMISSIONS_VOLATILE))));
}

TEST(SettingsStorage, ConstructorPersistent)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    registerSettingsCallbackList.push_back(menu1RegisterSettigsCallback);
    SettingsFileMock* settingsFileMock = new SettingsFileMock(defaultSettingsFile, defaultSettingsFileSize);

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(SettingsFile::FileClosed, settingsFileMock->getOpenState());
    EXPECT_TRUE(settingsStorage->isPersistentStorageEnabled());

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, ConstructorPersistentNullCallbackIgnored)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    registerSettingsCallbackList.push_back(menu1RegisterSettigsCallback);
    registerSettingsCallbackList.push_back(nullptr);
    SettingsFileMock* settingsFileMock = new SettingsFileMock(defaultSettingsFile, defaultSettingsFileSize);

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(SettingsFile::FileClosed, settingsFileMock->getOpenState());
    EXPECT_TRUE(settingsStorage->isPersistentStorageEnabled());

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}


TEST(SettingsStorage, ConstructorNonPersistent)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList);

    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_FALSE(settingsStorage->isPersistentStorageEnabled());

    delete settingsStorage;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, ConstructorNonPersistentNullCallbackIgnored)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    registerSettingsCallbackList.push_back(nullptr);

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList);

    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_FALSE(settingsStorage->isPersistentStorageEnabled());

    delete settingsStorage;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, ConstructorFailSettingsFileSystemError)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock(defaultSettingsFile, defaultSettingsFileSize);

    settingsFileMock->_setForceMockMode(true);
    settingsFileMock->_setOpenForReadResult(SettingsFile::IOError);

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    EXPECT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);
    EXPECT_TRUE(settingsStorage->isPersistentStorageEnabled());

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, Destructor)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock(defaultSettingsFile, defaultSettingsFileSize);

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_TRUE(settingsStorage->isPersistentStorageEnabled());

    EXPECT_NO_THROW(delete settingsStorage);

    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, DisablePersistentStorage)
{
    NEW_POPULATED_SETTINGS_T(settings);

    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock(defaultSettingsFile, defaultSettingsFileSize);

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    // When
    ASSERT_TRUE(settingsStorage->disablePersistentStorage());

    // Then
    EXPECT_FALSE(settingsStorage->isPersistentStorageEnabled());

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
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
    result = settingsStorage->getSettingAsReal("menu1/setting1", outputValue, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsRealValidShort)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    double expectedValue = _valueSetting1.settingValueData.real;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getSettingAsReal("menu1/setting1", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsRealInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getSettingAsReal(nullptr, outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsRealVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getSettingAsReal("", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsRealKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getSettingAsReal("menu1/setting3", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsRealTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getSettingAsReal("menu1/setting2", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->getSettingAsInt("menu1/setting2", outputValue, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsIntValidShort)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    int64_t expectedValue = _valueSetting2.settingValueData.integer;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getSettingAsInt("menu1/setting2", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsIntInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getSettingAsInt(nullptr, outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsIntVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getSettingAsInt("", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsIntKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getSettingAsInt("menu1/setting3", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsIntTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getSettingAsInt("menu1/setting1", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueBuffer, outputValueSize, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValueBuffer);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsStringValidShort)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    const char* expectedValue = _valueSetting3.settingValueData.string;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ(expectedValue, outputValueBuffer);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsStringInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getSettingAsString(nullptr, outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsStringVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getSettingAsString("", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsStringKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getSettingAsString("menu2/setting4", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsStringTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getSettingAsString("menu1/setting1", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetSettingAsStringInsufficientBufferSize)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INSUFFICIENT_BUFFER_SIZE_ERROR;

    // When
    char outputValueBuffer[5];
    size_t outputValueSize = 5;
    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->putSettingValueAsInt(key, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    int64_t outputValue;
    result = settingsStorage->getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsIntInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    int64_t value = 12;

    // When
    result = settingsStorage->putSettingValueAsInt(nullptr, value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsIntVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    int64_t value = 12;

    // When
    result = settingsStorage->putSettingValueAsInt("", value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsIntKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;
    int64_t value = 12;

    // When
    result = settingsStorage->putSettingValueAsInt("menu1/setting3", value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->putSettingValueAsInt(key, value);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage->getSettingAsReal(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->putSettingValueAsReal(key, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    double outputValue;
    result = settingsStorage->getSettingAsReal(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsRealInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    double value = 12.34;

    // When
    result = settingsStorage->putSettingValueAsReal(nullptr, value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsRealVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    double value = 12.34;

    // When
    result = settingsStorage->putSettingValueAsReal("", value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsRealKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;
    double value = 12.34;

    // When
    result = settingsStorage->putSettingValueAsReal("menu1/setting3", value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->putSettingValueAsReal(key, value);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage->getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->putSettingValueAsString(key, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    char outputValueBuffer[12];
    size_t outputValueSize = 12;
    result = settingsStorage->getSettingAsString(key, outputValueBuffer, outputValueSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValueBuffer);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsStringInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    const char* value = "new string";

    // When
    result = settingsStorage->putSettingValueAsString(nullptr, value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsStringVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    const char* value = "new string";

    // When
    result = settingsStorage->putSettingValueAsString("", value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsStringInvalidValue)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    const char* key = "menu2/setting3";

    // When
    result = settingsStorage->putSettingValueAsString(key, nullptr);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, PutSettingValueAsStringKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;
    const char* value = "new string";

    // When
    result = settingsStorage->putSettingValueAsString("menu2/setting4", value);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->putSettingValueAsString(key, value);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage->getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->addSettingAsInt(key, expectedPermissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage->getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    expectedValue++;

    result = settingsStorage->putSettingValueAsInt(key, expectedValue); // Add a value to the key to see if the value is updated.
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    result = settingsStorage->getSettingAsInt(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsIntInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    int64_t expectedValue = 12;

    // When
    result = settingsStorage->addSettingAsInt(nullptr, permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsIntVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    int64_t expectedValue = 12;

    // When
    result = settingsStorage->addSettingAsInt("", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsIntKeyExists)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_EXISTS_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    int64_t expectedValue = 12;

    // When
    result = settingsStorage->addSettingAsInt("menu1/setting1", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsIntInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS_VOLATILE) + 1);
    int64_t expectedValue = 12;

    // When
    result = settingsStorage->addSettingAsInt("menu2/setting4", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->addSettingAsReal(key, expectedPermissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage->getSettingAsReal(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    expectedValue++;

    result = settingsStorage->putSettingValueAsReal(key, expectedValue); // Add a value to the key to see if the value is updated.
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    result = settingsStorage->getSettingAsReal(key, outputValue, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsRealInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    double expectedValue = 12.07;

    // When
    result = settingsStorage->addSettingAsReal(nullptr, permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsRealVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    double expectedValue = 12.07;

    // When
    result = settingsStorage->addSettingAsReal("", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsRealKeyExists)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_EXISTS_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    double expectedValue = 12.07;

    // When
    result = settingsStorage->addSettingAsReal("menu1/setting1", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsRealInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS_VOLATILE) + 1);
    double expectedValue = 12.07;

    // When
    result = settingsStorage->addSettingAsReal("menu2/setting4", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
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
    result = settingsStorage->addSettingAsString(key, expectedPermissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    result = settingsStorage->getSettingAsString(key, outputValue, outputBufferSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValue);

    expectedValue = "new string 2";

    result = settingsStorage->putSettingValueAsString(key, expectedValue); // Add a value to the key to see if the value is updated.
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    result = settingsStorage->getSettingAsString(key, outputValue, outputBufferSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsStringInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    const char* expectedValue = "new string";

    // When
    result = settingsStorage->addSettingAsString(nullptr, permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsStringInvalidDefaultValue)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;

    // When
    result = settingsStorage->addSettingAsString("menu1/setting3", permissions, nullptr);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsStringVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    const char* expectedValue = "new string";

    // When
    result = settingsStorage->addSettingAsString("", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsStringKeyExists)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_EXISTS_ERROR;
    SettingPermissions_t permissions = SettingPermissions_t::USER;
    const char* expectedValue = "new string";

    // When
    result = settingsStorage->addSettingAsString("menu1/setting1", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, AddSettingKeyAsStringInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;
    SettingPermissions_t permissions = static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS_VOLATILE) + 1);
    const char* expectedValue = "new string";

    // When
    result = settingsStorage->addSettingAsString("menu2/setting4", permissions, expectedValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysVoidKeyPrefix)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage->listSettingsKeys("", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAllPermissionsListedResultAny)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage->addSettingAsInt("menu1/setting3", ALL_PERMISSIONS, expectedValue);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage->listSettingsKeys("menu", ALL_PERMISSIONS, MatchSettingsWithAllPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();
    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAllPermissionsListedResultNone)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage->listSettingsKeys("menu", ALL_PERMISSIONS, MatchSettingsWithAllPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_EQ(outputKeys.end(), it);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAnyPermissionsListedResultAll)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage->addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage->listSettingsKeys("menu", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAnyPermissionsListedResultNone)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage->listSettingsKeys("menu", NO_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_EQ(outputKeys.end(), it);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidMatchSettingsWithAnyPermissionsListedResultMixed)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage->addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage->listSettingsKeys("menu", SettingPermissions_t::ADMIN | SettingPermissions_t::SYSTEM, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu1/setting3", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAllPermissionsListedResultAll)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage->listSettingsKeys("menu", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, ExcludeSettingsWithAllPermissionsListed, outputKeys);

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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAllPermissionsListedResultNone)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage->listSettingsKeys("menu", SettingPermissions_t::USER, ExcludeSettingsWithAllPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_EQ(outputKeys.end(), it);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAllPermissionsListedResultMixed)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage->addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage->listSettingsKeys("menu", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, ExcludeSettingsWithAllPermissionsListed, outputKeys);

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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAnyPermissionsListedResultMixed)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage->addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage->listSettingsKeys("menu", SettingPermissions_t::ADMIN, ExcludeSettingsWithAnyPermissionsListed, outputKeys);

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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAnyPermissionsListedResultAll)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage->addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage->listSettingsKeys("menu", SettingPermissions_t::SYSTEM, ExcludeSettingsWithAnyPermissionsListed, outputKeys);

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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}
TEST(SettingsStorage, listSettingsKeysValidExcludeSettingsWithAnyPermissionsListedResultNone)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    int64_t expectedValue = 12;

    result = settingsStorage->addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, expectedValue);
    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    // When
    result = settingsStorage->listSettingsKeys("menu", SettingPermissions_t::USER | SettingPermissions_t::SYSTEM, ExcludeSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    auto it = outputKeys.begin();

    ASSERT_EQ(outputKeys.end(), it);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysInvalidKeyPrefix)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    result = settingsStorage->listSettingsKeys(nullptr, ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    result = settingsStorage->listSettingsKeys("menu1", static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS_VOLATILE) + 1), MatchSettingsWithAnyPermissionsListed, outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, listSettingsKeysInvalidFilterMode)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    result = settingsStorage->listSettingsKeys("menu1", ALL_PERMISSIONS, static_cast<SettingPermissionsFilterMode_t>(-1), outputKeys);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}


TEST(SettingsStorage, listSettingsKeysEmptyPrefix)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingsKeysList_t outputKeys;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    result = settingsStorage->listSettingsKeys("", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys);

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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsValidAllFilterByKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::NO_ERROR;
    double expectedReal = 12.91;
    int64_t expectedInt = 12;
    const char* expectedString = "12,91";

    result = settingsStorage->putSettingValueAsReal("menu1/setting1", expectedReal);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsInt("menu1/setting2", expectedInt);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsString("menu2/setting3", expectedString);
    ASSERT_EQ(expectedResult, result);

    result = settingsStorage->restoreDefaultSettings("", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed);
    EXPECT_EQ(expectedResult, result);

    constexpr size_t outStringSize = 32;
    double outputValueReal;
    int64_t outputValueInt;
    char outputValueString[outStringSize];
    SettingPermissions_t outputPermissions;

    result = settingsStorage->getSettingAsReal("menu1/setting1", outputValueReal, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(_real1_default, outputValueReal);

    result = settingsStorage->getSettingAsInt("menu1/setting2", outputValueInt, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(_int2_default, outputValueInt);

    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueString, outStringSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_STREQ(_string3_default, outputValueString);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsValidSomeFilterByKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::NO_ERROR;
    double expectedReal = 12.91;
    int64_t expectedInt = 12;
    const char* expectedString = "12,91";

    result = settingsStorage->putSettingValueAsReal("menu1/setting1", expectedReal);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsInt("menu1/setting2", expectedInt);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsString("menu2/setting3", expectedString);
    ASSERT_EQ(expectedResult, result);

    result = settingsStorage->restoreDefaultSettings("menu1", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed);
    EXPECT_EQ(expectedResult, result);

    constexpr size_t outStringSize = 32;
    double outputValueReal;
    int64_t outputValueInt;
    char outputValueString[outStringSize];
    SettingPermissions_t outputPermissions;

    result = settingsStorage->getSettingAsReal("menu1/setting1", outputValueReal, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(_real1_default, outputValueReal);

    result = settingsStorage->getSettingAsInt("menu1/setting2", outputValueInt, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(_int2_default, outputValueInt);

    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueString, outStringSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_STREQ(expectedString, outputValueString);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsValidNoneFilterByKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::NO_ERROR;
    double expectedReal = 12.91;
    int64_t expectedInt = 12;
    const char* expectedString = "12,91";

    result = settingsStorage->putSettingValueAsReal("menu1/setting1", expectedReal);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsInt("menu1/setting2", expectedInt);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsString("menu2/setting3", expectedString);
    ASSERT_EQ(expectedResult, result);

    result = settingsStorage->restoreDefaultSettings("menu7", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed);
    EXPECT_EQ(expectedResult, result);

    constexpr size_t outStringSize = 32;
    double outputValueReal;
    int64_t outputValueInt;
    char outputValueString[outStringSize];
    SettingPermissions_t outputPermissions;

    result = settingsStorage->getSettingAsReal("menu1/setting1", outputValueReal, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedReal, outputValueReal);

    result = settingsStorage->getSettingAsInt("menu1/setting2", outputValueInt, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedInt, outputValueInt);

    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueString, outStringSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_STREQ(expectedString, outputValueString);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsValidAllFilterByPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::NO_ERROR;
    double expectedReal = 12.91;
    int64_t expectedInt = 12;
    const char* expectedString = "12,91";

    result = settingsStorage->putSettingValueAsReal("menu1/setting1", expectedReal);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsInt("menu1/setting2", expectedInt);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsString("menu2/setting3", expectedString);
    ASSERT_EQ(expectedResult, result);

    result = settingsStorage->restoreDefaultSettings("", SettingPermissions_t::SYSTEM, ExcludeSettingsWithAllPermissionsListed);
    EXPECT_EQ(expectedResult, result);

    constexpr size_t outStringSize = 32;
    double outputValueReal;
    int64_t outputValueInt;
    char outputValueString[outStringSize];
    SettingPermissions_t outputPermissions;

    result = settingsStorage->getSettingAsReal("menu1/setting1", outputValueReal, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(_real1_default, outputValueReal);

    result = settingsStorage->getSettingAsInt("menu1/setting2", outputValueInt, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(_int2_default, outputValueInt);

    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueString, outStringSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_STREQ(_string3_default, outputValueString);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsValidSomeFilterByPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::NO_ERROR;
    double expectedReal = 12.91;
    int64_t expectedInt = 12;
    const char* expectedString = "12,91";

    result = settingsStorage->addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::SYSTEM, _int2_default);
    ASSERT_EQ(expectedResult, result);

    result = settingsStorage->putSettingValueAsReal("menu1/setting1", expectedReal);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsInt("menu1/setting2", expectedInt);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsInt("menu1/setting3", expectedInt);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsString("menu2/setting3", expectedString);
    ASSERT_EQ(expectedResult, result);

    result = settingsStorage->restoreDefaultSettings("", SettingPermissions_t::USER, ExcludeSettingsWithAnyPermissionsListed);
    EXPECT_EQ(expectedResult, result);

    constexpr size_t outStringSize = 32;
    double outputValueReal;
    int64_t outputValueInt;
    char outputValueString[outStringSize];
    SettingPermissions_t outputPermissions;

    result = settingsStorage->getSettingAsReal("menu1/setting1", outputValueReal, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedReal, outputValueReal);

    result = settingsStorage->getSettingAsInt("menu1/setting2", outputValueInt, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedInt, outputValueInt);

    result = settingsStorage->getSettingAsInt("menu1/setting3", outputValueInt, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(_int2_default, outputValueInt);

    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueString, outStringSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_STREQ(expectedString, outputValueString);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsValidNoneFilterByPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::NO_ERROR;
    double expectedReal = 12.91;
    int64_t expectedInt = 12;
    const char* expectedString = "12,91";

    result = settingsStorage->addSettingAsInt("menu1/setting3", SettingPermissions_t::ADMIN | SettingPermissions_t::USER, _int2_default);
    ASSERT_EQ(expectedResult, result);

    result = settingsStorage->putSettingValueAsReal("menu1/setting1", expectedReal);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsInt("menu1/setting2", expectedInt);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsInt("menu1/setting3", expectedInt);
    ASSERT_EQ(expectedResult, result);
    result = settingsStorage->putSettingValueAsString("menu2/setting3", expectedString);
    ASSERT_EQ(expectedResult, result);

    result = settingsStorage->restoreDefaultSettings("", ALL_PERMISSIONS, MatchSettingsWithAllPermissionsListed);
    EXPECT_EQ(expectedResult, result);

    constexpr size_t outStringSize = 32;
    double outputValueReal;
    int64_t outputValueInt;
    char outputValueString[outStringSize];
    SettingPermissions_t outputPermissions;

    result = settingsStorage->getSettingAsReal("menu1/setting1", outputValueReal, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedReal, outputValueReal);

    result = settingsStorage->getSettingAsInt("menu1/setting2", outputValueInt, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedInt, outputValueInt);

    result = settingsStorage->getSettingAsInt("menu1/setting3", outputValueInt, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_EQ(expectedInt, outputValueInt);

    result = settingsStorage->getSettingAsString("menu2/setting3", outputValueString, outStringSize, &outputPermissions);
    EXPECT_EQ(SettingsStorage::NO_ERROR, result);
    EXPECT_STREQ(expectedString, outputValueString);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::INVALID_INPUT_ERROR;

    result = settingsStorage->restoreDefaultSettings(nullptr, ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed);
    EXPECT_EQ(expectedResult, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsInvalidPermissions)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::INVALID_INPUT_ERROR;

    result = settingsStorage->restoreDefaultSettings("menu1", static_cast<SettingPermissions_t>(static_cast<uint64_t>(ALL_PERMISSIONS_VOLATILE) + 1), MatchSettingsWithAnyPermissionsListed);
    EXPECT_EQ(expectedResult, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, restoreDefaultSettingsInvalidFilterMode)
{
    NEW_POPULATED_SETTINGS_STORAGE;
    SettingsStorage::SettingError_t expectedResult = SettingsStorage::INVALID_INPUT_ERROR;

    result = settingsStorage->restoreDefaultSettings("menu1", ALL_PERMISSIONS, static_cast<SettingPermissionsFilterMode_t>(-1));
    EXPECT_EQ(expectedResult, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsRealValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    double expectedValue = _valueSetting1.settingDefaultValueData.real;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting1.settingPermissions, outputPermissions;

    // When
    double outputValue;
    result = settingsStorage->getDefaultSettingAsReal("menu1/setting1", outputValue, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsRealValidShort)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    double expectedValue = _valueSetting1.settingDefaultValueData.real;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getDefaultSettingAsReal("menu1/setting1", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsRealInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getDefaultSettingAsReal(nullptr, outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsRealVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getDefaultSettingAsReal("", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsRealKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getDefaultSettingAsReal("menu1/setting3", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsRealTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    double outputValue;
    result = settingsStorage->getDefaultSettingAsReal("menu1/setting2", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsIntValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    int64_t expectedValue = _valueSetting2.settingDefaultValueData.integer;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting2.settingPermissions, outputPermissions;

    // When
    int64_t outputValue;
    result = settingsStorage->getDefaultSettingAsInt("menu1/setting2", outputValue, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsIntValidShort)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    int64_t expectedValue = _valueSetting2.settingDefaultValueData.integer;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getDefaultSettingAsInt("menu1/setting2", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedValue, outputValue);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsIntInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getDefaultSettingAsInt(nullptr, outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsIntVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getDefaultSettingAsInt("", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsIntKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getDefaultSettingAsInt("menu1/setting3", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsIntTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    int64_t outputValue;
    result = settingsStorage->getDefaultSettingAsInt("menu1/setting1", outputValue);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsStringValid)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    const char* expectedValue = _valueSetting3.settingDefaultValueData.string;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;
    SettingPermissions_t expectedPermissions = _valueSetting3.settingPermissions, outputPermissions;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getDefaultSettingAsString("menu2/setting3", outputValueBuffer, outputValueSize, &outputPermissions);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expectedPermissions, outputPermissions);
    EXPECT_STREQ(expectedValue, outputValueBuffer);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsStringValidShort)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    const char* expectedValue = _valueSetting3.settingDefaultValueData.string;
    SettingsStorage::SettingError_t expected_result = SettingsStorage::NO_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getDefaultSettingAsString("menu2/setting3", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ(expectedValue, outputValueBuffer);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsStringInvalidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getDefaultSettingAsString(nullptr, outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsStringVoidKey)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INVALID_INPUT_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getDefaultSettingAsString("", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsStringKeyNotFound)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::KEY_NOT_FOUND_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getDefaultSettingAsString("menu2/setting4", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsStringTypeMismatch)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::TYPE_MISMATCH_ERROR;

    // When
    char outputValueBuffer[10];
    size_t outputValueSize = 10;
    result = settingsStorage->getDefaultSettingAsString("menu1/setting1", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, GetDefaultSettingAsStringInsufficientBufferSize)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    // Want
    SettingsStorage::SettingError_t expected_result = SettingsStorage::INSUFFICIENT_BUFFER_SIZE_ERROR;

    // When
    char outputValueBuffer[5];
    size_t outputValueSize = 5;
    result = settingsStorage->getDefaultSettingAsString("menu2/setting3", outputValueBuffer, outputValueSize);

    // Then
    EXPECT_EQ(expected_result, result);

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageValidVolatile)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\n\r1874197929\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    SettingsStorage::SettingsKeysList_t outputKeys;
    EXPECT_EQ(SettingsStorage::NO_ERROR, settingsStorage->listSettingsKeys("", SettingPermissions_t::VOLATILE, MatchSettingsWithAnyPermissionsListed, outputKeys));

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

    outputKeys.clear();
    EXPECT_EQ(SettingsStorage::NO_ERROR, settingsStorage->listSettingsKeys("", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys));

    it = outputKeys.begin();
    ASSERT_EQ(outputKeys.end(), it);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageValidNoVolatile)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    ASSERT_EQ(SettingsStorage::NO_ERROR, result);

    SettingsStorage::SettingsKeysList_t outputKeys;
    EXPECT_EQ(SettingsStorage::NO_ERROR, settingsStorage->listSettingsKeys("", SettingPermissions_t::VOLATILE, MatchSettingsWithAnyPermissionsListed, outputKeys));

    auto it = outputKeys.begin();
    ASSERT_EQ(outputKeys.end(), it);

    outputKeys.clear();
    EXPECT_EQ(SettingsStorage::NO_ERROR, settingsStorage->listSettingsKeys("", ALL_PERMISSIONS, MatchSettingsWithAnyPermissionsListed, outputKeys));

    it = outputKeys.begin();
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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidEndNewLine)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\n\r1874197929");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageNoCRC)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageNoCRC2)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\n\r");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidCRC)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\n\r1874197929\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageDataAfterCRC)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\n\r1048123282\nmenu2/setting1\t0\t19.234\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidFormatOneTab)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting10\t1.23\n\r3431848188\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidFormatOneTab2)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t01.23\n\r3523440152\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidFormatNoTab)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting101.23\n\r1154240075\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageNoNewLine)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\r403323339\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageMisplacedNewLine)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.2\n3\n\r403323339\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidRealValue)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.2j3\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\n\r3024992554\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidIntValue)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45.7\nmenu2/setting3\t2\tstring3\n\r3375632971\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidValueTypeNum)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t3\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\n\r3588291006\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidValueTypeNegativeNum)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\nmenu1/setting2\t-1\t45\nmenu2/setting3\t2\tstring3\n\r1361869624\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidValueTypeChar)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\tl\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\n\r1759725303\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidValueTypeStringAsReal)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t0\tstring3\n\r1799368084\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidKeyReal)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("\t0\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t0\tstring3\n\r1141571301\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidKeyInt)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\n\t1\t45\nmenu2/setting3\t0\tstring3\n\r1061568119\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, loadSettingsFromPersistentStorageInvalidKeyString)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45\n\t0\tstring3\n\r664071405\n");

    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    ASSERT_EQ(SettingsStorage::SETTINGS_FILESYSTEM_ERROR, result);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, storeSettingsFromPersistentStorageValidVolatile)
{
    NEW_POPULATED_SETTINGS_T(settings);
    SettingsStorage::SettingError_t result;
    SettingsStorage::RegisterSettingsCallbackList_t registerSettingsCallbackList;
    registerSettingsCallbackList.push_back(menu1RegisterSettigsCallback);
    registerSettingsCallbackList.push_back(menu2RegisterSettigsCallback);
    SettingsFileMock* settingsFileMock = new SettingsFileMock("menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\nmenu3/setting4\t2\tstring4\n\r802044068\n", -1);
    SettingsStorage* settingsStorage = new SettingsStorage(result, linuxOSShim, registerSettingsCallbackList, settingsFileMock);

    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    EXPECT_EQ(SettingsStorage::NO_ERROR, settingsStorage->storeSettingsInPersistentStorage());
    EXPECT_STREQ("menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\n\r1874197929\n", settingsFileMock->_getInternalBuffer());

    SettingsStorage::SettingsKeysList_t outputKeys;
    EXPECT_EQ(SettingsStorage::NO_ERROR, settingsStorage->listSettingsKeys("", SettingPermissions_t::VOLATILE, MatchSettingsWithAnyPermissionsListed, outputKeys));

    auto it = outputKeys.begin();

    ASSERT_NE(outputKeys.end(), it);
    EXPECT_STREQ("menu3/setting4", it->c_str());
    ++it;

    ASSERT_EQ(outputKeys.end(), it);

    delete settingsStorage;
    delete settingsFileMock;
    TEAR_DOWN_NEW_POPULATED_SETTINGS_T;
}

TEST(SettingsStorage, storeSettingsFromPersistentStorageValidNoVolatile)
{
    NEW_POPULATED_SETTINGS_STORAGE;

    EXPECT_EQ(SettingsStorage::NO_ERROR, result);

    EXPECT_EQ(SettingsStorage::NO_ERROR, settingsStorage->storeSettingsInPersistentStorage());
    EXPECT_STREQ("menu1/setting1\t0\t1.23\nmenu1/setting2\t1\t45\nmenu2/setting3\t2\tstring3\n\r1874197929\n", settingsFileMock->_getInternalBuffer());

    SettingsStorage::SettingsKeysList_t outputKeys;
    EXPECT_EQ(SettingsStorage::NO_ERROR, settingsStorage->listSettingsKeys("", SettingPermissions_t::VOLATILE, ExcludeSettingsWithAnyPermissionsListed, outputKeys));

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

    TEAR_DOWN_NEW_POPULATED_SETTINGS_STORAGE;
}
