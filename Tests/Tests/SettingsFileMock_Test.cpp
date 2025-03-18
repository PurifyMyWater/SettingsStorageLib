#include "SettingsFileMock.h"
#include "gtest/gtest.h"

TEST(SettingsFileMock, ConstructorAuto)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    EXPECT_STREQ(expected_internalBuffer, settingsFileMock._getInternalBuffer());
}

TEST(SettingsFileMock, ConstructorManualOk)
{
    const char*   expected_internalBuffer     = "internal buffer";
    const int64_t expected_internalBufferSize = static_cast<int64_t>(strlen(expected_internalBuffer)) + 30;

    SettingsFileMock settingsFileMock(expected_internalBuffer, expected_internalBufferSize);

    EXPECT_STREQ(expected_internalBuffer, settingsFileMock._getInternalBuffer());
}

TEST(SettingsFileMock, OpenForReadOkFromConstructor)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    SettingsFile::SettingsFileResult result          = settingsFileMock.openForRead();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, OpenForReadOkFromClosed)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForRead();
    settingsFileMock.close();
    SettingsFile::SettingsFileResult result = settingsFileMock.openForRead();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, OpenForReadFailFromOpenForRead)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForRead();
    SettingsFile::SettingsFileResult result = settingsFileMock.openForRead();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, OpenForReadFailFromOpenForWrite)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForWrite();
    SettingsFile::SettingsFileResult result = settingsFileMock.openForRead();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, OpenForWriteOkFromConstructor)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    SettingsFile::SettingsFileResult result          = settingsFileMock.openForWrite();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, OpenForWriteOkFromClosed)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForWrite();
    settingsFileMock.close();
    SettingsFile::SettingsFileResult result = settingsFileMock.openForWrite();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, OpenForWriteFailFromOpenForRead)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForRead();
    SettingsFile::SettingsFileResult result = settingsFileMock.openForWrite();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, OpenForWriteFailFromOpenForWrite)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForWrite();
    SettingsFile::SettingsFileResult result = settingsFileMock.openForWrite();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, CloseOkFromRead)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForRead();
    SettingsFile::SettingsFileResult result = settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, CloseOkFromWrite)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForWrite();
    SettingsFile::SettingsFileResult result = settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, CloseFailFromConstructor)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    SettingsFile::SettingsFileResult result          = settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, CloseFailFromClosed)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForRead();
    settingsFileMock.close();
    SettingsFile::SettingsFileResult result = settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, ReadOk)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForRead();
    char                             byte;
    SettingsFile::SettingsFileResult result = settingsFileMock.read(&byte);

    EXPECT_EQ(expected_result, result);
    EXPECT_EQ(expected_internalBuffer[0], byte);
}

TEST(SettingsFileMock, ReadFailFromConstructor)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    char                             byte;
    SettingsFile::SettingsFileResult result = settingsFileMock.read(&byte);

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, ReadFailFromClosed)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForRead();
    settingsFileMock.close();
    char                             byte;
    SettingsFile::SettingsFileResult result = settingsFileMock.read(&byte);

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, ReadFailFromEndOfFile)
{
    const char* expected_internalBuffer = "in";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::EndOfFile;
    settingsFileMock.openForRead();
    char byte;

    SettingsFile::SettingsFileResult result = settingsFileMock.read(&byte);
    EXPECT_EQ(SettingsFile::Success, result);
    EXPECT_EQ(expected_internalBuffer[0], byte);
    result = settingsFileMock.read(&byte);
    EXPECT_EQ(SettingsFile::Success, result);
    EXPECT_EQ(expected_internalBuffer[1], byte);

    result = settingsFileMock.read(&byte);
    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, ReadLineOk)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForRead();
    std::string                      buffer;
    SettingsFile::SettingsFileResult result = settingsFileMock.readLine(buffer);

    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ(expected_internalBuffer, buffer.c_str());
}

TEST(SettingsFileMock, ReadLineOkNewLine)
{
    const char* expected_internalBuffer = "internal buffer\n";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForRead();
    std::string                      buffer;
    SettingsFile::SettingsFileResult result = settingsFileMock.readLine(buffer);

    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ(expected_internalBuffer, buffer.c_str());
}

TEST(SettingsFileMock, ReadLineFailFromConstructor)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    std::string                      buffer;
    SettingsFile::SettingsFileResult result = settingsFileMock.readLine(buffer);

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, ReadLineFailFromClosed)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForRead();
    settingsFileMock.close();
    std::string                      buffer;
    SettingsFile::SettingsFileResult result = settingsFileMock.readLine(buffer);

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, ReadLineFailFromEndOfFile)
{
    const char* expected_internalBuffer = "internal buffer\nbuffer internal";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForRead();
    std::string                      buffer;
    SettingsFile::SettingsFileResult result = settingsFileMock.readLine(buffer);

    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("internal buffer\n", buffer.c_str());

    buffer.clear();
    result = settingsFileMock.readLine(buffer);

    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("buffer internal", buffer.c_str());

    result = settingsFileMock.readLine(buffer);

    EXPECT_EQ(SettingsFile::EndOfFile, result);
}

