#include "al2o3_platform/platform.h"
#include "al2o3_os/filesystem.hpp"
#include "al2o3_tinystl/vector.hpp"

#include <unistd.h>       // getcwd
#include <errno.h>        // errno
#include <sys/stat.h>     // stat
#include <stdio.h>        // remove
#include <dirent.h>        // directory functions

// internal and platform path are the same on posix
AL2O3_EXTERN_C bool Os_IsNormalisedPath(char const *path) {
	return true;
}

AL2O3_EXTERN_C bool Os_GetNormalisedPathFromPlatformPath(char const *path, char *pathOut, size_t maxSize) {
	// just copy
	if (strlen(path) >= maxSize) {
		return false;
	}
	strcpy(pathOut, path);
	return true;
}

AL2O3_EXTERN_C bool Os_GetPlatformPathFromNormalisedPath(char const *path, char *pathOut, size_t maxSize) {
	// just copy
	if (strlen(path) >= maxSize) {
		return false;
	}
	strcpy(pathOut, path);
	return true;
}

AL2O3_EXTERN_C size_t Os_GetLastModifiedTime(const char *fileName) {
	struct stat fileInfo;

	if (!stat(fileName, &fileInfo)) {
		return (size_t) fileInfo.st_mtime;
	} else {
		// return an impossible large mod time as the file doesn't exist
		return ~0;
	}
}

AL2O3_EXTERN_C bool Os_GetCurrentDir(char *dirOut, size_t maxSize) {
	char buffer[maxSize];
	if (getcwd(buffer, maxSize) == NULL) {
		return false;
	}
	if (Os_GetPlatformPathFromNormalisedPath(buffer, dirOut, maxSize) == false) {
		return false;
	}

	size_t len = strlen(dirOut);
	if (dirOut[len] != '/') {
		if (len + 1 >= maxSize) {
			return false;
		}
		dirOut[len] = '/';
		dirOut[len + 1] = 0;
	}
	return true;

}

AL2O3_EXTERN_C bool Os_SetCurrentDir(char const *path) {
	return chdir(path) == 0;
}

AL2O3_EXTERN_C bool Os_FileExists(char const *path) {
	struct stat st;
	int result = stat(path, &st);
	if (result != 0) {
		return false;
	}

	return !(st.st_mode & S_IFDIR);
}

AL2O3_EXTERN_C bool Os_DirExists(char const *path) {
	struct stat st;
	int result = stat(path, &st);
	if (result != 0) {
		return false;
	}

	return (st.st_mode & S_IFDIR);
}

bool Os_FileDelete(char const *fileName) {
	char buffer[2048];

	if (Os_IsNormalisedPath(fileName)) {
		strcpy(buffer, fileName);
	} else {
		bool platformOk = Os_GetPlatformPathFromNormalisedPath(fileName, buffer, sizeof(buffer));
		if (platformOk == false) {
			return false;
		}
	}

#ifdef _WIN32
	return DeleteFileA(GetNativePath(fileName).c_str()) != 0;
#else
	return remove(buffer) == 0;
#endif
}

bool Os_DirCreate(char const *pathName) {
	using namespace Os::FileSystem;

	// Create each of the parents if necessary
	tinystl::string parentPath = GetParentPath(pathName);
	if ((uint32_t) parentPath.size() > 1 && !DirExists(parentPath)) {
		if (!Os_DirCreate(parentPath.c_str())) {
			return false;
		}
	}

	bool success = mkdir(GetPlatformPathFromNormalisedPath(pathName).c_str(), S_IRWXU) == 0 || errno == EEXIST;

	return success;
}

