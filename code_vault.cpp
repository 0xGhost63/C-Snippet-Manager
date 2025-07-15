#include <iostream>
#include <string>
#include <fstream>
#include <thread>
#include <chrono>
#include <vector>
#include <sstream>
#include <filesystem>
using namespace std;
struct snippet
{
    int id;
    string title;
    string language;
    string tags;
    string code;
    bool is_favourite;
};
void update_snippets_file(); // protorype
vector<snippet> arr;         // Vector array for dynamic arrays :)

void load_snippets()
{ // Fun to load data from file into arrays every time the program starts
    arr.clear();
    ifstream file("Snippets.txt");
    if (!file.is_open())
        return;

    string line;
    snippet temp;
    while (getline(file, line))
    {
        if (line.find("ID : ") == 0)
            temp.id = stoi(line.substr(6));
        else if (line.find("Title : ") == 0)
            temp.title = line.substr(8);
        else if (line.find("Language : ") == 0)
            temp.language = line.substr(11);
        else if (line.find("Tags : ") == 0)
            temp.tags = line.substr(7);
        else if (line.find("Code : ") == 0)
        {
            temp.code = line.substr(7) + "\n";
            while (getline(file, line))
            {
                if (line.find("Favourite : ") == 0)
                {
                    string fav = line.substr(11);
                    temp.is_favourite = (fav == "Yes" || fav == "yes");
                    break;
                }
                temp.code += line + "\n";
            }
        }

        else if (line == "-------------------------------")
        {
            arr.push_back(temp);
            temp = {}; // clear for next
        }
    }
    file.close();
}

int getLastID()
{
    ifstream id_file("id.txt");
    int id = 1;
    if (id_file >> id)
    {
        return id;
    }
    return 1;
}

void saveLastID(int id)
{
    ofstream id_file("id.txt");
    id_file << id;
}

void printLine(string text)
{
    string line = "\n=========================" + text + "=========================\n";
    for (char ch : line)
    {
        cout << ch << flush;
        this_thread::sleep_for(chrono::milliseconds(20)); // typing delay
    }
}
void showmenu()
{
    printLine(" Available Options ");
    cout<<"\n";
    cout << "Enter the choice number to proceed \n\n";
    cout << "1. Add Snippet\n";
    cout << "2. View All Snippets\n";
    cout << "3. View Snippet by ID\n";
    cout << "4. Search Snippets\n";
    cout << "5. Edit Snippet\n";
    cout << "6. Delete Snippet\n";
    cout << "7. Show Favourited snippets\n";
    cout << "8. Import/Export\n";
    cout << "0. Exit\n";
    printLine("=");
    cout<<"\n";
}

void add_snippet()
{
    string title, lang, tags, code, favourite, line;
    printLine("Add New Snippet");
    cout << "Enter the title of the code snippet\n";
    cin.ignore(); // flush leftover newline
    getline(cin, title);

    cout << "From which language the code snippet belongs?\n";
    cin >> lang;
    cin.ignore();

    cout << "Enter the relevant tags of the snippet(use commas)\n";
    getline(cin, tags);

    //  MULTI-LINE CODE SUPPORT
    cout << "Enter the code snippet (type END on a new line to finish):\n";
    code = "";
    while (getline(cin, line))
    {
        if (line == "END")
            break;
        code += line + "\n";
    }

    cout << "Would you like to add this code snippet to favourites? (yes or no)\n";
    cin >> favourite;

    if (!favourite.empty())
    {
        favourite[0] = toupper(favourite[0]); // Capitalize for uniform check
    }

    int id = getLastID(); //  Consistent ID logic
    bool fav = (favourite == "Yes");
    snippet new_snippet = {id, title, lang, tags, code, fav};
    arr.push_back(new_snippet); //  Save to vector first

    //  INSTEAD of writing to file manually, we now use update_snippets_file()
    update_snippets_file(); // Save all current snippets to file
    saveLastID(id + 1);     //  ID updated for next snippet
    printLine("Snippet added successfully!");
}

void showSnippets()
{
    printLine("All Snippets");

    for (const auto &s : arr)
    {
        cout << "[" << s.id << "] "
             << s.title << " | "
             << s.language << " | Tags: " << s.tags
             << " | Favourite: " << (s.is_favourite ? "Yes" : "No") << endl;
        cout << "Code Preview: " << (s.code.length() > 30 ? s.code.substr(0, 30) + "..." : s.code) << "\n";
        cout << "----------------------------------\n";
    }
}

