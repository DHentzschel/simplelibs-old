#ifndef KEY_H
#define KEY_H

enum class Key {
    /* The unknown key */
    KeyUnknown = -1,
    /* Mouse */
    Mouse1 = 0,
    Mouse2 = 1,
    Mouse3 = 2,
    Mouse4 = 3,
    Mouse5 = 4,
    Mouse6 = 5,
    Mouse7 = 6,
    Mouse8 = 7,
    MouseLeft = Mouse1,
    MouseRight = Mouse2,
    MouseMiddle = Mouse3,
    /* Printable keys */
    Space = 32,
    Apostrophe = 39,  /* ' */
    Comma = 44,  /* , */
    Minus = 45,  /* - */
    Period = 46,  /* . */
    Slash = 47,  /* / */
    Key0 = 48,
    Key1 = 49,
    Key2 = 50,
    Key3 = 51,
    Key4 = 52,
    Key5 = 53,
    Key6 = 54,
    Key7 = 55,
    Key8 = 56,
    Key9 = 57,
    KeySemicolon = 59,
    KeyEqual = 61,
    /* Letters */
    KeyA = 0x41,
    KeyB = 0x42,
    KeyC = 0x43,
    KeyD = 0x44,
    KeyE = 0x45,
    KeyF = 0x46,
    KeyG = 0x47,
    KeyH = 0x48,
    KeyI = 0x49,
    KeyJ = 0x4A,
    KeyK = 0x4B,
    KeyL = 0x4C,
    KeyM = 0x4D,
    KeyN = 0x4E,
    KeyO = 0x4F,
    KeyP = 0x50,
    KeyQ = 0x51,
    KeyR = 0x52,
    KeyS = 0x53,
    KeyT = 0x54,
    KeyU = 0x55,
    KeyV = 0x56,
    KeyW = 0x57,
    KeyX = 0x58,
    KeyY = 0x59,
    KeyZ = 0x5A,
    LeftBracket = 91,
    Backslash = 92,
    RightBracket = 93,
    GraveAccent = 96,
    KeyWorld1 = 161, /* non-US #1 */
    KeyWorld2 = 162, /* non-US #2 */
    /* Function keys */
    Escape = 256,
    KeyEnter = 257,
    KeyTab = 258,
    KeyBackspace = 259,
    KeyInsert = 260,
    KeyDelete = 261,
    ArrowRight = 262,
    ArrowLeft = 263,
    ArrowDown = 264,
    ArrowUp = 265,
    PageUp = 266,
    PageDown = 267,
    Home = 268,
    End = 269,
    CapsLock = 280,
    ScrollLock = 281,
    NumLock = 282,
    PrintScreen = 283,
    Pause = 284,
    F1 = 290,
    F2 = 291,
    F3 = 292,
    F4 = 293,
    F5 = 294,
    F6 = 295,
    F7 = 296,
    F8 = 297,
    F9 = 298,
    F10 = 299,
    F11 = 300,
    F12 = 301,
    F13 = 302,
    F14 = 303,
    F15 = 304,
    F16 = 305,
    F17 = 306,
    F18 = 307,
    F19 = 308,
    F20 = 309,
    F21 = 310,
    F22 = 311,
    F23 = 312,
    F24 = 313,
    F25 = 314,
    Numpad0 = 320,
    Numpad1 = 321,
    Numpad2 = 322,
    Numpad3 = 323,
    Numpad4 = 324,
    Numpad5 = 325,
    Numpad6 = 326,
    Numpad7 = 327,
    Numpad8 = 328,
    Numpad9 = 329,
    NumpadDecimal = 330,
    NumpadDivide = 331,
    NumpadMultiply = 332,
    NumpadSubtract = 333,
    NumpadAdd = 334,
    NumpadEnter = 335,
    NumpadEqual = 336,
    ShiftLeft = 340,
    ControlLeft = 341,
    AltLeft = 342,
    SuperLeft = 343,
    ShiftRight = 344,
    ControlRight = 345,
    AltRight = 346,
    SuperRight = 347,
    Menu = 348,

    Last = Menu
};

typedef Key Button;

#endif // KEY_H