// Project Identifier: C0F4DFE8B340D81183C208F70F9D2D797908754D
#include <vector>
#include <iostream>
#include <algorithm>
#include <string>
#include <map>
#include <getopt.h>
#include <unordered_map>
#include "TableEntry.h"

using namespace std;


class less_than {
 private:
  size_t col_num;
  TableEntry value;

 public:
  less_than(size_t col_in, TableEntry val_in) : col_num{ col_in }, value{ val_in } {}
  bool operator()(vector<TableEntry>& row) {return row[col_num] < value;}
  bool operator()(const TableEntry& entry) {return entry < value;}
  bool operator()(pair<const TableEntry, int>& pair) {return pair.first < value;}
};

class equal_than {
 private:
  size_t col_num;
  TableEntry value;

 public:
  equal_than(size_t col_in, TableEntry val_in) : col_num{ col_in }, value{ val_in } {}
  bool operator()(vector<TableEntry>& row) {return row[col_num] == value;}
  bool operator()(const TableEntry& entry) {return entry == value;}
  bool operator()(pair<const TableEntry, int>& pair) {return pair.first == value;}
};

class greater_than {
 private:
  size_t col_num;
  TableEntry value;

 public:
  greater_than(size_t col_in, TableEntry val_in) : col_num{ col_in }, value{ val_in } {}
  bool operator()(vector<TableEntry>& row) {return row[col_num] > value;}
  bool operator()(const TableEntry& entry) {return entry > value;}
  bool operator()(pair<const TableEntry, int>& pair) {return pair.first > value;}
};

TableEntry convert(EntryType& type_in, const string& value_in) {
  if (type_in == EntryType::String) return TableEntry(value_in);
  else if (type_in == EntryType::Double) return TableEntry(stod(value_in));
  else if (type_in == EntryType::Int) return TableEntry(stoi(value_in));
  else {
    if (value_in == "true") {
      return TableEntry(true);
    } else {
      return TableEntry(false);
    }
  }
}

template<class ForwardIterator, class Compare>
int print_if(ForwardIterator first, ForwardIterator last, Compare pred, vector<pair<string, uint>>& print, bool q) {
  int prints = 0;

  while (first != last) {
    if (pred(*first)) {
      if (not q) {
        for (auto& i : print) cout << first->at(i.second) << ' ';
        cout << '\n';
      }
      ++prints;
    }
    ++first;
  }
  return prints;
}

class Table {
 public:
  Table(string name_in, int col_in) : name{ name_in } {
    col_type.resize(col_in);
    col_names.reserve(col_in * 2);
  }

  size_t col_check(const string& col_in) {
    auto col = col_names.find(col_in);
    if (col != col_names.end()) return col->second;
    else throw string{ '3' + col_in + ' ' + name};
  }

  template<class ForwardIterator>
  int print_range(ForwardIterator first, ForwardIterator last, vector<pair<string, uint>>& print, bool q) {
    int prints = 0;

    while (first != last) {
      if (not q) {
        for (auto i : first->second) {
          for (auto& it : print) {
            cout << table[i][it.second] << ' ';
          }
          cout << '\n';
          ++prints;
        }
      } else prints += int(first->second.size());
      ++first;
    }
    return prints;
  }

