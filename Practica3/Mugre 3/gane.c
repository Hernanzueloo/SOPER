/**
 * @brief It tests game module
 *
 * @file game_test.c
 * @author Diego Rodríguez Ortiz
 * @version 3.0
 * @date 09-03-2021
 * @copyright GNU Public License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "game_test.h"
#include "game.h"
#include "player.h"
#include "enemy.h"
#include "space.h"
#include "test.h"

#define MAX_TESTS 83
#define ID 29
#define ID_OBJECT 121
#define ID_LOC 122
#define ID_LOC2 123
#define ID_LINK 1
#define GAMEFILE "anthill.dat"
#define TESTFILE "fich_game_test.txt"

/**
 * @brief Main function for game unit tests.
 *
 * You may execute ALL or a SINGLE test
 *   1.- No parameter -> ALL test are executed
 *   2.- A number means a particular test (the one identified by that number)
 *       is executed
 *
 */
int main(int argc, char **argv)
{

  int test = 0;
  int all = 1;

  if (argc < 2)
  {
    printf("Running all test for module game:\n");
  }
  else
  {
    test = atoi(argv[1]);
    all = 0;
    printf("Running test %d:\t", test);
    if (test < 1 || test > MAX_TESTS)
    {
      printf("Error: unknown test %d\t", test);
      exit(EXIT_SUCCESS);
    }
  }
  if (all || test == 1)
    test1_game_create();
  if (all || test == 2)
    test2_game_create();
  if (all || test == 3)
    test1_game_create_from_file();
  if (all || test == 4)
    test2_game_create_from_file();
  if (all || test == 5)
    test1_game_get_last_command();
  if (all || test == 6)
    test2_game_get_last_command();
  if (all || test == 7)
    test1_game_update();
  if (all || test == 8)
    test1_game_destroy();
  if (all || test == 9)
    test2_game_destroy();
  if (all || test == 10)
    test1_game_is_over();
  if (all || test == 11)
    test2_game_is_over();
  if (all || test == 12)
    test1_game_get_player();
  if (all || test == 13)
    test2_game_get_player();
  if (all || test == 14)
    test1_game_set_player();
  if (all || test == 15)
    test2_game_set_player();
  if (all || test == 16)
    test3_game_set_player();
  if (all || test == 17)
    test1_game_get_player_objects();
  if (all || test == 18)
    test2_game_get_player_objects();
  if (all || test == 19)
    test1_game_get_player_location();
  if (all || test == 20)
    test2_game_get_player_location();
  if (all || test == 21)
    test3_game_get_player_location();
  if (all || test == 22)
    test1_game_set_player_location();
  if (all || test == 23)
    test2_game_set_player_location();
  if (all || test == 24)
    test3_game_set_player_location();
  if (all || test == 25)
    test1_game_get_player_health();
  if (all || test == 26)
    test2_game_get_player_health();
  if (all || test == 27)
    test1_game_set_player_health();
  if (all || test == 28)
    test2_game_set_player_health();
  if (all || test == 29)
    test3_game_set_player_health();
  if (all || test == 30)
    test1_game_get_enemy();
  if (all || test == 31)
    test2_game_get_enemy();
  if (all || test == 32)
    test1_game_set_enemy();
  if (all || test == 33)
    test2_game_set_enemy();
  if (all || test == 34)
    test3_game_set_enemy();
  if (all || test == 35)
    test4_game_set_enemy();
  if (all || test == 36)
    test1_game_get_enemy_location();
  if (all || test == 37)
    test2_game_get_enemy_location();
  if (all || test == 38)
    test3_game_get_enemy_location();
  if (all || test == 39)
    test1_game_set_enemy_location();
  if (all || test == 40)
    test2_game_set_enemy_location();
  if (all || test == 41)
    test3_game_set_enemy_location();
  if (all || test == 42)
    test1_game_get_enemy_health();
  if (all || test == 43)
    test2_game_get_enemy_health();
  if (all || test == 44)
    test1_game_set_enemy_health();
  if (all || test == 45)
    test2_game_set_enemy_health();
  if (all || test == 46)
    test3_game_set_enemy_health();
  if (all || test == 47)
    test1_game_get_object();
  if (all || test == 48)
    test2_game_get_object();
  if (all || test == 49)
    test3_game_get_object();
  if (all || test == 50)
    test1_game_add_object();
  if (all || test == 51)
    test1_game_add_object();
  if (all || test == 52)
    test2_game_add_object();
  if (all || test == 53)
    test3_game_add_object();
  if (all || test == 54)
    test1_game_get_object_id_at();
  if (all || test == 55)
    test2_game_get_object_id_at();
  if (all || test == 56)
    test1_game_get_num_objects();
  if (all || test == 57)
    test2_game_get_num_objects();
  if (all || test == 58)
    test3_game_get_num_objects();
  if (all || test == 59)
    test1_game_get_object_location();
  if (all || test == 60)
    test2_game_get_object_location();
  if (all || test == 61)
    test3_game_get_object_location();
  if (all || test == 62)
    test4_game_get_object_location();
  if (all || test == 63)
    test1_game_set_object_location();
  if (all || test == 64)
    test2_game_set_object_location();
  if (all || test == 65)
    test3_game_set_object_location();
  if (all || test == 66)
    test1_game_get_space();
  if (all || test == 67)
    test2_game_get_space();
  if (all || test == 68)
    test3_game_get_space();
  if (all || test == 69)
    test1_game_add_space();
  if (all || test == 70)
    test2_game_add_space();
  if (all || test == 71)
    test3_game_add_space();
  if (all || test == 72)
    test1_game_get_space_id_at();
  if (all || test == 73)
    test2_game_get_space_id_at();
  if (all || test == 74)
    test3_game_get_space_id_at();
  if (all || test == 75)
    test1_game_set_link();
  if (all || test == 76)
    test2_game_set_link();
  if (all || test == 77)
    test3_game_set_link();
  if (all || test == 78)
    test1_game_get_link();
  if (all || test == 79)
    test2_game_get_link();
  if (all || test == 80)
    test3_game_get_link();
  if (all || test == 81)
    test1_game_get_connection_status();
  if (all || test == 82)
    test2_game_get_connection_status();
  if (all || test == 83)
    test2_game_get_connection_status();

  PRINT_PASSED_PERCENTAGE;

  return 0;
}

