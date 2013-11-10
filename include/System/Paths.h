#ifndef TDW_PATHS_H
#define TDW_PATHS_H

// file used to store path defines for debug/release mode (they're different)

#ifdef _DEBUG
 #ifdef _WIN32
  #define PATH_DELIM "/"
  #define DATA_PATH "../data/"
  #define FONTS_PATH "../data/fonts/"
  #define DB_PATH "../data/db/"
  #define MAP_PATH "../data/maps/"
 #else
  #define PATH_DELIM "\\"
  #define DATA_PATH "..\\data\\"
  #define FONTS_PATH "..\\data\\fonts\\"
  #define DB_PATH "..\\data\\db\\"
  #define MAP_PATH "..\\data\\maps\\"
 #endif
#else
 #ifdef _WIN32
  #define PATH_DELIM "/"
  #define DATA_PATH "./data/"
  #define FONTS_PATH "./data/fonts/"
  #define DB_PATH "./data/db/"
  #define MAP_PATH "./data/maps/"
 #else
  #define PATH_DELIM "\\"
  #define DATA_PATH ".\\data\\"
  #define FONTS_PATH ".\\data\\fonts\\"
  #define DB_PATH ".\\data\\db\\"
  #define MAP_PATH ".\\data\\maps\\"
 #endif
#endif

#endif
