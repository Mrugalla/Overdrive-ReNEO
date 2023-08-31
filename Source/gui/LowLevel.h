#pragma once
#include "Knob.h"
#include "SplineEditor.h"

namespace gui
{
    struct GateVisualizers :
        public Comp,
        public Timer
    {
        GateVisualizers(Utils& u) :
            Comp(u, "", CursorType::Default),
            gateVisualizers{ 0.f, 0.f },
            gateVisualizersBounds()
        {
            startTimerHz(24);
        }
    
        void timerCallback() override
        {
            const auto& processor = utils.audioProcessor;
            const auto& gate = processor.gate;
            const auto valL = gate.visualizerValues[0].load();
            const auto valR = gate.visualizerValues[1].load();

            if (gateVisualizers[0] == valL && gateVisualizers[1] == valR)
                return;

            gateVisualizers[0] = valL;
            gateVisualizers[1] = valR;

            repaint();
        }

        void paint(Graphics& g) override
        {
            for (auto ch = 0; ch < utils.audioProcessor.getChannelCountOfBus(true, 0); ++ch)
            {
                g.setColour(Colours::c(ColourID::Bg).interpolatedWith(Colours::c(ColourID::Bias), gateVisualizers[ch]));
                g.fillRoundedRectangle(gateVisualizersBounds[ch], utils.thicc);
            }
        }

        void resized() override
        {
            gateVisualizersBounds[0] = getLocalBounds().toFloat();
            gateVisualizersBounds[0].setWidth(gateVisualizersBounds[0].getWidth() * .5f);
            gateVisualizersBounds[1] = gateVisualizersBounds[0].withX(gateVisualizersBounds[0].getRight());

            for(auto& g : gateVisualizersBounds)
				g = g.reduced(utils.thicc * 2.f);
        }

    protected:
        std::array<float, 2> gateVisualizers;
        std::array<BoundsF, 2> gateVisualizersBounds;

        float fastTanh(float x) noexcept
        {
            if (x > 3.f) return 1.f;
            else if (x < -3.f) return -1.f;
            const auto exp2x = std::exp(2.f * x);
            return (exp2x - 1.f) / (exp2x + 1.f);
        }
    };

    struct LowLevel :
        public Comp
    {
        LowLevel(Utils& u) :
            Comp(u, "", CursorType::Default),
            gateThreshold(u),
            muffle(u),
            drive(u),
            pan(u),
            scrap(u),
            visuals(u)
        {
            makeParameter(gateThreshold, PID::GateThreshold, "Gate");
            makeParameter(muffle, PID::Muffle, "Muffle");
            makeParameter(drive, PID::Drive, "Drive");
            makeParameter(pan, PID::Pan, "Pan");
            makeParameter(scrap, PID::Scrap, "Scrap");

            layout.init
            (
                { 1, 3, 1, 3, 1, 8, 1, 5, 1, 3, 1 },
                { 1, 8, 1 }
            );

            addAndMakeVisible(gateThreshold);
            addAndMakeVisible(muffle);
            addAndMakeVisible(drive);
            addAndMakeVisible(pan);
            addAndMakeVisible(scrap);
            addAndMakeVisible(visuals);
        }

       
    protected:
        Knob gateThreshold, muffle, drive, pan, scrap;
        GateVisualizers visuals;

        void paint(Graphics& g) override
        {
            g.setColour(Colours::c(ColourID::Txt));
            auto font = getFontLobster();
            font.setHeight(utils.thicc * 10.f);
            g.setFont(font);
            g.drawFittedText
            (
                "a tribute to mda overdrive",
                layout.bottom().toNearestInt(), Just::centred, 1
            );
        }

        void resized() override
        {
            layout.resized();

            layout.place(gateThreshold, 1, 1, 1, 1, false);
            layout.place(visuals, 1, 2, 1, 1);
            layout.place(muffle, 3, 1, 1, 1, false);
            layout.place(drive, 5, 1, 1, 1, false);
            layout.place(scrap, 7, 1, 1, 1, false);
            layout.place(pan, 9, 1, 1, 1, false);
        }
    };
}