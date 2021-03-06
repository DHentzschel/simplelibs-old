#include "form.h"

#pragma comment(linker, "\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "msimg32.lib")

#include <Windows.h>
#include <Windowsx.h>
#include <commctrl.h>
#include <shlwapi.h>

#include <alist.h>
#include <functions.h>
#include <vector2.h>

#include "keyboard.h"
#include "mouse.h"
#include "pprinf.h"
#include "screen.h"

#include "outputbox.h"

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

class FormPrivate {
public:
    FormPrivate(Form* form);

    void initialize();

    void setPosition(int x, int y);

    void centerToScreen();

    void setSize(int x, int y);

    void setIsMaximized(bool value);

    void setIsMinimized(bool value);

    void setWindowTitle(const AString& title);

    void hide();

    void show();

    /* Events */
    void processEvents();

    void invokeFormMovedEvent(LPARAM lParam);

    void invokeFormMovingEvent(LPARAM lParam);

    void invokeFormResizingEvent(WPARAM wParam, LPARAM lParam);

    void invokeFormSizeEvent(WPARAM wParam, LPARAM lParam);

    void invokeFormGainedFocusEvent();

    void invokeFormLostFocusEvent();

    void invokeFormClosedEvent();

    void invokeFormClosingEvent();

    void invokeFormShownEvent(WPARAM wParam);

    void invokeFormPositionChangedEvent(LPARAM lParam);

    void invokeFormKeyEvent(WPARAM wParam, LPARAM lParam, bool isKeyDown);

    void invokeMouseEnteringEvent(bool inClientArea);

    void invokeMouseLeavingEvent(bool inClientArea);

    void invokeMouseMovingEvent(LPARAM lParam, bool inClientArea);

    void invokeMouseScrollingEvent(WPARAM wParam, LPARAM lParam);

    void invokeMouseButtonDownEvent(WPARAM wParam, LPARAM lParam, bool isDoubleClick, bool inClientArea);

    void invokeMouseButtonUpEvent(WPARAM wParam, LPARAM lParam, bool isDoubleClick, bool inClientArea);

    /* WINAPI attributes */
    MSG message;

    HWND hwnd;

    WNDCLASSW wc;

    /* Attributes */
	Vector2i previousPosition;

	Vector2i position;

	Vector2i previousSize;

	Vector2i size;

    bool isShown;

    bool isMaximized;

    bool isMinimized;

    Form* form;

    AList<std::shared_ptr<Control>> controls;

    static FormPrivate* get(HWND hwnd);

    static bool* keys_;

    static AList<Form*> formList_;
};

bool* FormPrivate::keys_ = nullptr;

AList<Form*> FormPrivate::formList_;

Form::Form() :
    private_(new FormPrivate(this))
{
    private_->formList_.append(this);
}

Form::~Form()
{
    private_->formList_.removeFirst(this);
    delete private_;
}

void Form::setPosition(int x, int y)
{
    private_->setPosition(x, y);
}

void Form::setSize(int x, int y)
{
    private_->setSize(x, y);
}

void Form::setIsMaximized(bool value)
{
    private_->setIsMaximized(value);
}

void Form::setIsMinimized(bool value)
{
    private_->setIsMinimized(value);
}

void Form::setWindowTitle(const AString& title)
{
    private_->setWindowTitle(title);
}

void Form::hide()
{
    private_->hide();
}

void Form::show()
{
    private_->show();
}

void Form::addControl(std::shared_ptr<Control>& control)
{
    private_->controls.append(control);
    control->setHwnd(private_->hwnd);
    control->create();
}

FormPrivate::FormPrivate(Form* form) :
    position(Vector2i(100, 100)),
    size(Vector2i(100, 100)),
    form(form)
{
    initialize();
}

