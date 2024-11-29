#ifndef SETTINGSSTORAGE_SETTINGSPARSER_H
#define SETTINGSSTORAGE_SETTINGSPARSER_H

#include "SettingsStorage.h"

class SettingsParser
{
public:
    typedef enum {NO_ERROR = 0, } ParserError_t;

    /**
     * @brief Construct a new Settings Parser object
     *
     * @param pathToSettingsFile The path to the settings file to parse.
     */
    explicit SettingsParser(const char* pathToSettingsFile);

    /**
     * @brief Destroy the Settings Parser object
     */
    ~SettingsParser();

    /**
     * @brief Read the settings from the provided settings file.
     *
     * @param result The result of the operation.
     * @return SettingsStorage::Settings_t* A pointer to the settings read from the file or nullptr if the file is corrupted.
     */
    SettingsStorage::Settings_t* readSettingsFromPersistentStorage(ParserError_t* result);

    /**
     * @brief Write the settings to the provided settings file.
     *
     * @param settings The settings to write to the file.
     * @return ParserError_t The result of the operation.
     */
    ParserError_t writeSettingsToPersistentStorage(SettingsStorage::Settings_t& settings);
};

#endif // SETTINGSSTORAGE_SETTINGSPARSER_H