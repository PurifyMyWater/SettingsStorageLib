#ifndef SETTINGSFILE_H
#define SETTINGSFILE_H

#include <cstdint>
#include <string>

/**
 * @brief This class is an interface for a settings file.
 * It allows reading and writing to a settings file.
 * The user of this library must implement this class to use the settings storage.
 * It represents a specific file in the file system. To allow the system to select a specific file, the implementation may use a constructor with a filename in it.
 */
class SettingsFile
{
public:
    using SettingsFileResult = enum { EndOfFile = -1, Success = 0, InvalidState, IOError };
    using FileStatus = enum { FileClosed = 0, FileOpenedForRead, FileOpenedForWrite };

    /**
     * @brief Open the file for reading.
     * @return SettingsFileResult The result of the operation.
     * @retval Success The file was successfully opened.
     * @retval InvalidState The file is already open.
     */
    [[nodiscard]] virtual SettingsFileResult openForRead() = 0;

    /**
     * @brief Read a byte from the file.
     * @param byte The byte read from the file.
     * @return SettingsFileResult The result of the operation.
     * @retval EndOfFile The end of the file was reached.
     * @retval Success The byte was successfully read.
     * @retval InvalidState The file is not open.
     * @retval IOError An error occurred while reading the file.
     */
    [[nodiscard]] virtual SettingsFileResult read(char* byte) = 0;

    /**
     * @brief Read a line from the file. (Including the newline character '\n')
     * @param buffer The buffer to store the line read from the file.
     * @return SettingsFileResult The result of the operation.
     * @retval EndOfFile The end of the file was reached, and all the remaining data is stored in the buffer.
     * @retval Success The line was successfully read.
     * @retval InvalidState The file is not open.
     * @retval IOError An error occurred while reading the file.
     */
    [[nodiscard]] virtual SettingsFileResult readLine(std::string& buffer) = 0;

    /**
     * @brief Open the file for writing.
     * @return SettingsFileResult The result of the operation.
     * @retval Success The file was successfully opened.
     * @retval InvalidState The file is already open.
     */
    [[nodiscard]] virtual SettingsFileResult openForWrite() = 0;

    /**
     * @brief Write a byte to the file.
     * @param byte The byte to write to the file.
     * @return SettingsFileResult The result of the operation.
     * @retval Success The byte was successfully written.
     * @retval InvalidState The file is not open.
     * @retval IOError An error occurred while writing the file.
     */
    [[nodiscard]] virtual SettingsFileResult write(char byte) = 0;

    /**
     * @brief Write a buffer to the file.
     * @param data The buffer to write to the file.
     * @return SettingsFileResult The result of the operation.
     * @retval Success The buffer was successfully written.
     * @retval InvalidState The file is not open.
     * @retval IOError An error occurred while writing the file.
     */
    [[nodiscard]] virtual SettingsFileResult write(const std::string& data) = 0;

    /**
     * @brief Close the file.
     * @return SettingsFileResult The result of the operation.
     * @retval Success The file was successfully closed.
     * @retval InvalidState The file is not open.
     */
    [[nodiscard]] virtual SettingsFileResult close() = 0;

    /**
     * @brief Get the open state of the file.
     * @return FileStatus The open state of the file.
     */
    [[nodiscard]] virtual FileStatus getOpenState() = 0;

    virtual ~SettingsFile() = default;
};

#endif // SETTINGSFILE_H
