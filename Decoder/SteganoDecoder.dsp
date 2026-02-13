declare filename "SteganoDecoder.dsp";
declare name "SteganoDecoder";
import("stdfaust.lib");

f_carrier = 17000;


process(stego_in) = signal_final

with {

// ÉTAPE CRUCIALE : Le bandpass d'ordre 4 rejette violemment

// tout ce qui n'est pas à 15kHz (donc il tue le kick et la basse)

filtrage_entree = stego_in : fi.bandpass(4, f_carrier-1000, f_carrier+1000);


// Démodulation

demodulated = filtrage_entree * os.osc(f_carrier);


// Nettoyage final : On ne garde que la voix reconstruite

// Gain de 30 pour compenser l'atténuation du processus

signal_final = demodulated : fi.lowpass(4, 1500) * 60 : aa.hardclip2;

}; 