  void print_table(bool q) {
    string temp;
    int col_num;
    cin >> temp;
    col_num = stoi(temp);
    vector<pair<string, uint>> col_name;
    col_name.reserve(col_num);
    
    for (int i = 0; i < col_num; ++i) {
      cin >> temp;
      col_name.push_back(pair<string, uint>{temp, col_check(temp)});
    }

    cin >> temp;

    if (temp == "ALL") {
      if (not q) {
        for (auto& i : col_name) {
          cout << i.first << ' ';
        }
        cout << '\n';
        for (size_t i = 0; i < table.size(); ++i) {
          for (auto& j : col_name) {
            cout << table[i][j.second] << ' ';
          }
          cout << '\n';
        }
      }
      cout << "Printed " << table.size() << " matching rows from " << name << '\n';
    } else if (temp == "WHERE") {
      string colName;
      char op;
      size_t colNum;

      cin >> colName;
      cin >> op;
      cin >> temp;
      colNum = col_check(colName);

      if (not q) {
        for (auto& i : col_name) cout << i.first << ' ';
        cout << '\n';
      }
      int prints = 0;
      if (op == '<') {
        if (colName == index_name and index_type == "bst") {
          TableEntry val = convert(col_type[colNum], temp);
          auto end = bst.lower_bound(val);
          prints = print_range(bst.begin(), end, col_name, q);
        } else prints = print_if(table.begin(),table.end(), less_than(colNum, convert(col_type[colNum], temp)), col_name, q);
      } else if (op == '=') {
        if (colName == index_name and index_type == "bst") {
          TableEntry val = convert(col_type[colNum], temp);
          auto row = bst.find(val);
          if (row != bst.end()) {
            for (auto i : row->second) {
              if (not q) {
                for (auto& col : col_name) cout << table[i][col.second] << ' ';
                cout << '\n';
              }
              ++prints;
            }
          }
        } else if (colName == index_name and index_type== "hash") {
          TableEntry val = convert(col_type[colNum], temp);
          auto row = hash.find(val);
          if (row != hash.end()) {
            for (auto i : row->second) {
              if (not q) {
                for (auto& col : col_name) cout << table[i][col.second] << ' ';
                cout << '\n';
              }
              ++prints;
            }
          }
        } else {
          prints = print_if(table.begin(), table.end(), equal_than(colNum, convert(col_type[colNum], temp)), col_name, q);
        }
      } else {
        if (colName == index_name and index_type == "bst") {
          TableEntry val = convert(col_type[colNum], temp);
          auto begin = bst.upper_bound(val);
          prints = print_range(begin, bst.end(), col_name, q);
        } else {
          prints = print_if(table.begin(), table.end(), greater_than(colNum, convert(col_type[colNum], temp)), col_name, q);
        }
      }
      cout << "Printed " << prints << " matching rows from " << name << '\n';
    }
  }

  void insert_table() {
    string temp;
    int row_num;
    cin >> temp;
    row_num = stoi(temp);
    cin >> temp;
    size_t curr_rows = table.size();

    table.resize(curr_rows + row_num);
    for (auto& i : table) i.reserve(col_names.size());
    if (index_name.empty()) {
      for (size_t i = curr_rows; i < curr_rows + row_num; ++i) {
        for (size_t j = 0; j < col_names.size(); ++j) {
          cin >> temp;
          table[i].emplace_back(convert(col_type[j], temp));
        }
      }
    } else {
      for (uint i = uint(curr_rows); i < curr_rows + row_num; ++i) {
        for (size_t j = 0; j < col_names.size(); ++j) {
          cin >> temp;
          table[i].emplace_back(convert(col_type[j], temp));
        }
        if (index_type == "bst") bst[table[i][index_num]].emplace_back(i);
        else hash[table[i][index_num]].emplace_back(i);
      }
    }
    cout << "Added " << row_num << " rows to " << name << " from position " << curr_rows << " to " << curr_rows + row_num - 1 << '\n';
  }
  
 private:
  friend class Database;

  string name;
  string index_name;
  size_t index_num;
  string index_type;

  vector<vector<TableEntry>> table;
  vector<EntryType> col_type;
  unordered_map<string, uint> col_names;
  map<TableEntry, vector<uint32_t>> bst;
  unordered_map<TableEntry, vector<uint32_t>> hash;
};

class Database {
 private:
  bool quiet_mode = false;

 public:
  unordered_map<string, Table*> database;

  ~Database() {
    for (auto i : database) delete i.second;
  }

  void getopt (int argc, char** argv) {
    opterr = false;
    int choice;
    int index = 0;

    option long_options[] = {
      {"help", no_argument, nullptr, 'h'},
      {"quiet", no_argument, nullptr, 'q'},
      {nullptr, 0, nullptr, '\0'}
    };

    while ((choice = getopt_long(argc, argv, "hq", long_options, &index)) != -1) {
      switch (choice)
      {
        case 'h':
          cout << "Usage: \'./silly\n\t[--help | -h]\n"
                <<                   "\t[--quiet | -q]\n"
                << "EECS 281 SillyQL program\n";
          exit(0);

        case 'q':
          quiet_mode = true;
          break;
      }
    }
  }

  Table* table_check(const string& table_in) {
    auto table_check = database.find(table_in);
    if (table_check == database.end()) throw '2' + table_in;
    return table_check->second;
  }

  size_t col_check(const string& col_in, Table* table_in) {
    auto col = table_in->col_names.find(col_in);
    if (col != table_in->col_names.end()) return col->second;
    else throw string{ '3' + col_in + ' ' + table_in->name };
  }