void searchById()
{
    printLine("Search by Snippet ID !");
    int search_id;
    string line;
    bool is_found = false;

    cout << "Enter the code ID that you want to search : ";
    cin >> search_id;

    string target_id = "ID : " + to_string(search_id);

    ifstream search_file("Snippets.txt");

    if (!search_file.is_open())
    {
        cout << "Error opening file!" << endl;
        return;
    }

    while (getline(search_file, line))
    {
        if (line == target_id)
        {
            is_found = true;
            cout << line << endl;
            continue;
        }

        if (is_found)
        {
            cout << line << endl;
            if (line == "-------------------------------")
            {
                break; // stop after the snippet block ends
            }
        }
    }

    if (!is_found)
    {
        cout << "No snippet found with ID: " << search_id << endl;
    }

    search_file.close();
}
void search_snippets() // Search by snippet tags !!!
{
    printLine("Search Snippet By Tags");
    string line;
    string target;
    bool is_found = false;

    cout << "Enter the tag to search (case-sensitive): ";
    cin.ignore();
    getline(cin, target);

    ifstream search_file("Snippets.txt");

    if (!search_file.is_open())
    {
        cout << "Error opening file!" << endl;
        return;
    }

    while (getline(search_file, line))
    {
        // Check if this line contains "Tags" and the target tag
        if (line.find("Tags :") != string::npos && line.find(target) != string::npos)
        {
            is_found = true;
            cout << "\nMatching snippet:\n";
            cout << line << endl;

            // Print rest of the snippet block
            while (getline(search_file, line))
            {
                cout << line << endl;
                if (line == "-------------------------------")
                    break;
            }
        }
    }

    if (!is_found)
    {
        cout << "No snippet found with tag: " << target << endl;
    }

    search_file.close();
}

void update_snippets_file()
{
    ofstream write_file("Snippets.txt");

    for (snippet s : arr)
    {
        write_file << "-------------------------------\n";
        write_file << "ID : " << s.id << endl;
        write_file << "Title : " << s.title << endl;
        write_file << "Language : " << s.language << endl;
        write_file << "Tags : " << s.tags << endl;
        write_file << "Code : " << s.code << endl;
        write_file << "Favourite : " << (s.is_favourite ? "Yes" : "No") << endl;
        write_file << "-------------------------------\n";
    }

    write_file.close();
}

// EDIT SNIPPET
void edit_snippet()
{
    int target_id;
    int arr_number = -1;
    int option;

    cout << "Enter the snippet id of the snippet that you want to edit ! ";
    cin >> target_id;
    cout << "The current contents of the snippet id # " << target_id << " are " << endl;

    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i].id == target_id)
        {
            arr_number = i;
            cout << "ID " << arr[i].id << endl;
            cout << "Title : " << arr[i].title << endl;
            cout << "Language : " << arr[i].language << endl;
            cout << "Tags : " << arr[i].tags << endl;
            cout << "Code : " << arr[i].code << endl;
            cout << "Favourite : " << (arr[i].is_favourite ? "Yes" : "No") << endl;
            break;
        }
    }

    if (arr_number == -1)
    {
        cout << "Snippet with ID " << target_id << " not found!\n";
        return;
    }

    cout << "Which part of the snippet would you like to edit (1-5) ?\n";
    cout << "1 - Title \n";
    cout << "2 - Language \n";
    cout << "3 - Tags \n";
    cout << "4 - Code \n";
    cout << "5 - Favourite \n";
    cout << "NOTE : Snippet ID is UN-EDITABLE !!!" << endl;

    cin >> option;
    string title, lang, tags, code, favourite, line;

    switch (option)
    {
    case 1:
        cout << "Enter the new title of the snippet:\n";
        cin.ignore();
        getline(cin, title);
        arr[arr_number].title = title;
        break;
    case 2:
        cout << "Enter the new language of the snippet:\n";
        cin.ignore();
        getline(cin, lang);
        arr[arr_number].language = lang;
        break;
    case 3:
        cout << "Enter the new tags of the snippet (use commas) : \n";
        cin.ignore();
        getline(cin, tags);
        arr[arr_number].tags = tags;
        break;
    case 4:
        cout << "Enter the new code (type END to finish):\n";
        code = "";
        while (getline(cin, line))
        {
            if (line == "END")
                break;
            code += line + "\n";
        }
        arr[arr_number].code = code;
        break;
    case 5:
        cout << "Star or unstar the snippet (yes or no): \n";
        cin >> favourite;
        arr[arr_number].is_favourite = (favourite == "yes" || favourite == "Yes");
        break;
    default:
        cout << "Invalid option selected!" << endl;
        break;
    }

    update_snippets_file();
}

