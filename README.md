kpmouse -- Mouse control from the numeric keypad
============================================================

`kpmouse` is an utility to control the mouse with the numeric keypad. Unlike the X11 mouse keys feature, movement is logarithmic and always movement starts from the center of the screen. Only once the movement window became small linear movement is enabled.

Activation
------------

`kpmouse` should run as a background process within the X session on which it will control the mouse. It will intercept `KeyPress` and `KeyRelease` events on the numeric keypad when **NumLock is off**. Other modifiers (Ctrl, Shift, Alt, Meta, Caps Lock) can bee activated in any combination and will not be affected, so that hitting '/' with Ctrl pressed will be interpreted as a Ctrl+Click. Numbers 1-9 control movement, keys /, * and - control the left, middle and right mouse buttons. Unlike in X11 mouse keys, 5 has no role and 0 servers to reverse the last movement step.

Movement
----------

Logarithmic movement occurs inside a "movement window". At the first step the movement window covers the entire screen. Steps within the window are executed using the 1-9 keys (except 5):

```
+-------------------------+
|                         |
|    7       8      9     |
|                         |
|    4              6     |
|                         |
|    1       2      3     | 
|                         | 
+-------------------------+
```

After executing the step, the movement window changes to have **half** its width and **half** its height and is centered on the destination point shown above, where the mouse pointer has already been moved to.

These logarithmic steps can be executed up to `KPM_LOG_STEPS` (by default 4, see `user_config.h`) times in sequence. After this limit, movement becomes linear while obeying the same key-direction relation. The step size during linear movement is determined by dividing the width and height of the last logarithmic movement window by `KPM_LINEAR_STEPS` (by default 5). Once linear movement is activated, the linear movement will continue until the movement is **terminated**.

Movements can be reversed using the 0 key on the numeric keypad. During logarithmic movement, reversion simply undoes the last movement, until the mouse pointer reaches the center of the screen. Once linear movement has started, reverting the last step will reverse **all linear steps**, reverting the movement to the last logarithmic movement (thus linear movement can start from scratch).

### Movement termination

Movement terminates when any of these occur:

- 2 seconds elapsed since the last step
- A click or button press/release has been ordered

Buttons
---------

Mouse buttons:
- Left button: `/`
- Middle button: `*`
- Right button: `-`

Clicking is done by "clicking" the corresponding key. If the key is held by more than 500ms, the mouse button **down** event is sent, but the release of the key will **not** cause the corresponding mouse button **up** event. To trigger the button up event, "click" the mouse button key again.

With a mouse button **down** , situations may occur:
- If a key different from the one that caused the button **down** event is pressed, the button **up** event will be of the mouse button that was originally pressed. That is, a long press of `/` followed by movement and then a click on `*` will count a drag using the left mouse button and will not cause a click of the middle button.
- There is no expiration time that would cause a **up** event of the pressed mouse button
- Undoing movement will not undo the button **down** event nor will send a **up** event
- After a **down** event, a long press (more than 500ms) on any mouse button key will have no effect

Compilation
--------------

There are two dependencies: X11 and libxdo (usually the package is named after `xdotool`, the executable).

```bash
make
```

The following variables can be set (e.g., make CFLAGS='-g -O0'):
- `CFLAGS`: Additional compiler flags
- `LFLAGS`: Additional linker flags
- `XDO_LFLAGS`: How to link with libxdo.so. Default is `-lxdo`
- `XDO_INCLUDES`: Override lib xdo includes (e.g., `-I/path/...`)
- `X11_LFLAGS`: How to link with X11 (default is determined by `pkg-config` and is usually `-lX11`)
- `X11_INCLUDES`: Override X11 include dirs (e.g., `-I/path/.../`)

Configuration
----------------

Edit `user_config.h` (and `user_config.c` for changing the keybindings) and recompile.


<!--  LocalWords:  kpmouse KeyPress KeyRelease NumLock Ctrl KPM config libxdo
 -->
<!--  LocalWords:  xdotool CFLAGS LFLAGS XDO lxdo xdo lX dirs
 -->