  void create() {
    string dname;
    int dcol_num;
    cin >> dname;
    string temp;
    cin >> temp;
    dcol_num = stoi(temp);

    if (database.find(dname) != database.end()) throw string{ '1' + dname};

    Table* table = new Table(dname, dcol_num);
    for (size_t i = 0; i < table->col_type.size(); ++i) {
      cin >> temp;
      if (temp == "string") table->col_type[i] = EntryType::String;
      else if (temp == "double") table->col_type[i] = EntryType::Double;
      else if (temp == "bool") table->col_type[i] = EntryType::Bool;
      else table->col_type[i] = EntryType::Int;
    }

    vector<string> print_vec(dcol_num);
    for (uint i = 0; i < uint(dcol_num); ++i) {
      cin >> dname;
      table->col_names[dname] = i;
      print_vec[i] = dname;
    }

    database[table->name] = table;
    cout << "New table " << table->name << " with column(s) ";
    for (auto& i : print_vec) cout << i << ' ';
    cout << "created\n";
  }

  void remove() {
    string temp;
    cin >> temp;
    auto name = database.find(temp);
    if (name != database.end()) {
      delete name->second;
      database.erase(name);
      cout << "Table " << temp << " removed\n";
      return;
    }
    throw string{ '2' + temp };
  }

  void insert() {
    string table;
    string temp;
    int num;
    cin >> temp >> table;
    auto table_check = database.find(table);
    if (table_check == database.end()) {
      cin >> temp;
      num = stoi(temp);
      throw string{ '2' + table};
      for (int i = 0; i < num; ++i) {
        getline(cin, temp);
        throw string{ '4' };
      }

    }

    Table* table_ptr = table_check->second;

    table_ptr->insert_table();
  }

  void update(const string& name_in, const string& type_in, const size_t& num_in, Table* table_in) {
    if (not table_in->hash.empty()) table_in->hash.clear();
    else if (not table_in->bst.empty()) table_in->bst.clear();

    if (type_in == "bst") {
      for (uint i = 0; i < table_in->table.size(); ++i) table_in->bst[table_in->table[i][num_in]].emplace_back(i);
    } else if (type_in == "hash") {
      table_in->hash.reserve(table_in->table.size());
      for (uint i = 0; i < table_in->table.size(); ++i) table_in->hash[table_in->table[i][num_in]].emplace_back(i);
    }
    table_in->index_name = name_in;
    table_in->index_type = type_in;
    table_in->index_num = num_in;
  }

  void delete_from() {
    string table;
    string col_name;
    char op;
    string value;
    size_t col_num = 0;
    size_t row_num = 0;

    cin >> table >> table;
    cin >> col_name >> col_name;
    cin >> op;
    cin >> value;

    Table* t_ptr = table_check(table);
    row_num = t_ptr->table.size();
    col_num = col_check(col_name, t_ptr);
    auto end = t_ptr->table.end();

    if (op == '<') end = remove_if(t_ptr->table.begin(), t_ptr->table.end(), less_than(col_num, convert(t_ptr->col_type[col_num], value)));
    else if (op == '=') end = remove_if(t_ptr->table.begin(), t_ptr->table.end(), equal_than(col_num, convert(t_ptr->col_type[col_num], value)));
    else end = remove_if(t_ptr->table.begin(), t_ptr->table.end(), greater_than(col_num, convert(t_ptr->col_type[col_num], value)));

    t_ptr->table.erase(end, t_ptr->table.end());
    update(t_ptr->index_name, t_ptr->index_type, t_ptr->index_num, t_ptr);
    cout << "Deleted " << row_num - t_ptr->table.size() << " rows from " << table << '\n';
  }

  void generate() {
    string table;
    string col_name;
    string type;
    string value;
    size_t col_num = 0;
    size_t distinct = 0;

    cin >> table >> table;
    cin >> type;
    cin >> col_name >> col_name >> col_name;
    Table* t_ptr = table_check(table);
    col_num = col_check(col_name, t_ptr);
    update(col_name, type, col_num, t_ptr);
    distinct = max(t_ptr->bst.size(), t_ptr->hash.size());
    
    cout << "Created " << type << " index for table " << table << " on column " << col_name << ", with " << distinct << " distinct keys" <<'\n';
  }

  void print() {
    string table;
    cin >> table >> table;
    Table* t_ptr = table_check(table);
    t_ptr->print_table(quiet_mode);
  }

