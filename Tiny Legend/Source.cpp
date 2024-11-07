#include <iostream>
#include <Windows.h>
#include <conio.h>
#include <vector>
#include <string>
#include <fstream>

using namespace std;

struct instance {
	int x = 16, y = 16,
		color = { BACKGROUND_GREEN };
};
struct texture {
	instance attribute;
	vector< vector<int> > map;
}screen;
struct nonstaticObject {
	instance attribute;
	char input = 'z';
	int speed = 1;
	int type = 0;
};
struct staticObject {
	struct instance {
		int x = 0, x1 = 0,
			y = 0, y1 = 0;
		int color = { BACKGROUND_GREEN };
	}attribute;
};
struct game {
	bool gameOver = false;
	int hpMax = 8;
	int hp = 3;
	int mapIndex = 0;
	int score = 0;
	vector<nonstaticObject> entity{};
	vector<nonstaticObject> bullet{};
}tinyLegend;

void saveScore();

//visual Functions
void draw(texture screen, vector<texture> tile, vector<nonstaticObject> objects, vector<nonstaticObject> projectile);
void keysPressed(nonstaticObject player);
void hpBar();

//create new item
void mapLoad(texture& screen, vector<staticObject> objects);
void changeMap();
void map();
void map1();
void fireBullets(nonstaticObject player, vector<nonstaticObject>& bulletList);
void shapeRoom(vector<staticObject>& wall, int& lineNum, int posStart[2], int posGap[2], int gapSize[2], int posEnd[2]);
void newObj(vector<staticObject>& wall, int lineNum, int x, int y, int x1, int y1);

//entity Functions
void Input(nonstaticObject& player);
void enemyThwomp(nonstaticObject& enemy, nonstaticObject& player);
void moveBullet(nonstaticObject& arrow);
void checkAtDoor();
void bulletCollision(vector<nonstaticObject>& enemyList, vector<nonstaticObject>& bulletList);

//entity suplemetary functions
void inputSort(char input, int& yDifference, int& xDifference);
bool moveTo(nonstaticObject& obj, int yPos, int xPos);
void calcMoveDir(nonstaticObject& enemy, nonstaticObject player, int movementModifier);

//suplementary functions
void colour(int colorIndex) { SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorIndex); } //credit to ED
void colorCell(int cell);
void pause(int endline) {
	for (int a = 0; a < endline; a++) cout << endl;
	system("pause");
}
int cRand(int min, int max) {
	int range = (max - min) + 1;
	return rand() % range + min;
}
void print();


int main() {
	srand(time(NULL));
	nonstaticObject player;
	player.attribute.color = BACKGROUND_RED;
	player.attribute.x = screen.attribute.x / 2;
	player.attribute.y = screen.attribute.y - 2;
	tinyLegend.entity.push_back(player);

	//textures
	// 
	texture floorTexture;
	floorTexture.map = {
		{ 0, 0, 0, 0},
		{ 0, 4, 4, 0},
		{12,12, 0, 0},
		{ 0, 0, 4, 4}
	};
	floorTexture.attribute.x = 4;
	floorTexture.attribute.y = 4;
	texture wallTexture;
	wallTexture.map = {
		{ 3, 3, 3, 3},
		{ 3, 9, 9, 1},
		{ 3, 9, 9, 1},
		{ 1, 1, 1, 1}
	};
	wallTexture.attribute.x = 4;
	wallTexture.attribute.y = 4;
	//Enemy
	texture slimeTexture;
	slimeTexture.map = {
		{ 0,15,10, 0},
		{15,15,10, 2},
		{ 5,10, 5, 2},
		{ 2, 2, 2, 2}
	};
	slimeTexture.attribute.x = 4;
	slimeTexture.attribute.y = 4;
	//player
	texture playerTexture;
	playerTexture.map = {
		{ 0,15,15,15},
		{ 0,15,15,15},
		{ 0,15,15,15},
		{ 0,15, 0,15}
	};
	playerTexture.attribute.x = 4;
	playerTexture.attribute.y = 4;
	texture bulletTexture;
	bulletTexture.map = {
		{ 0,14,14, 0},
		{14,14, 6, 6},
		{14, 6, 6, 12},
		{ 0, 6,12, 0}
	};
	bulletTexture.attribute.x = 4;
	bulletTexture.attribute.y = 4;


	vector<texture> tileset{
		floorTexture,
		wallTexture,
		playerTexture,
		slimeTexture,
		bulletTexture
	};

	map();

	while (!tinyLegend.gameOver) {
		int currentMap = tinyLegend.mapIndex;
		draw(screen, tileset, tinyLegend.entity, tinyLegend.bullet);
		cout << "\n\n"; hpBar();
		cout << "\n\n"; keysPressed(tinyLegend.entity.at(0));

		Sleep(500);
		Input(tinyLegend.entity.at(0));

		bulletCollision(tinyLegend.entity, tinyLegend.bullet);

		if (tinyLegend.mapIndex == 1 && tinyLegend.entity.size() > 1) {
			for (int a = 1; a < tinyLegend.entity.size(); a++) {
				enemyThwomp(tinyLegend.entity.at(a), tinyLegend.entity.at(0));
			}
		}

		//check if at door
		checkAtDoor();

		//change map
		if (tinyLegend.mapIndex != currentMap) changeMap();

		system("cls");

		if (tinyLegend.hp <= 0) tinyLegend.gameOver = true;
		else if (tinyLegend.entity.at(0).input == 'p') break;
	}

	saveScore();

	colour(7);
	pause(2);
}

