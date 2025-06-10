/*

     ,o888888o.    8888888 8888888888          .8.             d888888o.   8 8888     ,88'
    8888     `88.        8 8888               .888.          .`8888:' `88. 8 8888    ,88'
 ,8 8888       `8.       8 8888              :88888.         8.`8888.   Y8 8 8888   ,88'
 88 8888                 8 8888             . `88888.        `8.`8888.     8 8888  ,88'
 88 8888                 8 8888            .8. `88888.        `8.`8888.    8 8888 ,88'
 88 8888                 8 8888           .8`8. `88888.        `8.`8888.   8 8888 88'
 88 8888                 8 8888          .8' `8. `88888.        `8.`8888.  8 888888<
 `8 8888       .8'       8 8888         .8'   `8. `88888.   8b   `8.`8888. 8 8888 `Y8.
    8888     ,88'        8 8888        .888888888. `88888.  `8b.  ;8.`8888 8 8888   `Y8.
     `8888888P'          8 8888       .8'       `8. `88888.  `Y8888P ,88P' 8 8888     `Y8.

     VER/1.1.0
     BUILD/10 JUNE 2025

*/

#include <cstdlib>
#include <filesystem>
std::string getDataPath()
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

int initial;
#include <chrono>
struct Task
{
        int         id;
        std::string description;
        std::string status;
        std::string createdAt;
        std::string lastUpdate;

        Task(std::string addedDescription)
        {
                id          = ++initial;
                description = addedDescription;
                status      = "TO-DO";
                createdAt   = currentTime();
                lastUpdate  = currentTime();
        };

        std::string currentTime()
        {
                auto        time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
                std::string now_c  = std::ctime(&time_t);
                return now_c.substr(0, now_c.length() - 1);
        }
};

#include <iostream>
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;
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
                j.at("description").get_to(t.description);
                j.at("status").get_to(t.status);
                j.at("created_at").get_to(t.createdAt);
                j.at("last_update").get_to(t.lastUpdate);
        } catch (const std::exception& e) {
                std::cerr << "[!][ERROR] INVALID JSON FORMAT, EXCEPTION: " << e.what() << '\n';
        }
}

/*
tasks.json object = {"id", "description", "status", "created_at", "last_update"}
tasks.json array = [{object1}, {object2}, {object3}, ...]
*/

#include <cctype>
std::string uppercase(const std::string& string)
{
        std::string STRING = string;
        for (char& character : STRING) {
                character = std::toupper(character);
        }

        return STRING;
}