void delete_snippet()
{
    int target_id;
    bool found = false;

    cout << "Enter the ID of the snippet you want to delete : ";
    cin >> target_id;

    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i].id == target_id)
        {
            found = true;
            cout << "Are you sure you want to delete this snippet? (yes/no)\n";
            cout << "ID: " << arr[i].id << endl;
            cout << "Title: " << arr[i].title << endl;
            cout << "Language: " << arr[i].language << endl;
            cout << "Tags: " << arr[i].tags << endl;
            cout << "Code: " << arr[i].code << endl;
            cout << "Favourite: " << (arr[i].is_favourite ? "Yes" : "No") << endl;
            string confirm;
            cin >> confirm;

            if (confirm == "yes" || confirm == "Yes")
            {
                arr.erase(arr.begin() + i); // ❌ delete from vector
                update_snippets_file();     // ✅ rewrite entire file
                cout << "Snippet deleted successfully!\n";
            }
            else
            {
                cout << "Deletion cancelled.\n";
            }
            break;
        }
    }

    if (!found)
    {
        cout << "Snippet with ID " << target_id << " not found!\n";
    }
}
void showFavourites()
{
    int counter = 1;
    printLine("Favourited Snippets");
    for (int i = 0; i < arr.size(); i++)
    {
        if (arr[i].is_favourite)
        {
            cout << counter++ << " - " << arr[i].title << " [" << arr[i].language << "]\n";
            cout << arr[i].code << "\n";
            printLine("==");
        }
    }
}
// AI-CODE FUNCTIONS  !!!
void exportToJson(string filename)
{
    ofstream jsonFile(filename);
    if (!jsonFile.is_open())
    {
        cout << "Error: Could not create " << filename << "\n";
        return;
    }

    jsonFile << "[\n";
    for (size_t i = 0; i < arr.size(); ++i)
    {
        const snippet &s = arr[i];
        stringstream escapedCode;
        for (char c : s.code)
        {
            if (c == '\n')
                escapedCode << "\\n";
            else if (c == '"')
                escapedCode << "\\\"";
            else
                escapedCode << c;
        }

        jsonFile << "  {\n";
        jsonFile << "    \"id\": " << s.id << ",\n";
        jsonFile << "    \"title\": \"" << s.title << "\",\n";
        jsonFile << "    \"language\": \"" << s.language << "\",\n";
        jsonFile << "    \"tags\": \"" << s.tags << "\",\n";
        jsonFile << "    \"code\": \"" << escapedCode.str() << "\",\n";
        jsonFile << "    \"is_favourite\": " << (s.is_favourite ? "true" : "false") << "\n";
        jsonFile << "  }" << (i != arr.size() - 1 ? "," : "") << "\n";
    }
    jsonFile << "]";
    jsonFile.close();
    cout << "✅ Snippets exported to " << filename << " successfully.\n";
}

void exportToTxt(string filename)
{
    ofstream write_file(filename);
    if (!write_file.is_open())
    {
        cout << "Error: Could not create " << filename << "\n";
        return;
    }

    for (snippet s : arr)
    {
        write_file << "-------------------------------\n";
        write_file << "ID : " << s.id << "\n";
        write_file << "Title : " << s.title << "\n";
        write_file << "Language : " << s.language << "\n";
        write_file << "Tags : " << s.tags << "\n";
        write_file << "Code : " << s.code;
        write_file << "Favourite : " << (s.is_favourite ? "Yes" : "No") << "\n";
        write_file << "-------------------------------\n";
    }

    write_file.close();
    cout << "✅ Snippets exported to " << filename << " successfully.\n";
}