void draw(texture screen, vector<texture> tile, vector<nonstaticObject> objects, vector<nonstaticObject> projectile) {
	for (int y = 0; y < screen.attribute.y; y++) for (int ytile = 0; ytile < 4; ytile++) {
		for (int x = 0; x < screen.attribute.x; x++) {
			int currentTile = screen.map[y][x];
			

			for (int a = 1; a < objects.size(); a++) {
				if (objects.at(a).attribute.x == x && objects.at(a).attribute.y == y) currentTile = objects[a].type + 2;
			}
			for (int a = 0; a < projectile.size(); a++) {
				if (projectile.at(a).attribute.x == x && projectile.at(a).attribute.y == y) currentTile = 4;
			}

			if (objects.at(0).attribute.x == x && objects.at(0).attribute.y == y) currentTile = 2;

			for (int xtile = 0; xtile < 4; xtile++) {
				if (currentTile >= 2 && tile[currentTile].map[ytile][xtile] == 0) colorCell(tile[0].map[ytile][xtile]);
				else colorCell(tile[currentTile].map[ytile][xtile]);
				cout << "  ";
			}
		}
		colour(7);
		cout << endl;
	}
}
void hpBar() {
	cout << " ";
	for (int a = 0; a < tinyLegend.hpMax; a++) {
		(a < tinyLegend.hp) ? colour(BACKGROUND_RED) : colour(BACKGROUND_INTENSITY); 
		cout << "   "; 
		colour(7); 
		cout << " ";
	}

	cout << "HP Bar";
}
void keysPressed(nonstaticObject player) {

	cout << "     ";
	(player.input == 'w') ? colour(BACKGROUND_RED | BACKGROUND_GREEN) : colour(0 | BACKGROUND_INTENSITY);  cout << " W "; colour(7);
	cout << endl;
	cout << " ";
	(player.input == 'a') ? colour(BACKGROUND_RED | BACKGROUND_GREEN) : colour(0 | BACKGROUND_INTENSITY); cout << " A "; colour(7); cout << " ";
	(player.input == 's') ? colour(BACKGROUND_RED | BACKGROUND_GREEN) : colour(0 | BACKGROUND_INTENSITY); cout << " S "; colour(7); cout << " ";
	(player.input == 'd') ? colour(BACKGROUND_RED | BACKGROUND_GREEN) : colour(0 | BACKGROUND_INTENSITY); cout << " D "; colour(7); cout << " ";

	cout << "X to Shoot,";
	cout << "P to exit\n";
}


