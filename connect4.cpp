#include <iostream>
#include <ostream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <stdarg.h>

using namespace std;

//=================================================================
// Macro Definitions
//=================================================================
#define PRINT_PAIRS(pairs)  \
        printf("%s %d Pairs=====",__FUNCTION__, __LINE__);\
        for(auto const& pair:pairs) printf("(%d,%d) ", pair.first, pair.second); \
		printf("\n");

#define PRINT_BOARD(board1) \
         LOG("BOARD");\
         for(auto const& board : board1) { \
	       cout<<"{ "; \
           for (auto const& row : board.second){\
	         for (auto const& entry : row) {\
		       cout<<entry<<" "; \
		     }\
		   }\
		   cout<<"} "<<endl; \
	     }
	     

//=================================================================
// LOGGER
//=================================================================
#define LOG(...) log( 0, "INFO ", __FILE__, __LINE__, __VA_ARGS__);
void log(int lvl, string id, string file, int line, string msg, ...)
{
  std::ostringstream ostr;
  ostr << "[" << id << "] " << msg;
  ostr << endl;
  va_list args;
  va_start(args, msg);
  vfprintf(stderr, ostr.str().c_str(), args);
  va_end(args);
}

//=================================================================
// Typedefs
//=================================================================
typedef map<int   , vector<string>        > TBoard;
typedef map<string, vector<pair<int, int>>> TPlayerCordinates;

typedef pair  <int, int      > TPair;
typedef vector<pair<int, int>> TPairs;
typedef vector<TPairs        > TPairsList;

typedef enum {
  HORIZONTAL,
  VERTICAL,
  DIAGONAL
}TAlignment;

//=================================================================
// External Functions
//=================================================================
bool sortBySecond(const pair<int, int>& a, const pair<int, int>& b)
{
  return (a.second < b.second);
}

string createResult(TPair& pair)
{
  string res  = "("+to_string(pair.first);
  res        += "x";
  res        += to_string(pair.second)+")";
  return res;
}

//=================================================================
// class Connect4
//=================================================================
class Connect4 {

public:

  Connect4 () {}
  ~Connect4() { _board.clear(); _coordinates.clear(); }
  
  void createBoard (string str[], int length);
  bool isValidBoard();
  
  void populatePlayerCoordinates(string player);
  void updatePlayerCoordinates  (TPair& pair, string player);
  
  string getHorizontal(string player);
  string getVertical  (string player);
  string getDiagonal  (string player); 

private:

  bool  isFreeSlot  (int row, int col);
  TPair findFreeSlot(TPairs& hPairs, TAlignment align);
  
  TPairs getCordinates(string player) { return _coordinates[player]; }  
  string getBoardEntry(int x, int y);
  
  TPairsList findConsecutivePairs(TPairs& cordinates, TAlignment alignment);
  TPairs getConsecutives(TPairs::iterator it);
  
  TPairs getSecondDPairsOf(TPair& start);
  TPair  getThirdDPairOf  (TPair& start, TPair& mid);
  
  string            _player;
  TBoard            _board;
  TPlayerCordinates _coordinates;
};

void Connect4::createBoard(string strArr[], int length)
{
  LOG("Creating board");
  if (length<7) return;
  _player = strArr[0];
  if ((_player != "R") && (_player != "Y")) return;
  LOG("Got player %s", _player.c_str());
  for (int i=2;i<length+1;i++) {
    if (strArr[i-1].empty() || (strArr[i-1].size()!=15)) return; // Size of string (x,x,x,x,x,x,x)
    vector<string> row;
    string tmp = strArr[i-1];
    for (int j=1;j<15;j++) {
      if ((j%2)==1) { row.push_back(tmp.substr(j,1)); };
    }
    _board[i-1] = row;
  }
  PRINT_BOARD(_board);
}

bool Connect4::isValidBoard()
{
  if (_board.size()<6) return false;
  for(int row=1;row<=6;row++) {
    for(int col=1;col<=7;col++) {
	  string entry = _board[row][col-1];
	  if ((entry != "x") && (entry != "R") && (entry != "Y")) { LOG("INVALID Board"); return false; }
	}
  }
  return true;
}

