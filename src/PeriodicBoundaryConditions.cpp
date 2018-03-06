/*Raul P. Pelaez 2018. Periodic Boundary Conditions.
  This utility applies PBC to the positions in a spunto-like file

  USAGE:
  cat file | ./pbc [opt] > file.pbc
  
  [opt]:
    -L X                 To specify equal box length, X, in the three directions
    -L X Y Z             To specify independent lengths along each direction
    -Lx X -Ly Y -Lz Z    Same as above
    If only Lx and Lz are present, 2D mode will be assumed.

  INPUT FORMAT
  # Lines starting with # will be printed without modification
  x y z WHATEVER
  .
  .
  .
  #
  x2 y2 z2 WHATEVER
  .
  .
  .
  (any superpunto file with only xyz or xyrc columns will work, characters after xyz will be printed without modification)

*/
#include<string.h>
#include"superRead.h"
#include"vector_algebra.h"

using namespace superIO;
using namespace std;

struct Box{
  double3 boxSize, minusInvBoxSize;  
  Box(double3 L): boxSize(L), minusInvBoxSize(make_double3(double(-1.0)/L.x, double(-1.0)/L.y, double(-1.0)/L.z)){
    if(boxSize.x==double(0.0)) 	minusInvBoxSize.x = double(0.0);
    if(boxSize.y==double(0.0))	minusInvBoxSize.y = double(0.0);
    if(boxSize.z==double(0.0))	minusInvBoxSize.z = double(0.0);
  }
  inline double3 apply_pbc(const double3 &r) const{
    //return  r - floorf(r/L+double(0.5))*L; //MIC Algorithm
    const double3 offset = floor(r*minusInvBoxSize + double(0.5)); //MIC Algorithm
    return  r + offset*boxSize;
  }
  template< class vecType>
  inline bool isInside(const vecType &pos){
    double3 boxSizehalf = double(0.5)*boxSize;
    if(pos.x <= -boxSizehalf.x || pos.x > boxSizehalf.x) return false;
    if(pos.y <= -boxSizehalf.y || pos.y > boxSizehalf.y) return false;
    if(pos.z <= -boxSizehalf.z || pos.z > boxSizehalf.z) return false;
    return true;
  }
};



void print_help();
void PeriodicBoundaryConditions(FILE *in, Box box);

int main(int argc, char *argv[]){
  FILE *in = stdin;
  if(isatty(0)){
    std::cerr<<"ERROR: NO DATA ON STDIN DETECTED!"<<std::endl;
    print_help();
    exit(1);
  }
  double Lread = 0;
  double Lx = 0, Ly = 0, Lz = 0;
  double L[3] = {0,0,0};
  /* Parse cli input */
  //Check if L is in input
  for(int i=1;i<argc; i++){
    /*With -L you can have one or three numbers*/
    if(strcmp(argv[i], "-L")==0){
      Lx = strtod(argv[i+1], NULL);
      if(argc>i+3){
	Ly = strtod(argv[i+2], NULL);
	Lz = strtod(argv[i+3], NULL);
      }
      if(!Ly || !Lz ) Lread = Lx;
    }
    if(strcmp(argv[i], "-Lx")==0){
      Lx = strtod(argv[i+1], NULL);
    }
    if(strcmp(argv[i], "-Ly")==0){
      Ly = strtod(argv[i+1], NULL);
    }
    if(strcmp(argv[i], "-Lz")==0){
      Lz = strtod(argv[i+1], NULL);
    }
    if(strcmp(argv[i], "-h")==0){
      print_help();
      exit(0);
    }    
  }
  if(!Lread && !(Lx&&Ly)){cerr<<"ERROR!! NO VALID L WAS GIVEN!!"<<endl; print_help(); exit(1);}

  if(!Lz && !Lread) Lz = -1;
  if(Lread && !Lz) Lz = Lread;
  if(!Lx||!Ly)  Lx = Ly = Lread;

  L[0] = Lx;
  L[1] = Ly;
  L[2] = Lz;
  
  PeriodicBoundaryConditions(in, Box(make_double3(Lx, Ly, Lz)));

  return 0;
}

void PeriodicBoundaryConditions(FILE *in, Box box){

  int ncols = 3;
  //2D mode
  if(box.boxSize.z<0) ncols = 2;
  
  char *line;
  size_t linesize;

  std::string n1; n1.reserve(100);

  superFile file(in);
  double3 tmp = make_double3(0);

  while(1){

    int linesize = file.getNextLine(line);
    if(linesize<0){
      break;
    }
    if(file.iscomment(line, linesize, '#')){
      line[linesize-1] = '\n';
      file.write(line, linesize);
      continue;
    }
    // int color;
    // double r;
    int lastChar = string2numbers(line, linesize, ncols, (double*)&tmp.x);
    
    //lastChar += file.parseNumbers(line+lastChar, linesize-lastChar, 1, &r);
    //lastChar += file.parseNumbers(line+lastChar, linesize-lastChar, 1, &color);
    tmp = box.apply_pbc(tmp);
    

    n1.clear();
    number2string(n1, tmp.x);     n1+=" ";
    number2string(n1, tmp.y);     n1+=" ";
    number2string(n1, tmp.z);     n1+=" ";

    //file::write calls are super cheap, so dont mind over using it
    file.write(n1.c_str(), n1.size());
    file.write(line+lastChar, linesize - lastChar-1);
    char endLine = '\n';
    file.write(&endLine, 1);
  }

 
}

void print_help(){
std::cerr<<"Raul P. Pelaez 2018. Periodic Boundary Conditions."<<std::endl;
std::cerr<<"  This utility applies PBC to the positions in a spunto-like file"<<std::endl;
std::cerr<<""<<std::endl;
std::cerr<<"  USAGE:"<<std::endl;
std::cerr<<"  cat file | ./pbc [opt] > file.pbc"<<std::endl;
std::cerr<<"  "<<std::endl;
std::cerr<<"  [opt]:"<<std::endl;
std::cerr<<"    -L X                 To specify equal box length, X, in the three directions"<<std::endl;
std::cerr<<"    -L X Y Z             To specify independent lengths along each direction"<<std::endl;
std::cerr<<"    -Lx X -Ly Y -Lz Z    Same as above"<<std::endl;
std::cerr<<"    If only Lx and Lz are present, 2D mode will be assumed."<<std::endl;
std::cerr<<""<<std::endl;
std::cerr<<"  INPUT FORMAT"<<std::endl;
std::cerr<<"  # Lines starting with # will be printed without modification"<<std::endl;
std::cerr<<"  x y z WHATEVER"<<std::endl;
std::cerr<<"  ."<<std::endl;
std::cerr<<"  ."<<std::endl;
std::cerr<<"  ."<<std::endl;
std::cerr<<"  #"<<std::endl;
std::cerr<<"  x2 y2 z2 WHATEVER"<<std::endl;
std::cerr<<"  ."<<std::endl;
std::cerr<<"  ."<<std::endl;
std::cerr<<"  ."<<std::endl;
std::cerr<<"  (any superpunto file with only xyz or xyrc columns will work, characters after xyz will be printed without modification)"<<std::endl;


}
