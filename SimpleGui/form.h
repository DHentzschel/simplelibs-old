#ifndef FORM_H
#define FORM_H

#include <astring.h>
#include <vector2.h>

class CloseCallback;

class FormPrivate;

class Form {
    friend class FormPrivate;
public:
    Form();

    ~Form();

    void setPosition(int x, int y);

    void centerToScreen();

    void setSize(int x, int y);

    void setIsMaximized(bool value);

    void setIsMinimized(bool value);

    void setWindowTitle(const AString& title);

    void hide();

    void show();

protected:
    // controls

    virtual void formMovedEvent(const Vector2& previousPosition, const Vector2& position) {}

    virtual void formResizedEvent(const Vector2& previousSize, const Vector2& size) {}

    virtual void formGainedFocusEvent() {}

    virtual void formLostFocusEvent() {}

    virtual void formClosedEvent() {}

    virtual void formShownEvent(bool isShown) {}

    virtual void formMaximizedEvent(const Vector2& size) {}

    virtual void formMinimizedEvent(const Vector2& size) {}

    virtual void formRestoredEvent(const Vector2& size) {}

    virtual void formPositionChangedEvent(const Vector2& position) {}

    virtual void formPositionChangingEvent(const Vector2& position) {}

    virtual void initializeComponents() = 0;

private:
    FormPrivate* private_;

};

#endif // FORM_H