/*Extra destroy functions are addded in case the items are not added properly to the game*/
void test1_game_create()
{
  Game *game;
  game = game_create();
  PRINT_TEST_RESULT(game != NULL);
  game_destroy(game);
}

void test2_game_create()
{
  
  Game *game;
  game = game_create();
  PRINT_TEST_RESULT(game_get_num_objects(game) == 0);
  game_destroy(game);
}

void test1_game_create_from_file()
{
  Game *game;
  game = game_create_from_file(NULL);

  PRINT_TEST_RESULT(game == NULL);

  game_destroy(game);
}

void test2_game_create_from_file()
{
  Game *game;
  game = game_create_from_file(GAMEFILE);
  PRINT_TEST_RESULT(game != NULL);
  game_destroy(game);
}

void test1_game_get_last_command()
{
  /*Game *game = NULL;
  T_Command command = NO_CMD;
  FILE *f=NULL;

  game = game_create();
  if(!game){
    return;
  }

  f=fopen(TESTFILE, "w");
  if(!f){
    game_destroy(game);
    return;
  }
  fprintf(f, "q");
  fclose(f);

  f=fopen(TESTFILE, "r");
  if(!f){
    game_destroy(game);
    return;
  }

  command = command_get_user_input(game_get_last_command(game), f);
  PRINT_TEST_RESULT(command == EXIT);

  game_destroy(game);
  fclose(f);*/
}

void test2_game_get_last_command()
{
  /*Game *game = NULL;
  T_Command command = NO_CMD;
  FILE *f=NULL;

  game = game_create();
  if(!game){
    return;
  }

  f=fopen(TESTFILE, "w");
  if(!f){
    game_destroy(game);
    return;
  }
  fprintf(f, "Move w");
  fclose(f);

  f=fopen(TESTFILE, "r");
  if(!f){
    game_destroy(game);
    return;
  }

  command = command_get_user_input(game_get_last_command(game), f);
  PRINT_TEST_RESULT(command != EXIT);

  game_destroy(game);
  fclose(f);*/
}