void mapLoad(texture& screen, vector<staticObject> objects) {
	for (int y = 0; y < screen.attribute.y; y++) {
		screen.map.push_back({});
		for (int x = 0; x < screen.attribute.x; x++) {
			screen.map[y].push_back(0);
			for (int a = 0; a < objects.size(); a++) {
				if (x >= objects.at(a).attribute.x && x <= objects.at(a).attribute.x1 &&
					y >= objects.at(a).attribute.y && y <= objects.at(a).attribute.y1) {
					screen.map[y][x] = 1;
					a = objects.size();
				}
			}
		}
	}
}
void changeMap() {
	tinyLegend.entity.erase(tinyLegend.entity.begin() + 1, tinyLegend.entity.end());
	screen.map.clear();
	if (tinyLegend.mapIndex == 0) map();
	else if (tinyLegend.mapIndex == 1) map1();
}
void map() {
	screen.attribute.x = 16;
	screen.attribute.y = 16;
	vector<staticObject> walls;

	int xScreenSizeThird = (screen.attribute.x / 3);
	int yScreenSizeThird = (screen.attribute.y / 3),
		startPos[2] = { 0,0 },
		endPos[2] = { screen.attribute.x - 1, screen.attribute.y - 1 },
		gapPos[2] = { xScreenSizeThird, yScreenSizeThird },
		gapSize[2] = { 4, 4 };

	int displace = 0;
	shapeRoom(walls, displace, startPos, gapPos, gapSize, endPos);

	int xHalfScreen = screen.attribute.x / 2, yHalfScreen = screen.attribute.y / 2;
	newObj(walls, displace, xHalfScreen - 2, yHalfScreen - 1, xHalfScreen + 1, yHalfScreen);
	newObj(walls, displace + 1, xHalfScreen - 1, yHalfScreen - 2, xHalfScreen, yHalfScreen + 1);
	newObj(walls, displace + 2, 0, screen.attribute.y - 1, screen.attribute.x, screen.attribute.y - 1);
	newObj(walls, displace + 3, 0, 0, screen.attribute.x, 0);
	newObj(walls, displace + 4, 0, 0, 0, screen.attribute.y);

	mapLoad(screen, walls);
}
void map1() {
	screen.attribute.x = 16;
	screen.attribute.y = 32;
	vector<staticObject> wall;
	int currentItem = 0;
	int objectCoords[5][4] = {
		{0, 0, screen.attribute.x - 1, 0},
		{screen.attribute.x - 1, 0, screen.attribute.x - 1, screen.attribute.y - 1},
		{0, screen.attribute.y - 1, screen.attribute.x - 1, screen.attribute.y - 1},
		{0, 0, 0, screen.attribute.y - 7},
		{0, screen.attribute.y - 2, 0, screen.attribute.y}
	};

	for (int a = 0; a < 5; a++) {
		newObj(wall, currentItem, objectCoords[a][0], objectCoords[a][1], objectCoords[a][2], objectCoords[a][3]);
		currentItem++;
	}
	int start[2] = { 0,0 },
		gapPos[2] = { 3, 5 },
		gapSize[2] = { 3, 3 },
		end[2] = { screen.attribute.x / 2, screen.attribute.y / 2 };

	shapeRoom(wall, currentItem, start, gapPos, gapSize, end);

	nonstaticObject enemy;
	enemy.attribute.color = BACKGROUND_GREEN | BACKGROUND_INTENSITY;
	enemy.attribute.x = screen.attribute.x / 2 + 1;
	enemy.attribute.y = screen.attribute.y / 2 - 1;
	enemy.speed = 0;
	enemy.type = 1;
	tinyLegend.entity.push_back(enemy);
	mapLoad(screen, wall);
}
void fireBullets(nonstaticObject player, vector<nonstaticObject>& bulletList) {
	int count = bulletList.size();
	char direction[4] = { 'w','a','s','d' };
	for (int a = 0; a < 4; a++) {
		bulletList.emplace_back(nonstaticObject{});
		moveTo(bulletList.at(count), player.attribute.y, player.attribute.x);
		bulletList.at(count).type = 2;
		bulletList.at(count).speed = 5;
		bulletList.at(count).attribute.color = BACKGROUND_GREEN | BACKGROUND_RED;
		bulletList.at(count).input = direction[a];
		count++;
	}
}
void shapeRoom(vector<staticObject>& wall, int& lineNum, int posStart[2], int posGap[2], int gapSize[2], int posEnd[2]) {
	int one[2] = { posStart[0], posStart[1] },
		two[2] = { posGap[0], posGap[1] },
		three[2] = { posGap[0] + gapSize[0] + 1, posGap[1] + gapSize[1] + 1 },
		four[2] = { posEnd[0], posEnd[1] };

	//change the formula to generate the entrance always at the middle;

	int roomShape[8][4] = {
	{one[0], one[1], two[0], one[1]},
	{three[0], one[1], four[0], one[1]},

	{one[0], four[1], two[0], four[1]},
	{three[0], four[1], four[0], four[1]},

	{one[0], one[1], one[0], two[1]},
	{one[0], three[1], one[0], four[1]},

	{four[0], one[1], four[0], two[1]},
	{four[0], three[1], four[0], four[1]},
	};

	for (int a = 0; a < 8; a++) {
		newObj(wall, a + lineNum, roomShape[a][0], roomShape[a][1], roomShape[a][2], roomShape[a][3]);
	}
	lineNum += 8;
}
void shapeRoom(vector<staticObject>& wall, int& lineNum,  int root[2], int length, int height, int gapSize[2]) {
	//do later
}
void newObj(vector<staticObject>& wall, int lineNum, int x, int y, int x1, int y1) {
	wall.emplace_back(staticObject{});
	wall.at(lineNum).attribute.x = x;
	wall.at(lineNum).attribute.y = y;
	wall.at(lineNum).attribute.x1 = x1;
	wall.at(lineNum).attribute.y1 = y1;
}


