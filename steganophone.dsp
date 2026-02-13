declare filename "steganophone.dsp";
declare name "steganophone";
import("stdfaust.lib");

// --- PARAMÈTRES ---
f_carrier = 17000; 

process(music_in, secret_in) = stego_out
with {
    // 1. NETTOYAGE AGRESSIF
    // On vide spécifiquement la zone autour de 15kHz (largeur de 1000Hz)
    // C'est plus efficace que le lowpass pour supprimer les résidus de voix/batterie
    music_cleaned = music_in : fi.lowpass(9, 15000);

    // 2. PRÉPARATION DU SECRET
    // On resserre la bande passante pour la voix (300-1800Hz)
    // On augmente l'ordre à 4 pour ne pas "baver" hors du trou
    secret_conditioned = secret_in : fi.lowpass(4, 1800) : fi.highpass(4, 300);
    
    // 3. MODULATION AM
    secret_modulated = secret_conditioned * os.osc(f_carrier);

    // 4. MIXAGE
    // On baisse un peu plus la musique (0.7) et on booste le secret (0.15)
    // pour que le secret domine le bruit de fond résiduel
    gain_musique = 0.6;
    gain_secret = 0.3; 
    
    raw_mix = (music_cleaned * gain_musique) + (secret_modulated * gain_secret);

    // 5. PROTECTION
    // ma.clip est plus léger pour le CPU de la Teensy que aa.hardclip
    // ce qui évite les ralentissements (son grave)
    stego_out = raw_mix : aa.hardclip2;
};