  void join() {
    string table1;
    string table2;
    string col_comp1;
    string col_comp2;
    string value;
    size_t col_num1 = 0;
    size_t col_num2 = 0;
    int col_num = 0;

    cin >> table1;
    Table* t_ptr1 = table_check(table1);
    cin >> table2 >> table2;
    Table* t_ptr2 = table_check(table2);
    cin >> col_comp1 >> col_comp1;
    col_num1 = col_check(col_comp1, t_ptr1);
    cin >> col_comp2 >> col_comp2;
    col_num2 = col_check(col_comp2, t_ptr2);
    cin >> value >> value >> value;
    col_num = stoi(value);
    vector<pair<uint, string>> col_name;
    string temp;
    col_name.reserve(col_num);

    for (int i = 0; i < col_num; ++i) {
      uint table_num = 0;
      cin >> temp >> value;
      table_num = stoi(value);

      if (table_num == 1) {
        col_check(temp, t_ptr1);
        col_name.push_back(pair<uint, string>{ 1, temp });
      } else {
        col_check(temp, t_ptr2);
        col_name.push_back(pair<uint, string>{ 2, temp });
      }
    }
    if (not quiet_mode) {
      for (auto& i : col_name) cout << i.second << ' ';
      cout << '\n';
    }

    int prints = 0;
    if (t_ptr2->index_num == col_num2 and t_ptr2->index_name == col_comp2 and t_ptr2->index_type == "bst") {
      for (auto& i : t_ptr1->table) {
        auto iter = t_ptr2->bst.find(i[col_num1]);
        if (iter != t_ptr2->bst.end()) {
          for (auto j : iter->second) {
            if (not quiet_mode) {
              for (auto& k : col_name) {
                if (k.first == 1) cout << i[t_ptr1->col_names[k.second]] << ' ';
                else cout << t_ptr2->table[j][t_ptr2->col_names[k.second]] << ' ';
              }
              cout << '\n';
            }
            ++prints;
          }
        }
      }
    } else if (t_ptr2->index_num == col_num2 and t_ptr2->index_name == col_comp2 and t_ptr2->index_type == "hash") {
      for (auto& i : t_ptr1->table) {
        auto iter = t_ptr2->hash.find(i[col_num1]);
        if (iter != t_ptr2->hash.end()) {
          for (auto j : iter->second) {
            if (not quiet_mode) {
              for (auto& k : col_name) {
                if (k.first == 1) cout << i[t_ptr1->col_names[k.second]] << ' ';
                else cout << t_ptr2->table[j][t_ptr2->col_names[k.second]] << ' ';
              }
              cout << '\n';
            }
            ++prints;
          }
        }
      }
    } else {
      unordered_map<TableEntry, vector<size_t>> hash(t_ptr2->table.size() * 2);
      for (size_t i = 0; i < t_ptr2->table.size(); ++i) hash[t_ptr2->table[i][col_num2]].emplace_back(i);
      for (auto& i : t_ptr1->table) {
        auto iter = hash.find(i[col_num1]);
        if (iter != hash.end()) {
          for (auto j : iter->second) {
            if (not quiet_mode) {
              for (auto& k : col_name) {
                if (k.first == 1) cout << i[t_ptr1->col_names[k.second]] << ' ';
                else cout << t_ptr2->table[j][t_ptr2->col_names[k.second]] << ' ';
              }
              cout << '\n';
            }
            ++prints;
          }
        }
      }
    }
    cout << "Printed " << prints << " rows from joining " << table1 << " to " << table2 << '\n';
  }

};

int main(int argc, char** argv) {
  ios_base::sync_with_stdio(false);
  cin >> std::boolalpha;
  cout << std::boolalpha;

  Database data;
  data.getopt(argc, argv);
  string cmd;
  bool printed = false;

  while (true) {
    cout << "% ";
    cin >> cmd;
    printed = false;

    try {
      if (cmd[0] == 'C') data.create();
      else if (cmd[0] == 'R') data.remove();
      else if (cmd[0] == 'I') data.insert();
      else if (cmd[0] == 'D') data.delete_from();
      else if (cmd[0] == 'G') data.generate();
      else if (cmd[0] == 'J') data.join();
      else if (cmd[0] == 'P') data.print();
      else if (cmd[0] == 'Q') throw 0;
      else if (cmd[0] == '#') throw string{ "comment" };
      else throw string{ '4' };
    } catch (string e) {
      string junk;
      getline(cin, junk);

      if (e[0] == '1') cout << "Error during " << cmd << ": Cannot create already existing table " << e.substr(1, e.size() - 1) << '\n';
      else if (e[0] == '2') cout << "Error during " << cmd << ": " << e.substr(1, e.size() - 1) << " does not name a table in the database\n";
      else if (e[0] == '3') {
        size_t space = e.find(' ');
        if (not printed) {
          cout << "Error during " << cmd << ": " << e.substr(1, space) << "does not name a column in " << e.substr(space + 1, e.size() - space - 1) << '\n';
          printed = true;
        }
      } else if (e[0] == '4') cout << "Error: unrecognized command\n";
    } catch (int) {
      cout << "Thanks for being silly!\n";
      return 0;
    }
  }
}