void test1_game_update()
{
  /*Game *game;
  int playerHealth, enemyHealth;
  FILE *f=NULL;
  T_Command command = NO_CMD;

  game=game_create_from_file(GAMEFILE);
  if(!game){
    return;
  }
  playerHealth=game_get_player_health(game);
  enemyHealth=game_get_enemy_health(game);

  f=fopen(TESTFILE, "w");
  if(!f){
    game_destroy(game);
    return;
  }
  fprintf(f, "a");
  fclose(f);

  f=fopen(TESTFILE, "r");
  if(!f){
    game_destroy(game);
    return;
  }

  (void)command = command_get_user_input(game_get_last_command(game), f);
  game_update(game);
  PRINT_TEST_RESULT(playerHealth-1==game_get_player_health(game) || enemyHealth-1==game_get_enemy_health(game));

  game_destroy(game);
  fclose(f);*/
}

void test1_game_destroy()
{
  Game *game = NULL;
  game_destroy(game);

  PRINT_TEST_RESULT(game == NULL);
}

void test2_game_destroy()
{
  Game *game = NULL;
  game_destroy(game);

  PRINT_TEST_RESULT(game == NULL);
}

void test1_game_is_over()
{
  Game *game = NULL;
  PRINT_TEST_RESULT(game_is_over(game) == TRUE);
}

void test2_game_is_over()
{
  Game *game;
  game = game_create_from_file(GAMEFILE);
  PRINT_TEST_RESULT(game_is_over(game) == FALSE);

  game_destroy(game);
}

void test1_game_get_player()
{
  Game *game = NULL;
  game = game_create();

  PRINT_TEST_RESULT(game_get_player(game) == NULL);

  game_destroy(game);
}

void test2_game_get_player()
{
  Game *game = NULL;
  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_player(game) != NULL);

  game_destroy(game);
}

void test1_game_set_player()
{
  Game *game = NULL;
  Player *player;

  player = player_create(ID);

  PRINT_TEST_RESULT(game_set_player(game, player) == ERROR);

  game_destroy(game);
  player_destroy(player);
}

void test2_game_set_player()
{
  Game *game;
  Player *player = NULL;

  game = game_create();
  PRINT_TEST_RESULT(game_set_player(game, player) == ERROR);

  game_destroy(game);
  player_destroy(player);
}

void test3_game_set_player()
{
  Game *game;
  Player *player;

  game = game_create();
  player = player_create(ID);
  (void)game_set_player(game, player);

  PRINT_TEST_RESULT(game_get_player(game) == player);
  
  game_destroy(game);
}

void test1_game_get_player_objects()
{
  Game *game = NULL;

  PRINT_TEST_RESULT(game_get_player_objects(game) == NULL);
}

void test2_game_get_player_objects()
{
  Game *game = NULL;
  game = game_create();

  PRINT_TEST_RESULT(game_get_player_objects(game) != NULL);

  game_destroy(game);
}

void test1_game_get_player_location()
{
  Game *game = NULL;

  PRINT_TEST_RESULT(game_get_player_location(game) == NO_ID);
}

void test2_game_get_player_location()
{
  Game *game;
  Player *player;

  game = game_create();
  player = player_create(ID);

  game_set_player(game, player);
  player_set_location(player, NO_ID);

  PRINT_TEST_RESULT(game_get_player_location(game) == NO_ID);

  game_destroy(game);
}

void test3_game_get_player_location()
{
  Game *game;
  Player *player;

  game = game_create();
  player = player_create(ID);

  game_set_player(game, player);
  player_set_location(player, ID_LOC);

  PRINT_TEST_RESULT(game_get_player_location(game) == ID_LOC);

  game_destroy(game);
}

void test1_game_set_player_location()
{
  Game *game = NULL;
  Player *player = NULL;

  game = game_create();

  PRINT_TEST_RESULT(player_set_location(player, ID_LOC) == ERROR);

  game_destroy(game);
}

void test2_game_set_player_location()
{
  Game *game;
  Player *player;

  game = game_create();
  player = player_create(6);

  game_set_player(game, player);
  player_set_location(player, NO_ID);

  PRINT_TEST_RESULT(game_get_player_location(game) == NO_ID);

  game_destroy(game);
}