int Os_SystemRun(char const *fileName, int argc, const char **argv) {
	tinystl::string fixedFileName = Os::FileSystem::GetPlatformPathFromNormalisedPath(fileName);

#ifdef _DURANGO
	ASSERT(!"UNIMPLEMENTED");
		return -1;

#elif defined(_WIN32)
	// Add .exe extension if no extension defined
	if (GetExtension(fixedFileName).size() == 0)
		fixedFileName += ".exe";

	tinystl::string commandLine = "\"" + fixedFileName + "\"";
	for (unsigned i = 0; i < (unsigned) arguments.size(); ++i)
		commandLine += " " + arguments[i];

	HANDLE stdOut = NULL;
	if (stdOutFile != "") {
		SECURITY_ATTRIBUTES sa;
		sa.nLength = sizeof(sa);
		sa.lpSecurityDescriptor = NULL;
		sa.bInheritHandle = TRUE;

		stdOut = CreateFileA(stdOutFile,
												 GENERIC_ALL,
												 FILE_SHARE_WRITE | FILE_SHARE_READ,
												 &sa,
												 OPEN_ALWAYS,
												 FILE_ATTRIBUTE_NORMAL,
												 NULL);
	}

	STARTUPINFOA startupInfo;
	PROCESS_INFORMATION processInfo;
	memset(&startupInfo, 0, sizeof startupInfo);
	memset(&processInfo, 0, sizeof processInfo);
	startupInfo.cb = sizeof(STARTUPINFO);
	startupInfo.dwFlags |= STARTF_USESTDHANDLES;
	startupInfo.hStdOutput = stdOut;
	startupInfo.hStdError = stdOut;

	if (!CreateProcessA(
			NULL,
			(LPSTR) commandLine.c_str(),
			NULL,
			NULL,
			stdOut ? TRUE : FALSE,
			CREATE_NO_WINDOW,
			NULL,
			NULL,
			&startupInfo,
			&processInfo))
		return -1;

	WaitForSingleObject(processInfo.hProcess, INFINITE);
	DWORD exitCode;
	GetExitCodeProcess(processInfo.hProcess, &exitCode);

	CloseHandle(processInfo.hProcess);
	CloseHandle(processInfo.hThread);

	if (stdOut) {
		CloseHandle(stdOut);
	}

	return exitCode;
#elif defined(__linux__)
	tinystl::vector<const char*> argPtrs;
		tinystl::string              cmd(fixedFileName.c_str());
		char                         space = ' ';
		cmd.append(&space, &space + 1);
		for (unsigned i = 0; i < (unsigned)arguments.size(); ++i)
		{
				cmd.append(arguments[i].begin(), arguments[i].end());
		}

		int res = system(cmd.c_str());
		return res;
#else
	pid_t pid = fork();
	if (pid == 0) {
		// child processs
		tinystl::vector<const char *> argPtrs;
		argPtrs.push_back(fixedFileName.c_str());
		for (unsigned i = 0; i < (unsigned) argc; ++i) {
			argPtrs.push_back(argv[i]);
		}
		argPtrs.push_back(NULL);

		execv(argPtrs[0], (char **) &argPtrs[0]);
		// only get here if execvp failed, so log the error
		// and kill the child!
		perror("execv failed");
		exit(-1);
	} else if (pid > 0) {
		// parent process
		int exitCode = EINTR;
		while (exitCode == EINTR) {
			wait(&exitCode);
		}
		return exitCode;
	} else {
		return -1;
	}
#endif
}

struct Os_DirectoryEnumerator {
	char path[2048];
	DIR *dir;

	Os_DirectoryEnumeratorItem lastItem;
	bool cancelled;
};

AL2O3_EXTERN_C Os_DirectoryEnumeratorHandle Os_DirectoryEnumeratorCreate(char const *cpath) {
	ASSERT(cpath);

	auto enumerator = (Os_DirectoryEnumerator *) MEMORY_MALLOC(sizeof(Os_DirectoryEnumerator));
	if (enumerator == nullptr)
		return nullptr;

	Os_GetPlatformPathFromNormalisedPath(cpath, enumerator->path, sizeof(enumerator->path));

	enumerator->dir = nullptr;
	enumerator->cancelled = false;
	return (Os_DirectoryEnumeratorHandle) enumerator;
}

AL2O3_EXTERN_C void Os_DirectoryEnumeratorDestroy(Os_DirectoryEnumeratorHandle handle) {
	ASSERT(handle != nullptr);
	auto enumerator = (Os_DirectoryEnumerator *) handle;
	if (enumerator->dir != nullptr)
		closedir(enumerator->dir);
	MEMORY_FREE(enumerator);
}

AL2O3_EXTERN_C void Os_DirectoryEnumeratorAsyncStart(Os_DirectoryEnumeratorHandle handle,
																										 Os_DirectoryEnumeratorAsyncFunc func, void *userData) {
	LOGWARNINGF("Os_DirectoryEnumeratorAsyncStart isn't async on posix yet, will be a busy sync for now");

	while (auto item = Os_DirectoryEnumeratorSyncNext(handle)) {
		func(handle, userData, item);
	}
}

AL2O3_EXTERN_C Os_DirectoryEnumeratorItem const *Os_DirectoryEnumeratorSyncNext(Os_DirectoryEnumeratorHandle handle) {
	ASSERT(handle != nullptr);
	auto enumerator = (Os_DirectoryEnumerator *) handle;
	if (enumerator->cancelled)
		return nullptr;

	if (enumerator->dir == nullptr) {
		enumerator->dir = opendir(enumerator->path);
		if (enumerator->dir == nullptr) {
			return nullptr;
		}
	}

	dirent *entry = readdir(enumerator->dir);
	if (entry == nullptr)
		return nullptr;
	// skip . and ..
	if (strncmp(entry->d_name, ".", 1) == 0) {
		return Os_DirectoryEnumeratorSyncNext(handle);
	}
	if (strncmp(entry->d_name, "..", 2) == 0) {
		return Os_DirectoryEnumeratorSyncNext(handle);
	}

	enumerator->lastItem.filename = entry->d_name;
	enumerator->lastItem.directory = entry->d_type == DT_DIR;

	return &enumerator->lastItem;
}

AL2O3_EXTERN_C bool Os_DirectoryEnumeratorCancel(Os_DirectoryEnumeratorHandle handle) {
	ASSERT(handle != nullptr);
	auto enumerator = (Os_DirectoryEnumerator *) handle;
	enumerator->cancelled = true;
	return true;
}
AL2O3_EXTERN_C bool Os_DirectoryEnumeratorStallForAll(Os_DirectoryEnumeratorHandle handle) {
	// TODO no async means this doesn't do anything
	return true;
}

