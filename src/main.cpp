#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

std::string getSavePath()
{
        std::filesystem::path dataPath;

#ifdef _WIN32
        char*  appdata = nullptr;
        size_t len;
        _dupenv_s(&appdata, &len, "APPDATA");
        if (appdata != nullptr) {
                dataPath = appdata;
                free(appdata);
        }
#else
        char*  home = nullptr;
        size_t len;
        _dupenv_s(&home, &len, "HOME");
        if (home != nullptr) {
                dataPath = home;
                free(home);
        }
#endif

        if (!dataPath.empty()) {
                std::filesystem::path appFolder = dataPath / "ctask";
                std::filesystem::create_directories(appFolder);
                return (appFolder / "tasks.json").string();
        }

        return "tasks.json";
}

#include <chrono>

int upId = 1000;
struct Task
{
        int         id{};
        std::string description{};
        std::string status{};
        std::string createdAt;
        std::string lastUpdate;

        Task(std::string addedDescription)
        {
                id          = ++upId;
                description = addedDescription;
                status      = "To-do";
                createdAt   = currentTime();
                lastUpdate  = currentTime();
        };

        ~Task() {};

        std::string currentTime()
        {
                auto        time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::string now_c  = std::ctime(&time_t);
                return now_c.substr(0, now_c.length() - 1);
        }
};

void to_json(json& j, const Task& t)
{
        j = json{
                { "id", t.id },
                { "description", t.description },
                { "status", t.status },
                { "created_at", t.createdAt },
                { "last_update", t.lastUpdate },
        };
}
void from_json(const json& j, Task& t)
{
        try {
                j.at("id").get_to(t.id);
                // t.id          = j.at("id").get<int>();
                t.description = j.at("description").get<std::string>();
                t.status      = j.at("status").get<std::string>();
                t.createdAt   = j.at("created_at").get<std::string>();
                t.lastUpdate  = j.at("last_update").get<std::string>();
        } catch (const std::exception& e) {
                std::cerr << "Invalid JSON Format" << e.what() << '\n';
        }
}

#include <fstream>

void loadHighestId()
{
}

/*
tasks.json object = ["id", "description", "status", "created_at", "last_update"]
tasks.json array = [{object1}, {object2}, {object3}, ...]
*/