void Connect4::populatePlayerCoordinates(string player)
{  
  TPairs pairs;
  for (int i=0;i<_board.size();i++){
    vector<string> row = _board[i+1];
    auto it = row.begin();
    while ((it = std::find_if(it, row.end(), [&] (string const &e) { return e == player; }))
      != row.end()) {
      pairs.push_back(make_pair(i+1, std::distance(row.begin(), it+1)));
      it++;
    }
  }
  if (!pairs.empty()) _coordinates[player] = pairs;
}

void Connect4::updatePlayerCoordinates(TPair& pair, string player)
{
  if(_coordinates.find(player) != _coordinates.end())
    _coordinates[player].push_back(pair);
}

string Connect4::getHorizontal(string player)
{
  TPairs cordinates = getCordinates(player);
  sort(cordinates.begin(), cordinates.end());
  PRINT_PAIRS(cordinates)
  TPairsList lConsecutives = findConsecutivePairs(cordinates, HORIZONTAL);
  for (auto consecutives: lConsecutives) {
    if (consecutives.size() == 3){
      TPair pair = findFreeSlot(consecutives, HORIZONTAL);
      if (pair.first !=0 && pair.second !=0 ) return createResult(pair);
    }
  }
  return string(); 
}

string Connect4::getVertical(string player)
{
  TPairs cordinates = getCordinates(player);
  sort(cordinates.begin(), cordinates.end(), sortBySecond);  
  PRINT_PAIRS(cordinates)
  TPairsList lConsecutives = findConsecutivePairs(cordinates, VERTICAL);
  for (auto consecutives: lConsecutives) {
    if (consecutives.size() == 3){
      TPair pair = findFreeSlot(consecutives, VERTICAL);
      if (pair.first !=0 && pair.second !=0 ) return createResult(pair);
    }
  }
  return string(); 
}

string Connect4::getDiagonal(string player)
{
  TPairs cordinates = getCordinates(player);
  sort(cordinates.begin(), cordinates.end());
  PRINT_PAIRS(cordinates)
  //~ sort(cordinates.begin(), cordinates.end(), sortBySecond);
  //~ PRINT_PAIRS(cordinates)
  TPairsList lConsecutives = findConsecutivePairs(cordinates, DIAGONAL);
  for (auto consecutives: lConsecutives) {
	PRINT_PAIRS(consecutives)
    if (consecutives.size() == 3){
      TPair pair = findFreeSlot(consecutives, DIAGONAL);
      if (pair.first !=0 && pair.second !=0 ) return createResult(pair);
    }
  }
  return string(); 
}

bool Connect4::isFreeSlot(int row, int col)
{
  printf("%s Checking row=%d col=%d\n", __FUNCTION__, row, col);
  if (row<1 || row>6 || col<1 || col>7) return false;
  return (_board[row][col-1] == "x");
}

TPair Connect4::findFreeSlot(TPairs& hPairs, TAlignment align)
{
  if (align == VERTICAL) {
    int row = hPairs.front().first;
    printf("%s Row=%d\n", __FUNCTION__, row);
    if (row > 1) {
	  if (isFreeSlot(row-1, hPairs.front().second)) return make_pair(row-1, hPairs.front().second);
    }
    row = hPairs.back().first;
    printf("%s Row=%d\n", __FUNCTION__, row);
    if (row < 6) {
	  if (isFreeSlot(row+1, hPairs.front().second)) return make_pair(row+1, hPairs.front().second);
    }
  }else if (align == HORIZONTAL){
    int col = hPairs.front().second;
    printf("%s Column=%d\n", __FUNCTION__, col);
    if (col > 1) {
	  if (isFreeSlot(hPairs.front().first, col-1))  return make_pair(hPairs.front().first, col-1);
    } 
    col = hPairs.back().second;
    printf("%s Column=%d\n", __FUNCTION__, col);
    if (col < 7) {
      if (isFreeSlot(hPairs.front().first, col+1))  return make_pair(hPairs.front().first, col+1);
	}    
  }else {
    int row = hPairs.front().first;
    int col = hPairs.front().second;
    printf("%s %d Row=%d Col=%d\n", __FUNCTION__, __LINE__, row, col);
    if (row > 1) {
	  if (hPairs[1].second < col) {
	    if (isFreeSlot(row-1, col+1))   return make_pair(row-1, col+1);
	  } else if (hPairs[1].second > col) {
	    if (isFreeSlot(row-1, col-1))   return make_pair(row-1, col-1);
	  }
    }
    row = hPairs.back().first;
    col = hPairs.back().second;
    if (row < 6) {
      if (hPairs[1].second < col) {
	    if (isFreeSlot(row+1, col+1))   return make_pair(row+1, col+1);
	  } else if (hPairs[1].second > col) {
	    if (isFreeSlot(row+1, col-1))   return make_pair(row+1, col-1);
	  }
    }    
  }
  printf("%s No free slot\n", __FUNCTION__);
  return TPair();
}

