/*********************************************************************************/
/********** THIS FILE IS GENERATED BY TOUCHGFX DESIGNER, DO NOT MODIFY ***********/
/*********************************************************************************/
#include <gui_generated/containers/ballBase.hpp>
#include <touchgfx/Color.hpp>

ballBase::ballBase()
{
    setWidth(10);
    setHeight(10);
    circle1.setPosition(0, 0, 10, 10);
    circle1.setCenter(5, 5);
    circle1.setRadius(4);
    circle1.setLineWidth(0);
    circle1.setArc(0, 360);
    circle1Painter.setColor(touchgfx::Color::getColorFromRGB(255, 255, 255));
    circle1.setPainter(circle1Painter);
    add(circle1);

    circleborder.setPosition(0, 0, 10, 10);
    circleborder.setCenter(5, 5);
    circleborder.setRadius(4);
    circleborder.setLineWidth(1);
    circleborder.setArc(0, 360);
    circleborder.setCapPrecision(180);
    circleborderPainter.setColor(touchgfx::Color::getColorFromRGB(0, 0, 0));
    circleborder.setPainter(circleborderPainter);
    add(circleborder);
}

ballBase::~ballBase()
{

}

void ballBase::initialize()
{

}
