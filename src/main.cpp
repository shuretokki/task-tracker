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

     VER/1.2
     BUILD/10 JUNE 2025

*/

#include <cstdlib>
#include <filesystem>
using str = std::string;

str getDataPath()
{
        using path = std::filesystem::path;
        path dataPath;

#ifdef _WIN32
        char*  appdata = nullptr;
        size_t len;
        _dupenv_s(&appdata, &len, "APPDATA");
        if (appdata != nullptr) {
                dataPath = appdata;
                free(appdata);
        }
#else
        char* appdata = getenv("APPDATA");
        if (appdata != nullptr) {
            dataPath = appdata;
        }
#endif

        if (!dataPath.empty()) {
                path appFolder = dataPath / "ctask";
                std::filesystem::create_directories(appFolder);
                return (appFolder / "tasks.json").string();
        }

        return "tasks.json";
}

int initial;
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
                std::cerr << "[!] " << e.what() << '\n';
        }
}

#include <cctype>
str uppercase(str string)
{
        for (char& character : string)
                character = std::toupper(character);
        return string;
}

#include <map>
inline void keytext(const str& key, const str& status = "OUT")
{
        static const std::map<str, str> templates = { { "START", R"(
Type "ctask -h / --help" to show list of available arguments
                )" },
                                                      { "HELP", R"(
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
)" },

                                                      { "ERR_INVALID_ARG", R"(
[!] INVALID ARGUMENT, REFER: "ctask --help"
)" } };

        auto it                                   = templates.find(key);
        if (!(it != templates.end())) {
                std::cerr << "[!] KEY NOT FOUND\n";
        }

        if (status == "OUT")
                std::cout << it->second;
        else if (status == "ERR")
                std::cerr << it->second;
        else if (status == "LOG")
                std::clog << it->second;
}

/*
tasks.json object = {"id", "description", "status", "created_at", "last_update"}
tasks.json array = [{object1}, {object2}, {object3}, ...]
*/
#include <fstream>
int main(int argc, char** argv)
{
        if (argc == 1) {
                keytext("START");
                return 0;
        }

        using READ          = std::ifstream;
        using WRITE         = std::ofstream;
        const str DATA_PATH = getDataPath();
        /**/

        for (size_t i = 1; i < argc; ++i) {
                const str ARGUMENT = uppercase(argv[i]);
                bool use_flag_version{ false }, use_flag_help{ false }, use_flag_add{ false }, use_flag_update{ false },
                    use_flag_delete{ false }, use_flag_list{ false }, use_flag_mark1{ false }, use_flag_mark2{ false };

                if (ARGUMENT == "-V" || ARGUMENT == "--VERSION") {
                        if (!(argc < 3)) {
                                keytext("ERR_INVALID_ARG", "ERR");
                                return 1;
                        }
                        use_flag_version = true;
                } else if (ARGUMENT == "-H" || ARGUMENT == "--HELP") {
                        if (!(argc < 4)) {
                                keytext("ERR_INVALID_ARG", "ERR");
                                return 1;
                        }
                        use_flag_help = true;
                } else if (ARGUMENT == "ADD") {
                        if (!(argc < 5)) {
                                keytext("ERR_INVALID_ARG", "ERR");
                                return 1;
                        }
                        use_flag_add = true;
                } else if (ARGUMENT == "UPDATE") {
                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!] FAILED TO OPEN \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!] " << e.what() << '\n';
                                return 1;
                        }

                        if (!((i + 2) < argc && argc <= 4)) {
                                std::cerr << "[!] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        int targetId;
                        try {
                                targetId = std::stoi(argv[i + 1]);
                        } catch (std::invalid_argument& e) {
                                std::cerr << "[!] " << e.what() << '\n';
                                return 1;
                        }

                        bool      is_task_found   = false;
                        const str NEW_DESCRIPTION = argv[i + 2];
                        for (auto& task : j) {
                                if (!(task.is_object() && task.contains("id") && task["id"].get<int>() == targetId)) {
                                        continue;
                                }

                                Task      t("");
                                const str TEMP_DESCRIPTION = task["description"].get<str>();
                                task["description"]        = NEW_DESCRIPTION;
                                task["last_update"]        = t.currentTime();
                                is_task_found              = true;

                                std::cout << "[*] TASK " << TEMP_DESCRIPTION << " UPDATED TO -> " << NEW_DESCRIPTION
                                          << '\n';
                        }

                        if (!(is_task_found)) {
                                std::cerr << "[!] TASK NOT FOUND!\n";
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
                                std::cerr << "[!] FAILED TO OPEN \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!] " << e.what() << '\n';
                                return 1;
                        }

                        const str TARGET  = argv[i + 1];
                        bool      success = false;
                        try {
                                size_t pos;
                                int    targetId = std::stoi(TARGET, &pos);
                                std::cout << "check 1";
                                if (!(pos == TARGET.length())) {
                                        throw std::invalid_argument("[!] INVALID FORMAT\n");
                                }
                                std::cout << "check 2";
                                bool is_task_found = false;
                                for (auto task = j.begin(); task != j.end(); ++task) {
                                        if (!(task->is_object() && (*task)["id"].get<int>() == targetId)) {
                                                continue;
                                        }

                                        j.erase(task);
                                        is_task_found = true;
                                        break;
                                }
                                if (!is_task_found) {
                                        std::cerr << "[!] TASK WITH ID " << targetId << " NOT FOUND\n";
                                        return 1;
                                }

                                std::cout << "[*] TASK WITH ID " << targetId << " DELETED\n";
                                success = true;
                        } catch (const std::invalid_argument&) {
                                const str FILTER = uppercase(TARGET);
                                if (FILTER == "ALL") {
                                        j.clear();

                                        WRITE JSON__(DATA_PATH);
                                        JSON__ << j.dump(4);
                                        JSON__.close();
                                        std::cout << "[*] ALL TASK DELETED SUCCESSFULLY\n";

                                } else if (FILTER == "DONE") {
                                        bool is_task_found = false;
                                        for (auto task = j.begin(); task != j.end();) {
                                                if (task->is_object() && (*task)["status"] == "DONE") {
                                                        task = j.erase(task);
                                                        if (!is_task_found) {
                                                                is_task_found = true;
                                                        }
                                                } else {
                                                        ++task;
                                                }
                                        }
                                        if (!is_task_found) {
                                                std::cout << "~ No completed task yet!\n";
                                                return 0;
                                        }

                                        WRITE JSON__(DATA_PATH);
                                        JSON__ << j.dump(4);
                                        JSON__.close();

                                        std::cout << "[*] ALL COMPLETED TASK DELETED SUCCESFULLY\n";
                                        return 0;
                                }
                        }
                } else if (ARGUMENT == "MARK-IN-PROGRESS" || ARGUMENT == "IN-PROGRESS") {
                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!] FAILED TO LOAD \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (const json::parse_error& e) {
                                std::cerr << "[!] " << e.what() << '\n';
                                return 1;
                        }

                        if (!((i + 1) < argc && argc <= 3)) {
                                std::cerr << "[!] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        int targetId;
                        try {
                                targetId = std::stoi(argv[i + 1]);
                        } catch (std::invalid_argument& e) {
                                std::cerr << "[!] " << e.what() << '\n';
                                return 1;
                        }

                        bool is_task_found = false;
                        int  get_json_id;
                        str  get_json_desc;
                        for (auto& task : j) {
                                if (!(task.is_object() && task.contains("id") && task["id"].get<int>() == targetId)) {
                                        continue;
                                }
                                get_json_id   = task["id"].get<int>();
                                get_json_desc = task["description"].get<str>();

                                Task t("");
                                task["status"]      = "IN-PROGRESS";
                                task["last_update"] = t.currentTime();
                                is_task_found       = true;
                        }

                        if (!(is_task_found)) {
                                std::cerr << "[!] TASK NOT FOUND\n";
                                return 1;
                        }

                        WRITE JSON__(DATA_PATH);
                        JSON__ << j.dump(4);
                        JSON__.close();

                        std::cout << "[*] TASK [" << get_json_id << "] " << get_json_desc
                                  << "\n > SUCCESSFULLY MARKED IN-PROGRESS\n";
                        return 0;
                } else if (ARGUMENT == "MARK-DONE" || ARGUMENT == "DONE") {
                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!] FAILED TO LOAD \"tasks.json\"\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (const json::parse_error& e) {
                                std::cerr << "[!] " << e.what() << '\n';
                                return 1;
                        }

                        if ((i + 1) < argc && argc <= 3) {
                                int targetId;
                                try {
                                        targetId = std::stoi(argv[i + 1]);
                                } catch (std::invalid_argument& e) {
                                        std::cerr << "[!] " << e.what() << '\n';
                                        return 1;
                                }

                                int  get_json_id;
                                str  get_json_desc;
                                bool is_task_found = false;
                                for (auto& task : j) {
                                        if (!(task.is_object() && task.contains("id")
                                              && task["id"].get<int>() == targetId)) {
                                                continue;
                                        }
                                        get_json_id   = task["id"].get<int>();
                                        get_json_desc = task["description"].get<str>();

                                        Task t("");
                                        task["status"]      = "DONE";
                                        task["last_update"] = t.currentTime();
                                        is_task_found       = true;
                                }

                                if (!(is_task_found)) {
                                        std::cerr << "[!] TASK NOT FOUND\n";
                                        return 1;
                                }
                                try {
                                        WRITE JSON__(DATA_PATH);
                                        JSON__ << j.dump(4);
                                        JSON__.close();
                                } catch (json::exception& e) {
                                        std::cerr << "[!] " << e.what() << '\n';
                                        return 1;
                                }

                                std::cout << "[*] TASK [" << get_json_id << "] " << get_json_desc
                                          << "\n > SUCCESSFULLY MARKED DONE\n";
                                return 0;
                        } else {
                                std::cerr << "[!] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }
                } else if (ARGUMENT == "LIST") {
                        if (!(argc < 4)) {
                                std::cerr << "[!]\n";
                                return 1;
                        }

                        READ JSON_(DATA_PATH);
                        if (!JSON_.is_open()) {
                                std::cerr << "[!] FAILED TO OPEN \"tasks.json\"!\n";
                                return 1;
                        }

                        json j;
                        try {
                                j = json::parse(JSON_);
                                JSON_.close();
                        } catch (json::parse_error& e) {
                                std::cerr << "[!] " << e.what() << '\n';
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
                                        bool is_task_found = false;
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

                                                if (!is_task_found)
                                                        is_task_found = true;
                                        }

                                        if (!is_task_found) {
                                                std::cout << "~ No completed tasks yet!\n";
                                        }

                                        return 0;
                                } else if (FILTER == "DONE" || FILTER == "-D") {
                                        bool is_task_found = false;
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

                                                if (!is_task_found)
                                                        is_task_found = true;
                                        }

                                        if (!is_task_found) {
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
                                }
                        }
                } else {
                        std::cerr << "[!] INVALID ARGUMENT, REFER: \"ctask --help\"";
                        return 1;
                }

                if (use_flag_version) {
                        keytext("VERSION");
                } else if (use_flag_help) {
                        bool use_flag_extra_long{ false };
                        for (int j = 2; j < argc; ++j) {
                                const str FLAG = uppercase(argv[j]);
                                if (FLAG == "-L" || FLAG == "--LONG") {
                                        use_flag_extra_long = true;
                                }
                        }

                        if (use_flag_extra_long)
                                keytext("HELP_L");
                        else
                                keytext("HELP");
                } else if (use_flag_add) {
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
                        } catch (const json::exception&) {
                                std::cerr << "[!] FAILED TO READ OBJECTS FROM \"tasks.json\"\n";
                                std::cout << "[+] Creating new objects...\n[&]...\n";
                                try {
                                        j = json::array();
                                        std::cout << "[*] JSON CREATED SUCCESFULLY!\n";
                                } catch (json::exception& e) {
                                        std::cerr << "[!] " << e.what() << '\n';
                                        std::cout << "Exiting...\n";
                                        return 1;
                                }
                        }
                        JSON_.close();

                        if (!((i + 1) < argc && argc <= 3)) {
                                std::cerr << "[!] INVALID ARGUMENT, REFER: \"ctask --help\"";
                                return 1;
                        }

                        const str DESCRIPTION = argv[i + 1];
                        Task      task(DESCRIPTION);
                        std::cout << "[*] CREATED NEW TASK\n";
                        std::cout << "[+] ID=" << task.id << "  DESCRIPTION=" << task.description << '\n';

                        json j_struct = task;
                        j.push_back(j_struct);

                        WRITE JSON__(DATA_PATH);
                        JSON__ << j.dump(4);
                        JSON__.close();
                }
        }

        return 0;
}