void FormPrivate::initialize()
{
    if (keys_ == nullptr) {
        keys_ = const_cast<bool*>(Keyboard::getKeyArray());
    }

    const auto hinstance = PrivateProcessInformation::getHInstance();
    wc.style = CS_VREDRAW | CS_HREDRAW;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.lpszClassName = L"className"; /* Todo class name */
    wc.hInstance = static_cast<HINSTANCE>(hinstance);
    wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
    wc.lpszMenuName = NULL;
    wc.lpfnWndProc = WndProc;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);

    RegisterClassW(&wc);
    hwnd = CreateWindowW(wc.lpszClassName, L"Form", WS_OVERLAPPEDWINDOW | WS_VISIBLE, position.x, position.y, size.x, size.y, NULL, NULL, hinstance, NULL);
}

void FormPrivate::setPosition(int x, int y)
{
    position.x = x;
    position.y = y;
    MoveWindow(hwnd, position.x, position.y, size.x, size.y, true);
}

void FormPrivate::centerToScreen()
{
    const auto screenSize = Screen::getSize();
    setPosition(static_cast<int>((screenSize.x - size.x) * 0.5F), static_cast<int>((screenSize.y - size.y) * 0.5F));
}

void Form::centerToScreen()
{
    private_->centerToScreen();
}

void FormPrivate::setSize(int x, int y)
{
    size.x = x;
    size.y = y;
    MoveWindow(hwnd, position.x, position.y, size.x, size.y, true);
}

void FormPrivate::setIsMaximized(bool value)
{
    isMaximized = value;

    if (isShown) {
        ShowWindow(hwnd, (isMaximized ? SW_SHOWMAXIMIZED : SW_SHOWDEFAULT));
    }
}

void FormPrivate::setIsMinimized(bool value)
{
    isMinimized = value;

    if (isShown) {
        ShowWindow(hwnd, (isMinimized ? SW_SHOWMAXIMIZED : SW_SHOWDEFAULT));
    }
}

void FormPrivate::setWindowTitle(const AString & title)
{
    SetWindowTextA(hwnd, title.toCString());
}

void FormPrivate::hide()
{
    isShown = false;
    ShowWindow(hwnd, SW_HIDE);
    UpdateWindow(hwnd);
}

void FormPrivate::show()
{
    isShown = true;
    ShowWindow(hwnd, (isMaximized ? SW_SHOWMAXIMIZED : (isMinimized ? SW_SHOWMINIMIZED : SW_SHOWDEFAULT)));
    UpdateWindow(hwnd);

    for (auto control : controls) {
        control->show();
    }

    processEvents();
}

void FormPrivate::processEvents()
{
    while (GetMessage(&message, NULL, 0, 0)) {
        DispatchMessage(&message);
    }
}

