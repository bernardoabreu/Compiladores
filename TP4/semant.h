#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#include <map>
#include <iterator>


#define TRUE 1
#define FALSE 0


class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;

  std::map<Symbol, Class_> *class_map;

  bool check_Cycle(Symbol vertex, bool visited[], bool *recStack,std::map<Symbol,int> *index);
  bool check_Parent(Class_);
  bool error;


public:
  ClassTable(Classes);
  int errors() { return semant_errors; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);

  Class_ get_Class(Symbol c);
  bool inheritanceErrorCheck();
  bool hasError() { return error; };
};

SymbolTable<Symbol, Symbol> *method_map;
SymbolTable<Symbol, Symbol> *attribute_map;


ClassTableP classtable;

#endif