void importFromJson(string filename)
{
    arr.clear();
    ifstream jsonFile(filename);
    if (!jsonFile.is_open())
    {
        cout << "Error: Could not open " << filename << "\n";
        return;
    }

    string line, content;
    while (getline(jsonFile, line))
        content += line;
    jsonFile.close();

    size_t pos = 0;
    while ((pos = content.find("{", pos)) != string::npos)
    {
        snippet s;
        s.id = getLastID();

        size_t title_pos = content.find("\"title\":\"", pos);
        size_t lang_pos = content.find("\"language\":\"", pos);
        size_t tags_pos = content.find("\"tags\":\"", pos);
        size_t code_pos = content.find("\"code\":\"", pos);
        size_t fav_pos = content.find("\"is_favourite\":", pos);

        s.title = content.substr(title_pos + 9, content.find("\"", title_pos + 9) - (title_pos + 9));
        s.language = content.substr(lang_pos + 12, content.find("\"", lang_pos + 12) - (lang_pos + 12));
        s.tags = content.substr(tags_pos + 8, content.find("\"", tags_pos + 8) - (tags_pos + 8));

        string raw_code = content.substr(code_pos + 8, content.find("\"", code_pos + 8) - (code_pos + 8));
        for (size_t i = 0; i < raw_code.size(); ++i)
        {
            if (raw_code[i] == '\\' && raw_code[i + 1] == 'n')
            {
                s.code += '\n';
                ++i;
            }
            else if (raw_code[i] == '\\' && raw_code[i + 1] == '"')
            {
                s.code += '"';
                ++i;
            }
            else
                s.code += raw_code[i];
        }

        s.is_favourite = content.substr(fav_pos + 15, 4) == "true";
        arr.push_back(s);
        saveLastID(s.id + 1);
        pos = content.find("}", pos);
    }

    update_snippets_file();
    cout << "✅ Snippets imported from " << filename << " successfully.\n";
}

void importFromTxt(string filename)
{
    arr.clear();
    ifstream file(filename);
    if (!file.is_open())
    {
        cout << "Error: Could not open " << filename << "\n";
        return;
    }

    string line;
    snippet temp;
    while (getline(file, line))
    {
        if (line.find("ID : ") == 0)
            temp.id = stoi(line.substr(6));
        else if (line.find("Title : ") == 0)
            temp.title = line.substr(8);
        else if (line.find("Language : ") == 0)
            temp.language = line.substr(11);
        else if (line.find("Tags : ") == 0)
            temp.tags = line.substr(7);
        else if (line.find("Code : ") == 0)
        {
            temp.code = line.substr(7) + "\n";
            while (getline(file, line))
            {
                if (line.find("Favourite : ") == 0)
                {
                    temp.is_favourite = (line.substr(11) == "Yes");
                    break;
                }
                temp.code += line + "\n";
            }
        }
        else if (line == "-------------------------------")
        {
            arr.push_back(temp);
            saveLastID(temp.id + 1);
            temp = {};
        }
    }

    file.close();
    update_snippets_file();
    cout << "✅ Snippets loaded from " << filename << " successfully.\n";
}

int main()
{
    load_snippets();
    while (true)
    {
        showmenu();
        int option;
        cin >> option;
        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Invalid input! Please enter a number.\n";
            continue;
        }
        switch (option)
        {
        case 1:
            add_snippet();
            break;
        case 2:
            showSnippets();
            break;
        case 3:
            searchById();
            break;
        case 4:
            search_snippets();
            break;
        case 5:
            edit_snippet();
            break;
        case 6:
            delete_snippet();
            break;
        case 7:
            showFavourites();
            break;
        case 8:
        {
            int choice = 0;
            printLine("Import/Export Menu");
            cout << "1 - Import from JSON\n";
            cout << "2 - Import from TXT\n";
            cout << "3 - Export to JSON\n";
            cout << "4 - Export to TXT\n";
            cout << "Enter your choice: ";
            cin >> choice;

            if (cin.fail())
            {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Invalid input! Please enter a number.\n";
                break;
            }

            string filename;
            switch (choice)
            {
            case 1:
                cout << "Enter the JSON file to import from (e.g., Snippets.json): ";
                cin >> filename;
                importFromJson(filename);
                break;
            case 2:
                cout << "Enter the TXT file to import from (e.g., Snippets.txt): ";
                cin >> filename;
                importFromTxt(filename);
                break;
            case 3:
                cout << "Enter the JSON file to export to (e.g., Snippets.json): ";
                cin >> filename;
                exportToJson(filename);
                break;
            case 4:
                cout << "Enter the TXT file to export to (e.g., Snippets.txt): ";
                cin >> filename;
                exportToTxt(filename);
                break;
            default:
                cout << "Invalid choice. Please try again.\n";
                break;
            }
            break;
        }
    case 0:
        cout << "Have a nice day :)" << endl;
        return 0;
    default:
        cout << "Enter a valid operation !" << endl;
        break;
    }
}

return 0;
}

// Add Json
// Add Interface