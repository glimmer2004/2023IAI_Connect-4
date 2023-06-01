#include <iostream>
#include "Point.h"
#include "Strategy.h"
#include <vector>
#include "Judge.h"
#include <cmath>
#include <chrono>
#include <cstring>
#include <algorithm>

using namespace std;


class Node{
	private:
	public:
		int visitCount = 0;//num of visits
		int reward = 0;//victory num
		int ** board;
		int height, width;
		int ban_x,ban_y;
		int player;//0 for me, 1 for opponent
		int *top;
		Node* parent;
		std::vector<int> expandable;//records the x coordinates(denoting width) of all expandable next-steps
		Node** children;
		int numChildren = 0;
		int move_x = 0;
		int move_y = 0;

		Node(int ** _board,  int _height, int _width, int _ban_x,int _ban_y,int _player,int * _top,vector<int> _expandable,Node* _parent){

			this->board = new int*[_height];
			for (int i = 0; i < _height; i++) {
				this->board[i] = new int[_width];
				for (int j = 0; j < _width; j++) {
					this->board[i][j] = _board[i][j];
				}
			}
			this->height=_height; this->width= _width; 
			this->ban_x = _ban_x; this->ban_y=_ban_y;
			player=_player;
			this->top = new int[_width];
			for (int i = 0; i < _width; i++) {
				this->top[i] = _top[i];
			}
			this->parent = _parent;
			expandable=_expandable;
			numChildren=0;
			children=nullptr;
			//children, move_x, move_y uninitialized
		}
		~Node() {
			for (int i = 0; i < numChildren; i++) {
				delete children[i];
			}
			delete[] children;
			delete[] top;
			for (int i = 0; i < height; i++) {
				delete[] board[i];
			}
			delete[] board;
			// delete parent;
		}
};

class UCT{
	public:
		Node* root;
		int width,height;
		int ban_x,ban_y;
		int ** board;
		UCT(const int _height, const int _width, const int _ban_x, const int _ban_y, const int* _top, int ** _board){
			width=_width; height=_height; ban_x=_ban_x; ban_y=_ban_y;
			vector<int> expandable;
			board = new int*[height];
			for (int i = 0; i < height; i++) {
				board[i] = new int[width];
				memset(board[i], 0, width * sizeof(int));
			}
		
			int* top = new int[_width];
			for (int i=0;i< width;i++) top[i]=_top[i];
			for (int i=0;i<height;i++)
				for (int j=0;j<width;j++)
					board[i][j]=_board[i][j];
			for (int i=0;i<width;i++){
				if (ban_y==i && _top[i]-1==ban_x){
					top[i]--;
				} 
				if (_top[i]>0) expandable.push_back(i);
			}
			root=new Node(board,height,width,ban_x,ban_y,0,top,expandable,nullptr);
			std::srand(static_cast<unsigned int>(std::time(nullptr)));  // Seed the random number generator
		}
		~UCT() {
			for (int i = 0; i < height; i++) {
				delete[] board[i];
			}
			delete[] board;
			delete root;
		}

