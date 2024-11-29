#include "SettingsParserMock.h"

SettingsParserMock::SettingsParserMock(const char* pathToSettingsFile) : SettingsParser(pathToSettingsFile) { this->pathToSettingsFile = pathToSettingsFile; }

const char* SettingsParserMock::getPathToSettingsFile() const { return pathToSettingsFile; }

void SettingsParserMock::setReadSettingsReturnValue(SettingsStorage::Settings_t* settings) { readSettingsReturnValue = settings; }

void SettingsParserMock::setReadSettingsReturnResult(ParserError_t result) { readSettingsReturnResult = result; }

SettingsStorage::Settings_t* SettingsParserMock::readSettingsFromPersistentStorage(ParserError_t* result) const
{
    *result = readSettingsReturnResult;
    return readSettingsReturnValue;
}

void SettingsParserMock::setWriteSettingsReturnResult(ParserError_t result) { writeSettingsReturnResult = result; }

SettingsStorage::Settings_t& SettingsParserMock::getWriteSettingsArgument() const { return *writeSettingsArgument; }

SettingsParser::ParserError_t SettingsParserMock::writeSettingsToPersistentStorage(SettingsStorage::Settings_t& settings)
{
    writeSettingsArgument = &settings;
    return writeSettingsReturnResult;
}