void test3_game_set_player_location()
{
  Game *game;
  Player *player;

  game = game_create();
  player = player_create(ID);

  game_set_player(game, player);
  player_set_location(player, ID_LOC);

  PRINT_TEST_RESULT(game_get_player_location(game) == ID_LOC);

  game_destroy(game);
}

void test1_game_get_player_health()
{
  Game *game = NULL;

  PRINT_TEST_RESULT(game_get_player_health(game) == -1);
}

void test2_game_get_player_health()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_player_health(game) == 3);

  game_destroy(game);
}

void test1_game_set_player_health()
{
  Game *game = NULL;

  PRINT_TEST_RESULT(game_set_player_health(game, 3) == ERROR);
}

void test2_game_set_player_health()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_set_player_health(game, 3) == OK);

  game_destroy(game);
}

void test3_game_set_player_health()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);
  (void)game_set_player_health(game, 1);

  PRINT_TEST_RESULT(game_get_player_health(game) == 1);

  game_destroy(game);
}

void test1_game_get_enemy()
{
  Game *game=NULL;
  PRINT_TEST_RESULT(game_get_enemy(game) == NULL);
}

void test2_game_get_enemy()
{
  Game *game;
  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_enemy(game) != NULL);

  game_destroy(game);
}

void test1_game_set_enemy()
{
  Game *game=NULL;
  Enemy *enemy=NULL;

  PRINT_TEST_RESULT(game_set_enemy(game, enemy) == ERROR);
}

void test2_game_set_enemy()
{
  Game *game;
  Enemy *enemy=NULL;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_set_enemy(game, enemy) == ERROR);

  game_destroy(game);
}

void test3_game_set_enemy()
{
  Game *game;
  Enemy *enemy;

  game = game_create_from_file(GAMEFILE);
  enemy = enemy_create(ID);

  PRINT_TEST_RESULT(game_set_enemy(game, enemy) == OK);

  game_destroy(game);
}

void test4_game_set_enemy()
{
  Game *game;
  Enemy *enemy;

  game = game_create_from_file(GAMEFILE);
  enemy = enemy_create(ID);
  (void)game_set_enemy(game, enemy);

  PRINT_TEST_RESULT(enemy_get_id(enemy) == ID);

  game_destroy(game);
}

void test1_game_get_enemy_location()
{
  Game *game = NULL;

  PRINT_TEST_RESULT(game_get_enemy_location(game) == NO_ID);
}

void test2_game_get_enemy_location()
{
  Game *game;
  Enemy *enemy;

  game = game_create();
  enemy = enemy_create(ID);

  game_set_enemy(game, enemy);
  enemy_set_location(enemy, NO_ID);

  PRINT_TEST_RESULT(game_get_enemy_location(game) == NO_ID);

  game_destroy(game);
}

void test3_game_get_enemy_location()
{
  Game *game;
  Enemy *enemy;

  game = game_create();
  enemy = enemy_create(ID);

  game_set_enemy(game, enemy);
  enemy_set_location(enemy, ID_LOC);

  PRINT_TEST_RESULT(game_get_enemy_location(game) == ID_LOC);

  game_destroy(game);
}

void test1_game_set_enemy_location()
{
  Game *game = NULL;
  Enemy *enemy = NULL;

  game = game_create();

  PRINT_TEST_RESULT(enemy_set_location(enemy, ID_LOC) == ERROR);

  game_destroy(game);
}

void test2_game_set_enemy_location()
{
  Game *game;
  Enemy *enemy;

  game = game_create();
  enemy = enemy_create(ID);

  game_set_enemy(game, enemy);
  enemy_set_location(enemy, NO_ID);

  PRINT_TEST_RESULT(game_get_enemy_location(game) == NO_ID);

  game_destroy(game);
}

void test3_game_set_enemy_location()
{
  Game *game;
  Enemy *enemy;

  game = game_create();
  enemy = enemy_create(ID);

  game_set_enemy(game, enemy);
  enemy_set_location(enemy, ID_LOC);

  PRINT_TEST_RESULT(game_get_enemy_location(game) == ID_LOC);

  game_destroy(game);
}

void test1_game_get_enemy_health()
{
  Game *game = NULL;

  PRINT_TEST_RESULT(game_get_enemy_health(game) == -1);
}

