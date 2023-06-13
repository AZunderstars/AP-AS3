#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
using namespace std;

const char INPUT_SEPERATOR = ' ';
const string OUTPUT_SEPERATOR = ": ";
const string EMPTY_STRING = "";
const int FIRST_HOUR_CHAR = 0;
const int FIRST_MINUTE_CHAR = 3;
const char ZERO_CHARACTER = '0';
const char NULL_CHARACTER = '\0';
const char ENTER_CHARACTER = '\n';

typedef vector<string> Names;

typedef struct Time
{
    int hour;
    int minute;
} Time;

typedef vector<Time> Times;

typedef struct Translator
{
    string name;
    Times start_times;
    Times end_times;
    Names languages;
} Translator;

const Translator NO_TRANSLATOR_FOUND{"Not Found"};

typedef vector<Translator> Translators;

typedef struct Event
{
    string name;
    Time start_time;
    Time end_time;
    Names languages;
    Names translators;
} Event;

typedef vector<Event> Events;

bool compare_translator_names(Translator a, Translator b)
{
    return a.name < b.name;
}

int char_to_int(char c)
{
    int result = c - ZERO_CHARACTER;
    return result;
}

int string_to_num(string str)
{
    int result = 0;
    for (int i = 0; i < str.size(); i++)
        result = result * 10 + char_to_int(str[i]);
    return result;
}

int read_count(ifstream &file)
{
    string line;
    getline(file, line);
    return string_to_num(line);
}

vector<string> seperate_line(string line)
{
    vector<string> input_data;
    int i = 0;
    string data;
    while (true)
    {
        if (line[i] != INPUT_SEPERATOR && line[i] != ENTER_CHARACTER && line[i] != NULL_CHARACTER)
            data.push_back(line[i]);
        else
        {
            input_data.push_back(data);
            data = EMPTY_STRING;
        }
        if (line[i] == ENTER_CHARACTER || line[i] == NULL_CHARACTER)
            break;
        i++;
    }
    return input_data;
}

Time string_to_time(string str)
{
    Time time;
    time.hour = char_to_int(str[FIRST_HOUR_CHAR]) * 10 + char_to_int(str[FIRST_HOUR_CHAR + 1]);
    time.minute = char_to_int(str[FIRST_MINUTE_CHAR]) * 10 + char_to_int(str[FIRST_MINUTE_CHAR + 1]);
    return time;
}

Translator link_translator_data(vector<string> data)
{
    Translator translator;
    translator.name = data[0];
    translator.start_times.push_back(string_to_time(data[1]));
    translator.end_times.push_back(string_to_time(data[2]));
    for (int i = 3; i < data.size(); i++)
        translator.languages.push_back(data[i]);
    return translator;
}

Translator get_translator_data(ifstream &file)
{
    string line;
    getline(file, line);
    vector<string> translator_data = seperate_line(line);
    Translator translator = link_translator_data(translator_data);
    return translator;
}

Translators read_translators(ifstream &file)
{
    int translator_count = read_count(file);
    Translators translators;
    for (int i = 0; i < translator_count; i++)
        translators.push_back(get_translator_data(file));
    return translators;
}

Event link_event_data(vector<string> data)
{
    Event event;
    event.name = data[0];
    event.start_time = string_to_time(data[1]);
    event.end_time = string_to_time(data[2]);
    for (int i = 3; i < data.size(); i++)
    {
        event.languages.push_back(data[i]);
        event.translators.push_back(EMPTY_STRING);
    }
    return event;
}

Event get_event_data(ifstream &file)
{
    string line;
    getline(file, line);
    vector<string> event_data = seperate_line(line);
    Event event = link_event_data(event_data);
    return event;
}

Events read_events(ifstream &file)
{
    int event_count = read_count(file);
    Events events;
    for (int i = 0; i < event_count; i++)
        events.push_back(get_event_data(file));
    return events;
}

void read_from_file(string file_name, Translators &translators, Events &events)
{
    ifstream file;
    file.open(file_name);
    translators = read_translators(file);
    events = read_events(file);
}

Names find_remaining_languages(Translators translators, Event event)
{
    Names remaining_languages;
    for (int i = 0; i < event.languages.size(); i++)
    {
        if (event.translators[i] == EMPTY_STRING && event.translators[i] != NO_TRANSLATOR_FOUND.name)
            remaining_languages.push_back(event.languages[i]);
    }
    return remaining_languages;
}

bool does_know_language(Translator translator, string language)
{
    for (int i = 0; i < translator.languages.size(); i++)
    {
        if (translator.languages[i] == language)
            return true;
    }
    return false;
}

int count_translators(string language, Translators translators)
{
    int result = 0;
    for (int translator_count = 0; translator_count < translators.size(); translator_count++)
    {
        if (does_know_language(translators[translator_count], language))
            result++;
    }
    return result;
}

bool is_more_suitable(string current_suitable, string language_check, Translators translators)
{
    int current_suitable_translators = count_translators(current_suitable, translators);
    int language_check_translators = count_translators(language_check, translators);
    if (language_check_translators < current_suitable_translators)
        return true;
    else
        return false;
}

string find_language(Translators translators, Event event)
{
    Names remaining_languages = find_remaining_languages(translators, event);
    string suitable_language = remaining_languages[0];
    for (int i = 1; i < remaining_languages.size(); i++)
    {
        if (is_more_suitable(suitable_language, remaining_languages[i], translators))
            suitable_language = remaining_languages[i];
    }
    return suitable_language;
}

int time_to_min(Time time)
{
    return time.hour * 60 + time.minute;
}

bool capable_time(Time translator_start, Time translator_end, Time event_start, Time event_end)
{
    if (time_to_min(translator_start) <= time_to_min(event_start) && time_to_min(translator_end) >= time_to_min(event_end))
        return true;
    else
        return false;
}