void Input(nonstaticObject& player) {
	if (_kbhit()) player.input = _getch();
	else {
		player.input = 'z';
		return;
	}
	player.input = tolower(player.input);
	int xPos = 0, yPos = 0;

	inputSort(player.input, yPos, xPos);

	for (int positionsMoved = 0; positionsMoved < player.speed; positionsMoved++) {
		if (moveTo(player, player.attribute.y + yPos, player.attribute.x + xPos)) {
			positionsMoved = player.speed;
		}
	}


}
void enemyThwomp(nonstaticObject& enemy, nonstaticObject& player) {
	int increment[2] = { 0,0 };

	inputSort(enemy.input, increment[1], increment[0]);

	if (enemy.speed > 0) { //move
		int saveNewPos[2] = { enemy.attribute.x += increment[0], enemy.attribute.y += increment[1] };

		if (moveTo(enemy, saveNewPos[1], saveNewPos[0])) {
			enemy.speed = 0;
			enemy.attribute.x -= increment[0];
			enemy.attribute.y -= increment[1];
		}

		//check if colided with player
		if (enemy.attribute.x == player.attribute.x && enemy.attribute.y == player.attribute.y) {
			tinyLegend.hp--;
			player.attribute.x = 1;
			player.attribute.y = 1;
			tinyLegend.score -= 10;
		}
		enemy.speed--;
	}
	else calcMoveDir(enemy, player, 5); // calculate movement direction and reset movementAmmount
}
void moveBullet(nonstaticObject& arrow) {
	if (arrow.speed != 0) {
		int xPos = 0, yPos = 0;
		inputSort(arrow.input, yPos, xPos);
		moveTo(arrow, arrow.attribute.y += yPos, arrow.attribute.x += xPos);

		arrow.speed--;
	}
	else tinyLegend.bullet.erase(tinyLegend.bullet.begin());
}
void checkAtDoor() {
	if (tinyLegend.mapIndex == 0) {
		if (tinyLegend.entity.at(0).attribute.x == screen.attribute.x - 1) {
			tinyLegend.mapIndex = 1;
			tinyLegend.entity.at(0).attribute.x = 1;
			tinyLegend.entity.at(0).attribute.y = 28;
		}
		else if (tinyLegend.entity.at(0).attribute.x == 0) {
			tinyLegend.mapIndex = 2;
			tinyLegend.entity.at(0).attribute.x = 15;
		}
		else if (tinyLegend.entity.at(0).attribute.y == 0) {
			tinyLegend.mapIndex = 3;
			tinyLegend.entity.at(0).attribute.y = 15;
		}
	}
	else if (tinyLegend.mapIndex == 1) {
		if (tinyLegend.entity.at(0).attribute.x == 0) {
			tinyLegend.mapIndex = 0;
			tinyLegend.entity.at(0).attribute.x = 14;
			tinyLegend.entity.at(0).attribute.y = 8;
		}
	}
}
void bulletCollision(vector<nonstaticObject>& enemyList, vector<nonstaticObject>& bulletList) {
	for (int a = 0; a < bulletList.size(); a++) {
		if (tinyLegend.mapIndex == 1) for (int b = 1; b < enemyList.size(); b++) {
			char bulletDir = bulletList[a].input;
			int ybulletHitBox[2] = { bulletList[a].attribute.y, bulletList[a].attribute.y },
				xbulletHitBox[2] = { bulletList[a].attribute.x, bulletList[a].attribute.x };

			if (bulletDir == 'w' || bulletDir == 's') {
				ybulletHitBox[0]--;
				ybulletHitBox[1]++;
			}
			else if (bulletDir == 'a' || bulletDir == 'd') {
				xbulletHitBox[0]--;
				xbulletHitBox[1]++;
			}

			bool enemy_hit = (
				xbulletHitBox[0] <= enemyList[b].attribute.x && 
				xbulletHitBox[1] >= enemyList[b].attribute.x &&
				ybulletHitBox[0] <= enemyList[b].attribute.y && 
				ybulletHitBox[1] >= enemyList[b].attribute.y
				);

			if (enemy_hit) {
				tinyLegend.score += 10;
				bulletList[a].speed = 1;
				enemyList.erase(enemyList.begin() + b);
			}
		}
		moveBullet(bulletList[a]);
	}
}


