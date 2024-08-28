#include "file_operation.h"

#include <sys/stat.h>
#include <sys/types.h>

#include <cerrno>
#include <cstring>

#include "log.h"

namespace base {

bool create_folder_if_not_exit(std::string foler_path) {
    // Check if the directory exists
    struct stat info;
    if (stat(foler_path.c_str(), &info) != 0) {
        // Directory does not exist, create it
        if (mkdir(foler_path.c_str(), 0755) == 0) {
            LogDebug() << "Directory created successfully.";
            return true;
        } else {
            LogError() << "Failed to create directory : " << std::strerror(errno);
        }
    } else if (info.st_mode & S_IFDIR) {
        return true;
    } else {
        LogError() << "Path exists but is not a directory.";
    }
    return false;
}

}  // namespace base