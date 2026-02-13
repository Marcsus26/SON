declare filename "SteganoDecoder.dsp";
declare name "SteganoDecoder";
import("stdfaust.lib");

f_carrier = 17000; 

// --- PETIT OUTIL DE PRÉSENCE ---
// On booste les fréquences de la clarté (entre 1.5kHz et 2.5kHz)
presence(gain) = fi.peak_eq(gain, 2000, 1000);

process(stego_in) = signal_final
with {
    // 1. ÉLARGISSEMENT DE LA FENÊTRE
    // On passe de +/- 1000Hz à +/- 2500Hz pour récupérer les harmoniques de la voix
    // Note : Ton encodeur doit aussi laisser passer 2500Hz pour que ça marche !
    filtrage_entree = stego_in : fi.bandpass(4, f_carrier-1000, f_carrier+1000);

    // 2. DÉMODULATION
    demodulated = filtrage_entree * os.osc(f_carrier);

    // 3. CHAÎNE DE TRAITEMENT DE LA VOIX
    signal_final = demodulated 
        : fi.lowpass(4, 3000)    // On laisse passer plus d'aigus pour la clarté
        : fi.highpass(2, 150)   // On coupe les résidus de basses "boueuses"
        : presence(6)           // On ajoute 6dB de "punch" sur les fréquences de la parole
        * 80                    // Gain de sortie (à ajuster)
        : aa.hardclip2;         // Protection finale
};