int main(int argc, char** argv)
{
        const std::string save_path = getSavePath();

        if (argc == 1) {
                std::cout << "Type \"ctask -h / --help\" to show list of available arguments\n";
                return 0;
        }

        for (int i = 1; i < argc; ++i) {
                std::string arg = argv[i];
                if (arg == "-h" || arg == "--help") {
                        if (i + 1 < argc) {
                                std::string description = argv[i + 1];
                                if (description == "-l" || description == "--long") {
                                        std::cout << "ctask by shuretokki\n\n"
                                                  << "This program manages tasks stored in 'tasks.json'.\n"
                                                  << "Each task has an ID, description, status, creation time, and "
                                                     "last "
                                                     "update time.\n\n"
                                                  << "Commands:\n"
                                                  << "  -h, --help\n"
                                                  << "    Displays a concise list of available commands.\n"
                                                  << "    Example: ctask -h\n\n"
                                                  << "  --help --long\n"
                                                  << "    Shows this detailed help message with examples.\n"
                                                  << "    Example: ctask --help --long\n\n"
                                                  << "  -v, --version\n"
                                                  << "    Displays the program version and build information.\n"
                                                  << "    Example: ctask -v\n\n"
                                                  << "  add <description>\n"
                                                  << "    Creates a new task with the given description and 'To-do' "
                                                     "status.\n"
                                                  << "    Example: ctask add \"Write report\"\n\n"
                                                  << "  update <id> <description>\n"
                                                  << "    Updates the description of the task with the specified "
                                                     "ID.\n"
                                                  << "    Example: ctask update 1001 \"Revise report\"\n\n"
                                                  << "  delete <id>\n"
                                                  << "    Deletes the task with the specified ID.\n"
                                                  << "    Example: ctask delete 1001\n\n"
                                                  << "  mark-in-progress <id>\n"
                                                  << "    Marks the task with the specified ID as 'In progress'.\n"
                                                  << "    Example: ctask mark-in-progress 1001\n\n"
                                                  << "  done <id>\n"
                                                  << "    Marks the task with the specified ID as 'Done'.\n"
                                                  << "    Example: ctask done 1001\n\n"
                                                  << "  list [status]\n"
                                                  << "    Lists all tasks or filters by status (todo, in-progress, "
                                                     "done).\n"
                                                  << "    Example: ctask list\n"
                                                  << "    Example: ctask list done\n";
                                        return 0;
                                } else {
                                        std::cout << "Invalid argument after " << arg << ':' << description << '\n';
                                        return 1;
                                }
                        } else {
                                std::cout << "ctask by shuretokki\n"
                                          << "Usage: ctask [command] [arguments]\n\n"
                                          << "Commands:\n"
                                          << "  -h, --help            Show this help message\n"
                                          << "  --help --long         Show detailed help with examples\n"
                                          << "  -v, --version         Display version information\n"
                                          << "  add <description>     Add a new task with the given description\n"
                                          << "  update <id> <desc>    Update the description of the task with <id>\n"
                                          << "  delete <id>           Delete the task with <id>\n"
                                          << "  mark-in-progress <id> Mark the task with <id> as 'In progress'\n"
                                          << "  done <id>             Mark the task with <id> as 'Done'\n"
                                          << "  list [status]         List all tasks or filter by status (todo, "
                                             "in-progress, done)\n";
                                return 0;
                        }
                } else if (arg == "-v" || arg == "--version") {
                        if (argc > 2) {
                                std::cout << "Invalid arguments!\n";
                                return 1;
                        } else {
                                std::cout << "[ctask]\n"
                                          << "Version: 1.0.0\n"
                                          << "Build: 2025-06-09\n"
                                          << "Author: shuretokki\n"
                                          << "Description: A simple CLI tool for managing tasks stored in JSON "
                                             "format.\n";
                                return 0;
                        }
                } else if (arg == "add") {

                        std::ifstream readJson(save_path);

                        json j;
                        if (readJson.is_open()) {
                                try {
                                        readJson >> j;
                                        for (const auto& task : j) {
                                                if (task.contains("id") && task["id"].is_number_integer()) {
                                                        int id = task["id"].get<int>();
                                                        if (id > upId) {
                                                                upId = id;
                                                        }
                                                }
                                        }
                                } catch (const json::exception& e) {
                                        std::cerr << "Failed to open \"tasks.json\": " << e.what() << '\n';
                                        j = json::array();
                                }
                                readJson.close();
                        } else {
                                std::cerr << "Failed to open \"tasks.json\"!";
                                j = json::array();
                        }

                        if ((i + 1) < argc) {
                                std::string description = argv[i + 1];
                                Task        task_to_json(description);
                                std::cout << "Created new task: " << task_to_json.id << " " << task_to_json.description
                                          << '\n';

                                json j_struct = task_to_json;
                                std::cout << j_struct.dump(4) << '\n';

                                j.push_back(j_struct);

                                std::ofstream writeJson(save_path);
                                writeJson << j.dump(4);
                                writeJson.close();

                                return 0;
                        }
                } else if (arg == "update") {
                        std::ifstream readJson(save_path);
                        if (!readJson.is_open()) {
                                std::cerr << "Failed to open \"tasks.json\"";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "Error parsing json: " << e.what() << '\n';
                                return 1;
                        }
                        if ((i + 2) < argc && argc <= 4) {
                                int targetId;
                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "Invalid ID Format!\n";
                                        return 1;
                                }

                                bool        taskFound      = false;
                                std::string newDescription = argv[i + 2];
                                for (auto& task : j) {
                                        if (task.is_object() && task.contains("id")
                                            && task["id"].get<int>() == targetId) {
                                                Task t("");
                                                task["description"] = newDescription;
                                                task["last_update"] = t.currentTime();
                                                taskFound           = true;
                                        }
                                }

                                if (taskFound) {
                                        std::ofstream writeJson(save_path);
                                        writeJson << j.dump(4);
                                        writeJson.close();
                                } else {
                                        std::cerr << "Task not found!\n";
                                        return 1;
                                }

                        } else {
                                std::cerr << "Invalid arguments!\n";
                                return 1;
                        }
                } else if (arg == "delete") {
                        std::ifstream readJson(save_path);
                        if (!readJson.is_open()) {
                                std::cerr << "Failed to open \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                        } catch (json::exception& e) {
                                std::cerr << "Error parsing json: " << e.what() << '\n';
                                return 1;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                int targetId;
                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "Invalid ID Format: " << e.what() << '\n';
                                        return 1;
                                }

                                bool taskFound = false;
                                for (auto task = j.begin(); task != j.end(); ++task) {
                                        if (task->is_object() && (*task)["id"].get<int>() == targetId) {
                                                j.erase(task);
                                                taskFound = true;
                                                break;
                                        }
                                }

                                if (taskFound) {
                                        std::ofstream writeJson(save_path);
                                        writeJson << j.dump(4);
                                        writeJson.close();
                                } else {
                                        std::cerr << "Task not found!\n";
                                        return 1;
                                }

                        } else {
                                std::cerr << "Invalid arguments!\n";
                                return 1;
                        }
                } else if (arg == "mark-in-progress") {
                        std::ifstream readJson(save_path);
                        if (!readJson.is_open()) {
                                std::cerr << "Failed to load \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (const json::parse_error& e) {
                                std::cerr << "Error parsing json: " << e.what() << '\n';
                                return 1;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                int targetId;
                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "Invalid ID Format: " << e.what() << '\n';
                                        return 1;
                                }

                                bool taskFound = false;
                                for (auto& task : j) {
                                        if (task.is_object() && task.contains("id")
                                            && task["id"].get<int>() == targetId) {
                                                Task t("");
                                                task["status"]      = "In progress";
                                                task["last_update"] = t.currentTime();
                                                taskFound           = true;
                                        }
                                }

                                if (taskFound) {
                                        std::ofstream writeJson(save_path);
                                        writeJson << j.dump(4);
                                        writeJson.close();
                                } else {
                                        std::cerr << "Task not found!\n";
                                        return 1;
                                }
                        } else {
                                std::cerr << "Invalid arguments!\n";
                                return 1;
                        }
                } else if (arg == "done") {
                        std::ifstream readJson(save_path);
                        if (!readJson.is_open()) {
                                std::cerr << "Failed to load \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (const json::parse_error& e) {
                                std::cerr << "Error parsing json: " << e.what() << '\n';
                                return 1;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                int targetId;

                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "Invalid ID Format: " << e.what() << '\n';
                                        return 1;
                                }

                                bool taskFound = false;
                                for (auto& task : j) {
                                        if (task.is_object() && task.contains("id")
                                            && task["id"].get<int>() == targetId) {
                                                Task t("");
                                                task["status"]      = "Done";
                                                task["last_update"] = t.currentTime();
                                                taskFound           = true;
                                        }
                                }

                                if (taskFound) {
                                        std::ofstream writeJson(save_path);
                                        writeJson << j.dump(4);
                                        writeJson.close();
                                } else {
                                        std::cerr << "Task not found!\n";
                                        return 1;
                                }
                        } else {
                                std::cerr << "Invalid arguments!\n";
                                return 1;
                        }
                } else if (arg == "list") {
                        std::ifstream readJson(save_path);
                        if (!readJson.is_open()) {
                                std::cerr << "Failed to open \"tasks.json\"!\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "Error parsing json: " << e.what() << '\n';
                                return 1;
                        }

                        if (argc == 2) {
                                for (auto& task : j) {
                                        if (task.is_object() && task.contains("description")) {
                                                std::cout << "[" << task["id"] << "] "
                                                          << task["description"].get<std::string>()
                                                          << "\nStatus     : " << task["status"].get<std::string>()
                                                          << "\nCreated at : " << task["created_at"].get<std::string>()
                                                          << "\nLast update: " << task["last_update"].get<std::string>()
                                                          << "\n\n";
                                        }
                                }
                                return 0;
                        } else if ((i + 1) < argc && argc <= 3) {
                                std::string filter = argv[i + 1];
                                if (filter == "done") {
                                        for (auto& task : j) {
                                                if (task.is_object() && task["status"] == "Done") {
                                                        std::cout << "[" << task["id"] << "] "
                                                                  << task["description"].get<std::string>()
                                                                  << "\nCreated at : "
                                                                  << task["created_at"].get<std::string>()
                                                                  << "\nLast update: "
                                                                  << task["last_update"].get<std::string>() << "\n\n";
                                                }
                                        }
                                        return 0;
                                } else if (filter == "todo") {
                                        for (auto& task : j) {
                                                if (task.is_object() && task["status"] == "To-do") {
                                                        std::cout << "[" << task["id"] << "] "
                                                                  << task["description"].get<std::string>()
                                                                  << "\nCreated at : "
                                                                  << task["created_at"].get<std::string>()
                                                                  << "\nLast update: "
                                                                  << task["last_update"].get<std::string>() << "\n\n";
                                                }
                                        }
                                        return 0;
                                } else if (filter == "in-progress") {
                                        for (auto& task : j) {
                                                if (task.is_object() && task["status"] == "In progress") {
                                                        std::cout << "[" << task["id"] << "] "
                                                                  << task["description"].get<std::string>()
                                                                  << "\nCreated at : "
                                                                  << task["created_at"].get<std::string>()
                                                                  << "\nLast update: "
                                                                  << task["last_update"].get<std::string>() << "\n\n";
                                                }
                                        }
                                        return 0;
                                } else {
                                        std::cerr << "Invalid arguments!\n";
                                        return 1;
                                }
                        } else {
                                std::cerr << "Invalid arguments!\n";
                                return 1;
                        }
                }
        }

        return 0;
}