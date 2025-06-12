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
using str = std::string;
#include <chrono>
struct Task
{
        int id;
        str description;
        str status;
        str createdAt;
        str lastUpdate;

        Task(str addedDescription)
        {
                id          = ++initial;
                description = addedDescription;
                status      = "TO-DO";
                createdAt   = currentTime();
                lastUpdate  = currentTime();
        };

        str currentTime()
        {
                using system_clock = std::chrono::system_clock;
                /**/
                auto time_t        = system_clock::to_time_t(system_clock::now());
                str  now_c         = std::ctime(&time_t);
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
                std::cerr << "[!][ERROR] " << e.what() << '\n';
        }
}

#include <cctype>
str uppercase(str str)
{
        for (char& ch : str)
                ch = std::toupper(ch);
        return str;
}

#include <map>
inline void txt_template(const str& key)
{
        static const std::map<str, str> templates = { { "HELP", R"(
Usage: ctask [command] [arguments] ...
Commands:
  -h, --help             Show this help message
  --help --long          Show detailed help with examples
  -v, --version          Display version information
  add <description>      Add a new task with the given description
  update <id> <desc>     Update the description of the task with <id>
  delete <id>            Delete the task with <id>
  delete FILTER done     Delete task marked as done
  delete fitler all      Delete all task
  mark-in-progress <id>  Mark the task with <id> as 'In progress'
  done <id>              Mark the task with <id> as 'DONE'
  list [status]          List all tasks or FILTER by status (todo, in-progress, done)

Note: 
All tasks stored in "%APPDATA%\ctask\tasks.json".
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

  [delete] [<id>], [delete] [FILTER] [all]/[done]
    Deletes the task with the specified ID.
    Example: ctask delete 1001, ctask delete FILTER all

  [mark-in-progress] [<id>]
    Marks the task with the specified ID as 'In progress'.
    Example: ctask mark-in-progress 1001

  [done] [<id>]
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

        auto it                                   = templates.find(key);
        if (it != templates.end()) {
                std::cout << it->second;
        }
}

/*
tasks.json object = {"id", "description", "status", "created_at", "last_update"}
tasks.json array = [{object1}, {object2}, {object3}, ...]
*/
#include <fstream>
int main(int argc, char** argv)
{
        using READ          = std::ifstream;
        using WRITE         = std::ofstream;
        /**/

        const str DATA_PATH = getDataPath();
        if (argc == 1) {
                std::cout << "Type \"ctask -h / --help\" to show list of available arguments\n";
                return 0;
        }

        for (size_t i = 1; i < argc; ++i) {
                const str ARGUMENT = uppercase(argv[i]);
                if (ARGUMENT == "HELP" || ARGUMENT == "-H" || ARGUMENT == "--HELP") {
                        if (argc == 2) {
                                txt_template("HELP");
                                return 0;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                const str FILTER = uppercase(argv[i + 1]);
                                if (!(FILTER == "-L" || FILTER == "--LONG")) {
                                        std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                        return 1;
                                }

                                txt_template("HELP_L");
                                return 0;
                        }
                } else if (ARGUMENT == "-V" || ARGUMENT == "--VERSION") {
                        if (!(argc < 3)) {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        txt_template("VERSION");
                        return 0;
                } else if (ARGUMENT == "ADD") {
                        READ JSON_(DATA_PATH);

                        json j;
                        try {
                                JSON_ >> j;
                                for (const auto& task : j) {
                                        if (!(task.contains("id") && task["id"].is_number_integer())) {
                                                continue;
                                        }

                                        int id = task["id"].get<int>();
                                        if (id > initial)
                                                initial = id;
                                }
                        } catch (const json::exception& e) {
                                std::cerr << "[!] FAILED TO READ OBJECTS FROM \"tasks.json\"\n";
                                std::cout << "[+] Creating new objects...\n[&]...\n";
                                try {
                                        j = json::array();
                                        std::cout << "[*][SUCCESS] JSON_ CREATED SUCCESFULLY!\n";
                                } catch (json::exception& e) {
                                        std::cerr << "[!][ERROR] " << e.what() << '\n';
                                        std::cout << "Exiting...\n";
                                        return 1;
                                }
                        }
                        JSON_.close();

                        if (!((i + 1) < argc && argc <= 3)) {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        const str DESCRIPTION = argv[i + 1];
                        Task      task(DESCRIPTION);
                        std::cout << "[*][SUCCESS] CREATED NEW TASK\n";
                        std::cout << "[+] ID=" << task.id << "  DESCRIPTION=" << task.description << '\n';

                        json j_struct = task;
                        j.push_back(j_struct);

                        WRITE JSON__(DATA_PATH);
                        JSON__ << j.dump(4);
                        JSON__.close();

                        return 0;

                } else if (ARGUMENT == "UPDATE") {
                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO OPEN \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!][ERROR] " << e.what() << '\n';
                                return 1;
                        }

                        if (!((i + 2) < argc && argc <= 4)) {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        int targetId;
                        try {
                                targetId = std::stoi(argv[i + 1]);
                        } catch (std::invalid_argument& e) {
                                std::cerr << "[!][ERROR] " << e.what() << '\n';
                                return 1;
                        }

                        bool      taskFound       = false;
                        const str NEW_DESCRIPTION = argv[i + 2];
                        for (auto& task : j) {
                                if (!(task.is_object() && task.contains("id") && task["id"].get<int>() == targetId)) {
                                        continue;
                                }

                                Task      t("");
                                const str TEMP_DESCRIPTION = task["description"].get<str>();
                                task["description"]        = NEW_DESCRIPTION;
                                task["last_update"]        = t.currentTime();
                                taskFound                  = true;

                                std::cout << "[*][SUCCESS] TASK " << task["id"] << "/" << TEMP_DESCRIPTION
                                          << " UPDATED TO -> " << NEW_DESCRIPTION << '\n';
                        }

                        if (!(taskFound)) {
                                std::cerr << "[!][ERROR] TASK NOT FOUND!\n";
                                return 1;
                        }

                        WRITE JSON__(DATA_PATH);
                        JSON__ << j.dump(4);
                        JSON__.close();

                        return 0;

                } else if (ARGUMENT == "DELETE") {
                        if (!(argc == 3)) {
                                return 1;
                        }

                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO OPEN \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!][ERROR] " << e.what() << '\n';
                                return 1;
                        }

                        const str TARGET  = argv[i + 1];
                        bool      success = false;
                        try {
                                size_t pos;
                                int    targetId = std::stoi(TARGET, &pos);

                                if (!(pos == TARGET.length())) {
                                        throw std::invalid_argument("[!][ERROR] INVALID FORMAT\n");
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
                                        std::cerr << "[!][ERROR] TASK WITH ID " << targetId << " NOT FOUND\n";
                                        return 1;
                                }

                                std::cout << "[*][SUCCESS] TASK WITH ID " << targetId << " DELETED\n";
                                success = true;
                        } catch (const std::invalid_argument&) {
                                const str FILTER = uppercase(TARGET);
                                if (FILTER == "ALL") {
                                        j.clear();

                                        WRITE JSON__(DATA_PATH);
                                        JSON__ << j.dump(4);
                                        JSON__.close();
                                        std::cout << "[*][SUCCESS] ALL TASK DELETED SUCCESSFULLY\n";

                                } else if (FILTER == "DONE") {
                                        for (auto task = j.begin(); task != j.end();) {
                                                if (task->is_object()
                                                    && (*task)["status"].get<std::string>() == "DONE") {
                                                        j.erase(task);
                                                } else {
                                                        ++task;
                                                }
                                        }
                                }
                        }

                        WRITE JSON__(DATA_PATH);
                        JSON__ << j.dump(4);
                        JSON__.close();

                        return 0;

                } else if (ARGUMENT == "MARK-IN-PROGRESS" || ARGUMENT == "IN-PROGRESS") {
                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO LOAD \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (const json::parse_error& e) {
                                std::cerr << "[!][ERROR] " << e.what() << '\n';
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
                                std::cerr << "[!][ERROR] " << e.what() << '\n';
                                return 1;
                        }

                        bool taskFound = false;
                        for (auto& task : j) {
                                if (!(task.is_object() && task.contains("id") && task["id"].get<int>() == targetId)) {
                                        continue;
                                }

                                Task t("");
                                task["status"]      = "IN-PROGRESS";
                                task["last_update"] = t.currentTime();
                                taskFound           = true;
                        }

                        if (!(taskFound)) {
                                std::cerr << "[!][ERROR] TASK NOT FOUND\n";
                                return 1;
                        }

                        WRITE JSON__(DATA_PATH);
                        JSON__ << j.dump(4);
                        JSON__.close();

                        return 0;

                } else if (ARGUMENT == "MARK-DONE" || ARGUMENT == "DONE") {
                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO LOAD \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (const json::parse_error& e) {
                                std::cerr << "[!][ERROR] " << e.what() << '\n';
                                return 1;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                int targetId;
                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "[!][ERROR] " << e.what() << '\n';
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
                                        std::cout << "[*][SUCCESS] TASK " << task["id"] << "/"
                                                  << task["description"].get<str>() << " MARKED AS DONE\n";
                                }

                                if (!(taskFound)) {
                                        std::cerr << "[!][ERROR] TASK NOT FOUND\n";
                                        return 1;
                                }

                                WRITE JSON__(DATA_PATH);
                                JSON__ << j.dump(4);
                                JSON__.close();

                                return 0;

                        } else {
                                std::cerr << "[!][ERROR] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }
                } else if (ARGUMENT == "LIST") {
                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!][ERROR] FAILED TO OPEN \"tasks.json\"!\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!][ERROR] " << e.what() << '\n';
                                return 1;
                        }

                        if (argc == 2) {
                                for (auto& task : j) {
                                        if (!(task.is_object())) {
                                                continue;
                                        }

                                        std::cout << "[" << task["id"] << "] " << task["status"].get<std::string>()
                                                  << "/" << task["description"].get<std::string>() << '\n';
                                }
                                return 0;

                        } else if ((i + 1) < argc && argc <= 3) {
                                const str FILTER = uppercase(argv[i + 1]);
                                if (FILTER == "-L" || FILTER == "--LONG") {
                                        bool taskFound = false;
                                        for (auto& task : j) {
                                                if (!(task.is_object())) {
                                                        continue;
                                                }

                                                std::cout << "[" << task["id"] << "] "
                                                          << task["status"].get<std::string>() << "/"
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
                                } else if (FILTER == "DONE" || FILTER == "-D") {
                                        bool taskFound = false;
                                        for (auto& task : j) {
                                                if (!(task.is_object() && task["status"] == "DONE")) {
                                                        continue;
                                                }

                                                std::cout << "[" << task["id"] << "] "
                                                          << task["status"].get<std::string>() << "/"
                                                          << task["description"].get<std::string>();

                                                std::cout << "\n[*] CREATED     "
                                                          << "\t" << task["created_at"].get<std::string>();
                                                std::cout << "\n[*] LAST UPDATE "
                                                          << "\t" << task["last_update"].get<std::string>() << "\n\n";

                                                if (!taskFound)
                                                        taskFound = true;
                                        }

                                        if (!taskFound) {
                                                std::cout << "~ No completed tasks yet!\n";
                                        }

                                        return 0;
                                } else if (FILTER == "TODO" || FILTER == "TO-DO" || FILTER == "-T") {
                                        for (auto& task : j) {
                                                if (!(task.is_object() && task["status"] == "TO-DO")) {
                                                        continue;
                                                }

                                                std::cout << "[" << task["id"] << "] "
                                                          << task["status"].get<std::string>() << "/"
                                                          << task["description"].get<std::string>();

                                                std::cout << "\n[*] CREATED     "
                                                          << "\t" << task["created_at"].get<std::string>();
                                                std::cout << "\n[*] LAST UPDATE "
                                                          << "\t" << task["last_update"].get<std::string>() << "\n\n";
                                        }

                                        return 0;
                                } else if (FILTER == "IN-PROGRESS" || FILTER == "-I") {
                                        for (auto& task : j) {
                                                if (!(task.is_object() && task["status"] == "In progress")) {
                                                        continue;
                                                }

                                                std::cout << "[" << task["id"] << "] "
                                                          << task["status"].get<std::string>() << "/"
                                                          << task["description"].get<std::string>();

                                                std::cout << "\n[*] CREATED     "
                                                          << "\t" << task["created_at"].get<std::string>();
                                                std::cout << "\n[*] LAST UPDATE "
                                                          << "\t" << task["last_update"].get<std::string>() << "\n\n";
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
