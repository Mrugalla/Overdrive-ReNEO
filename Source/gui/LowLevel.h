#pragma once
#include "Knob.h"
#include "SplineEditor.h"

namespace gui
{
    struct LowLevel :
        public Comp
    {
        LowLevel(Utils& u) :
            Comp(u, "", CursorType::Default),
            muffle(u),
            drive(u),
            pan(u),
            scrap(u)
        {
            makeParameter(muffle, PID::Muffle, "Muffle");
            makeParameter(drive, PID::Drive, "Drive");
            makeParameter(pan, PID::Pan, "Pan");
            makeParameter(scrap, PID::Scrap, "Scrap");

            layout.init
            (
                { 1, 3, 1, 8, 1, 5, 1, 3, 1 },
                { 1, 3, 1 }
            );

            addAndMakeVisible(muffle);
            addAndMakeVisible(drive);
            addAndMakeVisible(pan);
            addAndMakeVisible(scrap);
        }

    protected:
        Knob muffle, drive, pan, scrap;
        
        void paint(Graphics& g) override
        {
            g.setColour(Colours::c(ColourID::Txt));
            auto font = getFontLobster();
            font.setHeight(utils.thicc * 10.f);
            g.setFont(font);
            g.drawFittedText("a tribute to mda overdrive",
                layout.bottom().toNearestInt(), Just::centred, 1);
        }

        void resized() override
        {
            layout.resized();

            layout.place(muffle, 1, 1, 1, 1, false);
            layout.place(drive, 3, 1, 1, 1, false);
            layout.place(scrap, 5, 1, 1, 1, false);
            layout.place(pan, 7, 1, 1, 1, false);
        }
    };
}