		//given a node pointer, randomPlay will play randomly from the state of this node and return the result of the game(0 for tie, 1 for victory, -1 for losing)
		int randomPlay(Node* node) {
			std::srand(static_cast<unsigned int>(std::time(nullptr)));  // Seed the random number generator
			int player = node->player; // 0 is my turn, 1 is the opponent's turn
			cerr<<1<<endl;
			// Initialize tmpboard
			int** tmpboard = new int*[height];
			cerr<<2<<endl;
			// cerr<<"board of is now"<<endl;
			// for (int i=0;i<height;i++){
			// 	for (int j=0;j<width;j++){
			// 		cerr<<node->board[i][j]<<" ";
			// 	}
			// 	cerr<<endl;
			// }
			cerr<<"height is "<<height<<endl;
			cerr<<"width is "<<width<<endl;
			for (int i = 0; i < height; i++) {
				tmpboard[i] = new int[width];
				for (int j = 0; j < width; j++) {
					tmpboard[i][j] = node->board[i][j];
				}
			}
			cerr<<3<<endl;
			
			// Initialize tmptop
			int* tmptop = new int[width];
			cerr<<4<<endl;
			for (int i = 0; i < width; i++) {
				tmptop[i] = node->top[i];
			}
			cerr<<5<<endl;
			
			std::vector<int> positiveIndices;
			for (int i = 0; i < width; i++) {
				if (tmptop[i] > 0) {
					if (i == ban_y && tmptop[i] - 1 == ban_x) {
						tmptop[i]--;
					} 
					if (tmptop[i]>0){
						positiveIndices.push_back(i);
					}
				}
			}//determine which indices are available slots
			cerr<<6<<endl;
			if (isTie(width, tmptop)) {
				cerr<<"is tie0"<<endl;
				return 0;
			}
			cerr<<7<<endl;
			while (true) {
				// cerr<<"present state chessboard"<<endl;
				// for (int i=0;i<height;i++){
				// 	for (int j=0;j<width;j++){
				// 		cerr<<tmpboard[i][j]<<" ";
				// 	}
				// 	cerr<<endl;
				// }
				if (player == 0) {					
					int randomIndex = positiveIndices[std::rand() % positiveIndices.size()];
					cerr<<8<<endl;
					tmpboard[tmptop[randomIndex] - 1][randomIndex] = 1;
					if (userWin(tmptop[randomIndex] - 1,randomIndex,  height, width, tmpboard)) {
						cerr<<"user wins"<<endl;
						// for (int i=0;i<height;i++){
						// 	for (int j=0;j<width;j++){
						// 		cerr<<tmpboard[i][j]<<" ";
						// 	}
						// 	cerr<<endl;
						// }
						delete[] tmptop;
						for (int i = 0; i < height; i++) {
							delete[] tmpboard[i];
						}
						delete[] tmpboard;
						return 1;
					}
					
					tmptop[randomIndex]--;


					if (ban_y==randomIndex && ban_x==tmptop[randomIndex]){
						tmptop[randomIndex]--;
					}
										
					if (isTie(width, tmptop)) {
						cerr<<"tie_player"<<endl;
						// for (int i=0;i<height;i++){
						// 	for (int j=0;j<width;j++){
						// 		cerr<<tmpboard[i][j]<<" ";
						// 	}
						// 	cerr<<endl;
						// }
						delete[] tmptop;
						for (int i = 0; i < height; i++) {
							delete[] tmpboard[i];
						}
						delete[] tmpboard;
						return 0;
					}
					cerr<<12<<endl;
					
					if (tmptop[randomIndex] <= 0) {
						positiveIndices.erase(
							std::remove(
								positiveIndices.begin(),
								positiveIndices.end(),
								ban_y
							),
							positiveIndices.end()
						);
					}
					cerr<<13<<endl;
					
					player = 1 - player;
				}
				
				if (player == 1) {
					cerr<<20<<endl;
					int randomIndex = positiveIndices[std::rand() % positiveIndices.size()];
					
					tmpboard[tmptop[randomIndex] - 1][randomIndex] = 2;
					
					if (machineWin(tmptop[randomIndex] - 1, randomIndex, height, width, tmpboard)) {
						cerr<<"machine wins"<<endl;
						// for (int i=0;i<height;i++){
						// 	for (int j=0;j<width;j++){
						// 		cerr<<tmpboard[i][j]<<" ";
						// 	}
						// 	cerr<<endl;
						// }
						delete[] tmptop;
						for (int i = 0; i < height; i++) {
							delete[] tmpboard[i];
						}
						delete[] tmpboard;
						return -1;
					}	

					tmptop[randomIndex]--;
					if (ban_y==randomIndex && ban_x==tmptop[randomIndex]){
						tmptop[randomIndex]--;
					}			

					if (isTie(width, tmptop)) {
						cerr<<"tie_machine"<<endl;
						// for (int i=0;i<height;i++){
						// 	for (int j=0;j<width;j++){
						// 		cerr<<tmpboard[i][j]<<" ";
						// 	}
						// 	cerr<<endl;
						// }
						delete[] tmptop;
						for (int i = 0; i < height; i++) {
							delete[] tmpboard[i];
						}
						delete[] tmpboard;
						return 0;
					}
					cerr<<25<<endl;

					if (tmptop[randomIndex] == 0) {
						positiveIndices.erase(
							std::remove(
								positiveIndices.begin(),
								positiveIndices.end(),
								ban_y
							),
							positiveIndices.end()
						);
					}	
					cerr<<26<<endl;
					player = 1 - player;
				}
			}
		}

