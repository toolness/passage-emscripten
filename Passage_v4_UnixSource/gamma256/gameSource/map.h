// checks if position is blocked by wall
char isBlocked( int inX, int inY );


// checks if chest is present
// assumes position is not blocked
#define CHEST_NONE 0
#define CHEST_CLOSED 1
#define CHEST_OPEN 2
char isChest( int inX, int inY );


// 8-bit binary indicating which of six external chest gems are present
#define CHEST_RED_GEM      0x01
#define CHEST_GREEN_GEM    0x02
#define CHEST_ORANGE_GEM   0x04
#define CHEST_BLUE_GEM     0x08
#define CHEST_YELLOW_GEM   0x10
#define CHEST_MAGENTA_GEM   0x20

// assumes a chest is present
unsigned char getChestCode( int inX, int inY );

void getChestCenter( int inX, int inY, int *outCenterX, int *outCenterY );

void openChest( int inX, int inY );


// resets map to a fresh state
void resetMap();
