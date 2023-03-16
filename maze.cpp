#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <queue>
#include <random>
#include <thread>
using namespace std;
int entrancex, entrancey, wayoutx, wayouty;
bool isSolved = false;
const short int pass = 0, wall = 1, entrance = 2, wayout = 3;
map<int, string> cellChars = {
    {wall, "## "}, {pass, "  "}, {entrance, "🏁 "}, {wayout, "🏆 "}};
vector<pair<int, int>> path;
int **Generator(int a, int b) {
  int **maze = new int *[a];
  for (int i = 0; i < a; i++)
    maze[i] = new int[b];
  return maze;
}
bool isDeadEnd(int x, int y, int **maze, int height, int width) {
  return (x == 1 || maze[y][x - 2] == pass || y == 1 ||
          maze[y - 2][x] == pass || x == width - 2 || maze[y][x + 2] == pass ||
          y == height - 2 || maze[y + 2][x] == pass);
}
bool ended(int **maze, int height, int width) {
  for (int i = 1; i < height - 1; i += 2)
    for (int j = 1; j < width - 1; j += 2)
      if (maze[i][j] == wall)
        return false;
  return true;
}
void MazeGen(int **maze, int height, int width) {
  std::vector<int> dirs{0, 1, 2, 3};
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<> distr(0, 100);
  for (int i = 0; i < height; i++)
    for (int j = 0; j < width; j++)
      maze[i][j] = wall;
  int x = 3, y = 3;
  while (!ended(maze, height, width)) {
    maze[y][x] = pass;
    shuffle(dirs.begin(), dirs.end(), gen);
    bool moved = false;
    for (int d : dirs) {
      int dx = 0, dy = 0;
      if (d == 0 && y >= 3 && maze[y - 2][x] == wall) {
        dx = 0;
        dy = -2;
      } else if (d == 1 && y <= height - 4 && maze[y + 2][x] == wall) {
        dx = 0;
        dy = 2;
      } else if (d == 2 && x >= 3 && maze[y][x - 2] == wall) {
        dx = -2;
        dy = 0;
      } else if (d == 3 && x <= width - 4 && maze[y][x + 2] == wall) {
        dx = 2;
        dy = 0;
      }
      if (dx != 0 || dy != 0) {
        maze[y + dy / 2][x + dx / 2] = pass;
        x += dx;
        y += dy;
        moved = true;
        break;
      }
    }
    if (!moved && isDeadEnd(x, y, maze, height, width)) {
      do {
        x = 2 * (distr(gen) % ((width - 1) / 2)) + 1;
        y = 2 * (distr(gen) % ((height - 1) / 2)) + 1;
      } while (maze[y][x] != pass);
    }
  }
  int entranceSide = distr(gen) % 2;
  entrancex = distr(gen) % height;
  entrancey = (entranceSide == 0) ? 0 : width - 1;
  maze[entrancey][entrancex] = pass;
  wayoutx = distr(gen) % height;
  wayouty = (entranceSide == 0) ? width - 2 : 0;
  maze[wayouty][wayoutx] = pass;
}
void SolveMaze(int **maze, int rows, int cols, int startX, int startY, int endX,
               int endY) {
  queue<pair<int, int>> q;
  vector<vector<int>> visited(rows, vector<int>(cols));
  vector<vector<pair<int, int>>> parent(rows, vector<pair<int, int>>(cols));
  q.emplace(startX, startY);
  visited[startX][startY] = true;
  while (!q.empty()) {
    pair<int, int> curr = q.front();
    q.pop();
    int x = curr.first;
    int y = curr.second;
    if (x == endX && y == endY) {
      break;
    }
    if (x > 0 && maze[x - 1][y] == 0 && !visited[x - 1][y]) {
      q.emplace(x - 1, y);
      visited[x - 1][y] = true;
      parent[x - 1][y] = curr;
    }
    if (y > 0 && maze[x][y - 1] == 0 && !visited[x][y - 1]) {
      q.emplace(x, y - 1);
      visited[x][y - 1] = true;
      parent[x][y - 1] = curr;
    }
    if (x < rows - 1 && maze[x + 1][y] == 0 && !visited[x + 1][y]) {
      q.emplace(x + 1, y);
      visited[x + 1][y] = true;
      parent[x + 1][y] = curr;
    }
    if (y < cols - 1 && maze[x][y + 1] == 0 && !visited[x][y + 1]) {
      q.emplace(x, y + 1);
      visited[x][y + 1] = true;
      parent[x][y + 1] = curr;
    }
  }
  int x = endX, y = endY;
  while (x != startX || y != startY) {
    path.emplace_back(x, y);
    pair<int, int> p = parent[x][y];
    x = p.first;
    y = p.second;
  }
  path.emplace_back(startX, startY);
  reverse(path.begin(), path.end());
  maze[entrancey][entrancex] = entrance;
  maze[wayouty][wayoutx] = wayout;
  int prevX = -1, prevY = -1;
  for (auto &i : path) {
    x = i.first;
    y = i.second;
    system("clear");
    for (int x = 0; x < cols; x++)
      cout << "\e[1;33m" << setw(2) << x << " \e[0m";
    cout << endl;
    for (int r = 0; r < rows; ++r) {
      for (int c = 0; c < cols; ++c) {
        if (r == x && c == y) {
          string direction;
          if (prevX == x && prevY == y - 1)
            direction = "> "; // right arrow
          else if (prevX == x && prevY == y + 1)
            direction = " <"; // left arrow
          else if (prevX == x - 1 && prevY == y)
            direction = " ∨ "; // down arrow
          else if (prevX == x + 1 && prevY == y)
            direction = " ∧ "; // up arrow
          cout << "\e[1;31m" << setw(3) << direction << "\e[0m";
        } else
          cout << setw(3) << cellChars[maze[r][c]];
      }
      cout << "\e[1;34m" << setw(3) << r << "\e[0m\n";
    }
    cout << "\n\e[1;33mx\e[0m: " << y << "; \e[1;34my\e[0m:" << x << endl;
    prevX = x;
    prevY = y;
    this_thread::sleep_for(chrono::milliseconds(300));
  }
  isSolved = true;
}
void Print(int a, int b, int **mas) {
  mas[entrancey][entrancex] = entrance;
  mas[wayouty][wayoutx] = wayout;
  cout << "\e[1;33m";
  for (int x = 0; x < b; x++)
    cout << setw(2) << x << " ";
  cout << "\e[0m\n";
  for (int i = 0; i < a; i++) {
    for (int j = 0; j < b; j++)
      cout << setw(3) << cellChars[mas[i][j]];
    cout << "\e[1;34m" << setw(3) << i << "\e[0m\n";
  }
  mas[entrancey][entrancex] = mas[wayouty][wayoutx] = pass;
}
void Help() {
  ifstream myfile("help.txt");
  if (myfile) {
    cout << myfile.rdbuf();
    myfile.close();
  } else
    cout << "\e[1;31mФайл допомоги не існує!\e[0m\n";
}
int main() {
  int **Labyrinth = nullptr, M = 0, N = 0;
  char menu = '0';
  while (true) {
    system("clear");
    cout << "1) Ініціалізація лабіринту\n"
         << "2) Пройти лабіринт\n"
         << "3) Вивести лабіринт\n"
         << "4) Зберегти результат в файл\n"
         << "5) Очищення\n"
         << "6) Допомога\n"
         << "7) Вихід\n"
         << "Виберіть пункт: ";
    cin >> menu;
    if (menu == '1') {
      system("clear");
      if (!Labyrinth) {
        do {
          system("clear");
          cout << "\e[1;33mВводити можна лише непарне число не менше 3\e[0m\n"
               << "Введіть розмір: ";
          cin >> N;
        } while (N % 2 == 0 || isdigit(N) || N <= 3);
        M = N;
        Labyrinth = Generator(N, M);
        MazeGen(Labyrinth, N, M);
        system("clear");
        cout << "Згенерований лабіринт:\n";
        Print(N, M, Labyrinth);
        cout << "\e[1;32mВведення успішно виконано\e[0m\n";
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      } else {
        cout << "\e[1;33mДані вже ініціалізовано!\e[0m\n";
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      }
    } else if (menu == '2') {
      system("clear");
      if (isSolved) {
        cout << "\e[1;33mЛабіринт уже пройдено! Щоб пройти з початку "
                "перейдіть в пункт 'Очищення'\e[0m\n";
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      } else if (!Labyrinth) {
        cout << "\e[1;31mЛабіринт не ініціалізовано!\e[0m\n";
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      } else {
        SolveMaze(Labyrinth, N, M, entrancey, entrancex, wayouty, wayoutx);
        cout << "\e[1;32m\t Лабіринт успішно пройдено\e[0m\n";
        system("read -p \" Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      }
    } else if (menu == '3') {
      system("clear");
      if (!Labyrinth) {
        cout << "\e[1;31mЛабіринт не ініціалізовано!\e[0m\n";
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      } else {
        Print(M, N, Labyrinth);
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      }
    } else if (menu == '4') {
      system("clear");
      if (!Labyrinth || !isSolved) {
        cout << "\e[1;31mЛабіринт не пройдений або не ініціалізований\e[0m\n";
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      } else {
        int x, y;
        ofstream saveto;
        saveto.open("result.txt");
        saveto << "Шлях\n";
        for (auto &i : path) {
          x = i.first;
          y = i.second;
          saveto << "(" << x << ", " << y << ")" << endl;
        }
        cout << "\e[1;32mФайл успішно збережено\e[0m\n";
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
      }
      continue;
    } else if (menu == '5') {
      system("clear");
      if (!Labyrinth) {
        cout << "\e[1;31mЛабіринт не ініціалізовано!\e[0m\n";
        system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
        continue;
      }
      for (int i = 0; i < M; i++)
        delete[] Labyrinth[i];
      delete[] Labyrinth;
      M = N;
      isSolved = Labyrinth = nullptr;
      cout << "\e[1;32mЛабіринт і координати успішно очищені\e[0m\n";
      system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
      continue;
    } else if (menu == '6') {
      system("clear");
      Help();
      system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
      continue;
    } else if (menu == '7') {
      if (Labyrinth) {
        for (int i = 0; i < M; i++)
          delete[] Labyrinth[i];
        delete[] Labyrinth;
        exit(0);
      }
    } else {
      system("clear");
      cout << "\e[1;31mНевірне значення!\e[0m\n";
      system("read -p \"Натисніть будь-яку клавішу для продовження\" -n 1");
      continue;
    }
  }
}