void test2_game_get_enemy_health()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_enemy_health(game) == 3);

  game_destroy(game);
}

void test1_game_set_enemy_health()
{
  Game *game = NULL;

  PRINT_TEST_RESULT(game_set_enemy_health(game, 3) == ERROR);
}

void test2_game_set_enemy_health()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_set_enemy_health(game, 3) == OK);

  game_destroy(game);
}

void test3_game_set_enemy_health()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);
  (void)game_set_enemy_health(game, 1);

  PRINT_TEST_RESULT(game_get_enemy_health(game) == 1);

  game_destroy(game);
}

void test1_game_get_object()
{
  Game *game=NULL;

  PRINT_TEST_RESULT(game_get_object(game, ID) == NULL);
}

void test2_game_get_object()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_object(game, NO_ID) == NULL);

  game_destroy(game);
}

void test3_game_get_object()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_object(game, ID_OBJECT) != NULL);

  game_destroy(game);
}

void test1_game_add_object()
{
  Game *game = NULL;
  Object *obj = NULL;

  PRINT_TEST_RESULT(game_add_object(game, obj) == ERROR);
}

void test2_game_add_object()
{
  Game *game;
  Object *obj=NULL;

  game = game_create();

  PRINT_TEST_RESULT(game_add_object(game, obj) == ERROR);

  game_destroy(game);
}

void test3_game_add_object()
{
  Game *game;
  Object *obj;

  game = game_create();
  obj = object_create(ID_OBJECT);

  PRINT_TEST_RESULT(game_add_object(game, obj) == OK);

  game_destroy(game);
}

void test1_game_get_object_id_at()
{
  Game *game=NULL;

  PRINT_TEST_RESULT(game_get_object_id_at(game, 0) == NO_ID);
}

void test2_game_get_object_id_at()
{
  Game *game;
  Object *obj;

  game = game_create();
  obj = object_create(ID_OBJECT);
  (void)game_add_object(game, obj);

  PRINT_TEST_RESULT(game_get_object_id_at(game, 0) == ID_OBJECT);

  game_destroy(game);
}

void test1_game_get_num_objects()
{
  Game *game=NULL;

  PRINT_TEST_RESULT(game_get_num_objects(game) == -1);
}

void test2_game_get_num_objects()
{
  Game *game;

  game = game_create();

  PRINT_TEST_RESULT(game_get_num_objects(game) == 0);

  game_destroy(game);
}

void test3_game_get_num_objects()
{
  Game *game;
  Object *obj;

  game = game_create();
  obj = object_create(ID_OBJECT);
  (void)game_add_object(game, obj);

  PRINT_TEST_RESULT(game_get_num_objects(game) == 1);

  game_destroy(game);
}

void test1_game_get_object_location()
{
  Game *game=NULL;
  Object *obj;

  obj = object_create(ID_OBJECT);

  PRINT_TEST_RESULT(game_get_object_location(game, ID_OBJECT) == NO_ID);

  object_destroy(obj);
}

void test2_game_get_object_location()
{
  Game *game=NULL;

  game = game_create();

  PRINT_TEST_RESULT(game_get_object_location(game, ID_OBJECT) == NO_ID);

  game_destroy(game);
}

void test3_game_get_object_location()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_object_location(game, NO_ID) == NO_ID);

  game_destroy(game);
}

void test4_game_get_object_location()
{
  Game *game;
  Object *obj;

  game = game_create_from_file(GAMEFILE);
  obj = object_create(ID_OBJECT);
  (void)game_add_object(game, obj);
  (void)game_set_object_location(game, ID, ID_LOC);

  PRINT_TEST_RESULT(game_get_object_location(game, ID_OBJECT) == ID_LOC);

  game_destroy(game);
}

void test1_game_set_object_location()
{
  Game *game=NULL;

  PRINT_TEST_RESULT(game_set_object_location(game, NO_ID, NO_ID) == ERROR);
}

void test2_game_set_object_location()
{
  Game *game;
  Object *obj;

  game = game_create();
  obj = object_create(ID_OBJECT);
  (void)game_add_object(game, obj);

  PRINT_TEST_RESULT(game_set_object_location(game, ID, ID_LOC) == ERROR);

  game_destroy(game);
}

