#ifndef GAMEPVEPRESENTER_HPP
#define GAMEPVEPRESENTER_HPP

#include <gui/model/ModelListener.hpp>
#include <mvp/Presenter.hpp>

using namespace touchgfx;

class GamePVEView;

class GamePVEPresenter : public touchgfx::Presenter, public ModelListener
{
public:
    GamePVEPresenter(GamePVEView& v);

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

    virtual ~GamePVEPresenter() {}

private:
    GamePVEPresenter();

    GamePVEView& view;
};

#endif // GAMEPVEPRESENTER_HPP