TEST(SettingsFileMock, WriteOk)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForWrite();
    SettingsFile::SettingsFileResult result = settingsFileMock.write('a');
    settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("a", settingsFileMock._getInternalBuffer());
}

TEST(SettingsFileMock, WriteFailFromConstructor)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    SettingsFile::SettingsFileResult result          = settingsFileMock.write('a');

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, WriteFailFromClosed)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForWrite();
    settingsFileMock.close();
    SettingsFile::SettingsFileResult result = settingsFileMock.write('a');

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, WriteBuffferOk)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock.openForWrite();
    SettingsFile::SettingsFileResult result = settingsFileMock.write("abc");
    settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("abc", settingsFileMock._getInternalBuffer());
}

TEST(SettingsFileMock, WriteBuffferFailFromConstructor)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    SettingsFile::SettingsFileResult result          = settingsFileMock.write("abc");

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, WriteBuffferFailFromClosed)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::InvalidState;
    settingsFileMock.openForWrite();
    settingsFileMock.close();
    SettingsFile::SettingsFileResult result = settingsFileMock.write("abc");

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, FullMockOpenForRead)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);
    settingsFileMock._setForceMockMode(true);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock._setOpenForReadResult(expected_result);
    SettingsFile::SettingsFileResult result = settingsFileMock.openForRead();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, FullMockOpenForWrite)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);
    settingsFileMock._setForceMockMode(true);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock._setOpenForWriteResult(expected_result);
    SettingsFile::SettingsFileResult result = settingsFileMock.openForWrite();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, FullMockClose)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);
    settingsFileMock._setForceMockMode(true);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock._setCloseResult(expected_result);
    settingsFileMock.openForRead();
    SettingsFile::SettingsFileResult result = settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, FullMockRead)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);
    settingsFileMock._setForceMockMode(true);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock._setReadResult(expected_result);
    settingsFileMock._setReadOutput('a');
    settingsFileMock.openForRead();
    char                             byte;
    SettingsFile::SettingsFileResult result = settingsFileMock.read(&byte);

    EXPECT_EQ(expected_result, result);
    EXPECT_EQ('a', byte);
}

TEST(SettingsFileMock, FullMockReadLine)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);
    settingsFileMock._setForceMockMode(true);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock._setReadLineResult(expected_result);
    settingsFileMock._setReadLineOutput("line");
    settingsFileMock.openForRead();
    std::string                      buffer;
    SettingsFile::SettingsFileResult result = settingsFileMock.readLine(buffer);

    EXPECT_EQ(expected_result, result);
    EXPECT_STREQ("line", buffer.c_str());
}

TEST(SettingsFileMock, FullMockWrite)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);
    settingsFileMock._setForceMockMode(true);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock._setWriteResult(expected_result);
    settingsFileMock.openForWrite();
    SettingsFile::SettingsFileResult result = settingsFileMock.write('a');
    settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, FullMockWriteBuffer)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);
    settingsFileMock._setForceMockMode(true);

    SettingsFile::SettingsFileResult expected_result = SettingsFile::Success;
    settingsFileMock._setWriteBufferResult(expected_result);
    settingsFileMock.openForWrite();
    SettingsFile::SettingsFileResult result = settingsFileMock.write("abc");
    settingsFileMock.close();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, getOpenStateConstructor)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::FileStatus expected_result = SettingsFile::FileClosed;
    SettingsFile::FileStatus result          = settingsFileMock.getOpenState();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, getOpenStateOpenForRead)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::FileStatus expected_result = SettingsFile::FileOpenedForRead;
    settingsFileMock.openForRead();
    SettingsFile::FileStatus result = settingsFileMock.getOpenState();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, getOpenStateOpenForWrite)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::FileStatus expected_result = SettingsFile::FileOpenedForWrite;
    settingsFileMock.openForWrite();
    SettingsFile::FileStatus result = settingsFileMock.getOpenState();

    EXPECT_EQ(expected_result, result);
}

TEST(SettingsFileMock, getOpenStateClosed)
{
    const char* expected_internalBuffer = "internal buffer";

    SettingsFileMock settingsFileMock(expected_internalBuffer);

    SettingsFile::FileStatus expected_result = SettingsFile::FileClosed;
    settingsFileMock.openForRead();
    settingsFileMock.close();
    SettingsFile::FileStatus result = settingsFileMock.getOpenState();

    EXPECT_EQ(expected_result, result);
}