void test3_game_set_object_location()
{
  Game *game;
  Object *obj;

  game = game_create_from_file(GAMEFILE);
  obj = object_create(ID_OBJECT);
  (void)game_add_object(game, obj);
  (void)game_set_object_location(game, ID, ID_LOC);

  PRINT_TEST_RESULT(game_get_object_location(game, ID_OBJECT) == ID_LOC);

  game_destroy(game);
}

void test1_game_get_space()
{
  Game *game=NULL;

  PRINT_TEST_RESULT(game_get_space(game, ID_LOC) == NULL);
}

void test2_game_get_space()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_space(game, NO_ID) == NULL);

  game_destroy(game);
}

void test3_game_get_space()
{
  Game *game;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_space(game, ID_LOC) != NULL);

  game_destroy(game);
}

void test1_game_add_space()
{
  Game *game=NULL;
  Space *space=NULL;

  PRINT_TEST_RESULT(game_add_space(game, space) == ERROR);
}

void test2_game_add_space()
{
  Game *game;
  Space *space;

  game = game_create();
  space = space_create(ID_LOC);

  PRINT_TEST_RESULT(game_add_space(game, space) == OK);

  game_destroy(game);
}

void test3_game_add_space()
{
  Game *game;
  Space *space;

  game = game_create();
  space = space_create(ID_LOC);
  (void)game_add_space(game, space);

  PRINT_TEST_RESULT(game_get_space_id_at(game, 0) == ID_LOC);

  game_destroy(game);
}

void test1_game_get_space_id_at()
{
  Game *game=NULL;
  PRINT_TEST_RESULT(game_get_space_id_at(game, 0) == NO_ID);
}

void test2_game_get_space_id_at()
{
  Game *game;
  game = game_create();
  PRINT_TEST_RESULT(game_get_space_id_at(game, 0) == NO_ID);
  game_destroy(game);
}

void test3_game_get_space_id_at()
{
  Game *game;
  Space *space;

  game = game_create();
  space = space_create(ID_LOC);
  (void)game_add_space(game, space);

  PRINT_TEST_RESULT(game_get_space_id_at(game, 0) == ID_LOC);

  game_destroy(game);
}

void test1_game_set_link()
{
  Game *game;
  Link *link=NULL;
  game = game_create();
  PRINT_TEST_RESULT(game_set_link(game, link) == ERROR);
  game_destroy(game);
}

void test2_game_set_link()
{
  Game *game=NULL;
  Link *link;

  link = link_create(ID_LINK);

  PRINT_TEST_RESULT(game_set_link(game, link) == ERROR);

  link_destroy(link);
}

void test3_game_set_link()
{
  Game *game;
  Link *link;

  game = game_create();
  link = link_create(ID_LINK);

  PRINT_TEST_RESULT(game_set_link(game, link) == OK);
  
  game_destroy(game);
}

void test1_game_get_link()
{
  Game *game=NULL;

  PRINT_TEST_RESULT(game_get_link(game, ID_LINK) == NULL);
}

void test2_game_get_link()
{
  Game *game;
  Link *link;

  game = game_create();
  link = link_create(ID_LINK);

  PRINT_TEST_RESULT(game_get_link(game, ID_LINK) == NULL);
  
  link_destroy(link);
  game_destroy(game);
}

void test3_game_get_link()
{
  Game *game;
  Link *link;

  game = game_create();
  link = link_create(ID_LINK);
  (void)game_set_link(game, link);

  PRINT_TEST_RESULT(game_get_link(game, ID_LINK) == link);

  game_destroy(game);
}

void test1_game_get_connection_status()
{
  Game *game=NULL;
  Id origin = NO_ID, destination = NO_ID;

  PRINT_TEST_RESULT(game_get_connection_status(game, origin, destination) == CLOSE);
}

void test2_game_get_connection_status()
{
  Game *game;
  Id origin = NO_ID, destination = NO_ID;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_connection_status(game, origin, destination) == CLOSE);

  game_destroy(game);
}

void test3_game_get_connection_status()
{
  Game *game;
  Id origin = ID_LOC, destination = ID_LOC2;

  game = game_create_from_file(GAMEFILE);

  PRINT_TEST_RESULT(game_get_connection_status(game, origin, destination) == OPEN);

  game_destroy(game);
}   