bool has_free_time(Translator translator, Time start_time, Time end_time)
{
    for (int i = 0; i < translator.start_times.size(); i++)
    {
        if (capable_time(translator.start_times[i], translator.end_times[i], start_time, end_time))
            return true;
    }
    return false;
}

bool is_translator_capable(Translator translator, string language, Time start_time, Time end_time)
{
    return does_know_language(translator, language) && has_free_time(translator, start_time, end_time);
}

Translators find_capable_translators(Translators translators, string language, Time start_time, Time end_time)
{
    Translators capable_translators;
    for (int i = 0; i < translators.size(); i++)
    {
        if (is_translator_capable(translators[i], language, start_time, end_time))
            capable_translators.push_back(translators[i]);
    }
    return capable_translators;
}

int find_least_languages(Translators translators)
{
    int least_languages = translators[0].languages.size();
    for (int i = 1; i < translators.size(); i++)
    {
        if (translators[i].languages.size() < least_languages)
            least_languages = translators[i].languages.size();
    }
    return least_languages;
}

Translators knows_fewer_languages(Translators translators)
{
    Translators fewer_languages;
    if (translators.size() == 0)
        return fewer_languages;
    int least_languages = find_least_languages(translators);
    for (int i = 0; i < translators.size(); i++)
    {
        if (translators[i].languages.size() == least_languages)
            fewer_languages.push_back(translators[i]);
    }
    return fewer_languages;
}

Translator find_first_alphabetically(Translators translators)
{
    if (translators.size() == 0)
        return NO_TRANSLATOR_FOUND;
    sort(translators.begin(), translators.end(), compare_translator_names);
    return translators[0];
}

Translator find_translator(Translators translators, string language, Time start_time, Time end_time)
{
    Translators capable_translators = find_capable_translators(translators, language, start_time, end_time);
    Translators fewer_languages = knows_fewer_languages(capable_translators);
    return find_first_alphabetically(fewer_languages);
}

int find_translator_index(Translators translators, string translator_name)
{
    int result_index;
    for (int i = 0; i < translators.size(); i++)
    {
        if (translators[i].name == translator_name)
        {
            result_index = i;
            break;
        }
    }
    return result_index;
}

int find_free_time(Translator translator, Time start_time, Time end_time)
{
    int result_index;
    for (int i = 0; i < translator.start_times.size(); i++)
    {
        if (capable_time(translator.start_times[i], translator.end_times[i], start_time, end_time))
        {
            result_index = i;
            break;
        }
    }
    return result_index;
}

bool are_times_equal(Time time1, Time time2)
{
    if (time_to_min(time1) == time_to_min(time2))
        return true;
    else
        return false;
}

void add_to_new_times(Times &start_times, Times &end_times, Time new_start_time, Time new_end_time)
{
    if (!are_times_equal(new_start_time, new_end_time))
    {
        start_times.push_back(new_start_time);
        end_times.push_back(new_end_time);
    }
}

void generate_new_times(Translator translator, Time event_start_time, Time event_end_time, Times &start_times, Times &end_times)
{
    int free_time_index = find_free_time(translator, event_start_time, event_end_time);
    Time new_start_time, new_end_time;
    new_start_time = translator.start_times[free_time_index];
    new_end_time = event_start_time;
    add_to_new_times(start_times, end_times, new_start_time, new_end_time);
    new_start_time = event_end_time;
    new_end_time = translator.end_times[free_time_index];
    add_to_new_times(start_times, end_times, new_start_time, new_end_time);
}

void add_delete_times(Translator &translator, Time event_start_time, Time event_end_time, Times start_times, Times end_times)
{
    int free_time_index = find_free_time(translator, event_start_time, event_end_time);

    translator.start_times.erase(translator.start_times.begin() + free_time_index);
    translator.end_times.erase(translator.end_times.begin() + free_time_index);
    for (int i = 0; i < start_times.size(); i++)
    {
        translator.start_times.push_back(start_times[i]);
        translator.end_times.push_back(end_times[i]);
    }
}

void change_free_time(Translators &translators, Translator translator, Time start_time, Time end_time)
{
    if (translator.name == NO_TRANSLATOR_FOUND.name)
        return;
    Times new_start_times, new_end_times;
    generate_new_times(translator, start_time, end_time, new_start_times, new_end_times);
    add_delete_times(translator, start_time, end_time, new_start_times, new_end_times);
}

int find_language_index(Event event, string language)
{
    int result_index;
    for (int i = 0; i < event.languages.size(); i++)
    {
        if (event.languages[i] == language)
            result_index = i;
    }
    return result_index;
}

void arrange_event(Translators &translators, Event &event)
{
    for (int i = 0; i < event.languages.size(); i++)
    {
        string language = find_language(translators, event);
        Translator translator = find_translator(translators, language, event.start_time, event.end_time);
        int language_index = find_language_index(event, language);
        event.translators[language_index] = translator.name;
        change_free_time(translators, translator, event.start_time, event.end_time);
    }
}

void plan_events(Translators &translators, Events &events)
{
    for (int i = 0; i < events.size(); i++)
        arrange_event(translators, events[i]);
}

void print_results(Events events)
{
    for (int event_count = 0; event_count < events.size(); event_count++)
    {
        cout << events[event_count].name << endl;
        for (int i = 0; i < events[event_count].languages.size(); i++)
            cout << events[event_count].languages[i] << OUTPUT_SEPERATOR << events[event_count].translators[i] << endl;
    }
}

int main(int argc, char *argv[])
{
    Translators translators;
    Events events;
    read_from_file(argv[1], translators, events);
    plan_events(translators, events);
    print_results(events);
    return 0;
}