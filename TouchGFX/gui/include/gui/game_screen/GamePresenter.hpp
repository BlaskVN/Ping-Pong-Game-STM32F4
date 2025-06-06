#ifndef GAMEPRESENTER_HPP
#define GAMEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GameView;

class GamePresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GamePresenter(GameView& v);

    /**
     * The activate function is called automatically when this screen is "switched in"
     * (ie. made active). Initialization logic can be placed here.
     */
    virtual void activate();

    /**
     * The deactivate function is called automatically when this screen is "switched out"
     * (ie. made inactive). Teardown functionality can be placed here.
     */
    virtual void deactivate();

    virtual ~GamePresenter() {}

private:
    GamePresenter();

    GameView& view;
};

#endif // GAMEPRESENTER_HPP
