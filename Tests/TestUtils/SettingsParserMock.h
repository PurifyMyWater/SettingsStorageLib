#ifndef SETTINGSPARSERMOCK_H
#define SETTINGSPARSERMOCK_H

#include "SettingsParser.h"

class SettingsParserMock : public SettingsParser
{
public:
    explicit SettingsParserMock(const char* pathToSettingsFile);
    void setReadSettingsReturnValue(SettingsStorage::Settings_t* settings);
    void setReadSettingsReturnResult(ParserError_t result);
    void setWriteSettingsReturnResult(ParserError_t result);
    [[nodiscard]] const char* getPathToSettingsFile() const;
    [[nodiscard]] SettingsStorage::Settings_t& getWriteSettingsArgument() const;

    SettingsStorage::Settings_t* readSettingsFromPersistentStorage(ParserError_t* result) const override;

    ParserError_t writeSettingsToPersistentStorage(SettingsStorage::Settings_t& settings) override;

private:
    SettingsStorage::Settings_t* readSettingsReturnValue = nullptr;
    ParserError_t readSettingsReturnResult = NO_ERROR;
    ParserError_t writeSettingsReturnResult = NO_ERROR;
    SettingsStorage::Settings_t* writeSettingsArgument = nullptr;
    const char* pathToSettingsFile;
};

#endif // SETTINGSPARSERMOCK_H
