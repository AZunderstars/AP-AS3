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

typedef struct Time_range
{
    Time start;
    Time end;
} Time_range;

typedef vector<Time_range> Time_ranges;

typedef struct Translator
{
    string name;
    Time_ranges time_ranges;
    Names languages;
} Translator;

const Translator NO_TRANSLATOR_FOUND{"Not Found"};

typedef vector<Translator> Translators;

typedef struct Event
{
    string name;
    Time_range time_range;
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
    Time_range time_range = {string_to_time(data[1]), string_to_time(data[2])};
    translator.time_ranges.push_back(time_range);
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
    event.time_range = {string_to_time(data[1]), string_to_time(data[2])};
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
    return language_check_translators < current_suitable_translators;
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

bool capable_time(Time_range translator_time_range, Time_range event_time_range)
{
    return time_to_min(translator_time_range.start) <= time_to_min(event_time_range.start) &&
           time_to_min(translator_time_range.end) >= time_to_min(event_time_range.end);
}

bool has_free_time(Translator translator, Time_range event_time_range)
{
    for (int i = 0; i < translator.time_ranges.size(); i++)
    {
        if (capable_time(translator.time_ranges[i], event_time_range))
            return true;
    }
    return false;
}

bool is_translator_capable(Translator translator, string language, Time_range event_time_range)
{
    return does_know_language(translator, language) && has_free_time(translator, event_time_range);
}

Translators find_capable_translators(Translators translators, string language, Time_range event_time_range)
{
    Translators capable_translators;
    for (int i = 0; i < translators.size(); i++)
    {
        if (is_translator_capable(translators[i], language, event_time_range))
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

Translator find_translator(Translators translators, string language, Time_range event_time_range)
{
    Translators capable_translators = find_capable_translators(translators, language, event_time_range);
    Translators fewer_languages = knows_fewer_languages(capable_translators);
    return find_first_alphabetically(fewer_languages);
}

Translators update_translators(Translators translators, Translator updated_translator)
{
    if (updated_translator.name == NO_TRANSLATOR_FOUND.name)
        return translators;
    int translator_index;
    for (int i = 0; i < translators.size(); i++)
    {
        if (translators[i].name == updated_translator.name)
        {
            translator_index = i;
            break;
        }
    }
    translators[translator_index] = updated_translator;
    return translators;
}

int find_free_time(Translator translator, Time_range event_time_ranges)
{
    int result_index;
    for (int i = 0; i < translator.time_ranges.size(); i++)
    {
        if (capable_time(translator.time_ranges[i], event_time_ranges))
        {
            result_index = i;
            break;
        }
    }
    return result_index;
}

bool are_times_equal(Time time1, Time time2)
{
    return time_to_min(time1) == time_to_min(time2);
}

Time_ranges add_to_time_ranges(Time_ranges time_ranges, Time_range new_time_range)
{
    if (!are_times_equal(new_time_range.start, new_time_range.end))
        time_ranges.push_back(new_time_range);
    return time_ranges;
}

Time_ranges generate_new_times(Time_range translator_free_time, Time_range event_time_range)
{
    Time_ranges new_time_ranges;
    Time_range new_free_time;
    new_free_time.start = translator_free_time.start;
    new_free_time.end = event_time_range.start;
    new_time_ranges = add_to_time_ranges(new_time_ranges, new_free_time);
    new_free_time.start = event_time_range.end;
    new_free_time.end = translator_free_time.end;
    new_time_ranges = add_to_time_ranges(new_time_ranges, new_free_time);
    return new_time_ranges;
}

void change_free_time(Translator &translator, Time_range event_time_range)
{
    if (translator.name == NO_TRANSLATOR_FOUND.name)
        return;
    int free_time_index = find_free_time(translator, event_time_range);
    Time_ranges new_time_ranges = generate_new_times(translator.time_ranges[free_time_index], event_time_range);
    translator.time_ranges.erase(translator.time_ranges.begin() + free_time_index);
    translator.time_ranges.insert(translator.time_ranges.end(), new_time_ranges.begin(), new_time_ranges.end());
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
        Translator translator = find_translator(translators, language, event.time_range);
        int language_index = find_language_index(event, language);
        event.translators[language_index] = translator.name;
        change_free_time(translator, event.time_range);
        translators = update_translators(translators, translator);
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
