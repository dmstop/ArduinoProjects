#include <LedControl.h>

int DIN = 2;
int CS = 3;
int CLK = 4;
LedControl lc = LedControl(DIN, CLK, CS, 0);

enum Direction {NONE = 0, UP = 1, DOWN = 2, RIGHT = 3, LEFT = 4 };

struct Room
{
  int X = 0;
  int Y = 0;
  int LinksPointer = 0;
  Direction Links[4];
};

void setup() {
  Serial.begin(9600);

  randomSeed(analogRead(A0));

  lc.shutdown(0, false);
  lc.setIntensity(0, 6);
  lc.clearDisplay(0);
}

void loop() {
  lc.clearDisplay(0);

  int roomCount = 1;
  Room rooms[16];

  roomCount = 1;
  rooms[0].LinksPointer = 0;
  for (int i = 0; i < 4; i++)
  {
    rooms[0].Links[i] = NONE;
  }

  int roomStackPointer = 0;
  Room roomStack[16];

  while (roomCount < 16)
  {
    Room currentRoom = roomStack[roomStackPointer];

    Direction availableDirs[4];
    int availCount;

    checkDirections(availableDirs, availCount, currentRoom, roomCount, rooms);

    if (availCount == 0)
    {
      roomStack[roomStackPointer].LinksPointer = 0;
      for (int i = 0; i < 4; i++)
      {
        roomStack[roomStackPointer].Links[i] = NONE;
      }
      
      roomStackPointer--;
      continue;
    }

    Direction newDir = availableDirs[random(0, availCount)];
    switch (newDir)
    {
      case LEFT:
        {
          rooms[roomCount - 1].Links[rooms[roomCount - 1].LinksPointer] = LEFT;
          rooms[roomCount - 1].LinksPointer++;

          roomStackPointer++;
          roomStack[roomStackPointer].X = currentRoom.X - 1;
          roomStack[roomStackPointer].Y = currentRoom.Y;
          roomStack[roomStackPointer].Links[roomStack[roomStackPointer].LinksPointer] = RIGHT;
          roomStack[roomStackPointer].LinksPointer++;

          roomCount++;
          rooms[roomCount - 1].X = currentRoom.X - 1;
          rooms[roomCount - 1].Y = currentRoom.Y;
          rooms[roomCount - 1].Links[rooms[roomCount - 1].LinksPointer] = RIGHT;
          rooms[roomCount - 1].LinksPointer++;
          break;
        }
      case RIGHT:
        {
          rooms[roomCount - 1].Links[rooms[roomCount - 1].LinksPointer] = RIGHT;
          rooms[roomCount - 1].LinksPointer++;

          roomStackPointer++;
          roomStack[roomStackPointer].X = currentRoom.X + 1;
          roomStack[roomStackPointer].Y = currentRoom.Y;
          roomStack[roomStackPointer].Links[roomStack[roomStackPointer].LinksPointer] = LEFT;
          roomStack[roomStackPointer].LinksPointer++;

          roomCount++;
          rooms[roomCount - 1].X = currentRoom.X + 1;
          rooms[roomCount - 1].Y = currentRoom.Y;
          rooms[roomCount - 1].Links[rooms[roomCount - 1].LinksPointer] = LEFT;
          rooms[roomCount - 1].LinksPointer++;
          break;
        }
      case UP:
        {
          rooms[roomCount - 1].Links[rooms[roomCount - 1].LinksPointer] = UP;
          rooms[roomCount - 1].LinksPointer++;

          roomStackPointer++;
          roomStack[roomStackPointer].X = currentRoom.X;
          roomStack[roomStackPointer].Y = currentRoom.Y - 1;
          roomStack[roomStackPointer].Links[roomStack[roomStackPointer].LinksPointer] = DOWN;
          roomStack[roomStackPointer].LinksPointer++;

          roomCount++;
          rooms[roomCount - 1].X = currentRoom.X;
          rooms[roomCount - 1].Y = currentRoom.Y - 1;
          rooms[roomCount - 1].Links[rooms[roomCount - 1].LinksPointer] = DOWN;
          rooms[roomCount - 1].LinksPointer++;
          break;
        }
      case DOWN:
        {
          rooms[roomCount - 1].Links[rooms[roomCount - 1].LinksPointer] = DOWN;
          rooms[roomCount - 1].LinksPointer++;

          roomStackPointer++;
          roomStack[roomStackPointer].X = currentRoom.X;
          roomStack[roomStackPointer].Y = currentRoom.Y + 1;
          roomStack[roomStackPointer].Links[roomStack[roomStackPointer].LinksPointer] = UP;
          roomStack[roomStackPointer].LinksPointer++;

          roomCount++;
          rooms[roomCount - 1].X = currentRoom.X;
          rooms[roomCount - 1].Y = currentRoom.Y + 1;
          rooms[roomCount - 1].Links[rooms[roomCount - 1].LinksPointer] = UP;
          rooms[roomCount - 1].LinksPointer++;
          break;
        }
    }
  }

  for (int i = 0; i < 16; i++)
  {
    Room room = rooms[i];

    //lc.setLed(0, room.X * 2, room.Y * 2, true);

    bool right = false;
    bool down = false;
    for (int j = 0; j < room.LinksPointer; j++)
    {
      switch (room.Links[j])
      {
        case RIGHT:
          right = true;
          break;
        case DOWN:
          down = true;
          break;
      }
    }

    if (!right)
    {
      Serial.println("NoRight");
      lc.setLed(0, room.Y * 2, room.X * 2 + 1, true);
      lc.setLed(0, room.Y * 2 + 1, room.X * 2 + 1, true);
    }
    if (!down)
    {
      Serial.println("NoDown");
      lc.setLed(0, room.Y * 2 + 1, room.X * 2, true);
      lc.setLed(0, room.Y * 2 + 1 , room.X * 2 + 1, true);
      lc.setLed(0, room.Y * 2 + 1 , room.X * 2 - 1, true);
    }
  }

  delay(5000);
}

