#include "SettingsParser.h"
#include "SettingsStorage.h"

SettingsParser::SettingsParser(const char* pathToSettingsFile) {}
SettingsParser::~SettingsParser() {}
SettingsStorage::Settings_t* SettingsParser::readSettingsFromPersistentStorage(ParserError_t* result) const {}
SettingsParser::ParserError_t SettingsParser::writeSettingsToPersistentStorage(SettingsStorage::Settings_t& settings) {}
