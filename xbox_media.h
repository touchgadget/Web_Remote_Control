// Set member alignment to 1. No pad bytes between members.
#pragma pack(push, 1)

#define WALL_POWER_ON   (enum decode_type_t)200
#define WALL_POWER_OFF  (enum decode_type_t)201

typedef struct {
    const char *cell_name;
    decode_type_t IRType;
    uint64_t IRCode;
    uint16_t nbits;
    uint16_t repeat;
    uint8_t colSpan;
} Touch_IRcode_t;

typedef const struct {
    uint8_t rows;
    uint8_t cols;
    const Touch_IRcode_t *IR_cells;
} Touch_Page_t;

const Touch_IRcode_t XBox_Cells[15][3] = {
    // Row 0, top row
    {
        {"View", NEC, 0x11B7689, 32},
        {"XBOX", NEC, 0x11B26D9, 32},
        {"Menu", NEC, 0x11BF609, 32},
    },
    // Row 1
    {
        {"Reverse", NEC, 0x11BA857, 32},
        {"Play/pause", NEC, 0x11B0EF1, 32},
        {"Forward", NEC, 0x11B28D7, 32},
    },
    // Row 2
    {
        {"Up", NEC, 0x11B7887, 32, 0, 3},
        {},
        {}
    },
    // Row 3
    {
        {"Left", NEC, 0x11B04FB, 32},
        {"Select", NEC, 0x11B44BB, 32},
        {"Right", NEC, 0x11B847B, 32},
    },
    // Row 4
    {
        {"Down", NEC, 0x11BF807, 32, 0, 3},
        {},
        {}
    },
    // Row 5
    {
        {"Wall Power Off", WALL_POWER_OFF, 0, 0},
        {"Y (Yellow)", NEC, 0x11BE619, 32},
        {"Wall Power On", WALL_POWER_ON, 0, 0}
    },
    // Row 6
    {
        {"X (Blue)", NEC, 0x11B16E9, 32},
        {"A (Green)", NEC, 0x11B6699, 32},
        {"B (Red)", NEC, 0x11BA659, 32},
    },
    // Row 7
    {
        {"Back", NEC, 0x11BC43B, 32},
        {"OneGuide", NEC, 0x11B649B, 32},
        {"CC", NEC, 0x11BB24D, 32},
    },
    // Row 8
    {
        {"Sound Power", NEC, 0x10EF08F7, 32},       // Logitech speakers
        {"TV Power", NEC, 0x20DF10EF, 32},          // Vizio TV
        {"Wall Power On", WALL_POWER_ON, 0, 0}
    },
    // Row 9
    {
        {"Vol Up", NEC, 0x10EF58A7UL, 32},          // Logitech speakers
        {"Return/Last", NEC, 0x11BA45B, 32},
        {"Chan Up", NEC, 0x11B48B7, 32},
    },
    // Row 10
    {
        {"Vol Down", NEC, 0x10EF708FUL, 32},        // Logitech speakers
        {"Mute", NEC, 0x10EF6897UL, 32},            // Logitech speakers
        {"Chan Down", NEC, 0x11BC837, 32},
    },
    // Row 11
    {
        {"1", NEC, 0x11B807F, 32},
        {"2", NEC, 0x11B40BF, 32},
        {"3", NEC, 0x11BC03F, 32},
    },
    // Row 12
    {
        {"4", NEC, 0x11B20DF, 32},
        {"5", NEC, 0x11BA05F, 32},
        {"6", NEC, 0x11B609F, 32},
    },
    // Row 13
    {
        {"7", NEC, 0x11BE01F, 32},
        {"8", NEC, 0x11B10EF, 32},
        {"9", NEC, 0x11B906F, 32},
    },
    // Row 14
    {
        {"CLR", NEC, 0x11B50AF, 32},
        {"0", NEC, 0x11B00FF, 32},
        {".", NEC, 0x11BB847, 32},
    },
};

Touch_Page_t XBox_Media = {
    15, 3, (const Touch_IRcode_t *)XBox_Cells
};

#pragma pack(pop)