void inputSort(char input, int& yDifference, int& xDifference) {
	switch (input) {
	 case 'w': yDifference--; break;
	 case 'a': xDifference--; break;
	 case 's': yDifference++; break;
	 case 'd': xDifference++; break;
	 case 'x': fireBullets(tinyLegend.entity.at(0), tinyLegend.bullet);
	}
}
bool moveTo(nonstaticObject& obj, int yPos, int xPos) {
	int colided = false;
	if (xPos < 0 || xPos > screen.attribute.x - 1 ||
		yPos < 0 || yPos > screen.attribute.y - 1)
	{
		colided = true;
	}
	else colided = (screen.map[yPos][xPos] == 1);

	if (colided) return true;
	else {
		obj.attribute.x = xPos;
		obj.attribute.y = yPos;
	}
	return false;
}
void calcMoveDir(nonstaticObject& enemy, nonstaticObject player, int movementModifier) {
	int direction[2] = { player.attribute.x - enemy.attribute.x, player.attribute.y - enemy.attribute.y };
	char inputs[2][2] = { { 'a','d' }, { 'w','s' } };
	bool axisToMove = rand() % 2;

	if (direction[axisToMove] < 0) enemy.input = inputs[axisToMove][0];
	else if (direction[axisToMove] > 0) enemy.input = inputs[axisToMove][1];
	else {
		axisToMove = !axisToMove;
		if (direction[axisToMove] < 0) enemy.input = inputs[axisToMove][0];
		else if (direction[axisToMove] > 0) enemy.input = inputs[axisToMove][1];
	}

	enemy.speed = abs(direction[axisToMove]);
	if (enemy.speed > movementModifier) enemy.speed = movementModifier;
}

void saveScore() {
	string fileName = "SaveFile.txt";
	ifstream fileOpen(fileName);
	if (!fileOpen.is_open()) {
		ofstream fileCreate(fileName);
		fileCreate.close();
	}
	fileOpen.close();

	string lineRead = "", playerName = "";
	int number = 0;
	ifstream fileRead(fileName);
	if (fileRead.is_open()) {
		while (getline(fileRead, lineRead)) {
			lineRead.erase(0, 5);
			number = stoi(lineRead);
			if (tinyLegend.score > number) {
				while (true) {
					cout << "New High Score!!!\nInput Initials (3 chars) - ";
					getline(cin, playerName);
					if (playerName.length() == 3) {
						cout << "Previous ";  break;
					}
				}

				ofstream fileWrite(fileName);
				if (fileWrite.is_open()) {
					fileWrite << playerName << " - " << tinyLegend.score;
					fileWrite.close();
				}
			}
		}
		fileRead.close();
	}

	cout << "High Score: " << lineRead << endl;
	cout << "Your score: " << tinyLegend.score;
}

void colorCell(int cell) {
	switch (cell) {
	case 0:  colour(0); break;
	case 1:  colour(BACKGROUND_BLUE); break;
	case 2:  colour(BACKGROUND_GREEN); break;
	case 3:  colour(BACKGROUND_BLUE | BACKGROUND_GREEN); break;
	case 4:  colour(BACKGROUND_RED); break;
	case 5:  colour(BACKGROUND_RED | BACKGROUND_BLUE); break;
	case 6:  colour(BACKGROUND_RED | BACKGROUND_GREEN); break;
	case 7:  colour(BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED); break;
	case 8:  colour(BACKGROUND_INTENSITY); break;
	case 9:  colour(BACKGROUND_INTENSITY | BACKGROUND_BLUE); break;
	case 10: colour(BACKGROUND_INTENSITY | BACKGROUND_GREEN); break;
	case 11: colour(BACKGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN); break;
	case 12: colour(BACKGROUND_INTENSITY | BACKGROUND_RED); break;
	case 13: colour(BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_BLUE); break;
	case 14: colour(BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN); break;
	case 15: colour(BACKGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_GREEN | BACKGROUND_RED); break;
	}
}
