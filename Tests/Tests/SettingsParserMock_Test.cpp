#include "SettingsParserMock.h"
#include "gtest/gtest.h"

TEST(SettingsParserMock, Constructor)
{
    // Want
    const char* pathToSettingsFile = "path/to/settings/file";

    // Use
    SettingsParserMock settingsParserMock(pathToSettingsFile);

    // Test
    ASSERT_STREQ(pathToSettingsFile, settingsParserMock.getPathToSettingsFile());
}

TEST(SettingsParserMock, ReadSettingsFromPersistentStorage)
{
    // Want
    SettingsStorage::Settings_t settings;
    SettingsStorage::SettingValue_t value = {.settingValueType = SettingsStorage::INTEGER, .settingValueData = 56, .settingPermissions = SettingPermissions_t::SYSTEM};
    settings.insert("key", sizeof("key"), &value);
    SettingsParser::ParserError_t result = SettingsParser::ParserError_t::NO_ERROR;

    // Mock
    SettingsParserMock settingsParserMock("path/to/settings/file");
    settingsParserMock.setReadSettingsReturnValue(&settings);
    settingsParserMock.setReadSettingsReturnResult(result);

    // Use
    SettingsStorage::Settings_t* realSettings = settingsParserMock.readSettingsFromPersistentStorage(&result);

    // Test
    ASSERT_EQ(&settings, realSettings);
    ASSERT_EQ(result, SettingsParser::ParserError_t::NO_ERROR);
}

TEST(SettingsParserMock, WriteSettingsToPersistentStorage)
{
    // Want
    SettingsStorage::Settings_t settings;
    SettingsStorage::SettingValue_t value = {.settingValueType = SettingsStorage::INTEGER, .settingValueData = 56, .settingPermissions = SettingPermissions_t::SYSTEM};
    settings.insert("key", sizeof("key"), &value);
    SettingsParser::ParserError_t result = SettingsParser::ParserError_t::NO_ERROR;

    // Mock
    SettingsParserMock settingsParserMock("path/to/settings/file");
    settingsParserMock.setWriteSettingsReturnResult(result);

    // Use
    SettingsParser::ParserError_t realResult = settingsParserMock.writeSettingsToPersistentStorage(settings);

    // Test
    ASSERT_EQ(result, realResult);
    ASSERT_EQ(&settings, &settingsParserMock.getWriteSettingsArgument());
}