TPairs Connect4::getConsecutives(TPairs::iterator it)
{
  TPairs consecutives;
  consecutives.push_back(*it);
  consecutives.push_back(*(it+1));
  consecutives.push_back(*(it+2));
  PRINT_PAIRS(consecutives)
  return consecutives;
}

TPairsList Connect4::findConsecutivePairs(TPairs& cordinates, TAlignment align)
{
  TPairs consecutives;
  TPairsList listCons;
  TPairs::iterator it = cordinates.begin();
  for (it=cordinates.begin(); it!=cordinates.end(); it++) {
    int i;
    if (align == VERTICAL) {
	  i= (*it).first;
      if ( (i+1 == (*(it+1)).first) && (i+2 == (*(it+2)).first)) {
		if (((*it).second == (*(it+1)).second) && ((*it).second == (*(it+2)).second)) {
		  consecutives=getConsecutives(it);
          //~ consecutives.push_back(*it);
          //~ consecutives.push_back(*(it+1));
          //~ consecutives.push_back(*(it+2));
          PRINT_PAIRS(consecutives)
          listCons.push_back(consecutives);
          consecutives.clear();
          it += 2;
          if(it==cordinates.end()) break;
	    }
	  }
    } else if (align == HORIZONTAL) {
	  i =(*it).second;
      if ( (i+1 == (*(it+1)).second) && (i+2 == (*(it+2)).second))  {
		if (((*it).first == (*(it+1)).first) && ((*it).first == (*(it+2)).first)) {
          consecutives=getConsecutives(it);
          //~ consecutives.push_back(*it);
          //~ consecutives.push_back(*(it+1));
          //~ consecutives.push_back(*(it+2));
          PRINT_PAIRS(consecutives)
          listCons.push_back(consecutives);
          consecutives.clear();
          it += 2;
          if(it==cordinates.end()) break;
	    }
	  }
    } else {
	  TPair  start  = *it;
      TPairs sPairs = getSecondDPairsOf(start);
      TPairs::iterator mid;
      for (mid= sPairs.begin(); mid!= sPairs.end(); mid++) {
	    TPair end= getThirdDPairOf(start, *mid);
	    if ((end.first!=0) &&(end.second!=0)) {
          consecutives.push_back(start);
          consecutives.push_back(*mid);
          consecutives.push_back(end);
          listCons.push_back(consecutives);
          consecutives.clear();
	    }
	  }
	}
  }
  printf("%s Checking consecutive pairs\n", __FUNCTION__);  
  return listCons;
}

string Connect4::getBoardEntry(int x, int y)
{ 
  if (x>6 || x<1 || y<1 || y>6) return string();
  //~ vector<string> row = _board[x];
  //~ cout<<"_board["<<x<<"]["<<(y)<<"]="<<_board[x][y-1]<<endl;
  //cout<<"Entries in row="<<x<<" ";
  //for (auto const& e:row) cout<<e<<" ";
  //cout<<endl;
  return(_board[x][y-1]); //string starts from index 0. so for y, it will be y-1
}

TPairs Connect4::getSecondDPairsOf(TPair& start)
{
  TPairs sPairs;
  int x = start.first;
  int y = start.second;
  LOG("%s %d Start %d,%d",__FUNCTION__,__LINE__,x,y);
  string player = getBoardEntry(x, y);
  if (x>1 && y>1 && player==getBoardEntry(x-1, y-1)) sPairs.push_back(make_pair(x-1, y-1));
  if (x>1 && y<7 && player==getBoardEntry(x-1, y+1)) sPairs.push_back(make_pair(x-1, y+1));
  if (x<7 && y>1 && player==getBoardEntry(x+1, y-1)) sPairs.push_back(make_pair(x+1, y-1));
  if (x<7 && y<7 && player==getBoardEntry(x+1, y+1)) sPairs.push_back(make_pair(x+1, y+1));
  //~ for (auto const& pair : sPairs) cout<<"("<<pair.first<<","<<pair.second<<") ";
  //~ cout<<endl;
  return sPairs;
}

