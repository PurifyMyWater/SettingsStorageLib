#ifndef SETTINGSFILE_H
#define SETTINGSFILE_H

#include <cstdint>

/**
 * @brief This class is an interface for a settings file.
 * It allows reading and writing to a settings file.
 * The user of this library must implement this class to use the settings storage.
 * It represents a specific file in the file system. To allow the system to select a specific file, the implementation may use a constructor with a filename in it.
 */
class SettingsFile
{
public:
    using SettingsFileResult = enum { EndOfFile = -1, Success = 0, InvalidState, IOError};
    using FileStatus = enum { FileClosed = 0, FileOpenedForRead, FileOpenedForWrite };

    [[nodiscard]] virtual SettingsFileResult openForRead() = 0;
    [[nodiscard]] virtual SettingsFileResult read(char* byte) = 0;
    [[nodiscard]] virtual SettingsFileResult readLine(char* buffer, uint32_t bufferSize) = 0;

    [[nodiscard]] virtual SettingsFileResult openForWrite() = 0;
    [[nodiscard]] virtual SettingsFileResult write(char byte) = 0;
    [[nodiscard]] virtual SettingsFileResult write(const char* data, uint32_t dataSize) = 0;

    [[nodiscard]] virtual SettingsFileResult close() = 0;
    [[nodiscard]] virtual FileStatus getOpenState() = 0;

    virtual ~SettingsFile() = default;
};

#endif //SETTINGSFILE_H