void FormPrivate::invokeFormMovedEvent(LPARAM lParam)
{
    previousPosition = position;
    position = Vector2i(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    form->formMovedEvent(previousPosition, position);
}

void FormPrivate::invokeFormMovingEvent(LPARAM lParam)
{
    const auto& windowRect = *(RECT*) lParam;
    previousPosition = position;
    position = Vector2i(windowRect.left, windowRect.top);
    form->formMovingEvent(previousPosition, position);
}

void FormPrivate::invokeFormResizingEvent(WPARAM wParam, LPARAM lParam)
{
    const auto& windowRect = *(RECT*) lParam;
    previousSize = size;
    size = Vector2i(windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
    form->formResizingEvent(previousSize, size, static_cast<WindowEdge>(wParam));
}

void FormPrivate::invokeFormSizeEvent(WPARAM wParam, LPARAM lParam)
{
    const auto size = Vector2i(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

    switch (wParam) {
    case SIZE_MAXIMIZED:
        form->formMaximizedEvent(size);
        break;
    case SIZE_MINIMIZED:
        form->formMinimizedEvent(size);
        break;
    case SIZE_RESTORED:
        form->formRestoredEvent(size);
        break;
    }
}

void FormPrivate::invokeFormGainedFocusEvent()
{
    form->formGainedFocusEvent();
}

void FormPrivate::invokeFormLostFocusEvent()
{
    form->formLostFocusEvent();
}

void FormPrivate::invokeFormClosedEvent()
{
    form->formClosedEvent();
}

void FormPrivate::invokeFormClosingEvent()
{
    form->formClosingEvent();
}

void FormPrivate::invokeFormShownEvent(WPARAM wParam)
{
    form->formShownEvent(static_cast<bool>(wParam));
}

void FormPrivate::invokeFormPositionChangedEvent(LPARAM lParam)
{
    const auto& windowPos = *reinterpret_cast<WINDOWPOS*>(lParam);
    form->formPositionChangedEvent(Vector2i(windowPos.x, windowPos.y));
}

void FormPrivate::invokeFormKeyEvent(WPARAM wParam, LPARAM lParam, bool isKeyDown)
{
    const auto key = static_cast<Key>(wParam);

    const auto repeatCount = static_cast<ushort>(lParam & 0xFFF);
    lParam >>= 16;
    const auto scanCode = static_cast<byte>(lParam & 0xFF);
    lParam >>= 8;
    const bool isExtendedKey = lParam & 0b1;
    lParam >>= 5;
    const bool contextCode = lParam & 0b1;
    lParam >>= 1;
    const bool wasAlreadyDown = lParam & 0b1;
    lParam >>= 1;
    const bool transitionState = lParam & 0b1;

    keys_[static_cast<int>(key)] = isKeyDown;

    if (isKeyDown) {
        form->formKeyDownEvent(key, repeatCount, scanCode, isExtendedKey, wasAlreadyDown);
    }
    else {
        form->formKeyUpEvent(key, repeatCount, scanCode, isExtendedKey, wasAlreadyDown);
    }
}

void FormPrivate::invokeMouseEnteringEvent(bool inClientArea)
{
    form->formMouseEnteringEvent(Mouse::getPosition(), inClientArea);
}

void FormPrivate::invokeMouseLeavingEvent(bool inClientArea)
{
    form->formMouseLeavingEvent(inClientArea);
}

void FormPrivate::invokeMouseMovingEvent(LPARAM lParam, bool inClientArea)
{
    const auto position = Vector2i(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    auto& mousePosition = Mouse::getPosition();
    mousePosition.x = position.x;
    mousePosition.y = position.y;
    form->formMouseMovingEvent(position, inClientArea);
}

void FormPrivate::invokeMouseScrollingEvent(WPARAM wParam, LPARAM lParam)
{
    const auto wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
    const auto position = Vector2i(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    form->formMouseScrollingEvent(wheelDelta, position);
}

void FormPrivate::invokeMouseButtonDownEvent(WPARAM wParam, LPARAM lParam, bool isDoubleClick, bool inClientArea)
{
    const auto position = Vector2i(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    /* TODO: cast is unsafe for foreign keys */
    form->formMouseButtonDown(static_cast<Key>(wParam), position, isDoubleClick, inClientArea);
}

void FormPrivate::invokeMouseButtonUpEvent(WPARAM wParam, LPARAM lParam, bool isDoubleClick, bool inClientArea)
{
    const auto position = Vector2i(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
    /* TODO: cast is unsafe for foreign keys */
    form->formMouseButtonUp(static_cast<Key>(wParam), position, inClientArea);
}

FormPrivate* FormPrivate::get(HWND hwnd)
{
    for (auto* form : formList_) {
        if (form->private_->hwnd == hwnd) {
            return form->private_;
        }
    }
    return nullptr;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    auto* formPrivate = FormPrivate::get(hwnd);
    if (formPrivate != nullptr) {
        //HDC hdc;
        //PAINTSTRUCT paintstruct;
        switch (msg) {
        case WM_CREATE:
            // not fired
            break;
        case WM_DESTROY:
            formPrivate->invokeFormClosedEvent();
            PostQuitMessage(0);
            break;
        case WM_MOVE:
            formPrivate->invokeFormMovedEvent(lParam);
            break;
        case WM_MOVING:
            formPrivate->invokeFormMovingEvent(lParam);
            break;
        case WM_SIZING:
            formPrivate->invokeFormResizingEvent(wParam, lParam);
            break;
        case WM_SIZE:
            formPrivate->invokeFormSizeEvent(wParam, lParam);
            break;
        case WM_SETFOCUS:
            formPrivate->invokeFormGainedFocusEvent();
            break;
        case WM_KILLFOCUS:
            formPrivate->invokeFormLostFocusEvent();
            break;
        case WM_CLOSE:
            formPrivate->invokeFormClosingEvent();
            break;
        case WM_SHOWWINDOW:
            formPrivate->invokeFormShownEvent(wParam);
            break;
        case WM_WINDOWPOSCHANGED:
        {
            formPrivate->invokeFormPositionChangedEvent(lParam);
            RECT rc;
            GetClientRect(hwnd, &rc);
            HDWP hDWP = BeginDeferWindowPos(1);
            hDWP = DeferWindowPos(hDWP, GetDlgItem(hwnd, 100), NULL,
                rc.right - 110, 10, 100, 25, SWP_NOZORDER | SWP_NOREDRAW);
            break;
        }
        case WM_KEYDOWN:
        case WM_KEYUP:
            formPrivate->invokeFormKeyEvent(wParam, lParam, msg == WM_KEYDOWN);
            break;
            /* TODO: Following code is not reachable, the events won't get invoked. What to do? */
            /*
            case WM_MOUSEHOVER:
                formPrivate->invokeMouseEnteringEvent(true); // not invoked
                break;
            case WM_NCMOUSEHOVER:
                formPrivate->invokeMouseEnteringEvent(false); // not invoked
                break;
            case WM_MOUSELEAVE:
                formPrivate->invokeMouseLeavingEvent(true); // not invoked
                break; */
        case WM_NCMOUSELEAVE:
            formPrivate->invokeMouseLeavingEvent(false);
            formPrivate->invokeMouseEnteringEvent(true);
            break;
        case WM_MOUSEMOVE:
            formPrivate->invokeMouseMovingEvent(lParam, true);
            break;
        case WM_NCMOUSEMOVE:
            formPrivate->invokeMouseMovingEvent(lParam, false);
            break;
        case WM_MOUSEWHEEL:
            formPrivate->invokeMouseScrollingEvent(wParam, lParam);
            break;
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_XBUTTONDOWN:
            formPrivate->invokeMouseButtonDownEvent(wParam, lParam, false, true);
            break;
        case WM_NCLBUTTONDOWN:
        case WM_NCRBUTTONDOWN:
        case WM_NCMBUTTONDOWN:
        case WM_NCXBUTTONDOWN:
            formPrivate->invokeMouseButtonDownEvent(wParam, lParam, false, false);
            break;
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDBLCLK:
            formPrivate->invokeMouseButtonDownEvent(wParam, lParam, true, true);
            break;
        case WM_NCLBUTTONDBLCLK:
        case WM_NCRBUTTONDBLCLK:
        case WM_NCMBUTTONDBLCLK:
        case WM_NCXBUTTONDBLCLK:
            formPrivate->invokeMouseButtonDownEvent(wParam, lParam, true, false);
            break;
        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        case WM_MBUTTONUP:
        case WM_XBUTTONUP:
            formPrivate->invokeMouseButtonUpEvent(wParam, lParam, false, true);
            break;
        case WM_NCLBUTTONUP:
        case WM_NCRBUTTONUP:
        case WM_NCMBUTTONUP:
        case WM_NCXBUTTONUP:
            formPrivate->invokeMouseButtonUpEvent(wParam, lParam, false, false);
            break;
        case WM_PAINT:
            /*hdc = BeginPaint(hwnd, &paintstruct);
            TextOutW(hdc, 20, 20, L"Test", 10);
            EndPaint(hwnd, &paintstruct);*/
            // paint any controls
            break;
        }
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}