void checkDirections(Direction availableDirs[], int &count, Room currentRoom, int roomCount, Room rooms[16])
{
  int availDirsPointer = 0;

  bool isLeft = false;
  if (currentRoom.X != 0)
  {
    isLeft = true;
    for (int i = 0; i < roomCount; i++)
    {
      if (rooms[i].X == currentRoom.X - 1)
      {
        if (rooms[i].Y == currentRoom.Y)
        {
          isLeft = false;
          break;
        }
      }
    }
  }
  if (isLeft)
  {
    availableDirs[availDirsPointer] = LEFT;
    availDirsPointer++;
  }

  bool isRight = false;
  if (currentRoom.X != 3)
  {
    isRight = true;
    for (int i = 0; i < roomCount; i++)
    {
      if (rooms[i].X == currentRoom.X + 1)
      {
        if (rooms[i].Y == currentRoom.Y)
        {
          isRight = false;
          break;
        }
      }
    }
  }
  if (isRight)
  {
    availableDirs[availDirsPointer] = RIGHT;
    availDirsPointer++;
  }

  bool isUp = false;
  if (currentRoom.Y != 0)
  {
    isUp = true;
    for (int i = 0; i < roomCount; i++)
    {
      if (rooms[i].X == currentRoom.X)
      {
        if (rooms[i].Y == currentRoom.Y - 1)
        {
          isUp = false;
          break;
        }
      }
    }
  }
  if (isUp)
  {
    availableDirs[availDirsPointer] = UP;
    availDirsPointer++;
  }

  bool isDown = false;
  if (currentRoom.Y != 3)
  {
    isDown = true;
    for (int i = 0; i < roomCount; i++)
    {
      if (rooms[i].X == currentRoom.X)
      {
        if (rooms[i].Y == currentRoom.Y + 1)
        {
          isDown = false;
          break;
        }
      }
    }
  }
  if (isDown)
  {
    availableDirs[availDirsPointer] = DOWN;
    availDirsPointer++;
  }

  count = availDirsPointer;
}