#include <map>
inline void txt_template(const std::string& key)
{
        static const std::map<std::string, std::string> templates = { { "HELP", R"(
Usage: ctask [command] [arguments] ...
Commands:
  -h, --help             Show this help message
  --help --long          Show detailed help with examples
  -v, --version          Display version information
  add <description>      Add a new task with the given description
  update <id> <desc>     Update the description of the task with <id>
  delete <id>            Delete the task with <id>
  mark-in-progress <id>  Mark the task with <id> as 'In progress'
  done <id>              Mark the task with <id> as 'DONE'
  list [status]          List all tasks or filter by status (todo, in-progress, done)
)" },
                                                                      { "HELP_L", R"(
Usage: ctask [command] [arguments] ...
Commands:
  [-h], [--help]
    Displays a concise list of available commands.
    Example: ctask -h

  [-l], [--long]
    Shows this detailed help message with examples.
    Example: ctask --help --long

  [-v], [--version]
    Displays the program version and build information.
    Example: ctask -v

  [add] [<description>]
    Creates a new task with the given description.
    Example: ctask add "Write report"

  [update] [<id>] [<description>]
    Updates the description of the task with the specified 
    Example: ctask update 1001 "Revise report"

  [delete] [<id>], [delete] [filter] [<filter>]
    Deletes the task with the specified ID.
    Example: ctask delete 1001

  [mark-in-progress <id>]
    Marks the task with the specified ID as 'In progress'.
    Example: ctask mark-in-progress 1001

  [done <id>]
    Marks the task with the specified ID as 'DONE'.
    Example: ctask done 1001

  [list], [list <status>]
    Lists all tasks or filters by status (todo, in-progress, done).
    Example: ctask list
    Example: ctask list done

Note: All tasks stored in "%APPDATA%\ctask\tasks.json".
)" },
                                                                      { "VERSION", R"(
[ctask] ============
Version : 1.1.0
Build   : 2025-06-10
Author  : shuretokki
)" } };
        ////////////////////////////////////////////////////////////////////////////////
        auto it                                                   = templates.find(key);
        if (it != templates.end()) {
                std::cout << it->second;
        }
}

#include <fstream>
int main(int argc, char** argv)
{
        const std::string data_path = getDataPath();
        if (argc == 1) {
                std::cout << "Type \"ctask -h / --help\" to show list of available arguments\n";
                return 0;
        }

        for (size_t i = 1; i < argc; ++i) {
                std::string argument = argv[i];
                argument             = uppercase(argument);
                if (argument == "-H" || argument == "--HELP") {

                        if (argc == 2) {
                                txt_template("HELP");
                                return 0;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                std::string filter = argv[i + 1];
                                filter             = uppercase(filter);
                                if (!(filter == "-L" || filter == "--LONG")) {
                                        std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                        return 1;
                                }

                                txt_template("HELP_L");
                                return 0;
                        }
                } else if (argument == "-V" || argument == "--VERSION") {
                        if (!(argc < 3)) {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        txt_template("VERSION");
                        return 0;
                } else if (argument == "ADD") {
                        std::ifstream readJson(data_path);

                        json j;
                        try {
                                readJson >> j;
                                for (const auto& task : j) {
                                        if (!(task.contains("id") && task["id"].is_number_integer())) {
                                                continue;
                                        }

                                        int id = task["id"].get<int>();
                                        if (id > initial)
                                                initial = id;
                                }
                        } catch (const json::exception& e) {
                                std::cerr << "[!][ERROR] FAILED TO READ OBJECTS FROM \"tasks.json\"\n";
                                std::cout << "Creating new objects...\n[&]...\n";
                                try {
                                        j = json::array();
                                        std::cout << "[*][SUCCESS] JSON CREATED SUCCESFULLY!\n";
                                } catch (json::exception& e) {
                                        std::cerr << "[!][ERROR] FAILED TO CREATE JSON: " << e.what() << '\n';
                                        std::cout << "Exiting...\n";
                                        return 1;
                                }
                        }
                        readJson.close();

                        if (!((i + 1) < argc && argc <= 3)) {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        std::string description = argv[i + 1];
                        Task        task_to_json(description);
                        std::cout << "[*][SUCCESS] CREATED NEW TASK\n";
                        std::cout << "[+] ID=" << task_to_json.id << "  DESCRIPTION=" << task_to_json.description
                                  << '\n';

                        json j_struct = task_to_json;
                        j.push_back(j_struct);

                        std::ofstream writeJson(data_path);
                        writeJson << j.dump(4);
                        writeJson.close();

                        return 0;

                } else if (argument == "UPDATE") {
                        std::ifstream readJson(data_path);
                        if (!readJson.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO OPEN \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!][ERROR] FAILED TO PARSE JSON: " << e.what() << '\n';
                                return 1;
                        }
                        if ((i + 2) < argc && argc <= 4) {
                                int targetId;
                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "[!][ERROR] INVALID ID FORMAT: " << e.what() << '\n';
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

                                if (!(taskFound)) {
                                        std::cerr << "[!][ERROR] TASK NOT FOUND!\n";
                                        return 1;
                                }

                                std::ofstream writeJson(data_path);
                                writeJson << j.dump(4);
                                writeJson.close();

                        } else {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }
                } else if (argument == "DELETE") {
                        std::ifstream readJson(data_path);
                        if (!readJson.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO OPEN \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!][ERROR] FAILED TO PARSE JSON: " << e.what() << '\n';
                                return 1;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                int targetId;
                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "[!][ERROR] INVALID ID FORMAT: " << e.what() << '\n';
                                        return 1;
                                }

                                bool taskFound = false;
                                for (auto task = j.begin(); task != j.end(); ++task) {
                                        if (!(task->is_object() && (*task)["id"].get<int>() == targetId)) {
                                                continue;
                                        }

                                        j.erase(task);
                                        taskFound = true;
                                        break;
                                }

                                if (!taskFound) {
                                        std::cerr << "[!][ERROR] TASK NOT FOUND\n";
                                        return 1;
                                }

                                std::ofstream writeJson(data_path);
                                writeJson << j.dump(4);
                                writeJson.close();
                        } else if ((i + 2) < argc && argc <= 4) {
                                std::string check_argument = argv[1 + 1];
                                check_argument             = uppercase(check_argument);
                                if (!(check_argument == "FILTER")) {
                                        std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                        return 1;
                                }

                                std::string filter = argv[i + 2];
                                filter             = uppercase(filter);
                                if (filter == "ALL") {
                                        j.clear();

                                        std::ofstream writeJson(data_path);
                                        writeJson << j.dump(4);
                                        writeJson.close();
                                        std::cout << "[*][SUCCESS] ALL TASK DELETED SUCCESSFULLY\n";

                                } else if (filter == "DONE") {
                                        for (auto task = j.begin(); task != j.end();) {
                                                if (task->is_object()
                                                    && (*task)["status"].get<std::string>() == "DONE") {
                                                        j.erase(task);
                                                } else {
                                                        ++task;
                                                }
                                        }

                                        std::ofstream writeJson(data_path);
                                        writeJson << j.dump(4);
                                        writeJson.close();
                                        std::cout << "[*][SUCCESS] ALL COMPLETED TASK SUCCESSFULLY DELETED\n";

                                } else {
                                        std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                        return 1;
                                }
                        }
                        return 0;

                } else if (argument == "MARK-IN-PROGRESS") {
                        std::ifstream readJson(data_path);
                        if (!readJson.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO LOAD \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (const json::parse_error& e) {
                                std::cerr << "[!][ERROR] FAILED TO PARSE JSON: " << e.what() << '\n';
                                return 1;
                        }

                        if (!((i + 1) < argc && argc <= 3)) {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        int targetId;
                        try {
                                targetId = std::stoi(argv[i + 1]);
                        } catch (std::invalid_argument& e) {
                                std::cerr << "[!][ERROR] INVALID ID FORMAT: " << e.what() << '\n';
                                return 1;
                        }

                        bool taskFound = false;
                        for (auto& task : j) {
                                if (!(task.is_object() && task.contains("id") && task["id"].get<int>() == targetId)) {
                                        continue;
                                }

                                Task t("");
                                task["status"]      = "In progress";
                                task["last_update"] = t.currentTime();
                                taskFound           = true;
                        }

                        if (!(taskFound)) {
                                std::cerr << "[!][ERROR] TASK NOT FOUND\n";
                                return 1;
                        }

                        std::ofstream writeJson(data_path);
                        writeJson << j.dump(4);
                        writeJson.close();

                        return 0;

                } else if (argument == "DONE") {
                        std::ifstream readJson(data_path);
                        if (!readJson.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO LOAD \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (const json::parse_error& e) {
                                std::cerr << "[!][ERROR] FAILED TO PARSE JSON: " << e.what() << '\n';
                                return 1;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                int targetId;
                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "[!][ERROR] INVALID ID FORMAT: " << e.what() << '\n';
                                        return 1;
                                }

                                bool taskFound = false;
                                for (auto& task : j) {
                                        if (!(task.is_object() && task.contains("id")
                                              && task["id"].get<int>() == targetId)) {
                                                continue;
                                        }

                                        Task t("");
                                        task["status"]      = "DONE";
                                        task["last_update"] = t.currentTime();
                                        taskFound           = true;
                                }

                                if (!(taskFound)) {
                                        std::cerr << "[!][ERROR] TASK NOT FOUND\n";
                                        return 1;
                                }

                                std::ofstream writeJson(data_path);
                                writeJson << j.dump(4);
                                writeJson.close();

                        } else {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }
                } else if (argument == "LIST") {
                        std::ifstream readJson(data_path);
                        if (!readJson.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO OPEN \"tasks.json\"!\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(readJson);
                                readJson.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!][ERROR] FAILED TO PARSE JSON: " << e.what() << '\n';
                                return 1;
                        }

                        if (argc == 2) {
                                for (auto& task : j) {
                                        if (!(task.is_object() && task.contains("description"))) {
                                                continue;
                                        }

                                        std::cout << "[" << task["id"] << "] "
                                                  << task["description"].get<std::string>();

                                        std::cout << "\nSTATUS     "
                                                  << ": " << task["status"].get<std::string>();
                                        std::cout << "\nCREATED    "
                                                  << ": " << task["created_at"].get<std::string>();
                                        std::cout << "\nLAST UPDATE"
                                                  << ": " << task["last_update"].get<std::string>() << "\n\n";
                                }

                                return 0;

                        } else if ((i + 1) < argc && argc <= 3) {
                                std::string filter = argv[i + 1];
                                filter             = uppercase(filter);
                                if (filter == "DONE") {
                                        bool taskFound = false;
                                        for (auto& task : j) {
                                                if (!(task.is_object() && task["status"] == "DONE")) {
                                                        continue;
                                                }

                                                std::cout << "[" << task["id"] << "] "
                                                          << task["description"].get<std::string>();

                                                std::cout << "\nCREATED    "
                                                          << ": " << task["created_at"].get<std::string>();
                                                std::cout << "\nLAST UPDATE"
                                                          << ": " << task["last_update"].get<std::string>() << "\n\n";

                                                if (!taskFound)
                                                        taskFound = true;
                                        }

                                        if (!taskFound) {
                                                std::cout << "~ No completed tasks yet!\n";
                                        }

                                        return 0;
                                } else if (filter == "TODO") {
                                        for (auto& task : j) {
                                                if (!(task.is_object() && task["status"] == "TO-DO")) {
                                                        continue;
                                                }

                                                std::cout << "[" << task["id"] << "] "
                                                          << task["description"].get<std::string>();

                                                std::cout << "\n[*]CREATED     "
                                                          << ":\t" << task["created_at"].get<std::string>();
                                                std::cout << "\n[*]LAST UPDATE "
                                                          << ":\t" << task["last_update"].get<std::string>() << "\n\n";
                                        }

                                        return 0;
                                } else if (filter == "IN-PRORESS" || filter == "PROGRESS") {
                                        for (auto& task : j) {
                                                if (!(task.is_object() && task["status"] == "In progress")) {
                                                        continue;
                                                }

                                                std::cout << "[" << task["id"] << "] "
                                                          << task["description"].get<std::string>();

                                                std::cout << "\nCreated    "
                                                          << ": " << task["created_at"].get<std::string>();
                                                std::cout << "\nLast update"
                                                          << ": " << task["last_update"].get<std::string>() << "\n\n";
                                        }

                                        return 0;
                                } else {
                                        std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                        return 1;
                                }
                        }
                } else {
                        std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                        return 1;
                }
        }

        return 0;
}
