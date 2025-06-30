#ifndef SELECTMODEVIEW_HPP
#define SELECTMODEVIEW_HPP

#include <gui_generated/selectmode_screen/SelectModeViewBase.hpp>
#include <gui/selectmode_screen/SelectModePresenter.hpp>

class SelectModeView : public SelectModeViewBase
{
public:
    SelectModeView();
    virtual ~SelectModeView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
protected:
};

#endif // SELECTMODEVIEW_HPP