TPair Connect4::getThirdDPairOf(TPair& start, TPair& mid)
{
  int x  = start.first;
  int y  = start.second;
  int mx = mid.first;
  int my = mid.second;
  LOG("%s %d Start=%d,%d Mid=%d,%d",__FUNCTION__,__LINE__,x,y,mx,my);
  string player = getBoardEntry(x, y);
  if (mx>1 && my<y) {       //Check top left or bottom left of mid pair 
    if (mx>x) {             //Check mid is below start
	  if (my>1 && my<7 && player==getBoardEntry(mx+1, my-1))  return make_pair(mx+1, my-1); //Check end is the same player
    }
    else {                  // If mid is above start
	  if (my>1 && my<7 && player==getBoardEntry(mx-1, my-1))  return make_pair(mx-1, my-1);
	}
  }else if (mx>1 && my>y) { //Check top right or bottom right of mid pair
	if (mx>x) {             //Check mid is below start
	  if (my>1 && my<7 && player==getBoardEntry(mx+1, my+1))  return make_pair(mx+1, my+1); //Check end is the same player
    }
    else {                  // If mid is above start
	  if (my>1 && my<7 && player==getBoardEntry(mx-1, my+1))  return make_pair(mx-1, my+1);
	} 
  }
  return TPair();
}

string GameChallenge(string strArr[], int arrLength) 
{
  // code goes here  
  if (arrLength != 7) return string();
  Connect4 con4;
  
  con4.createBoard(strArr, arrLength);
  if (!con4.isValidBoard()) return string();
  
  string player = strArr[0];
  con4.populatePlayerCoordinates(player);
  
  string pairH, pairV, pairD;
  pairH = con4.getHorizontal(player);
  LOG("pairH=%s", pairH.c_str());
  pairV = con4.getVertical(player);  
  LOG("pairV=%s", pairV.c_str());
  pairD = con4.getDiagonal(player);
  LOG("pairD=%s", pairD.c_str());
  
  
  if (!pairV.empty()) pairH += pairV;
  if (!pairD.empty()) pairH += pairD;
  
  return pairH;
}

int main(void) { 
   
  // keep this function call here
  //string A[] = {"R", "(x,x,x,R,R,R,x)", "(x,x,x,x,x,x,x)", "(x,x,x,x,x,x,x)", "(x,x,x,R,x,x,x)", "(x,x,x,R,x,x,x)", "(x,x,x,R,R,x,x)"};
  //string A[] = {"R", "(x,x,Y,R,R,R,x)", "(x,x,x,x,x,x,x)", "(x,x,x,x,x,x,x)", "(x,x,x,R,x,x,x)", "(x,x,x,R,x,x,x)", "(x,x,x,R,R,x,x)"};
  //string A[] = {"R", "(x,x,Y,R,R,R,Y)", "(x,x,x,x,x,x,x)", "(x,x,x,x,x,x,x)", "(x,x,x,R,x,x,x)", "(x,x,x,R,x,x,x)", "(x,x,x,R,R,x,x)"};
  //string A[] = {"R", "(x,x,x,R,R,R,x)", "(x,x,x,x,x,x,x)", "(x,x,x,x,x,x,x)", "(x,x,x,R,x,x,x)", "(x,x,x,R,x,x,x)", "(x,x,x,R,R,x,x)"};
  string A[] = {"R", "(x,x,x,x,x,x,x)",
	                 "(x,x,x,x,x,x,x)", 
	                 "(x,x,x,x,x,x,x)", 
	                 "(x,x,Y,x,R,R,x)", 
	                 "(x,R,Y,x,R,R,Y)", 
	                 "(R,R,R,x,R,Y,Y)"
	           };
  int arrLength = sizeof(A) / sizeof(*A);
  cout << GameChallenge(A, arrLength);
  return 0;    
}


