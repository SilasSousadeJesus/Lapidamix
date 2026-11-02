#pragma once

namespace lapidamix
{
    // Limites das regras automáticas. Os valores de cada instrumento ficam no perfil (D30, D98).
    struct CorrectionSettings
    {
        struct HighPass
        {
            float marginOctaves;
            float minimumFrequency;
            float maximumFrequency;
            float conservativeFrequency;
            float minimumConfidence;
            float maximumOctaveJumpShare;
        };

        struct Mud
        {
            float regionLow;
            float regionHigh;
            float fundamentalMargin;
            float minimumRegionRatio;
            float maximumLevelDb;
            float maximumCutDb;
            float q;
        };

        struct Presence
        {
            float regionLow;
            float regionHigh;
            float frequency;
            float minimumLevelDb;
            float maximumBoostDb;
            float q;
        };

        struct Resonance
        {
            float minimumProminenceDb;
            float minimumPresence;
            float minimumNotesShare;
            float protectedLow;
            float protectedHigh;
            float maximumFrequency;
            float cutRatio;
            float maximumCutDb;
            float q;
        };

        float referenceLow;
        float referenceHigh;
        float minimumActionDb;

        HighPass highPass;
        Mud mud;
        Presence presence;
        Resonance resonance;
    };
}