		// back-propagate the result of the node up the tree to its ancestors(increment visitCount value and add game result to ancestor's reward)
		void backPropagate(Node * node, int result){
			// cerr<<"1: "<<node->board[1][1]<<endl;
			node->visitCount++;
			node->reward+=result;
			Node* tmpNode=node;
			while (tmpNode->parent){
				tmpNode=tmpNode->parent;
				tmpNode->visitCount++;
				tmpNode->reward+=result;
			}
			// delete tmpNode;//ALERT: release pointers!!! why will this line delete node as well as tmpnode?
			// cerr<<"2: "<<node->board[1][1]<<endl;
		}

		Node* bestChild(Node* node){
			double bestScore = -1000;
			int bestIndex = 0;
			for (int i = 0; i < node->numChildren; i++) {
				Node* child = node->children[i];
				int childScore;
				if (child->visitCount==0) childScore = 10000000;
				else {
					childScore = ((double)(child->reward))/(child->visitCount)+1.41*sqrt(log((double)node->visitCount)/log(child->visitCount));
				}
				if (childScore>bestScore){
					bestScore = childScore; bestIndex=i;
				}
			}
			cerr<<"best index is "<<bestIndex<<endl;
			return node->children[bestIndex];
			
		}
		// Node* defaultChild(){// DIDN'T IMPLEMENT THIS FUNCTION, DO I NEED THIS FUNCTION?
		// }//ALERT

		void expand(Node* node){
			node->children = new Node*[node->expandable.size()]; // Initialize children array with appropriate size
			int** new_board = new int*[height];
			int* new_top = new int[width];
			for (int i = 0; i < height; i++) {
				new_board[i] = new int[width];
			}

			for (int& element : node->expandable) {
				node->numChildren++;
				// Copy board state
				for (int i = 0; i < height; i++) {
					for (int j = 0; j < width; j++) {
						new_board[i][j] = node->board[i][j];//ALERT: room for optimization, don't create new_board every time?
					}
				}
				
				vector<int> new_expandable = node->expandable;
				
				// Copy top array
				for (int i = 0; i < width; i++) {
					new_top[i] = node->top[i];
				}
				
				if (ban_y == element && ban_x == new_top[element] - 1) {
					if (ban_y == 0) {
						new_expandable.erase(
							std::remove(
								new_expandable.begin(),
								new_expandable.end(),
								ban_y
							),
							new_expandable.end()
						);
						
					}
					new_top[ban_y]--;
				}
				
				new_board[new_top[element] - 1][element] = node->player + 1;
				new_top[element]--;
				
				if (new_top[element] == 0) {
					new_expandable.erase(
							std::remove(
								new_expandable.begin(),
								new_expandable.end(),
								ban_y
							),
							new_expandable.end()
						);
				}
				
				Node* newNode = new Node(new_board, height, width, ban_x, ban_y, 1 - node->player, new_top, new_expandable, node);
				newNode->move_x = new_top[element];
				newNode->move_y = element;
				node->children[node->numChildren - 1] = newNode;

			}

			delete[] new_top;
			for (int i = 0; i < height; i++) {
				delete[] new_board[i];
			}
			delete[] new_board;
		}

		Node* treePolicy(){
			auto startTime = std::chrono::high_resolution_clock::now();
			double durationInSeconds = 2.8; // ALERT: change time limit?
			auto endTime = startTime + std::chrono::duration<double>(durationInSeconds);
			cerr<<"entering the first loop in treepolicy"<<endl;
			while (std::chrono::high_resolution_clock::now() < endTime) {
				auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - startTime).count();
    			std::cerr << "Elapsed time: " << duration << " seconds" << std::endl;

				Node* current=root;
				cerr<<"root's childnum is"<<current->numChildren<<endl;
				while (current->children){
					cerr<<"searching for leaf node"<<endl;
					current=bestChild(current);
				}
				cerr<<"selected best child"<<endl;
				if (current->visitCount==0){
					cerr<<"this node's visit count is zero"<<endl;
					int result=randomPlay(current);
					backPropagate(current,result);
					cerr<<"finished random play for vc=0"<<endl;
				}
				else{
					cerr<<"this node's visit count isn't zero"<<endl;
					expand(current);
					std::srand(static_cast<unsigned int>(std::time(nullptr)));
					current=current->children[std::rand() % current->numChildren];
					int result=randomPlay(current);
					backPropagate(current,result);
					cerr<<"finished expansion and random play for vc!=0"<<endl;
				}
			}
			int bestScore = -10000;
			int bestIndex = 0;

