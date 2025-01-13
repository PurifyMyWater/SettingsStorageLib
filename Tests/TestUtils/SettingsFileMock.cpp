//
// Created by victor on 8/01/25.
//

#include "SettingsFileMock.h"

#include <cassert>
#include <cstdlib>
#include <cstring>

SettingsFileMock::SettingsFileMock(const char* internalBuffer, int64_t internalBufferSize)
{
    assert(internalBuffer != nullptr && "Internal buffer is nullptr");
    this->internalBufferSize = static_cast<int64_t>(strlen(internalBuffer)) + 1;
    if (internalBufferSize < 0)
    {
        this->internalBuffer = strdup(internalBuffer);
    }
    else if (internalBufferSize >= this->internalBufferSize)
    {
        this->internalBuffer = static_cast<char*>(malloc(internalBufferSize));
        this->internalBufferSize = internalBufferSize;
        strcpy(this->internalBuffer, internalBuffer);
    }
    else
    {
        assert(false && "Too small internal buffer size");
    }
    assert(this->internalBuffer != nullptr && "Memory allocation failed");
    this->fileDataSize = static_cast<uint32_t>(strlen(internalBuffer));
    this->fileDataIndex = 0;
    this->fileStatus = FileClosed;

    fullMockEnabled = false;
    readResult = Success;
    readOutput = 'a';
    readLineResult = Success;
    strcpy(readLineOutput, "a");
    writeResult = Success;
    writeBufferResult = Success;
    openForReadResult = Success;
    openForWriteResult = Success;
    closeResult = Success;
}

SettingsFile::SettingsFileResult SettingsFileMock::read(char* byte)
{
    if (fullMockEnabled)
    {
        *byte = readOutput;
        return readResult;
    }

    if (this->fileStatus != FileOpenedForRead)
    {
        return InvalidState;
    }

    if (this->fileDataIndex >= this->fileDataSize)
    {
        return EndOfFile;
    }

    *byte = this->internalBuffer[this->fileDataIndex++];
    return Success;
}

SettingsFile::SettingsFileResult SettingsFileMock::readLine(char* buffer, uint32_t bufferSize)
{
    if (fullMockEnabled)
    {
        strcpy(buffer, readLineOutput);
        return readLineResult;
    }

    if (this->fileStatus != FileOpenedForRead || buffer == nullptr || bufferSize == 0)
    {
        return InvalidState;
    }

    if (this->fileDataIndex >= this->fileDataSize)
    {
        return EndOfFile;
    }

    const uint32_t maxSize = bufferSize -1; // Leave space for the last character
    char c = internalBuffer[fileDataIndex++];
    int64_t i;
    for (i = 0; i < maxSize && fileDataIndex <= fileDataSize && c != '\n' && c != '\0'; i++, c = internalBuffer[fileDataIndex++])
    {
        buffer[i] = c;
    }
    buffer[i] = c;
    buffer[i + 1] = '\0';
    return Success;
}

SettingsFile::SettingsFileResult SettingsFileMock::write(char byte)
{
    if (fullMockEnabled)
    {
        return writeResult;
    }

    if (this->fileStatus != FileOpenedForWrite)
    {
        return InvalidState;
    }

    if (this->fileDataIndex >= this->internalBufferSize - 1)
    {
        return EndOfFile;
    }

    this->internalBuffer[this->fileDataIndex++] = byte;
    this->internalBuffer[this->fileDataIndex] = '\0';

    return Success;
}

SettingsFile::SettingsFileResult SettingsFileMock::write(const char* data, uint32_t dataSize)
{
    if (fullMockEnabled)
    {
        return writeBufferResult;
    }

    if (this->fileStatus != FileOpenedForWrite || data == nullptr || dataSize == 0)
    {
        return InvalidState;
    }

    for (int64_t i = 0; i < dataSize; i++)
    {
        if (this->fileDataIndex >= this->internalBufferSize - 1)
        {
            return EndOfFile;
        }

        this->internalBuffer[this->fileDataIndex++] = data[i];
    }
    this->internalBuffer[this->fileDataIndex] = '\0';

    return Success;
}

SettingsFile::SettingsFileResult SettingsFileMock::openForRead()
{
    if (fullMockEnabled)
    {
        return openForReadResult;
    }

    if (this->fileStatus != FileClosed)
    {
        return InvalidState;
    }

    this->fileStatus = FileOpenedForRead;
    this->fileDataIndex = 0;
    this->fileDataSize = static_cast<uint32_t>(strlen(this->internalBuffer));
    return Success;
}

SettingsFile::SettingsFileResult SettingsFileMock::openForWrite()
{
    if (fullMockEnabled)
    {
        return openForWriteResult;
    }

    if (this->fileStatus != FileClosed)
    {
        return InvalidState;
    }

    this->fileStatus = FileOpenedForWrite;
    this->fileDataIndex = 0;
    this->internalBuffer[this->fileDataIndex] = '\0';
    return Success;
}

SettingsFile::SettingsFileResult SettingsFileMock::close()
{
    if (fullMockEnabled)
    {
        return closeResult;
    }

    if (this->fileStatus == FileClosed)
    {
        return InvalidState;
    }

    if (this->fileStatus == FileOpenedForWrite)
    {
        this->internalBuffer[this->fileDataIndex] = '\0';
    }
    fileStatus = FileClosed;
    return Success;
}

SettingsFile::FileStatus SettingsFileMock::getOpenState() { return this->fileStatus; }

char* SettingsFileMock::_getInternalBuffer() const { return this->internalBuffer; }

void SettingsFileMock::_setForceMockMode(bool fullMockEnabled) { this->fullMockEnabled = fullMockEnabled; }

void SettingsFileMock::_setReadResult(SettingsFileResult result) { this->readResult = result; }

void SettingsFileMock::_setReadOutput(char byte) { this->readOutput = byte; }

void SettingsFileMock::_setReadLineResult(SettingsFileResult result) { this->readLineResult = result; }

void SettingsFileMock::_setReadLineOutput(const char* line) { strcpy(this->readLineOutput, line); }

void SettingsFileMock::_setWriteResult(SettingsFileResult result) { this->writeResult = result; }

void SettingsFileMock::_setWriteBufferResult(SettingsFileResult result) { this->writeBufferResult = result; }

void SettingsFileMock::_setOpenForReadResult(SettingsFileResult result) { this->openForReadResult = result; }

void SettingsFileMock::_setOpenForWriteResult(SettingsFileResult result) { this->openForWriteResult = result; }

void SettingsFileMock::_setCloseResult(SettingsFileResult result) { this->closeResult = result; }
