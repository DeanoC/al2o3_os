#include "al2o3_platform/platform.h"
#include "al2o3_catch2/catch2.hpp"
#include "al2o3_os/file.h"

TEST_CASE("Read Testing 1, 2, 3 text file (C)", "[OS File]") {
	{
  Os_FileHandle fh = Os_FileOpen("test.txt", Os_FM_Write);
  REQUIRE(fh != NULL);
  static char expectedBytes[] = "Testing 1, 2, 3";
  size_t bytesWritten = Os_FileWrite(fh, expectedBytes, strlen(expectedBytes));
  bool closeOk = Os_FileClose(fh);
  REQUIRE(closeOk);
	}

	Os_FileHandle fh = Os_FileOpen("test.txt", Os_FM_ReadBinary);
  REQUIRE(fh != NULL);
	REQUIRE(Os_FileIsOpen(fh));

  static char expectedBytes[] = "Testing 1, 2, 3";
  char buffer[1024];
  size_t bytesRead = Os_FileRead(fh, buffer, 1024);
  REQUIRE(bytesRead == 16);
  REQUIRE(strcmp(expectedBytes, buffer) == 0);

  bool closeOk = Os_FileClose(fh);
  REQUIRE(closeOk);
}

TEST_CASE("Open and close (C)", "[OS File]") {
  Os_FileHandle fh = Os_FileOpen("test.txt", Os_FM_Read);
  REQUIRE(fh != NULL);
  REQUIRE(Os_FileIsOpen(fh));
  bool closeOk = Os_FileClose(fh);
  REQUIRE(closeOk);
}

TEST_CASE("Write Testing 1, 2, 3 text file (C)", "[OS File]") {

  Os_FileHandle fh = Os_FileOpen("test2.txt", Os_FM_Write);
  REQUIRE(fh != NULL);

  static char expectedBytes[] = "Testing 1, 2, 3";

  size_t bytesWritten = Os_FileWrite(fh, expectedBytes, strlen(expectedBytes));
  REQUIRE(bytesWritten == strlen(expectedBytes));

  // there not really an easy way of testing flush so we test it doesn't crash
  Os_FileFlush(fh);

  bool closeWriteOk = Os_FileClose(fh);
  REQUIRE(closeWriteOk);


  // verify write
  Os_FileHandle fhr = Os_FileOpen("test2.txt", Os_FM_Read);
  REQUIRE(fhr != NULL);
  char buffer[1024];
  size_t bytesRead = Os_FileRead(fhr, buffer, 1024);
  buffer[bytesRead] = 0;
  REQUIRE(bytesRead == strlen(expectedBytes));
  REQUIRE(strcmp(expectedBytes, buffer) == 0);

  bool closeReadOk = Os_FileClose(fhr);
  REQUIRE(closeReadOk);
}

TEST_CASE("Seek & Tell Testing 1, 2, 3 text file (C)", "[OS File]") {

  Os_FileHandle fh = Os_FileOpen("test.txt", Os_FM_ReadBinary);
  REQUIRE(fh != NULL);

  char buffer[1024];

  bool seek0 = Os_FileSeek(fh, 4, Os_FSD_Begin);
  REQUIRE(seek0);
  REQUIRE(Os_FileTell(fh) == 4);
  size_t bytesRead0 = Os_FileRead(fh, buffer, 1024);
  REQUIRE(bytesRead0 == 12);
  REQUIRE(Os_FileTell(fh) == 16);

  Os_FileSeek(fh, 4, Os_FSD_Begin);
  bool seek1 = Os_FileSeek(fh, 4, Os_FSD_Current);
  REQUIRE(seek1);
  REQUIRE(Os_FileTell(fh) == 8);
  size_t bytesRead1 = Os_FileRead(fh, buffer, 1024);
  REQUIRE(bytesRead1 == 8);
  REQUIRE(Os_FileTell(fh) == 16);

  bool seek2 = Os_FileSeek(fh, -4, Os_FSD_End);
  REQUIRE(seek2);
  REQUIRE(Os_FileTell(fh) == 12);
  size_t bytesRead2 = Os_FileRead(fh, buffer, 1024);
  REQUIRE(bytesRead2 == 4);
  REQUIRE(Os_FileTell(fh) == 16);

  bool closeOk = Os_FileClose(fh);
  REQUIRE(closeOk);
}

TEST_CASE("Size (C)", "[OS File]") {

  Os_FileHandle fh = Os_FileOpen("test.txt", Os_FM_ReadBinary);
  REQUIRE(fh != NULL);

  size_t size = Os_FileSize(fh);
  REQUIRE(size == 16);

  bool closeOk = Os_FileClose(fh);
  REQUIRE(closeOk);
}
