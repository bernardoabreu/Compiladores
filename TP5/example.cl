
(*  Example cool program testing as many aspects of the code generator
    as possible.
 *)

class B {
  tt:Int <- 0;
};


class A inherits IO{
  b:Bool <- true;

  s : String <- "Hello";

  a(x:Int,y:Int):Int { x+y} ;

};



class Main inherits A {

  c : A <- new A;

  v2 : Int <- 1 + 4;
  cas : SELF_TYPE;
  d : Int <- (new A).a(5,2);

  b():Int {1};

  x : SELF_TYPE;
  a : Int <- (new SELF_TYPE).a(1,2);
  bb:Bool <- b;

  main():Object { 
    {
      isvoid 0;
      not true;
      ~2;
      1+3;
      4-8;
      10*3;
      8/4;
      1<2;
      5<=3;
      1 = 1;
      
      (new Main) = (new Main);
      
      while true loop
      5
      pool;
      
      if true then 1 else 0 fi;
      
      a <- 4;
      a <- v2;

      x <- self;
      
      let 
      x1 : Int <- 0, 
      x2 : Int <- 1,
      x3 : Int <- 2,
      x4 : Int <- 3,
      x5 : Int <- 4,
      x6 : Int <- 5,
      x7 : Int <- 6,
      x8 : Int <- 7
       in {
        out_int(x1);
        out_int(x2);
        out_int(x3);
        out_int(x4);
        let bb:Int <- 20 in bb;
        out_int(x5);
        out_int(x6);
        out_int(x7);
        out_int(x8);
      };
      
      let x:Int in x<-a;
      
      self@A.a(5,3);
      
      c.a(5,3);
      
      case cas of
                a : Int => a <- 5;
                b : String => 0;
                c : Bool => 0;
                d : B => d;
      esac;
    }
  };

    a(x:Int,y:Int):Int { x+2} ;
};

class D inherits B{

  bb:Bool;

};
