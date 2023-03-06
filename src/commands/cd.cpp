#include <string>
#include <filesystem>

#include "stream_manager.hpp"
#include "variable_manager.h"
#include "commands/cmds.h"

static int cd_single(StreamManager& stream_manager, std::string_view path, std::filesystem::path& current_path, VariableManager& variable_manager) {
    if (path == ".") {
        return 0;
    } else if (path == "..") {
        current_path = current_path.parent_path();
        return 0;
    } else if (path == "~") {
        current_path = variable_manager.get("HOME_DIR");
        return 0;
    } else if (std::filesystem::is_directory(current_path.append(path))) {
        return 0;
    } else {
        stream_manager.err() << '`' << path << "` is not a directory.\n";
        return 1;
    }
    return 0;
}

int cmd::cd(const std::vector<std::string>& arg, StreamManager& stream_manager, VariableManager& variable_manager) {
    if (arg.size() != 2) {
        return 1;
    }

    std::filesystem::path current_path(variable_manager.get("PWD"));
    std::string_view path = arg[1];

    if(path[0] == '/'){
        current_path = current_path.root_directory();
    }

    for (size_t i = 0; i < path.size();) {
        auto slash = path.find('/', i);
        if (slash == std::string::npos) {
            slash = path.size();
        }
        if (cd_single(stream_manager, path.substr(i, slash-i), current_path, variable_manager)) {
            return 1;
        }
        i = slash + 1;
    }

    variable_manager.set("PWD", current_path.string());
    return 0;
}