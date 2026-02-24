declare filename "steganophone.dsp";
declare name "steganophone";
import("stdfaust.lib");

// --- PARAMÈTRES ---
f_carrier = 18000; 

process(music_in, secret_in) = stego_out
with {
    music_cleaned = music_in : fi.lowpass(9, 15000);

    secret_conditioned = secret_in : fi.lowpass(4, 1800) : fi.highpass(4, 300);
    
    secret_modulated = secret_conditioned * os.osc(f_carrier);

    gain_musique = 0.5;
    gain_secret = 0.4; 
    
    raw_mix = (music_cleaned * gain_musique) + (secret_modulated * gain_secret);

    stego_out = raw_mix : aa.hardclip2;
};