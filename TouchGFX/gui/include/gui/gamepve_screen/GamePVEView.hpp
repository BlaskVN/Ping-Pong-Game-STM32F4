#ifndef GAMEPVEVIEW_HPP
#define GAMEPVEVIEW_HPP

#include <gui_generated/gamepve_screen/GamePVEViewBase.hpp>
#include <gui/gamepve_screen/GamePVEPresenter.hpp>

class GamePVEView : public GamePVEViewBase
{
public:
    GamePVEView();
    virtual ~GamePVEView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // GAMEPVEVIEW_HPP
