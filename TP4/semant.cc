

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    bool main_class = false;
    
    Class_ c;
    class_map = new std::map<Symbol, Class_>();

    install_basic_classes();

    for(int i = classes->first(); classes->more(i); i = classes->next(i)) {
       
        Class_ cur_class = classes->nth(i);

        Symbol cur_name = cur_class->get_name();

        // Check if Main class is present
        if (cur_name == Main){
            main_class = true;
        }

        // Check for class redefinition
        if ( class_map->find(cur_name) != class_map->end()){
            semant_error(cur_class) << "Redefinition of class " << cur_name << endl;
        }
        else{
            class_map->insert(std::make_pair(cur_name, cur_class));
        }



    }


    if (main_class == false){
        semant_error() << "Missing definition of Main class." << endl;
    }


}



bool ClassTable::isCyclicUtil(Symbol vertex, bool visited[], bool *recStack,std::map<Symbol,int> *index){

    int v = (*index)[vertex];

    if(visited[v] == false)
    {
        // Mark the current node as visited and part of recursion stack
        visited[v] = true;
        recStack[v] = true;
        
        Class_ c = (*class_map)[vertex];
        Symbol parent = c->get_parent();

        //Check Parent
        if(parent!=No_class){
            if ((parent == Int) || (parent == Str) || (parent == Bool)){
                semant_error(c) << "Invalid inheritance" << endl;
            }
            else{

                std::map<Symbol, Class_>::iterator it;
                it = class_map->find(parent);
                if (it == class_map->end()){
                    semant_error(c) << "Trying to inherit from inexistant class " << parent << endl;
                }
                else{
                    // Recur for parent
                    int p = (*index)[parent];
                    if ( !visited[p] && isCyclicUtil(parent, visited, recStack,index) )
                        return true;
                    else if (recStack[p])
                        return true;
                }
            }
        
        }
 
    }
    recStack[v] = false;  // remove the vertex from recursion stack
    return false;
}
 


// Returns true if the graph contains a cycle, else false.
bool ClassTable::inheritanceCycle(){
    // Mark all the vertices as not visited and not part of recursion
    // stack
    
    int size = class_map->size(); 
    int i;

    bool *visited = new bool[size];
    bool *recStack = new bool[size];
    std::map<Symbol,int> index;

    for(i = 0; i < size; i++)    {
        visited[i] = false;
        recStack[i] = false;
    }
 

    i = 0;
    for (std::map<Symbol, Class_>::iterator it=class_map->begin(); it!=class_map->end(); ++it){
        index[it->first] = i;
        i++;
    }

 
    for (std::map<Symbol, Class_>::iterator it=class_map->begin(); it!=class_map->end(); ++it)
        if (isCyclicUtil(it->first, visited, recStack,&index)){
            semant_error()<<"Cyclic inheritance"<<endl;
            return true;
        }
 
    return false;
}


void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
    curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
    class_(Object, 
           No_class,
           append_Features(
                   append_Features(
                           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
                           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
                   single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
           filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
    class_(IO, 
           Object,
           append_Features(
                   append_Features(
                           append_Features(
                                   single_Features(method(out_string, single_Formals(formal(arg, Str)),
                                              SELF_TYPE, no_expr())),
                                   single_Features(method(out_int, single_Formals(formal(arg, Int)),
                                              SELF_TYPE, no_expr()))),
                           single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
                   single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
           filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
    class_(Int, 
           Object,
           single_Features(attr(val, prim_slot, no_expr())),
           filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
    class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
    class_(Str, 
           Object,
           append_Features(
                   append_Features(
                           append_Features(
                                   append_Features(
                                           single_Features(attr(val, Int, no_expr())),
                                           single_Features(attr(str_field, prim_slot, no_expr()))),
                                   single_Features(method(length, nil_Formals(), Int, no_expr()))),
                           single_Features(method(concat, 
                                      single_Formals(formal(arg, Str)),
                                      Str, 
                                      no_expr()))),
                   single_Features(method(substr, 
                              append_Formals(single_Formals(formal(arg, Int)), 
                                     single_Formals(formal(arg2, Int))),
                              Str, 
                              no_expr()))),
           filename);


    //Add basic classes to graph
    class_map->insert(std::make_pair(Object, Object_class));
    class_map->insert(std::make_pair(IO, IO_class));
    class_map->insert(std::make_pair(Int, Int_class));
    class_map->insert(std::make_pair(Bool, Bool_class));
    class_map->insert(std::make_pair(Str, Str_class));

    // class_map[Object] = Object_class;
    // class_map[IO] = IO_class;
    // class_map[Int] = Int_class;
    // class_map[Bool] = Bool_class;
    // class_map[Str] = Str_class;

}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()
{                                                 
    semant_errors++;                            
    return error_stream;
} 



/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);

    /* some semantic analysis code may go here */
    if(!classtable->inheritanceCycle()){
        //TODO
        //CheckFeatures
        
        //TODO
        //SetTypes
    }

    if (classtable->errors()) {
        cerr << "Compilation halted due to static semantic errors." << endl;
        exit(1);
    }
}