			for (int i=0;i<root->numChildren;i++){
				int childScore;
				if (root->children[i]->visitCount==0) childScore = 0;
				else {
					childScore = ((double)(root->children[i]->reward))/(root->children[i]->visitCount)+2*sqrt(log((double)root->visitCount)/log(root->children[i]->visitCount));
				}
				if (childScore>bestScore) {
					bestIndex=i;
					bestScore=childScore;
				}
			}
			return root->children[bestIndex];
		}
		
};


/*
	策略函数接口,该函数被对抗平台调用,每次传入当前状态,要求输出你的落子点,该落子点必须是一个符合游戏规则的落子点,不然对抗平台会直接认为你的程序有误
	
	input:
		为了防止对对抗平台维护的数据造成更改，所有传入的参数均为const属性
		M, N : 棋盘大小 M - 行数 N - 列数 均从0开始计， 左上角为坐标原点，行用x标记，列用y标记
		top : 当前棋盘每一列列顶的实际位置. e.g. 第i列为空,则_top[i] == M, 第i列已满,则_top[i] == 0
		_board : 棋盘的一维数组表示, 为了方便使用，在该函数刚开始处，我们已经将其转化为了二维数组board
				你只需直接使用board即可，左上角为坐标原点，数组从[0][0]开始计(不是[1][1])
				board[x][y]表示第x行、第y列的点(从0开始计)
				board[x][y] == 0/1/2 分别对应(x,y)处 无落子/有用户的子/有程序的子,不可落子点处的值也为0
		lastX, lastY : 对方上一次落子的位置, 你可能不需要该参数，也可能需要的不仅仅是对方一步的
				落子位置，这时你可以在自己的程序中记录对方连续多步的落子位置，这完全取决于你自己的策略
		noX, noY : 棋盘上的不可落子点(注:其实这里给出的top已经替你处理了不可落子点，也就是说如果某一步
				所落的子的上面恰是不可落子点，那么UI工程中的代码就已经将该列的top值又进行了一次减一操作，
				所以在你的代码中也可以根本不使用noX和noY这两个参数，完全认为top数组就是当前每列的顶部即可,
				当然如果你想使用lastX,lastY参数，有可能就要同时考虑noX和noY了)
		以上参数实际上包含了当前状态(M N _top _board)以及历史信息(lastX lastY),你要做的就是在这些信息下给出尽可能明智的落子点
	output:
		你的落子点Point
*/
extern "C"  Point* getPoint(const int M, const int N, const int* top, const int* _board, 
	const int lastX, const int lastY, const int noX, const int noY){
	/*
		不要更改这段代码
	*/
	int x = -1, y = -1;//最终将你的落子点存到x,y中
	int** board = new int*[M];
	for(int i = 0; i < M; i++){
		board[i] = new int[N];
		for(int j = 0; j < N; j++){
			board[i][j] = _board[i * N + j];
		}
	}
	
	/*
		根据你自己的策略来返回落子点,也就是根据你的策略完成对x,y的赋值
		该部分对参数使用没有限制，为了方便实现，你可以定义自己新的类、.h文件、.cpp文件
	*/
	//Add your own code below
	UCT* uct = new UCT(M, N, noX, noY, top, board);
	Node* bestNode = uct->treePolicy();
	x = bestNode->move_x;
	y = bestNode->move_y;

	/*
		不要更改这段代码
	*/
	clearArray(M, N, board);
	return new Point(x, y);
}


/*
	getPoint函数返回的Point指针是在本dll模块中声明的，为避免产生堆错误，应在外部调用本dll中的
	函数来释放空间，而不应该在外部直接delete
*/
extern "C"  void clearPoint(Point* p){
	delete p;
	return;
}

/*
	清除top和board数组
*/
void clearArray(int M, int N, int** board){
	for(int i = 0; i < M; i++){
		delete[] board[i];
	}
	delete[] board;
}


/*
	添加你自己的辅助函数，你可以声明自己的类、函数，添加新的.h .cpp文件来辅助实现你的想法
*/
